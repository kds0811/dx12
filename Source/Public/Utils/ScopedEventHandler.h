#pragma once
#include <optional>
#include "EventManager.h"

/// \brief RAII wrapper for subscribing to events in the EventManager.
///        Ensures automatic detachment of the subscriber when the object goes out of scope.
class ScopedEventHandler
{
    std::optional<size_t> mCallBackID{};  ///< Unique ID of the subscription, if initialized.
    std::string mEventName{};             ///< Name of the event this handler is attached to.
public:
    /// \brief Subscribes a free function to the specified event.
    /// \param eventName The name of the event to subscribe to.
    /// \param func The free function to attach as a subscriber.
    template <typename... CallbackArgs>
    explicit inline ScopedEventHandler(const std::string& eventName, void (*func)(CallbackArgs...));

    /// \brief Subscribes a member function of an object to the specified event.
    /// \param eventName The name of the event to subscribe to.
    /// \param obj The object whose member function will be subscribed.
    /// \param method The member function to attach as a subscriber.
    template <typename T, typename... CallbackArgs>
    explicit inline ScopedEventHandler(const std::string& eventName, T* obj, void (T::*method)(CallbackArgs...));

    /// \brief Detaches the subscriber from the event manually.
    inline void Detach();

    /// \brief Checks if this handler is initialized and attached to an event.
    /// \return True if the handler is attached to an event, false otherwise.
    [[nodiscard]] inline bool IsInitialized() const;

    /// \brief Destructor ensures automatic detachment of the subscriber when this object goes out of scope.
    inline ~ScopedEventHandler();

private:
    /// \brief Internal method to handle attachment of callbacks to the event.
    /// \param eventName The name of the event to subscribe to.
    /// \param args Arguments required for attaching the callback.
    template <typename... CallbackArgs>
    void Attach(const std::string& eventName, CallbackArgs&&... args);
};

template <typename... CallbackArgs>
inline ScopedEventHandler::ScopedEventHandler(const std::string& eventName, void (*func)(CallbackArgs...)) : mEventName(eventName)
{
    Attach(mEventName, func);
}

template <typename T, typename... CallbackArgs>
inline ScopedEventHandler::ScopedEventHandler(const std::string& eventName, T* obj, void (T::*method)(CallbackArgs...)) : mEventName(eventName)
{
    Attach(mEventName, obj, method);
}

inline void ScopedEventHandler::Detach()
{
    if (!IsInitialized()) return;

    auto eventManager = EventManager::GetEventManager();
    if (eventManager)
    {
        eventManager->DetachFromEvent(mEventName, mCallBackID.value());
        mEventName = "";
        mCallBackID = std::nullopt;
    }
}

inline bool ScopedEventHandler::IsInitialized() const
{
    return mCallBackID.has_value();
}

template <typename... CallbackArgs>
inline void ScopedEventHandler::Attach(const std::string& eventName, CallbackArgs&&... args)
{
    auto eventManager = EventManager::GetEventManager();
    if (eventManager)
    {
        mCallBackID = eventManager->AttachToEvent(eventName, std::forward<CallbackArgs>(args)...);
    }
}

inline ScopedEventHandler::~ScopedEventHandler()
{
    if (IsInitialized())
    {
        Detach();
    }
}