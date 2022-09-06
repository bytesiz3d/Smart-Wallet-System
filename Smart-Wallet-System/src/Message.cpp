#include "sws/Message.h"
#include "sws/Client_Info.h"
#include "sws/Serializer.h"
#include "sws/Transaction.h"

namespace sws
{
	IMessage::IMessage(KIND _kind, id_t _client_id) : kind{_kind}, client_id{_client_id}
	{}

	void
	IMessage::serialize(Serializer &serializer)
	{
		serializer.push(kind);
		serializer.push(client_id);
	}

	IRequest::IRequest(IMessage::KIND _kind, id_t _client_id) : IMessage(_kind, _client_id)
	{
	}

	std::unique_ptr<IRequest>
	IRequest::deserialize_base(Deserializer &deserializer)
	{
		auto kind = deserializer.read<KIND>();
		auto client_id = deserializer.read<id_t>();

		std::unique_ptr<IRequest> req{};

		switch (kind)
		{
		case KIND_UPDATE_INFO:
			req = std::make_unique<Request_Update_Info>(client_id, Client_Info{});
			break;

		case KIND_DEPOSIT:
			req = std::make_unique<Request_Deposit>(client_id, Tx_Deposit{0});
			break;

		case KIND_WITHDRAWAL:
			req = std::make_unique<Request_Withdrawal>(client_id, Tx_Withdrawal{0});
			break;

		default:
			return nullptr;
		}

		req->deserialize(deserializer);
		return req;
	}

	void
	IRequest::deserialize(Deserializer &deserializer)
	{
	}

	IResponse::IResponse(IMessage::KIND _kind, id_t _client_id, Error _error)
		: IMessage{_kind, _client_id}, error{std::move(_error)}
	{
	}

	void
	IResponse::serialize(Serializer &serializer)
	{
		IMessage::serialize(serializer);
		serializer.push(error.msg);
	}

	std::unique_ptr<IResponse>
	IResponse::deserialize_base(Deserializer &deserializer)
	{
		auto kind = deserializer.read<KIND>();
		auto client_id = deserializer.read<id_t>();
		auto error_msg = deserializer.read<std::string>();

		std::unique_ptr<IResponse> res;

		switch (kind)
		{
		case KIND_UPDATE_INFO:
			res = std::make_unique<Response_Update_Info>(client_id, Error{error_msg});
			break;

		case KIND_DEPOSIT:
			res = std::make_unique<Response_Deposit>(client_id, Error{error_msg});
			break;

		case KIND_WITHDRAWAL:
			res = std::make_unique<Response_Withdrawal>(client_id, Error{error_msg});
			break;

		default:
			return nullptr;
		}

		res->deserialize(deserializer);
		return res;
	}

	void
	IResponse::deserialize(Deserializer &deserializer)
	{
	}
}