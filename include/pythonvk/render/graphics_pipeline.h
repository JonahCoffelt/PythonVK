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

        inline VkPipeline getHandle() { return pipeline; }
        inline VkPipelineLayout getLayout() { return pipelineLayout; }
        inline std::vector<VkPipelineShaderStageCreateInfo> getShaderStages() { return shaderStages; }
        inline std::vector<VkDynamicState> getDynamicStates() { return dynamicStates; }
        inline VkPipelineVertexInputStateCreateInfo getVertexInput() { return vertexInput; }
        inline VkPipelineInputAssemblyStateCreateInfo getInputAssembly() { return inputAssembly; }
        inline VkPipelineViewportStateCreateInfo getViewportState() { return viewportState; }
        inline VkPipelineRasterizationStateCreateInfo getRasterizer() { return rasterizer; }
        inline VkPipelineMultisampleStateCreateInfo getMultisampling() { return multisampling; }
        inline VkPipelineColorBlendAttachmentState getColorBlendAttachment() { return colorBlendAttachment; }
        inline VkPipelineColorBlendStateCreateInfo getColorBlendingState() { return colorBlendingState; }
        inline VkPipelineDynamicStateCreateInfo getDynamicState() { return dynamicState; }
        inline VkGraphicsPipelineCreateInfo getPipelineInfo() { return pipelineInfo; }
};

#endif