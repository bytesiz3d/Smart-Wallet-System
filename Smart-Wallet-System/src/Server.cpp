#include "sws/Server.h"

namespace sws
{
	void
	Session::serve(tcp::Connection con, std::shared_ptr<Request_Queue> requests, std::shared_ptr<Response_Queue> responses)
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

	Session::Session(tcp::Connection &&con)
	{
		serving_thread = std::thread(serve, std::move(con), requests, responses);
	}

	Session::~Session()
	{
		wait_for_disconnect();
	}

	std::unique_ptr<IRequest>
	Session::receive_request()
	{
		if (requests->empty())
			return nullptr;

		return requests->pop();
	}

	void
	Session::send_response(std::unique_ptr<IResponse> &&response)
	{
		responses->push(std::move(response));
	}

	void
	Session::wait_for_disconnect()
	{
		serving_thread.join();
	}

	Server::Client::Client(tcp::Connection &&con)
		: session{std::move(con)}
	{
	}

	Error
	Server::deposit(id_t client_id, uint64_t amount)
	{
		if (active_clients.contains(client_id) == false)
			return Error{"Client not found"};
		auto &client = active_clients.at(client_id);

		client.data.balance += amount;
		return Error{};
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
		return Error{};
	}

	Result<Client_Info>
	Server::update_info(id_t client_id, const Client_Info &new_info)
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

	void
	Server::listen_for_connections(Server *self)
	{
		tcp::Server server;
		while (self->listening_thread_should_exit.test() == false)
		{
			auto connection = server.accept();
			self->listening_thread_connections.push({self->next_client_id++, std::move(connection)});
		}
	}

	Server *
	Server::instance()
	{
		static Server SERVER{};
		return &SERVER;
	}

	void
	Server::update_state()
	{
		// Start sessions
		while (listening_thread_connections.empty())
		{
			auto [id, con] = listening_thread_connections.pop();
			active_clients.emplace(id, std::move(con)); // start a new session
		}

		// See if there's any requests
		for (auto &[id, client] : active_clients)
		{
			while (true)
			{
				auto req = client.session.receive_request();
				if (req == nullptr)
					break;

				auto res = client.log.execute_new_command(this, req->command());
				client.session.send_response(std::move(res));
			}
		}
	}

	std::vector<id_t>
	Server::clients()
	{
		std::vector<id_t> ids;
		for (auto &[id, client] : active_clients)
			ids.push_back(id);
		return ids;
	}

	Client_Data_with_Logs
	Server::client_data(id_t client_id)
	{
		assert(active_clients.contains(client_id));

		auto &client = active_clients.at(client_id);

		Client_Data_with_Logs data = {};
		*static_cast<Client_Data*>(&data) = client.data;
		data.logs = client.log.describe_commands();

		return data;
	}
}