#pragma once
#include "sws/Message.h"
#include "sws/Command.h"
#include "sws/Server.h"

#include <stdint.h>

namespace sws
{
	class ITransaction
	{
	public:
		uint64_t amount;

	protected:
		explicit ITransaction(uint64_t _amount);

		virtual Error
		is_valid() const final;
	};

	class Tx_Deposit : public ITransaction
	{
	public:
		explicit Tx_Deposit(uint64_t _amount);
	};

	class Request_Deposit : public IRequest
	{
		Tx_Deposit deposit;

	public:
		Request_Deposit(id_t _client_id, Tx_Deposit _deposit);

		void
		serialize(Serializer &serializer) override;

		void
		deserialize(Deserializer &deserializer) override;

		std::unique_ptr<ICommand>
		command() override;
	};

	class Response_Deposit : public IResponse
	{
	public:
		Response_Deposit(id_t _client_id, Error _error);
	};

	class Command_Deposit : public ICommand
	{
		Tx_Deposit deposit;
	public:
		explicit Command_Deposit(id_t client_id, Tx_Deposit _deposit);

		void
		execute(Server *server) override;

		void
		undo(Server *server) override;

		void
		redo(Server *server) override;
	};

	class Tx_Withdrawal : public ITransaction
	{
	public:
		explicit Tx_Withdrawal(uint64_t _amount);
	};

	class Request_Withdrawal : public IRequest
	{
		Tx_Withdrawal withdrawal;

	public:
		Request_Withdrawal(id_t _client_id, Tx_Withdrawal _withdrawal);

		void
		serialize(Serializer &serializer) override;

		void
		deserialize(Deserializer &deserializer) override;

		std::unique_ptr<ICommand>
		command() override;
	};

	class Response_Withdrawal : public IResponse
	{
	public:
		Response_Withdrawal(id_t _client_id, Error _error);
	};

	class Command_Withdrawal : public ICommand
	{
		Tx_Withdrawal withdrawal;
	public:
		explicit Command_Withdrawal(id_t client_id, Tx_Withdrawal _withdrawal);

		void
		execute(Server *server) override;

		void
		undo(Server *server) override;

		void
		redo(Server *server) override;
	};

	class Request_Query_Balance : public IRequest
	{
	public:
		explicit Request_Query_Balance(id_t _client_id);

		std::unique_ptr<ICommand>
		command() override;
	};

	class Response_Query_Balance : public IResponse
	{
		uint64_t amount;

	public:
		Response_Query_Balance(id_t _client_id, Error _error, uint64_t _amount);

		void
		serialize(Serializer &serializer) override;

		void
		deserialize(Deserializer &deserializer) override;
	};

	class Command_Query_Balance : public ICommand
	{
	public:
		explicit Command_Query_Balance(id_t client_id);

		void
		execute(Server *server) override;

		void
		undo(Server *server) override;

		void
		redo(Server *server) override;
	};
}