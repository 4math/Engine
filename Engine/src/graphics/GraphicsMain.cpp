#include "GraphicsMain.h"

void graphics::GraphicsManager::Initialize()
{
	if (!InitializeVulkan())
		return;

	m_initialized = true;
}

void graphics::GraphicsManager::Shutdown()
{
	ShutdownSwapChain();

	vkDestroyBuffer(m_vk_device, m_vk_vertex_buffer, nullptr);
	vkFreeMemory(m_vk_device, m_vk_vertex_buffer_memory, nullptr);

	vkDestroyBuffer(m_vk_device, m_vk_index_buffer, nullptr);
	vkFreeMemory(m_vk_device, m_vk_index_buffer_memory, nullptr);

	for (size_t i = 0; i < m_max_frames_in_flight; i++)
	{
		vkDestroySemaphore(m_vk_device, m_semaphores_image_available[i], nullptr);
		vkDestroySemaphore(m_vk_device, m_semaphores_finished[i], nullptr);
		vkDestroyFence(m_vk_device, m_in_flight_fences[i], nullptr);
	}

	vkDestroyCommandPool(m_vk_device, m_vk_command_pool, nullptr);

	vkDestroyDevice(m_vk_device, nullptr);

	if (m_enable_validation_layers)
		DestroyDebugUtilsMessengerEXT(nullptr);

	vkDestroySurfaceKHR(m_vk_instance, m_vk_surface, nullptr);
	vkDestroyInstance(m_vk_instance, nullptr);

	m_initialized = false;
}

void graphics::GraphicsManager::ShutdownSwapChain()
{
	for (auto framebuffer : m_vk_swapchain_framebuffers)
		vkDestroyFramebuffer(m_vk_device, framebuffer, nullptr);

	vkFreeCommandBuffers(m_vk_device, m_vk_command_pool, static_cast<uint32_t>(m_vk_command_buffers.size()), m_vk_command_buffers.data());

	vkDestroyPipeline(m_vk_device, m_vk_graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(m_vk_device, m_vk_pipeline_layout, nullptr);
	vkDestroyRenderPass(m_vk_device, m_vk_render_pass, nullptr);

	for (auto image_view : m_vk_image_views)
		vkDestroyImageView(m_vk_device, image_view, nullptr);

	vkDestroySwapchainKHR(m_vk_device, m_vk_swapchain, nullptr);
}

bool graphics::GraphicsManager::InitializeVulkan()
{
#ifdef _DEBUG
	m_enable_validation_layers = true;
#endif
	m_shader_manager = std::make_shared<graphics::ShaderManager>("src/shaders/");
	try
	{
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFramebuffers();
		CreateCommandPool();
		CreateVertexBuffers();
		CreateIndexBuffers();
		CreateCommandBuffers();
		CreateSync();
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

	VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = m_app_name.c_str(),
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = m_engine_name.c_str(),
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0
	};

	auto extensions = GetRequiredExtensions();

	VkInstanceCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app_info,
		.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data()
	};

	if (m_enable_validation_layers) 
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
		create_info.ppEnabledLayerNames = m_validation_layers.data();
	}
	else 
		create_info.enabledLayerCount = 0;

	auto result = vkCreateInstance(&create_info, nullptr, &m_vk_instance);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create VkInstance, error: " + FormatVkResult(result));
}

void graphics::GraphicsManager::SetupDebugMessenger()
{
	if (!m_enable_validation_layers) return;

	VkDebugUtilsMessengerCreateInfoEXT create_info = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
			// Add VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT for full info
		.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = DebugCallback
	};
	

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
		VkDeviceQueueCreateInfo queue_create_info = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = queue_family,
			.queueCount = 1,
			.pQueuePriorities = &queue_priority
		};

		queue_create_infos.push_back(queue_create_info);
	}

	VkPhysicalDeviceFeatures device_features = {};

	VkDeviceCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
		.pQueueCreateInfos = queue_create_infos.data(),
		.enabledLayerCount = 0,
		.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size()),
		.ppEnabledExtensionNames = device_extensions.data(),
		.pEnabledFeatures = &device_features
	};
	
	auto result = vkCreateDevice(m_vk_physical_device, &create_info, nullptr, &m_vk_device);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create VkDevice, error: " + FormatVkResult(result));

	vkGetDeviceQueue(m_vk_device, indices.graphics_family.value(), 0, &m_vk_graphics_queue);
	vkGetDeviceQueue(m_vk_device, indices.present_family.value(), 0, &m_vk_present_queue);
}

