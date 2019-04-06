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
	default: return std::string("UNKNOWN");
	}
	return std::string("UNKNOWN");
}