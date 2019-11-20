#pragma once

#include "Colorful/Public/RInterface.h"

#include <vulkan/vulkan.h>

class vkContext : public rContext
{
};
typedef std::shared_ptr<vkContext> vkContextPtr;
