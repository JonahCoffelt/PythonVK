#ifndef PYVK_RENDER_PASS_H
#define PYVK_RENDER_PASS_H

// Includes
#include <pythonvk/util/includes.h>
#include <pythonvk/device/logical_device.h>
#include <pythonvk/render/swap_chain.h>


// Class Declaration
class RenderPass {
    private:
        VkRenderPass renderPass = VK_NULL_HANDLE;
        LogicalDevice* device;
        SwapChain* swapChain;

        VkRenderPassCreateInfo renderPassCreateInfo;
        VkAttachmentDescription attachmentDescription;
        VkAttachmentReference attachmentReference;
        VkSubpassDescription subpassDescription;
        VkSubpassDependency subpassDependency;

        void setRenderPassCreateInfo();
        void setAttachmentDescription();
        void setAttachmentReference();
        void setSubpassDescription();
        void setSubpassDependency();

    public:
        RenderPass(LogicalDevice* device, SwapChain* swapChain);
        ~RenderPass();
        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;

        inline VkRenderPass getHandle() { return renderPass; }
        inline const VkRenderPassCreateInfo& getRenderPassCreateInfo() const { return renderPassCreateInfo; }
        inline const VkAttachmentDescription& getAttachmentDescription() const { return attachmentDescription; }
        inline const VkAttachmentReference& getAttachmentReference() const { return attachmentReference; }
        inline const VkSubpassDescription& getSubpassDescription() const { return subpassDescription; }
        inline const VkSubpassDependency& getSubpassDependency() const { return subpassDependency; }
        inline LogicalDevice* getDevice() { return device; }
        inline SwapChain* getSwapChain() { return swapChain; }
};

#endif