#ifndef	GPU_PIPELINE_MANAGER_H
#define GPU_PIPELINE_MANAGER_H

#include "../../Common/Other/VkU.h"

#include <map>

class GpuPipelineManager
{
	// layout
	std::map<std::string, VkPipelineLayout>	pipelineLayouts;

	// vertex input
	std::map<std::string, std::vector<VkVertexInputBindingDescription>>		inputBindings;
	std::map<std::string, std::vector<VkVertexInputAttributeDescription>>	inputAttributes;

	// viewport
	std::map<std::string, std::vector<VkViewport>>							viewports;
	std::map<std::string, std::vector<VkRect2D>>							scissors;

	// color blend
	std::map<std::string, std::vector<VkPipelineColorBlendAttachmentState>>	colorBlendAttachment;

	// dynamic
	std::map<std::string, std::vector<VkDynamicState>>						dynamic;

	// pipeline stages
	std::map<std::string, std::vector<VkPipelineShaderStageCreateInfo>>	shaderStages;
	std::map<std::string, VkPipelineVertexInputStateCreateInfo>			vertexInputStates;
	std::map<std::string, VkPipelineInputAssemblyStateCreateInfo>		inputAssemblyStates;
	std::map<std::string, VkPipelineTessellationStateCreateInfo>		tessalationStates;
	std::map<std::string, VkPipelineViewportStateCreateInfo>			viewportStates;
	std::map<std::string, VkPipelineRasterizationStateCreateInfo>		rasterizationStates;
	std::map<std::string, VkPipelineMultisampleStateCreateInfo>			multisampleStates;
	std::map<std::string, VkPipelineDepthStencilStateCreateInfo>		depthStencilStates;
	std::map<std::string, VkPipelineColorBlendStateCreateInfo>			colorBlendStates;
	std::map<std::string, VkPipelineDynamicStateCreateInfo>				dynamicStates;

	// pipeline info
	std::vector<VkGraphicsPipelineCreateInfo>				graphicsPipelineInfos;
	std::map<std::string, VkGraphicsPipelineCreateInfo*>	graphicsPipelineInfoPtrs;

	std::vector<VkPipeline> graphicsPipelines;
	std::map<std::string, size_t> graphicsPipelineIndices;

public:
	void GeneratePipelines(VkDevice _device)
	{
		graphicsPipelines.resize(graphicsPipelineInfos.size());
		VkU::CreateGraphicsPipelines(_device, (uint32_t)graphicsPipelineInfos.size(), graphicsPipelineInfos.data(), graphicsPipelines.data());
	}

	void ShutDown(VkDevice _device)
	{
		for (size_t iPipeline = 0; iPipeline != graphicsPipelines.size(); ++iPipeline)
			vkDestroyPipeline(_device, graphicsPipelines[iPipeline], nullptr);

		for (std::map<std::string, VkPipelineLayout>::iterator iter = pipelineLayouts.begin(); iter != pipelineLayouts.end(); ++iter)
			vkDestroyPipelineLayout(_device, iter->second, nullptr);
	}

	/// ADD
	void AddPipelineLayout(VkDevice _device, const char* _name, std::vector<VkDescriptorSetLayout> _descriptorSetLayout, std::vector<VkPushConstantRange> _pushConstantRange)
	{
		VkPipelineLayout pipelineLayout;

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineLayoutCreateInfo.setLayoutCount = (uint32_t)_descriptorSetLayout.size();
		pipelineLayoutCreateInfo.pSetLayouts = _descriptorSetLayout.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = (uint32_t)_pushConstantRange.size();
		pipelineLayoutCreateInfo.pPushConstantRanges = _pushConstantRange.data();
		VkU::CreatePipelineLayout(_device, &pipelineLayoutCreateInfo, &pipelineLayout);

		pipelineLayouts[_name] = pipelineLayout;
	}

	void AddInputBindings(const char* _name, std::vector<VkVertexInputBindingDescription> _inputBindings)
	{
		inputBindings[_name] = _inputBindings;
	}
	void AddInputAttributes(const char* _name, std::vector<VkVertexInputAttributeDescription> _inputAttributes)
	{
		inputAttributes[_name] = _inputAttributes;
	}
	void AddViewports(const char* _name, std::vector<VkViewport> _viewports)
	{
		viewports[_name] = _viewports;
	}
	void AddScissors(const char* _name, std::vector<VkRect2D> _rect2D)
	{
		scissors[_name] = _rect2D;
	}
	void AddColorBlendAttachments(const char* _name, std::vector<VkPipelineColorBlendAttachmentState> _colorBlendAttachment)
	{
		colorBlendAttachment[_name] = _colorBlendAttachment;
	}
	void AddDynamics(const char* _name, std::vector<VkDynamicState> _dynamic)
	{
		dynamic[_name] = _dynamic;
	}

