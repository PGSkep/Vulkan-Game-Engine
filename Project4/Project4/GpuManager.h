#ifndef	GPU_MANAGER_H
#define GPU_MANAGER_H

#include "GpuInstanceManager.h"
#include "GpuDeviceManager.h"
#include "GpuMemoryManager.h"
#include "GpuSwapchainManager.h"
#include "GpuSamplerManager.h"
#include "GpuDescriptorManager.h"
#include "GpuShaderManager.h"
#include "GpuPipelineManager.h"
#include "GpuRenderManager.h"

#include "../../Common/Utility/Math3D.h"
#include "Camera.h"

Math3D::Mat4 viewProjection[2];

class GpuManager
{
	GpuInstanceManager		gpuInstanceManager;
	GpuDeviceManager		gpuDeviceManager;
	GpuMemoryManager		gpuMemoryManager;
	GpuSwapchainManager		gpuSwapchainManager;
	GpuDescriptorManager	gpuDescriptorManager;
	GpuShaderManager		gpuShaderManager;
	GpuPipelineManager		gpuPipelineManager;
	GpuSamplerManager		gpuSamplerManager;
	GpuRenderManager		gpuRenderManager;

public:
	void Init()
	{
		Camera camera;
		camera.position = { 0.0f, 0.0f, -10.0f };
		camera.rotation = { 0.0f, 0.0f, 0.0f };
		camera.fov = 45.0f;
		camera.aspect = 800.0f / 600.0f;
		camera.nearV = 0.1f;
		camera.farV = 1000.0f;
		camera.speed = 10.0f;

		camera.Update();

		viewProjection[0] = camera.GetLookAt();
		viewProjection[1] = camera.GetProjectionMatrix();

		// HARDCODED
		uint32_t graphicsGroupID = 0;
		uint32_t graphicsQueueID = 0;

		/// INSTANCE
		GpuInstanceManager::InstanceInfo instanceInfo;
		instanceInfo.appInfo = VkU::GetVkApplicationInfo("Application Name", 0, "Engine Name", 0, VK_MAKE_VERSION(1, 0, VK_HEADER_VERSION));
		instanceInfo.instanceLayers = { "VK_LAYER_LUNARG_standard_validation" };
		instanceInfo.instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME };
		GpuInstanceManager::DebugInfo debugInfo;
		debugInfo.debugFlags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
		debugInfo.debugCallback = VkU::DebugReportCallback;
		GpuInstanceManager::SurfaceInfo surfaceInfo;
		surfaceInfo.name = "main window";
		surfaceInfo.title = "Title";
		surfaceInfo.wndProc = VkU::MainWndProc;
		surfaceInfo.width = 800;
		surfaceInfo.height = 600;
		gpuInstanceManager.Init(instanceInfo, debugInfo, surfaceInfo);

		/// DEVICE
		size_t physicalDeviceIndex = 0;
		gpuDeviceManager.Init(
			&gpuInstanceManager.physicalDevices[physicalDeviceIndex],
			{ "VK_KHR_swapchain" },
			VkU::PhysicalDevivceGetFeaturesFromStrArr({ "fillModeNonSolid", "multiViewport", "tessellationShader", "geometryShader", "samplerAnisotropy" }),
			{ Device::QueueFamilyRequirements::Get(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, true,{ 1.0f }), },
			{ gpuInstanceManager.swapchain.surface });

		/// MEMORY
		size_t instanceCount = 9;

		Vertex::STRUCTURE	modelColorLightStruct = Vertex::STRUCTURE::GetStructure(Vertex::VERTEXTYPE::POS3_COLOR3_ROT3_SCALE3_LIGHTCOUNT);
		size_t				modelColorLightSize = Vertex::STRUCTURE::GetStride(modelColorLightStruct) * instanceCount;
		void*				modelColorLightData = new uint8_t[modelColorLightSize];