void graphics::GraphicsManager::CreateSurface()
{
	VkWin32SurfaceCreateInfoKHR create_info = {
		.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		.hinstance = GetModuleHandle(nullptr),
		.hwnd = m_environment_manager->GetWindowHandle()
	};
	
	auto result = vkCreateWin32SurfaceKHR(m_vk_instance, &create_info, nullptr, &m_vk_surface);

	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create VkSurface, error: " + FormatVkResult(result));
}

void graphics::GraphicsManager::CreateSwapChain()
{
	SwapChainSupportDetails swapchain_support = QuerySwapChainSupport(m_vk_physical_device, m_vk_surface);

	VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swapchain_support.formats);
	VkPresentModeKHR present_mode = ChooseSwapPresentMode(swapchain_support.present_modes);
	VkExtent2D extent = ChooseSwapExtent(swapchain_support.capabilities, m_environment_manager->WindowWidth(), m_environment_manager->WindowHeight());

	uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;
	if (swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount)
		image_count = swapchain_support.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR create_info = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = m_vk_surface,
		.minImageCount = image_count,
		.imageFormat = surface_format.format,
		.imageColorSpace = surface_format.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.preTransform = swapchain_support.capabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = present_mode,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE
	};
	
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

	uint32_t swapchain_image_count = 0;
	vkGetSwapchainImagesKHR(m_vk_device, m_vk_swapchain, &swapchain_image_count, nullptr);
	m_vk_swapchain_images.resize(swapchain_image_count);
	vkGetSwapchainImagesKHR(m_vk_device, m_vk_swapchain, &swapchain_image_count, m_vk_swapchain_images.data());

	m_vk_swapchain_extent = extent;
	m_vk_swapchain_image_format = surface_format.format;
}

void graphics::GraphicsManager::CreateImageViews()
{
	m_vk_image_views.resize(m_vk_swapchain_images.size());

	for (int i = 0; i < m_vk_image_views.size(); i++)
	{
		VkImageViewCreateInfo create_info = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = m_vk_swapchain_images[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = m_vk_swapchain_image_format
		};
		
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.layerCount = 1;

		auto result = vkCreateImageView(m_vk_device, &create_info, nullptr, &m_vk_image_views[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to create imageView, error: " + FormatVkResult(result));
	}
}

void graphics::GraphicsManager::CreateRenderPass()
{
	VkAttachmentDescription color_attachment = {
		.format = m_vk_swapchain_image_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};
	
	// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment 
	// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain 
	// VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL : Images to be used as destination for a memory copy operation 

	VkAttachmentReference attachment_reference = {
		attachment_reference.attachment = 0,
		attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
	
	VkSubpassDescription subpass = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &attachment_reference
	};
	
	VkSubpassDependency dependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	};
	
	VkRenderPassCreateInfo render_pass_info = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &color_attachment,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency
	};
	
	auto result = vkCreateRenderPass(m_vk_device, &render_pass_info, nullptr, &m_vk_render_pass);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create VkRenderPass, error: " + FormatVkResult(result));
	}
}

void graphics::GraphicsManager::CreateGraphicsPipeline()
{
// Using normalized device coordinates 
// Compiled SPIR-V shaders 

// Shader module and stages creation 
	VkShaderModule vert_shader_module = m_shader_manager->CreateShaderModule("BasicVert.spv", BinaryInput, m_vk_device);
	VkShaderModule frag_shader_module = m_shader_manager->CreateShaderModule("BasicFrag.spv", BinaryInput, m_vk_device);

	VkPipelineShaderStageCreateInfo vert_shader_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = vert_shader_module,
		.pName = "main"
	};

	VkPipelineShaderStageCreateInfo frag_shader_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = frag_shader_module,
		.pName = "main"
	};
	
	VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_create_info, frag_shader_create_info };

// Vertex and input assembly 
	auto binding_description = Vertex::GetBindingDescription();
	auto attribute_descriptions = Vertex::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &binding_description,
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size()),
		.pVertexAttributeDescriptions = attribute_descriptions.data()
	};

	VkPipelineInputAssemblyStateCreateInfo input_assembly = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};

// Viewports and scissors 
	VkViewport viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)m_vk_swapchain_extent.width,
		.height = (float)m_vk_swapchain_extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	
	VkRect2D scissor = {
		.offset = { 0 ,0 },
		.extent = m_vk_swapchain_extent
	};
	
	VkPipelineViewportStateCreateInfo viewport_state = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor
	};
	
// Rasterizer 
	VkPipelineRasterizationStateCreateInfo rasterizer = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.lineWidth = 1.0f
	};
