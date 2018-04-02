#ifndef	GPU_DESCRIPTOR_MANAGER_H
#define GPU_DESCRIPTOR_MANAGER_H

#include "../../Common/Other/VkU.h"

#include "GpuMemoryManager.h"

#include <map>
#include <vector>
#include <string>

class GpuDescriptorManager
{
	std::vector<VkWriteDescriptorSet>					writeDescriptorSets;
	std::vector<std::vector<VkDescriptorBufferInfo>>	descriptorBufferInfos;

public:
	std::map<std::string, VkDescriptorSetLayout>	descriptorSetLayouts;
	std::map<std::string, VkDescriptorPool>			descriptorPools;

	struct Descriptor
	{
		VkDescriptorSet handle;
		std::vector<std::string> uniformBindings;
		// TODO: samplers & etc

		static inline Descriptor Get(VkDescriptorSet _handle, std::vector<std::string> _uniformBindings)
		{
			Descriptor descriptor;
			descriptor.handle = _handle;
			descriptor.uniformBindings = _uniformBindings;
			return descriptor;
		}
	};
	struct Descriptors
	{
		std::map<std::string, Descriptor> descriptors;
	};
	std::vector<Descriptors> setBinding;

	std::vector<std::vector<std::string>> outdatedDescriptors;

	void Init(size_t _highestBinding)
	{
		setBinding.resize(_highestBinding + 1);
		outdatedDescriptors.resize(_highestBinding + 1);
	}
	void ShutDown(VkDevice _device)
	{
		for (std::map<std::string, VkDescriptorPool>::iterator iter = descriptorPools.begin(); iter != descriptorPools.end(); ++iter)
			vkDestroyDescriptorPool(_device, iter->second, nullptr);

		for (std::map<std::string, VkDescriptorSetLayout>::iterator iter = descriptorSetLayouts.begin(); iter != descriptorSetLayouts.end(); ++iter)
			vkDestroyDescriptorSetLayout(_device, iter->second, nullptr);
	}

	// Descriptor
	VkDescriptorSetLayout GetDescriptorSetLayout(const char* _name)
	{
		std::map<std::string, VkDescriptorSetLayout>::iterator iter = descriptorSetLayouts.find(_name);
		if (iter == descriptorSetLayouts.end())
			return VK_NULL_HANDLE;

		return iter->second;
	}
	void AddDescriptorSetLayout(VkDevice _device, const char* _name, std::vector<VkDescriptorSetLayoutBinding> _descriptorSetLayoutBinding)
	{
		VkDescriptorSetLayout descriptorSetLayout;

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = VK_FLAGS_NONE;
		descriptorSetLayoutCreateInfo.bindingCount = (uint32_t)_descriptorSetLayoutBinding.size();
		descriptorSetLayoutCreateInfo.pBindings = _descriptorSetLayoutBinding.data();
		VkU::CreateDescriptorSetLayout(_device, &descriptorSetLayoutCreateInfo, &descriptorSetLayout);

		descriptorSetLayouts[_name] = descriptorSetLayout;
	}

	VkDescriptorPool GetDescriptorPool(const char* _name)
	{
		std::map<std::string, VkDescriptorPool>::iterator iter = descriptorPools.find(_name);
		if (iter == descriptorPools.end())
			return VK_NULL_HANDLE;

		return iter->second;
	}
	void AddDescriptorPool(VkDevice _device, const char* _name, VkDescriptorPoolCreateFlags _flags, uint32_t _maxSets, std::vector<VkDescriptorPoolSize> _pPoolSizes)
	{
		VkDescriptorPool descriptorPool;

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.pNext = nullptr;
		descriptorPoolCreateInfo.flags = _flags;
		descriptorPoolCreateInfo.maxSets = _maxSets;
		descriptorPoolCreateInfo.poolSizeCount = (uint32_t)_pPoolSizes.size();
		descriptorPoolCreateInfo.pPoolSizes = _pPoolSizes.data();
		VkU::CreateDescriptorPool(_device, &descriptorPoolCreateInfo, &descriptorPool);

		descriptorPools[_name] = descriptorPool;
	}

