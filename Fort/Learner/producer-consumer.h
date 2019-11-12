#pragma once

#include "node.h"

/// Single Consumer, Single Producer, so called "Lockfree"

namespaceStart(fort)

class Product
{
public:
	Product() = default;
	virtual ~Product() = default;

	virtual void Process() {}
};

class ProductRepository
{
public:
	ProductRepository() = default;
	virtual ~ProductRepository() = default;

	inline void SetMaxSize(size_t maxSize)
	{
		assert(maxSize > 0u);
		m_MaxSize = maxSize;
	}

	inline void Store(std::shared_ptr<Product> newProduct)
	{

	}

	inline std::shared_ptr<Product> Get()
	{

	}
protected:
private:
	size_t m_MaxSize = 0u;
};

class Producer
{
public:
	Producer() = default;
	virtual ~Producer() = default;

	virtual std::shared_ptr<Product> Produce() {
		auto newProduct = std::make_shared<Product>();
		newProduct->Process();
		return newProduct;
	}
protected:
private:
};

class Consumer
{
public:
	Consumer() = default;
	virtual ~Consumer() = default;

	virtual void Consume(std::shared_ptr<Product>) {
	}
protected:
private:
};

namespaceEnd(fort)
