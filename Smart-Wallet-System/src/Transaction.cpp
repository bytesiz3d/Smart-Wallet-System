#include "sws/Transaction.h"
#include "sws/Server.h"
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

	Request_Deposit::Request_Deposit()
		: IRequest{IRequest::KIND_DEPOSIT, {}}, deposit{0}
	{
	}

	Request_Deposit::Request_Deposit(id_t _client_id, Tx_Deposit _deposit)
		: IRequest{IRequest::KIND_DEPOSIT, _client_id}, deposit{std::move(_deposit)}
	{
	}

	Json
	Request_Deposit::serialize()
	{
		auto req = IRequest::serialize();
		req["request"] = {
			{"amount", deposit.amount}
		};
		return req;
	}

	void
	Request_Deposit::deserialize(const Json &json)
	{
		IRequest::deserialize(json);
		deposit.amount = json["request"]["amount"];
	}

	std::unique_ptr<ICommand>
	Request_Deposit::command()
	{
		return std::make_unique<Command_Deposit>(client_id, deposit);
	}

	Response_Deposit::Response_Deposit()
		: IResponse{IRequest::KIND_DEPOSIT, {}, {}}
	{
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

	Request_Withdrawal::Request_Withdrawal()
		: IRequest{IRequest::KIND_WITHDRAWAL, {}}, withdrawal{0}
	{
	}

	Request_Withdrawal::Request_Withdrawal(id_t _client_id, Tx_Withdrawal _withdrawal)
		: IRequest{IRequest::KIND_WITHDRAWAL, _client_id}, withdrawal{std::move(_withdrawal)}
	{
	}

	Json
	Request_Withdrawal::serialize()
	{
		auto req = IRequest::serialize();
		req["request"] = {
			{"amount", withdrawal.amount}
		};
		return req;
	}

	void
	Request_Withdrawal::deserialize(const Json &json)
	{
		IRequest::deserialize(json);
		withdrawal.amount = json["request"]["amount"];
	}

	std::unique_ptr<ICommand>
	Request_Withdrawal::command()
	{
		return std::make_unique<Command_Withdrawal>(client_id, withdrawal);
	}

	Response_Withdrawal::Response_Withdrawal()
		: IResponse{IRequest::KIND_WITHDRAWAL, {}, {}}
	{
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

	Json
	Response_Query_Balance::serialize()
	{
		auto res = IResponse::serialize();
		res["response"] = {
			{"amount", amount}
		};
		return res;
	}

	void
	Response_Query_Balance::deserialize(const Json &json)
	{
		IResponse::deserialize(json);
		amount = json["response"]["amount"];
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