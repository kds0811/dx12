#pragma once
#include <source_location>

namespace kds::app
{
    struct CheckerToken
    {
    };

    static constexpr CheckerToken Check;

    struct HrGrabber
    {
        HrGrabber(unsigned int hr, std::source_location loc = std::source_location::current()) noexcept;
        unsigned int Hr;
        std::source_location Loc;
    };

    void operator>>(HrGrabber g, CheckerToken);

}  // namespace kds::app