// Optional 
	rasterizer.depthBiasConstantFactor = 0.0f; 
	rasterizer.depthBiasClamp = 0.0f; 
	rasterizer.depthBiasSlopeFactor = 0.0f;

// Multisampling 
	VkPipelineMultisampleStateCreateInfo multisampling = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE
	};
// Optional 
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

// Color attachment and blending 
	VkPipelineColorBlendAttachmentState color_blend_attachment = {
		.blendEnable = VK_FALSE,
		.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT
	};
// Optional 
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo color_blending = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,	// Optional
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment
	};

// Pipeline layout 
	VkPipelineLayoutCreateInfo pipeline_layout_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO
	};

	auto pipeline_layout_creation_result = vkCreatePipelineLayout(m_vk_device, &pipeline_layout_info, nullptr, &m_vk_pipeline_layout);
	if (pipeline_layout_creation_result != VK_SUCCESS)
		throw std::runtime_error("Failed to create VkPipelineLayout, error: " + FormatVkResult(pipeline_layout_creation_result));

// Graphics pipeline creation 
	VkGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = shader_stages;
	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly;
	pipeline_info.pViewportState = &viewport_state;
	pipeline_info.pRasterizationState = &rasterizer;
	pipeline_info.pMultisampleState = &multisampling;
	pipeline_info.pColorBlendState = &color_blending;
	// Optional 
	pipeline_info.pDepthStencilState = nullptr;
	pipeline_info.pDynamicState = nullptr;
	// 
	pipeline_info.layout = m_vk_pipeline_layout;
	pipeline_info.renderPass = m_vk_render_pass;
	pipeline_info.subpass = 0;
	// Optional deriving from an existing pipeline 
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_info.basePipelineIndex = 0;
	// 

	auto graphics_pipeline_creation_result = vkCreateGraphicsPipelines(m_vk_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_vk_graphics_pipeline);
	if (graphics_pipeline_creation_result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create VkPipeline, error: " + FormatVkResult(graphics_pipeline_creation_result));
	}

	vkDestroyShaderModule(m_vk_device, frag_shader_module, nullptr);
	vkDestroyShaderModule(m_vk_device, vert_shader_module, nullptr);
}

void graphics::GraphicsManager::CreateFramebuffers()
{
	m_vk_swapchain_framebuffers.resize(m_vk_image_views.size());
	for (size_t i = 0; i < m_vk_image_views.size(); i++)
	{
		VkImageView attachments[] = { m_vk_image_views[i] };

		VkFramebufferCreateInfo frameduffer_info = {};
		frameduffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameduffer_info.renderPass = m_vk_render_pass;
		frameduffer_info.attachmentCount = 1;
		frameduffer_info.pAttachments = attachments;
		frameduffer_info.width = m_vk_swapchain_extent.width;
		frameduffer_info.height = m_vk_swapchain_extent.height;
		frameduffer_info.layers = 1;
		
		auto result = vkCreateFramebuffer(m_vk_device, &frameduffer_info, nullptr, &m_vk_swapchain_framebuffers[i]);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create VkFramebuffer, error: " + FormatVkResult(result));
		}
	}
}

void graphics::GraphicsManager::CreateCommandPool()
{
	QueueFamilyIndices queue_familiy_indices = FindQueueFamilies(m_vk_physical_device);

	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = queue_familiy_indices.graphics_family.value();
	pool_info.flags = 0;
	/*	VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: 
	Hint that command buffers are rerecorded with new commands very often 
	(may change memory allocation behavior)
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: 
	Allow command buffers to be rerecorded individually, 
	without this flag they all have to be reset together*/

	auto result = vkCreateCommandPool(m_vk_device, &pool_info, nullptr, &m_vk_command_pool);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create VkCommandPool, error: " + FormatVkResult(result));
}

void graphics::GraphicsManager::CreateVertexBuffers()
{
	VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;
	CreateBuffer(
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // Buffer can be used as source in a memory transfer operation 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer,
		staging_buffer_memory);

	void* data;
	vkMapMemory(m_vk_device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, vertices.data(), (size_t)buffer_size);
	vkUnmapMemory(m_vk_device, staging_buffer_memory);

	CreateBuffer(
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, // Buffer can be used as destination in a memory transfer operation 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_vk_vertex_buffer,
		m_vk_vertex_buffer_memory);

	CopyBuffer(staging_buffer, m_vk_vertex_buffer, buffer_size);

	vkDestroyBuffer(m_vk_device, staging_buffer, nullptr);
	vkFreeMemory(m_vk_device, staging_buffer_memory, nullptr);
}

