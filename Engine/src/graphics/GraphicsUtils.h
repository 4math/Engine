#pragma once

#include "vulkan/vulkan.h"
#include <string>
#include <vector>
#include <iostream>
#include <optional>

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphics_family;

	bool IsComplete()
	{
		return graphics_family.has_value();
	}
};

std::vector<VkExtensionProperties> ListInstanceExtensions(bool print_);
std::vector<VkPhysicalDevice> ListPhysicalDevices(VkInstance instance_);
bool IsDeviceSuitable(VkPhysicalDevice device_);
QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device_);