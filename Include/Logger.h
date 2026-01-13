#pragma once

#include <memory>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Log
{
    inline void Init()
    {
        if (spdlog::get("Renderer"))
        {
            return;
        }

        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("renderer.log", true);
        std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};
        auto logger = std::make_shared<spdlog::logger>("Renderer", sinks.begin(), sinks.end());

        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        logger->set_level(spdlog::level::info);

        spdlog::set_default_logger(logger);
        spdlog::flush_on(spdlog::level::warn);
    }

    inline std::shared_ptr<spdlog::logger> Get()
    {
        auto logger = spdlog::get("Renderer");
        if (!logger)
        {
            Init();
            logger = spdlog::get("Renderer");
        }

        return logger;
    }
}