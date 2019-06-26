#pragma once

#include "VulkanObject.h"

class VulkanSemaphore : public VulkanObject<VkSemaphore>
{
public:
	void Create();
	void Destory() override;
protected:
private:
};

class VulkanFence : public VulkanObject<VkFence>
{
public:
	enum eFenceState
	{
		eUnSignaled,
		eSignaled
	};

	void Create(eFenceState state = eUnSignaled);
	void Destory() override;
protected:
private:
};

class VulkanEvent : public VulkanObject<VkEvent>
{
public:
	void Destory() override
	{
		assert(IsValid());

		Reset();
	}
protected:
private:
};
