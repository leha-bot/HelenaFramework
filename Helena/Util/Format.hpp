#ifndef HELENA_UTIL_FORMAT_HPP
#define HELENA_UTIL_FORMAT_HPP

#include <Helena/Dependencies/Fmt.hpp>
#include <Helena/Engine/Log.hpp>

namespace Helena::Util
{
    template <typename... Args>
    [[nodiscard]] std::string Format(const std::string_view msg, Args&&... args) noexcept
    {
        try {
            if(!msg.empty()) {
                return fmt::vformat(msg, fmt::make_format_args(std::forward<Args>(args)...));
            }
        } catch(const fmt::format_error&) {
            Log::Console<Log::Exception>(
                "\n----------------------------------------\n"
                "|| Error: format syntax invalid!\n"
                "|| Format: {}"
                "\n----------------------------------------", msg);
        } catch(const std::bad_alloc&) {
            Log::Console<Log::Exception>(
                "\n----------------------------------------\n"
                "|| Error: not enough memory for alloc\n"
                "|| Format: {}"
                "\n----------------------------------------", msg);
        }

        return std::string{};
    }
}

#endif // HELENA_UTIL_FORMAT_HPP