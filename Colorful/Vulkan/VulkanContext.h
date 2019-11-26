#pragma once

#include "Colorful/Public/RAsset.h"

#define vkMemoryAllocator nullptr /// For future use

class vkContext : public rContext
{
};
typedef std::shared_ptr<vkContext> vkContextPtr;
