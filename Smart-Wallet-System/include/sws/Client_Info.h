#pragma once
#include "sws/Message.h"
#include "sws/Command.h"
#include <string>

namespace sws
{
	struct Client_Info
	{
		std::string name;
		uint8_t age;

		static constexpr size_t NATIONAL_ID_LEN = 14;
		char national_id[NATIONAL_ID_LEN + 1];

		std::string address;

		Error
		is_valid() const;

		virtual Json
		serialize() const;

		virtual void
		deserialize(const Json &json);
	};

	class Request_Update_Info : public IRequest
	{
		Client_Info client_info;

	public:
		Request_Update_Info(); // uses deserialize for initialization
		explicit Request_Update_Info(Client_Info _client_info);

		Json
		serialize() const override;

		bool
		deserialize(const Json &json) override;

		std::unique_ptr<ICommand>
		command() override;
	};

	class Response_Update_Info : public IResponse
	{
	public:
		Response_Update_Info(); // uses deserialize
		explicit Response_Update_Info(Error _error);
	};

	class Server;
	class Command_Update_Info : public ICommand
	{
		Client_Info old_info, new_info;
	public:
		Command_Update_Info() = default;
		explicit Command_Update_Info(Client_Info _new_info);

		std::unique_ptr<IResponse>
		execute(Server *server, cid_t client_id) override;

		Error
		undo(Server *server, cid_t client_id) override;

		Error
		redo(Server *server, cid_t client_id) override;

		Json
		serialize() override;

		bool
		deserialize(const Json &json) override;
	};
}