	void AddShaderStage(const char* _name, std::vector<VkPipelineShaderStageCreateInfo> _shaderStage)
	{
		shaderStages[_name] = _shaderStage;
	}
	void AddVertexInputState(const char* _name, VkPipelineVertexInputStateCreateInfo _vertexInputState)
	{
		vertexInputStates[_name] = _vertexInputState;
	}
	void AddInputAssemblyState(const char* _name, VkPipelineInputAssemblyStateCreateInfo _inputAssemblyState)
	{
		inputAssemblyStates[_name] = _inputAssemblyState;
	}
	void AddTessallationState(const char* _name, VkPipelineTessellationStateCreateInfo _tessellationState)
	{
		tessalationStates[_name] = _tessellationState;
	}
	void AddViewportState(const char* _name, VkPipelineViewportStateCreateInfo _viewportState)
	{
		viewportStates[_name] = _viewportState;
	}
	void AddRasterizationState(const char* _name, VkPipelineRasterizationStateCreateInfo _rasterizationState)
	{
		rasterizationStates[_name] = _rasterizationState;
	}
	void AddMultisampleState(const char* _name, VkPipelineMultisampleStateCreateInfo _multisampleState)
	{
		multisampleStates[_name] = _multisampleState;
	}
	void AddDepthStencilState(const char* _name, VkPipelineDepthStencilStateCreateInfo _depthStencilState)
	{
		depthStencilStates[_name] = _depthStencilState;
	}
	void AddColorBlendState(const char* _name, VkPipelineColorBlendStateCreateInfo _colorBlendState)
	{
		colorBlendStates[_name] = _colorBlendState;
	}
	void AddDynamicState(const char* _name, VkPipelineDynamicStateCreateInfo _dynamicState)
	{
		dynamicStates[_name] = _dynamicState;
	}

	void AddGraphicsPipelineInfo(const char* _name, const char* _shader, const char* _vertexInput, const char* _inputAssembly, const char* _tessalation, const char* _viewport, const char* _rasterization, const char* _multisample, const char* _depthStencil, const char* _colorBlend, const char* _dynamic, const char* _pipelineLayout, VkRenderPass _renderPass, VkPipeline _basePipeline)
	{
		std::vector<VkPipelineShaderStageCreateInfo>* shaderStageState = GetShaderStagePtr(_shader);

		graphicsPipelineInfos.push_back(VkU::GetVkGraphicsPipelineCreateInfo((uint32_t)shaderStageState->size(), shaderStageState->data(), GetVertexInputStatePtr(_vertexInput), GetInputAssemblyStatePtr(_inputAssembly), GetTessallationStatePtr(_tessalation), GetViewportStatePtr(_viewport), GetRasterizationStatePtr(_rasterization), GetMultisampleStatePtr(_multisample), GetDepthStencilStatePtr(_depthStencil), GetColorBlendStatePtr(_colorBlend), GetDynamicStatePtr(_dynamic), GetPipelineLayout(_pipelineLayout), _renderPass, _basePipeline));
		graphicsPipelineInfoPtrs[_name] = &graphicsPipelineInfos[graphicsPipelineInfos.size() - 1];
		graphicsPipelineIndices[_name] = graphicsPipelineInfos.size() - 1;
	}

	/// GET
	VkPipelineLayout GetPipelineLayout(const char* _name)
	{
		std::map<std::string, VkPipelineLayout>::iterator iter = pipelineLayouts.find(_name);
		if (iter != pipelineLayouts.end())
			return iter->second;
		else
			return VK_NULL_HANDLE;
	}

	std::vector<VkVertexInputBindingDescription>* GetInputBindingsPtr(const char* _name)
	{
		std::map<std::string, std::vector<VkVertexInputBindingDescription>>::iterator iter = inputBindings.find(_name);
		if (iter != inputBindings.end())
			return &iter->second;
		else
			return nullptr;
	}
	std::vector<VkVertexInputAttributeDescription>* GetInputAttributesPtr(const char* _name)
	{
		std::map<std::string, std::vector<VkVertexInputAttributeDescription>>::iterator iter = inputAttributes.find(_name);
		if (iter != inputAttributes.end())
			return &iter->second;
		else
			return nullptr;
	}
	std::vector<VkViewport>* GetViewportsPtr(const char* _name)
	{
		std::map<std::string, std::vector<VkViewport>>::iterator iter = viewports.find(_name);
		if (iter != viewports.end())
			return &iter->second;
		else
			return nullptr;
	}
	std::vector<VkRect2D>* GetScissorsPtr(const char* _name)
	{
		std::map<std::string, std::vector<VkRect2D>>::iterator iter = scissors.find(_name);
		if (iter != scissors.end())
			return &iter->second;
		else
			return nullptr;
	}
	std::vector<VkPipelineColorBlendAttachmentState>* GetColorBlendAttachmentsPtr(const char* _name)
	{
		std::map<std::string, std::vector<VkPipelineColorBlendAttachmentState>>::iterator iter = colorBlendAttachment.find(_name);
		if (iter != colorBlendAttachment.end())
			return &iter->second;
		else
			return nullptr;
	}
	std::vector<VkDynamicState>* GetDynamicsPtr(const char* _name)
	{
		std::map<std::string, std::vector<VkDynamicState>>::iterator iter = dynamic.find(_name);
		if (iter != dynamic.end())
			return &iter->second;
		else
			return nullptr;
	}

