#include "sws/Client_Info.h"
#include "sws/Server.h"
#include <utility>
#include <ctype.h>

namespace sws
{
	Error
	Client_Info::is_valid() const
	{
		if (name.empty())
			return Error{"Name is empty"};

		if (age < 18 || age > 120)
			return Error{"Age is out of bounds"};

		if (strlen(national_id) != NATIONAL_ID_LEN)
			return Error{"Invalid national id"};

		for (int i = 0; i < NATIONAL_ID_LEN; i++)
		{
			if (isdigit(national_id[i]) == false)
				return Error{"Invalid national id"};
		}

		if (address.empty())
			return Error{"Address is empty"};

		return Error{};
	}

	Json
	Client_Info::serialize() const
	{
		return Json{
			{"name", name},
			{"age", age},
			{"national_id", national_id},
			{"address", address},
		};
	}

	void
	Client_Info::deserialize(const Json &json)
	{
		name = json["name"];
		age  = json["age"];

		std::string n_id = json["national_id"];
		strcpy(national_id, n_id.c_str());

		address = json["address"];
	}

	Request_Update_Info::Request_Update_Info()
		: IRequest{KIND_UPDATE_INFO}
	{
	}

	Request_Update_Info::Request_Update_Info(Client_Info _client_info)
		: IRequest{KIND_UPDATE_INFO}, client_info{std::move(_client_info)}
	{
	}

	Json
	Request_Update_Info::serialize() const
	{
		auto req = IRequest::serialize();
		req["request"] = client_info.serialize();
		return req;
	}

	bool
	Request_Update_Info::deserialize(const Json &json)
	{
		IRequest::deserialize(json);
		client_info.deserialize(json["request"]);
		return true;
	}

	std::unique_ptr<ICommand>
	Request_Update_Info::command()
	{
		return std::make_unique<Command_Update_Info>(client_info);
	}

	Response_Update_Info::Response_Update_Info()
		: IResponse{KIND_UPDATE_INFO, {}}
	{
	}

	Response_Update_Info::Response_Update_Info(Error _error)
		: IResponse{KIND_UPDATE_INFO, std::move(_error)}
	{
	}

	Command_Update_Info::Command_Update_Info(Client_Info _new_info)
		: new_info{std::move(_new_info)}
	{
	}

	std::unique_ptr<IResponse>
	Command_Update_Info::execute(Server *server, cid_t client_id)
	{
		auto [old, err] = server->update_info(client_id, new_info);
		if (err == false)
			old_info = old;

		return std::make_unique<Response_Update_Info>(err);
	}

	Error
	Command_Update_Info::undo(Server *server, cid_t client_id)
	{
		auto [_, err] = server->update_info(client_id, old_info);
		return err;
	}

	Error
	Command_Update_Info::redo(Server *server, cid_t client_id)
	{
		auto [_, err] = server->update_info(client_id, new_info);
		return err;
	}

	Json
	Command_Update_Info::serialize()
	{
		return Json{
			{"update_info", {
				{"old_info", old_info.serialize()},
				{"new_info", new_info.serialize()},
			}}
		};
	}

	bool
	Command_Update_Info::deserialize(const Json &json)
	{
		if (json.size() > 1)
			return false;

		if (json.contains("update_info") == false)
			return false;

		old_info.deserialize(json["update_info"]["old_info"]);
		new_info.deserialize(json["update_info"]["new_info"]);
		return true;
	}
}
