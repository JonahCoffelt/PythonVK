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

        VkFramebuffer framebuffer;

        VkFramebufferCreateInfo framebufferInfo;
        std::vector<VkImageView> attachments;
        VkExtent2D extent;
        uint32_t layers;

        void setFramebufferInfo();

    public:
        Framebuffer(LogicalDevice* device, RenderPass* renderPass, std::vector<VkImageView> attachments, uint32_t layers = 1);
        ~Framebuffer();

        inline VkFramebuffer getHandle() { return framebuffer; }
        inline RenderPass* getRenderPass() { return renderPass; }
        inline SwapChain* getSwapChain() { return swapChain; }
        inline VkFramebufferCreateInfo getFramebufferInfo() { return framebufferInfo; }
        inline std::vector<VkImageView> getAttachments() { return attachments; }
        inline VkExtent2D getExtent() { return extent; }
        inline uint32_t getLayers() { return layers; }
};

#endif