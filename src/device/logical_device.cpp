#include <pythonvk/device/logical_device.h>

/**
 * @brief Construct a new LogicalDevice object
 * 
 * @param physicalDevice 
 */
LogicalDevice::LogicalDevice(PhysicalDevice* physicalDevice, std::vector<const char*> requiredExtensions, std::vector<const char*> preferredExtensions): physicalDevice(physicalDevice) {
    // Get queue create infos
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = getQueueCreateInfos();
    // Leave device features empty for now
    VkPhysicalDeviceFeatures deviceFeatures{};
    // Get enabled extensions
    std::vector<const char*> requestedExtensions(requiredExtensions.begin(), requiredExtensions.end());
    requestedExtensions.insert(requestedExtensions.end(), preferredExtensions.begin(), preferredExtensions.end());
    std::vector<const char*> enabledExtensions = physicalDevice->getEnabledExtensionsList(requestedExtensions);

    // Get device create info
    VkDeviceCreateInfo createInfo = getDeviceCreateInfo(queueCreateInfos, deviceFeatures, enabledExtensions);
    
    // Create the logical device
    VkResult result = vkCreateDevice(physicalDevice->getHandle(), &createInfo, nullptr, &device);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    retrieveQueues();
}

void LogicalDevice::waitIdle() {
    if (device == VK_NULL_HANDLE) { return; }
    vkDeviceWaitIdle(device);
}

VkDeviceCreateInfo LogicalDevice::getDeviceCreateInfo(
    const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
    const VkPhysicalDeviceFeatures& deviceFeatures,
    const std::vector<const char*>& enabledExtensions
) {
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    return createInfo;
}

std::set<uint32_t> LogicalDevice::getUniqueQueueFamilies() {
    std::set<uint32_t> uniqueQueueFamilies;

    // Add all queue families that are present
    if (physicalDevice->hasGraphicsFamily()) {
        uniqueQueueFamilies.insert(physicalDevice->getGraphicsFamilyIndex());
    }
    if (physicalDevice->hasPresentFamily()) {
        uniqueQueueFamilies.insert(physicalDevice->getPresentFamilyIndex());
    }
    if (physicalDevice->hasComputeFamily()) {
        uniqueQueueFamilies.insert(physicalDevice->getComputeFamilyIndex());
    }
    if (physicalDevice->hasTransferFamily()) {
        uniqueQueueFamilies.insert(physicalDevice->getTransferFamilyIndex());
    }
    if (physicalDevice->hasSparseBindingFamily()) {
        uniqueQueueFamilies.insert(physicalDevice->getSparseBindingFamilyIndex());
    }

    return uniqueQueueFamilies;
}

std::vector<VkDeviceQueueCreateInfo> LogicalDevice::getQueueCreateInfos() {
    // Get the unique queue families
    std::set<uint32_t> uniqueQueueFamilies = getUniqueQueueFamilies();
    // Priority for queue scheduling (required even for one queue)
    float queuePriority = 1.0f;
    
    // Get info for all queues
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    // Create info for each queue
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        // Create queue create info
        VkDeviceQueueCreateInfo queueCreateInfo{};
        // Fill info
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        // Add to list
        queueCreateInfos.push_back(queueCreateInfo);
    }

    return queueCreateInfos;
}

void LogicalDevice::retrieveQueues() {
    VkQueue queue;

    if (physicalDevice->hasGraphicsFamily()) {
        vkGetDeviceQueue(device, physicalDevice->getGraphicsFamilyIndex(), 0, &queue);
        graphicsQueue = queue;
    }
    if (physicalDevice->hasPresentFamily()) {
        vkGetDeviceQueue(device, physicalDevice->getPresentFamilyIndex(), 0, &queue);
        presentQueue = queue;
    }
    if (physicalDevice->hasComputeFamily()) {
        vkGetDeviceQueue(device, physicalDevice->getComputeFamilyIndex(), 0, &queue);
        computeQueue = queue;
    }
    if (physicalDevice->hasTransferFamily()) {
        vkGetDeviceQueue(device, physicalDevice->getTransferFamilyIndex(), 0, &queue);
        transferQueue = queue;
    }
    if (physicalDevice->hasSparseBindingFamily()) {
        vkGetDeviceQueue(device, physicalDevice->getSparseBindingFamilyIndex(), 0, &queue);
        sparseBindingQueue = queue;
    }
}

LogicalDevice::~LogicalDevice() {
    if (device == VK_NULL_HANDLE) { return; }
    vkDestroyDevice(device, nullptr);
}