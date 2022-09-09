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
#include <future>
#include <type_traits>
#include <utility>

namespace sws
{
	using cid_t = int32_t;

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
		ILogger* logger;
		Log() = default;

		static Log*
		instance();

	public:
		static void
		set_default_logger();

		static void
		set_logger(ILogger *_logger);

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

	// Adapted from https://github.com/MoustaphaSaad/mn
	template <typename F>
	struct Defer
	{
		F f;
		Defer(F f) : f(f) {}
		~Defer() { f(); }
	};

	#define sws_DEFER_1(x, y) x##y
	#define sws_DEFER_2(x, y) sws_DEFER_1(x, y)
	#define sws_DEFER_3(x)    sws_DEFER_2(x, __COUNTER__)
	#define defer sws::Defer sws_DEFER_3(_defer_) = [&]()

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

	class Thread_With_Exit_Flag
	{
		std::shared_ptr<std::atomic_flag> should_exit;
		std::future<void> future;
	public:
		template<typename TCallable, typename... TArgs>
		explicit Thread_With_Exit_Flag(TCallable &&fn, TArgs &&...args)
			: should_exit{std::make_shared<std::atomic_flag>()},
			  future{std::async(std::forward<TCallable>(fn), std::forward<TArgs>(args)..., should_exit)}
		{
		}

		Thread_With_Exit_Flag(Thread_With_Exit_Flag &&) noexcept = default;

		void
		exit()
		{
			should_exit->test_and_set();
			if (future.valid())
				future.wait();
		}

		bool
		is_done()
		{
			auto status = future.wait_for(std::chrono::seconds{0});
			return status != std::future_status::timeout;
		}

		~Thread_With_Exit_Flag()
		{
			exit();
		}
	};
}

namespace fmt
{
	template<>
	struct formatter<sws::Error>
	{
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const sws::Error &err, FormatContext &ctx) {
			if (err.msg.empty())
				return ctx.out();
			return format_to(ctx.out(), "{}", err.msg);
		}
	};
}