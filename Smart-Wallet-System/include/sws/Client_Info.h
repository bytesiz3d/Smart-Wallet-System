#pragma once
#include "sws/Message.h"
#include "sws/Command.h"
#include <string>

namespace sws
{
	class Client_Info
	{
	public:
		std::string name;
		uint8_t age;

		constexpr static uint64_t NATIONAL_ID_MAX = uint64_t(1e15); // 14 digits
		uint64_t national_id;

		std::string address;

		Error
		is_valid() const;
	};

	class Request_Update_Info : public IRequest
	{
		Client_Info client_info;

	public:
		Request_Update_Info(); // uses deserialize for initialization
		Request_Update_Info(id_t _client_id, Client_Info _client_info);

		Json
		serialize() override;

		void
		deserialize(const Json &json) override;

		std::unique_ptr<ICommand>
		command() override;
	};

	class Response_Update_Info : public IResponse
	{
	public:
		Response_Update_Info(); // uses deserialize
		Response_Update_Info(id_t _client_id, Error _error);
	};

	class Server;
	class Command_Update_Info : public ICommand
	{
		Client_Info old_info, new_info;
	public:
		explicit Command_Update_Info(id_t client_id, Client_Info _new_info);

		void
		execute(Server *server) override;

		void
		undo(Server *server) override;

		void
		redo(Server *server) override;
	};
}