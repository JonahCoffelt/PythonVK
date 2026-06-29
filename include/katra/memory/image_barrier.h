#ifndef KATRA_MEMORY_IMAGE_BARRIER_H
#define KATRA_MEMORY_IMAGE_BARRIER_H

// Includes
#include <katra/util/includes.h>
#include <katra/image/image.h>


// Class Declaration
class ImageBarrier {
    private:
        LogicalDevice* device;
        Image* image;

        VkImageMemoryBarrier imageMemoryBarrier;
        VkAccessFlags srcAccessMask;
        VkAccessFlags dstAccessMask;
        VkFormat format;
        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;
        VkImageLayout oldLayout;
        VkImageLayout newLayout;
        uint32_t srcQueueFamilyIndex;
        uint32_t dstQueueFamilyIndex;
        VkImageAspectFlags aspectMask;
        uint32_t baseMipLevel;
        uint32_t levelCount;
        uint32_t baseArrayLayer;
        uint32_t layerCount;

        void setImageMemoryBarrier();

    public:
        ImageBarrier(
            LogicalDevice* device, 
            Image* image, 
            VkImageLayout oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            VkImageLayout newLayout = VK_IMAGE_LAYOUT_UNDEFINED, 
            VkFormat format = VK_FORMAT_D32_SFLOAT_S8_UINT,
            uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            uint32_t baseMipLevel = 0,
            uint32_t levelCount = 1,
            uint32_t baseArrayLayer = 0,
            uint32_t layerCount = 1
        );
        ~ImageBarrier();
        ImageBarrier(const ImageBarrier&) = delete;
        ImageBarrier& operator=(const ImageBarrier&) = delete;

        inline const VkImageMemoryBarrier& getHandle() const { return imageMemoryBarrier; }
        inline VkPipelineStageFlags getSourceStage() { return sourceStage; }
        inline VkPipelineStageFlags getDestinationStage() { return destinationStage; }
        inline VkAccessFlags getSrcAccessMask() { return srcAccessMask; }
        inline VkAccessFlags getDstAccessMask() { return dstAccessMask; }
        inline VkImageLayout getOldLayout() { return oldLayout; }
        inline VkImageLayout getNewLayout() { return newLayout; }
        inline uint32_t getSrcQueueFamilyIndex() { return srcQueueFamilyIndex; }
        inline uint32_t getDstQueueFamilyIndex() { return dstQueueFamilyIndex; }
        inline VkImageAspectFlags getAspectMask() { return aspectMask; }
        inline uint32_t getBaseMipLevel() { return baseMipLevel; }
        inline uint32_t getLevelCount() { return levelCount; }
};

#endif