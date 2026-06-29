#include <katra/memory/image_barrier.h>

ImageBarrier::ImageBarrier(
    LogicalDevice* device, 
    Image* image, 
    VkImageLayout oldLayout, 
    VkImageLayout newLayout, 
    VkFormat format,
    uint32_t srcQueueFamilyIndex, 
    uint32_t dstQueueFamilyIndex, 
    uint32_t baseMipLevel, 
    uint32_t levelCount, 
    uint32_t baseArrayLayer, 
    uint32_t layerCount): 
    device(device), 
    image(image), 
    oldLayout(oldLayout), 
    newLayout(newLayout), 
    format(format),
    srcQueueFamilyIndex(srcQueueFamilyIndex), 
    dstQueueFamilyIndex(dstQueueFamilyIndex), 
    baseMipLevel(baseMipLevel), 
    levelCount(levelCount), 
    baseArrayLayer(baseArrayLayer), 
    layerCount(layerCount) {

    setImageMemoryBarrier();
}

void ImageBarrier::setImageMemoryBarrier() {
    
    // Choose the aspect mask
    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        // These formats have stencil
        if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) {
            aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } 
    else {
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    // Choose the access
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        srcAccessMask = 0;
        dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } 
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } 
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        srcAccessMask = 0;
        dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }
    
    imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
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
}

ImageBarrier::~ImageBarrier() {}