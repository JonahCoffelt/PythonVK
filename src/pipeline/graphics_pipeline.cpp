#include <katra/pipeline/graphics_pipeline.h>

/**
 * @brief Construct a new Graphics Pipeline object
 * 
 */
GraphicsPipeline::GraphicsPipeline(
    RenderPass* renderPass, 
    std::string vertShaderPath, 
    std::string fragShaderPath, 
    VertexInput* vertexInput,
    std::vector<DescriptorLayout*> descriptorLayouts
): device(renderPass->getDevice()), renderPass(renderPass), vertexInput(vertexInput), descriptorLayouts(descriptorLayouts) {
    
    // Create shader modules
    vertShaderModule = new ShaderModule(device, vertShaderPath);
    fragShaderModule = new ShaderModule(device, fragShaderPath);
    
    // Set pipeline state
    setShaderStages();
    setInputAssembly();
    setDynamicState();
    setViewportState();
    setRasterizer();
    setMultisampling();
    setColorBlendAttachment();
    setColorBlendingState();

    // Create pipeline layout
    setPipelineLayoutInfo();
    VkResult result = vkCreatePipelineLayout(device->getHandle(), &pipelineLayoutInfo, nullptr, &pipelineLayout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    
    // Create pipeline
    setPipelineInfo();
    result = vkCreateGraphicsPipelines(device->getHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    // Clean up shader modules
    delete vertShaderModule;
    delete fragShaderModule;
    vertShaderModule = nullptr;
    fragShaderModule = nullptr;
}

/**
 * @brief Set the shader stages
 * 
 */
void GraphicsPipeline::setShaderStages() {
    // Create shader stage info for vertex shader
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule->getHandle();
    vertShaderStageInfo.pName = "main";

    // Create shader stage info for fragment shader
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule->getHandle();
    fragShaderStageInfo.pName = "main";

    // Add to shader stages
    shaderStages.clear();
    shaderStages.push_back(vertShaderStageInfo);
    shaderStages.push_back(fragShaderStageInfo);
}

/**
 * @brief Set the input assembly
 */
void GraphicsPipeline::setInputAssembly() {
    inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
}

/**
 * @brief Set the dynamic state
 */
void GraphicsPipeline::setDynamicState() {
    dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
}

/**
 * @brief Set the viewport state
 */
void GraphicsPipeline::setViewportState() {
    viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
}

/**
 * @brief Set the rasterizer
 */
void GraphicsPipeline::setRasterizer() {
    rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.lineWidth = 1.0f;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
}

/**
 * @brief Set the multisampling
 */
void GraphicsPipeline::setMultisampling() {
    multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional
}

/**
 * @brief Set the color blend attachment
 */
void GraphicsPipeline::setColorBlendAttachment() {
    colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
}

/**
 * @brief Set the color blending state
 */
void GraphicsPipeline::setColorBlendingState() {
    colorBlendingState = {};
    colorBlendingState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendingState.logicOpEnable = VK_FALSE;
    colorBlendingState.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlendingState.attachmentCount = 1;
    colorBlendingState.pAttachments = &colorBlendAttachment;
    colorBlendingState.blendConstants[0] = 0.0f; // Optional
    colorBlendingState.blendConstants[1] = 0.0f; // Optional
    colorBlendingState.blendConstants[2] = 0.0f; // Optional
    colorBlendingState.blendConstants[3] = 0.0f; // Optional
}

/**
 * @brief Set the pipeline layout info
 */
void GraphicsPipeline::setPipelineLayoutInfo() {
    descriptorLayoutHandles.clear();
    for (DescriptorLayout* descriptorLayout : descriptorLayouts) {
        descriptorLayoutHandles.push_back(descriptorLayout->getHandle());
    }

    pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorLayoutHandles.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
}

/**
 * @brief Set the pipeline info
 */
void GraphicsPipeline::setPipelineInfo() {
    pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInput->getCreateInfo();
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlendingState;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass->getHandle();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional
}

/**
 * @brief Destroy the Graphics Pipeline object
 * 
 */
 GraphicsPipeline::~GraphicsPipeline() {
    if (pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(device->getHandle(), pipeline, nullptr);
    }
    if (pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device->getHandle(), pipelineLayout, nullptr);
    }
}