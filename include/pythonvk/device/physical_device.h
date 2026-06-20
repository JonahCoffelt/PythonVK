#ifndef PYVK_PHYSICAL_DEVICE_H
#define PYVK_PHYSICAL_DEVICE_H

// Includes
#include <pythonvk/util/includes.h>
#include <pythonvk/render/instance.h>
#include <pythonvk/render/surface.h>


// Helper Structs
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> computeFamily;
    std::optional<uint32_t> transferFamily;
    std::optional<uint32_t> sparseBindingFamily;
    std::optional<uint32_t> presentFamily;
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


// Class Declaration
class PhysicalDevice {
    private:
        VkPhysicalDevice device;
        Surface* surface;
        
        // Properties
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        VkPhysicalDeviceMemoryProperties memoryProperties;
        
        // Extensions
        std::vector<VkExtensionProperties> extensions;
        void findExtensions();

        // Queue Families
        QueueFamilyIndices queueFamilyIndices;
        void findQueueFamilyIndices();

        // Swap Chain Support
        SwapChainSupportDetails swapChainDetails;
        void findSwapChainDetails();
        
        // Score
        unsigned int score;
        void setScore();

    public:
        PhysicalDevice(VkPhysicalDevice device, Surface* surface);
        ~PhysicalDevice();
        PhysicalDevice(const PhysicalDevice&) = delete;
        PhysicalDevice& operator=(const PhysicalDevice&) = delete;
        PhysicalDevice(PhysicalDevice&&) = default;
        PhysicalDevice& operator=(PhysicalDevice&&) = default;
        inline VkPhysicalDevice getHandle() { return device; }

        // Device Suitability
        inline unsigned int getScore() { return score; }

        // Extensions
        bool checkExtensionSupport(std::vector<const char*> requiredExtensions);
        std::vector<const char*> getEnabledExtensionsList(std::vector<const char*> requestedExtensions);

        // Queue Families
        inline const QueueFamilyIndices& getQueueFamilyIndices() const { return queueFamilyIndices; }
        inline bool hasGraphicsFamily() { return queueFamilyIndices.graphicsFamily.has_value(); }
        inline bool hasComputeFamily() { return queueFamilyIndices.computeFamily.has_value(); }
        inline bool hasTransferFamily() { return queueFamilyIndices.transferFamily.has_value(); }
        inline bool hasSparseBindingFamily() { return queueFamilyIndices.sparseBindingFamily.has_value(); }
        inline bool hasPresentFamily() { return queueFamilyIndices.presentFamily.has_value(); }
        inline uint32_t getGraphicsFamilyIndex() { return queueFamilyIndices.graphicsFamily.value(); }
        inline uint32_t getComputeFamilyIndex() { return queueFamilyIndices.computeFamily.value(); }
        inline uint32_t getTransferFamilyIndex() { return queueFamilyIndices.transferFamily.value(); }
        inline uint32_t getSparseBindingFamilyIndex() { return queueFamilyIndices.sparseBindingFamily.value(); }
        inline uint32_t getPresentFamilyIndex() { return queueFamilyIndices.presentFamily.value(); }

        // Swap Chain
        inline const SwapChainSupportDetails& getSwapChainDetails() const { return swapChainDetails; }
        inline const VkSurfaceCapabilitiesKHR& getSurfaceCapabilities() const { return swapChainDetails.capabilities; }
        inline const std::vector<VkSurfaceFormatKHR>& getSurfaceFormats() const { return swapChainDetails.formats; }
        inline const std::vector<VkPresentModeKHR>& getPresentModes() const { return swapChainDetails.presentModes; }
        inline bool supportsSwapChain() { return !swapChainDetails.formats.empty() && !swapChainDetails.presentModes.empty(); }

        // Device properties
        inline uint32_t getAPIVersion() { return deviceProperties.apiVersion; }
        inline uint32_t getDriverVersion() { return deviceProperties.driverVersion; }
        inline uint32_t getVendorID() { return deviceProperties.vendorID; }
        inline uint32_t getDeviceID() { return deviceProperties.deviceID; }
        inline VkPhysicalDeviceType getDeviceType() { return deviceProperties.deviceType; }
        inline const char* getName() const { return deviceProperties.deviceName; }
        inline const uint8_t* getPipelineCacheUUID() const { return deviceProperties.pipelineCacheUUID; }
        inline const VkPhysicalDeviceLimits& getLimits() const { return deviceProperties.limits; }
        inline const VkPhysicalDeviceSparseProperties& getSparseProperties() const { return deviceProperties.sparseProperties; }

