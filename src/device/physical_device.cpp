#include <katra/device/physical_device.h>

/**
 * @brief Construct a new Physical Device:: Physical Device object
 * 
 * @param device 
 * @param surface 
 */
PhysicalDevice::PhysicalDevice(VkPhysicalDevice device, Surface* surface): device(device), surface(surface) {
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);


    findExtensions();
    findQueueFamilyIndices();
    findSwapChainDetails();
    setScore();
}

/**
 * @brief Finds all extensions for the device and saves to extensions vector
 * 
 */
void PhysicalDevice::findExtensions() {
    // Get number of extensions
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    
    // Get extensions
    extensions.resize(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());
}

/**
 * @brief Check if the device supports all the required extensions
 * 
 * @param requiredExtensions 
 * @return bool 
 */
bool PhysicalDevice::checkExtensionSupport(std::vector<const char*> requiredExtensions) {
    // Save list of extensions not yet found
    std::set<std::string> remainingExtensions(requiredExtensions.begin(), requiredExtensions.end());
    
    // Loop through device extensions and tick off remainingExtensions
    for (const auto& extension : extensions) {
        remainingExtensions.erase(extension.extensionName);
    }

    return remainingExtensions.empty();
}

/**
 * @brief Get the enabled extensions list
 * 
 * @param requestedExtensionNames 
 * @return std::vector<const char*> 
 */
std::vector<const char*> PhysicalDevice::getEnabledExtensionsList(std::vector<const char*> requestedExtensionNames) {
    // Initalize empty
    std::vector<const char*> enabledExtensions;

    // Add extensions if they are found on device
    for (auto& extension : extensions) {
        for (auto& requestedExtensionName : requestedExtensionNames) {
            if (strcmp(extension.extensionName, requestedExtensionName) == 0) {
                enabledExtensions.push_back(requestedExtensionName);
                break;
            }
        }
    }

    return enabledExtensions;
}

/**
 * @brief Find the swap chain details for the device
 * 
 */
void PhysicalDevice::findSwapChainDetails() {    
    // Get the capabilities struct
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface->getHandle(), &swapChainDetails.capabilities);

    // Get number of formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface->getHandle(), &formatCount, nullptr);
    // Load the formats into swapChainDetails.formats
    if (formatCount != 0) {
        swapChainDetails.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface->getHandle(), &formatCount, swapChainDetails.formats.data());
    }

    // Get number of present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface->getHandle(), &presentModeCount, nullptr);
    // Load the present modes into swapChainDetails.presentModes
    if (presentModeCount != 0) {
        swapChainDetails.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface->getHandle(), &presentModeCount, swapChainDetails.presentModes.data());
    }
}

VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat PhysicalDevice::getDepthFormat() {
    return findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );

}

VkSampleCountFlagBits PhysicalDevice::getMaxUsableSampleCount() {
    VkSampleCountFlags counts = getLimits().framebufferColorSampleCounts & getLimits().framebufferDepthSampleCounts;

    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
    
    return VK_SAMPLE_COUNT_1_BIT;
}


/**
 * @brief Calculate the device score (kind of arbitrary, but servicable)
 * 
 */
void PhysicalDevice::setScore() {
    score = 0;
    
    // Add device type score
    switch (getDeviceType()) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            score += 1000000;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            score += 100000;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            score += 100;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            score += 10;
            break;
        default:
            score += 0;
            break;
    }

    // Add VRAM (in MB) to score
    VkMemoryHeap* heaps = getMemoryHeaps();
    for (uint32_t i = 0; i < getMemoryHeapCount(); i++) {
        VkMemoryHeap heap = heaps[i];
        if (heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            score += heap.size / (1024 * 1024);
        }
    }

    // Add max texture size
    score += getLimits().maxImageDimension2D;
}

/**
 * @brief Query info about the available types of memory
 * 
 * @param typeFilter 
 * @param properties 
 * @return uint32_t 
 */
uint32_t PhysicalDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

/**
 * @brief Final all the queue family indices on the device and save them to queueFamilyIndices
 * 
 */
void PhysicalDevice::findQueueFamilyIndices() {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        // Check graphics family
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueFamilyIndices.graphicsFamily = i;
        }

        // Check compute family
        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            queueFamilyIndices.computeFamily = i;
        }

        // Check transfer family
        if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT && !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            queueFamilyIndices.transferFamily = i;
        }

        // Check sparse binding family
        if (queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
            queueFamilyIndices.sparseBindingFamily = i;
        }

        // Check present family
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface->getHandle(), &presentSupport);
        if (presentSupport) {
            queueFamilyIndices.presentFamily = i;
        }

        // Increment index
        i++;
    }

}

PhysicalDevice::~PhysicalDevice() {

}