#pragma once

#include "sws/Message.h"
#include "sws/Command.h"

namespace sws
{
	class Request_ID : public IRequest
	{
		cid_t id_to_set;
	public:
		Request_ID(); // uses deserialize for initialization
		Request_ID(cid_t _id_to_set);

		Json
		serialize() const override;

		void
		deserialize(const Json &json) override;

		std::unique_ptr<ICommand>
		command(cid_t client_id) override;

		cid_t
		get_id() const;
	};

	class Response_ID : public IResponse
	{
		cid_t id_set;
	public:
		Response_ID(); // uses deserialize
		Response_ID(Error _error, cid_t _id_set);

		Json
		serialize() const override;

		void
		deserialize(const Json &json) override;

		cid_t
		get_id() const;
	};

	class Command_ID : public IMetaCommand
	{
		cid_t id_to_set;
	public:
		explicit Command_ID(cid_t client_id, cid_t _id_to_set);

		std::unique_ptr<IResponse>
		execute(Server *server) override;

		std::string
		describe() override;
	};
}