#include "sws/TCP.h"

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

namespace sws::tcp
{
	static const sockaddr_in SERVER_SOCKET = {
		.sin_family = AF_INET,
		.sin_port   = ::htons(8888),
		.sin_addr   = {.s_addr = ::inet_addr("127.0.0.1")},
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
	Connection::is_valid() const
	{
		return this->handle >= 0;
	}

	void
	Connection::send_message(Mem_View bytes) const
	{
		size_t size = bytes.size;

		int sent = ::send(this->handle, &size, sizeof(size), 0);
		assert(sent == sizeof(size));

		sent = ::send(this->handle, bytes.data, size, 0);
		assert(sent == size);
	}

	Mem_Block
	Connection::receive_message() const
	{
		size_t size = 0;
		int read    = ::recv(this->handle, &size, sizeof(size), 0);
		if (read == 0)
			return {};

		assert(read == sizeof(size));

		Mem_Block bytes(size);
		read = ::recv(this->handle, bytes.data, size, 0);
		assert(read == size);

		return bytes;
	}

	Client::Client()
		: Connection(::socket(AF_INET, SOCK_STREAM, 0))
	{
		assert(this->handle >= 0);

		sockaddr_in server = SERVER_SOCKET;

		int connect_result = ::connect(this->handle, (sockaddr *) &server, sizeof(server));
		assert(connect_result >= 0);
	}

	Server::Server()
	{
		this->handle = ::socket(AF_INET, SOCK_STREAM, 0);
		assert(this->handle >= 0);

		sockaddr_in server = SERVER_SOCKET;

		int bind_result = ::bind(this->handle, (sockaddr *) &server, sizeof(server));
		assert(bind_result >= 0);

		int listen_result = ::listen(this->handle, MAX_CLIENTS);
		assert(listen_result >= 0);
	}

	Server::~Server()
	{
		printf("%d DYING\n", this->handle);
		::close(this->handle);
	}

	Connection
	Server::accept() const
	{
		sockaddr_in client  = {};
		socklen_t client_sz = sizeof(client);
		int client_desc     = ::accept(this->handle, (sockaddr *) &client, &client_sz);
		assert(client_desc >= 0);

		return Connection(client_desc);
	}
}
