#ifndef PYVK_INSTANCE_H
#define PYVK_INSTANCE_H

// Includes
#include <pythonvk/util/includes.h>
#include <pythonvk/debug/debugger.h>
#include <pythonvk/util/constants.h>


// Class Declaration
class Instance {
    private:
        VkInstance instance = VK_NULL_HANDLE;

        bool useValidation;
        std::string applicationName;
        VkApplicationInfo appInfo{};
        std::vector<const char*> extensions;
        VkInstanceCreateInfo createInfo{};
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};

        void setAppInfo(std::string name = "PythonVK", unsigned int variant = 0, unsigned int major_version = 1, unsigned int minor_version = 0, unsigned int patch = 0);
        void setRequiredExtensions();
        void setInstanceCreateInfo();
        
    public:
        Instance(std::string name = "PythonVK", bool useValidation = true, unsigned int variant = 0, unsigned int major_version = 1, unsigned int minor_version = 0, unsigned int patch = 0);
        ~Instance();
        Instance(const Instance&) = delete;
        Instance& operator=(const Instance&) = delete;

        inline VkInstance getHandle() { return instance; }

};

#endif