#include "GraphicsMain.h"

void graphics::GraphicsManager::Initialize()
{
	if (!InitializeVulkan())
		return;

	m_initialized = true;
}

void graphics::GraphicsManager::Shutdown()
{
	vkDestroyInstance(m_vk_instance, nullptr);
	m_initialized = false;
}

bool graphics::GraphicsManager::InitializeVulkan()
{
	try
	{
		CreateInstance();
		PickPhysicalDevice();
	}
	catch (const std::exception& e)
	{
#ifdef _DEBUG
		std::cerr << e.what() << "\n";
#endif // _DEBUG
		return false;
	}

	return true;
}

void graphics::GraphicsManager::CreateInstance()
{
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = m_app_name.c_str();
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = m_engine_name.c_str();
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;

	uint32_t extension_count = 0;
	const char** required_extensions;
	required_extensions = glfwGetRequiredInstanceExtensions(&extension_count);
	
	auto present_extensions = ListInstanceExtensions(false);
	for (unsigned extension_id = 0; extension_id < extension_count; extension_id++)
	{
		bool status = false;
		std::string required_extension_name = required_extensions[extension_id];
		for (const auto present_extension : present_extensions)
		{
			std::string present_extension_name = present_extension.extensionName;
			if (required_extension_name == present_extension_name)
			{
				status = true;
				break;
			}
		}

		if (status != true)
			throw std::runtime_error("Required extension " + required_extension_name + " was not found");
	}

	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	create_info.enabledExtensionCount = extension_count;
	create_info.ppEnabledExtensionNames = required_extensions;
	create_info.enabledLayerCount = 0;

	VkResult result = vkCreateInstance(&create_info, nullptr, &m_vk_instance);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create VkInstance, error code: " + std::to_string(result));
}

void graphics::GraphicsManager::PickPhysicalDevice()
{
	auto devices = ListPhysicalDevices(m_vk_instance);

	for (const auto& device : devices) 
	{
		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties(device, &device_properties);

		VkPhysicalDeviceFeatures device_features;
		vkGetPhysicalDeviceFeatures(device, &device_features);

		bool is_suitable = 
			device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			device_features.geometryShader;

		if (is_suitable) 
		{
			m_vk_physical_device = device;
			break;
		}
	}

	if (m_vk_physical_device == VK_NULL_HANDLE)
		throw std::runtime_error("Failed to find a suitable GPU");
}

void graphics::GraphicsManager::BeginFrame()
{

}

void graphics::GraphicsManager::EndFrame()
{

}
