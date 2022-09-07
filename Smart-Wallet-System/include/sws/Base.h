#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <fmt/format.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <thread>
#include <memory>
#include <queue>
#include <utility>

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

		Error() = default;

		template<typename... TArgs>
		explicit Error(fmt::format_string<TArgs...> fmt, TArgs &&...args)
			: msg(fmt::format(fmt, std::forward<TArgs>(args)...))
		{}

		explicit operator bool() const { return msg.empty() == false; }
		bool operator==(bool v) const { return this->operator bool() == v; }
		bool operator!=(bool v) const { return !operator==(v); }
	};

	template<typename T>
	struct Result
	{
		T val;
		Error err;

		Result(Error e)
			: err(std::move(e))
		{
		}

		// creates a result instance from a value
		template<typename... TArgs>
		Result(TArgs &&...args)
			: val(std::forward<TArgs>(args)...), err{}
		{
		}

		Result(const Result&) = delete;
		Result(Result&&) noexcept = default;
		Result& operator=(const Result&) = delete;
		Result& operator=(Result&&) noexcept = default;
		~Result() = default;
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