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
		req["request"] = {
			{"name", client_info.name},
			{"age", client_info.age},
			{"national_id", client_info.national_id},
			{"address", client_info.address},
		};
		return req;
	}

	void
	Request_Update_Info::deserialize(const Json &json)
	{
		IRequest::deserialize(json);
		client_info.name = json["request"]["name"];
		client_info.age  = json["request"]["age"];

		std::string n_id = json["request"]["national_id"];
		strcpy(client_info.national_id, n_id.c_str());

		client_info.address = json["request"]["address"];
	}

	std::unique_ptr<ICommand>
	Request_Update_Info::command(id_t client_id)
	{
		return std::make_unique<Command_Update_Info>(client_id, client_info);
	}

	Response_Update_Info::Response_Update_Info()
		: IResponse{KIND_UPDATE_INFO, {}}
	{
	}

	Response_Update_Info::Response_Update_Info(Error _error)
		: IResponse{KIND_UPDATE_INFO, std::move(_error)}
	{
	}

	Command_Update_Info::Command_Update_Info(id_t client_id, Client_Info _new_info)
		: ICommand(client_id), new_info{std::move(_new_info)}
	{
	}

	std::unique_ptr<IResponse>
	Command_Update_Info::execute(Server *server)
	{
		auto [old, err] = server->update_info(client_id, new_info);
		if (err == false)
			old_info = old;

		return std::make_unique<Response_Update_Info>(err);
	}

	void
	Command_Update_Info::undo(Server *server)
	{
		auto [_, err] = server->update_info(client_id, old_info);
		if (err)
			Log::error("{}", err);
	}

	void
	Command_Update_Info::redo(Server *server)
	{
		auto [_, err] = server->update_info(client_id, new_info);
		if (err)
			Log::error("{}", err);
	}

	std::string
	Command_Update_Info::describe()
	{
		std::string description;

		if (old_info.name != new_info.name)
			description += fmt::format("name: {} -> {}", old_info.name, new_info.name);

		if (old_info.age != new_info.age)
			description += fmt::format(" age: {} -> {}", old_info.age, new_info.age);

		if (strcmp(old_info.national_id, new_info.national_id) != 0)
			description += fmt::format(" national_id: {} -> {}", old_info.national_id, new_info.national_id);

		if (old_info.address != new_info.address)
			description += fmt::format(" address: {} -> {}", old_info.address, new_info.address);

		return description;
	}
}
