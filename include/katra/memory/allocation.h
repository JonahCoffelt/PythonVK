#ifndef KATRA_MEMORY_ALLOCATION_H
#define KATRA_MEMORY_ALLOCATION_H

// Includes
#include <katra/util/includes.h>
#include <katra/device/logical_device.h>


// Class Declaration
class Allocation {
    private:
        VkDeviceMemory memory;
        LogicalDevice* device;

        VkMemoryAllocateInfo allocateInfo;
        VkMemoryRequirements memoryRequirements;
        VkMemoryPropertyFlags properties;

        void setAllocateInfo();

    public:
        Allocation(LogicalDevice* device, VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags properties);
        ~Allocation();
        Allocation(const Allocation&) = delete;
        Allocation& operator=(const Allocation&) = delete;

        inline VkDeviceMemory getHandle() { return memory; }
        inline LogicalDevice* getDevice() { return device; }
};

#endif