		for (size_t x = 0; x != 3; ++x)
		{
			for (size_t y = 0; y != 3; ++y)
			{
				size_t index = y + (x * 3);

				void* indexAddress = Vertex::STRUCTURE::GetIndexAddress(modelColorLightData, modelColorLightStruct, index);

				void* indexPos3			= Vertex::STRUCTURE::GetAttributeAddress(indexAddress, modelColorLightStruct, Vertex::ATTRIBUTE::POS3);
				void* indexColor3		= Vertex::STRUCTURE::GetAttributeAddress(indexAddress, modelColorLightStruct, Vertex::ATTRIBUTE::COLOR3);
				void* indexRot3			= Vertex::STRUCTURE::GetAttributeAddress(indexAddress, modelColorLightStruct, Vertex::ATTRIBUTE::ROT3);
				void* indexScale3		= Vertex::STRUCTURE::GetAttributeAddress(indexAddress, modelColorLightStruct, Vertex::ATTRIBUTE::SCALE3);
				void* indexLightCount	= Vertex::STRUCTURE::GetAttributeAddress(indexAddress, modelColorLightStruct, Vertex::ATTRIBUTE::LIGHTCOUNT);

				*(Math3D::Vec3*)indexPos3	= { -2.0f + (float)x * 2.0f, -2.0f + (float)y * 2.0f, 0.0f };
				*(Math3D::Vec3*)indexColor3	= { (float)index / 9.0f, (float)index / 9.0f, (float)index / 9.0f };
				*(Math3D::Vec3*)indexRot3	= { 0.3f, 0.0f, 0.0f };
				*(Math3D::Vec3*)indexScale3	= { 1.0f, 1.0f, 1.0f };
				*(uint16_t*)indexLightCount	= { 0 };
			}
		}

		VkDeviceSize bufferTransferSize = 144144;
		VkDeviceSize vertexBufferSize = 144144 + 880 + modelColorLightSize;
		VkDeviceSize indexBufferSize = (15816 + 36) * sizeof(uint32_t);
		VkDeviceSize uniformBufferSize = sizeof(viewProjection);

		gpuMemoryManager.Init(gpuDeviceManager.device,
			gpuDeviceManager.device.queueGroups[graphicsGroupID].commandPool,
			gpuDeviceManager.device.queueGroups[graphicsGroupID].queues[graphicsQueueID],
			bufferTransferSize,
			vertexBufferSize,
			indexBufferSize,
			uniformBufferSize);

		gpuMemoryManager.AddVertex(gpuDeviceManager.device.handle, "staticMeshInstanceData", modelColorLightData, modelColorLightSize, modelColorLightStruct);
		delete[] modelColorLightData;

		gpuMemoryManager.AddMesh(gpuDeviceManager.device.handle, "CubePos", "../../Data/Models/TestCube.fbx", Vertex::VERTEXTYPE::POS3, true, false);
		gpuMemoryManager.AddMesh(gpuDeviceManager.device.handle, "CubeOth", "../../Data/Models/TestCube.fbx", Vertex::VERTEXTYPE::UV_NORMAL_COLOR3, true, false);
		gpuMemoryManager.AddMesh(gpuDeviceManager.device.handle, "CubeInd", "../../Data/Models/TestCube.fbx", Vertex::VERTEXTYPE::POS3, false, true);

		gpuMemoryManager.AddMesh(gpuDeviceManager.device.handle, "BunnyWarriorPos", "../../Data/Models/Lagomorph Walk Start & Cycle Stationary V1.fbx", Vertex::VERTEXTYPE::POS3_UV_NORMAL_COLOR3, true, true);

		gpuMemoryManager.AddUniform(gpuDeviceManager.device.handle, "viewProjection", viewProjection, sizeof(viewProjection));

		/// SWAPCHAIN
		gpuMemoryManager.transferManager.WaitTransfer(gpuDeviceManager.device.handle);
		gpuMemoryManager.transferManager.BeginCommandBuffer();
		gpuSwapchainManager.Init(gpuDeviceManager.device, &gpuInstanceManager.swapchain, &gpuMemoryManager.transferManager, gpuDeviceManager.device.queueGroups[graphicsGroupID].commandPool);
		gpuMemoryManager.transferManager.EndCommandBuffer();

		/// SAMPLER
		gpuSamplerManager.AddSampler("Standard", VkU::GetVkSamplerCreateInfo(VK_FILTER_NEAREST, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, 0.0f, VK_TRUE, 16, VK_FALSE, VK_COMPARE_OP_ALWAYS, 0.0f, 0.0f, VK_BORDER_COLOR_INT_OPAQUE_BLACK, VK_FALSE), gpuDeviceManager.device.handle);

