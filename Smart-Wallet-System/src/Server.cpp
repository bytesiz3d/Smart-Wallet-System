#include "sws/Server.h"

namespace sws
{
	void
	Client_Session::serve(tcp::Connection con, std::shared_ptr<Request_Queue> requests, std::shared_ptr<Response_Queue> responses)
	{
		// TODO: send ack?
		while (true)
		{
			auto msg = con.receive_message();
			if (msg.empty())
				break;

			auto req = IRequest::deserialize_base(msg);
			requests->push(std::move(req));

			while (responses->empty()) // Wait for the server to process the request
				std::this_thread::sleep_for(std::chrono::milliseconds(50));

			auto res = responses->pop();
			con.send_message(res->serialize());
		}
	}

	Client_Session::Client_Session(tcp::Connection &&con)
	{
		serving_thread = std::thread(serve, std::move(con), requests, responses);
	}

	Client_Session::~Client_Session()
	{
		wait_for_disconnect();
	}

	std::unique_ptr<IRequest>
	Client_Session::receive_request()
	{
		if (requests->empty())
			return nullptr;

		return requests->pop();
	}

	void
	Client_Session::send_response(std::unique_ptr<IResponse> &&response)
	{
		responses->push(std::move(response));
	}

	void
	Client_Session::wait_for_disconnect()
	{
		serving_thread.join();
	}

	Error
	Server::deposit(id_t client_id, uint64_t amount)
	{
		if (active_clients.contains(client_id) == false)
			return Error{"Client not found"};
		auto &client = active_clients.at(client_id);

		client.data.balance += amount;
	}

	Error
	Server::withdraw(id_t client_id, uint64_t amount)
	{
		if (active_clients.contains(client_id) == false)
			return Error{"Client not found"};
		auto &client = active_clients.at(client_id);

		if (client.data.balance < amount)
			return Error{"Insufficient funds"};

		client.data.balance -= amount;
	}

	Result<Client_Info>
	Server::update_info(id_t client_id, Client_Info &&new_info)
	{
		if (active_clients.contains(client_id) == false)
			return Error{"Client not found"};
		auto &client = active_clients.at(client_id);

		if (new_info.is_valid() == false)
			return Error{"Invalid info"};

		auto info = static_cast<Client_Info*>(&client.data);
		auto old_info = *info;
		*info = new_info;
		return old_info;
	}

	Result<uint64_t>
	Server::query_balance(id_t client_id)
	{
		if (active_clients.contains(client_id) == false)
			return Error{"Client not found"};
		auto &client = active_clients.at(client_id);

		return client.data.balance;
	}
}