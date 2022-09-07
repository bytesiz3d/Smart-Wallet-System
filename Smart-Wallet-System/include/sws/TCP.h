#pragma once
#include <nlohmann/json.hpp>

using Json = nlohmann::json;
namespace sws::tcp
{
	class Connection
	{
	protected:
		int handle;

	public:
		Connection(int _handle);

		~Connection();

		Connection(Connection &&con);

		bool
		is_valid() const;

		void
		send_message(const Json &json) const;

		Json
		receive_message() const;
	};

	class Client : public Connection
	{
	public:
		Client();
	};

	class Server
	{
		constexpr static size_t MAX_CLIENTS = 8;
		int handle;

	public:
		Server();

		~Server();

		Connection
		accept() const;
	};
}
