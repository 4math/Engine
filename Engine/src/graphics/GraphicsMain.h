#pragma once

#include "../environment/EnvironmentMain.h"
#include "vulkan/vulkan.h"
#include "GraphicsUtils.h"
#include <iostream>
#include <optional>

namespace graphics
{

	class GraphicsManager
	{
		// VARIABLES
	private:
		bool m_initialized = false;
		environment::EnvironmentManager* m_environment_manager = nullptr;
		std::string m_engine_name;
		std::string m_app_name;

		VkInstance m_vk_instance = VK_NULL_HANDLE;
		VkPhysicalDevice m_vk_physical_device = VK_NULL_HANDLE;
		VkDevice m_vk_device = VK_NULL_HANDLE;
		VkQueue m_vk_graphics_queue = VK_NULL_HANDLE;

		// CONSTRUCTORS/DESTRUCTORS
	public:
		GraphicsManager(environment::EnvironmentManager* environment_manager_, 
			const std::string& engine_name_ = "Engine", const std::string& app_name_ = "Default App") : 
			m_environment_manager(environment_manager_), m_engine_name(engine_name_), m_app_name(app_name_) { Initialize(); }
		~GraphicsManager() { Shutdown(); }

		// METHODES
	private:
		void Initialize();
		void Shutdown();

		bool InitializeVulkan();
		void CreateInstance();
		void PickPhysicalDevice();
		void CreateLogicalDevice();

	public:
		void BeginFrame();
		void EndFrame();
	};

}
