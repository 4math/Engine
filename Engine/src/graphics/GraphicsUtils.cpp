#include "GraphicsUtils.h"

std::vector<VkExtensionProperties> graphics::ListInstanceExtensions(bool print_)
{
	uint32_t extension_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> extensions(extension_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

	if (print_)
		for (const auto& extension : extensions)
			std::cout << extension.extensionName << std::endl;

	return extensions;
}

bool graphics::CheckValidationLayerSupport(std::vector<const char*> &validation_layers_)
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validation_layers_)
	{
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			return false;
		}
	}
	return true;
}

std::vector<VkPhysicalDevice> graphics::ListPhysicalDevices(VkInstance instance_)
{
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

	return devices;
}

std::string graphics::FormatVkResult(VkResult code_)
{
	switch (code_)
	{
	case VK_SUCCESS: return std::string("VK_SUCCESS");
	case VK_NOT_READY: return std::string("VK_NOT_READY");
	case VK_TIMEOUT: return std::string("VK_TIMEOUT");
	case VK_EVENT_SET: return std::string("VK_EVENT_SET");
	case VK_EVENT_RESET: return std::string("VK_EVENT_RESET");
	case VK_INCOMPLETE: return std::string("VK_INCOMPLETE");
	case VK_ERROR_OUT_OF_HOST_MEMORY: return std::string("VK_ERROR_OUT_OF_HOST_MEMORY");
	case VK_ERROR_OUT_OF_DEVICE_MEMORY: return std::string("VK_ERROR_OUT_OF_DEVICE_MEMORY");
	case VK_ERROR_INITIALIZATION_FAILED: return std::string("VK_ERROR_INITIALIZATION_FAILED");
	case VK_ERROR_DEVICE_LOST: return std::string("VK_ERROR_DEVICE_LOST");
	case VK_ERROR_MEMORY_MAP_FAILED: return std::string("VK_ERROR_MEMORY_MAP_FAILED");
	case VK_ERROR_LAYER_NOT_PRESENT: return std::string("VK_ERROR_LAYER_NOT_PRESENT");
	case VK_ERROR_EXTENSION_NOT_PRESENT: return std::string("VK_ERROR_EXTENSION_NOT_PRESENT");
	case VK_ERROR_FEATURE_NOT_PRESENT: return std::string("VK_ERROR_FEATURE_NOT_PRESENT");
	case VK_ERROR_INCOMPATIBLE_DRIVER: return std::string("VK_ERROR_INCOMPATIBLE_DRIVER");
	case VK_ERROR_TOO_MANY_OBJECTS: return std::string("VK_ERROR_TOO_MANY_OBJECTS");
	case VK_ERROR_FORMAT_NOT_SUPPORTED: return std::string("VK_ERROR_FORMAT_NOT_SUPPORTED");
	case VK_ERROR_FRAGMENTED_POOL: return std::string("VK_ERROR_FRAGMENTED_POOL");
	case VK_ERROR_OUT_OF_POOL_MEMORY: return std::string("VK_ERROR_OUT_OF_POOL_MEMORY");
	case VK_ERROR_INVALID_EXTERNAL_HANDLE: return std::string("VK_ERROR_INVALID_EXTERNAL_HANDLE");
	case VK_ERROR_SURFACE_LOST_KHR: return std::string("VK_ERROR_SURFACE_LOST_KHR");
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return std::string("VK_ERROR_NATIVE_WINDOW_IN_USE_KHR");
	case VK_SUBOPTIMAL_KHR: return std::string("VK_SUBOPTIMAL_KHR");
	case VK_ERROR_OUT_OF_DATE_KHR: return std::string("VK_ERROR_OUT_OF_DATE_KHR");
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return std::string("VK_ERROR_INCOMPATIBLE_DISPLAY_KHR");
	case VK_ERROR_VALIDATION_FAILED_EXT: return std::string("VK_ERROR_VALIDATION_FAILED_EXT");
	case VK_ERROR_INVALID_SHADER_NV: return std::string("VK_ERROR_INVALID_SHADER_NV");
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return std::string("VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT");
	case VK_ERROR_FRAGMENTATION_EXT: return std::string("VK_ERROR_FRAGMENTATION_EXT");
	case VK_ERROR_NOT_PERMITTED_EXT: return std::string("VK_ERROR_NOT_PERMITTED_EXT");
	case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT: return std::string("VK_ERROR_INVALID_DEVICE_ADDRESS_EXT");
	case VK_RESULT_RANGE_SIZE: return std::string("VK_RESULT_RANGE_SIZE");
	case VK_RESULT_MAX_ENUM: return std::string("VK_RESULT_MAX_ENUM");
	default: break;
	}
	return std::string("UNKNOWN");
}

graphics::SwapChainSupportDetails graphics::QuerySwapChainSupport(VkPhysicalDevice device_, VkSurfaceKHR surface_)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_, surface_, &details.capabilities);

	uint32_t format_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device_, surface_, &format_count, nullptr);

	if (format_count != 0)
	{
		details.formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device_, surface_, &format_count, details.formats.data());
	}

	uint32_t present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device_, surface_, &present_mode_count, nullptr);

	if (present_mode_count != 0) {
		details.present_modes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device_, surface_, &present_mode_count, details.present_modes.data());
	}

	return details;
}

VkSurfaceFormatKHR graphics::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats_)
{
	if (available_formats_.size() == 1 && available_formats_[0].format == VK_FORMAT_UNDEFINED)
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	for (const auto& available_format : available_formats_)
		if (available_format.format == VK_FORMAT_B8G8R8A8_UNORM && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return available_format;

	return available_formats_[0];
}

VkPresentModeKHR graphics::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes_) 
{
	VkPresentModeKHR best = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& available_present_mode : available_present_modes_) {
		if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
			return available_present_mode;
		else if (available_present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			best = available_present_mode;
	}

	return best;
}

VkExtent2D graphics::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities_, int width_, int height_) 
{
	if (capabilities_.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
	{
		return capabilities_.currentExtent;
	}
	else 
	{
		VkExtent2D actual_extent = { static_cast<uint32_t>(width_), static_cast<uint32_t>(height_) };

		actual_extent.width = std::max(capabilities_.minImageExtent.width, std::min(capabilities_.maxImageExtent.width, actual_extent.width));
		actual_extent.height = std::max(capabilities_.minImageExtent.height, std::min(capabilities_.maxImageExtent.height, actual_extent.height));

		return actual_extent;
	}
}

uint32_t graphics::FindMemoryType(uint32_t type_filter_, VkMemoryPropertyFlags properties_, VkPhysicalDevice physical_device_)
{
	VkPhysicalDeviceMemoryProperties mem_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device_, &mem_properties);
	for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
	{
		if (type_filter_ & (1 << i) && (mem_properties.memoryTypes[i].propertyFlags & properties_) == properties_)
			return i;
	}

	throw std::runtime_error("Failed to find suitable memory type");
}
