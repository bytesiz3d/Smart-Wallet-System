#pragma once
#include <nlohmann/json.hpp>

using Json = nlohmann::json;
namespace sws::tcp
{
	constexpr int TIMEOUT_INFINITE = -1;
	constexpr int TIMEOUT_INSTANT = 0;

	class Connection
	{
	protected:
		int handle;

		bool
		read_bytes(void *data, size_t size, int timeout_ms) const;

		bool
		write_bytes(void *data, size_t size) const;

	public:
		Connection(int _handle);

		~Connection();

		Connection(Connection &&con);

		bool
		is_valid() const;

		bool
		send_message(const Json &json) const;

		Json
		receive_message(int timeout_ms) const;

		bool
		ping() const;

		static bool
		message_is_ping(const Json &json);
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
		accept(int timeout_ms = TIMEOUT_INFINITE) const; // infinite by default
	};
}
