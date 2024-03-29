#pragma once
#include "sws/Base.h"
#include <memory>

using Json = nlohmann::json;

namespace sws
{
	class ICommand;

	class IMessage
	{
	protected:
		enum KIND : uint8_t
		{
			KIND_ID,
			KIND_UPDATE_INFO,
			KIND_DEPOSIT,
			KIND_WITHDRAWAL,
			KIND_QUERY_BALANCE,
			KIND_UNDO,
			KIND_REDO,
			KIND_TIMEOUT,
		};
		IMessage(KIND _kind);

	public:
		// Override only if additional data is added
		virtual Json
		serialize() const;

		// Override only if additional data is added
		virtual bool
		deserialize(const Json &json);

	private:
		KIND kind;
	};

	class IRequest : public IMessage
	{
	protected:
		IRequest(KIND _kind);

	public:
		static std::unique_ptr<IRequest>
		deserialize_base(const Json &json);

		// Force override
		virtual std::unique_ptr<ICommand>
		command() = 0;
	};

	class IResponse : public IMessage
	{
	protected:
		IResponse(KIND _kind, Error error);

	public:
		Error error;

		virtual Json
		serialize() const override; // add error

		virtual bool
		deserialize(const Json &json) override; // add error

		static std::unique_ptr<IResponse>
		deserialize_base(const Json &json);
	};

	class Response_Timeout : public IResponse
	{
	public:
		Response_Timeout();
	};
}