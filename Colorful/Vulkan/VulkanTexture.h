#pragma once

#include "Colorful/Vulkan/VulkanContext.h"

class vkTexture : public vkDeviceObject<VkImage>, public rTexture
{
public:
	vkTexture(const class vkDevice &device);
	vkTexture(const class vkDevice &device, const std::string &fileName);
	void destroy(const class vkDevice &device) override final;
protected:
	void transitionImageLayout();
private:
};

class vkTexture1D : public vkTexture
{

};

class vkTexture2D : public vkTexture
{

};

class vkTexture3D : public vkTexture
{

};

class vkTextureCube : public vkTexture
{

};

class vkTexture1DArray : public vkTexture
{

};

class vkTexture2DArray : public vkTexture
{

};

class vkTexture3DArray : public vkTexture
{

};

class vkTextureCubeArray : public vkTexture
{

};
