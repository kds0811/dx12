#include "GraphicError.h"
#include <comdef.h>
#include <stdexcept>
#include <format>
#include <string>
#include "Logger.h"

namespace Kds::App
{
    HrGrabber::HrGrabber(unsigned int hr, std::source_location loc) noexcept : Hr(hr), Loc(loc) {}

    void operator>>(HrGrabber g, CheckerToken)
    {
        if (FAILED(g.Hr))
        {
            _com_error Error(g.Hr);
            auto message = std::format("Graphics error: {}\n \n {}({})", Error.ErrorMessage(), g.Loc.file_name(), g.Loc.line());
            Log::LogError(message);
            throw std::runtime_error{message};
        }
    }
}  // namespace kds::app 
