#ifndef PYVK_PIPELINE_H
#define PYVK_PIPELINE_H

// Includes
#include <pythonvk/util/includes.h>
#include <pythonvk/device/logical_device.h>
#include <pythonvk/render/render_pass.h>
#include <pythonvk/render/shader_module.h>


// Class Declaration
class GraphicsPipeline {
    private:
        LogicalDevice* device;
        RenderPass* renderPass;
        
        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

        ShaderModule* vertShaderModule;
        ShaderModule* fragShaderModule;

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        VkPipelineVertexInputStateCreateInfo vertexInput;
        VkPipelineInputAssemblyStateCreateInfo inputAssembly;
        std::vector<VkDynamicState> dynamicStates;
        VkPipelineDynamicStateCreateInfo dynamicState;
        VkPipelineViewportStateCreateInfo viewportState;
        VkPipelineRasterizationStateCreateInfo rasterizer;
        VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendingState;
        VkPipelineLayoutCreateInfo pipelineLayoutInfo;
        VkGraphicsPipelineCreateInfo pipelineInfo;

        void setShaderStages();
        void setVertexInput();
        void setInputAssembly();
        void setDynamicState();
        void setViewportState();
        void setRasterizer();
        void setMultisampling();
        void setColorBlendAttachment();
        void setColorBlendingState();
        void setPipelineLayoutInfo();
        void setPipelineInfo();

    public:
        GraphicsPipeline(LogicalDevice* device, RenderPass* renderPass, std::string vertShaderPath, std::string fragShaderPath);
        ~GraphicsPipeline();
        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

        inline VkPipeline getHandle() { return pipeline; }
        inline VkPipelineLayout getLayout() { return pipelineLayout; }
        inline const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() const { return shaderStages; }
        inline const std::vector<VkDynamicState>& getDynamicStates() const { return dynamicStates; }
        inline const VkPipelineVertexInputStateCreateInfo& getVertexInput() const { return vertexInput; }
        inline const VkPipelineInputAssemblyStateCreateInfo& getInputAssembly() const { return inputAssembly; }
        inline const VkPipelineViewportStateCreateInfo& getViewportState() const { return viewportState; }
        inline const VkPipelineRasterizationStateCreateInfo& getRasterizer() const { return rasterizer; }
        inline const VkPipelineMultisampleStateCreateInfo& getMultisampling() const { return multisampling; }
        inline const VkPipelineColorBlendAttachmentState& getColorBlendAttachment() const { return colorBlendAttachment; }
        inline const VkPipelineColorBlendStateCreateInfo& getColorBlendingState() const { return colorBlendingState; }
        inline const VkPipelineDynamicStateCreateInfo& getDynamicState() const { return dynamicState; }
        inline const VkGraphicsPipelineCreateInfo& getPipelineInfo() const { return pipelineInfo; }
};

#endif