#include <katra/buffer/buffer.h>

Buffer::Buffer(
    LogicalDevice* device, 
    uint32_t size, 
    VkBufferUsageFlags usage, 
    VkMemoryPropertyFlags memoryType,
    VkSharingMode sharingMode, 
    VkFlags flags
): device(device), size(size), usage(usage), memoryType(memoryType), sharingMode(sharingMode), flags(flags) {
    setCreateInfo();
    
    VkResult result = vkCreateBuffer(device->getHandle(), &createInfo, nullptr, &buffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }
    
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device->getHandle(), buffer, &memoryRequirements);
    allocation = new Allocation(device, memoryRequirements, memoryType);

    result = vkBindBufferMemory(device->getHandle(), buffer, allocation->getHandle(), 0);
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

void Buffer::write(const void* data, uint32_t size, uint32_t offset) {
    if (!mapped) {
        vkMapMemory(device->getHandle(), allocation->getHandle(), 0, size, 0, &mappedMemory);
    }
    memcpy((char*)mappedMemory + offset, data, size);
    if (!mapped) {
        vkUnmapMemory(device->getHandle(), allocation->getHandle());
    }
}

void Buffer::mapMemory() {
    vkMapMemory(device->getHandle(), allocation->getHandle(), 0, size, 0, &mappedMemory);
    mapped = true;
}

void Buffer::unmapMemory() {
    vkUnmapMemory(device->getHandle(), allocation->getHandle());
    mapped = false;
}

Buffer::~Buffer() {
    if (buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device->getHandle(), buffer, nullptr);
        buffer = VK_NULL_HANDLE;
    }
    if (allocation != nullptr) {
        delete allocation;
        allocation = nullptr;
    }
}