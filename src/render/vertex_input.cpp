#include <katra/render/vertex_input.h>

/**
 * @brief Construct a new Vertex Input object
 * 
 */
VertexInput::VertexInput(LogicalDevice* device, uint32_t binding, uint32_t stride, VkVertexInputRate inputRate, std::vector<VertexAttribute> attributes): device(device) {
    bindingDescription.binding = binding;
    bindingDescription.stride = stride;
    bindingDescription.inputRate = inputRate;

    attributeDescriptions.clear();
    for (VertexAttribute attribute : attributes) {
        VkVertexInputAttributeDescription attributeDescription{};
        attributeDescription.binding = attribute.binding;
        attributeDescription.location = attribute.location;
        attributeDescription.format = attribute.format;
        attributeDescription.offset = attribute.offset;
        attributeDescriptions.push_back(attributeDescription);
    }

    setCreateInfo();
}

void VertexInput::setCreateInfo() {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.vertexBindingDescriptionCount = 1;
    createInfo.pVertexBindingDescriptions = &bindingDescription;
    createInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    createInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
}

VertexInput::~VertexInput() {
    
}