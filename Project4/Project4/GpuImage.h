#ifndef	GPU_IMAGE_H
#define GPU_IMAGE_H

#include "../../Common/Other/VkU.h"
#include "Device.h"

class GpuImage
{
public:
	VkImage			handle;
	VkDeviceMemory	memory;
	VkImageView		view;

	void Init(Device _device, VkExtent3D _extent, VkFormat _format, VkImageUsageFlags _usage, VkImageAspectFlags _aspectMask, VkMemoryPropertyFlags _memoryPropertyFlags)
	{
		VkImageCreateInfo imageCreateInfo;
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = nullptr;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = _format;
		imageCreateInfo.extent = _extent;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = _usage;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = nullptr;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
		VkU::CreateImage(_device.handle, &imageCreateInfo, &handle);

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(_device.handle, handle, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo;
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = VkU::FindMemoryType(_device.physicalDevice->memoryProperties, memoryRequirements.memoryTypeBits, _memoryPropertyFlags);
		VkU::AllocateMemory(_device.handle, &memoryAllocateInfo, &memory);

		VkU::BindImageMemory(_device.handle, handle, memory, 0);

		VkImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		imageViewCreateInfo.image = handle;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = _format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = _aspectMask;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		VkU::CreateImageView(_device.handle, &imageViewCreateInfo, &view);
	}
	void ShutDown(VkDevice _device)
	{
		vkDestroyImageView(_device, view, nullptr);
		vkDestroyImage(_device, handle, nullptr);
		vkFreeMemory(_device, memory, nullptr);
	}
};

#endif