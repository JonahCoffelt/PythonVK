#include <katra/render/swap_chain.h>

SwapChain::SwapChain(
    PhysicalDevice* physicalDevice, 
    LogicalDevice* logicalDevice, 
    Surface* surface, 
    VkPresentModeKHR presentMode, 
    VkFormat format, 
    VkColorSpaceKHR colorSpace
): physicalDevice(physicalDevice), logicalDevice(logicalDevice), surface(surface) {
    // Use the window from the surface
    window = surface->getWindow();

    // Set swap chain properties
    setSwapSurfaceFormat(format, colorSpace);
    setSwapPresentMode(presentMode);
    setSwapExtent();
    setImageCount();
    setCreateInfo();

    // Create the swap chain
    VkResult result = vkCreateSwapchainKHR(logicalDevice->getHandle(), &createInfo, nullptr, &swapChain);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!!!");
    }

    setImages();
    setImageViews();
}

void SwapChain::setCreateInfo() {
    VkSurfaceCapabilitiesKHR capabilities = physicalDevice->getSurfaceCapabilities();

    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface->getHandle();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Swap chain sharing only needs the families that access its images: graphics + present.
    if (logicalDevice->getGraphicsFamilyIndex() != logicalDevice->getPresentFamilyIndex()) {
        queueFamilyIndices = {
            logicalDevice->getGraphicsFamilyIndex(),
            logicalDevice->getPresentFamilyIndex()
        };
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else {
        queueFamilyIndices.clear();
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    // Additional swap chain params
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;  // For when swap chain becomes invalid (eg window resize)
}

void SwapChain::setSwapSurfaceFormat(VkFormat preferredFormat, VkColorSpaceKHR preferredColorSpace) {
    // Get available surface formats on the physical device
    const std::vector<VkSurfaceFormatKHR>& availableFormats = physicalDevice->getSurfaceFormats();
    
    // Loop through available formats and find the preferred format
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == preferredFormat && availableFormat.colorSpace == preferredColorSpace) {
            surfaceFormat = availableFormat;
            return;
        }
    }

    // If preferred format is not found, use the first available format
    surfaceFormat = availableFormats[0];
}

void SwapChain::setSwapPresentMode(VkPresentModeKHR preferredMode) {
    // Get available present modes on the physical device
    const std::vector<VkPresentModeKHR>& availablePresentModes = physicalDevice->getPresentModes();

    // Loop through available present modes and find the preferred present mode
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == preferredMode) {
            presentMode = availablePresentMode;
            return;
        }
    }
    // If preferred present mode is not found, use the first available present mode
    presentMode = availablePresentModes[0];
}

void SwapChain::setSwapExtent() {
    VkSurfaceCapabilitiesKHR capabilities = physicalDevice->getSurfaceCapabilities();

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        extent = capabilities.currentExtent;
    } else {
        glfwGetFramebufferSize(window->getHandle(), &width, &height);

        extent.width = std::clamp(
            static_cast<uint32_t>(width),
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width
        );
        extent.height = std::clamp(
            static_cast<uint32_t>(height),
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height
        );
    }

    width = static_cast<int>(extent.width);
    height = static_cast<int>(extent.height);
}

void SwapChain::setImageCount() {
    // Get capabilities from the physical device
    VkSurfaceCapabilitiesKHR capabilities = physicalDevice->getSurfaceCapabilities();
    
    // We would like 1 more image than min to avoid waiting on driver operations
    imageCount = capabilities.minImageCount + 1;
    // Of course, do not exceed max
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }
}

void SwapChain::setImages() {
    // Get the images from the swap chain
    vkGetSwapchainImagesKHR(logicalDevice->getHandle(), swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(logicalDevice->getHandle(), swapChain, &imageCount, images.data());
}

void SwapChain::setImageViews() {
    // Create image views for each image
    imageViews.resize(imageCount);

    // Loop over all images
    for (size_t i = 0; i < imageCount; i++) {
        // Create image view from the swap chain image at i
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = surfaceFormat.format;
        
        // Set swizzle
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // Specify image purpose and access
        // Here, images are just simple color targets
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        // Create the image view
        VkResult result = vkCreateImageView(logicalDevice->getHandle(), &createInfo, nullptr, &imageViews[i]);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

uint32_t SwapChain::acquireImage(Semaphore* semaphore, uint64_t timeout) {
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(logicalDevice->getHandle(), swapChain, timeout, semaphore ? semaphore->getHandle() : VK_NULL_HANDLE, VK_NULL_HANDLE, &imageIndex);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to acquire image!");
    }
    return imageIndex;
}

SwapChain::~SwapChain() {
    if (logicalDevice == nullptr || logicalDevice->getHandle() == VK_NULL_HANDLE) {
        return;
    }

    for (auto imageView : imageViews) {
        vkDestroyImageView(logicalDevice->getHandle(), imageView, nullptr);
    }
    imageViews.clear();

    if (swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(logicalDevice->getHandle(), swapChain, nullptr);
        swapChain = VK_NULL_HANDLE;
    }
}
