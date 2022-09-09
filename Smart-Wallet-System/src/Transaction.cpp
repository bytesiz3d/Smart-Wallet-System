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
		: IRequest{KIND_DEPOSIT}, deposit{}
	{
	}

	Request_Deposit::Request_Deposit(Transaction _deposit)
		: IRequest{KIND_DEPOSIT}, deposit{_deposit}
	{
	}

	Json
	Request_Deposit::serialize() const
	{
		auto req       = IRequest::serialize();
		req["request"] = {
			{"amount", deposit.amount}
		};
		return req;
	}

	bool
	Request_Deposit::deserialize(const Json &json)
	{
		IRequest::deserialize(json);
		deposit.amount = json["request"]["amount"];
		return true;
	}

	std::unique_ptr<ICommand>
	Request_Deposit::command()
	{
		return std::make_unique<Command_Deposit>(deposit);
	}

	Response_Deposit::Response_Deposit()
		: IResponse{KIND_DEPOSIT, {}}
	{
	}

	Response_Deposit::Response_Deposit(Error _error)
		: IResponse{KIND_DEPOSIT, std::move(_error)}
	{
	}

	Command_Deposit::Command_Deposit(Transaction _deposit)
		: deposit{_deposit}
	{
	}

	std::unique_ptr<IResponse>
	Command_Deposit::execute(Server *server, cid_t client_id)
	{
		if (auto err = deposit.is_valid())
			return std::make_unique<Response_Deposit>(err);

		auto err = server->deposit(client_id, deposit);
		return std::make_unique<Response_Deposit>(err);
	}

	Error
	Command_Deposit::undo(Server *server, cid_t client_id)
	{
		return server->withdraw(client_id, deposit);
	}

	Error
	Command_Deposit::redo(Server *server, cid_t client_id)
	{
		return server->deposit(client_id, deposit);
	}

	Json
	Command_Deposit::serialize()
	{
		return Json{
			{"deposit", deposit.amount}
		};
	}

	bool
	Command_Deposit::deserialize(const Json &json)
	{
		if (json.size() != 1)
			return false;

		if (json.contains("deposit") == false)
			return false;

		deposit.amount = json["deposit"];
		return true;
	}

	Request_Withdrawal::Request_Withdrawal()
		: IRequest{KIND_WITHDRAWAL}, withdrawal{0}
	{
	}

	Request_Withdrawal::Request_Withdrawal(Transaction _withdrawal)
		: IRequest{KIND_WITHDRAWAL}, withdrawal{_withdrawal}
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

	bool
	Request_Withdrawal::deserialize(const Json &json)
	{
		IRequest::deserialize(json);
		withdrawal.amount = json["request"]["amount"];
		return true;
	}

	std::unique_ptr<ICommand>
	Request_Withdrawal::command()
	{
		return std::make_unique<Command_Withdrawal>(withdrawal);
	}

	Response_Withdrawal::Response_Withdrawal()
		: IResponse{KIND_WITHDRAWAL, {}}
	{
	}

	Response_Withdrawal::Response_Withdrawal(Error _error)
		: IResponse{KIND_WITHDRAWAL, std::move(_error)}
	{
	}

	Command_Withdrawal::Command_Withdrawal(Transaction _withdrawal)
		: withdrawal(_withdrawal)
	{
	}

	std::unique_ptr<IResponse>
	Command_Withdrawal::execute(Server *server, cid_t client_id)
	{
		if (auto err = withdrawal.is_valid())
			return std::make_unique<Response_Withdrawal>(err);

		auto err = server->withdraw(client_id, withdrawal);
		return std::make_unique<Response_Withdrawal>(err);
	}

	Error
	Command_Withdrawal::undo(Server *server, cid_t client_id)
	{
		return server->deposit(client_id, withdrawal);
	}

	Error
	Command_Withdrawal::redo(Server *server, cid_t client_id)
	{
		return server->withdraw(client_id, withdrawal);
	}

	Json
	Command_Withdrawal::serialize()
	{
		return Json{
			{"withdrawal", withdrawal.amount}
		};
	}

	bool
	Command_Withdrawal::deserialize(const Json &json)
	{
		if (json.size() != 1)
			return false;

		if (json.contains("withdrawal") == false)
			return false;

		withdrawal.amount = json["withdrawal"];
		return true;
	}

	Request_Query_Balance::Request_Query_Balance()
		: IRequest{KIND_QUERY_BALANCE}
	{
	}

	std::unique_ptr<ICommand>
	Request_Query_Balance::command()
	{
		return std::make_unique<Command_Query_Balance>();
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

	bool
	Response_Query_Balance::deserialize(const Json &json)
	{
		IResponse::deserialize(json);
		balance = json["response"]["balance"];
		return true;
	}

	uint64_t
	Response_Query_Balance::get_balance() const
	{
		return balance;
	}

	std::unique_ptr<IResponse>
	Command_Query_Balance::execute(Server *server, cid_t client_id)
	{
		auto [balance, err] = server->query_balance(client_id);
		return std::make_unique<Response_Query_Balance>(err, balance);
	}
}