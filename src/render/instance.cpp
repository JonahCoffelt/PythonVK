#include <pythonvk/render/instance.h>

/**
 * @brief Construct a new Instance object
 * 
 * @param useValidation Enable validation layers
 */
Instance::Instance(
    std::string name, 
    bool useValidation, 
    uint32_t apiVersion): 
        useValidation(useValidation),
        apiVersion(apiVersion)
    {

    // Set app info and extensions
    setAppInfo(name, apiVersion);
    setRequiredExtensions();
    setInstanceCreateInfo();

    // Create the Vulkan instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

/**
 * @brief Set the instance creation info from app info and extensions
 * 
 */
void Instance::setInstanceCreateInfo() {
    createInfo = {};
    
    // General info
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;

    // Use validation layers if enabled
    if (useValidation) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

        debugMessengerCreateInfo = Debugger::getDebugMessengerCreateInfo();
        createInfo.pNext = &debugMessengerCreateInfo;
    }

    // Mac Compatibility
    #ifdef __APPLE__
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #endif
}

/**
 * @brief Set the app info, containing name and version
 * 
 * @param name 
 * @param variant 
 * @param major_version 
 * @param minor_version 
 * @param patch 
 */
void Instance::setAppInfo(std::string name, uint32_t apiVersion) {
    applicationName = std::move(name);
    appInfo = {};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = applicationName.c_str();
    appInfo.pEngineName = "PythonVK";
    appInfo.applicationVersion = 0;
    appInfo.engineVersion = 0;
    appInfo.apiVersion = apiVersion;
}

/**
 * @brief Set the required extensions for glfw (must use glfw)
 * 
 */
void Instance::setRequiredExtensions() {
    extensions.clear();

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    extensions.assign(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (useValidation) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    #ifdef __APPLE__
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    #endif
} 

/**
 * @brief Destroy the vkInstance
 * 
 */
Instance::~Instance() {
    if (!instance) { return; }
    vkDestroyInstance(instance, nullptr);
}