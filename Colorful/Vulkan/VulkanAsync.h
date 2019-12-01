#pragma once

#include "Colorful/Public/RInterface.h"

class vkFence : public vkObject<VkFence>
{
public:
	enum eFenceState
	{
		eUnsignaled,
		eSignaled,
		eFenceState_MaxEnum
	};

	vkFence(const class vkDevice &device, eFenceState state);
	void destory(const class vkDevice &device);
protected:
private:
};
