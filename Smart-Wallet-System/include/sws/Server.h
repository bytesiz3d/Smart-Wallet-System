#pragma once

#include "sws/Base.h"
#include "sws/Client_Info.h"
#include "sws/TCP.h"

#include <queue>
#include <thread>

namespace sws
{
	class IRequest;
	class IResponse;
	struct Transaction;

	// Client thread to server
	using Request_Queue = Thread_Safe_Queue<std::unique_ptr<IRequest>>;
	// Server to client thread
	using Response_Queue = Thread_Safe_Queue<std::unique_ptr<IResponse>>;

	class Session
	{
		std::shared_ptr<Request_Queue> requests;
		std::shared_ptr<Response_Queue> responses;
		Thread_With_Exit_Flag serving_thread;

		static void
		serve(tcp::Connection con, std::shared_ptr<Request_Queue> requests, std::shared_ptr<Response_Queue> responses, std::shared_ptr<std::atomic_flag> should_exit);

	public:
		Session() = delete;
		explicit Session(tcp::Connection &&con);
		Session(Session &&) noexcept = default;
		~Session();

		std::unique_ptr<IRequest>
		receive_request();

		void
		send_response(std::unique_ptr<IResponse> &&response);

		bool
		client_disconnected();

		void
		disconnect();
	};

	struct Client_Data_with_Logs : public Client_Info
	{
		cid_t client_id;
		uint64_t balance;
		std::vector<std::string> logs;
	};

	class Server
	{
		using Connection_Queue = Thread_Safe_Queue<tcp::Connection>;
		std::shared_ptr<Connection_Queue> listening_thread_connections;
		Thread_With_Exit_Flag listening_thread;

		struct Client : public Client_Info
		{
			cid_t client_id;
			uint64_t balance;
			Command_Log log;

			Json
			serialize() const;

			void
			deserialize(const Json &json);
		};

		std::unordered_map<cid_t, Session> active_clients;
		std::unordered_map<cid_t, Client> registered_clients;

		static void
		listen_for_connections(std::shared_ptr<Connection_Queue> queue, std::shared_ptr<std::atomic_flag> should_exit);

		void
		start_session(tcp::Connection &&con);

		void
		load_registered_clients(std::string_view path);

		void
		save_registered_clients(std::string_view path);

		Server();
	public:
		static std::shared_ptr<Server>
		instance();

		~Server();

		Error
		deposit(cid_t client_id, Transaction deposit);

		Error
		withdraw(cid_t client_id, Transaction withdrawal);

		Result<Client_Info>
		update_info(cid_t client_id, const Client_Info &new_info);

		Result<uint64_t>
		query_balance(cid_t client_id);

		Error
		undo(cid_t client_id);

		Error
		redo(cid_t client_id);

		void
		update_state();

		Result<cid_t>
		new_session_with_id(cid_t client_id);

		std::vector<std::pair<cid_t, bool>>
		clients();

		Client_Data_with_Logs
		client_data(cid_t client_id);

		void
		stop_listening_for_connections();

		using Plot_Data = std::pair<std::vector<std::string>, std::vector<int>>;
		Plot_Data
		balance_distribution();

		Plot_Data
		highest_n_balances(size_t N = 5);
	};
};