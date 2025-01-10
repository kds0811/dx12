#pragma once
#include <source_location>

namespace Kds::App
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

}  // namespace Kds::App