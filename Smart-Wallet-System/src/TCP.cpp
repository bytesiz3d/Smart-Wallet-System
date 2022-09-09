#include "sws/TCP.h"
#include "sws/Base.h"

#include <assert.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

namespace sws::tcp
{
	inline static sockaddr_in
	SERVER_SOCKET()
	{
		return sockaddr_in{
			.sin_family = AF_INET,
			.sin_port   = ::htons(8888),
			.sin_addr   = {.s_addr = ::inet_addr("127.0.0.1")}, // LOCALHOST
		};
	};

	Connection::Connection(int _handle)
		: handle{_handle}
	{
	}

	Connection::~Connection()
	{
		if (this->is_valid())
			::close(this->handle);
	}

	Connection::Connection(Connection &&con)
		: handle{con.handle}
	{
		con.handle = -1;
	}

	bool
	Connection::read_bytes(void *data, size_t size, int timeout_ms) const
	{
		assert(size != size_t(-1));
		pollfd pfd_read = {.fd = this->handle, .events = POLLIN};

		if (int ready = ::poll(&pfd_read, 1, timeout_ms); ready < 0)
			assert(false && "poll failed");
		else if (ready == 0)
			return false;

		int read_size = ::recv(this->handle, data, size, 0);
		return read_size == size;
	}

	bool
	Connection::write_bytes(void *data, size_t size) const
	{
		assert(size != size_t(-1));
		pollfd pfd_write = {.fd = this->handle, .events = POLLERR};

		if (int have_errors = ::poll(&pfd_write, 1, 30); have_errors < 0)
			assert(false && "poll failed");
		else if (have_errors > 0)
			return false;

		int write_size = ::send(this->handle, data, size, MSG_NOSIGNAL);
		if (errno == EPIPE)
			return false;

		return write_size == size;
	}

	bool
	Connection::is_valid() const
	{
		return this->handle >= 0;
	}

	bool
	Connection::send_message(const Json &json) const
	{
		std::string msg = json.dump();
		size_t size     = msg.length();

		if (write_bytes(&size, sizeof(size)) == false)
			return false;

		return write_bytes(msg.data(), size);
	}

	Json
	Connection::receive_message(int timeout_ms) const
	{
		size_t size = 0;
		if (bool ok = read_bytes(&size, sizeof(size), timeout_ms); ok == false)
			return Json{};

		std::string msg(size, '\0');
		if (bool ok = read_bytes(msg.data(), size, timeout_ms); ok == false)
			return Json{};

		return Json::parse(msg);
	}

	bool
	Connection::ping() const
	{
		return send_message({
			{"ping", true}
		});
	}

	bool
	Connection::message_is_ping(const Json &json)
	{
		return json.size() == 1 &&
			   json.contains("ping") &&
			   json["ping"].get<bool>();
	}

	Client::Client()
		: Connection(::socket(AF_INET, SOCK_STREAM, 0))
	{
		assert(this->handle >= 0);

		sockaddr_in server = SERVER_SOCKET();

		int connect_result = ::connect(this->handle, (sockaddr *) &server, sizeof(server));
		if (connect_result < 0)
		{ // create an invalid client
			::close(this->handle);
			this->handle = -1;
		}
	}

	Server::Server()
	{
		this->handle = ::socket(AF_INET, SOCK_STREAM, 0);
		assert(this->handle >= 0);

		sockaddr_in server = SERVER_SOCKET();

		int bind_result = ::bind(this->handle, (sockaddr *) &server, sizeof(server));
		assert(bind_result >= 0);

		int listen_result = ::listen(this->handle, MAX_CLIENTS);
		assert(listen_result >= 0);
	}

	Server::~Server()
	{
		::close(this->handle);
	}

	Connection
	Server::accept(int timeout_ms) const
	{
		pollfd pfd_accept = {.fd = this->handle, .events = POLLIN};
		if (int ready = ::poll(&pfd_accept, 1, timeout_ms); ready < 0)
			assert(false && "poll failed");
		else if (ready == 0)
			return Connection{-1};

		int client_handle = ::accept(this->handle, nullptr, nullptr);
		assert(client_handle >= 0);

		return Connection{client_handle};
	}
}
