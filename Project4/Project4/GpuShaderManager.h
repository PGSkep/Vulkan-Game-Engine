#ifndef	GPU_SHADER_MANAGER_H
#define GPU_SHADER_MANAGER_H

#include "../../Common/Other/VkU.h"
#include "../../Common/Utility/Loader.h"

#include <map>

class GpuShaderManager
{
	std::map<std::string, VkShaderModule> shaderModules;

public:
	void AddShaderModule(const char* _name, const char* _filename, VkDevice _device)
	{
		std::vector<char> shaderCode;
		Loader::LoadText(_filename, shaderCode);

		VkShaderModule shaderModule;

		VkShaderModuleCreateInfo shaderModuleCreateInfo;
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		shaderModuleCreateInfo.codeSize = shaderCode.size();
		shaderModuleCreateInfo.pCode = (uint32_t*)shaderCode.data();
		VkU::CreateShaderModule(_device, &shaderModuleCreateInfo, &shaderModule);

		shaderModules[_name] = shaderModule;
	}

	void ShutDown(VkDevice _device)
	{
		for (std::map<std::string, VkShaderModule>::iterator iter = shaderModules.begin(); iter != shaderModules.end(); ++iter)
			vkDestroyShaderModule(_device, iter->second, nullptr);
	}

	VkShaderModule GetShaderModule(const char* _name)
	{
		std::map<std::string, VkShaderModule>::iterator iter = shaderModules.find(_name);
		if (iter != shaderModules.end())
			return iter->second;
		else
			return VK_NULL_HANDLE;
	}
};

#endif