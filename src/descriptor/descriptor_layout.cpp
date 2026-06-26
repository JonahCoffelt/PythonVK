#include <katra/descriptor/descriptor_layout.h>


DescriptorLayout::DescriptorLayout(LogicalDevice* device, std::vector<DescriptorLayoutBinding> bindings): device(device) {
    // Convert DescriptorLayoutBinding to VkDescriptorSetLayoutBinding
    for (const DescriptorLayoutBinding& binding : bindings) {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding.binding;
        layoutBinding.descriptorType = binding.type;
        layoutBinding.descriptorCount = binding.count;
        layoutBinding.stageFlags = binding.stageFlags;
        layoutBinding.pImmutableSamplers = nullptr;
        this->bindings.push_back(layoutBinding);
    }

    // Set the create info
    setCreateInfo();

    // Create the descriptor set layout
    VkResult result = vkCreateDescriptorSetLayout(device->getHandle(), &createInfo, nullptr, &descriptorSetLayout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void DescriptorLayout::setCreateInfo() {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pBindings = bindings.data();
    createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
}

DescriptorLayout::~DescriptorLayout() {
    if (descriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device->getHandle(), descriptorSetLayout, nullptr);
    }
}