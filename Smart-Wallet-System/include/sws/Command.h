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
		explicit ICommand(cid_t _client_id);

		cid_t client_id;

	public:
		virtual std::unique_ptr<IResponse>
		execute(Server *) = 0;

		virtual Error
		undo(Server *) = 0;

		virtual Error
		redo(Server *) = 0;

		virtual std::string
		describe() = 0;

		virtual bool
		is_meta() const;
	};

	class IMetaCommand : public ICommand
	{
	protected:
		explicit IMetaCommand(cid_t _client_id);

	public:
		virtual Error
		undo(Server *) override final;

		virtual Error
		redo(Server *) override final;

		bool
		is_meta() const override final;
	};

	class Command_Log
	{
		std::vector<std::unique_ptr<ICommand>> commands;
		size_t next_command;

	public:
		std::unique_ptr<IResponse>
		execute_new_command(Server *server, std::unique_ptr<ICommand> &&command);

		Error
		undo_prev_command(Server *server);

		Error
		redo_next_command(Server *server);

		std::vector<std::string>
		describe_commands();
	};

}