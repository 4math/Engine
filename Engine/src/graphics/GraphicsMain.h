#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

#include "../environment/EnvironmentMain.h"
#include "GraphicsShaders.h"
#include "GraphicsUtils.h"
#include <iostream>
#include <optional>
#include <cstring>
#include <set>
#include <memory>
#include <limits>

namespace graphics
{
	class GraphicsManager
	{
		// VARIABLES
	private:
		bool m_initialized = false;

// Synchronization block 
		std::vector<VkSemaphore> m_semaphores_image_available;
		std::vector<VkSemaphore> m_semaphores_finished;
		std::vector<VkFence> m_in_flight_fences;

		int m_max_frames_in_flight = 1;
		size_t m_current_frame = 0;

// Managers and information block 
		std::shared_ptr<graphics::ShaderManager> m_shader_manager;
		std::shared_ptr<environment::EnvironmentManager> m_environment_manager;
		std::string m_engine_name;
		std::string m_app_name;

// debug block 
		bool m_enable_validation_layers = false;
		VkDebugUtilsMessengerEXT m_vk_debug_messenger = VK_NULL_HANDLE;
		std::vector<const char*> m_validation_layers = { "VK_LAYER_LUNARG_standard_validation" };

// Swap chain block 
		VkSwapchainKHR m_vk_swapchain = VK_NULL_HANDLE;
		std::vector<VkImage> m_vk_swapchain_images;
		VkFormat m_vk_swapchain_image_format = VK_FORMAT_UNDEFINED;
		VkExtent2D m_vk_swapchain_extent = { 0, 0 };
		std::vector<VkFramebuffer> m_vk_swapchain_framebuffers;
		std::vector<VkCommandBuffer> m_vk_command_buffers;
		std::vector<VkImageView> m_vk_image_views;
		VkBuffer m_vk_vertex_buffer = VK_NULL_HANDLE;
		VkDeviceMemory m_vk_vertex_buffer_memory = VK_NULL_HANDLE;

// GPU block 
		VkInstance m_vk_instance = VK_NULL_HANDLE;
		VkPhysicalDevice m_vk_physical_device = VK_NULL_HANDLE;
		VkDevice m_vk_device = VK_NULL_HANDLE;
		VkSurfaceKHR m_vk_surface = VK_NULL_HANDLE;

// Render block 
		VkQueue m_vk_graphics_queue = VK_NULL_HANDLE;
		VkQueue m_vk_present_queue = VK_NULL_HANDLE;
		VkRenderPass m_vk_render_pass = VK_NULL_HANDLE;
		VkPipelineLayout m_vk_pipeline_layout = VK_NULL_HANDLE;
		VkPipeline m_vk_graphics_pipeline = VK_NULL_HANDLE;
		VkCommandPool m_vk_command_pool = VK_NULL_HANDLE;

		const std::vector<const char*> device_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		// CONSTRUCTORS/DESTRUCTORS
	public:
		GraphicsManager(
			std::shared_ptr<environment::EnvironmentManager>& environment_manager_, 
			int max_frames_in_flight_ = 1,
			const std::string& engine_name_ = "Engine", 
			const std::string& app_name_ = "Default App") : 
			m_environment_manager(environment_manager_), 
			m_max_frames_in_flight(max_frames_in_flight_),
			m_engine_name(engine_name_), 
			m_app_name(app_name_)
		{ Initialize(); }

		~GraphicsManager() { Shutdown(); }

		// METHODES
	private:
		void Initialize();
		void Shutdown();
		void ShutdownSwapChain();

		bool InitializeVulkan();
		void CreateInstance();
		void SetupDebugMessenger();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSurface();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateRenderPass();
		void CreateGraphicsPipeline();
		void CreateFramebuffers();
		void CreateCommandPool();
		void CreateVertexBuffers();
		void CreateCommandBuffers();
		void CreateSync();
		void RecreateSwapChain();

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
		void WaitDevice();
	};

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity_,
		VkDebugUtilsMessageTypeFlagsEXT message_type_,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data_,
		void* user_data_);
}
