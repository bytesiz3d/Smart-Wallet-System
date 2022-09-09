#include "sws/Server.h"
#include "sws/Client_ID.h"
#include "sws/Transaction.h"

#include <fstream>

namespace sws
{
	void
	Session::serve(tcp::Connection con, std::shared_ptr<Request_Queue> requests, std::shared_ptr<Response_Queue> responses, std::shared_ptr<std::atomic_flag> should_exit)
	{
		while (should_exit->test() == false)
		{
			auto msg = con.receive_message(400);
			if (msg.empty())
			{ // Ping client to see if they're online
				if (con.ping() == false)
					return;
				else
					continue;
			}

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

	Server::Server()
		: listening_thread_connections{std::make_shared<Connection_Queue>()},
		  listening_thread{listen_for_connections, listening_thread_connections}
	{
		load_registered_clients("server.json");
	}

	Server::~Server()
	{
		save_registered_clients("server.json");
	}

	Error
	Server::deposit(cid_t client_id, Transaction deposit)
	{
		if (active_clients.contains(client_id) == false)
			return Error{"Client not found"};
		auto &client = registered_clients.at(client_id);

		if (auto err = deposit.is_valid())
			return err;

		client.balance += deposit.amount;
		return Error{};
	}

	Error
	Server::withdraw(cid_t client_id, Transaction withdrawal)
	{
		if (active_clients.contains(client_id) == false)
			return Error{"Client not found"};
		auto &client = registered_clients.at(client_id);

		if (auto err = withdrawal.is_valid())
			return err;

		if (client.balance < withdrawal.amount)
			return Error{"Insufficient funds"};

		client.balance -= withdrawal.amount;
		return Error{};
	}

	Result<Client_Info>
	Server::update_info(cid_t client_id, const Client_Info &new_info)
	{
		if (active_clients.contains(client_id) == false)
			return Error{"Client not found"};
		auto &client = registered_clients.at(client_id);

		if (auto err = new_info.is_valid())
			return Error{"Invalid info: {}", err};

		auto info     = static_cast<Client_Info *>(&client);
		auto old_info = *info;
		*info         = new_info;
		return old_info;
	}

	Result<uint64_t>
	Server::query_balance(cid_t client_id)
	{
		if (active_clients.contains(client_id) == false)
			return Error{"Client not found"};
		auto &client = registered_clients.at(client_id);

		return client.balance;
	}

	Error
	Server::undo(cid_t client_id)
	{
		if (active_clients.contains(client_id) == false)
			return Error{"Client not found"};
		auto &client = registered_clients.at(client_id);

		return client.log.undo_prev_command(this, client_id);
	}

	Error
	Server::redo(cid_t client_id)
	{
		if (active_clients.contains(client_id) == false)
			return Error{"Client not found"};
		auto &client = registered_clients.at(client_id);

		return client.log.redo_next_command(this, client_id);
	}

	// -1 to start a new client
	Result<cid_t>
	Server::new_session_with_id(cid_t client_id)
	{
		if (client_id == -1)
		{
			::srandom(::clock());
			return ::random();
		}

		if (registered_clients.contains(client_id) == false)
			return Error{"ID doesn't exist"};
		return client_id;
	}

	void
	Server::listen_for_connections(std::shared_ptr<Connection_Queue> queue, std::shared_ptr<std::atomic_flag> should_exit)
	{
		tcp::Server server{};
		while (should_exit->test() == false)
		{
			auto connection = server.accept(200); // timeout = 200 ms
			if (connection.is_valid())
				queue->push(std::move(connection));
		}
	}

	void
	Server::start_session(tcp::Connection &&con)
	{
		auto msg = con.receive_message(400);
		if (msg.empty()) // Client didn't request ID
			return;

		auto req = IRequest::deserialize_base(msg);
		auto req_id = dynamic_cast<Request_ID *>(req.get());
		if (req_id == nullptr)
			return;

		auto res = req_id->command()->execute(this, -1); // use a dummy caller client id
		auto res_id = dynamic_cast<Response_ID *>(res.get());
		if (res_id == nullptr)
			return;

		con.send_message(res_id->serialize());

		if (res_id->error)
			return;

		// New session is valid
		auto client_id = res_id->get_id();

		// Add to registered clients if needed
		if (registered_clients.contains(client_id) == false)
		{
			registered_clients.emplace(client_id, Client{});
			registered_clients.at(client_id).client_id = client_id;
		}

		active_clients.emplace(client_id, std::move(con));
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
			start_session(listening_thread_connections->pop());

		std::vector<cid_t> clients_to_remove;
		for (auto &[id, session] : active_clients)
		{
			assert(registered_clients.contains(id));
			auto &client = registered_clients.at(id);

			// See if there's any requests
			while (true)
			{
				auto req = session.receive_request();
				if (req == nullptr)
					break;

				auto res = client.log.execute_new_command(this, id, req->command());
				session.send_response(std::move(res));
			}

			// See if there's any disconnects
			if (session.client_disconnected())
				clients_to_remove.push_back(id);
		}

		for (auto id : clients_to_remove)
			active_clients.erase(id);
	}

	std::vector<std::pair<cid_t, bool>>
	Server::clients()
	{
		std::vector<std::pair<cid_t, bool>> ids;
		for (auto &[id, client] : registered_clients)
			ids.emplace_back(id, active_clients.contains(id));

		return ids;
	}

	Client_Data_with_Logs
	Server::client_data(cid_t client_id)
	{
		assert(registered_clients.contains(client_id));
		auto &client = registered_clients.at(client_id);

		Client_Data_with_Logs data = {};

		*static_cast<Client_Info *>(&data) = *static_cast<Client_Info *>(&client);
		data.client_id = client.client_id;
		data.balance   = client.balance;
		data.logs      = client.log.describe_commands();

		return data;
	}

	void
	Server::stop_listening_for_connections()
	{
		listening_thread.exit();
	}

	void
	Server::load_registered_clients(std::string_view path)
	{
		std::ifstream input{path.data()};
		if (input.is_open() == false)
			return;

		Json json = Json::parse(input);
		if (json.is_array() == false)
			return;

		for (auto &j: json)
		{
			Client client{};
			client.deserialize(j);
			registered_clients.emplace(client.client_id, std::move(client));
		}
	}

	void
	Server::save_registered_clients(std::string_view path)
	{
		std::ofstream output{path.data()};
		if (output.is_open() == false)
			return;

		Json arr = Json::array();
		for (auto &[_, client] : registered_clients)
			arr.push_back(client.serialize());

		output << arr;
	}

	Json
	Server::Client::serialize() const
	{
		auto json = Client_Info::serialize();
		json["id"] = client_id;
		json["balance"] = balance;
		json["log"] = log.serialize();
		return json;
	}

	void
	Server::Client::deserialize(const Json &json)
	{
		Client_Info::deserialize(json);
		client_id = json["id"];
		balance = json["balance"];
		log.deserialize(json["log"]);
	}
}