#include <katra/descriptor/descriptor_set.h>

/**
 * @brief Construct a new DescriptorSet object
 * 
 */
DescriptorSet::DescriptorSet(DescriptorPool* pool, DescriptorLayout* layout): pool(pool), layout(layout) {
    // Get the device from the pool
    device = pool->getDevice();
    
    // Set the allocate info
    setAllocateInfo();

    VkResult result = vkAllocateDescriptorSets(device->getHandle(), &allocateInfo, &descriptorSet);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor set!");
    }
}

void DescriptorSet::setAllocateInfo() {
    allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = pool->getHandle();
    allocateInfo.pSetLayouts = &layout->getHandle();
    allocateInfo.descriptorSetCount = 1;
}

void DescriptorSet::update(Buffer* buffer, uint32_t binding, VkDescriptorType descriptorType) {
    // Set the buffer info
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer->getHandle();
    bufferInfo.offset = 0;
    bufferInfo.range = buffer->getSize();
    
    // Set the write descriptor set
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = descriptorType;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr; // Optional
    descriptorWrite.pTexelBufferView = nullptr; // Optional

    // Update the descriptor set
    vkUpdateDescriptorSets(device->getHandle(), 1, &descriptorWrite, 0, nullptr);
}

DescriptorSet::~DescriptorSet() {
    // Descriptor sets are freed when the owning DescriptorPool is destroyed.
}