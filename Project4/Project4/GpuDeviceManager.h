#ifndef	GPU_DEVICE_MANAGER_H
#define GPU_DEVICE_MANAGER_H

#include "../../Common/Other/VkU.h"
#include "Device.h"

class GpuDeviceManager
{
public:
	Device device;

	void Init(PhysicalDevice* _physicalDevice, std::vector<const char*> _deviceExtensions, VkPhysicalDeviceFeatures _features, std::vector<Device::QueueFamilyRequirements> _queueFamilyRequirements, std::vector<VkSurfaceKHR> _surfaces)
	{
		device.Init(_physicalDevice, _deviceExtensions, _features, _queueFamilyRequirements, _surfaces);
	}
	void ShutDown()
	{
		device.ShutDown();
	}
};

#endif