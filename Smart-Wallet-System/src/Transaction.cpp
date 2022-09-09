#include "sws/Transaction.h"
#include "sws/Server.h"
#include <utility>

namespace sws
{
	Error
	Transaction::is_valid() const
	{
		if (amount <= 0)
			return Error{"Invalid amount"};

		return Error{};
	}

	Request_Deposit::Request_Deposit()
		: IRequest{IRequest::KIND_DEPOSIT}, deposit{}
	{
	}

	Request_Deposit::Request_Deposit(Transaction _deposit)
		: IRequest{IRequest::KIND_DEPOSIT}, deposit{_deposit}
	{
	}

	Json
	Request_Deposit::serialize() const
	{
		auto req       = IRequest::serialize();
		req["request"] = {
			{"amount", deposit.amount}};
		return req;
	}

	void
	Request_Deposit::deserialize(const Json &json)
	{
		IRequest::deserialize(json);
		deposit.amount = json["request"]["amount"];
	}

	std::unique_ptr<ICommand>
	Request_Deposit::command(id_t client_id)
	{
		return std::make_unique<Command_Deposit>(client_id, deposit);
	}

	Response_Deposit::Response_Deposit()
		: IResponse{IRequest::KIND_DEPOSIT, {}}
	{
	}

	Response_Deposit::Response_Deposit(Error _error)
		: IResponse{IRequest::KIND_DEPOSIT, std::move(_error)}
	{
	}

	Command_Deposit::Command_Deposit(id_t client_id, Transaction _deposit)
		: ICommand(client_id), deposit{_deposit}
	{
	}

	std::unique_ptr<IResponse>
	Command_Deposit::execute(Server *server)
	{
		if (auto err = deposit.is_valid())
			return std::make_unique<Response_Deposit>(err);

		auto err = server->deposit(client_id, deposit.amount);
		return std::make_unique<Response_Deposit>(err);
	}

	Error
	Command_Deposit::undo(Server *server)
	{
		return server->withdraw(client_id, deposit.amount);
	}

	Error
	Command_Deposit::redo(Server *server)
	{
		return server->deposit(client_id, deposit.amount);
	}

	std::string
	Command_Deposit::describe()
	{
		return fmt::format("Deposit {}", deposit.amount);
	}

	Request_Withdrawal::Request_Withdrawal()
		: IRequest{IRequest::KIND_WITHDRAWAL}, withdrawal{0}
	{
	}

	Request_Withdrawal::Request_Withdrawal(Transaction _withdrawal)
		: IRequest{IRequest::KIND_WITHDRAWAL}, withdrawal{_withdrawal}
	{
	}

	Json
	Request_Withdrawal::serialize() const
	{
		auto req       = IRequest::serialize();
		req["request"] = {
			{"amount", withdrawal.amount}};
		return req;
	}

	void
	Request_Withdrawal::deserialize(const Json &json)
	{
		IRequest::deserialize(json);
		withdrawal.amount = json["request"]["amount"];
	}

	std::unique_ptr<ICommand>
	Request_Withdrawal::command(id_t client_id)
	{
		return std::make_unique<Command_Withdrawal>(client_id, withdrawal);
	}

	Response_Withdrawal::Response_Withdrawal()
		: IResponse{IRequest::KIND_WITHDRAWAL, {}}
	{
	}

	Response_Withdrawal::Response_Withdrawal(Error _error)
		: IResponse{IRequest::KIND_WITHDRAWAL, std::move(_error)}
	{
	}

	Command_Withdrawal::Command_Withdrawal(id_t client_id, Transaction _withdrawal)
		: ICommand(client_id), withdrawal(_withdrawal)
	{
	}

	std::unique_ptr<IResponse>
	Command_Withdrawal::execute(Server *server)
	{
		if (auto err = withdrawal.is_valid())
			return std::make_unique<Response_Withdrawal>(err);

		auto err = server->withdraw(client_id, withdrawal.amount);
		return std::make_unique<Response_Withdrawal>(err);
	}

	Error
	Command_Withdrawal::undo(Server *server)
	{
		return server->deposit(client_id, withdrawal.amount);
	}

	Error
	Command_Withdrawal::redo(Server *server)
	{
		return server->withdraw(client_id, withdrawal.amount);
	}

	std::string
	Command_Withdrawal::describe()
	{
		return fmt::format("Withdrawal {}", withdrawal.amount);
	}

	Request_Query_Balance::Request_Query_Balance()
		: IRequest{KIND_QUERY_BALANCE}
	{
	}

	std::unique_ptr<ICommand>
	Request_Query_Balance::command(id_t client_id)
	{
		return std::make_unique<Command_Query_Balance>(client_id);
	}

	Response_Query_Balance::Response_Query_Balance()
		: IResponse{KIND_QUERY_BALANCE, {}}, balance{}
	{
	}

	Response_Query_Balance::Response_Query_Balance(Error _error, uint64_t _balance)
		: IResponse{KIND_QUERY_BALANCE, std::move(_error)}, balance{_balance}
	{
	}

	Json
	Response_Query_Balance::serialize() const
	{
		auto res        = IResponse::serialize();
		res["response"] = {
			{"balance", balance}};
		return res;
	}

	void
	Response_Query_Balance::deserialize(const Json &json)
	{
		IResponse::deserialize(json);
		balance = json["response"]["balance"];
	}

	uint64_t
	Response_Query_Balance::get_balance() const
	{
		return balance;
	}

	Command_Query_Balance::Command_Query_Balance(id_t client_id)
		: IMetaCommand{client_id}
	{
	}

	std::unique_ptr<IResponse>
	Command_Query_Balance::execute(Server *server)
	{
		auto [balance, err] = server->query_balance(client_id);
		return std::make_unique<Response_Query_Balance>(err, balance);
	}

	std::string
	Command_Query_Balance::describe()
	{
		return "Query Balance";
	}
}