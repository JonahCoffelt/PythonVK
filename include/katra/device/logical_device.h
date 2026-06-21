#ifndef KATRA_LOGICAL_DEVICE_H
#define KATRA_LOGICAL_DEVICE_H

// Includes
#include <katra/util/includes.h>
#include <katra/device/physical_device.h>


// Forward Declarations
class Semaphore;


// Class Declaration
class LogicalDevice {
    private:
        VkDevice device = VK_NULL_HANDLE;
        PhysicalDevice* physicalDevice;

        VkDeviceCreateInfo createInfo{};
        VkPhysicalDeviceFeatures deviceFeatures{};
        std::vector<const char*> enabledExtensions;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        std::vector<uint32_t> queueFamilyIndices;
        std::optional<VkQueue> graphicsQueue;
        std::optional<VkQueue> presentQueue;
        std::optional<VkQueue> computeQueue;
        std::optional<VkQueue> transferQueue;
        std::optional<VkQueue> sparseBindingQueue;

        void setDeviceCreateInfo();
        void setQueueFamilyIndices();
        void setQueueCreateInfos();
        void setDeviceFeatures();
        void setEnabledExtensions(std::vector<const char*> requiredExtensions, std::vector<const char*> preferredExtensions);
        void retrieveQueues();

    public:
        LogicalDevice(PhysicalDevice* physicalDevice, std::vector<const char*> requiredExtensions, std::vector<const char*> preferredExtensions);
        ~LogicalDevice();
        LogicalDevice(const LogicalDevice&) = delete;
        LogicalDevice& operator=(const LogicalDevice&) = delete;

        inline VkDevice getHandle() { return device; }
        inline PhysicalDevice* getPhysicalDevice() { return physicalDevice; }
        inline const std::vector<uint32_t>& getQueueFamilyIndices() const { return queueFamilyIndices; }
        inline const VkDeviceCreateInfo& getCreateInfo() const { return createInfo; }
        inline const VkPhysicalDeviceFeatures& getDeviceFeatures() const { return deviceFeatures; }
        inline const std::vector<const char*>& getEnabledExtensions() const { return enabledExtensions; }
        inline const std::vector<VkDeviceQueueCreateInfo>& getQueueCreateInfos() const { return queueCreateInfos; }

        void waitIdle();
        void present(VkSwapchainKHR swapChain, uint32_t imageIndex, std::vector<Semaphore*> waitSemaphores = {});

        inline bool hasGraphicsQueue() { return graphicsQueue.has_value(); }
        inline bool hasPresentQueue() { return presentQueue.has_value(); }
        inline bool hasComputeQueue() { return computeQueue.has_value(); }
        inline bool hasTransferQueue() { return transferQueue.has_value(); }
        inline bool hasSparseBindingQueue() { return sparseBindingQueue.has_value(); }
        inline uint32_t getGraphicsFamilyIndex() { return hasGraphicsQueue() ? physicalDevice->getGraphicsFamilyIndex() : 0; }
        inline uint32_t getPresentFamilyIndex() { return hasPresentQueue() ? physicalDevice->getPresentFamilyIndex() : 0; }
        inline uint32_t getComputeFamilyIndex() { return hasComputeQueue() ? physicalDevice->getComputeFamilyIndex() : 0; }
        inline uint32_t getTransferFamilyIndex() { return hasTransferQueue() ? physicalDevice->getTransferFamilyIndex() : 0; }
        inline uint32_t getSparseBindingFamilyIndex() { return hasSparseBindingQueue() ? physicalDevice->getSparseBindingFamilyIndex() : 0; }
        inline VkQueue getGraphicsQueue() { return hasGraphicsQueue() ? graphicsQueue.value() : nullptr; }
        inline VkQueue getPresentQueue() { return hasPresentQueue() ? presentQueue.value() : nullptr; }
        inline VkQueue getComputeQueue() { return hasComputeQueue() ? computeQueue.value() : nullptr; }
        inline VkQueue getTransferQueue() { return hasTransferQueue() ? transferQueue.value() : nullptr; }
        inline VkQueue getSparseBindingQueue() { return hasSparseBindingQueue() ? sparseBindingQueue.value() : nullptr; }
};

#endif