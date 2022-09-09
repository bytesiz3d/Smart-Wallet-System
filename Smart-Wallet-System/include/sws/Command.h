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
		ICommand() = default;

	public:
		virtual std::unique_ptr<IResponse>
		execute(Server *, cid_t) = 0;

		virtual Error
		undo(Server *, cid_t) = 0;

		virtual Error
		redo(Server *, cid_t) = 0;

		virtual Json
		serialize() = 0;

		virtual bool
		deserialize(const Json &json) = 0;

		static std::unique_ptr<ICommand>
		deserialize_base(const Json &json);

		virtual bool
		is_meta() const;
	};

	class IMetaCommand : public ICommand
	{
	protected:
		IMetaCommand() = default;

	public:
		Error
		undo(Server *, cid_t) final;

		Error
		redo(Server *, cid_t) final;

		Json
		serialize() final;

		bool
		deserialize(const Json &json) final;

		bool
		is_meta() const override final;
	};

	class Command_Log
	{
		std::vector<std::unique_ptr<ICommand>> commands;
		size_t next_command;

	public:
		std::unique_ptr<IResponse>
		execute_new_command(Server *server, cid_t client_id, std::unique_ptr<ICommand> &&command);

		Error
		undo_prev_command(Server *server, cid_t client_id);

		Error
		redo_next_command(Server *server, cid_t client_id);

		std::vector<std::string>
		describe_commands();

		Json
		serialize() const;

		void
		deserialize(const Json &json);
	};

}