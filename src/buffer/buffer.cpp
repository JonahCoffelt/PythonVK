#include <katra/buffer/buffer.h>

Buffer::Buffer(
    LogicalDevice* device, 
    uint32_t size, 
    VkBufferUsageFlags usage, 
    VkSharingMode sharingMode, 
    VkFlags flags
): device(device), size(size), usage(usage), sharingMode(sharingMode), flags(flags) {
    setCreateInfo();
    
    VkResult result = vkCreateBuffer(device->getHandle(), &createInfo, nullptr, &buffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }
    
    setMemoryInfo();
    result = vkAllocateMemory(device->getHandle(), &memoryInfo, nullptr, &memory);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }

    result = vkBindBufferMemory(device->getHandle(), buffer, memory, 0);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to bind buffer memory!");
    }
}

void Buffer::setCreateInfo() {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = usage;
    createInfo.sharingMode = sharingMode;
    createInfo.flags = flags;
}

void Buffer::setMemoryInfo() {
    vkGetBufferMemoryRequirements(device->getHandle(), buffer, &memoryRequirements);

    memoryInfo = {};
    memoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryInfo.allocationSize = memoryRequirements.size;
    memoryInfo.memoryTypeIndex = device->getPhysicalDevice()->findMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void Buffer::write(const void* data, uint32_t size, uint32_t offset) {
    void* mappedData;
    vkMapMemory(device->getHandle(), memory, offset, size, 0, &mappedData);
    memcpy(mappedData, data, size);
    vkUnmapMemory(device->getHandle(), memory);
}

Buffer::~Buffer() {
    if (buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device->getHandle(), buffer, nullptr);
    }
    if (memory != VK_NULL_HANDLE) {
        vkFreeMemory(device->getHandle(), memory, nullptr);
    }
}