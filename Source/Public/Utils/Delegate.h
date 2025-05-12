#pragma once
#include <functional>
#include <unordered_map>

class IDelegate
{
public:
	IDelegate() = default;
	virtual ~IDelegate() = default;
};

template <typename ... CallBackArgs>
class Delegate : public IDelegate
{
public:
	using CallBackFunction = std::function<void(CallBackArgs ...)>;
	using CallBackMap = std::unordered_map<size_t, CallBackFunction>;

private:
	CallBackMap mCallBackMap{};
	size_t mID = 0;

public:
	Delegate() = default;

	size_t Attach(const CallBackFunction& func)
	{
		auto id = GetID();
		mCallBackMap.emplace(id, func);
		return id;
	}

	template <typename T>
	size_t Attach(T* obj, void (T::* method)(CallBackArgs...))
	{
		auto id = GetID();
		auto closure = [obj, method](CallBackArgs... args) { (obj->*method)(std::forward<CallBackArgs>(args) ...); };
		mCallBackMap.emplace(id, closure);
		return id;
	}

	void Detach(size_t id)
	{
		mCallBackMap.erase(id);
	}

	void InvokeAll(CallBackArgs&& ... args)
	{
		for (const auto& [id, func] : mCallBackMap)
		{
			std::invoke(func, std::forward<CallBackArgs>(args)...);
		}
	}

	void Clear()
	{
		mCallBackMap.clear();
	}

	bool IsEmpty()
	{
		return mCallBackMap.empty();
	}

private:
	size_t GetID()
	{
		return mID++;
	}
};
