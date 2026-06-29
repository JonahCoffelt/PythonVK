#ifndef KATRA_RENDER_PASS_H
#define KATRA_RENDER_PASS_H

// Includes
#include <katra/util/includes.h>
#include <katra/device/logical_device.h>
#include <katra/render/swap_chain.h>
#include <katra/image/image_view.h>


// Class Declaration
class RenderPass {
    private:
        VkRenderPass renderPass = VK_NULL_HANDLE;
        LogicalDevice* device;
        SwapChain* swapChain;
        ImageView* depthImageView;

        VkRenderPassCreateInfo renderPassCreateInfo{};
        std::vector<VkAttachmentDescription> attachments;
        VkAttachmentDescription attachmentDescription{};
        VkAttachmentDescription depthAttachmentDescription{};
        VkAttachmentReference attachmentReference{};
        VkAttachmentReference depthAttachmentReference{};
        VkSubpassDescription subpassDescription{};
        VkSubpassDependency subpassDependency{};

        void setRenderPassCreateInfo();
        void setAttachmentDescription();
        void setAttachmentReference();
        void setDepthAttachmentDescription();
        void setDepthAttachmentReference();
        void setSubpassDescription();
        void setSubpassDependency();

    public:
        RenderPass(SwapChain* swapChain, ImageView* depthImageView = nullptr);
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
        inline ImageView* getDepthImageView() { return depthImageView; }

        inline bool hasDepth() { return depthImageView != nullptr; }
};

#endif