#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

#include "../environment/EnvironmentMain.h"
#include "GraphicsUtils.h"
#include <iostream>
#include <optional>
#include <cstring>
#include <set>
#include <memory>

namespace graphics
{
	class GraphicsManager
	{
		// VARIABLES
	private:
		bool m_initialized = false;
		std::shared_ptr<environment::EnvironmentManager> m_environment_manager;
		std::string m_engine_name;
		std::string m_app_name;

		bool m_enable_validation_layers = false;
		std::vector<const char*> m_validation_layers = { "VK_LAYER_LUNARG_standard_validation" };

		std::vector<VkImage> m_vk_swapchain_images;

		VkInstance m_vk_instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_vk_debug_messenger = VK_NULL_HANDLE;
		VkPhysicalDevice m_vk_physical_device = VK_NULL_HANDLE;
		VkDevice m_vk_device = VK_NULL_HANDLE;
		VkQueue m_vk_graphics_queue = VK_NULL_HANDLE;
		VkQueue m_vk_present_queue = VK_NULL_HANDLE;
		VkSurfaceKHR m_vk_surface = VK_NULL_HANDLE;
		VkSwapchainKHR m_vk_swapchain = VK_NULL_HANDLE;
		VkFormat m_vk_swapchain_image_format;
		VkExtent2D m_vk_swapchain_extent;

		const std::vector<const char*> device_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		// CONSTRUCTORS/DESTRUCTORS
	public:
		GraphicsManager(std::shared_ptr<environment::EnvironmentManager> environment_manager_, 
			const std::string& engine_name_ = "Engine", const std::string& app_name_ = "Default App") : 
			m_environment_manager(environment_manager_), m_engine_name(engine_name_), m_app_name(app_name_) { Initialize(); }

		~GraphicsManager() { Shutdown(); }

		// METHODES
	private:
		void Initialize();
		void Shutdown();

		bool InitializeVulkan();
		void CreateInstance();
		void SetupDebugMessenger();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSurface();
		void CreateSwapChain();

		bool IsDeviceSuitable(VkPhysicalDevice device_);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device_);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device_);

		std::vector<const char*> GetRequiredExtensions();
		VkResult CreateDebugUtilsMessengerEXT(
			const VkDebugUtilsMessengerCreateInfoEXT* create_info_,
			const VkAllocationCallbacks* allocator_);
		void DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* allocator_);
	public:
		void BeginFrame();
		void EndFrame();
	};

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity_,
		VkDebugUtilsMessageTypeFlagsEXT message_type_,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data_,
		void* user_data_);
}
