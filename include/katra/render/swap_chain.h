#ifndef KATRA_SWAP_CHAIN_H
#define KATRA_SWAP_CHAIN_H

// Includes
#include <katra/util/includes.h>
#include <katra/render/window.h>
#include <katra/render/surface.h>
#include <katra/device/physical_device.h>
#include <katra/device/logical_device.h>
#include <katra/synchronization/semaphore.h>
#include <katra/image/image.h>
#include <katra/image/image_view.h>

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

        std::vector<Image*> images;
        std::vector<ImageView*> imageViews;
        uint32_t imageCount;

        void setCreateInfo();
        void setSwapSurfaceFormat(VkFormat preferredFormat, VkColorSpaceKHR preferredColorSpace);
        void setSwapPresentMode(VkPresentModeKHR preferredMode);
        void setSwapExtent();
        void setImageCount();
        void setImages();
        void setImageViews();

    public:
        SwapChain(
            LogicalDevice* logicalDevice, 
            Surface* surface, 
            VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR, 
            VkFormat format = VK_FORMAT_B8G8R8A8_UNORM, 
            VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        );
        ~SwapChain();
        SwapChain(const SwapChain&) = delete;
        SwapChain& operator=(const SwapChain&) = delete;

        inline VkSwapchainKHR getHandle() { return swapChain; }
        inline PhysicalDevice* getPhysicalDevice() { return physicalDevice; }
        inline LogicalDevice* getLogicalDevice() { return logicalDevice; }
        inline Surface* getSurface() { return surface; }
        inline Window* getWindow() { return window; }

        uint32_t acquireImage(Semaphore* semaphore = nullptr, uint64_t timeout = UINT64_MAX);

        inline const VkSurfaceFormatKHR& getSurfaceFormat() const { return surfaceFormat; }
        inline const VkPresentModeKHR& getPresentMode() const { return presentMode; }
        inline const VkExtent2D& getExtent() const { return extent; }
        inline const std::vector<uint32_t>& getQueueFamilyIndices() const { return queueFamilyIndices; }
        inline uint32_t getImageCount() const { return imageCount; }
        inline const std::vector<Image*>& getImages() const { return images; }
        inline const std::vector<ImageView*>& getImageViews() const { return imageViews; }
        inline int getWidth() const { return width; }
        inline int getHeight() const { return height; }
};

#endif