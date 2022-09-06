#pragma once
#include "sws/Base.h"
#include <memory>

namespace sws
{
	class ICommand;
	class Serializer;
	class Deserializer;

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
		IMessage(KIND _kind, id_t _client_id);
		const id_t client_id;

	public:
		virtual void
		serialize(Serializer &serializer);
		virtual void
		deserialize(Deserializer &deserializer) = 0;

	private:
		const KIND kind;
	};

	class IRequest : public IMessage
	{
	protected:
		IRequest(KIND _kind, id_t _client_id);

	public:
		static std::unique_ptr<IRequest>
		deserialize_base(Deserializer &deserializer);

		virtual void
		deserialize(Deserializer &deserializer) override;

		virtual std::unique_ptr<ICommand>
		command() = 0;
	};

	class IResponse : public IMessage
	{
	protected:
		IResponse(KIND _kind, id_t _client_id, Error error);

	public:
		Error error;

		static std::unique_ptr<IResponse>
		deserialize_base(Deserializer &deserializer);

		virtual void
		serialize(Serializer &serializer) override;

		virtual void
		deserialize(Deserializer &deserializer) override;
	};
}