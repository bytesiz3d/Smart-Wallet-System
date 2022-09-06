#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <string>
#include <string_view>
#include <type_traits>

namespace sws
{
	using id_t = uint32_t;
	struct Mem
	{
		void *data;
		size_t size;
	};

	class Mem_Block : public Mem
	{
	public:
		explicit Mem_Block(size_t _size = 0);

		~Mem_Block();

		Mem_Block(Mem_Block &&other) noexcept;
	};

	class Mem_View : public Mem
	{
	public:
		template<typename T>
		inline Mem_View(T &value)
		{
			if constexpr (std::is_same_v<T, Mem_Block>)
			{
				this->data = value.data;
				this->size = value.size;
			}
			else
			{
				this->data = &value;
				this->size = sizeof(value);
			}
		}

		inline Mem_View(void *_data, size_t _size) : Mem{_data, _size}
		{
		}
	};

	class ILogger
	{
	public:
		virtual void info(std::string_view message) = 0;
		virtual void debug(std::string_view message) = 0;
		virtual void warning(std::string_view message) = 0;
		virtual void error(std::string_view message) = 0;
		virtual void fatal(std::string_view message) = 0;
	};

	class Log
	{
		ILogger *logger;
		Log() = default;
		static Log *instance();
	public:
		static void set_default_logger();
		static void set_logger(ILogger *_logger);

		static void info(std::string_view message);
		static void debug(std::string_view message);
		static void warning(std::string_view message);
		static void error(std::string_view message);
		static void fatal(std::string_view message);
	};

	class Error
	{
	public:
		std::string msg;
		inline operator bool() const
		{
			return msg.empty() == false;
		}
	};
}