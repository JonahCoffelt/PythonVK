#include <katra/command/command_buffer.h>

CommandBuffer::CommandBuffer(CommandPool* pool, VkCommandBufferLevel level): pool(pool), level(level) {
    device = pool->getDevice();
    setCreateInfo();

    VkResult result = vkAllocateCommandBuffers(device->getHandle(), &createInfo, &commandBuffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffer!");
    }
}

void CommandBuffer::setCreateInfo() {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    createInfo.commandPool = pool->getHandle();
    createInfo.level = level;
    createInfo.commandBufferCount = 1;
}

void CommandBuffer::begin() {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}

void CommandBuffer::end() {
    VkResult result = vkEndCommandBuffer(commandBuffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to end recording command buffer!");
    }
}

void CommandBuffer::beginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer, std::vector<float> clearColor, VkSubpassContents subpassContents) {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->getHandle();
    renderPassInfo.framebuffer = framebuffer->getHandle();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = framebuffer->getExtent();
    
    VkClearValue clearValue = {{{clearColor[0], clearColor[1], clearColor[2], clearColor[3]}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, subpassContents);
}

void CommandBuffer::endRenderPass() {
    vkCmdEndRenderPass(commandBuffer);
}

void CommandBuffer::bindPipeline(GraphicsPipeline* pipeline) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getHandle());
}

void CommandBuffer::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) {
    VkViewport viewport{};
    viewport.x = x;
    viewport.y = y;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = minDepth;
    viewport.maxDepth = maxDepth;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}

void CommandBuffer::setScissor(int32_t x, int32_t y, uint32_t width, uint32_t height) {
    VkRect2D scissor{};
    scissor.offset = {x, y};
    scissor.extent = {width, height};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void CommandBuffer::bindVertexBuffer(Buffer* buffer, uint32_t binding, uint32_t offset) {
    VkBuffer buffers[] = {buffer->getHandle()};
    VkDeviceSize offsets[] = {offset};
    vkCmdBindVertexBuffers(commandBuffer, binding, 1, buffers, offsets);
}

void CommandBuffer::bindIndexBuffer(Buffer* buffer, uint32_t offset) {
    vkCmdBindIndexBuffer(commandBuffer, buffer->getHandle(), offset, VK_INDEX_TYPE_UINT16);
}

void CommandBuffer::bindDescriptorSet(GraphicsPipeline* pipeline, DescriptorSet* descriptorSet, uint32_t firstSet, VkPipelineBindPoint bindPoint) {
    VkDescriptorSet handle = descriptorSet->getHandle();
    vkCmdBindDescriptorSets(commandBuffer, bindPoint, pipeline->getLayout(), firstSet, 1, &handle, 0, nullptr);
}

void CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) {
    vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::copyBuffer(Buffer* src, Buffer* dst, uint32_t size, uint32_t srcOffset, uint32_t dstOffset) {
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = srcOffset;
    copyRegion.dstOffset = dstOffset;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, src->getHandle(), dst->getHandle(), 1, &copyRegion);
}

void CommandBuffer::reset() {
    VkResult result = vkResetCommandBuffer(commandBuffer, 0);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to reset command buffer!");
    }
}

void CommandBuffer::submit(VkQueue queue, std::vector<Semaphore*> waitSemaphores, std::vector<Semaphore*> signalSemaphores, Fence* fence, VkPipelineStageFlags waitStage) {
    std::vector<VkSemaphore> waitHandles;
    waitHandles.reserve(waitSemaphores.size());
    for (Semaphore* semaphore : waitSemaphores) {
        waitHandles.push_back(semaphore->getHandle());
    }

    std::vector<VkSemaphore> signalHandles;
    signalHandles.reserve(signalSemaphores.size());
    for (Semaphore* semaphore : signalSemaphores) {
        signalHandles.push_back(semaphore->getHandle());
    }

    std::vector<VkPipelineStageFlags> waitStages(waitHandles.size(), waitStage);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitHandles.size());
    submitInfo.pWaitSemaphores = waitHandles.empty() ? nullptr : waitHandles.data();
    submitInfo.pWaitDstStageMask = waitStages.empty() ? nullptr : waitStages.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalHandles.size());
    submitInfo.pSignalSemaphores = signalHandles.empty() ? nullptr : signalHandles.data();

    VkFence fenceHandle = fence ? fence->getHandle() : VK_NULL_HANDLE;
    VkResult result = vkQueueSubmit(queue, 1, &submitInfo, fenceHandle);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to submit command buffer!");
    }
}

CommandBuffer::~CommandBuffer() {
    vkFreeCommandBuffers(device->getHandle(), pool->getHandle(), 1, &commandBuffer);
}