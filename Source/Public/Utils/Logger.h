#pragma once
#include <string>
#include <Windows.h>
#include <concepts>
#include <type_traits>

namespace Log
{
namespace details
{

template <typename T>
    requires std::is_convertible_v<T, std::string> && (!std::is_same_v<std::decay_t<T>, void*>) && (!std::is_same_v<std::decay_t<T>, std::nullptr_t>)
static std::string Logs(T&& arg)
{
    return std::string(std::forward<T>(arg));
}

static std::string Logs()
{
    return "";
}

template <typename T>
    requires std::is_same_v<std::decay_t<T>, std::nullptr_t>
static std::string Logs(T arg)
{
    return " Is Nullptr";
}

template <typename T>
    requires std::is_floating_point_v<T> || std::is_integral_v<T>
static std::string Logs(T arg)
{
    return " " + std::to_string(arg) + " ";
}

template <typename T>
    requires std::is_pointer_v<T> && (!std::is_same_v<T, const char*>)
static std::string Logs(T arg)
{
    if (arg)
    {
        std::string address = std::format(" Address 0x{:x}", reinterpret_cast<uintptr_t>(arg));
        return address;
    }
    else
    {
        return std::string(" is nullptr ");
    }
}

template <>
static std::string Logs<bool>(bool arg)
{
    return arg ? " TRUE " : " FALSE ";
}

static std::string Logs(const char* arg)
{
    if (!arg) return " Is nullptr";

    return std::string(arg);
}

template <>
static std::string Logs<const wchar_t*>(const wchar_t* arg)
{
    if (!arg) return " Is nullptr";

    int size = WideCharToMultiByte(CP_UTF8, 0, arg, -1, nullptr, 0, nullptr, nullptr);
    if (size == 0) return " Conversion error ";

    std::string result(size - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, arg, -1, &result[0], size, nullptr, nullptr);

    return result;
}

template <>
static std::string Logs<std::string&>(std::string& arg)
{
    return arg;
}

static std::string Logs(std::wstring& arg)
{
    if (arg.empty())
    {
        return std::string();
    }

    int size = WideCharToMultiByte(CP_UTF8, 0, arg.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (size == 0)
    {
        throw std::runtime_error("Failed to convert wstring to string");
    }

    std::string result(size - 1, '\0');  
    WideCharToMultiByte(CP_UTF8, 0, arg.c_str(), -1, &result[0], size, nullptr, nullptr);

    return result;
}

template <>
static std::string Logs<void*>(void* arg)
{
    return " IS VOID PTR ";
}

template <>
static std::string Logs<char>(char arg)
{
    return std::string(1, arg);
}

template <>
static std::string Logs<wchar_t>(wchar_t arg)
{
    return std::string(1, static_cast<char>(arg));
}

template <typename Key, typename Value>
static std::string Logs(const std::pair<const Key, Value>& arg)
{
    return Logs(arg.first) + ": " + Logs(arg.second);
}

// Contatiners concept
template <typename T>
concept Container = requires(T t) {
    t.begin();
    t.end();
    requires std::input_iterator<decltype(t.begin())>;
    requires !std::is_same_v<std::decay_t<T>, std::string>;
    requires !std::is_same_v<std::decay_t<T>, std::wstring>;
};

// containers handling
template <typename T>
    requires Container<T>
static std::string Logs(T& arg)
{
    std::string result = "[";
    for (auto it = arg.begin(); it != arg.end(); ++it)
    {
        result += Logs(*it);
        if (std::next(it) != arg.end()) result += ", ";
    }
    result += "]";
    return result;
}


template <typename... Types>
static std::string Logs(Types&&... args)
{
    return (Logs(std::forward<Types>(args)) + ...);
}

// Main functions
template <typename... Types>
static void LogMain(Types&&... args)
{
    std::string message;
    message.reserve(128);
    message = Logs(std::forward<Types>(args)...) + "\n";
    ::OutputDebugString(message.c_str());
}

}  // namespace details

#define LOG_WARNING(...) details::LogMain("warning: ", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) details::LogMain("error: ", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_MESSAGE(...) details::LogMain("information: ", __FILE__, __LINE__, __VA_ARGS__)

}  // namespace Log

using namespace Log;