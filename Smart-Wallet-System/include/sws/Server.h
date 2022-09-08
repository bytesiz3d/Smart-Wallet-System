#pragma once

#include "sws/Base.h"
#include "sws/TCP.h"
#include "sws/Client_Info.h"

#include <thread>
#include <queue>

namespace sws
{
	class Server;

	class IRequest;
	class IResponse;

	// Client thread to server
	using Request_Queue = Thread_Safe_Queue<std::unique_ptr<IRequest>>;
	// Server to client thread
	using Response_Queue = Thread_Safe_Queue<std::unique_ptr<IResponse>>;

	class Session
	{
		std::thread serving_thread;

		std::shared_ptr<Request_Queue> requests;
		std::shared_ptr<Response_Queue> responses;

		static void
		serve(tcp::Connection con, std::shared_ptr<Request_Queue> requests, std::shared_ptr<Response_Queue> responses);

	public:
		Session() = delete;
		explicit Session(tcp::Connection &&con);
		~Session();

		std::unique_ptr<IRequest>
		receive_request();

		void
		send_response(std::unique_ptr<IResponse> &&response);

		void
		wait_for_disconnect();
	};

	struct Client_Data : public Client_Info
	{
		id_t client_id;
		uint64_t balance;
	};

	struct Client_Data_with_Logs : public Client_Data
	{
		std::vector<std::string> logs;
	};

	class Server
	{
		std::thread listening_thread;
		std::atomic_flag listening_thread_should_exit;
		Thread_Safe_Queue<std::pair<id_t, tcp::Connection>> listening_thread_connections;
		std::atomic<id_t> next_client_id;

		struct Client
		{
			Client_Data data;
			Session session;
			Command_Log log;

			Client() = delete;
			explicit Client(tcp::Connection &&con);
		};
		std::unordered_map<id_t, Client> active_clients;

		static void
		listen_for_connections(std::shared_ptr<Server> self);

		Server();
	public:
		static std::shared_ptr<Server>
		instance();

		Error
		deposit(id_t client_id, uint64_t amount);

		Error
		withdraw(id_t client_id, uint64_t amount);

		Result<Client_Info>
		update_info(id_t client_id, const Client_Info &new_info);

		Result<uint64_t>
		query_balance(id_t client_id);

		void
		update_state();

		std::vector<id_t>
		clients();

		Client_Data_with_Logs
		client_data(id_t client_id);
	};
};