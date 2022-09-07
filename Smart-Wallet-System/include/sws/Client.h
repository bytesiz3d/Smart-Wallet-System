#pragma once
#include "sws/TCP.h"
#include "sws/Client_Info.h"
#include <future>

namespace sws
{
	class Client
	{
		std::shared_ptr<tcp::Client> tcp_client;

		static Error
		send_request(std::shared_ptr<tcp::Client> client, Json json);

	public:
		Client();

		std::future<Error>
		deposit(uint64_t amount);

		std::future<Error>
		withdraw(uint64_t amount);

		std::future<Error>
		update_info(const Client_Info &new_info);

		std::future<Result<uint64_t>>
		query_balance();

		// TODO: UNDO/REDO REQUESTS
		std::future<Error>
		undo();

		std::future<Error>
		redo();
	};
}