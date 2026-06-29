#include <katra/render/render_pass.h>

/**
 * @brief Construct a new Render Pass object
 * 
 */
RenderPass::RenderPass(SwapChain* swapChain, ImageView* depthImageView): device(swapChain->getLogicalDevice()), swapChain(swapChain), depthImageView(depthImageView) {
    // Set the attachment description, reference, subpass description, and subpass dependency
    setAttachmentDescription();
    setAttachmentReference();
    if (depthImageView) {
        setDepthAttachmentDescription();
        setDepthAttachmentReference();
    }
    setSubpassDescription();
    setSubpassDependency();
    setRenderPassCreateInfo();

    // Create the render pass
    VkResult result = vkCreateRenderPass(device->getHandle(), &renderPassCreateInfo, nullptr, &renderPass);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

/**
 * @brief Set the render pass create info
 * 
 */
void RenderPass::setRenderPassCreateInfo() {
    attachments.clear();
    attachments.push_back(attachmentDescription);
    if (depthImageView) {
        attachments.push_back(depthAttachmentDescription);
    }
    
    renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;
}

/**
 * @brief Set the attachment description
 * 
 */
void RenderPass::setAttachmentDescription() {
    attachmentDescription = {};
    // Formatting
    attachmentDescription.format = swapChain->getSurfaceFormat().format;
    // Multisample (none for now)
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    // Load and store ops
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // Image layouts
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}

/**
 * @brief Set the attachment reference
 * 
 */
void RenderPass::setAttachmentReference() {
    attachmentReference = {};
    attachmentReference.attachment = 0;
    attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

void RenderPass::setDepthAttachmentDescription() {
    depthAttachmentDescription = {};
    depthAttachmentDescription.format = depthImageView->getFormat();
    depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void RenderPass::setDepthAttachmentReference() {
    depthAttachmentReference = {};
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}


/**
 * @brief Set the subpass description
 * 
 */
void RenderPass::setSubpassDescription() {
    subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &attachmentReference;
    if (depthImageView) {
        subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
    }
}

/**
 * @brief Set the subpass dependency
 * 
 */
void RenderPass::setSubpassDependency() {
    subpassDependency = {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    if (depthImageView == nullptr) {
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.srcAccessMask = 0;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;        
    }
    else {
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        subpassDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
}

/**
 * @brief Destroy the Render Pass object
 * 
 */
RenderPass::~RenderPass() {
    if (renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device->getHandle(), renderPass, nullptr);
    }
}