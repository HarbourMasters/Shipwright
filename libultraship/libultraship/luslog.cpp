#include "spdlog/spdlog.h"
#include <string>
#include "luslog.h"

extern "C" {
    void luslog(const char* file, int32_t line, int32_t logLevel, const char* msg) {
        std::string str(msg);
        spdlog::level::level_enum lvl = (spdlog::level::level_enum)logLevel;
        auto loc = spdlog::source_loc{ file, line, SPDLOG_FUNCTION };

        spdlog::default_logger_raw()->log(loc, lvl, str);
    }

    void lusprintf(const char* file, int32_t line, int32_t logLevel, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
    	char buffer[4096];

        vsnprintf(buffer, sizeof(buffer), fmt, args);
        luslog(file, line, logLevel, buffer);
    }
}