#include "sws/Server.h"

namespace sws
{
	void
	Session::serve(tcp::Connection con, std::shared_ptr<Request_Queue> requests, std::shared_ptr<Response_Queue> responses, std::shared_ptr<std::atomic_flag> should_exit)
	{
		// TODO: Better termination
		// TODO: send ack?
		while (should_exit->test() == false)
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
		: requests{std::make_shared<Request_Queue>()}, responses{std::make_shared<Response_Queue>()},
		  serving_thread{serve, std::move(con), requests, responses}
	{
	}

	Session::~Session()
	{
		disconnect();
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
	Session::disconnect()
	{
		serving_thread.exit();
	}

	bool
	Session::client_disconnected()
	{
		return serving_thread.is_done();
	}

	Server::Client::Client(tcp::Connection &&con)
		: data{}, session{std::move(con)}, log{}
	{
	}

	Server::Server()
		: listening_thread_connections{std::make_shared<Connection_Queue>()},
		  listening_thread{listen_for_connections, listening_thread_connections}
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

		if (auto err = new_info.is_valid())
			return Error{"Invalid info: {}", err};

		auto info     = static_cast<Client_Info *>(&client.data);
		auto old_info = *info;
		*info         = new_info;
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
	Server::listen_for_connections(std::shared_ptr<Connection_Queue> queue, std::shared_ptr<std::atomic_flag> should_exit)
	{
		tcp::Server server{};
		id_t next_client_id = 0;
		while (should_exit->test() == false)
		{
			auto connection = server.accept(200); // timeout = 200 ms
			if (connection.is_valid())
				queue->push({next_client_id++, std::move(connection)});
		}
	}

	std::shared_ptr<Server>
	Server::instance()
	{
		static std::shared_ptr<Server> SERVER{new Server};
		return SERVER;
	}

	void
	Server::update_state()
	{
		// Start sessions
		while (listening_thread_connections->empty() == false)
		{
			auto [id, con] = listening_thread_connections->pop();
			active_clients.emplace(id, std::move(con)); // start a new session
			active_clients.at(id).data.client_id = id;
		}

		std::vector<id_t> clients_to_remove;
		for (auto &[id, client] : active_clients)
		{
			// See if there's any requests
			while (true)
			{
				auto req = client.session.receive_request();
				if (req == nullptr)
					break;

				auto res = client.log.execute_new_command(this, req->command(id));
				client.session.send_response(std::move(res));
			}

			// See if there's any disconnects
			if (client.session.client_disconnected())
				clients_to_remove.push_back(id);
		}

		for (auto id: clients_to_remove)
			active_clients.erase(id);
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

		Client_Data_with_Logs data         = {};
		*static_cast<Client_Data *>(&data) = client.data;
		data.logs                          = client.log.describe_commands();

		return data;
	}

	Error
	Server::undo(id_t client_id)
	{
		if (active_clients.contains(client_id) == false)
			return Error{"Client not found"};
		auto &client = active_clients.at(client_id);

		return client.log.undo_prev_command(this);
	}

	Error
	Server::redo(id_t client_id)
	{
		if (active_clients.contains(client_id) == false)
			return Error{"Client not found"};
		auto &client = active_clients.at(client_id);

		return client.log.redo_next_command(this);
	}

	void
	Server::stop_listening_for_connections()
	{
		listening_thread.exit();
	}
}