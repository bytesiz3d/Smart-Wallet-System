#pragma once
#include <memory>
#include <vector>

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

		virtual std::string
		describe() = 0;
	};

	class Command_Log
	{
		std::vector<std::unique_ptr<ICommand>> commands;
		size_t next_command;

	public:
		void
		execute_new_command(Server *server, std::unique_ptr<ICommand> &&command);

		void
		undo_prev_command(Server *server);

		void
		redo_next_command(Server *server);

		std::vector<std::string>
		describe_commands();
	};

}