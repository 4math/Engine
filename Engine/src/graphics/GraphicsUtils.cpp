#include "GraphicsUtils.h"

std::vector<VkExtensionProperties> ListInstanceExtensions(bool print_)
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

std::vector<VkPhysicalDevice> ListPhysicalDevices(VkInstance instance_)
{
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

	return devices;
}

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device_)
{
	QueueFamilyIndices indices;

	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device_, &queue_family_count, nullptr);

	std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device_, &queue_family_count, queue_families.data());

	int i = 0;
	for (const auto& queue_family : queue_families)
	{
		if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphics_family = i;
		}

		if (indices.IsComplete()) 
			break;

		i++;
	}

	return indices;
}

bool IsDeviceSuitable(VkPhysicalDevice device_) 
{
	QueueFamilyIndices indices = FindQueueFamilies(device_);
	return indices.IsComplete();
}