#ifndef PYVK_DEBUGGER_H
#define PYVK_DEBUGGER_H

// Includes
#include <pythonvk/util/includes.h>


// Constants
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};


// Forward Declarations
class Instance;


// Class Declaration
class Debugger {
    private:
        Instance* instance;
        VkDebugUtilsMessengerEXT debugMessenger;

    public:
        Debugger(Instance* isntance);
        ~Debugger();

        inline VkDebugUtilsMessengerEXT getHandle() { return debugMessenger; }
        static VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo();
        VkResult createDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* createInfo);
        void destroyDebugUtilsMessengerEXT();
};


// Helper Functions
inline bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

inline static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    // Only show warnings and errors
    if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        return VK_FALSE;
    }

    // Ignore GENERAL type (driver info spam)
    if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        return VK_FALSE;
    }

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

#endif