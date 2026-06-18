#ifndef PYVK_INSTANCE_H
#define PYVK_INSTANCE_H

// Includes
#include <pythonvk/util/includes.h>
#include <pythonvk/debug/debugger.h>
#include <pythonvk/util/constants.h>


// Class Declaration
class Instance {
    private:
        VkInstance instance;
        
    public:
        Instance();
        ~Instance();

        inline VkInstance getHandle() { return instance; }
};


// Helper Functions

inline std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
} 

#endif