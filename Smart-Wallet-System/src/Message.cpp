#include "sws/Message.h"
#include "sws/Client_Info.h"
#include "sws/Transaction.h"
#include "sws/Undo_Redo.h"
#include "sws/Client_ID.h"

namespace sws
{
	IMessage::IMessage(KIND _kind) : kind{_kind}
	{}

	Json
	IMessage::serialize() const
	{
		return Json{
			{"kind", kind},
		};
	}

	bool
	IMessage::deserialize(const Json &json)
	{
		kind = json["kind"];
		return true;
	}

	IRequest::IRequest(IMessage::KIND _kind) : IMessage{_kind}
	{
	}

	std::unique_ptr<IRequest>
	IRequest::deserialize_base(const Json &json)
	{
		if (json.contains("kind") == false)
			return nullptr;

		KIND kind = json["kind"];
		std::unique_ptr<IRequest> req{};

		switch (kind)
		{
		case KIND_UPDATE_INFO:
			req = std::make_unique<Request_Update_Info>();
			break;

		case KIND_DEPOSIT:
			req = std::make_unique<Request_Deposit>();
			break;

		case KIND_WITHDRAWAL:
			req = std::make_unique<Request_Withdrawal>();
			break;

		case KIND_QUERY_BALANCE:
			req = std::make_unique<Request_Query_Balance>();
			break;

		case KIND_UNDO:
			req = std::make_unique<Request_Undo>();
			break;

		case KIND_REDO:
			req = std::make_unique<Request_Redo>();
			break;

		case KIND_ID:
			req = std::make_unique<Request_ID>();
			break;

		default:
			return nullptr;
		}

		req->deserialize(json);
		return req;
	}

	IResponse::IResponse(IMessage::KIND _kind, Error _error)
		: IMessage{_kind}, error{std::move(_error)}
	{
	}

	Json
	IResponse::serialize() const
	{
		auto res = IMessage::serialize();
		res["error"] = error.msg;
		return res;
	}

	bool
	IResponse::deserialize(const Json &json)
	{
		IMessage::deserialize(json);
		error.msg = json["error"];
		return true;
	}

	std::unique_ptr<IResponse>
	IResponse::deserialize_base(const Json &json)
	{
		if (json.contains("kind") == false)
			return nullptr;

		KIND kind = json["kind"];
		std::unique_ptr<IResponse> res{};

		switch (kind)
		{
		case KIND_UPDATE_INFO:
			res = std::make_unique<Response_Update_Info>();
			break;

		case KIND_DEPOSIT:
			res = std::make_unique<Response_Deposit>();
			break;

		case KIND_WITHDRAWAL:
			res = std::make_unique<Response_Withdrawal>();
			break;

		case KIND_QUERY_BALANCE:
			res = std::make_unique<Response_Query_Balance>();
			break;

		case KIND_UNDO:
			res = std::make_unique<Response_Undo>();
			break;

		case KIND_REDO:
			res = std::make_unique<Response_Redo>();
			break;

		case KIND_ID:
			res = std::make_unique<Response_ID>();
			break;

		default:
			return nullptr;
		}

		res->deserialize(json);
		return res;
	}

	Response_Timeout::Response_Timeout()
		: IResponse{KIND_TIMEOUT, Error{"Server Timeout"}}
	{
	}
}