#include "sws/Client.h"
#include "sws/Transaction.h"
#include "sws/Undo_Redo.h"

namespace sws
{
	Error
	Client::send_request(std::shared_ptr<tcp::Client> client, Json json)
	{
		client->send_message(json);
		auto res_json = client->receive_message();
		auto res = IResponse::deserialize_base(res_json);
		return res->error;
	}

	Client::Client()
		: tcp_client{std::make_shared<tcp::Client>()}
	{
	}

	std::future<Error>
	Client::deposit(uint64_t amount)
	{
		Request_Deposit deposit{Transaction{amount}};
		return std::async(send_request, tcp_client, deposit.serialize());
	}

	std::future<Error>
	Client::withdraw(uint64_t amount)
	{
		Request_Withdrawal withdrawal{Transaction{amount}};
		return std::async(send_request, tcp_client, withdrawal.serialize());
	}

	std::future<Error>
	Client::update_info(const Client_Info &new_info)
	{
		Request_Update_Info update{new_info};
		return std::async(send_request, tcp_client, update.serialize());
	}

	std::future<Result<uint64_t>>
	Client::query_balance()
	{
		Request_Query_Balance query{};

		return std::async([client = tcp_client, json = query.serialize()] {
			client->send_message(json);
			auto res_json = client->receive_message();
			Response_Query_Balance res{};
			res.deserialize(res_json);
			return res.result();
		});
	}

	std::future<Error>
	Client::undo()
	{
		Request_Undo undo{};
		return std::async(send_request, tcp_client, undo.serialize());
	}

	std::future<Error>
	Client::redo()
	{
		Request_Redo redo{};
		return std::async(send_request, tcp_client, redo.serialize());
	}

	bool
	Client::has_valid_connection()
	{
		return tcp_client->is_valid();
	}
}