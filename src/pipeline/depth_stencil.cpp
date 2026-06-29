#include <katra/pipeline/depth_stencil.h>


DepthStencil::DepthStencil(
    bool depthTest, bool depthWrite, VkCompareOp depthCompareOp, bool stencilTest
): depthTest(depthTest), depthWrite(depthWrite), depthCompareOp(depthCompareOp), stencilTest(stencilTest) {
    setDepthStencilCreateInfo();
}

void DepthStencil::setDepthStencilCreateInfo() {
    depthStencilCreateInfo = {};
    depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    // Depth test
    depthStencilCreateInfo.depthTestEnable = depthTest;
    depthStencilCreateInfo.depthWriteEnable = depthWrite;
    depthStencilCreateInfo.depthCompareOp = depthCompareOp;
    depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilCreateInfo.minDepthBounds = 0.0f;
    depthStencilCreateInfo.maxDepthBounds = 1.0f;
    // Stencil test
    depthStencilCreateInfo.stencilTestEnable = stencilTest;
    depthStencilCreateInfo.front = {}; // Optional
    depthStencilCreateInfo.back = {}; // Optional
}

DepthStencil::~DepthStencil() {}