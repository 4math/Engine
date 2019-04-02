#include "GraphicsMain.h"

void graphics::GraphicsManager::Initialize()
{
	if (!InitializeVulkan())
		return;

	m_initialized = true;
}

void graphics::GraphicsManager::Shutdown()
{
	vkDestroyDevice(m_vk_device, nullptr);
	if (m_enable_validation_layers)
	{
		DestroyDebugUtilsMessengerEXT(nullptr);
	}
	vkDestroyInstance(m_vk_instance, nullptr);
	m_initialized = false;
}

bool graphics::GraphicsManager::InitializeVulkan()
{
#ifdef _DEBUG
	m_enable_validation_layers = true;
#endif
	try
	{
		CreateInstance();
		SetupDebugMessenger();
		PickPhysicalDevice();
		CreateLogicalDevice();
	}
	catch (const std::exception& e)
	{
#ifdef _DEBUG
		std::cerr << e.what() << "\n";
#endif
		return false;
	}
	return true;
}

void graphics::GraphicsManager::CreateInstance()
{
	if (m_enable_validation_layers && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = m_app_name.c_str();
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = m_engine_name.c_str();
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	auto extensions = GetRequiredExtensions();
	create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	create_info.ppEnabledExtensionNames = extensions.data();

	if (m_enable_validation_layers) 
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
		create_info.ppEnabledLayerNames = m_validation_layers.data();
	}
	else 
	{
		create_info.enabledLayerCount = 0;
	}

	VkResult result = vkCreateInstance(&create_info, nullptr, &m_vk_instance);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create VkInstance, error code: " + std::to_string(result));
	}
}

void graphics::GraphicsManager::SetupDebugMessenger()
{
	if (!m_enable_validation_layers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	// Add VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT for full info
	createInfo.messageType = 
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;

	if (CreateDebugUtilsMessengerEXT(&createInfo, nullptr) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void graphics::GraphicsManager::PickPhysicalDevice()
{
	auto devices = ListPhysicalDevices(m_vk_instance);

	for (const auto& device : devices) 
	{
		if (IsDeviceSuitable(device)) 
		{
			m_vk_physical_device = device;
			break;
		}
	}

	if (m_vk_physical_device == VK_NULL_HANDLE)
		throw std::runtime_error("Failed to find a suitable GPU");
}

void graphics::GraphicsManager::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(m_vk_physical_device);

	float queue_priority = 1.0f;

	VkDeviceQueueCreateInfo queue_create_info = {};
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueCount = 1;
	queue_create_info.queueFamilyIndex = indices.graphics_family.value();
	queue_create_info.pQueuePriorities = &queue_priority;

	VkPhysicalDeviceFeatures device_features = {};

	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = &queue_create_info;
	create_info.queueCreateInfoCount = 1;
	create_info.pEnabledFeatures = &device_features;
	create_info.enabledExtensionCount = 0;
	create_info.enabledLayerCount = 0;

	VkResult result = vkCreateDevice(m_vk_physical_device, &create_info, nullptr, &m_vk_device);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create VkDevice, error code: " + std::to_string(result));
	}

	vkGetDeviceQueue(m_vk_device, indices.graphics_family.value(), 0, &m_vk_graphics_queue);
}

bool graphics::GraphicsManager::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_validation_layers) 
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

std::vector<const char*> graphics::GraphicsManager::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (m_enable_validation_layers) 
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

VkResult graphics::GraphicsManager::CreateDebugUtilsMessengerEXT(
	const VkDebugUtilsMessengerCreateInfoEXT* create_info_,
	const VkAllocationCallbacks* allocator_)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vk_instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) 
	{
		return func(m_vk_instance, create_info_, allocator_, &m_vk_debug_messenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void graphics::GraphicsManager::DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* allocator_)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vk_instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) 
	{
		func(m_vk_instance, m_vk_debug_messenger, allocator_);
	}
}

void graphics::GraphicsManager::BeginFrame()
{

}

void graphics::GraphicsManager::EndFrame()
{

}

VKAPI_ATTR VkBool32 VKAPI_CALL graphics::DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity_,
	VkDebugUtilsMessageTypeFlagsEXT message_type_,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data_,
	void* user_data_)
{
	std::cerr << "Validation layer : " << callback_data_->pMessage << std::endl;
	return VK_FALSE;
}
