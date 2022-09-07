#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <string>
#include <string_view>
#include <type_traits>
#include <thread>
#include <memory>
#include <queue>

namespace sws
{
	using id_t = uint32_t;

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

	template <typename T>
	class Thread_Safe_Queue
	{
		std::mutex mtx;
		std::queue<std::unique_ptr<T>> q;
	public:
		inline bool
		empty()
		{
			mtx.lock();
			bool empty = q.empty();
			mtx.unlock();
			return empty;
		}

		inline void
		push(std::unique_ptr<T> &&val)
		{
			mtx.lock();
			q.push(std::move(val));
			mtx.unlock();
		}

		inline std::unique_ptr<T>
		pop()
		{
			mtx.lock();
			auto front = std::move(q.front());
			q.pop();
			mtx.unlock();
			return front;
		}
	};
}