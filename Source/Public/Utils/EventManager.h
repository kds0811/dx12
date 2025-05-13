#pragma once
#include <string>
#include <unordered_map>
#include "Delegate.h"
#include <memory>
#include <optional>
#include "Logger.h"

class ScopedEventHandler;

/// \brief Singleton class that manages events and their subscribers.
///        Provides mechanisms to create, invoke, and manage events with type-safe callbacks.
///        Subscribing to events is controlled via the ScopedEventHandler class, which ensures
///        automatic detachment of subscribers using the RAII pattern.
class EventManager
{
    friend ScopedEventHandler;
    using EventMap = std::unordered_map<std::string, std::unique_ptr<IDelegate>>;

    EventMap mEventMap;  // storage

public:
    /// \brief Returns the singleton instance of the EventManager.
    /// \return Pointer to the singleton EventManager instance.
    [[nodiscard]] static EventManager* GetEventManager();

    /// \brief Creates a new event with the specified name if it doesn't already exist.
    /// \param eventName The unique name of the event.
    /// \tparam CallbackArgs The types of arguments that the callback functions for this event will accept.
    template <typename... CallbackArgs>
    void CreateEventInstance(const std::string& eventName);

    /// \brief Invokes all subscribers of the specified event with the provided arguments.
    /// \param eventName The name of the event to invoke.
    /// \param args Arguments to pass to the subscribers.
    /// \tparam CallbackArgs The types of arguments that the callback functions for this event will accept.
    template <typename... CallbackArgs>
    void InvokeAllEventSubscribers(const std::string& eventName, CallbackArgs&&... args);

    /// \brief Clears all subscribers from the specified event.
    /// \param eventName The name of the event to clear.
    /// \tparam CallbackArgs The types of arguments that the callback functions for this event will accept.
    template <typename... CallbackArgs>
    void ClearEvent(const std::string& eventName);

    /// \brief Checks if the specified event has any subscribers.
    /// \param eventName The name of the event to check.
    /// \tparam CallbackArgs The types of arguments that the callback functions for this event will accept.
    /// \return An optional boolean indicating whether the event is empty (true if empty, false otherwise).
    template <typename... CallbackArgs>
    [[nodiscard]] std::optional<bool> IsEventEmpty(const std::string& eventName);

private:
    // Private constructor and deleted copy/move operations enforce the singleton pattern.
    // Attach and Detach methods are private and can only be accessed by ScopedEventHandler.
    // This ensures proper resource management through RAII.
    // Attaches a free function or member function as a subscriber to the specified event.
    // Returns an optional ID representing the subscription, or std::nullopt if the event doesn't exist.
    EventManager() {}
    EventManager(const EventManager&) = delete;
    EventManager(const EventManager&&) noexcept = delete;
    EventManager& operator=(const EventManager&) = delete;
    EventManager& operator=(const EventManager&&) noexcept = delete;

    template <typename... CallbackArgs>
    [[nodiscard]] std::optional<size_t> AttachToEvent(const std::string& eventName, void (*func)(CallbackArgs...));

    template <typename T, typename... CallbackArgs>
    [[nodiscard]] std::optional<size_t> AttachToEvent(const std::string& eventName, T* obj, void (T::*method)(CallbackArgs...));

    template <typename... CallbackArgs>
    void DetachFromEvent(const std::string& eventName, size_t callBackID);

    [[nodiscard]] inline bool HasEvent(const std::string& eventName);

    template <typename... CallbackArgs>
    [[nodiscard]] Delegate<CallbackArgs...>* GetDelegate(const std::string& eventName);
};

EventManager* EventManager::GetEventManager()
{
    static EventManager* instance = new EventManager();
    return instance;
}

template <typename... CallbackArgs>
inline void EventManager::CreateEventInstance(const std::string& eventName)
{
    if (!HasEvent(eventName))
    {
        mEventMap[eventName] = std::make_unique<Delegate<CallbackArgs...>>();
    }
    else
    {
        LOG_ERROR("Event with ", nameEvent, " allready is created");
    }
}

template <typename... CallbackArgs>
inline std::optional<size_t> EventManager::AttachToEvent(const std::string& eventName, void (*func)(CallbackArgs...))
{
    auto delegate = GetDelegate<CallbackArgs...>(eventName);
    if (delegate)
    {
        return delegate->Attach(func);
    }
    return std::nullopt;
}

template <typename T, typename... CallbackArgs>
inline std::optional<size_t> EventManager::AttachToEvent(const std::string& eventName, T* obj, void (T::*method)(CallbackArgs...))
{
    auto delegate = GetDelegate<CallbackArgs...>(eventName);
    if (delegate)
    {
        return delegate->Attach(obj, method);
    }
    return std::nullopt;
}

template <typename... CallbackArgs>
inline void EventManager::DetachFromEvent(const std::string& eventName, size_t callBackID)
{
    auto delegate = GetDelegate<CallbackArgs...>(eventName);
    if (delegate)
    {
        delegate->Detach(callBackID);
    }
}

template <typename... CallbackArgs>
inline void EventManager::InvokeAllEventSubscribers(const std::string& eventName, CallbackArgs&&... args)
{
    auto delegate = GetDelegate<CallbackArgs...>(eventName);
    if (delegate && !delegate->IsEmpty())
    {
        delegate->InvokeAll(std::forward<CallbackArgs>(args)...);
    }
}

template <typename... CallbackArgs>
inline void EventManager::ClearEvent(const std::string& eventName)
{
    auto delegate = GetDelegate<CallbackArgs...>(eventName);
    if (delegate)
    {
        delegate->Clear();
    }
}

template <typename... CallbackArgs>
inline std::optional<bool> EventManager::IsEventEmpty(const std::string& nameEvent)
{
    auto delegate = GetDelegate<CallbackArgs...>(nameEvent);
    if (delegate)
    {
        return delegate->IsEmpty();
    }
    return std::nullopt;
}

template <typename... CallbackArgs>
inline Delegate<CallbackArgs...>* EventManager::GetDelegate(const std::string& nameEvent)
{
    if (!HasEvent(nameEvent))
    {
        return nullptr;
    }
    return static_cast<Delegate<CallbackArgs...>*>(mEventMap.at(nameEvent).get());
}

inline bool EventManager::HasEvent(const std::string& nameEvent)
{
    if (!mEventMap.contains(nameEvent))
    {
        LOG_ERROR("An event named  ", nameEvent, "  doesn't exist");
        return false;
    }
    return true;
}