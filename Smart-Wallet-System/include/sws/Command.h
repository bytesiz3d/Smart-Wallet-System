#pragma once
#include "sws/Base.h"

#include <memory>
#include <vector>

namespace sws
{
	class Server;
	class IResponse;

	class ICommand
	{
	protected:
		explicit ICommand(id_t _client_id);

		id_t client_id;

	public:
		virtual std::unique_ptr<IResponse>
		execute(Server *) = 0;

		virtual void
		undo(Server *) = 0;

		virtual void
		redo(Server *) = 0;

		virtual std::string
		describe() = 0;
	};

	class Command_Log
	{
		std::vector<std::unique_ptr<ICommand>> commands;
		size_t next_command;

	public:
		std::unique_ptr<IResponse>
		execute_new_command(Server *server, std::unique_ptr<ICommand> &&command);

		void
		undo_prev_command(Server *server);

		void
		redo_next_command(Server *server);

		std::vector<std::string>
		describe_commands();
	};

}