        // Device Features
        // General
        inline VkBool32 getRobustBufferAccess() { return deviceFeatures.robustBufferAccess; } 
        inline VkBool32 getSamplerAnisotropy() { return deviceFeatures.samplerAnisotropy; }
        inline VkBool32 getDepthClamp() { return deviceFeatures.depthClamp; }
        inline VkBool32 getFillModeNonSolid() { return deviceFeatures.fillModeNonSolid; }
        inline VkBool32 getIndependentBlend() { return deviceFeatures.independentBlend; }
        // 3D Rendering
        inline VkBool32 getMultiDrawIndirect() { return deviceFeatures.multiDrawIndirect; }
        inline VkBool32 getDrawIndirectFirstInstance() { return deviceFeatures.drawIndirectFirstInstance; }
        inline VkBool32 getGeometryShader() { return deviceFeatures.geometryShader; }
        inline VkBool32 getTessellationShader() { return deviceFeatures.tessellationShader; }
        inline VkBool32 getSampleRateShading() { return deviceFeatures.sampleRateShading; }
        inline VkBool32 getMultiViewport() { return deviceFeatures.multiViewport; }
        inline VkBool32 getDepthBounds() { return deviceFeatures.depthBounds; }
        inline VkBool32 getWideLines() { return deviceFeatures.wideLines; }
        // Compute Shaders
        inline VkBool32 getShaderFloat64() { return deviceFeatures.shaderFloat64; }
        inline VkBool32 getShaderInt64() { return deviceFeatures.shaderInt64; }
        inline VkBool32 getShaderInt16() { return deviceFeatures.shaderInt16; }
        inline VkBool32 getVertexPipelineStoresAndAtomics() { return deviceFeatures.vertexPipelineStoresAndAtomics; }
        inline VkBool32 getFragmentStoresAndAtomics() { return deviceFeatures.fragmentStoresAndAtomics; }
        inline VkBool32 getShaderStorageImageExtendedFormats() { return deviceFeatures.shaderStorageImageExtendedFormats; }
        inline VkBool32 getShaderUniformBufferArrayDynamicIndexing() { return deviceFeatures.shaderUniformBufferArrayDynamicIndexing; }
        inline VkBool32 getShaderSampledImageArrayDynamicIndexing() { return deviceFeatures.shaderSampledImageArrayDynamicIndexing; }
        // Texture Compression
        inline VkBool32 getTextureCompressionBC() { return deviceFeatures.textureCompressionBC; }
        inline VkBool32 getTextureCompressionASTC_LDR() { return deviceFeatures.textureCompressionASTC_LDR; }
        // Sparse Resources
        inline VkBool32 getSparseBinding() { return deviceFeatures.sparseBinding; }
        inline VkBool32 getSparseResidencyImage2D() { return deviceFeatures.sparseResidencyImage2D; }

        // Memory properties
        inline uint32_t getMemoryTypeCount() { return memoryProperties.memoryTypeCount; }
        inline VkMemoryType* getMemoryTypes() { return memoryProperties.memoryTypes; }
        inline uint32_t getMemoryHeapCount() { return memoryProperties.memoryHeapCount; }
        inline VkMemoryHeap* getMemoryHeaps() { return memoryProperties.memoryHeaps; }
};


// Helper functions

static inline std::vector<PhysicalDevice> getPhysicalDevices(Instance* instance, Surface* surface) {
    // Get number of devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance->getHandle(), &deviceCount, nullptr);

    // Get the list of devices
    std::vector<VkPhysicalDevice> vkDevices(deviceCount);
    vkEnumeratePhysicalDevices(instance->getHandle(), &deviceCount, vkDevices.data());

    // Loop through to create PhysicalDevice objects
    std::vector<PhysicalDevice> devices;
    devices.reserve(vkDevices.size());
    for (const auto& vkDevice : vkDevices) {
        devices.emplace_back(vkDevice, surface);
    }

    return devices;
}

#endif