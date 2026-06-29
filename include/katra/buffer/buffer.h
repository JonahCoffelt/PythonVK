#ifndef KATRA_BUFFER_H
#define KATRA_BUFFER_H

// Includes
#include <katra/util/includes.h>
#include <katra/device/logical_device.h>
#include <katra/memory/allocation.h>


// Class Declaration
class Buffer {
    private:
        LogicalDevice* device;

        VkBuffer buffer = VK_NULL_HANDLE;
        Allocation* allocation;
        void* mappedMemory = nullptr;
        bool mapped = false;

        uint32_t size;
        VkBufferUsageFlags usage;
        VkMemoryPropertyFlags memoryType;
        VkSharingMode sharingMode;
        VkFlags flags;

        VkBufferCreateInfo createInfo{};

        void setCreateInfo();

    public:
        Buffer(
            LogicalDevice* device, 
            uint32_t size, 
            VkBufferUsageFlags usage, 
            VkMemoryPropertyFlags memoryType = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, 
            VkFlags flags = 0
        );
        ~Buffer();
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        void write(const void* data, uint32_t size, uint32_t offset = 0);
        void mapMemory();
        void unmapMemory();

        inline VkBuffer getHandle() { return buffer; }
        inline Allocation* getAllocation() { return allocation; }
        inline LogicalDevice* getLogicalDevice() { return device; }
        inline void* getMappedMemoryPointer() { return mappedMemory; }
        inline const VkBufferCreateInfo& getCreateInfo() const { return createInfo; }
        inline uint32_t getSize() { return size; }
        inline VkBufferUsageFlags getUsage() { return usage; }
        inline VkSharingMode getSharingMode() { return sharingMode; }
        inline VkFlags getFlags() { return flags; }
};
    

#endif