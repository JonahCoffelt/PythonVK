#include <pythonvk/render/framebuffer.h>

/**
 * @brief Construct a new Framebuffer object
 * 
 * @param device Logical device
 * @param renderPass Render pass
 * @param attachments Attachments
 * @param layers Layers
 */
Framebuffer::Framebuffer(LogicalDevice* device, RenderPass* renderPass, std::vector<VkImageView> attachments, uint32_t layers): device(device), renderPass(renderPass) {
    // Get swap chain from render pass
    this->swapChain = renderPass->getSwapChain();
    
    // Set attachments, extent, and layers
    this->attachments = attachments;
    this->extent = swapChain->getExtent();
    this->layers = layers;
    
    // Set framebuffer info
    setFramebufferInfo();

    // Create framebuffer
    VkResult result = vkCreateFramebuffer(device->getHandle(), &framebufferInfo, nullptr, &framebuffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create framebuffer!");
    }
}

/**
 * @brief Set the framebuffer creation info
 * 
 */
void Framebuffer::setFramebufferInfo() {
    framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass->getHandle();
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = layers;
}

/**
 * @brief Destroy the Framebuffer object
 * 
 */
Framebuffer::~Framebuffer() {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(device->getHandle(), framebuffer, nullptr);
    }
}