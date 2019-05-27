#pragma once

#include "vulkan/vulkan.h"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <optional>
#include <algorithm>

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

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	std::vector<VkExtensionProperties> ListInstanceExtensions(bool print_);
	bool CheckValidationLayerSupport(std::vector<const char*> &validation_layers_);
	std::vector<VkPhysicalDevice> ListPhysicalDevices(VkInstance instance_);
	std::string FormatVkResult(VkResult code_);

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device_, VkSurfaceKHR surface_);
	VkSurfaceFormatKHR ÑhooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats_);
	VkPresentModeKHR ÑhooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes_);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities_, int width_, int height_);
}
