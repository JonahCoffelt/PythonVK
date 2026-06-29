#include <katra/image/image.h>

Image::Image(
    LogicalDevice* device, 
    uint32_t width, 
    uint32_t height, 
    VkFormat format, 
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memoryType,
    VkSharingMode sharingMode,
    VkFlags flags
): device(device), width(width), height(height), format(format), tiling(tiling), usage(usage), memoryType(memoryType), sharingMode(sharingMode), flags(flags) {
    setImageCreateInfo();

    VkResult result = vkCreateImage(device->getHandle(), &createInfo, nullptr, &image);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device->getHandle(), image, &memoryRequirements);
    allocation = new Allocation(device, memoryRequirements, memoryType);
    
    vkBindImageMemory(device->getHandle(), image, allocation->getHandle(), 0);
}

Image::Image(
    LogicalDevice* device,
    VkImage existingImage,
    uint32_t width,
    uint32_t height,
    VkFormat format
): image(existingImage), device(device), allocation(nullptr), width(width), height(height), format(format), ownsImage(false) {}

void Image::setImageCreateInfo() {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent.width = width;
    createInfo.extent.height = height;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.format = format;
    createInfo.tiling = tiling;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = usage;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.sharingMode = sharingMode;
    createInfo.flags = flags;
}

Image::~Image() {
    if (ownsImage && image != VK_NULL_HANDLE) {
        vkDestroyImage(device->getHandle(), image, nullptr);
        image = VK_NULL_HANDLE;
    }
    if (allocation != nullptr) {
        delete allocation;
        allocation = nullptr;
    }
}