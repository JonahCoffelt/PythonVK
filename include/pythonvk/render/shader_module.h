#ifndef PYVK_SHADER_MODULE_H
#define PYVK_SHADER_MODULE_H

// Includes
#include <pythonvk/util/includes.h>
#include <pythonvk/device/logical_device.h>


// Class Declaration
class ShaderModule {
    private:
        VkShaderModule shaderModule = VK_NULL_HANDLE;
        LogicalDevice* device;

        VkShaderModuleCreateInfo createInfo{};
        std::vector<char> code;

        void setCreateInfo();
        void readFile(const std::string& filename);
    
    public:
        ShaderModule(LogicalDevice* device, const std::string& filename);
        ~ShaderModule();
        ShaderModule(const ShaderModule&) = delete;
        ShaderModule& operator=(const ShaderModule&) = delete;

        inline VkShaderModule getHandle() { return shaderModule; }
        inline LogicalDevice* getDevice() { return device; }
        inline const VkShaderModuleCreateInfo& getCreateInfo() const { return createInfo; }

};

#endif