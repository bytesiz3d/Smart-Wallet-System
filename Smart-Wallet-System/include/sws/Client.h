#pragma once
#include "sws/TCP.h"
#include "sws/Client_Info.h"
#include <future>

namespace sws
{
	// TODO: Singleton
	class Response_Future : public std::future<std::unique_ptr<IResponse>>
	{
	public:
		Response_Future() = default;

		Response_Future(std::future<std::unique_ptr<IResponse>> &&ft)
			: std::future<std::unique_ptr<IResponse>>{std::move(ft)}
		{
		}

		bool
		is_done()
		{
			auto status = this->wait_for(std::chrono::seconds{0});
			return status != std::future_status::timeout;
		}
	};

	class Client
	{
		std::shared_ptr<tcp::Client> tcp_client;

		static std::unique_ptr<IResponse>
		send_request(std::shared_ptr<tcp::Client> client, Json json);

	public:
		Client();

		Response_Future
		deposit(uint64_t amount);

		Response_Future
		withdraw(uint64_t amount);

		Response_Future
		update_info(const Client_Info &new_info);

		Response_Future
		query_balance();

		Response_Future
		undo();

		Response_Future
		redo();

		bool
		has_valid_connection();
	};
}