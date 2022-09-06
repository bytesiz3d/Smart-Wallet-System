#pragma once
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <vector>

#include "sws/Base.h"

namespace sws
{
	class Serializer
	{
		std::vector<uint8_t> bytes;

	public:
		template <typename T>
		inline void
		push(const T &value)
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				size_t length = value.length();
				push(length);
				bytes.insert(bytes.end(), value.begin(), value.end());
			}
			else
			{
				bytes.insert(bytes.end(), &value, &value + sizeof(T));
			}
		}

		inline Mem_Block
		finish()
		{
			if (bytes.empty())
				return Mem_Block{};

			Mem_Block block{bytes.size()};
			::memcpy(block.data, bytes.data(), bytes.size());
			return block;
		}
	};

	class Deserializer
	{
		Mem_View bytes;
		size_t offset;

	public:
		Deserializer(Mem_View _bytes) : bytes(_bytes)
		{
			offset = 0;
		}

		template <typename T>
		inline T
		read()
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				size_t length = read<size_t>();

				auto data = reinterpret_cast<const char*>(bytes.data);
				std::string str(&data[offset], &data[offset + length]);

				offset += length;
				assert(offset < bytes.size);

				return str;
			}
			else
			{
				T value = *reinterpret_cast<T*>(bytes.data);
				offset += sizeof(T);
				assert(offset < bytes.size);

				return value;
			}
		}

		inline bool
		is_done() const
		{
			assert(offset < bytes.size);
			return offset == bytes.size;
		}
	};
}