#include "VulkanTexture.h"
#include "VulkanEngine.h"

vkTexture::vkTexture(const vkDevice &device, const std::string &)
{
	assert(!isValid() && device.isValid());
}

void vkTexture::destroy(const vkDevice &device)
{
	assert(device.isValid());
}
