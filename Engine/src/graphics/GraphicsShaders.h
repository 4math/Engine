#pragma once

#include "GraphicsUtils.h"
#include "vulkan/vulkan.h"
#include <string>
#include <vector>
#include <fstream>

namespace graphics 
{
	enum ShaderInputType
	{
		BinaryInput,
		CodeInput
	};

	class ShaderManager
	{
		// VARIABLES
	private:
		bool m_initialized = false;

		std::string m_base_dir;

		// CONSTRUCTORS/DESTRUCTORS
	public:
		ShaderManager(const std::string& base_dir_ = "") : m_base_dir(base_dir_) { Initialize(); }
		~ShaderManager() { Shutdown(); }

		// METHODES
	private:
		void Initialize();
		void Shutdown();

		std::vector<char> LoadShaderBinary(const std::string& file_name_);
		std::string LoadShaderCode(const std::string& file_name_);
		VkShaderModule CreateShaderModule(const std::vector<char>& binary_, VkDevice device_);

	public:
		VkShaderModule CreateShaderModule(const std::string& file_name_, ShaderInputType input_type_, VkDevice device_);

	};
}