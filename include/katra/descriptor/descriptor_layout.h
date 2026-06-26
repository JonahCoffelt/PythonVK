#ifndef KATRA_DESCRIPTOR_LAYOUT_H
#define KATRA_DESCRIPTOR_LAYOUT_H

// Includes
#include <katra/util/includes.h>
#include <katra/device/logical_device.h>


// Structs
struct DescriptorLayoutBinding {
    uint32_t binding;
    uint32_t count;
    VkDescriptorType type;
    VkShaderStageFlags stageFlags;
};


// Class Declaration
class DescriptorLayout {
    private:
        LogicalDevice* device;

        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSetLayoutCreateInfo createInfo{};

        std::vector<VkDescriptorSetLayoutBinding> bindings;

        void setCreateInfo();

    public:
        DescriptorLayout(LogicalDevice* device, std::vector<DescriptorLayoutBinding> bindings);
        ~DescriptorLayout();
        DescriptorLayout(const DescriptorLayout&) = delete;
        DescriptorLayout& operator=(const DescriptorLayout&) = delete;

        inline const VkDescriptorSetLayout& getHandle() const { return descriptorSetLayout; }
        inline const VkDescriptorSetLayoutCreateInfo& getCreateInfo() const { return createInfo; }
};

#endif