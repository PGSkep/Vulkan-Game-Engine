#ifndef	TRANSFER_MANAGER_H
#define TRANSFER_MANAGER_H

#include "../../Common/Other/VkU.h"
#include "GpuBuffer.h"

class TransferManager
{
public:
	VkQueue			queue;
	VkCommandBuffer	commandBuffer;
	VkFence			fence;
	VkSemaphore		semaphore;

	VkDeviceSize	transferBufferSize;
	GpuBuffer		transferBuffer;

	void Init(Device _device, VkCommandBuffer _commandBuffer, VkQueue _queue, VkDeviceSize _transferBufferSize)
	{
		queue = _queue;
		commandBuffer = _commandBuffer;

		transferBufferSize = _transferBufferSize;

		VkFenceCreateInfo fenceCreateInfo;
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VkU::CreateFence(_device.handle, &fenceCreateInfo, &fence);

		VkSemaphoreCreateInfo semaphoreCreateInfo;
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		VkU::CreateSemaphore_(_device.handle, &semaphoreCreateInfo, &semaphore);

		transferBuffer.Init(_device, transferBufferSize, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	}
	void ShutDown(VkDevice _device)
	{
		transferBuffer.ShutDown(_device);

		vkDestroyFence(_device, fence, nullptr);
		vkDestroySemaphore(_device, semaphore, nullptr);
	}

	void TransferBuffer(VkDevice _device, void* _data, size_t _dataSize, VkBuffer _target, VkDeviceSize _targetOffset)
	{
		VkU::WaitForFences(_device, 1, &fence, VK_TRUE, ~0U);
		VkU::ResetFences(_device, 1, &fence);

		/// TODO: Implement transfering data larger than transferBufferSize by making multiple transfers

		void* data = nullptr;
		VkU::MapMemory(_device, transferBuffer.memory, 0, _dataSize, &data);
		memcpy((void*)((size_t)data), _data, _dataSize);
		vkUnmapMemory(_device, transferBuffer.memory);

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;
		VkU::BeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		VkBufferCopy bufferCopy;
		bufferCopy.srcOffset = 0;
		bufferCopy.dstOffset = _targetOffset;
		bufferCopy.size = _dataSize;
		vkCmdCopyBuffer(commandBuffer, transferBuffer.handle, _target, 1, &bufferCopy);

		VkU::EndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;
		VkU::QueueSubmit(queue, 1, &submitInfo, fence);
	}

	void WaitTransfer(VkDevice _device)
	{
		VkU::WaitForFences(_device, 1, &fence, VK_TRUE, ~0U);
	}

	void BeginCommandBuffer()
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;
		VkU::BeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
	}
	void EndCommandBuffer()
	{
		VkU::EndCommandBuffer(commandBuffer);
	}
};

#endif