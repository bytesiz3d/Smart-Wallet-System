#include "sws/Command.h"
#include "sws/Base.h"
#include "assert.h"

namespace sws
{
	ICommand::ICommand(id_t _client_id)
		: client_id{_client_id}
	{
	}

	void
	Command_Log::execute_new_command(Server *server, std::unique_ptr<ICommand> &&command)
	{
		// Any command after next_command should be deleted
		commands.resize(next_command);

		// Execute command
		command->execute(server);

		commands.push_back(std::move(command));
		next_command++;
		assert(commands.size() == next_command);
	}

	void
	Command_Log::undo_prev_command(Server *server)
	{
		if (next_command == 0)
		{
			Log::debug("Undoing with no commands left");
			return;
		}

		commands[--next_command]->undo(server);
	}

	void
	Command_Log::redo_next_command(Server *server)
	{
		if (next_command == commands.size())
		{
			Log::debug("Redoing with no commands left");
			return;
		}

		commands[next_command++]->redo(server);
	}

	std::vector<std::string>
	Command_Log::describe_commands()
	{
		std::vector<std::string> descriptions(commands.size());
		for (size_t i = 0; i < descriptions.size(); i++)
			descriptions[i] = commands[i]->describe();

		return descriptions;
	}
}