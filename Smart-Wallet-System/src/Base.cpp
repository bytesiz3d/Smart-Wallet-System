#include "sws/Base.h"
#include <assert.h>
#include <stdio.h>

namespace sws
{
class Logger_Default : public ILogger
	{
		Logger_Default() = default;
	public:
		static ILogger* instance()
		{
			static Logger_Default LOG{};
			return &LOG;
		}

		inline void info(std::string_view message) override
		{
			printf("[INFO] %s\n", message.data());
		}

		inline void debug(std::string_view message) override
		{
			fprintf(stderr, "[DEBUG] %s\n", message.data());
		}

		inline void warning(std::string_view message) override
		{
			fprintf(stderr, "[WARN] %s\n", message.data());
		}

		inline void error(std::string_view message) override
		{
			fprintf(stderr, "[ERROR] %s\n", message.data());
		}

		inline void fatal(std::string_view message) override
		{
			fprintf(stderr, "[FATAL] %s\n", message.data());
			assert(false && "Fatal Error");
		}
	};

	Log *
	Log::instance()
	{
		static Log LOG{};
		return &LOG;
	}

	void
	Log::set_default_logger()
	{
		Log::instance()->logger = Logger_Default::instance();
	}

	void
	Log::set_logger(ILogger *_logger)
	{
		Log::instance()->logger = _logger;
	}

	void
	Log::info(std::string_view message)
	{
		Log::instance()->logger->info(message);
	}

	void
	Log::debug(std::string_view message)
	{
		Log::instance()->logger->debug(message);
	}

	void
	Log::warning(std::string_view message)
	{
		Log::instance()->logger->warning(message);
	}

	void
	Log::error(std::string_view message)
	{
		Log::instance()->logger->error(message);
	}

	void
	Log::fatal(std::string_view message)
	{
		Log::instance()->logger->fatal(message);
	}
}