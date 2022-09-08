#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <fmt/format.h>
#include <memory>
#include <queue>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <utility>

namespace sws
{
	using id_t = uint32_t;

	class ILogger
	{
	public:
		virtual void
		info(std::string_view message) = 0;
		virtual void
		debug(std::string_view message) = 0;
		virtual void
		warning(std::string_view message) = 0;
		virtual void
		error(std::string_view message) = 0;
		virtual void
		fatal(std::string_view message) = 0;
	};

	class Log
	{
		std::shared_ptr<ILogger> logger;
		Log() = default;

		static std::shared_ptr<Log>
		instance();

	public:
		static void
		set_default_logger();

		static void
		set_logger(std::shared_ptr<ILogger> _logger);

		template<typename... TArgs>
		static void
		info(fmt::format_string<TArgs...> fmt, TArgs &&...args)
		{
			Log::instance()->logger->info(fmt::format(fmt, std::forward<TArgs>(args)...));
		}

		template<typename... TArgs>
		static void
		debug(fmt::format_string<TArgs...> fmt, TArgs &&...args)
		{
			Log::instance()->logger->debug(fmt::format(fmt, std::forward<TArgs>(args)...));
		}

		template<typename... TArgs>
		static void
		warning(fmt::format_string<TArgs...> fmt, TArgs &&...args)
		{
			Log::instance()->logger->warning(fmt::format(fmt, std::forward<TArgs>(args)...));
		}

		template<typename... TArgs>
		static void
		error(fmt::format_string<TArgs...> fmt, TArgs &&...args)
		{
			Log::instance()->logger->error(fmt::format(fmt, std::forward<TArgs>(args)...));
		}

		template<typename... TArgs>
		static void
		fatal(fmt::format_string<TArgs...> fmt, TArgs &&...args)
		{
			Log::instance()->logger->fatal(fmt::format(fmt, std::forward<TArgs>(args)...));
		}
	};

	class Error
	{
	public:
		std::string msg;
		Error() = default;

		template<typename... TArgs>
		explicit Error(fmt::format_string<TArgs...> fmt, TArgs &&...args)
			: msg(fmt::format(fmt, std::forward<TArgs>(args)...))
		{
		}

		explicit operator bool() const
		{
			return msg.empty() == false;
		}
		bool
		operator==(bool v) const
		{
			return this->operator bool() == v;
		}
		bool
		operator!=(bool v) const
		{
			return !operator==(v);
		}
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

		Result(const Result &)     = delete;
		Result(Result &&) noexcept = default;
		Result &
		operator=(const Result &) = delete;
		Result &
		operator=(Result &&) noexcept = default;
		~Result()                     = default;
	};

	template<typename T>
	class Thread_Safe_Queue
	{
		std::mutex mtx;
		std::queue<T> q;

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
		push(T &&val)
		{
			mtx.lock();
			q.push(std::move(val));
			mtx.unlock();
		}

		inline T
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