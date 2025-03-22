#pragma once
#include <string>
#include <Windows.h>

namespace Log
{

    static void LogError(const std::string& str)
    {
        ::OutputDebugString(("error " + str + "\n").c_str());
    }
    static void LogWarning(const std::string& str)
    {
        ::OutputDebugString(("warning " + str + "\n").c_str());
    }
    static void LogMessage(const std::string& str)
    {
        ::OutputDebugString(("information " + str + "\n").c_str());
    }




}  // namespace Log