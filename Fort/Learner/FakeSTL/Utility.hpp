#pragma once

#include "Gear/Gear.h"

namespaceStart(FakeSTL)

struct ContainerBase
{
	void orphanAll() noexcept 
	{
	}

	void swapAll(ContainerBase&) noexcept
	{
	}
};

struct IteratorBase
{
	void adopt(const void*) noexcept
	{
	}

	const ContainerBase* getCont() const noexcept 
	{
		return nullptr;
	}

	static constexpr bool UnwrapWhenUnverified = true;
};

struct ContainerProxy
{
	ContainerProxy() noexcept
		: Container(nullptr)
		, FirstIter(nullptr)
	{
	}

	const ContainerBase* Container;
	IteratorBase* FirstIter;
};

namespaceEnd(FakeSTL)
