#include "GraphicsMain.h"

void graphics::GraphicsManager::Initialize()
{
	if (!InitializeVulkan())
		return;

	m_initialized = true;
}

void graphics::GraphicsManager::Shutdown()
{
	vkDestroySwapchainKHR(m_vk_device, m_vk_swapchain, nullptr);
	vkDestroyDevice(m_vk_device, nullptr);
	if (m_enable_validation_layers)
		DestroyDebugUtilsMessengerEXT(nullptr);

	if (m_vk_instance != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(m_vk_instance, m_vk_surface, nullptr);
		vkDestroyInstance(m_vk_instance, nullptr);
	}

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
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapChain();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
		return false;
	}
	return true;
}

void graphics::GraphicsManager::CreateInstance()
{
	if (m_enable_validation_layers && !CheckValidationLayerSupport(m_validation_layers))
		throw std::runtime_error("Validation layers requested, but not available!");

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

	auto result = vkCreateInstance(&create_info, nullptr, &m_vk_instance);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create VkInstance, error: " + FormatVkResult(result));
}

void graphics::GraphicsManager::SetupDebugMessenger()
{
	if (!m_enable_validation_layers) return;

	VkDebugUtilsMessengerCreateInfoEXT create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	// Add VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT for full info
	create_info.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = DebugCallback;

	auto result = CreateDebugUtilsMessengerEXT(&create_info, nullptr);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to set up debug messenger, error: " + FormatVkResult(result));
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

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(), indices.present_family.value() };

	for (auto queue_family : unique_queue_families)
	{
		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueCount = 1;
		queue_create_info.queueFamilyIndex = queue_family;
		queue_create_info.pQueuePriorities = &queue_priority;

		queue_create_infos.push_back(queue_create_info);
	}

	VkPhysicalDeviceFeatures device_features = {};

	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.queueCreateInfoCount = queue_create_infos.size();
	create_info.pEnabledFeatures = &device_features;
	create_info.ppEnabledExtensionNames = device_extensions.data();
	create_info.enabledExtensionCount = device_extensions.size();
	create_info.enabledLayerCount = 0;

	auto result = vkCreateDevice(m_vk_physical_device, &create_info, nullptr, &m_vk_device);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create VkDevice, error: " + FormatVkResult(result));

	vkGetDeviceQueue(m_vk_device, indices.graphics_family.value(), 0, &m_vk_graphics_queue);
	vkGetDeviceQueue(m_vk_device, indices.present_family.value(), 0, &m_vk_present_queue);
}

void graphics::GraphicsManager::CreateSurface()
{
	VkWin32SurfaceCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	create_info.hwnd = m_environment_manager->GetWindowHandle();
	create_info.hinstance = GetModuleHandle(nullptr);

	auto result = vkCreateWin32SurfaceKHR(m_vk_instance, &create_info, nullptr, &m_vk_surface);

	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create VkSurface, error: " + FormatVkResult(result));
}

void graphics::GraphicsManager::CreateSwapChain()
{
	SwapChainSupportDetails swapchain_support = QuerySwapChainSupport(m_vk_physical_device, m_vk_surface);

	VkSurfaceFormatKHR surface_format = ÑhooseSwapSurfaceFormat(swapchain_support.formats);
	VkPresentModeKHR present_mode = ÑhooseSwapPresentMode(swapchain_support.present_modes);
	VkExtent2D extent = ChooseSwapExtent(swapchain_support.capabilities, m_environment_manager->WindowWidth(), m_environment_manager->WindowHeight());

	uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;
	if (swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount)
		image_count = swapchain_support.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = m_vk_surface;
	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	create_info.preTransform = swapchain_support.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	QueueFamilyIndices indices = FindQueueFamilies(m_vk_physical_device);
	uint32_t queue_family_indices[] = { indices.graphics_family.value(), indices.present_family.value() };

	if (indices.graphics_family != indices.present_family) 
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	}
	else 
	{
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;
	}

	auto result = vkCreateSwapchainKHR(m_vk_device, &create_info, nullptr, &m_vk_swapchain);
	if (result != VK_SUCCESS)
		throw std::runtime_error(FormatVkResult(result));
}

std::vector<const char*> graphics::GraphicsManager::GetRequiredExtensions()
{
	uint32_t glfw_extension_count = 0;
	auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

	if (m_enable_validation_layers) 
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

VkResult graphics::GraphicsManager::CreateDebugUtilsMessengerEXT(
	const VkDebugUtilsMessengerCreateInfoEXT* create_info_,
	const VkAllocationCallbacks* allocator_)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vk_instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) 
		return func(m_vk_instance, create_info_, allocator_, &m_vk_debug_messenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

bool graphics::GraphicsManager::IsDeviceSuitable(VkPhysicalDevice device_)
{
	QueueFamilyIndices indices = FindQueueFamilies(device_);

	bool extensions_supported = CheckDeviceExtensionSupport(device_);

	bool swap_chain_adequate = false;
	if (extensions_supported)
	{
		SwapChainSupportDetails swap_chain_details = QuerySwapChainSupport(device_, m_vk_surface);
		swap_chain_adequate = !swap_chain_details.formats.empty() && !swap_chain_details.present_modes.empty();
	}

	return indices.IsComplete() && extensions_supported && swap_chain_adequate;
}

bool graphics::GraphicsManager::CheckDeviceExtensionSupport(VkPhysicalDevice device_) 
{
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(device_, nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> available_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(device_, nullptr, &extension_count, available_extensions.data());

	std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

	for (const auto& extension : available_extensions)
		required_extensions.erase(extension.extensionName);

	return required_extensions.empty();
}

graphics::QueueFamilyIndices graphics::GraphicsManager::FindQueueFamilies(VkPhysicalDevice device_)
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
			indices.graphics_family = i;

		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device_, i, m_vk_surface, &present_support);

		if (queue_family.queueCount > 0 && present_support) 
			indices.present_family = i;

		if (indices.IsComplete())
			break;

		i++;
	}

	return indices;
}

void graphics::GraphicsManager::DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* allocator_)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vk_instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) 
		func(m_vk_instance, m_vk_debug_messenger, allocator_);
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
