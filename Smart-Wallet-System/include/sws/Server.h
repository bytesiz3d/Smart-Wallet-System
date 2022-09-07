#pragma once

#include "sws/Base.h"
#include "sws/TCP.h"
#include "sws/Client_Info.h"

#include <thread>
#include <queue>

namespace sws
{
	class IRequest;
	class IResponse;

	// Client thread to server
	using Request_Queue = Thread_Safe_Queue<IRequest>;
	// Server to client thread
	using Response_Queue = Thread_Safe_Queue<IResponse>;

	class Client_Session
	{
		std::thread serving_thread;

		std::shared_ptr<Request_Queue> requests;
		std::shared_ptr<Response_Queue> responses;

		static void
		serve(tcp::Connection con, std::shared_ptr<Request_Queue> requests, std::shared_ptr<Response_Queue> responses);

	public:
		explicit Client_Session(tcp::Connection &&con);
		~Client_Session();

		std::unique_ptr<IRequest>
		receive_request();

		void
		send_response(std::unique_ptr<IResponse> &&response);

		void
		wait_for_disconnect();
	};

	class Client_Data : public Client_Info
	{
		id_t client_id;
		uint64_t balance;
	};

	class Client
	{
		Client_Data data;
		Client_Session session;
	};

	class Server
	{
		std::thread listening_thread;
		std::unordered_map<id_t, Client> active_clients;
	};
};