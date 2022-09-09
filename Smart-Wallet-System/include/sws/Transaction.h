#pragma once
#include "sws/Message.h"
#include "sws/Command.h"
#include "sws/Server.h"

#include <stdint.h>

namespace sws
{
	struct Transaction
	{
		int64_t amount{};

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
		serialize() const override;

		bool
		deserialize(const Json &json) override;

		std::unique_ptr<ICommand>
		command(cid_t client_id) override;
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
		Command_Deposit() = default;
		Command_Deposit(cid_t client_id, Transaction _deposit);

		std::unique_ptr<IResponse>
		execute(Server *server) override;

		Error
		undo(Server *server) override;

		Error
		redo(Server *server) override;

		Json
		serialize() override;

		bool
		deserialize(const Json &json) override;
	};

	class Request_Withdrawal : public IRequest
	{
		Transaction withdrawal;

	public:
		Request_Withdrawal(); // uses deserialize
		explicit Request_Withdrawal(Transaction _withdrawal);

		Json
		serialize() const override;

		bool
		deserialize(const Json &json) override;

		std::unique_ptr<ICommand>
		command(cid_t client_id) override;
	};

	class Response_Withdrawal : public IResponse
	{
	public:
		Response_Withdrawal(); // uses deserialize
		explicit Response_Withdrawal(Error _error);
	};

	class Command_Withdrawal : public ICommand
	{
		Transaction withdrawal;
	public:
		Command_Withdrawal() = default;
		Command_Withdrawal(cid_t client_id, Transaction _withdrawal);

		std::unique_ptr<IResponse>
		execute(Server *server) override;

		Error
		undo(Server *server) override;

		Error
		redo(Server *server) override;

		Json
		serialize() override;

		bool
		deserialize(const Json &json) override;
	};

	class Request_Query_Balance : public IRequest
	{
	public:
		Request_Query_Balance();

		std::unique_ptr<ICommand>
		command(cid_t client_id) override;
	};

	class Response_Query_Balance : public IResponse
	{
		uint64_t balance;
	public:
		Response_Query_Balance(); // uses deserialize
		Response_Query_Balance(Error _error, uint64_t _balance);

		Json
		serialize() const override;

		bool
		deserialize(const Json &json) override;

		uint64_t
		get_balance() const;
	};

	class Command_Query_Balance : public IMetaCommand
	{
	public:
		Command_Query_Balance() = default;
		explicit Command_Query_Balance(cid_t client_id);

		std::unique_ptr<IResponse>
		execute(Server *server) override;
	};
}