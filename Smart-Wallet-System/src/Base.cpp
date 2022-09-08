#include "sws/Base.h"
#include <assert.h>
#include <stdio.h>

#include <utility>

namespace sws
{
class Logger_Default : public ILogger
	{
		Logger_Default() = default;
	public:
		static ILogger* instance()
		{
			static thread_local std::unique_ptr<Logger_Default> LOG{new Logger_Default};
			return LOG.get();
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

	Log*
	Log::instance()
	{
		static thread_local Log LOG{};
		if (LOG.logger == nullptr)
			LOG.logger = Logger_Default::instance();
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
}