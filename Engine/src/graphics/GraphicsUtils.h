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
		std::optional<uint32_t> present_family;

		bool IsComplete()
		{
			return graphics_family.has_value() && present_family.has_value();
		}
	};

	std::vector<VkExtensionProperties> ListInstanceExtensions(bool print_);
	bool CheckValidationLayerSupport(std::vector<const char*> &validation_layers_);
	std::vector<VkPhysicalDevice> ListPhysicalDevices(VkInstance instance_);
	std::string FormatVkResult(VkResult code_);
}
