#pragma once
#include <functional>
#include <unordered_map>

/// \brief Base interface for all delegates.
class IDelegate
{
public:
    IDelegate() = default;
    virtual ~IDelegate() = default;
};

/// \brief Manages a collection of callback functions for a specific event signature.
///        Supports both free functions and member functions as subscribers.
template <typename... CallBackArgs>
class Delegate : public IDelegate
{
public:
    using CallBackFunction = std::function<void(CallBackArgs...)>;
    using CallBackMap = std::unordered_map<size_t, CallBackFunction>;

private:
    CallBackMap mCallBackMap{};
    size_t mID = 0;

public:
    Delegate() = default;

    /// \brief Attaches a free function as a subscriber to this delegate.
    /// \param func The free function to attach.
    /// \return A unique ID representing the subscription.
    [[nodiscard]] inline size_t Attach(const CallBackFunction& func);

    /// \brief Attaches a member function of an object as a subscriber to this delegate.
    /// \param obj The object whose member function will be subscribed.
    /// \param method The member function to attach.
    /// \return A unique ID representing the subscription.
    template <typename T>
    [[nodiscard]] size_t Attach(T* obj, void (T::*method)(CallBackArgs...));

    /// \brief Detaches a subscriber from this delegate using its unique ID.
    /// \param id The unique ID of the subscription to detach.
    inline void Detach(size_t id);

    /// \brief Invokes all subscribed callback functions with the provided arguments.
    /// \param args Arguments to pass to the callback functions.
    inline void InvokeAll(CallBackArgs&&... args);

    /// \brief Clears all subscribers from this delegate.
    inline void Clear();

    /// \brief Checks if this delegate has any subscribers.
    /// \return True if there are no subscribers, false otherwise.
    [[nodiscard]] inline bool IsEmpty() const;

private:
    /// \brief Generates and returns the next unique ID for a subscription.
    /// \return A unique ID for the new subscription.
    [[nodiscard]] inline size_t GetID() noexcept;
};

template <typename... CallBackArgs>
inline size_t Delegate<CallBackArgs...>::Attach(const CallBackFunction& func)
{
    auto id = GetID();
    mCallBackMap.emplace(id, func);
    return id;
}

template <typename... CallBackArgs>
inline void Delegate<CallBackArgs...>::Detach(size_t id)
{
    mCallBackMap.erase(id);
}

template <typename... CallBackArgs>
inline void Delegate<CallBackArgs...>::InvokeAll(CallBackArgs&&... args)
{
    for (const auto& [id, func] : mCallBackMap)
    {
        std::invoke(func, std::forward<CallBackArgs>(args)...);
    }
}

template <typename... CallBackArgs>
inline void Delegate<CallBackArgs...>::Clear()
{
    mCallBackMap.clear();
}

template <typename... CallBackArgs>
inline bool Delegate<CallBackArgs...>::IsEmpty() const
{
    return mCallBackMap.empty();
}

template <typename... CallBackArgs>
inline size_t Delegate<CallBackArgs...>::GetID() noexcept
{
    return mID++;
}

template <typename... CallBackArgs>
template <typename T>
inline size_t Delegate<CallBackArgs...>::Attach(T* obj, void (T::*method)(CallBackArgs...))
{
    auto id = GetID();
    auto closure = [obj, method](CallBackArgs... args) { (obj->*method)(std::forward<CallBackArgs>(args)...); };
    mCallBackMap.emplace(id, closure);
    return id;
}
