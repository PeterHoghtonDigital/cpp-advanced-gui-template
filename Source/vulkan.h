// This header contains the project's Vulkan implementation.

#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <iostream>
#include <iterator>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Vulkan
{
	inline VkInstance Instance = VK_NULL_HANDLE;
	inline VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
	inline VkDevice Device = VK_NULL_HANDLE;
	inline uint32_t QueueFamily = UINT32_MAX;
	inline VkQueue Queue = VK_NULL_HANDLE;
	inline VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
	inline ImGui_ImplVulkanH_Window MainWindowData{};
	inline constexpr uint32_t MINIMUM_IMAGE_COUNT = 3;
	inline bool SwapchainRebuild = false;

	// Print Vulkan errors
	inline void CheckVkResult(VkResult Result)
	{
		if (Result == VK_SUCCESS)
		{
			return;
		}

		std::cerr << "Vulkan error: VkResult = " << Result << '\n';

		if (Result < 0)
		{
			std::abort();
		}
	}

	// Check if a Vulkan feature is available
	inline bool IsExtensionAvailable(const ImVector<VkExtensionProperties>& Properties, const char* Extension)
	{
		for (const VkExtensionProperties& Property : Properties)
		{
			if (std::strcmp(Property.extensionName, Extension) == 0)
			{
				return true;
			}
		}
		return false;
	}

	// Find a suitable memory type on the selected graphics device
	inline uint32_t FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties)
	{
		VkPhysicalDeviceMemoryProperties MemoryProperties{};
		vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &MemoryProperties);

		for (uint32_t Index = 0; Index < MemoryProperties.memoryTypeCount; ++Index)
		{
			if ((TypeFilter & (1u << Index))
				&& (MemoryProperties.memoryTypes[Index].propertyFlags & Properties) == Properties)
			{
				return Index;
			}
		}

		std::cerr << "Vulkan error: Failed to find suitable memory type for selected device..." << '\n';
		std::abort();
	}

	// Initialize Vulkan
	inline bool Init(GLFWwindow* Window, VkPresentModeKHR VSync)
	{
		// Create Vulkan instance
		{
			VkApplicationInfo ApplicationInfo{};
			ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			ApplicationInfo.apiVersion = VK_API_VERSION_1_3;

			VkInstanceCreateInfo CreateInfo{};
			CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			CreateInfo.pApplicationInfo = &ApplicationInfo;

			uint32_t GlfwExtensionCount = 0;
			const char** GlfwExtensions = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);
			ImVector<const char*> InstanceExtensions;

			for (uint32_t Index = 0; Index < GlfwExtensionCount; ++Index)
			{
				InstanceExtensions.push_back(GlfwExtensions[Index]);
			}

			uint32_t ExtensionPropertyCount = 0;
			CheckVkResult(vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionPropertyCount, nullptr));
			ImVector<VkExtensionProperties> ExtensionProperties;
			ExtensionProperties.resize(ExtensionPropertyCount);
			CheckVkResult(
				vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionPropertyCount, ExtensionProperties.Data));

			if (IsExtensionAvailable(ExtensionProperties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
			{
				InstanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
			}

#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
			if (IsExtensionAvailable(ExtensionProperties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
			{
				InstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
				CreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
			}
#endif
			CreateInfo.enabledExtensionCount = static_cast<uint32_t>(InstanceExtensions.Size);
			CreateInfo.ppEnabledExtensionNames = InstanceExtensions.Data;
			CheckVkResult(vkCreateInstance(&CreateInfo, nullptr, &Instance));
		}

		// Create the GLFW surface
		if (glfwCreateWindowSurface(Instance, Window, nullptr, &MainWindowData.Surface) != VK_SUCCESS)
		{
			return false;
		}

		// Find a graphics device and prepare it for rendering
		{
			PhysicalDevice = ImGui_ImplVulkanH_SelectPhysicalDevice(Instance);

			if (PhysicalDevice == VK_NULL_HANDLE)
			{
				return false;
			}

			QueueFamily = ImGui_ImplVulkanH_SelectQueueFamilyIndex(PhysicalDevice);

			if (QueueFamily == static_cast<uint32_t>(-1))
			{
				return false;
			}

			VkBool32 Supported = VK_FALSE;

			CheckVkResult(
				vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, QueueFamily, MainWindowData.Surface, &Supported));

			if (Supported != VK_TRUE)
			{
				vkDestroySurfaceKHR(Instance, MainWindowData.Surface, nullptr);

				return false;
			}

			ImVector<const char*> DeviceExtensions;

			DeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

			uint32_t ExtensionPropertyCount = 0;

			CheckVkResult(
				vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionPropertyCount, nullptr));

			ImVector<VkExtensionProperties> ExtensionProperties;
			ExtensionProperties.resize(ExtensionPropertyCount);

			CheckVkResult(vkEnumerateDeviceExtensionProperties(
				PhysicalDevice, nullptr, &ExtensionPropertyCount, ExtensionProperties.Data));

#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
			if (IsExtensionAvailable(ExtensionProperties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
			{
				DeviceExtensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
			}
#endif

			const float QueuePriority = 1.0f;

			VkDeviceQueueCreateInfo QueueCreateInfo{};
			QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			QueueCreateInfo.queueFamilyIndex = QueueFamily;
			QueueCreateInfo.queueCount = 1;
			QueueCreateInfo.pQueuePriorities = &QueuePriority;

			VkDeviceCreateInfo CreateInfo{};
			CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			CreateInfo.queueCreateInfoCount = 1;
			CreateInfo.pQueueCreateInfos = &QueueCreateInfo;
			CreateInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.Size);
			CreateInfo.ppEnabledExtensionNames = DeviceExtensions.Data;

			CheckVkResult(vkCreateDevice(PhysicalDevice, &CreateInfo, nullptr, &Device));
			vkGetDeviceQueue(Device, QueueFamily, 0, &Queue);
		}

		// Create storage used by ImGui and textures
		{

			const VkDescriptorPoolSize PoolSizes[] = {
				{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE},
				{VK_DESCRIPTOR_TYPE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE},
			};

			VkDescriptorPoolCreateInfo CreateInfo{};
			CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			CreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			CreateInfo.maxSets = 0;
			for (const VkDescriptorPoolSize& PoolSize : PoolSizes)
			{
				CreateInfo.maxSets += PoolSize.descriptorCount;
			}
			CreateInfo.poolSizeCount = (uint32_t)IM_COUNTOF(PoolSizes);
			CreateInfo.pPoolSizes = PoolSizes;

			CheckVkResult(vkCreateDescriptorPool(Device, &CreateInfo, nullptr, &DescriptorPool));
		}

		// Create the window
		{
			const VkFormat RequestedFormats[] = {
				VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
			const VkColorSpaceKHR RequestedColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
			MainWindowData.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(PhysicalDevice, MainWindowData.Surface,
				RequestedFormats, std::size(RequestedFormats), RequestedColorSpace);

			MainWindowData.PresentMode = VSync;

			int Width = 0;
			int Height = 0;
			glfwGetFramebufferSize(Window, &Width, &Height);
			ImGui_ImplVulkanH_CreateOrResizeWindow(Instance, PhysicalDevice, Device, &MainWindowData, QueueFamily,
				nullptr, Width, Height, MINIMUM_IMAGE_COUNT, 0);
		}

		return true;
	}

	// Render the frame
	inline void Render(const std::function<void(VkCommandBuffer)>& RenderCallback, int ViewportWidth,
		int ViewportHeight, float Red, float Green, float Blue, float Alpha)
	{
		// Force Vulkan to rebuild the frame if the window size changed
		if (SwapchainRebuild || MainWindowData.Width != ViewportWidth || MainWindowData.Height != ViewportHeight)
		{
			ImGui_ImplVulkan_SetMinImageCount(MINIMUM_IMAGE_COUNT);
			ImGui_ImplVulkanH_CreateOrResizeWindow(Instance, PhysicalDevice, Device, &MainWindowData, QueueFamily,
				nullptr, ViewportWidth, ViewportHeight, MINIMUM_IMAGE_COUNT, 0);
			MainWindowData.FrameIndex = 0;
			SwapchainRebuild = false;
		}

		// Set the background colour
		MainWindowData.ClearValue.color.float32[0] = Red * Alpha;
		MainWindowData.ClearValue.color.float32[1] = Green * Alpha;
		MainWindowData.ClearValue.color.float32[2] = Blue * Alpha;
		MainWindowData.ClearValue.color.float32[3] = Alpha;

		// Get the resources used to build the current frame
		ImGui_ImplVulkanH_Window& WindowData = MainWindowData;
		VkSemaphore ImageAcquiredSemaphore =
			WindowData.FrameSemaphores[WindowData.SemaphoreIndex].ImageAcquiredSemaphore;
		VkSemaphore RenderCompleteSemaphore =
			WindowData.FrameSemaphores[WindowData.SemaphoreIndex].RenderCompleteSemaphore;
		VkResult Result = vkAcquireNextImageKHR(
			Device, WindowData.Swapchain, UINT64_MAX, ImageAcquiredSemaphore, VK_NULL_HANDLE, &WindowData.FrameIndex);

		// Force Vulkan to rebuild the frame before rendering if the frame is no longer valid
		if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
		{
			SwapchainRebuild = true;
			return;
		}

		CheckVkResult(Result);

		// Wait for the previous frame to finish
		ImGui_ImplVulkanH_Frame& Frame = WindowData.Frames[WindowData.FrameIndex];

		CheckVkResult(vkWaitForFences(Device, 1, &Frame.Fence, VK_TRUE, UINT64_MAX));
		CheckVkResult(vkResetFences(Device, 1, &Frame.Fence));
		CheckVkResult(vkResetCommandPool(Device, Frame.CommandPool, 0));

		// Build the current frame
		VkCommandBufferBeginInfo CommandBufferBeginInfo{};
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		CheckVkResult(vkBeginCommandBuffer(Frame.CommandBuffer, &CommandBufferBeginInfo));

		VkRenderPassBeginInfo RenderPassBeginInfo{};
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.renderPass = WindowData.RenderPass;
		RenderPassBeginInfo.framebuffer = Frame.Framebuffer;
		RenderPassBeginInfo.renderArea.extent.width = WindowData.Width;
		RenderPassBeginInfo.renderArea.extent.height = WindowData.Height;
		RenderPassBeginInfo.clearValueCount = 1;
		RenderPassBeginInfo.pClearValues = &WindowData.ClearValue;

		vkCmdBeginRenderPass(Frame.CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Flip the viewport vertically to match OpenGL
		// Note: To use Vulkan's default viewport set y to '0' and make height positive.
		VkViewport Viewport{};
		Viewport.x = 0.0f;
		Viewport.y = static_cast<float>(WindowData.Height);
		Viewport.width = static_cast<float>(WindowData.Width);
		Viewport.height = -static_cast<float>(WindowData.Height);
		Viewport.minDepth = 0.0f;
		Viewport.maxDepth = 1.0f;
		vkCmdSetViewport(Frame.CommandBuffer, 0, 1, &Viewport);

		VkRect2D Scissor{};
		Scissor.offset = {.x = 0, .y = 0};
		Scissor.extent.width = WindowData.Width;
		Scissor.extent.height = WindowData.Height;
		vkCmdSetScissor(Frame.CommandBuffer, 0, 1, &Scissor);

		// Render shaders and geometry
		if (RenderCallback)
		{
			RenderCallback(Frame.CommandBuffer);
		}

		// Renders the UI windows inside the main application window
		ImDrawData* DrawData = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(DrawData, Frame.CommandBuffer);

		vkCmdEndRenderPass(Frame.CommandBuffer);
		CheckVkResult(vkEndCommandBuffer(Frame.CommandBuffer));

		const VkPipelineStageFlags WaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo SubmitInfo{};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.waitSemaphoreCount = 1;
		SubmitInfo.pWaitSemaphores = &ImageAcquiredSemaphore;
		SubmitInfo.pWaitDstStageMask = &WaitStage;
		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &Frame.CommandBuffer;
		SubmitInfo.signalSemaphoreCount = 1;
		SubmitInfo.pSignalSemaphores = &RenderCompleteSemaphore;

		CheckVkResult(vkQueueSubmit(Queue, 1, &SubmitInfo, Frame.Fence));

		VkPresentInfoKHR PresentInfo{};
		PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		PresentInfo.waitSemaphoreCount = 1;
		PresentInfo.pWaitSemaphores = &RenderCompleteSemaphore;
		PresentInfo.swapchainCount = 1;
		PresentInfo.pSwapchains = &WindowData.Swapchain;
		PresentInfo.pImageIndices = &WindowData.FrameIndex;

		// Swap buffers
		const VkResult PresentResult = vkQueuePresentKHR(Queue, &PresentInfo);

		// Force Vulkan to rebuild next frame if the window changed while rendering the current frame
		if (PresentResult == VK_ERROR_OUT_OF_DATE_KHR || PresentResult == VK_SUBOPTIMAL_KHR)
		{
			SwapchainRebuild = true;
		}
		else
		{
			CheckVkResult(PresentResult);
		}

		// Set the resources used to build the next frame
		WindowData.SemaphoreIndex = (WindowData.SemaphoreIndex + 1) % WindowData.SemaphoreCount;
	}

	// Destroy Vulkan
	inline void Shutdown()
	{
		ImGui_ImplVulkanH_DestroyWindow(Instance, Device, &MainWindowData, nullptr);
		vkDestroyDescriptorPool(Device, DescriptorPool, nullptr);
		vkDestroySurfaceKHR(Instance, MainWindowData.Surface, nullptr);
		vkDestroyDevice(Device, nullptr);
		vkDestroyInstance(Instance, nullptr);
	}
};