// TODO: Change to Vulkan Memory Allocation! 
// https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator 

void graphics::GraphicsManager::CreateIndexBuffers()
{
	VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;
	CreateBuffer(
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // Buffer can be used as source in a memory transfer operation 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer,
		staging_buffer_memory);

	void* data;
	vkMapMemory(m_vk_device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, indices.data(), (size_t)buffer_size);
	vkUnmapMemory(m_vk_device, staging_buffer_memory);

	CreateBuffer(
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, // Buffer can be used as destination in a memory transfer operation 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_vk_index_buffer,
		m_vk_index_buffer_memory);

	CopyBuffer(staging_buffer, m_vk_index_buffer, buffer_size);

	vkDestroyBuffer(m_vk_device, staging_buffer, nullptr);
	vkFreeMemory(m_vk_device, staging_buffer_memory, nullptr);
}

void graphics::GraphicsManager::CreateCommandBuffers()
{
	m_vk_command_buffers.resize(m_vk_swapchain_framebuffers.size());

	VkCommandBufferAllocateInfo allocate_info = {};
	allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocate_info.commandPool = m_vk_command_pool;
	allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	//	VK_COMMAND_BUFFER_LEVEL_PRIMARY: 
	//Can be submitted to a queue for execution, but cannot be called from other command buffers.
	//	VK_COMMAND_BUFFER_LEVEL_SECONDARY : 
	//Cannot be submitted directly, but can be called from primary command buffers.
	allocate_info.commandBufferCount = (uint32_t)m_vk_command_buffers.size();

	auto allocate_result = vkAllocateCommandBuffers(m_vk_device, &allocate_info, m_vk_command_buffers.data());
	if (allocate_result != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate comand buffers, error: " + FormatVkResult(allocate_result));

	for (size_t i = 0; i < m_vk_command_buffers.size(); i++)
	{
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		//	VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: 
		//The command buffer will be rerecorded right after executing it once.
		//	VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT : 
		//This is a secondary command buffer that will be entirely within a single render pass.
		//	VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT : 
		//The command buffer can be resubmitted while it is also already pending execution.

		auto begin_result = vkBeginCommandBuffer(m_vk_command_buffers[i], &begin_info);
		if (begin_result != VK_SUCCESS)
			throw std::runtime_error("Failed to begin recordig command buffer, error: " + FormatVkResult(begin_result));

		VkRenderPassBeginInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = m_vk_render_pass;
		render_pass_info.framebuffer = m_vk_swapchain_framebuffers[i];
		render_pass_info.renderArea.offset = { 0,0 };
		render_pass_info.renderArea.extent = m_vk_swapchain_extent;

		VkClearValue clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color;

		vkCmdBeginRenderPass(m_vk_command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
		//	VK_SUBPASS_CONTENTS_INLINE: 
		//The render pass commands will be embedded in the primary command buffer itself
		//and no secondary command buffers will be executed.
		//	VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS : 
		//The render pass commands will be executed from secondary command buffers.

		vkCmdBindPipeline(m_vk_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_vk_graphics_pipeline);

		VkBuffer vertex_buffers[] = { m_vk_vertex_buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_vk_command_buffers[i], 0, 1, vertex_buffers, offsets);
		vkCmdBindIndexBuffer(m_vk_command_buffers[i], m_vk_index_buffer, 0, VK_INDEX_TYPE_UINT16);
	
		vkCmdDrawIndexed(m_vk_command_buffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(m_vk_command_buffers[i]);

		auto record_result = vkEndCommandBuffer(m_vk_command_buffers[i]);
		if (record_result != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer, error: " + FormatVkResult(record_result));
	}

}

void graphics::GraphicsManager::CreateSync()
{
	m_semaphores_image_available.resize(m_max_frames_in_flight);
	m_semaphores_finished.resize(m_max_frames_in_flight);
	m_in_flight_fences.resize(m_max_frames_in_flight);

	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info_ = {};
	fence_info_.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info_.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < m_max_frames_in_flight; i++)
	{
		auto image_avaliable_result = vkCreateSemaphore(m_vk_device, &semaphore_info, nullptr, &m_semaphores_image_available[i]);
		auto finished_result = vkCreateSemaphore(m_vk_device, &semaphore_info, nullptr, &m_semaphores_finished[i]);
		auto fence_result = vkCreateFence(m_vk_device, &fence_info_, nullptr, &m_in_flight_fences[i]);
		if (image_avaliable_result != VK_SUCCESS || finished_result != VK_SUCCESS || fence_result != VK_SUCCESS)
			throw std::runtime_error("Failed to create sync, errors: " +
				FormatVkResult(image_avaliable_result) + "\n" +
				FormatVkResult(finished_result) + "\n" +
				FormatVkResult(fence_result));
	}
}

void graphics::GraphicsManager::RecreateSwapChain()
{
	while (m_environment_manager->WindowHeight() == 0 || m_environment_manager->WindowWidth() == 0)
		glfwWaitEvents();

	WaitDevice();

	ShutdownSwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateFramebuffers();
	CreateCommandBuffers();
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

void graphics::GraphicsManager::CreateBuffer(VkDeviceSize size_, VkBufferUsageFlags usage_, VkMemoryPropertyFlags properties_, VkBuffer& buffer_, VkDeviceMemory& buffer_memory_)
{
	VkBufferCreateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size_;
	buffer_info.usage = usage_;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	buffer_info.flags = 0;

	auto result = vkCreateBuffer(m_vk_device, &buffer_info, nullptr, &buffer_);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create VkBuffer, error: " + FormatVkResult(result));

	VkMemoryRequirements mem_requirements;
	vkGetBufferMemoryRequirements(m_vk_device, buffer_, &mem_requirements);

	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	alloc_info.allocationSize = mem_requirements.size;
	alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits,
		properties_,
		m_vk_physical_device);

	result = vkAllocateMemory(m_vk_device, &alloc_info, nullptr, &buffer_memory_);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate vertex buffer memory, error: " + FormatVkResult(result));

	result = vkBindBufferMemory(m_vk_device, buffer_, buffer_memory_, 0);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to bind vertex buffer memory, error: " + FormatVkResult(result));
}

void graphics::GraphicsManager::CopyBuffer(VkBuffer src_buffer_, VkBuffer dst_buffer_, VkDeviceSize size_)
{
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = m_vk_command_pool;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(m_vk_device, &alloc_info, &command_buffer);

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(command_buffer, &begin_info);

	VkBufferCopy copy_region = {};
	copy_region.size = size_;
	vkCmdCopyBuffer(command_buffer, src_buffer_, dst_buffer_, 1, &copy_region);

	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	vkQueueSubmit(m_vk_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_vk_graphics_queue);

	vkFreeCommandBuffers(m_vk_device, m_vk_command_pool, 1, &command_buffer);
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
	vkWaitForFences(m_vk_device, 1, &m_in_flight_fences[m_current_frame], VK_TRUE, (std::numeric_limits<uint64_t>::max)());
	uint32_t image_index;
	auto acquire_result = vkAcquireNextImageKHR(m_vk_device, m_vk_swapchain, (std::numeric_limits<uint64_t>::max)(), 
		m_semaphores_image_available[m_current_frame], VK_NULL_HANDLE, &image_index);

	if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return;
	}
	else if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image, error : " + FormatVkResult(acquire_result));
	}

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore wait_semaphores[] = { m_semaphores_image_available[m_current_frame] };
	VkSemaphore signal_semaphores[] = { m_semaphores_finished[m_current_frame] };

	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &m_vk_command_buffers[image_index];

	vkResetFences(m_vk_device, 1, &m_in_flight_fences[m_current_frame]);
	
	auto submit_result = vkQueueSubmit(m_vk_graphics_queue, 1, &submit_info, m_in_flight_fences[m_current_frame]);
	if (submit_result != VK_SUCCESS)
		throw std::runtime_error("Failed to submit begin frame command buffer, error: " + FormatVkResult(submit_result));


	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;
	present_info.swapchainCount = 1;
	VkSwapchainKHR swap_chains[] = { m_vk_swapchain };
	present_info.pSwapchains = swap_chains;
	present_info.pImageIndices = &image_index;
	present_info.pResults = nullptr; // Optional, It's not necessary if only use a single swap chain 

	auto present_result = vkQueuePresentKHR(m_vk_present_queue, &present_info);
	if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR || m_environment_manager->ResizeState())
	{
		m_environment_manager->ResizeState() = false;
		RecreateSwapChain();
	}
	else if (present_result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain, error : " + FormatVkResult(present_result));
	}
}

void graphics::GraphicsManager::EndFrame()
{
	vkQueueWaitIdle(m_vk_present_queue);

	m_current_frame = (m_current_frame + 1) % m_max_frames_in_flight;
}

void graphics::GraphicsManager::WaitDevice()
{
	vkDeviceWaitIdle(m_vk_device);
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
