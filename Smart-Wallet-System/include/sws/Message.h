#pragma once
#include "sws/Base.h"
#include <memory>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

namespace sws
{
	class ICommand;

	class IMessage
	{
	protected:
		enum KIND : uint8_t
		{
			KIND_UPDATE_INFO,
			KIND_DEPOSIT,
			KIND_WITHDRAWAL,
			KIND_QUERY_BALANCE,
		};
		IMessage(KIND _kind);

	public:
		// Override only if additional data is added
		virtual Json
		serialize();

		// Override only if additional data is added
		virtual void
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
		command(id_t client_id) = 0;
	};

	class IResponse : public IMessage
	{
	protected:
		IResponse(KIND _kind, Error error);

	public:
		Error error;

		virtual Json
		serialize() override; // add error

		virtual void
		deserialize(const Json &json) override; // add error

		static std::unique_ptr<IResponse>
		deserialize_base(const Json &json);
	};
}