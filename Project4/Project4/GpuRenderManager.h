#ifndef	GPU_RENDER_MANAGER_H
#define GPU_RENDER_MANAGER_H

#include "../../Common/Other/VkU.h"

#include "GpuPipelineManager.h"
#include "GpuDescriptorManager.h"
#include "GpuMemoryManager.h"

#include <map>
#include <string>
#include <vector>

class GpuRenderManager
{
	VkFence renderDone;
	VkCommandBuffer submitCommandBuffer;
	VkQueue renderQueue;

public:
	void Init(Device _device, VkCommandPool _commandPool, VkQueue _queue)
	{
		VkFenceCreateInfo fenceCreateInfo;
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VkU::CreateFence(_device.handle, &fenceCreateInfo, &renderDone);

		VkCommandBufferAllocateInfo commandBufferAllocateInfo;
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = _commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;
		VkU::AllocateCommandBuffers(_device.handle, &commandBufferAllocateInfo, &submitCommandBuffer);

		renderQueue = _queue;
	}
	void ShutDown(VkDevice _device)
	{
		vkDestroyFence(_device, renderDone, nullptr);
	}

	// static mesh
	struct StaticMeshInfo
	{
		std::string vertexInstance;
		std::string vertexPosition;
		std::string vertexOther;

		std::string indice;

		std::string descriptorSet1;

		uint32_t instanceCount = 0;
	};
	struct StaticMeshPipelineInfo
	{
		std::string	pipeline;
		std::string	pipelineLayout;
		std::string	descriptorSet0;
	};
	struct StaticMeshPipeline
	{
		StaticMeshPipelineInfo staticMeshPipelineInfo;

		std::map<std::string, StaticMeshInfo> staticMeshes;
	};
	std::map<std::string, StaticMeshPipeline> staticMeshPipelines;

