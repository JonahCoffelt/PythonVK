#include <katra/render/render_pass.h>

RenderPass::RenderPass(
    SwapChain* swapChain,
    ImageView* depthImageView,
    ImageView* multisampleColorImageView,
    VkSampleCountFlagBits msaaSamples
): device(swapChain->getLogicalDevice()),
   swapChain(swapChain),
   depthImageView(depthImageView),
   multisampleColorImageView(multisampleColorImageView),
   msaaSamples(msaaSamples) {
    setAttachmentDescription();
    setAttachmentReference();
    if (depthImageView) {
        setDepthAttachmentDescription();
        setDepthAttachmentReference();
    }
    if (multisampleColorImageView) {
        setResolveAttachmentDescription();
        setResolveAttachmentReference();
    }
    setSubpassDescription();
    setSubpassDependency();
    setRenderPassCreateInfo();

    VkResult result = vkCreateRenderPass(device->getHandle(), &renderPassCreateInfo, nullptr, &renderPass);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void RenderPass::setRenderPassCreateInfo() {
    attachments.clear();
    attachments.push_back(attachmentDescription);
    if (depthImageView) {
        attachments.push_back(depthAttachmentDescription);
    }
    if (multisampleColorImageView) {
        attachments.push_back(resolveAttachmentDescription);
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

void RenderPass::setAttachmentDescription() {
    attachmentDescription = {};
    attachmentDescription.format = swapChain->getSurfaceFormat().format;
    attachmentDescription.samples = multisampleColorImageView ? msaaSamples : VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = multisampleColorImageView
        ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}

void RenderPass::setAttachmentReference() {
    attachmentReference = {};
    attachmentReference.attachment = 0;
    attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

void RenderPass::setDepthAttachmentDescription() {
    depthAttachmentDescription = {};
    depthAttachmentDescription.format = depthImageView->getFormat();
    depthAttachmentDescription.samples = multisampleColorImageView ? msaaSamples : VK_SAMPLE_COUNT_1_BIT;
    depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void RenderPass::setDepthAttachmentReference() {
    depthAttachmentReference = {};
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void RenderPass::setResolveAttachmentDescription() {
    resolveAttachmentDescription = {};
    resolveAttachmentDescription.format = swapChain->getSurfaceFormat().format;
    resolveAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    resolveAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    resolveAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    resolveAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    resolveAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    resolveAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    resolveAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}

void RenderPass::setResolveAttachmentReference() {
    resolveAttachmentReference = {};
    resolveAttachmentReference.attachment = 2;
    resolveAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

void RenderPass::setSubpassDescription() {
    subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &attachmentReference;
    if (depthImageView) {
        subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
    }
    if (multisampleColorImageView) {
        subpassDescription.pResolveAttachments = &resolveAttachmentReference;
    }
}

void RenderPass::setSubpassDependency() {
    subpassDependency = {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;

    if (depthImageView == nullptr) {
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.srcAccessMask = 0;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        return;
    }

    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    subpassDependency.srcAccessMask = multisampleColorImageView
        ? VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
        : VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
}

RenderPass::~RenderPass() {
    if (renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device->getHandle(), renderPass, nullptr);
    }
}
