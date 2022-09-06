#pragma once
#include <memory>

namespace sws
{
	class Server;
	class IRequest;

	class ICommand
	{
		id_t client_id;
	protected:
		explicit ICommand(id_t _client_id);

	public:
		virtual void
		execute(Server *) = 0;

		virtual void
		undo(Server *) = 0;

		virtual void
		redo(Server *) = 0;
	};
}