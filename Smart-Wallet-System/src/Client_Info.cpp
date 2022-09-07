#include "sws/Client_Info.h"
#include <utility>

namespace sws
{
	Error
	Client_Info::is_valid() const
	{
		if (name.empty())
			return Error{"Name is empty"};

		if (age < 18 || age > 120)
			return Error{"Age is out of bounds"};

		if (national_id / NATIONAL_ID_MAX)
			return Error{"Invalid national id"};

		if (address.empty())
			return Error{"Address is empty"};

		return Error{};
	}

	Request_Update_Info::Request_Update_Info()
		: IRequest{KIND_UPDATE_INFO, {}}
	{
	}

	Request_Update_Info::Request_Update_Info(id_t _client_id, Client_Info _client_info)
		: IRequest{KIND_UPDATE_INFO, _client_id}, client_info{std::move(_client_info)}
	{
	}

	Json
	Request_Update_Info::serialize()
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
		client_info.name        = json["request"]["name"];
		client_info.age         = json["request"]["age"];
		client_info.national_id = json["request"]["national_id"];
		client_info.address     = json["request"]["address"];
	}

	std::unique_ptr<ICommand>
	Request_Update_Info::command()
	{
		return std::make_unique<Command_Update_Info>(client_id, client_info);
	}

	Response_Update_Info::Response_Update_Info()
		: IResponse{KIND_UPDATE_INFO, {}, {}}
	{
	}

	Response_Update_Info::Response_Update_Info(id_t _client_id, Error _error)
		: IResponse{KIND_UPDATE_INFO, _client_id, std::move(_error)}
	{
	}

	Command_Update_Info::Command_Update_Info(id_t client_id, Client_Info _new_info)
		: ICommand(client_id), new_info{std::move(_new_info)}
	{
	}

	void
	Command_Update_Info::execute(Server *server)
	{
	}

	void
	Command_Update_Info::undo(Server *server)
	{
	}

	void
	Command_Update_Info::redo(Server *server)
	{
	}
}
