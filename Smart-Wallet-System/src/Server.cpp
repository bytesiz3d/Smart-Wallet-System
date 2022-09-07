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
}