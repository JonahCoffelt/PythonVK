#ifndef KATRA_DEPTH_STENCIL_H
#define KATRA_DEPTH_STENCIL_H

// Includes
#include <katra/util/includes.h>
#include <katra/device/logical_device.h>

// Class Declaration
class DepthStencil {
    private:
        VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
        bool depthTest;
        bool depthWrite;
        VkCompareOp depthCompareOp;
        bool stencilTest;
        bool stencilWrite;
        VkCompareOp stencilCompareOp;

        void setDepthStencilCreateInfo();

    public:
        DepthStencil(
            bool depthTest = false, 
            bool depthWrite = false, 
            VkCompareOp depthCompareOp = VK_COMPARE_OP_NEVER,
            bool stencilTest = false
        );
        ~DepthStencil();
        DepthStencil(const DepthStencil&) = delete;
        DepthStencil& operator=(const DepthStencil&) = delete;

        inline const VkPipelineDepthStencilStateCreateInfo& getCreateInfo() const { return depthStencilCreateInfo; }
};

#endif