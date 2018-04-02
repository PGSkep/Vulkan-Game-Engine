#ifndef	GPU_SWAPCHAIN_MANAGER_H
#define GPU_SWAPCHAIN_MANAGER_H

#include "../../Common/Other/VkU.h"

class GpuSwapchainManager
{
public:
	Swapchain* swapchain;

	void Init(Device _device, Swapchain* _swapchain, TransferManager* _transferManager, VkCommandPool _commandPool)
	{
		swapchain = _swapchain;

		uint32_t targetImageCount = 3;
		std::vector<VkCompositeAlphaFlagBitsKHR> preferedCompositeAlphas = { VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR };
		std::vector<VkPresentModeKHR> preferedPresentModes = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR };

		swapchain->InitSwapchain(_device, targetImageCount, preferedCompositeAlphas, preferedPresentModes, _transferManager, _commandPool);
	}
	void ShutDown(VkDevice _device)
	{
		swapchain->ShutDownSwapchain(_device);
		swapchain = nullptr;
	}
};
#endif