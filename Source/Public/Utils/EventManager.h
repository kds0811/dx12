#pragma once
#include <string>
#include <unordered_map>
#include "Delegate.h"
#include <memory>
#include <optional>
#include "Logger.h"

class ScopedEventHandler;

class EventManager
{
	friend ScopedEventHandler;
	using EventMap = std::unordered_map<std::string, std::unique_ptr<IDelegate>>;

	EventMap mEventMap; // storage

public:
	[[nodiscard]] static EventManager* GetEventManager();

	template<typename ... CallbackArgs>
	void CreateEvent(const std::string& eventName);

	template<typename ... CallbackArgs>
	void InvokeAllEventSubscribers(const std::string& eventName, CallbackArgs&& ... args);

	template<typename ... CallbackArgs>
	void ClearEvent(const std::string& eventName);

	template<typename ... CallbackArgs>
	[[nodiscard]] std::optional<bool> IsEventEmpty(const std::string& eventName);

private:
	EventManager() {}
	EventManager(const EventManager&) = delete;
	EventManager(const EventManager&&) noexcept = delete;
	EventManager& operator=(const EventManager&) = delete;
	EventManager& operator=(const EventManager&&) noexcept = delete;

	template<typename ... CallbackArgs>
	[[nodiscard]] std::optional<size_t> AttachToEvent(const std::string& eventName, void(*func)(CallbackArgs ...));

	template <typename T, typename... CallbackArgs>
	[[nodiscard]] std::optional<size_t> AttachToEvent(const std::string& eventName, T* obj, void (T::* method)(CallbackArgs...));

	template<typename ... CallbackArgs>
	void DetachFromEvent(const std::string& eventName, size_t callBackID);

	[[nodiscard]] inline bool HasEvent(const std::string& eventName);

	template<typename ...CallbackArgs>
	[[nodiscard]] Delegate<CallbackArgs...>* GetDelegate(const std::string& eventName);
};

EventManager* EventManager::GetEventManager()
{
	static EventManager* instance = new EventManager();
	return instance;
}

template<typename ...CallbackArgs>
inline void EventManager::CreateEvent(const std::string& eventName)
{
	if (!HasEvent(eventName))
	{
		mEventMap[eventName] = std::make_unique<Delegate<CallbackArgs ...>>();
	}
	else
	{
		LOG_ERROR("Event with ", nameEvent, " allready is created");
	}
}

template<typename ...CallbackArgs>
inline std::optional<size_t> EventManager::AttachToEvent(const std::string& eventName, void(*func)(CallbackArgs...))
{
	auto delegate = GetDelegate<CallbackArgs...>(eventName);
	if (delegate)
	{
		return delegate->Attach(func);
	}
	return std::nullopt;
}

template<typename T, typename ...CallbackArgs>
inline std::optional<size_t> EventManager::AttachToEvent(const std::string& eventName, T* obj, void(T::* method)(CallbackArgs...))
{
	auto delegate = GetDelegate<CallbackArgs...>(eventName);
	if (delegate)
	{
		return delegate->Attach(obj, method);
	}
	return std::nullopt;
}

template<typename ...CallbackArgs>
inline void EventManager::DetachFromEvent(const std::string& eventName, size_t callBackID)
{
	auto delegate = GetDelegate<CallbackArgs...>(eventName);
	if (delegate)
	{
		delegate->Detach(callBackID);
	}
}

template<typename ...CallbackArgs>
inline void EventManager::InvokeAllEventSubscribers(const std::string& eventName, CallbackArgs && ...args)
{
	auto delegate = GetDelegate<CallbackArgs...>(eventName);
	if (delegate && !delegate->IsEmpty())
	{
		delegate->InvokeAll(std::forward<CallbackArgs>(args)...);
	}
}

template<typename ...CallbackArgs>
inline void EventManager::ClearEvent(const std::string& eventName)
{
	auto delegate = GetDelegate<CallbackArgs...>(eventName);
	if (delegate)
	{
		delegate->Clear();
	}
}

template<typename ...CallbackArgs>
inline std::optional<bool> EventManager::IsEventEmpty(const std::string& nameEvent)
{
	auto delegate = GetDelegate<CallbackArgs...>(nameEvent);
	if (delegate)
	{
		return delegate->IsEmpty();
	}
	return std::nullopt;
}

template<typename ...CallbackArgs>
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