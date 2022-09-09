#include "sws/Client.h"
#include "sws/Transaction.h"
#include "sws/Undo_Redo.h"
#include "sws/Client_ID.h"

namespace sws
{
	std::unique_ptr<IResponse>
	Client::send_request(std::shared_ptr<tcp::Client> client, Json json)
	{
		bool send_ok = client->send_message(json);
		if (send_ok == false)
			return std::make_unique<Response_Timeout>();

		while (true)
		{
			auto res_json = client->receive_message(2000);

			if (res_json.empty())
				return std::make_unique<Response_Timeout>();

			if (client->message_is_ping(res_json)) // Received ping from server
			{
				auto hhh = res_json.dump();
				continue;
			}

			return IResponse::deserialize_base(res_json);
		}
	}

	Client::Client(cid_t _id)
		: id{-1}, tcp_client{std::make_shared<tcp::Client>()}
	{
		if (tcp_client->is_valid() == false)
			return;

		// Request to set ID
		Request_ID set_id{_id};
		auto res = send_request(tcp_client, set_id.serialize());
		if (auto res_id = dynamic_cast<Response_ID*>(res.get()))
		{
			id = res_id->get_id();
		}
		else
		{ // invalidate tcp client
			auto discard = std::move(tcp_client);
		}
	}

	Response_Future
	Client::deposit(int64_t amount)
	{
		Request_Deposit deposit{Transaction{amount}};
		return std::async(send_request, tcp_client, deposit.serialize());
	}

	Response_Future
	Client::withdraw(int64_t amount)
	{
		Request_Withdrawal withdrawal{Transaction{amount}};
		return std::async(send_request, tcp_client, withdrawal.serialize());
	}

	Response_Future
	Client::update_info(const Client_Info &new_info)
	{
		Request_Update_Info update{new_info};
		return std::async(send_request, tcp_client, update.serialize());
	}

	Response_Future
	Client::query_balance()
	{
		Request_Query_Balance query{};
		return std::async(send_request, tcp_client, query.serialize());
	}

	Response_Future
	Client::undo()
	{
		Request_Undo undo{};
		return std::async(send_request, tcp_client, undo.serialize());
	}

	Response_Future
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