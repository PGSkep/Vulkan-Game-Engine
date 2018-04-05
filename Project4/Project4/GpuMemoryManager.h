#ifndef	GPU_MEMORY_MANAGER_H
#define GPU_MEMORY_MANAGER_H

#include "../../Common/Other/VkU.h"

#include "TransferManager.h"
#include "MemoryMap.h"

#include "../../Common/Utility/Loader.h"
#include "../../Common/Utility/Vertex.h"
#include "../../Common/Utility/Error.h"

class GpuMemoryManager
{
private:
	// Memory
	GpuBuffer						vertexBuffer;
	struct VertexBufferMapInfo
	{
		Vertex::STRUCTURE	structure;
	};
	MemoryMap<VertexBufferMapInfo>	vertexMemoryMap;

	GpuBuffer						indexBuffer;
	struct IndexBufferMapInfo
	{
		uint32_t			count;
		Vertex::INDEX::type	type;
	};
	MemoryMap<IndexBufferMapInfo>	indexMemoryMap;

public:
	GpuBuffer						uniformBuffer;
	struct UniformBufferMapInfo
	{
		// lists all descriptor names that reference this uniform (and if it's up to date)
		struct DescriptorStatus
		{
			bool upToDate;
		};
		struct Names
		{
			std::map<std::string, DescriptorStatus> name;
		};
		std::map<size_t, Names> bindings;
	};
	MemoryMap<UniformBufferMapInfo>	uniformMemoryMap;

	std::vector<std::string> outdatedUniforms;

public:
	TransferManager	transferManager;

	void Init(Device _device, VkCommandPool _commandPool, VkQueue _queue, VkDeviceSize _transferBufferSize, VkDeviceSize _vertexBufferSize, VkDeviceSize _indexBufferSize, VkDeviceSize _uniformBufferSize)
	{
		VkCommandBuffer commandBuffer;
		VkCommandBufferAllocateInfo commandBufferAllocateInfo;
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = _commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;
		VkU::AllocateCommandBuffers(_device.handle, &commandBufferAllocateInfo, &commandBuffer);

		transferManager.Init(_device, commandBuffer, _queue, _transferBufferSize);

		vertexMemoryMap.Init(_vertexBufferSize);
		vertexBuffer.Init(_device, _vertexBufferSize, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		indexMemoryMap.Init(_indexBufferSize);
		indexBuffer.Init(_device, _indexBufferSize, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		uniformMemoryMap.Init(_uniformBufferSize);
		uniformBuffer.Init(_device, _uniformBufferSize, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	}
	void ShutDown(VkDevice _device)
	{
		uniformBuffer.ShutDown(_device);
		indexBuffer.ShutDown(_device);
		vertexBuffer.ShutDown(_device);

		uniformMemoryMap.ShutDown();
		indexMemoryMap.ShutDown();
		vertexMemoryMap.ShutDown();

		transferManager.ShutDown(_device);
	}

	struct VerticeBufferInfo
	{
		VkBuffer		buffer;
		VkDeviceSize	offset;
	};
	VerticeBufferInfo GetVerticeBufferInfo(const char* _name)
	{
		MemoryMap<VertexBufferMapInfo>::MemoryInfo vertexMapInfo = vertexMemoryMap.Get(_name);

		VerticeBufferInfo verticeBufferInfo;
		verticeBufferInfo.buffer = vertexBuffer.handle;
		verticeBufferInfo.offset = vertexMapInfo.offset;

		return verticeBufferInfo;
	}
	struct IndiceBufferInfo
	{
		VkBuffer		buffer;
		VkDeviceSize	offset;
		uint32_t		count;
	};
	IndiceBufferInfo GetIndiceBufferInfo(const char* _name)
	{
		IndiceBufferInfo indiceBufferInfo;

		MemoryMap<IndexBufferMapInfo>::MemoryInfo indexMapInfo = indexMemoryMap.Get(_name);

		indiceBufferInfo.buffer = indexBuffer.handle;
		indiceBufferInfo.offset = indexMapInfo.offset;
		indiceBufferInfo.count = indexMapInfo.userdataPtr->count;

		return indiceBufferInfo;
	}

	// Memory
	void AddVertex(VkDevice _device, const char* _name, void* _data, size_t _dataSize, Vertex::VERTEXTYPE::vertextype _vertexDatatype)
	{
		VertexBufferMapInfo vertexBufferMapInfo;
		vertexBufferMapInfo.structure = Vertex::STRUCTURE::GetStructure(_vertexDatatype);

		VkDeviceSize offset = vertexMemoryMap.Allocate(_name, _dataSize, vertexBufferMapInfo);

		transferManager.TransferBuffer(_device, _data, _dataSize, vertexBuffer.handle, offset);
	}
	void AddVertex(VkDevice _device, const char* _name, void* _data, size_t _dataSize, Vertex::STRUCTURE _structure)
	{
		VertexBufferMapInfo vertexBufferMapInfo;
		vertexBufferMapInfo.structure = _structure;

		VkDeviceSize offset = vertexMemoryMap.Allocate(_name, _dataSize, vertexBufferMapInfo);

		transferManager.TransferBuffer(_device, _data, _dataSize, vertexBuffer.handle, offset);
	}
	void AddIndex(VkDevice _device, const char* _name, void* _data, size_t _dataSize, uint32_t _count, Vertex::INDEX::type _type)
	{
		IndexBufferMapInfo indexBufferMapInfo;
		indexBufferMapInfo.count = _count;
		indexBufferMapInfo.type = _type;

		VkDeviceSize offset = indexMemoryMap.Allocate(_name, _dataSize, indexBufferMapInfo);

		transferManager.TransferBuffer(_device, _data, _dataSize, indexBuffer.handle, offset);
	}

	void AddMesh(VkDevice _device, const char* _name, const char* _filename, Vertex::VERTEXTYPE::vertextype _vertexDatatype, bool _includeVertex, bool _includeIndex)
	{
		Loader::Data3D data3D;
		Loader::LoadData3D(_filename, data3D, _vertexDatatype);
	
		if(_includeVertex)
			AddVertex(_device, _name, data3D.meshes[0].vertexData, data3D.meshes[0].vertexSize, data3D.meshes[0].vertexType);

		if(_includeIndex)
			AddIndex(_device, _name, data3D.meshes[0].indexData, data3D.meshes[0].indexCount * Vertex::INDEX::GetSize(data3D.meshes[0].indexType), data3D.meshes[0].indexCount, data3D.meshes[0].indexType);
	
		Loader::UnloadData3D(data3D);
	}

	void AddUniform(VkDevice _device, const char* _name, void* _data, size_t _dataSize)
	{
		VkDeviceSize offset = uniformMemoryMap.Allocate(_name, _dataSize, {});
		if (offset == -1)
			return;

		transferManager.TransferBuffer(_device, _data, _dataSize, uniformBuffer.handle, offset);

		outdatedUniforms.push_back(_name);
	}

	MemoryMap<UniformBufferMapInfo>::MemoryInfo GetUniformBufferMapInfo(const char* _name)
	{
		return uniformMemoryMap.Get(_name);
	}

	void UpdateOutdatedUniformsDescriptorSets()
	{

	}
};

#endif