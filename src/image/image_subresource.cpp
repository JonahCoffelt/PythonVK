#include <katra/image/image_subresource.h>

ImageSubresource::ImageSubresource(
    VkImageAspectFlags aspectMask, 
    uint32_t baseMipLevel, 
    uint32_t levelCount, 
    uint32_t baseArrayLayer, 
    uint32_t layerCount) : 
    aspectMask(aspectMask), baseMipLevel(baseMipLevel), levelCount(levelCount), baseArrayLayer(baseArrayLayer), layerCount(layerCount) 
{
    setSubresourceRange();
}

void ImageSubresource::setSubresourceRange() {
    subresourceRange.aspectMask = aspectMask;
    subresourceRange.baseMipLevel = baseMipLevel;
    subresourceRange.levelCount = levelCount;
    subresourceRange.baseArrayLayer = baseArrayLayer;
    subresourceRange.layerCount = layerCount;
}