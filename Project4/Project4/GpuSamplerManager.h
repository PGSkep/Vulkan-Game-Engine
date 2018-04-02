#ifndef	GPU_SAMPLER_MANAGER_H
#define GPU_SAMPLER_MANAGER_H

#include "../../Common/Other/VkU.h"

#include <map>
#include <string>

class GpuSamplerManager
{
	std::map<std::string, VkSampler> samplers;

public:
	void AddSampler(const char* _name, VkSamplerCreateInfo _samplerCreateInfo, VkDevice _device)
	{
		VkSampler sampler;

		VkU::CreateSampler(_device, &_samplerCreateInfo, &sampler);

		samplers[_name] = sampler;
	}
	VkSampler GetSampler(const char* _name)
	{
		std::map<std::string, VkSampler>::iterator iter = samplers.find(_name);
		if (iter != samplers.end())
			return iter->second;
		else
			return VK_NULL_HANDLE;
	}

	void ShutDown(VkDevice _device)
	{
		for (std::map<std::string, VkSampler>::iterator iter = samplers.begin(); iter != samplers.end(); ++iter)
			vkDestroySampler(_device, iter->second, nullptr);
	}
};

#endif