#include "sws/Undo_Redo.h"
#include "sws/Server.h"

namespace sws
{
	Request_Undo::Request_Undo()
		: IRequest{KIND_UNDO}
	{
	}

	std::unique_ptr<ICommand>
	Request_Undo::command(cid_t client_id)
	{
		return std::make_unique<Command_Undo>(client_id);
	}

	Response_Undo::Response_Undo()
		: IResponse{KIND_UNDO, Error{}}
	{
	}

	Response_Undo::Response_Undo(Error _error)
		: IResponse{KIND_UNDO, std::move(_error)}
	{
	}

	Command_Undo::Command_Undo(cid_t client_id)
		: IMetaCommand(client_id)
	{
	}

	std::unique_ptr<IResponse>
	Command_Undo::execute(Server *server)
	{
		auto err = server->undo(client_id);
		return std::make_unique<Response_Undo>(err);
	}

	std::string
	Command_Undo::describe()
	{
		return "Undo";
	}

	Request_Redo::Request_Redo()
		: IRequest{KIND_REDO}
	{
	}

	std::unique_ptr<ICommand>
	Request_Redo::command(cid_t client_id)
	{
		return std::make_unique<Command_Redo>(client_id);
	}

	Response_Redo::Response_Redo()
		: IResponse{KIND_REDO, Error{}}
	{
	}

	Response_Redo::Response_Redo(Error _error)
		: IResponse{KIND_REDO, std::move(_error)}
	{
	}

	Command_Redo::Command_Redo(cid_t client_id)
		: IMetaCommand(client_id)
	{
	}

	std::unique_ptr<IResponse>
	Command_Redo::execute(Server *server)
	{
		auto err = server->redo(client_id);
		return std::make_unique<Response_Redo>(err);
	}

	std::string
	Command_Redo::describe()
	{
		return "Redo";
	}
}