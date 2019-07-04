#pragma once
#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <functional>

template<typename T>
class Singleton
{
public:
	template<typename... Args>
	static T& instance(Args... args)
	{
		static auto once_function = std::bind(create_instance<Args...>, args...);
		return apply(once_function);
	}

private:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	static T& apply(const std::function<T&()>& function)
	{
		static T& instance_ref = function();
		return instance_ref;
	}

	template<typename... Args>
	static T& create_instance(Args... args)
	{
		static T instance_{ std::forward<Args>(args)... };
		return instance_;
	}

protected:
	Singleton() = default;
	virtual ~Singleton() = default;

};

/* usage

class Foo: public Singleton<Foo>
{
private:
friend class Singleton<Foo>
Foo()	{ }
~Foo()	{ }
};

*/

#endif // !SINGLETON_HPP
