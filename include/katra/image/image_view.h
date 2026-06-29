#ifndef KATRA_IMAGE_VIEW_H
#define KATRA_IMAGE_VIEW_H

// Includes
#include <katra/util/includes.h>
#include <katra/device/logical_device.h>
#include <katra/image/image.h>


// Class Declaration
class ImageView {
    private:
        LogicalDevice* device;
        Image* image;

        VkImageView imageView = VK_NULL_HANDLE;

        VkImageViewCreateInfo createInfo{};
        VkImageViewType viewType;
        VkFormat format;
        VkImageAspectFlags aspectFlags;
        uint32_t mipLevels;
        uint32_t arrayLayers;

        void setCreateInfo();

    public:
        ImageView(
            Image* image, 
            VkImageViewType viewType, 
            VkFormat format, 
            VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
            uint32_t mipLevels = 1,
            uint32_t arrayLayers = 1
        );
        ~ImageView();
        ImageView(const ImageView&) = delete;
        ImageView& operator=(const ImageView&) = delete;

        inline const VkImageView& getHandle() const { return imageView; }
        inline const VkImageViewCreateInfo& getCreateInfo() const { return createInfo; }
        inline const VkImageViewType& getViewType() const { return viewType; }
        inline const VkFormat& getFormat() const { return format; }
        inline const VkImageAspectFlags& getAspectFlags() const { return aspectFlags; }
        inline const uint32_t& getMipLevels() const { return mipLevels; }
        inline const uint32_t& getArrayLayers() const { return arrayLayers; }
        inline LogicalDevice* getLogicalDevice() { return device; }
        inline Image* getImage() { return image; }
    };

#endif