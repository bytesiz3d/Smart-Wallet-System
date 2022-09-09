#include "sws/Command.h"
#include "sws/Message.h"
#include "sws/Base.h"
#include "assert.h"

namespace sws
{
	ICommand::ICommand(cid_t _client_id)
		: client_id{_client_id}
	{
	}

	bool
	ICommand::is_meta() const
	{
		return false;
	}

	IMetaCommand::IMetaCommand(cid_t _client_id)
		: ICommand{_client_id}
	{
	}

	Error
	IMetaCommand::undo(Server *)
	{ // do nothing
		 return Error{};
	}

	Error
	IMetaCommand::redo(Server *)
	{ // do nothing
		return Error{};
	}

	bool
	IMetaCommand::is_meta() const
	{
		return true;
	}

	std::unique_ptr<IResponse>
	Command_Log::execute_new_command(Server *server, std::unique_ptr<ICommand> &&command)
	{
		// Execute command
		auto res = command->execute(server);

		if (res->error == false && command->is_meta() == false)
		{
			// Any command after next_command should be deleted
			commands.resize(next_command);
			commands.push_back(std::move(command));
			next_command++;
			assert(commands.size() == next_command);
		}

		return res;
	}

	Error
	Command_Log::undo_prev_command(Server *server)
	{
		if (next_command == 0)
			return Error{"Undoing with no commands left"};

		return commands[--next_command]->undo(server);
	}

	Error
	Command_Log::redo_next_command(Server *server)
	{
		if (next_command == commands.size())
			return Error{"Redoing with no commands left"};

		return commands[next_command++]->redo(server);
	}

	std::vector<std::string>
	Command_Log::describe_commands()
	{
		std::vector<std::string> descriptions(commands.size());
		for (size_t i = 0; i < descriptions.size(); i++)
		{
			descriptions[i] = commands[i]->describe();
			if (i == next_command-1)
				descriptions[i] += " <- CURRENT COMMAND";
		}

		return descriptions;
	}
}