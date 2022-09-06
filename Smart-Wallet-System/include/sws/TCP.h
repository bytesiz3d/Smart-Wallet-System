#pragma once
#include "sws/Base.h"

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
		send_message(Mem_View bytes) const;

		Mem_Block
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
