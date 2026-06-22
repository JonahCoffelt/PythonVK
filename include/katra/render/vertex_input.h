#ifndef KATRA_VERTEX_INPUT_H
#define KATRA_VERTEX_INPUT_H

#include <katra/util/includes.h>
#include <katra/device/logical_device.h>

// Structs
struct VertexAttribute {
    uint32_t binding;
    uint32_t location;
    VkFormat format;
    uint32_t offset;
};

class VertexInput {
    private:
        LogicalDevice* device;
        VkPipelineVertexInputStateCreateInfo createInfo;

        VkVertexInputBindingDescription bindingDescription;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        void setCreateInfo();

    public:
        VertexInput(LogicalDevice* device, uint32_t binding, uint32_t stride, VkVertexInputRate inputRate, std::vector<VertexAttribute> attributes);
        ~VertexInput();
        VertexInput(const VertexInput&) = delete;
        VertexInput& operator=(const VertexInput&) = delete;

        inline const VkPipelineVertexInputStateCreateInfo& getCreateInfo() const { return createInfo; }
        inline const VkVertexInputBindingDescription& getBindingDescription() const { return bindingDescription; }
        inline const std::vector<VkVertexInputAttributeDescription>& getAttributeDescriptions() const { return attributeDescriptions; }
};

#endif