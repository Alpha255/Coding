#pragma once

#include <stack>
#include <exception>
#include <mutex>
#include <memory>

struct empty_stack : public std::exception
{
	const char *what() const throw() override
	{
		return "empty stack !!!";
	}
};

template <typename T> class threadsafe_stack
{
public:
	threadsafe_stack() :
		_data(std::stack<T>())
	{
	}

	threadsafe_stack(const threadsafe_stack &other)
	{
		std::lock_guard<std::mutex> lock(other._mx);
		_data = other._data;
	}

	threadsafe_stack &operator= (const threadsafe_stack &other) = delete;

	void push(T new_value)
	{
		std::lock_guard<std::mutex> lock(_mx);
		_data.push(new_value);
	}

	std::shared_ptr<T> pop()
	{
		std::lock_guard<std::mutex> lock(_mx);
		if (_data.empty())
		{
			throw empty_stack();
		}

		std::shared_ptr<T> const res(std::make_shared<T>(_data.top()));
		_data.pop();
		return res;
	}

	void pop(T &value)
	{
		std::lock_guard<std::mutex> lock(_mx);
		if (_data.empty())
		{
			throw empty_stack();
		}

		value = _data.top();
		_data.pop();
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lock(_mx);
		return _data.empty();
	}
protected:
private:
	std::stack<T> _data;
	mutable std::mutex _mx;
};
