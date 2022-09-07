#pragma once
#include "sws/Message.h"
#include "sws/Command.h"
#include "sws/Server.h"

#include <stdint.h>

namespace sws
{
	struct Transaction
	{
		uint64_t amount;

		Error
		is_valid() const;
	};

	class Request_Deposit : public IRequest
	{
		Transaction deposit;

	public:
		Request_Deposit(); // uses deserialize for initialization
		Request_Deposit(Transaction _deposit);

		Json
		serialize() override;

		void
		deserialize(const Json &json) override;

		std::unique_ptr<ICommand>
		command(id_t client_id) override;
	};

	class Response_Deposit : public IResponse
	{
	public:
		Response_Deposit(); // uses deserialize
		Response_Deposit(Error _error);
	};

	class Command_Deposit : public ICommand
	{
		Transaction deposit;
	public:
		explicit Command_Deposit(id_t client_id, Transaction _deposit);

		std::unique_ptr<IResponse>
		execute(Server *server) override;

		void
		undo(Server *server) override;

		void
		redo(Server *server) override;

		std::string
		describe() override;
	};

	class Request_Withdrawal : public IRequest
	{
		Transaction withdrawal;

	public:
		Request_Withdrawal(); // uses deserialize
		Request_Withdrawal(Transaction _withdrawal);

		Json
		serialize() override;

		void
		deserialize(const Json &json) override;

		std::unique_ptr<ICommand>
		command(id_t client_id) override;
	};

	class Response_Withdrawal : public IResponse
	{
	public:
		Response_Withdrawal(); // uses deserialize
		Response_Withdrawal(Error _error);
	};

	class Command_Withdrawal : public ICommand
	{
		Transaction withdrawal;
	public:
		explicit Command_Withdrawal(id_t client_id, Transaction _withdrawal);

		std::unique_ptr<IResponse>
		execute(Server *server) override;

		void
		undo(Server *server) override;

		void
		redo(Server *server) override;

		std::string
		describe() override;
	};

	class Request_Query_Balance : public IRequest
	{
	public:
		explicit Request_Query_Balance();

		std::unique_ptr<ICommand>
		command(id_t client_id) override;
	};

	class Response_Query_Balance : public IResponse
	{
		uint64_t balance;

	public:
		Response_Query_Balance(Error _error, uint64_t _balance);

		Json
		serialize() override;

		void
		deserialize(const Json &json) override;
	};

	class Command_Query_Balance : public ICommand
	{
	public:
		explicit Command_Query_Balance(id_t client_id);

		std::unique_ptr<IResponse>
		execute(Server *server) override;

		void
		undo(Server *server) override;

		void
		redo(Server *server) override;

		std::string
		describe() override;
	};
}