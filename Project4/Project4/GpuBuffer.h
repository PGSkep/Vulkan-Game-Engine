#ifndef	GPU_BUFFER_H
#define GPU_BUFFER_H

#include "../../Common/Other/VkU.h"
#include "Device.h"

class GpuBuffer
{
public:
	VkBuffer		handle;
	VkDeviceMemory	memory;

	void Init(Device _device, VkDeviceSize _size, VkMemoryPropertyFlags _memoryPropertyFlags, VkBufferUsageFlags _usage)
	{
		VkBufferCreateInfo bufferCreateInfo;
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = VK_FLAGS_NONE;
		bufferCreateInfo.size = _size;
		bufferCreateInfo.usage = _usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;
		VkU::CreateBuffer(_device.handle, &bufferCreateInfo, &handle);

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(_device.handle, handle, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo;
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = VkU::FindMemoryType(_device.physicalDevice->memoryProperties, memoryRequirements.memoryTypeBits, _memoryPropertyFlags);
		VkU::AllocateMemory(_device.handle, &memoryAllocateInfo, &memory);

		VkU::BindBufferMemory(_device.handle, handle, memory, 0);
	}
	void ShutDown(VkDevice _device)
	{
		vkDestroyBuffer(_device, handle, nullptr);
		vkFreeMemory(_device, memory, nullptr);
	}
};

#endif