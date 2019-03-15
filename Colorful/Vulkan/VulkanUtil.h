#pragma once

#include "VulkanObject.h"

NamespaceBegin(VulkanUtil)

std::vector<const char *> FilterLayers(const std::vector<VkLayerProperties> &supportedLayers);
std::vector<const char *> FilterExtensions(const std::vector<VkExtensionProperties> &supportedExtensions);

NamespaceEnd(VulkanUtil)
