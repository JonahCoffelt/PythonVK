#ifndef KATRA_IMAGE_IMAGE_H
#define KATRA_IMAGE_IMAGE_H

// Includes
#include <katra/util/includes.h>
#include <katra/device/logical_device.h>
#include <katra/memory/allocation.h>


// Class Declaration
class Image {
    private:
        VkImage image = VK_NULL_HANDLE;
        LogicalDevice* device;
        Allocation* allocation;

        VkImageCreateInfo createInfo;
        uint32_t width;
        uint32_t height;
        VkFormat format;
        VkImageTiling tiling;
        VkImageUsageFlags usage;
        VkMemoryPropertyFlags memoryType;
        VkSharingMode sharingMode;
        VkFlags flags;
        bool ownsImage = true;

        void setImageCreateInfo();

    public:
        Image(
            LogicalDevice* device, 
            uint32_t width, 
            uint32_t height, 
            VkFormat format, 
            VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
            VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
            VkMemoryPropertyFlags memoryType = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            VkFlags flags = 0
        );
        Image(
            LogicalDevice* device,
            VkImage existingImage,
            uint32_t width,
            uint32_t height,
            VkFormat format
        );
        ~Image();
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        inline VkImage getHandle() { return image; }
        inline Allocation* getAllocation() { return allocation; }
        inline LogicalDevice* getLogicalDevice() { return device; }
        inline uint32_t getWidth() const { return width; }
        inline uint32_t getHeight() const { return height; }
        inline VkFormat getFormat() const { return format; }
};

#endif