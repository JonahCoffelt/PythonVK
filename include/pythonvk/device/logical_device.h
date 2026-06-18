#ifndef PYVK_LOGICAL_DEVICE_H
#define PYVK_LOGICAL_DEVICE_H

// Includes
#include <pythonvk/util/includes.h>
#include <pythonvk/device/physical_device.h>


// Class Declaration
class LogicalDevice {
    private:
        VkDevice device = VK_NULL_HANDLE;
        PhysicalDevice* physicalDevice;

        std::optional<VkQueue> graphicsQueue;
        std::optional<VkQueue> presentQueue;
        std::optional<VkQueue> computeQueue;
        std::optional<VkQueue> transferQueue;
        std::optional<VkQueue> sparseBindingQueue;

        VkDeviceCreateInfo getDeviceCreateInfo(
            const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
            const VkPhysicalDeviceFeatures& deviceFeatures,
            const std::vector<const char*>& enabledExtensions
        );
        std::set<uint32_t> getUniqueQueueFamilies();
        std::vector<VkDeviceQueueCreateInfo> getQueueCreateInfos();
        void retrieveQueues();

    public:
        LogicalDevice(PhysicalDevice* physicalDevice, std::vector<const char*> requiredExtensions, std::vector<const char*> preferredExtensions);
        ~LogicalDevice();
        LogicalDevice(const LogicalDevice&) = delete;
        LogicalDevice& operator=(const LogicalDevice&) = delete;

        inline VkDevice getHandle() { return device; }
        inline PhysicalDevice* getPhysicalDevice() { return physicalDevice; }

        void waitIdle();

        inline bool hasGraphicsQueue() { return graphicsQueue.has_value(); }
        inline bool hasPresentQueue() { return presentQueue.has_value(); }
        inline bool hasComputeQueue() { return computeQueue.has_value(); }
        inline bool hasTransferQueue() { return transferQueue.has_value(); }
        inline bool hasSparseBindingQueue() { return sparseBindingQueue.has_value(); }
        inline uint32_t getGraphicsFamilyIndex() { return physicalDevice->getGraphicsFamilyIndex(); }
        inline uint32_t getPresentFamilyIndex() { return physicalDevice->getPresentFamilyIndex(); }
        inline uint32_t getComputeFamilyIndex() { return physicalDevice->getComputeFamilyIndex(); }
        inline uint32_t getTransferFamilyIndex() { return physicalDevice->getTransferFamilyIndex(); }
        inline uint32_t getSparseBindingFamilyIndex() { return physicalDevice->getSparseBindingFamilyIndex(); }
        inline VkQueue getGraphicsQueue() { return graphicsQueue.value(); }
        inline VkQueue getPresentQueue() { return presentQueue.value(); }
        inline VkQueue getComputeQueue() { return computeQueue.value(); }
        inline VkQueue getTransferQueue() { return transferQueue.value(); }
        inline VkQueue getSparseBindingQueue() { return sparseBindingQueue.value(); }
};

#endif