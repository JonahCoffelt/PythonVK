#ifndef KATRA_COMMAND_BUFFER_H
#define KATRA_COMMAND_BUFFER_H

#include <katra/util/includes.h>
#include <katra/command/command_pool.h>
#include <katra/render/render_pass.h>
#include <katra/render/framebuffer.h>
#include <katra/pipeline/graphics_pipeline.h>
#include <katra/synchronization/semaphore.h>
#include <katra/synchronization/fence.h>
#include <katra/buffer/buffer.h>
#include <katra/descriptor/descriptor_set.h>
#include <katra/memory/image_barrier.h>


class CommandBuffer {
    private:
        CommandPool* pool;
        LogicalDevice* device;

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        VkCommandBufferAllocateInfo createInfo{};

        VkCommandBufferLevel level;

        void setCreateInfo();

    public:
        CommandBuffer(CommandPool* pool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        ~CommandBuffer();

        inline CommandPool* getPool() { return pool; }
        inline LogicalDevice* getLogicalDevice() { return device; }
        inline VkCommandBuffer& getHandle() { return commandBuffer; }
        inline const VkCommandBufferAllocateInfo& getCreateInfo() const { return createInfo; }
        inline const VkCommandBufferLevel getLevel() const { return level; }

        void begin();
        void end();
        void beginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer, std::vector<float> clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE);
        void endRenderPass();
        void reset();
        void bindPipeline(GraphicsPipeline* pipeline);
        void setViewport(float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);
        void setScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);
        void bindVertexBuffer(Buffer* buffer, uint32_t binding, uint32_t offset = 0);
        void bindIndexBuffer(Buffer* buffer, uint32_t offset = 0);
        void bindDescriptorSet(GraphicsPipeline* pipeline, DescriptorSet* descriptorSet, uint32_t firstSet = 0, VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);
        void draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);
        void drawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, uint32_t vertexOffset = 0, uint32_t firstInstance = 0);
        void copyBuffer(Buffer* src, Buffer* dst, uint32_t size, uint32_t srcOffset = 0, uint32_t dstOffset = 0);
        void copyBufferToImage(Buffer* src, Image* dst, uint32_t width, uint32_t height, uint32_t srcOffset = 0, uint32_t dstOffset = 0, uint32_t mipLLevel = 0, uint32_t baseArrayLayer = 0, uint32_t layerCount = 1);
        void pipelineBarrier(ImageBarrier* imageBarrier);

        void submit(VkQueue queue, std::vector<Semaphore*> waitSemaphores = {}, std::vector<Semaphore*> signalSemaphores = {}, Fence* fence = nullptr, VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
};

#endif