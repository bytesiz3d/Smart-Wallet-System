#include "sws/Transaction.h"

#include "sws/Serializer.h"
#include <utility>

namespace sws
{
	Error
	ITransaction::is_valid() const
	{
		if (amount <= 0)
			return Error{"Invalid amount"};

		return Error{};
	}

	ITransaction::ITransaction(uint64_t _amount)
		: amount{_amount}
	{
	}

	Tx_Deposit::Tx_Deposit(uint64_t _amount)
		: ITransaction{_amount}
	{
	}

	Request_Deposit::Request_Deposit(id_t _client_id, Tx_Deposit _deposit)
		: IRequest{IRequest::KIND_DEPOSIT, _client_id}, deposit{std::move(_deposit)}
	{
	}

	void
	Request_Deposit::serialize(Serializer &serializer)
	{
		IRequest::serialize(serializer);
		serializer.push(deposit.amount);
	}

	void
	Request_Deposit::deserialize(Deserializer &deserializer)
	{
		deposit.amount = deserializer.read<decltype(deposit.amount)>();
	}

	std::unique_ptr<ICommand>
	Request_Deposit::command()
	{
		return std::make_unique<Command_Deposit>(client_id, deposit);
	}

	Response_Deposit::Response_Deposit(id_t _client_id, Error _error)
		: IResponse{IRequest::KIND_DEPOSIT, _client_id, std::move(_error)}
	{
	}

	Command_Deposit::Command_Deposit(id_t client_id, Tx_Deposit _deposit)
		: ICommand(client_id), deposit{std::move(_deposit)}
	{
	}

	void
	Command_Deposit::execute(Server *server)
	{
	}

	void
	Command_Deposit::undo(Server *server)
	{
	}

	void
	Command_Deposit::redo(Server *server)
	{
	}

	Tx_Withdrawal::Tx_Withdrawal(uint64_t _amount)
		: ITransaction(_amount)
	{
	}

	Request_Withdrawal::Request_Withdrawal(id_t _client_id, Tx_Withdrawal _withdrawal)
		: IRequest{IRequest::KIND_WITHDRAWAL, _client_id}, withdrawal{std::move(_withdrawal)}
	{
	}

	void
	Request_Withdrawal::serialize(Serializer &serializer)
	{
		IRequest::serialize(serializer);
		serializer.push(withdrawal.amount);
	}

	void
	Request_Withdrawal::deserialize(Deserializer &deserializer)
	{
		withdrawal.amount = deserializer.read<decltype(withdrawal.amount)>();
	}

	std::unique_ptr<ICommand>
	Request_Withdrawal::command()
	{
		return std::make_unique<Command_Withdrawal>(client_id, withdrawal);
	}

	Response_Withdrawal::Response_Withdrawal(id_t _client_id, Error _error)
		: IResponse{IRequest::KIND_WITHDRAWAL, _client_id, std::move(_error)}
	{
	}

	Command_Withdrawal::Command_Withdrawal(id_t client_id, Tx_Withdrawal _withdrawal)
		: ICommand(client_id), withdrawal(std::move(_withdrawal))
	{
	}

	void
	Command_Withdrawal::execute(Server *server)
	{
	}

	void
	Command_Withdrawal::undo(Server *server)
	{
	}

	void
	Command_Withdrawal::redo(Server *server)
	{
	}

	Request_Query_Balance::Request_Query_Balance(id_t _client_id)
		: IRequest{KIND_QUERY_BALANCE, _client_id}
	{
	}

	std::unique_ptr<ICommand>
	Request_Query_Balance::command()
	{
		return std::make_unique<Command_Query_Balance>(client_id);
	}

	Response_Query_Balance::Response_Query_Balance(id_t _client_id, Error _error, uint64_t _amount)
		: IResponse{KIND_QUERY_BALANCE, _client_id, std::move(_error)}, amount{_amount}
	{
	}

	void
	Response_Query_Balance::serialize(Serializer &serializer)
	{
		IResponse::serialize(serializer);
		serializer.push(amount);
	}

	void
	Response_Query_Balance::deserialize(Deserializer &deserializer)
	{
		IResponse::deserialize(deserializer);
		amount = deserializer.read<decltype(amount)>();
	}

	Command_Query_Balance::Command_Query_Balance(id_t client_id)
		: ICommand{client_id}
	{
	}

	void
	Command_Query_Balance::execute(Server *server)
	{
	}

	void
	Command_Query_Balance::undo(Server *server)
	{
	}

	void
	Command_Query_Balance::redo(Server *server)
	{
	}
}