#ifndef KATRA_IMAGE_SUBRESOURCE_H
#define KATRA_IMAGE_SUBRESOURCE_H

// Includes
#include <katra/util/includes.h>


// Class Declaration
class ImageSubresource {
    private:
        VkImageSubresourceRange subresourceRange;

        VkImageAspectFlags aspectMask;
        uint32_t baseMipLevel;
        uint32_t levelCount;
        uint32_t baseArrayLayer;
        uint32_t layerCount;

        void setSubresourceRange();

    public:
        ImageSubresource(
            VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, 
            uint32_t baseMipLevel = 0, 
            uint32_t levelCount = 1, 
            uint32_t baseArrayLayer = 0, 
            uint32_t layerCount = 1
        );
        ~ImageSubresource() = default;
        ImageSubresource(const ImageSubresource&) = delete;
        ImageSubresource& operator=(const ImageSubresource&) = delete;

        inline const VkImageSubresourceRange& getSubresourceRange() const { return subresourceRange; }
        inline VkImageSubresourceLayers getSubresourceLayers() const {
            VkImageSubresourceLayers layers{};
            layers.aspectMask = aspectMask;
            layers.mipLevel = baseMipLevel;
            layers.baseArrayLayer = baseArrayLayer;
            layers.layerCount = layerCount;
            return layers;
        }
        inline VkImageAspectFlags getAspectMask() { return aspectMask; }
        inline uint32_t getBaseMipLevel() { return baseMipLevel; }
        inline uint32_t getLevelCount() { return levelCount; }
        inline uint32_t getBaseArrayLayer() { return baseArrayLayer; }
        inline uint32_t getLayerCount() { return layerCount; }

        void setAspectMask(VkImageAspectFlags aspectMask) { this->aspectMask = aspectMask; subresourceRange.aspectMask = aspectMask; }
        void setBaseMipLevel(uint32_t baseMipLevel) { this->baseMipLevel = baseMipLevel; subresourceRange.baseMipLevel = baseMipLevel; }
        void setLevelCount(uint32_t levelCount) { this->levelCount = levelCount; subresourceRange.levelCount = levelCount; }
        void setBaseArrayLayer(uint32_t baseArrayLayer) { this->baseArrayLayer = baseArrayLayer; subresourceRange.baseArrayLayer = baseArrayLayer; }
        void setLayerCount(uint32_t layerCount) { this->layerCount = layerCount; subresourceRange.layerCount = layerCount; }
};

#endif