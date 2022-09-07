#include "sws/Message.h"
#include "sws/Client_Info.h"
#include "sws/Transaction.h"

namespace sws
{
	IMessage::IMessage(KIND _kind) : kind{_kind}
	{}

	Json
	IMessage::serialize()
	{
		return Json{
			{"kind", kind},
		};
	}

	void
	IMessage::deserialize(const Json &json)
	{
		kind = json["kind"];
	}

	IRequest::IRequest(IMessage::KIND _kind) : IMessage{_kind}
	{
	}

	std::unique_ptr<IRequest>
	IRequest::deserialize_base(const Json &json)
	{
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
	IResponse::serialize()
	{
		auto res = IMessage::serialize();
		res["error"] = error.msg;
		return res;
	}

	void
	IResponse::deserialize(const Json &json)
	{
		IMessage::deserialize(json);
		error.msg = json["error"];
	}

	std::unique_ptr<IResponse>
	IResponse::deserialize_base(const Json &json)
	{
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

		default:
			return nullptr;
		}

		res->deserialize(json);
		return res;
	}
}