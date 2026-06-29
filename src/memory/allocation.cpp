#include <katra/memory/allocation.h>


Allocation::Allocation(
    LogicalDevice* device, 
    VkMemoryRequirements memoryRequirements, 
    VkMemoryPropertyFlags properties
): device(device), memoryRequirements(memoryRequirements), properties(properties) {
    
    setAllocateInfo();

    // Allocate memory
    VkResult result = vkAllocateMemory(device->getHandle(), &allocateInfo, nullptr, &memory);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
}

void Allocation::setAllocateInfo() {
    allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = device->getPhysicalDevice()->findMemoryType(memoryRequirements.memoryTypeBits, properties);
}

Allocation::~Allocation() {
    if (memory != VK_NULL_HANDLE) {
        vkFreeMemory(device->getHandle(), memory, nullptr);
    }
}