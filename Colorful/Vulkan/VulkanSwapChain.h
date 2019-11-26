#pragma once

#include "Colorful/Public/RInterface.h"

class vkSwapChain : public vkObject<VkSwapchainKHR>
{

};
typedef std::shared_ptr<vkSwapChain> vkSwapChainPtr;
