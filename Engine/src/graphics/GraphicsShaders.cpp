#include "GraphicsShaders.h"

void graphics::ShaderManager::Initialize()
{

}

void graphics::ShaderManager::Shutdown()
{

}

std::vector<char> graphics::ShaderManager::LoadShaderBinary(const std::string& file_name_)
{
	std::fstream input_file(m_base_dir + file_name_, std::ios::binary | std::ios::ate | std::ios::in);
	if (!input_file.is_open())
		throw std::runtime_error("Unable to open shader binary file");

	size_t file_size = static_cast<size_t>(input_file.tellg());
	std::vector<char> bytecode(file_size);
	
	input_file.seekg(0);
	input_file.read(bytecode.data(), file_size);
	input_file.close();

	return bytecode;
}

std::string graphics::ShaderManager::LoadShaderCode(const std::string& file_name_)
{
	// TODO: Compile shader here
	std::fstream input_file(file_name_, std::ios::in);
	if (!input_file.is_open()) 
		throw std::runtime_error("Unable to open shader code file");

	std::string code;

	return code;
}

VkShaderModule graphics::ShaderManager::CreateShaderModule(const std::vector<char>& binary_, VkDevice device_)
{
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = binary_.size();
	create_info.pCode = reinterpret_cast<const uint32_t*>(binary_.data());

	VkShaderModule shader_module;
	auto result = vkCreateShaderModule(device_, &create_info, nullptr, &shader_module);
	if (result != VK_SUCCESS)
		throw std::runtime_error(FormatVkResult(result));

	return shader_module;
}

VkShaderModule graphics::ShaderManager::CreateShaderModule(const std::string& file_name_, ShaderInputType input_type_, VkDevice device_)
{
	VkShaderModule shader_module;
	if (input_type_ == BinaryInput)
		shader_module = CreateShaderModule(LoadShaderBinary(file_name_), device_);
	else
		throw std::runtime_error("Shader code input is not yet supported");

	return shader_module;
}

VkVertexInputBindingDescription graphics::Vertex::GetBindingDescription()
{
	VkVertexInputBindingDescription binding_discription = {};

	binding_discription.binding = 0;
	binding_discription.stride = sizeof(Vertex);
	binding_discription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return binding_discription;
}

std::array<VkVertexInputAttributeDescription, 2> graphics::Vertex::GetAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions = {};

	attribute_descriptions[0].binding = 0;
	attribute_descriptions[0].location = 0;
	attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attribute_descriptions[0].offset = offsetof(Vertex, pos);
	attribute_descriptions[1].binding = 0;
	attribute_descriptions[1].location = 1;
	attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_descriptions[1].offset = offsetof(Vertex, color);

	return attribute_descriptions;
}
