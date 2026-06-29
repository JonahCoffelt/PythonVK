#ifndef KATRA_SAMPLER_H
#define KATRA_SAMPLER_H

// Includes
#include <katra/util/includes.h>
#include <katra/device/logical_device.h>

// Class Declaration
class Sampler {
    private:
        LogicalDevice* device;

        VkSampler sampler = VK_NULL_HANDLE;
        VkFilter filter;
        VkSamplerMipmapMode mipmapMode;
        VkSamplerAddressMode addressMode;
        float anisotropy;

        VkSamplerCreateInfo createInfo{};

        void setCreateInfo();

    public:
        Sampler(
            LogicalDevice* device,
            VkFilter filter = VK_FILTER_LINEAR,
            VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            float anisotropy = 1.0f
        );
        ~Sampler();
        Sampler(const Sampler&) = delete;
        Sampler& operator=(const Sampler&) = delete;

        inline const VkSampler& getHandle() const { return sampler; }
        inline const VkSamplerCreateInfo& getCreateInfo() const { return createInfo; }
        inline LogicalDevice* getLogicalDevice() { return device; }
};

#endif