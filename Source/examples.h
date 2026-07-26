#pragma once

#include <cstdint>
#include <cstring>
#include <fstream>
#include <imgui.h>
#include <ios>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "vulkan.h" // This header contains the project's Vulkan implementation

namespace Examples
{
	// Simple triangle mesh for testing Vulkan
	struct TriangleMesh
	{
		// Geometry
		static constexpr float VERTICES[9] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

		VkPipeline Pipeline = VK_NULL_HANDLE;
		VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
		VkDeviceMemory VertexMemory = VK_NULL_HANDLE;
		VkBuffer VertexBuffer = VK_NULL_HANDLE;

		// Initialize shaders and geometry
		TriangleMesh(const char* VertexShaderPath, const char* FragmentShaderPath)
		{
			// Load Vertex Shader
			std::ifstream VertexShaderFStream(std::string(VertexShaderPath) + ".spv", std::ios::ate | std::ios::binary);
			const std::streamsize VertexShaderSize = VertexShaderFStream.tellg();
			std::vector<uint32_t> VertexShaderSource(static_cast<size_t>(VertexShaderSize) / sizeof(uint32_t));
			VertexShaderFStream.seekg(0);
			VertexShaderFStream.read(reinterpret_cast<char*>(VertexShaderSource.data()), VertexShaderSize);

			// Load Fragment Shader
			std::ifstream FragmentShaderFStream(
				std::string(FragmentShaderPath) + ".spv", std::ios::ate | std::ios::binary);
			const std::streamsize FragmentShaderSize = FragmentShaderFStream.tellg();
			std::vector<uint32_t> FragmentShaderSource(static_cast<size_t>(FragmentShaderSize) / sizeof(uint32_t));
			FragmentShaderFStream.seekg(0);
			FragmentShaderFStream.read(reinterpret_cast<char*>(FragmentShaderSource.data()), FragmentShaderSize);

			// Create shaders
			VkShaderModuleCreateInfo VertexShaderCreateInfo{};
			VertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			VertexShaderCreateInfo.codeSize = VertexShaderSource.size() * sizeof(uint32_t);
			VertexShaderCreateInfo.pCode = VertexShaderSource.data();
			VkShaderModule VertexShader = VK_NULL_HANDLE;
			Vulkan::CheckVkResult(
				vkCreateShaderModule(Vulkan::Device, &VertexShaderCreateInfo, nullptr, &VertexShader));

			VkShaderModuleCreateInfo FragmentShaderCreateInfo{};
			FragmentShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			FragmentShaderCreateInfo.codeSize = FragmentShaderSource.size() * sizeof(uint32_t);
			FragmentShaderCreateInfo.pCode = FragmentShaderSource.data();
			VkShaderModule FragmentShader = VK_NULL_HANDLE;
			Vulkan::CheckVkResult(
				vkCreateShaderModule(Vulkan::Device, &FragmentShaderCreateInfo, nullptr, &FragmentShader));

			// Create graphics pipeline (link shaders)
			VkPipelineShaderStageCreateInfo VertexStage{};
			VertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			VertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
			VertexStage.module = VertexShader;
			VertexStage.pName = "main";

			VkPipelineShaderStageCreateInfo FragmentStage{};
			FragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			FragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			FragmentStage.module = FragmentShader;
			FragmentStage.pName = "main";

			const VkPipelineShaderStageCreateInfo ShaderStages[] = {VertexStage, FragmentStage};

			VkVertexInputBindingDescription BindingDescription{};
			BindingDescription.binding = 0;
			BindingDescription.stride = 3 * sizeof(float);
			BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			VkVertexInputAttributeDescription AttributeDescription{};
			AttributeDescription.location = 0;
			AttributeDescription.binding = 0;
			AttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
			AttributeDescription.offset = 0;

			VkPipelineVertexInputStateCreateInfo VertexInput{};
			VertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			VertexInput.vertexBindingDescriptionCount = 1;
			VertexInput.pVertexBindingDescriptions = &BindingDescription;
			VertexInput.vertexAttributeDescriptionCount = 1;
			VertexInput.pVertexAttributeDescriptions = &AttributeDescription;

			VkPipelineInputAssemblyStateCreateInfo InputAssembly{};
			InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

			VkPipelineViewportStateCreateInfo ViewportState{};
			ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			ViewportState.viewportCount = 1;
			ViewportState.scissorCount = 1;

			VkPipelineRasterizationStateCreateInfo Rasterizer{};
			Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			Rasterizer.cullMode = VK_CULL_MODE_NONE;
			// Flip the frontface to match OpenGL
			// Note: To use Vulkan's default frontface set to VK_FRONT_FACE_COUNTER_CLOCKWISE.
			Rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
			Rasterizer.lineWidth = 1.0f;

			VkPipelineMultisampleStateCreateInfo Multisampling{};
			Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

			VkPipelineColorBlendAttachmentState ColorBlendAttachment{};
			ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
												  | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

			VkPipelineColorBlendStateCreateInfo ColorBlending{};
			ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			ColorBlending.attachmentCount = 1;
			ColorBlending.pAttachments = &ColorBlendAttachment;

			const VkDynamicState DynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

			VkPipelineDynamicStateCreateInfo DynamicState{};
			DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			DynamicState.dynamicStateCount = 2;
			DynamicState.pDynamicStates = DynamicStates;

			VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
			PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

			Vulkan::CheckVkResult(
				vkCreatePipelineLayout(Vulkan::Device, &PipelineLayoutInfo, nullptr, &PipelineLayout));

			VkGraphicsPipelineCreateInfo PipelineInfo{};
			PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			PipelineInfo.stageCount = 2;
			PipelineInfo.pStages = ShaderStages;
			PipelineInfo.pVertexInputState = &VertexInput;
			PipelineInfo.pInputAssemblyState = &InputAssembly;
			PipelineInfo.pViewportState = &ViewportState;
			PipelineInfo.pRasterizationState = &Rasterizer;
			PipelineInfo.pMultisampleState = &Multisampling;
			PipelineInfo.pColorBlendState = &ColorBlending;
			PipelineInfo.pDynamicState = &DynamicState;
			PipelineInfo.layout = PipelineLayout;
			PipelineInfo.renderPass = Vulkan::MainWindowData.RenderPass;
			PipelineInfo.subpass = 0;

			Vulkan::CheckVkResult(
				vkCreateGraphicsPipelines(Vulkan::Device, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &Pipeline));

			vkDestroyShaderModule(Vulkan::Device, FragmentShader, nullptr);
			vkDestroyShaderModule(Vulkan::Device, VertexShader, nullptr);

			// Create geometry
			VkBufferCreateInfo BufferInfo{};
			BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			BufferInfo.size = sizeof(VERTICES);
			BufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			Vulkan::CheckVkResult(vkCreateBuffer(Vulkan::Device, &BufferInfo, nullptr, &VertexBuffer));

			VkMemoryRequirements MemoryRequirements{};
			vkGetBufferMemoryRequirements(Vulkan::Device, VertexBuffer, &MemoryRequirements);

			VkMemoryAllocateInfo AllocationInfo{};
			AllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			AllocationInfo.allocationSize = MemoryRequirements.size;
			AllocationInfo.memoryTypeIndex = Vulkan::FindMemoryType(MemoryRequirements.memoryTypeBits,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			Vulkan::CheckVkResult(vkAllocateMemory(Vulkan::Device, &AllocationInfo, nullptr, &VertexMemory));
			Vulkan::CheckVkResult(vkBindBufferMemory(Vulkan::Device, VertexBuffer, VertexMemory, 0));

			void* Data = nullptr;
			Vulkan::CheckVkResult(vkMapMemory(Vulkan::Device, VertexMemory, 0, sizeof(VERTICES), 0, &Data));
			std::memcpy(Data, VERTICES, sizeof(VERTICES));
			vkUnmapMemory(Vulkan::Device, VertexMemory);
		}

		// Render shaders and geometry
		void Render(VkCommandBuffer CommandBuffer) const
		{
			const VkDeviceSize Offset = 0;
			vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);
			vkCmdBindVertexBuffers(CommandBuffer, 0, 1, &VertexBuffer, &Offset);
			vkCmdDraw(CommandBuffer, 3, 1, 0, 0);
		}

		// Destroy shaders and geometry
		void Destroy() const
		{
			vkDestroyPipeline(Vulkan::Device, Pipeline, nullptr);
			vkDestroyPipelineLayout(Vulkan::Device, PipelineLayout, nullptr);
			vkDestroyBuffer(Vulkan::Device, VertexBuffer, nullptr);
			vkFreeMemory(Vulkan::Device, VertexMemory, nullptr);
		}
	};

	// Simple window for testing ImGui
	inline void CreateImGuiWindow(const char* Title, const char* Text, float PosX, float PosY, float SizeX, float SizeY)
	{
		ImGui::SetNextWindowPos(ImVec2(PosX, PosY), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(SizeX, SizeY), ImGuiCond_FirstUseEver);
		ImGui::Begin(Title);
		ImGui::TextWrapped("%s", Text);
		ImGui::End();
	}
}