	//void RecordUi();
	//void RecordParticle();
	void RecordStaticMeshes(VkCommandBuffer _commandBuffer, GpuPipelineManager* _pipelineManager, GpuDescriptorManager* _descriptorManager, GpuMemoryManager* _gpuMemoryManager)
	{
		for (std::map<std::string, StaticMeshPipeline>::iterator iterPipeline = staticMeshPipelines.begin(); iterPipeline != staticMeshPipelines.end(); ++iterPipeline)
		{
			VkPipeline pipeline = _pipelineManager->GetGraphicsPipeline(iterPipeline->second.staticMeshPipelineInfo.pipeline.c_str());
			vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

			VkPipelineLayout pipelineLayout = _pipelineManager->GetPipelineLayout(iterPipeline->second.staticMeshPipelineInfo.pipelineLayout.c_str());
			VkDescriptorSet descriptorSet0 = _descriptorManager->GetDescriptorSetHandle(0, iterPipeline->second.staticMeshPipelineInfo.descriptorSet0.c_str());
			vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet0, 0, nullptr);

			for (std::map<std::string, StaticMeshInfo>::iterator iterMesh = iterPipeline->second.staticMeshes.begin(); iterMesh != iterPipeline->second.staticMeshes.end(); ++iterMesh)
			{
				GpuMemoryManager::VerticeBufferInfo vertexInstance = _gpuMemoryManager->GetVerticeBufferInfo(iterMesh->second.vertexInstance.c_str());
				GpuMemoryManager::VerticeBufferInfo vertexPosition = _gpuMemoryManager->GetVerticeBufferInfo(iterMesh->second.vertexPosition.c_str());
				GpuMemoryManager::VerticeBufferInfo vertexOther = _gpuMemoryManager->GetVerticeBufferInfo(iterMesh->second.vertexOther.c_str());

				VkBuffer vertexBuffers[] = { vertexInstance.buffer, vertexPosition.buffer, vertexOther.buffer };
				VkDeviceSize vertexOffsets[] = { vertexInstance.offset, vertexPosition.offset, vertexOther.offset };

				//VkBuffer vertexBuffers[] = { vertexPosition.buffer, vertexOther.buffer };
				//VkDeviceSize vertexOffsets[] = { vertexPosition.offset, vertexOther.offset };

				vkCmdBindVertexBuffers(_commandBuffer, 0, sizeof(vertexBuffers) / sizeof(VkBuffer), vertexBuffers, vertexOffsets);

				GpuMemoryManager::IndiceBufferInfo index = _gpuMemoryManager->GetIndiceBufferInfo(iterMesh->second.indice.c_str());
				vkCmdBindIndexBuffer(_commandBuffer, index.buffer, index.offset, VK_INDEX_TYPE_UINT32);

				//VkDescriptorSet descriptorSet1 = _recordModelInfo.descriptorManager->GetDescriptorSetHandle(1, iterMesh->second.descriptorSet1.c_str());
				//vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &descriptorSet1, 0, nullptr);

				vkCmdDrawIndexed(_commandBuffer, index.count, iterMesh->second.instanceCount, 0, 0, 0);
			}
		}
	}

	struct AddStaticMeshInfo
	{
		const char* staticMeshPipelineName;
		StaticMeshPipelineInfo staticMeshPipelineInfo;
		const char* staticMeshName;
		StaticMeshInfo staticMeshInfo;
	};
	void AddStaticMeshes(AddStaticMeshInfo _addStaticMeshInfo)
	{
		std::map<std::string, StaticMeshPipeline>::iterator pipelineIter = staticMeshPipelines.find(_addStaticMeshInfo.staticMeshPipelineName);
		if (pipelineIter != staticMeshPipelines.end())
		{
			std::map<std::string, StaticMeshInfo>::iterator meshIter = pipelineIter->second.staticMeshes.find(_addStaticMeshInfo.staticMeshName);
			if (meshIter != pipelineIter->second.staticMeshes.end())
			{
				++meshIter->second.instanceCount;
			}
			else
			{
				pipelineIter->second.staticMeshes[_addStaticMeshInfo.staticMeshName] = _addStaticMeshInfo.staticMeshInfo;
			}
		}
		else
		{
			StaticMeshPipeline staticMeshPipeline;
			staticMeshPipeline.staticMeshPipelineInfo = _addStaticMeshInfo.staticMeshPipelineInfo;
			staticMeshPipeline.staticMeshes[_addStaticMeshInfo.staticMeshName] = _addStaticMeshInfo.staticMeshInfo;

			staticMeshPipelines[_addStaticMeshInfo.staticMeshPipelineName] = staticMeshPipeline;
		}
	}

	void Render(VkDevice _device, Swapchain* _swapchain)
	{
		VkU::WaitForFences(_device, 1, &renderDone, VK_TRUE, ~0U);
		VkU::ResetFences(_device, 1, &renderDone);

		uint32_t imageIndex = _swapchain->GetImageIndex();

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;
		VkU::BeginCommandBuffer(submitCommandBuffer, &commandBufferBeginInfo);

		VkClearValue clearColor[2];
		clearColor[0].color = { 0.1f, 0.2f, 0.3f, 0.0f };
		clearColor[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo;
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = _swapchain->renderPass;
		renderPassBeginInfo.framebuffer = _swapchain->framebuffers[imageIndex];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = _swapchain->extent;
		renderPassBeginInfo.clearValueCount = sizeof(clearColor) / sizeof(VkClearValue);
		renderPassBeginInfo.pClearValues = clearColor;

		VkCommandBuffer commandBuffer = _swapchain->commandBuffers[imageIndex];
		vkCmdBeginRenderPass(submitCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		vkCmdExecuteCommands(submitCommandBuffer, 1, &commandBuffer);
		vkCmdEndRenderPass(submitCommandBuffer);

		VkU::EndCommandBuffer(submitCommandBuffer);

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		std::vector<VkSemaphore> waitSemaphores = { _swapchain->imageAvailableSemaphore };

		VkSemaphore renderDoneSemaphore = _swapchain->renderDoneSemaphore;
		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = (uint32_t)waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &submitCommandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderDoneSemaphore;
		VkU::QueueSubmit(renderQueue, 1, &submitInfo, renderDone);

		VkSwapchainKHR swapchainHandle = _swapchain->handle;
		VkPresentInfoKHR presentInfo;
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderDoneSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchainHandle;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;
		VkU::QueuePresentKHR(renderQueue, &presentInfo);
	}
};

#endif