		/// DESCRIPTOR
		gpuDescriptorManager.Init(1);
		gpuDescriptorManager.AddDescriptorSetLayout(gpuDeviceManager.device.handle, "Vertex_1Uniform", {
			VkU::GetVkDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr),
		});

		gpuDescriptorManager.AddDescriptorPool(gpuDeviceManager.device.handle, "pool", VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, {
			VkU::GetVkDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1)
		});

		gpuDescriptorManager.AddDescriptorSet(gpuDeviceManager.device.handle, 0, "viewProjection", { "viewProjection" }, gpuMemoryManager.uniformMemoryMap, gpuDescriptorManager.GetDescriptorPool("pool"), gpuDescriptorManager.GetDescriptorSetLayout("Vertex_1Uniform"));

		/// SHADER
		gpuShaderManager.AddShaderModule("vert", "../../Data/Shaders/vert.spv", gpuDeviceManager.device.handle);
		gpuShaderManager.AddShaderModule("frag", "../../Data/Shaders/frag.spv", gpuDeviceManager.device.handle);

		/// PIPELINE
		gpuPipelineManager.AddPipelineLayout(gpuDeviceManager.device.handle, "Vertex_Uniform1_PushConstant_0~16", {
			gpuDescriptorManager.GetDescriptorSetLayout("Vertex_1Uniform"),
		}, {
			VkU::GetVkPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 4 + sizeof(uint32_t)),
		});

		gpuPipelineManager.AddInputBindings("Pos3_UvNormalColor3", { VkU::GetVertexInputBindingDescriptions(
		{
			VkU::BindingDescriptionInfo::Get(0, Vertex::VERTEXTYPE::POS3_COLOR3_ROT3_SCALE3_LIGHTCOUNT, VK_VERTEX_INPUT_RATE_INSTANCE),
			VkU::BindingDescriptionInfo::Get(1, Vertex::VERTEXTYPE::POS3, VK_VERTEX_INPUT_RATE_VERTEX),
			VkU::BindingDescriptionInfo::Get(2, Vertex::VERTEXTYPE::UV_NORMAL_COLOR3, VK_VERTEX_INPUT_RATE_VERTEX),
		}) });
		gpuPipelineManager.AddInputAttributes("Pos3_UvNormalColor3", { VkU::GetVertexInputAttributeDescriptions(
		{
			Vertex::VERTEXTYPE::POS3_COLOR3_ROT3_SCALE3_LIGHTCOUNT,
			Vertex::VERTEXTYPE::POS3,
			Vertex::VERTEXTYPE::UV_NORMAL_COLOR3,
		}) });

		gpuPipelineManager.AddViewports("1ClientSpace", gpuSwapchainManager.swapchain->viewports);
		gpuPipelineManager.AddScissors("1ClientSpace", gpuSwapchainManager.swapchain->scissors);
		gpuPipelineManager.AddColorBlendAttachments("AdditiveBlend", { VkU::GetVkPipelineColorBlendAttachmentState(VK_FALSE, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT) });
		gpuPipelineManager.AddDynamics("Viewport_Scissor", { { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, } });

		gpuPipelineManager.AddShaderStage("Pos3UvNormalColor3_Basic", { VkU::GetVkPipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, gpuShaderManager.GetShaderModule("vert")), VkU::GetVkPipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, gpuShaderManager.GetShaderModule("frag")), });
		gpuPipelineManager.AddVertexInputState("Pos3UvNormalColor3", VkU::GetVkPipelineVertexInputStateCreateInfo(gpuPipelineManager.GetInputBindingsPtr("Pos3_UvNormalColor3"), gpuPipelineManager.GetInputAttributesPtr("Pos3_UvNormalColor3")));
		gpuPipelineManager.AddInputAssemblyState("TriangleList", VkU::GetVkPipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE));
		gpuPipelineManager.AddTessallationState("0", VkU::GetVkPipelineTessellationStateCreateInfo(0));
		gpuPipelineManager.AddViewportState("1ClientSpace", VkU::GetVkPipelineViewportStateCreateInfo(*gpuPipelineManager.GetViewportsPtr("1ClientSpace"), *gpuPipelineManager.GetScissorsPtr("1ClientSpace")));
		gpuPipelineManager.AddRasterizationState("Fill_CounterClockwise_BackCull", VkU::GetVkPipelineRasterizationStateCreateInfo(VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f));
		gpuPipelineManager.AddMultisampleState("1Sample", VkU::GetVkPipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0f, nullptr, VK_FALSE, VK_FALSE));
		gpuPipelineManager.AddDepthStencilState("EnableWrite&Test_CompateOperationLess", VkU::GetVkPipelineDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE, {}, {}, 0.0f, 1.0f));
		gpuPipelineManager.AddColorBlendState("additiveBlend", VkU::GetVkPipelineColorBlendStateCreateInfo(VK_FALSE, VK_LOGIC_OP_COPY, gpuPipelineManager.GetColorBlendAttachmentsPtr("AdditiveBlend"), 0.0f, 0.0f, 0.0f, 0.0f));
		gpuPipelineManager.AddDynamicState("Viewport_Scissor", VkU::GetVkPipelineDynamicStateCreateInfo(gpuPipelineManager.GetDynamicsPtr("Viewport_Scissor")));

		gpuPipelineManager.AddGraphicsPipelineInfo("Standard", "Pos3UvNormalColor3_Basic", "Pos3UvNormalColor3", "TriangleList", "0", "1ClientSpace", "Fill_CounterClockwise_BackCull", "1Sample", "EnableWrite&Test_CompateOperationLess", "additiveBlend", "Viewport_Scissor", "Vertex_Uniform1_PushConstant_0~16", gpuSwapchainManager.swapchain->renderPass, VK_NULL_HANDLE);

		gpuPipelineManager.GeneratePipelines(gpuDeviceManager.device.handle);

		/// RENDER
		gpuRenderManager.Init(gpuDeviceManager.device, gpuDeviceManager.device.queueGroups[graphicsGroupID].commandPool, gpuDeviceManager.device.queueGroups[graphicsGroupID].queues[graphicsQueueID]);

		GpuRenderManager::AddStaticMeshInfo addStaticMeshInfo;
		addStaticMeshInfo.staticMeshPipelineName = "Standard";
		addStaticMeshInfo.staticMeshPipelineInfo.pipeline = "Standard";
		addStaticMeshInfo.staticMeshPipelineInfo.pipelineLayout = "Vertex_Uniform1_PushConstant_0~16";
		addStaticMeshInfo.staticMeshPipelineInfo.descriptorSet0 = "viewProjection";
		addStaticMeshInfo.staticMeshName = "Cube";

		addStaticMeshInfo.staticMeshInfo.vertexInstance = "staticMeshInstanceData";
		addStaticMeshInfo.staticMeshInfo.vertexPosition = "CubePos";
		addStaticMeshInfo.staticMeshInfo.vertexOther = "CubeOth";
		addStaticMeshInfo.staticMeshInfo.indice = "CubeInd";
		addStaticMeshInfo.staticMeshInfo.descriptorSet1 = "";

		addStaticMeshInfo.staticMeshInfo.instanceCount = 9;
		gpuRenderManager.AddStaticMeshes(addStaticMeshInfo);
	}
	void Run()
	{
		gpuDescriptorManager.UpdateOutdatedDescriptors(gpuDeviceManager.device.handle, gpuMemoryManager.uniformMemoryMap, gpuMemoryManager.uniformBuffer.handle);
		gpuMemoryManager.UpdateOutdatedUniformsDescriptorSets();

		gpuInstanceManager.swapchain.PrepareToRecord(gpuDeviceManager.device.handle);
		gpuInstanceManager.swapchain.BeginRecording();
		gpuRenderManager.RecordStaticMeshes(gpuInstanceManager.swapchain.commandBuffers[gpuInstanceManager.swapchain.imageIndex], &gpuPipelineManager, &gpuDescriptorManager, &gpuMemoryManager);
		gpuInstanceManager.swapchain.EndRecording();

		gpuRenderManager.Render(gpuDeviceManager.device.handle, &gpuInstanceManager.swapchain);
	}
	void ShutDown()
	{
		gpuRenderManager.ShutDown(gpuDeviceManager.device.handle);
		gpuPipelineManager.ShutDown(gpuDeviceManager.device.handle);
		gpuShaderManager.ShutDown(gpuDeviceManager.device.handle);
		gpuDescriptorManager.ShutDown(gpuDeviceManager.device.handle);
		gpuSamplerManager.ShutDown(gpuDeviceManager.device.handle);
		gpuSwapchainManager.ShutDown(gpuDeviceManager.device.handle);
		gpuMemoryManager.ShutDown(gpuDeviceManager.device.handle);
		gpuDeviceManager.ShutDown();
		gpuInstanceManager.ShutDown();
	}
};

#endif