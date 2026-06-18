#ifndef PYVK_SWAP_CHAIN_H
#define PYVK_SWAP_CHAIN_H

// Includes
#include <pythonvk/util/includes.h>
#include <pythonvk/render/window.h>
#include <pythonvk/render/surface.h>
#include <pythonvk/device/physical_device.h>
#include <pythonvk/device/logical_device.h>

// Class Declaration
class SwapChain {
    private:
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        PhysicalDevice* physicalDevice;
        LogicalDevice* logicalDevice;
        Surface* surface;
        Window* window;

        VkSwapchainCreateInfoKHR createInfo;
        VkSurfaceFormatKHR surfaceFormat;
        VkPresentModeKHR presentMode;
        
        VkExtent2D extent;
        int width;
        int height;

        std::vector<uint32_t> queueFamilyIndices;

        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
        uint32_t imageCount;

        void setCreateInfo();
        void setSwapSurfaceFormat(VkFormat preferredFormat, VkColorSpaceKHR preferredColorSpace);
        void setSwapPresentMode(VkPresentModeKHR preferredMode);
        void setSwapExtent();
        void setImageCount();
        void setImages();
        void setImageViews();

    public:
        SwapChain(PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice, Surface* surface);
        ~SwapChain();

        inline VkSwapchainKHR getHandle() { return swapChain; }
        inline PhysicalDevice* getPhysicalDevice() { return physicalDevice; }
        inline LogicalDevice* getLogicalDevice() { return logicalDevice; }
        inline Surface* getSurface() { return surface; }
        inline Window* getWindow() { return window; }

        inline VkSurfaceFormatKHR getSurfaceFormat() { return surfaceFormat; }
        inline VkPresentModeKHR getPresentMode() { return presentMode; }
        inline VkExtent2D getExtent() { return extent; }
        inline std::vector<uint32_t> getQueueFamilyIndices() { return queueFamilyIndices; }
        inline uint32_t getImageCount() { return imageCount; }
        inline std::vector<VkImage> getImages() { return images; }
        inline std::vector<VkImageView> getImageViews() { return imageViews; }
        inline int getWidth() { return width; }
        inline int getHeight() { return height; }
};

#endif