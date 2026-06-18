#include <pythonvk/debug/debugger.h>
#include <pythonvk/render/instance.h>

/**
 * @brief Construct a new Debugger object
 * 
 * @param instance Instance object to use
 */
Debugger::Debugger(Instance* instance): instance(instance) {
    // Check for validation layer support
    if (!checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    // Set the info from helper
    VkDebugUtilsMessengerCreateInfoEXT createInfo = getDebugMessengerCreateInfo();

    // Create the messenger by reference
    VkResult result = createDebugUtilsMessengerEXT(&createInfo);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

/**
 * @brief Get generic debug messenger create info. Used for Debugger and Instance creation
 * 
 * @return VkDebugUtilsMessengerCreateInfoEXT 
 */
VkDebugUtilsMessengerCreateInfoEXT Debugger::getDebugMessengerCreateInfo() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;

    return createInfo;
}

/**
 * @brief Create the debug messenger on this debugger
 * 
 * @param createInfo 
 * @return VkResult 
 */
VkResult Debugger::createDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* createInfo) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance->getHandle(), "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance->getHandle(), createInfo, nullptr, &debugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

/**
 * @brief Destroy the debug messenger on this debugger
 * 
 */
void Debugger::destroyDebugUtilsMessengerEXT() {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance->getHandle(), "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance->getHandle(), debugMessenger, nullptr);
    }
}

/**
 * @brief Destroy the vkDebugger
 * 
 */
Debugger::~Debugger() {
    if (!debugMessenger) { return; }
        
    destroyDebugUtilsMessengerEXT();
}