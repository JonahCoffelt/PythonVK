#ifndef KATRA_DESCRIPTOR_POOL_H
#define KATRA_DESCRIPTOR_POOL_H

// Includes
#include <katra/util/includes.h>
#include <katra/device/logical_device.h>


// Structs
struct DescriptorPoolSize {
    VkDescriptorType type;
    uint32_t count;
};


// Class Declaration
class DescriptorPool {
    private:
        LogicalDevice* device;
        uint32_t maxSets;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

        std::vector<VkDescriptorPoolSize> poolSizes;
        VkDescriptorPoolCreateInfo createInfo{};

        void setCreateInfo();

    public:
        DescriptorPool(LogicalDevice* device, uint32_t maxSets, std::vector<DescriptorPoolSize> poolSizes);
        ~DescriptorPool();
        DescriptorPool(const DescriptorPool&) = delete;
        DescriptorPool& operator=(const DescriptorPool&) = delete;

        inline VkDescriptorPool getHandle() { return descriptorPool; }
        inline const VkDescriptorPoolCreateInfo& getCreateInfo() const { return createInfo; }
        inline uint32_t getMaxSets() { return maxSets; }
        inline const std::vector<VkDescriptorPoolSize>& getPoolSizes() const { return poolSizes; }
        inline LogicalDevice* getDevice() { return device; }
};

#endif

