#pragma once

#include "Colorful/Vulkan/VulkanContext.h"

class vkTexture : public vkObject<VkImage>, public rTexture
{

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
