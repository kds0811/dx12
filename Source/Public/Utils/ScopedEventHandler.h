#pragma once
#include <optional>
#include "EventManager.h"

class ScopedEventHandler
{
	std::optional<size_t> mCallBackID{};
	std::string mEventName{};
public:
	template<typename ... CallbackArgs>
	explicit ScopedEventHandler(const std::string& eventName, void(*func)(CallbackArgs ...));

	template <typename T, typename... CallbackArgs>
	explicit ScopedEventHandler(const std::string& eventName, T* obj, void (T::* method)(CallbackArgs...));

	inline void Detach();

	[[nodiscard]] inline bool IsInitialized() const;

	inline ~ScopedEventHandler();

private:
	template<typename ... CallbackArgs>
	void Attach(const std::string& eventName, CallbackArgs&&... args);
};

template<typename ...CallbackArgs>
inline ScopedEventHandler::ScopedEventHandler(const std::string& eventName, void(*func)(CallbackArgs...))
	:
	mEventName(eventName)
{
	Attach(mEventName, func);
}

template<typename T, typename ...CallbackArgs>
inline ScopedEventHandler::ScopedEventHandler(const std::string& eventName, T* obj, void(T::* method)(CallbackArgs...))
	:
	mEventName(eventName)
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

template<typename ...CallbackArgs>
inline void ScopedEventHandler::Attach(const std::string& eventName, CallbackArgs && ...args)
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