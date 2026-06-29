#include <katra/image/image_view.h>


ImageView::ImageView(
    Image* image, 
    VkImageViewType viewType, 
    VkImageAspectFlags aspectFlags, 
    uint32_t mipLevels, 
    uint32_t arrayLayers
): device(image->getLogicalDevice()), image(image), viewType(viewType), aspectFlags(aspectFlags), mipLevels(mipLevels), arrayLayers(arrayLayers) {
    // Get format from image
    format = image->getFormat();
    
    // Set the create info
    setCreateInfo();

    // Create the image view
    VkResult result = vkCreateImageView(device->getHandle(), &createInfo, nullptr, &imageView);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }
}

void ImageView::setCreateInfo() {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image->getHandle();
    createInfo.viewType = viewType;
    createInfo.format = format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = mipLevels;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = arrayLayers;
}

ImageView::~ImageView() {
    if (imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(device->getHandle(), imageView, nullptr);
        imageView = VK_NULL_HANDLE;
    }
}