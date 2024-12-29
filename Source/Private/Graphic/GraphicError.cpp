#include "GraphicError.h"
#include <comdef.h>
#include <stdexcept>
#include <format>
#include <string>

namespace kds::app
{

    HrGrabber::HrGrabber(unsigned int hr, std::source_location loc) noexcept : Hr(hr), Loc(loc) {}

    

    void operator>>(HrGrabber g, CheckerToken)
    {
        if (FAILED(g.Hr))
        {
            _com_error Error(g.Hr);
            throw std::runtime_error{std::format("Graphics error: {}\n \n {}({})", Error.ErrorMessage(), g.Loc.file_name(), g.Loc.line())};
        }
    }
}  // namespace kds::app