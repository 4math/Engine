#pragma once

#include "GraphicsUtils.h"
#include "vulkan/vulkan.h"
#include "glm.hpp"
#include <string>
#include <vector>
#include <array>
#include <fstream>

namespace graphics 
{
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription GetBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
	};

	const std::vector<Vertex> vertices =
	{
		{{0.0f, -0.5f},{1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f},{0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f},{0.0f, 0.0f, 1.0f}},
	};

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