	std::vector<VkPipelineShaderStageCreateInfo>* GetShaderStagePtr(const char* _name)
	{
		std::map<std::string, std::vector<VkPipelineShaderStageCreateInfo>>::iterator iter = shaderStages.find(_name);
		if (iter != shaderStages.end())
			return &iter->second;
		else
			return nullptr;
	}
	VkPipelineVertexInputStateCreateInfo* GetVertexInputStatePtr(const char* _name)
	{
		std::map<std::string, VkPipelineVertexInputStateCreateInfo>::iterator iter = vertexInputStates.find(_name);
		if (iter != vertexInputStates.end())
			return &iter->second;
		else
			return nullptr;
	}
	VkPipelineInputAssemblyStateCreateInfo* GetInputAssemblyStatePtr(const char* _name)
	{
		std::map<std::string, VkPipelineInputAssemblyStateCreateInfo>::iterator iter = inputAssemblyStates.find(_name);
		if (iter != inputAssemblyStates.end())
			return &iter->second;
		else
			return nullptr;
	}
	VkPipelineTessellationStateCreateInfo* GetTessallationStatePtr(const char* _name)
	{
		std::map<std::string, VkPipelineTessellationStateCreateInfo>::iterator iter = tessalationStates.find(_name);
		if (iter != tessalationStates.end())
			return &iter->second;
		else
			return nullptr;
	}
	VkPipelineViewportStateCreateInfo* GetViewportStatePtr(const char* _name)
	{
		std::map<std::string, VkPipelineViewportStateCreateInfo>::iterator iter = viewportStates.find(_name);
		if (iter != viewportStates.end())
			return &iter->second;
		else
			return nullptr;
	}
	VkPipelineRasterizationStateCreateInfo* GetRasterizationStatePtr(const char* _name)
	{
		std::map<std::string, VkPipelineRasterizationStateCreateInfo>::iterator iter = rasterizationStates.find(_name);
		if (iter != rasterizationStates.end())
			return &iter->second;
		else
			return nullptr;
	}
	VkPipelineMultisampleStateCreateInfo* GetMultisampleStatePtr(const char* _name)
	{
		std::map<std::string, VkPipelineMultisampleStateCreateInfo>::iterator iter = multisampleStates.find(_name);
		if (iter != multisampleStates.end())
			return &iter->second;
		else
			return nullptr;
	}
	VkPipelineDepthStencilStateCreateInfo* GetDepthStencilStatePtr(const char* _name)
	{
		std::map<std::string, VkPipelineDepthStencilStateCreateInfo>::iterator iter = depthStencilStates.find(_name);
		if (iter != depthStencilStates.end())
			return &iter->second;
		else
			return nullptr;
	}
	VkPipelineColorBlendStateCreateInfo* GetColorBlendStatePtr(const char* _name)
	{
		std::map<std::string, VkPipelineColorBlendStateCreateInfo>::iterator iter = colorBlendStates.find(_name);
		if (iter != colorBlendStates.end())
			return &iter->second;
		else
			return nullptr;
	}
	VkPipelineDynamicStateCreateInfo* GetDynamicStatePtr(const char* _name)
	{
		std::map<std::string, VkPipelineDynamicStateCreateInfo>::iterator iter = dynamicStates.find(_name);
		if (iter != dynamicStates.end())
			return &iter->second;
		else
			return nullptr;
	}

	VkGraphicsPipelineCreateInfo* GetGraphicsPipelineInfoPtr(const char* _name)
	{
		return graphicsPipelineInfoPtrs[_name];
	}
	VkPipeline GetGraphicsPipeline(const char* _name)
	{
		std::map<std::string, size_t>::iterator iter = graphicsPipelineIndices.find(_name);
		if (iter != graphicsPipelineIndices.end())
			return graphicsPipelines[iter->second];
		else
			return VK_NULL_HANDLE;
	}
};

#endif