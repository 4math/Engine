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

	return std::vector<VkExtensionProperties>(extensions);
}