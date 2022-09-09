#include "sws/Command.h"
#include "sws/Message.h"
#include "sws/Base.h"

#include "sws/Transaction.h"
#include "sws/Client_ID.h"
#include "sws/Client_Info.h"
#include "sws/Undo_Redo.h"

#include "assert.h"

namespace sws
{
	ICommand::ICommand(cid_t _client_id)
		: client_id{_client_id}
	{
	}

	std::unique_ptr<ICommand>
	ICommand::deserialize_base(const Json &json)
	{
		std::unique_ptr<ICommand> com{nullptr};

		if (Command_Deposit{}.deserialize(json))
			com = std::make_unique<Command_Deposit>();

		else if (Command_Withdrawal{}.deserialize(json))
			com = std::make_unique<Command_Withdrawal>();

		else if (Command_Query_Balance{}.deserialize(json))
			com = std::make_unique<Command_Query_Balance>();

		else if (Command_Update_Info{}.deserialize(json))
			com = std::make_unique<Command_Update_Info>();

		else if (Command_Undo{}.deserialize(json))
			com = std::make_unique<Command_Undo>();

		else if (Command_Redo{}.deserialize(json))
			com = std::make_unique<Command_Redo>();

		else if (Command_ID{}.deserialize(json))
			com = std::make_unique<Command_ID>();

		if (com != nullptr)
			com->deserialize(json);

		return com;
	}

	bool
	ICommand::is_meta() const
	{
		return false;
	}

	IMetaCommand::IMetaCommand()
		: ICommand{}
	{
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

	Json
	IMetaCommand::serialize()
	{
		return Json{{"meta", true}};
	}

	bool
	IMetaCommand::deserialize(const Json &json)
	{
		return json.contains("meta") && json["meta"].get<bool>();
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
			descriptions[i] = commands[i]->serialize().dump();
			if (i == next_command-1)
				descriptions[i] += " <- CURRENT COMMAND";
		}

		return descriptions;
	}

	void
	Command_Log::load_commands(const std::vector<Json> &_commands)
	{
		for (auto &j : _commands)
		{
			auto com = ICommand::deserialize_base(j);
			commands.push_back(std::move(com));
		}
		next_command = commands.size();
	}
}