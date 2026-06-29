#include <katra/image/sampler.h>

Sampler::Sampler(
    LogicalDevice* device, 
    VkFilter filter, 
    VkSamplerMipmapMode mipmapMode, 
    VkSamplerAddressMode addressMode, 
    float anisotropy
): device(device), filter(filter), mipmapMode(mipmapMode), addressMode(addressMode), anisotropy(anisotropy) {

    // Get device anisotropy limit
    float anisotropyLimit = device->getPhysicalDevice()->getLimits().maxSamplerAnisotropy;
    anisotropy = std::min(anisotropy, anisotropyLimit);

    // Set the create info
    setCreateInfo();

    // Create the sampler
    VkResult result = vkCreateSampler(device->getHandle(), &createInfo, nullptr, &sampler);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create sampler!");
    }
}

void Sampler::setCreateInfo() {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.magFilter = filter;
    createInfo.minFilter = filter;
    createInfo.addressModeU = addressMode;
    createInfo.addressModeV = addressMode;
    createInfo.addressModeW = addressMode;
    createInfo.mipmapMode = mipmapMode;
    createInfo.minLod = 0.0f;
    createInfo.maxLod = 0.0f;
    createInfo.anisotropyEnable = anisotropy > 1.0f;
    createInfo.maxAnisotropy = anisotropy;
    createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    createInfo.unnormalizedCoordinates = VK_FALSE;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
}

Sampler::~Sampler() {
    if (sampler != VK_NULL_HANDLE) {
        vkDestroySampler(device->getHandle(), sampler, nullptr);
        sampler = VK_NULL_HANDLE;
    }
}