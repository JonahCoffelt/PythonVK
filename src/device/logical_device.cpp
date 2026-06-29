#include <katra/device/logical_device.h>
#include <katra/synchronization/semaphore.h>

/**
 * @brief Construct a new LogicalDevice object
 * 
 * @param physicalDevice 
 */
LogicalDevice::LogicalDevice(PhysicalDevice* physicalDevice, std::vector<const char*> requiredExtensions, std::vector<const char*> preferredExtensions): physicalDevice(physicalDevice) {
    setQueueFamilyIndices();
    setDeviceFeatures();
    setQueueCreateInfos();
    setEnabledExtensions(requiredExtensions, preferredExtensions);
    setDeviceCreateInfo();

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

void LogicalDevice::present(VkSwapchainKHR swapChain, uint32_t imageIndex, std::vector<Semaphore*> waitSemaphores) {
    std::vector<VkSemaphore> waitHandles;
    waitHandles.reserve(waitSemaphores.size());
    for (Semaphore* semaphore : waitSemaphores) {
        waitHandles.push_back(semaphore->getHandle());
    }

    VkSwapchainKHR swapChains[] = {swapChain};

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = static_cast<uint32_t>(waitHandles.size());
    presentInfo.pWaitSemaphores = waitHandles.empty() ? nullptr : waitHandles.data();
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    VkResult result = vkQueuePresentKHR(getPresentQueue(), &presentInfo);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to present swap chain image!");
    }
}

void LogicalDevice::setDeviceCreateInfo() {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();
    deviceFeatures.samplerAnisotropy = VK_TRUE;
}

void LogicalDevice::setDeviceFeatures() {
    deviceFeatures = {};
}

void LogicalDevice::setEnabledExtensions(std::vector<const char*> requiredExtensions, std::vector<const char*> preferredExtensions) {
    std::vector<const char*> requestedExtensions(requiredExtensions.begin(), requiredExtensions.end());
    requestedExtensions.insert(requestedExtensions.end(), preferredExtensions.begin(), preferredExtensions.end());
    enabledExtensions = physicalDevice->getEnabledExtensionsList(requestedExtensions);
}

void LogicalDevice::setQueueFamilyIndices() {
    queueFamilyIndices.clear();

    std::set<uint32_t> uniqueFamilies;
    if (physicalDevice->hasGraphicsFamily()) {
        uniqueFamilies.insert(physicalDevice->getGraphicsFamilyIndex());
    }
    if (physicalDevice->hasPresentFamily()) {
        uniqueFamilies.insert(physicalDevice->getPresentFamilyIndex());
    }
    if (physicalDevice->hasComputeFamily()) {
        uniqueFamilies.insert(physicalDevice->getComputeFamilyIndex());
    }
    if (physicalDevice->hasTransferFamily()) {
        uniqueFamilies.insert(physicalDevice->getTransferFamilyIndex());
    }
    if (physicalDevice->hasSparseBindingFamily()) {
        uniqueFamilies.insert(physicalDevice->getSparseBindingFamilyIndex());
    }

    queueFamilyIndices.assign(uniqueFamilies.begin(), uniqueFamilies.end());
}

void LogicalDevice::setQueueCreateInfos() {
    queueCreateInfos.clear();

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : queueFamilyIndices) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
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