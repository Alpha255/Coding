#pragma once

#include "node.h"

class Product
{
public:
	Product() = default;
	virtual ~Product() = default;

	virtual void Process() {}
};

template <typename MaxSize, typename T = Product> struct ProductRepository
{
	std::unordered_map<uint32_t, std::shared_ptr<T>> Products;

	inline bool IsFull() const
	{
		return Products.size() == MaxSize;
	}

	inline bool IsEmpty() const
	{
		return Products.size() == 0U;
	}
};

class Producer
{
public:

protected:
private:
};
