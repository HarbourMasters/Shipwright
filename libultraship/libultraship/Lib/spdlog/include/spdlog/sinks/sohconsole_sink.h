// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <spdlog/details/fmt_helper.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/details/os.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/synchronous_factory.h>
#include "SohImGuiImpl.h"
#include "GameSettings.h"
#include "Cvar.h"
#include <chrono>
#include <mutex>
#include <string>
#include <thread>

namespace spdlog {
namespace sinks {

/*
 * Android sink (logging using __android_log_write)
 */
template<typename Mutex>
class sohconsole_sink final : public base_sink<Mutex>
{
public:
    explicit sohconsole_sink(std::string tag = "spdlog", bool use_raw_msg = false)
        : tag_(std::move(tag))
        , use_raw_msg_(use_raw_msg)
    {}

protected:
    void sink_it_(const details::log_msg &msg) override
    {
        const Priority priority = convert_to_soh(msg.level);
        memory_buf_t formatted;
        if (use_raw_msg_)
        {
            details::fmt_helper::append_string_view(msg.payload, formatted);
        }
        else
        {
            base_sink<Mutex>::formatter_->format(msg, formatted);
        }
        formatted.push_back('\0');
        const char *msg_output = formatted.data();
        if (CVar_GetS32("gSinkEnabled", 0) && SohImGui::console->opened)
            SohImGui::console->Append("SoH Logging", priority, "%s", msg_output);
    }

    void flush_() override {}

private:
    static Priority convert_to_soh(spdlog::level::level_enum level) {
        switch (level) {
            case spdlog::level::trace:
                return Priority::INFO_LVL;
            case spdlog::level::debug:
                return Priority::LOG_LVL;
            case spdlog::level::info:
                return Priority::LOG_LVL;
            case spdlog::level::warn:
                return Priority::WARNING_LVL;
            case spdlog::level::err:
                return Priority::ERROR_LVL;
            case spdlog::level::critical:
                return Priority::ERROR_LVL;
            default:
                break;
        }
        return Priority::LOG_LVL;
    }

    std::string tag_;
    bool use_raw_msg_;
};

using soh_sink_mt = sohconsole_sink<std::mutex>;
using soh_sink_st = sohconsole_sink<details::null_mutex>;
} // namespace sinks
} // namespace spdlog