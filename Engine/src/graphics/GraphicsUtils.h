#pragma once

#include "vulkan/vulkan.h"
#include <string>
#include <vector>
#include <iostream>
#include <optional>

namespace graphics
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphics_family;

		bool IsComplete()
		{
			return graphics_family.has_value();
		}
	};

	std::vector<VkExtensionProperties> ListInstanceExtensions(bool print_);
	bool CheckValidationLayerSupport(std::vector<const char*> &validation_layers_);
	std::vector<VkPhysicalDevice> ListPhysicalDevices(VkInstance instance_);
	bool IsDeviceSuitable(VkPhysicalDevice device_);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device_);
	std::string FormatVkResult(VkResult code_);
}
