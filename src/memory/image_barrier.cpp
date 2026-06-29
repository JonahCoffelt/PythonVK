#include <katra/memory/image_barrier.h>

ImageBarrier::ImageBarrier(
    LogicalDevice* device, 
    Image* image, 
    VkImageLayout oldLayout, 
    VkImageLayout newLayout, 
    uint32_t srcQueueFamilyIndex, 
    uint32_t dstQueueFamilyIndex, 
    VkImageAspectFlags aspectMask, 
    uint32_t baseMipLevel, 
    uint32_t levelCount, 
    uint32_t baseArrayLayer, 
    uint32_t layerCount): 
    device(device), 
    image(image), 
    oldLayout(oldLayout), 
    newLayout(newLayout), 
    srcQueueFamilyIndex(srcQueueFamilyIndex), 
    dstQueueFamilyIndex(dstQueueFamilyIndex), 
    aspectMask(aspectMask), 
    baseMipLevel(baseMipLevel), 
    levelCount(levelCount), 
    baseArrayLayer(baseArrayLayer), 
    layerCount(layerCount) {
    setImageMemoryBarrier();
}

void ImageBarrier::setImageMemoryBarrier() {
    imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcAccessMask = 0;
    imageMemoryBarrier.dstAccessMask = 0;
    imageMemoryBarrier.image = image->getHandle();
    imageMemoryBarrier.newLayout = newLayout;
    imageMemoryBarrier.oldLayout = oldLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
    imageMemoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
    imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
    imageMemoryBarrier.subresourceRange.baseMipLevel = baseMipLevel;
    imageMemoryBarrier.subresourceRange.levelCount = levelCount;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = baseArrayLayer;
    imageMemoryBarrier.subresourceRange.layerCount = layerCount;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        srcAccessMask = 0;
        dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.srcAccessMask = srcAccessMask;
        imageMemoryBarrier.dstAccessMask = dstAccessMask;
    
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        imageMemoryBarrier.srcAccessMask = srcAccessMask;
        imageMemoryBarrier.dstAccessMask = dstAccessMask;
    
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }
}

ImageBarrier::~ImageBarrier() {}