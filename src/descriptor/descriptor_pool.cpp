#include <katra/descriptor/descriptor_pool.h>

DescriptorPool::DescriptorPool(LogicalDevice* device, uint32_t maxSets, std::vector<DescriptorPoolSize> poolSizes): device(device), maxSets(maxSets) {
    
    // Convert the pool sizes to the format expected by the Vulkan API
    for (const DescriptorPoolSize& poolSizeStruct : poolSizes) {
        VkDescriptorPoolSize poolSize {};
        poolSize.type = poolSizeStruct.type;
        poolSize.descriptorCount = poolSizeStruct.count;
        this->poolSizes.push_back(poolSize);
    }

    // Set the create info
    setCreateInfo();

    // Create the descriptor pool
    VkResult result = vkCreateDescriptorPool(device->getHandle(), &createInfo, nullptr, &descriptorPool);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void DescriptorPool::setCreateInfo() {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();
    createInfo.maxSets = maxSets;
}


DescriptorPool::~DescriptorPool() {
    if (descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(device->getHandle(), descriptorPool, nullptr);
    }
}