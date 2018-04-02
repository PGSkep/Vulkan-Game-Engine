#ifndef	SWAPCHAIN_H
#define SWAPCHAIN_H

#include "../../Common/Other/VkU.h"
#include "Device.h"
#include "GpuImage.h"
#include "TransferManager.h"

#include <string>
#include <Windows.h>

class Swapchain
{
public:
	// surface
	std::string		name;
	HINSTANCE		hInstance;
	HWND			hWnd;
	VkSurfaceKHR	surface;

	// swapchain
	VkSwapchainKHR handle;
	VkSurfaceFormatKHR surfaceFormat;
	VkExtent2D extent;

	// render pass
	VkRenderPass renderPass;

	// resources
	std::vector<VkImage> images;
	std::vector<VkImageView> views;
	GpuImage depthImage;
	std::vector<VkFramebuffer> framebuffers;

	// sync
	std::vector<VkFence> fences;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderDoneSemaphore;

	// record
	std::vector<VkCommandBuffer> commandBuffers;
	VkFence imageAvailable;
	uint32_t imageIndex = ~0U;

	std::vector<VkViewport>	viewports;
	std::vector<VkRect2D>	scissors;

	void InitSurface(const char* _name, const char* _title, WNDPROC _wndProc, int _width, int _height, VkInstance _instance)
	{
		/// OS Window
		name = _name;
		hInstance = GetModuleHandle(NULL);

		WNDCLASSEX winClassEx;
		winClassEx.cbSize = sizeof(WNDCLASSEX);
		winClassEx.style = CS_HREDRAW | CS_VREDRAW;
		winClassEx.lpfnWndProc = _wndProc;
		winClassEx.cbClsExtra = 0;
		winClassEx.cbWndExtra = 0;
		winClassEx.hInstance = hInstance;
		winClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		winClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
		winClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		winClassEx.lpszMenuName = NULL;
		winClassEx.lpszClassName = name.c_str();
		winClassEx.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&winClassEx))
			Error::Report("!RegisterClassEx");

		RECT rect = { 0, 0, 800, 600 };
		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		AdjustWindowRect(&rect, dwStyle, FALSE);

		hWnd = CreateWindowEx(0,
			name.c_str(),
			_title,
			dwStyle | WS_VISIBLE | WS_SYSMENU,
			0, 0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			NULL,
			NULL,
			hInstance,
			NULL);

		if (hWnd == NULL)
		{
			UnregisterClass(name.c_str(), hInstance);
			Error::Report("Failed to create a window");
		}

		ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);

		VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfoKHR;
		win32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		win32SurfaceCreateInfoKHR.pNext = nullptr;
		win32SurfaceCreateInfoKHR.flags = VK_RESERVED_FOR_FUTURE_USE;
		win32SurfaceCreateInfoKHR.hinstance = hInstance;
		win32SurfaceCreateInfoKHR.hwnd = hWnd;
		VkU::CreateWin32SurfaceKHR(_instance, &win32SurfaceCreateInfoKHR, surface);
	}
	void ShutDownSurface(VkInstance _instance)
	{
		vkDestroySurfaceKHR(_instance, surface, nullptr);

		UnregisterClass(name.c_str(), hInstance);
		DestroyWindow(hWnd);
	}

	void InitSwapchain(Device _device, uint32_t _targetImageCount, std::vector<VkCompositeAlphaFlagBitsKHR> _preferedCompositeAlphas, std::vector<VkPresentModeKHR> _preferedPresentModes, TransferManager* _transferManager, VkCommandPool _commandPool)
	{
		// HARDCODED
		/// SWAPCHAIN
		{
			VkSurfaceCapabilitiesKHR* surfaceCapabilities = &_device.physicalDevice->surfaceProperties[0].surfaceCapabilities;
			std::vector<VkSurfaceFormatKHR>* surfaceFormats = &_device.physicalDevice->surfaceProperties[0].surfaceFormats;
			std::vector<VkPresentModeKHR>* presentModes = &_device.physicalDevice->surfaceProperties[0].presentModes;

			if ((*surfaceFormats).size() == 1 && (*surfaceFormats)[0].format == VK_FORMAT_UNDEFINED)
				surfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			else
				surfaceFormat = (*surfaceFormats)[0];

			extent = surfaceCapabilities->currentExtent;

			VkSwapchainCreateInfoKHR swapchainCreateInfo;
			swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchainCreateInfo.pNext = nullptr;
			swapchainCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			swapchainCreateInfo.surface = surface;
			swapchainCreateInfo.minImageCount = _targetImageCount;
			swapchainCreateInfo.imageFormat = surfaceFormat.format;
			swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
			swapchainCreateInfo.imageExtent = extent;
			swapchainCreateInfo.imageArrayLayers = 1;
			swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCreateInfo.queueFamilyIndexCount = 0;
			swapchainCreateInfo.pQueueFamilyIndices = nullptr;
			swapchainCreateInfo.preTransform = surfaceCapabilities->currentTransform;
			swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
			swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
			swapchainCreateInfo.clipped = VK_TRUE;
			swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

			// image count
			if (swapchainCreateInfo.minImageCount > surfaceCapabilities->maxImageCount)
				swapchainCreateInfo.minImageCount = surfaceCapabilities->maxImageCount;
			else if (swapchainCreateInfo.minImageCount < surfaceCapabilities->minImageCount)
				swapchainCreateInfo.minImageCount = surfaceCapabilities->minImageCount;

			// width
			if (swapchainCreateInfo.imageExtent.width > surfaceCapabilities->maxImageExtent.width)
				swapchainCreateInfo.imageExtent.width = surfaceCapabilities->maxImageExtent.width;
			else if (swapchainCreateInfo.imageExtent.width < surfaceCapabilities->minImageExtent.width)
				swapchainCreateInfo.imageExtent.width = surfaceCapabilities->minImageExtent.width;

			// height
			if (swapchainCreateInfo.imageExtent.height > surfaceCapabilities->maxImageExtent.height)
				swapchainCreateInfo.imageExtent.height = surfaceCapabilities->maxImageExtent.height;
			else if (swapchainCreateInfo.imageExtent.height < surfaceCapabilities->minImageExtent.height)
				swapchainCreateInfo.imageExtent.height = surfaceCapabilities->minImageExtent.height;

			// composite alpha
			for (size_t i = 0; i != _preferedCompositeAlphas.size(); ++i)
			{
				if ((_preferedCompositeAlphas[i] & surfaceCapabilities->supportedCompositeAlpha) == _preferedCompositeAlphas[i])
				{
					swapchainCreateInfo.compositeAlpha = _preferedCompositeAlphas[i];
					break;
				}
			}

			// present mode
			for (size_t iPrefered = 0; iPrefered != _preferedPresentModes.size(); ++iPrefered)
			{
				for (size_t iMode = 0; iMode != (*presentModes).size(); ++iMode)
				{
					if ((*presentModes)[iMode] == _preferedPresentModes[iPrefered])
					{
						swapchainCreateInfo.presentMode = _preferedPresentModes[iPrefered];
						iPrefered = _preferedPresentModes.size() - 1;
						break;
					}
				}
			}

			VkU::CreateSwapchain(_device.handle, &swapchainCreateInfo, &handle);
		}

		/// RENDERPASS
		{
			VkAttachmentDescription attachmentDescriptions[2];
			VkAttachmentDescription* colorAttachmentDescription = &attachmentDescriptions[0];
			colorAttachmentDescription->flags = VK_FLAGS_NONE;
			colorAttachmentDescription->format = surfaceFormat.format;
			colorAttachmentDescription->samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachmentDescription->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachmentDescription->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachmentDescription->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentDescription->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachmentDescription->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachmentDescription->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			VkAttachmentDescription* depthAttachmentDescription = &attachmentDescriptions[1];
			depthAttachmentDescription->flags = VK_FLAGS_NONE;
			depthAttachmentDescription->format = _device.physicalDevice->depthFormat;
			depthAttachmentDescription->samples = VK_SAMPLE_COUNT_1_BIT;
			depthAttachmentDescription->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachmentDescription->storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachmentDescription->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachmentDescription->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachmentDescription->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachmentDescription->finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkAttachmentReference colorAttachmentReference;
			colorAttachmentReference.attachment = 0;
			colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			VkAttachmentReference depthAttachmentReference;
			depthAttachmentReference.attachment = 1;
			depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			VkSubpassDescription subpassDescription;
			subpassDescription.flags = VK_FLAGS_NONE;
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.inputAttachmentCount = 0;
			subpassDescription.pInputAttachments = nullptr;
			subpassDescription.colorAttachmentCount = 1;
			subpassDescription.pColorAttachments = &colorAttachmentReference;
			subpassDescription.pResolveAttachments = nullptr;
			subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
			subpassDescription.preserveAttachmentCount = 0;
			subpassDescription.pPreserveAttachments = nullptr;

			VkSubpassDependency subpassDependency;
			subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependency.dstSubpass = 0;
			subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependency.srcAccessMask = 0;
			subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependency.dependencyFlags = VK_FLAGS_NONE;

			VkRenderPassCreateInfo renderPassCreateInfo;
			renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassCreateInfo.pNext = nullptr;
			renderPassCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			renderPassCreateInfo.attachmentCount = 2;
			renderPassCreateInfo.pAttachments = attachmentDescriptions;
			renderPassCreateInfo.subpassCount = 1;
			renderPassCreateInfo.pSubpasses = &subpassDescription;
			renderPassCreateInfo.dependencyCount = 1;
			renderPassCreateInfo.pDependencies = &subpassDependency;
			VkU::CreateRenderPass(_device.handle, &renderPassCreateInfo, &renderPass);
		}

		/// RESOURCES
		{
			// images
			VkU::GetSwapchainImagesKHR(_device.handle, handle, images);

			// views
			VkImageViewCreateInfo imageViewCreateInfo;
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.pNext = nullptr;
			imageViewCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = surfaceFormat.format;
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;

			views.resize(images.size());
			for (size_t iView = 0; iView != views.size(); ++iView)
			{
				imageViewCreateInfo.image = images[iView];
				VkU::CreateImageView(_device.handle, &imageViewCreateInfo, &views[iView]);
			}

			// Depth
			depthImage.Init(_device, { extent.width, extent.height, 1 }, _device.physicalDevice->depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			// transition depth layout
			VkImageMemoryBarrier imageMemoryBarrier;
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.pNext = nullptr;
			imageMemoryBarrier.srcAccessMask = 0;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
			imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.image = depthImage.handle;
			imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
			imageMemoryBarrier.subresourceRange.levelCount = 1;
			imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
			imageMemoryBarrier.subresourceRange.layerCount = 1;
			vkCmdPipelineBarrier(_transferManager->commandBuffer, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

			// Framebuffers
			VkFramebufferCreateInfo framebufferCreateInfo;
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.pNext = nullptr;
			framebufferCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			framebufferCreateInfo.renderPass = renderPass;//*swapchains[iSwapchain].renderPass;
			framebufferCreateInfo.width = extent.width;
			framebufferCreateInfo.height = extent.height;
			framebufferCreateInfo.layers = 1;

			framebuffers.resize(views.size());
			for (size_t iFramebuffer = 0; iFramebuffer != framebuffers.size(); ++iFramebuffer)
			{
				std::vector<VkImageView> attachments;
				attachments = { views[iFramebuffer], depthImage.view };

				framebufferCreateInfo.attachmentCount = (uint32_t)attachments.size();
				framebufferCreateInfo.pAttachments = attachments.data();

				VkU::CreateFramebuffer(_device.handle, &framebufferCreateInfo, &framebuffers[iFramebuffer]);
			}
		}

		/// SYNC
		{
			// Fences
			VkFenceCreateInfo fenceCreateInfo;
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.pNext = nullptr;
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			fences.resize(images.size());
			for (size_t iFence = 0; iFence != views.size(); ++iFence)
				VkU::CreateFence(_device.handle, &fenceCreateInfo, &fences[iFence]);

			// Semaphores
			VkSemaphoreCreateInfo semaphoreCreateInfo;
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphoreCreateInfo.pNext = nullptr;
			semaphoreCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
			VkU::CreateSemaphore_(_device.handle, &semaphoreCreateInfo, &imageAvailableSemaphore);
			VkU::CreateSemaphore_(_device.handle, &semaphoreCreateInfo, &renderDoneSemaphore);
		}

		/// RECORD
		{
			// Command Buffer
			commandBuffers.resize(images.size());
			VkCommandBufferAllocateInfo commandBufferAllocateInfo;
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.pNext = nullptr;
			commandBufferAllocateInfo.commandPool = _commandPool;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
			commandBufferAllocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();
			VkU::AllocateCommandBuffers(_device.handle, &commandBufferAllocateInfo, commandBuffers.data());

			// Image available
			VkFenceCreateInfo fenceCreateInfo;
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.pNext = nullptr;
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			VkU::CreateFence(_device.handle, &fenceCreateInfo, &imageAvailable);

			// viewport
			viewports = { VkU::GetVkViewport(0.0f, 0.0f, (float)extent.width, (float)extent.height, 0.0f, 1.0f) };

			// scissor
			scissors = { VkU::GetVkRect2D({ 0, 0 }, extent) };
		}
	}
	void ShutDownSwapchain(VkDevice _device)
	{
		vkDestroySemaphore(_device, imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(_device, renderDoneSemaphore, nullptr);

		vkDestroyFence(_device, imageAvailable, nullptr);

		for (size_t i = 0; i != fences.size(); ++i)
			vkDestroyFence(_device, fences[i], nullptr);
		fences.clear();

		for (size_t i = 0; i != framebuffers.size(); ++i)
			vkDestroyFramebuffer(_device, framebuffers[i], nullptr);
		framebuffers.clear();

		depthImage.ShutDown(_device);

		for (size_t i = 0; i != views.size(); ++i)
			vkDestroyImageView(_device, views[i], nullptr);
		views.clear();

		vkDestroyRenderPass(_device, renderPass, nullptr);

		vkDestroySwapchainKHR(_device, handle, nullptr);
	}

	void PrepareToRecord(VkDevice _device)
	{
		VkU::WaitForFences(_device, 1, &imageAvailable, VK_TRUE, ~0U);
		VkU::ResetFences(_device, 1, &imageAvailable);
		VkU::AcquireNextImageKHR(_device, handle, ~0U, imageAvailableSemaphore, imageAvailable, &imageIndex);
	}
	uint32_t GetImageIndex()
	{
		return imageIndex;
	}

	VkCommandBuffer BeginRecording()
	{
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo;
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.pNext = nullptr;
		commandBufferInheritanceInfo.renderPass = renderPass;
		commandBufferInheritanceInfo.subpass = 0;
		commandBufferInheritanceInfo.framebuffer = framebuffers[imageIndex];
		commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
		commandBufferInheritanceInfo.queryFlags = 0;
		commandBufferInheritanceInfo.pipelineStatistics = 0;

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;
		VkU::BeginCommandBuffer(commandBuffers[imageIndex], &commandBufferBeginInfo);

		vkCmdSetViewport(commandBuffers[imageIndex], 0, (uint32_t)viewports.size(), viewports.data());
		vkCmdSetScissor(commandBuffers[imageIndex], 0, (uint32_t)scissors.size(), scissors.data());

		return commandBuffers[imageIndex];
	}

	void EndRecording()
	{
		VkU::EndCommandBuffer(commandBuffers[imageIndex]);
	}
};

#endif