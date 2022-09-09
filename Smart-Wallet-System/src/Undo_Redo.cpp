#include "sws/Undo_Redo.h"
#include "sws/Server.h"

namespace sws
{
	Request_Undo::Request_Undo()
		: IRequest{KIND_UNDO}
	{
	}

	std::unique_ptr<ICommand>
	Request_Undo::command()
	{
		return std::make_unique<Command_Undo>();
	}

	Response_Undo::Response_Undo()
		: IResponse{KIND_UNDO, Error{}}
	{
	}

	Response_Undo::Response_Undo(Error _error)
		: IResponse{KIND_UNDO, std::move(_error)}
	{
	}


	std::unique_ptr<IResponse>
	Command_Undo::execute(Server *server, cid_t client_id)
	{
		auto err = server->undo(client_id);
		return std::make_unique<Response_Undo>(err);
	}

	Request_Redo::Request_Redo()
		: IRequest{KIND_REDO}
	{
	}

	std::unique_ptr<ICommand>
	Request_Redo::command()
	{
		return std::make_unique<Command_Redo>();
	}

	Response_Redo::Response_Redo()
		: IResponse{KIND_REDO, Error{}}
	{
	}

	Response_Redo::Response_Redo(Error _error)
		: IResponse{KIND_REDO, std::move(_error)}
	{
	}

	std::unique_ptr<IResponse>
	Command_Redo::execute(Server *server, cid_t client_id)
	{
		auto err = server->redo(client_id);
		return std::make_unique<Response_Redo>(err);
	}
}