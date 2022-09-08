#pragma once
#include "sws/Message.h"
#include "sws/Command.h"

namespace sws
{
	class Request_Undo : public IRequest
	{
	public:
		Request_Undo();

		std::unique_ptr<ICommand>
		command(id_t client_id) override;
	};

	class Response_Undo : public IResponse
	{
	public:
		Response_Undo(); // uses deserialize
		explicit Response_Undo(Error _error);
	};

	class Command_Undo : public IMetaCommand
	{
	public:
		explicit Command_Undo(id_t client_id);

		std::unique_ptr<IResponse>
		execute(Server *server) override;

		std::string
		describe() override;
	};

	class Request_Redo : public IRequest
	{
	public:
		Request_Redo();

		std::unique_ptr<ICommand>
		command(id_t client_id) override;
	};

	class Response_Redo : public IResponse
	{
	public:
		Response_Redo(); // uses deserialize
		explicit Response_Redo(Error _error);
	};

	class Command_Redo : public IMetaCommand
	{
	public:
		explicit Command_Redo(id_t client_id);

		std::unique_ptr<IResponse>
		execute(Server *server) override;

		std::string
		describe() override;
	};
}