#include "sws/Client_ID.h"
#include "sws/Server.h"

namespace sws
{
	Request_ID::Request_ID()
		: IRequest{KIND_ID}, id_to_set{-1}
	{
	}

	Request_ID::Request_ID(cid_t _id_to_set)
		: IRequest{KIND_ID}, id_to_set{_id_to_set}
	{
	}

	Json
	Request_ID::serialize() const
	{
		auto req       = IRequest::serialize();
		req["request"] = {
			{"id_to_set", id_to_set}
		};
		return req;
	}

	bool
	Request_ID::deserialize(const Json &json)
	{
		IMessage::deserialize(json);
		id_to_set = json["request"]["id_to_set"];
		return true;
	}

	std::unique_ptr<ICommand>
	Request_ID::command()
	{
		// Dummy id
		return std::make_unique<Command_ID>(id_to_set);
	}

	cid_t
	Request_ID::get_id() const
	{
		return id_to_set;
	}

	Response_ID::Response_ID()
		: IResponse{KIND_ID, {}}, id_set{}
	{
	}

	Response_ID::Response_ID(Error _error, cid_t _id_set)
		: IResponse{KIND_ID, std::move(_error)}, id_set{_id_set}
	{
	}

	Json
	Response_ID::serialize() const
	{
		auto res        = IResponse::serialize();
		res["response"] = {
			{"id_to_set", id_set}};
		return res;
	}

	bool
	Response_ID::deserialize(const Json &json)
	{
		IResponse::deserialize(json);
		id_set = json["response"]["id_to_set"];
		return true;
	}

	cid_t
	Response_ID::get_id() const
	{
		return id_set;
	}

	Command_ID::Command_ID(cid_t _id_to_set)
		: id_to_set{_id_to_set} // dummy ID
	{
	}

	std::unique_ptr<IResponse>
	Command_ID::execute(Server *server, cid_t)
	{
		auto [_id, err] = server->new_session_with_id(id_to_set);
		return std::make_unique<Response_ID>(err, _id);
	}
}