	//Descriptor GetDescriptorSet(uint32_t _setBinding, const char* _setType, const char* _setName)
	//{
	//	std::map<std::string, Descriptors>::iterator fType = setBinding[_setBinding].descriptorType.find(_setType);
	//	if (fType == setBinding[_setBinding].descriptorType.end())
	//		return {};
	//
	//	std::map<std::string, Descriptor>::iterator fSet = fType->second.descriptors.find(_setName);
	//	if (fSet == fType->second.descriptors.end())
	//		return {};
	//
	//	return fSet->second;
	//}
	VkDescriptorSet GetDescriptorSetHandle(uint32_t _setBinding, const char* _setName)
	{
		std::map<std::string, Descriptor>::iterator iter = setBinding[_setBinding].descriptors.find(_setName);

		if (iter == setBinding[_setBinding].descriptors.end())
			return VK_NULL_HANDLE;
		else
			return iter->second.handle;
	}
	void AddDescriptorSet(VkDevice _device, uint32_t _setBinding, const char* _setName, std::vector<std::string> _uniformBindings, MemoryMap<GpuMemoryManager::UniformBufferMapInfo>& _uniformMemoryMap, VkDescriptorPool _descriptorPool, VkDescriptorSetLayout _pSetLayout)
	{
		for (size_t iUniform = 0; iUniform != _uniformBindings.size(); ++iUniform)
		{
			GpuMemoryManager::UniformBufferMapInfo* uniformBufferMapInfo = _uniformMemoryMap.Get(_uniformBindings[iUniform].c_str()).userdataPtr;
			if (uniformBufferMapInfo == nullptr)
				Error::Report(std::string("Uniform ") + _uniformBindings[iUniform].c_str() + " is not allocated");

			uniformBufferMapInfo->bindings[_setBinding].name[_setName].upToDate = false;
		}

		VkDescriptorSet set;

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.pNext = nullptr;
		descriptorSetAllocateInfo.descriptorPool = _descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = 1;
		descriptorSetAllocateInfo.pSetLayouts = &_pSetLayout;
		VkU::AllocateDescriptorSets(_device, &descriptorSetAllocateInfo, &set);

		setBinding[_setBinding].descriptors[_setName] = Descriptor::Get(set, _uniformBindings);

		outdatedDescriptors[_setBinding].push_back(_setName);
	}

	void UpdateOutdatedDescriptors(VkDevice _device, MemoryMap<GpuMemoryManager::UniformBufferMapInfo>& _uniformMemoryMap, VkBuffer _uniformBuffer)
	{
		VkWriteDescriptorSet writeDescriptorSet;
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.pNext = nullptr;
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.pImageInfo = nullptr;
		writeDescriptorSet.pTexelBufferView = nullptr;
		writeDescriptorSet.pTexelBufferView = nullptr;

		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSet.pImageInfo = nullptr;
		for (size_t iBinding = 0; iBinding != outdatedDescriptors.size(); ++iBinding)
		{
			writeDescriptorSet.dstBinding = (uint32_t)iBinding;

			for (size_t iName = 0; iName != outdatedDescriptors[iBinding].size(); ++iName)
			{
				std::map<std::string, Descriptor>::iterator iterDescriptor = setBinding[iBinding].descriptors.find(outdatedDescriptors[iBinding][iName]);

				writeDescriptorSet.dstSet = iterDescriptor->second.handle;

				descriptorBufferInfos.push_back({});
				for (size_t iUniform = 0; iUniform != iterDescriptor->second.uniformBindings.size(); ++iUniform)
				{
					MemoryMap<GpuMemoryManager::UniformBufferMapInfo>::MemoryInfo memInfo = _uniformMemoryMap.Get(iterDescriptor->second.uniformBindings[iUniform].c_str());

					descriptorBufferInfos[descriptorBufferInfos.size() - 1].push_back(VkU::GetVkDescriptorBufferInfo(_uniformBuffer, memInfo.offset, memInfo.size));
				}

				writeDescriptorSet.descriptorCount = (uint32_t)descriptorBufferInfos[descriptorBufferInfos.size() - 1].size();
				writeDescriptorSet.pBufferInfo = descriptorBufferInfos[descriptorBufferInfos.size() - 1].data();

				writeDescriptorSets.push_back(writeDescriptorSet);
			}
		}

		vkUpdateDescriptorSets(_device, (uint32_t)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);

		writeDescriptorSets.clear();
		descriptorBufferInfos.clear();
	}
};

#endif