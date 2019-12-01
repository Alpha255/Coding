#pragma once

#include "Colorful/Public/RInterface.h"

class vkFence : public vkDeviceObject<VkFence>
{
public:
	enum eFenceState
	{
		eUnsignaled,
		eSignaled,
		eFenceState_MaxEnum
	};

	vkFence(const class vkDevice &device, eFenceState state);
	void destory(const class vkDevice &device) override final;
};

class vkSemaphore : public vkDeviceObject<VkSemaphore>
{
public:
	vkSemaphore(const class vkDevice &device);
	void destory(const class vkDevice &device) override final;
};

class vkEvent : public vkDeviceObject<VkEvent>
{
public:
	vkEvent(const class vkDevice &device);
	void destory(const class vkDevice &device) override final;
};
