#ifndef	GPU_INSTANCE_MANAGER_H
#define GPU_INSTANCE_MANAGER_H

#include "../../Common/Other/VkU.h" 
#include "Swapchain.h"
#include "PhysicalDevice.h"

class GpuInstanceManager
{
public:
	VkInstance					handle = VK_NULL_HANDLE;
	VkDebugReportCallbackEXT	debugReportCallback = VK_NULL_HANDLE;
	Swapchain					swapchain;
	std::vector<PhysicalDevice>	physicalDevices;

	struct InstanceInfo
	{
		VkApplicationInfo			appInfo;
		std::vector<const char*>	instanceLayers;
		std::vector<const char*>	instanceExtensions;
	};
	struct DebugInfo
	{
		VkDebugReportFlagsEXT			debugFlags;
		PFN_vkDebugReportCallbackEXT	debugCallback;
	};
	struct SurfaceInfo
	{
		const char*	name;
		const char*	title;
		WNDPROC		wndProc;
		int			width;
		int			height;
	};
	void Init(InstanceInfo _instanceInfo, DebugInfo _debugInfo, SurfaceInfo _surfaceInfo)
	{
		/// INSTANCE
		VkValidationCheckEXT disabled_check = VK_VALIDATION_CHECK_ALL_EXT;
		VkValidationFlagsEXT validationFlags;
		validationFlags.sType = VK_STRUCTURE_TYPE_VALIDATION_FLAGS_EXT;
		validationFlags.pNext = nullptr;
		validationFlags.disabledValidationCheckCount = 1;
		validationFlags.pDisabledValidationChecks = &disabled_check;
		VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo;
		debugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		debugReportCallbackCreateInfo.pNext = (void*)&validationFlags;
		debugReportCallbackCreateInfo.flags = _debugInfo.debugFlags;
		debugReportCallbackCreateInfo.pfnCallback = _debugInfo.debugCallback;
		debugReportCallbackCreateInfo.pUserData = nullptr;
		VkInstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		if (_debugInfo.debugFlags == 0 || _debugInfo.debugCallback == nullptr)
			instanceCreateInfo.pNext = nullptr;
		else
			instanceCreateInfo.pNext = &debugReportCallbackCreateInfo;
		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &_instanceInfo.appInfo;
		instanceCreateInfo.enabledLayerCount = (uint32_t)_instanceInfo.instanceLayers.size();
		instanceCreateInfo.ppEnabledLayerNames = _instanceInfo.instanceLayers.data();
		instanceCreateInfo.enabledExtensionCount = (uint32_t)_instanceInfo.instanceExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = _instanceInfo.instanceExtensions.data();
		VkU::CreateInstance(&instanceCreateInfo, handle);

		/// DEBUG
		debugReportCallbackCreateInfo.pNext = nullptr;
		if (!(_debugInfo.debugFlags == 0 || _debugInfo.debugCallback == nullptr))
			VkU::CreateDebugReportCallbackEXT(handle, &debugReportCallbackCreateInfo, debugReportCallback);

		/// SURFACE
		swapchain.InitSurface(_surfaceInfo.name, _surfaceInfo.title, _surfaceInfo.wndProc, _surfaceInfo.width, _surfaceInfo.height, handle);

		/// PHYSICAL DEVICE
		std::vector<VkPhysicalDevice> vkPhysicalDevices;
		VkU::EnumeratePhysicalDevices(handle, vkPhysicalDevices);

		physicalDevices.resize(vkPhysicalDevices.size());
		for (size_t iPhysicalDevice = 0; iPhysicalDevice != physicalDevices.size(); ++iPhysicalDevice)
			physicalDevices[iPhysicalDevice].Init(vkPhysicalDevices[iPhysicalDevice], { swapchain.surface }, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_X8_D24_UNORM_PACK32, VK_FORMAT_D16_UNORM, VK_FORMAT_D16_UNORM_S8_UINT }); // HARDCODED
	}
	void ShutDown()
	{
		swapchain.ShutDownSurface(handle);

		PFN_vkDestroyDebugReportCallbackEXT FP_vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(handle, "vkDestroyDebugReportCallbackEXT");
		FP_vkDestroyDebugReportCallbackEXT(handle, debugReportCallback, nullptr);

		vkDestroyInstance(handle, nullptr);
	}
};

#endif