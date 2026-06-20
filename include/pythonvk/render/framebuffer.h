#ifndef PYTHONVK_RENDER_FRAMEBUFFER_H
#define PYTHONVK_RENDER_FRAMEBUFFER_H

// Includes
#include <pythonvk/util/includes.h>
#include <pythonvk/render/render_pass.h>
#include <pythonvk/render/swap_chain.h>


// Class Declaration
class Framebuffer {
    private:
        LogicalDevice* device;
        RenderPass* renderPass;
        SwapChain* swapChain;

        VkFramebuffer framebuffer = VK_NULL_HANDLE;

        VkFramebufferCreateInfo framebufferInfo;
        std::vector<VkImageView> attachments;
        VkExtent2D extent;
        uint32_t layers;

        void setFramebufferInfo();

    public:
        Framebuffer(LogicalDevice* device, RenderPass* renderPass, std::vector<VkImageView> attachments, uint32_t layers = 1);
        ~Framebuffer();
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        inline VkFramebuffer getHandle() { return framebuffer; }
        inline RenderPass* getRenderPass() { return renderPass; }
        inline SwapChain* getSwapChain() { return swapChain; }
        inline const VkFramebufferCreateInfo& getFramebufferInfo() const { return framebufferInfo; }
        inline const std::vector<VkImageView>& getAttachments() const { return attachments; }
        inline const VkExtent2D& getExtent() const { return extent; }
        inline uint32_t getLayers() const { return layers; }
};

#endif