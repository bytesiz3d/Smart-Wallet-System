#include "sws/Client_Info.h"
#include "sws/Serializer.h"
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

	Request_Update_Info::Request_Update_Info(id_t _client_id, Client_Info _client_info)
		: IRequest{KIND_UPDATE_INFO, _client_id}, client_info{std::move(_client_info)}
	{
	}

	void
	Request_Update_Info::serialize(Serializer &serializer)
	{
		IRequest::serialize(serializer);
		serializer.push(client_info.name);
		serializer.push(client_info.age);
		serializer.push(client_info.national_id);
		serializer.push(client_info.address);
	}

	void
	Request_Update_Info::deserialize(Deserializer &deserializer)
	{
		client_info.name        = deserializer.read<decltype(client_info.name)>();
		client_info.age         = deserializer.read<decltype(client_info.age)>();
		client_info.national_id = deserializer.read<decltype(client_info.national_id)>();
		client_info.address     = deserializer.read<decltype(client_info.address)>();
	}

	std::unique_ptr<ICommand>
	Request_Update_Info::command()
	{
		return std::make_unique<Command_Update_Info>(client_id, client_info);
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
