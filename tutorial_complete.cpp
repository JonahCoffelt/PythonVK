#include <katra/katra.h>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv;
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

const std::vector<const char*> REQUIRED_DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
const std::vector<const char*> PREFERED_DEVICE_EXTENSIONS = {
    "VK_KHR_portability_subset"
};

bool isDeviceSuitable(PhysicalDevice& device) {
    return  device.checkExtensionSupport(REQUIRED_DEVICE_EXTENSIONS) &&
            device.supportsSwapChain() &&
            device.hasGraphicsFamily() && 
            device.hasPresentFamily() &&
            device.hasSamplerAnisotropy();
}

class HelloTriangleApplication {
public:
    void run() {
        GLFWContext* context = new GLFWContext();
        initVulkan();
        mainLoop();
        cleanup();
        delete context;
    }

private:
    Window* window;
    Instance* instance;
    Debugger* debugger;
    Surface* surface;
    PhysicalDevice* physicalDevice;
    LogicalDevice* logicalDevice;
    SwapChain* swapChain;
    RenderPass* renderPass;
    
    // Pipeline
    GraphicsPipeline* graphicsPipeline;
    VertexInput* vertexInput;
    DescriptorLayout* descriptorLayout;

    std::vector<Framebuffer*> swapChainFramebuffers;
    CommandPool* commandPool;
    std::vector<CommandBuffer*> commandBuffers;
    std::vector<Semaphore*> imageAvailableSemaphores;
    std::vector<Semaphore*> renderFinishedSemaphores;
    std::vector<Fence*> inFlightFences;
    uint32_t currentFrame = 0;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    Buffer* vertexBuffer;
    Buffer* indexBuffer;
    std::vector<Buffer*> uniformBuffers;

    DescriptorPool* descriptorPool;
    std::vector<DescriptorSet*> descriptorSets;

    Image* textureImage;
    ImageView* textureImageView;
    Sampler* textureSampler;
    
    Image* depthImage;
    ImageView* depthImageView;
    DepthStencil* depthStencil;

    Image* multisampleImage;
    ImageView* multisampleImageView;

    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    
    void initVulkan() {
        instance = new Instance("Katra", enableValidationLayers, VK_API_VERSION_1_2);
        if (enableValidationLayers) { debugger = new Debugger(instance); }
        window = new Window("Katra", 800, 600);
        surface = new Surface(instance, window);

        pickPhysicalDevice();
        logicalDevice = new LogicalDevice(physicalDevice, REQUIRED_DEVICE_EXTENSIONS, PREFERED_DEVICE_EXTENSIONS);
        msaaSamples = physicalDevice->getMaxUsableSampleCount();
        
        swapChain = new SwapChain(logicalDevice, surface);

        commandPool = new CommandPool(logicalDevice, logicalDevice->getGraphicsFamilyIndex());
        createColorResources();
        createDepthResources();
        renderPass = new RenderPass(swapChain, depthImageView, multisampleImageView, msaaSamples);
        vertexInput = new VertexInput(logicalDevice, 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)},
            {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)},
            {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)}
        });
        descriptorLayout = new DescriptorLayout(logicalDevice, {
            {0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
            {1, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
        });
        depthStencil = new DepthStencil(
            true, true, VK_COMPARE_OP_LESS, false
        );
        graphicsPipeline = new GraphicsPipeline(renderPass, "shaders/shader.vert.spv", "shaders/shader.frag.spv", vertexInput, {descriptorLayout}, depthStencil);
        createFramebuffers();
        
        createCommandBuffers();
        createSyncObjects();
        
        loadModel();
        createBuffers();
        createTextureImage();
        createDescriptors();
    }

    void createBuffers() {
        // Create vertex buffer
        vertexBuffer = new Buffer(
            logicalDevice, 
            sizeof(vertices[0]) * vertices.size(), 
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        // Write vertex data to staging buffer
        Buffer* stagingBuffer = new Buffer(
            logicalDevice, 
            sizeof(vertices[0]) * vertices.size(), 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        stagingBuffer->write(vertices.data(), sizeof(vertices[0]) * vertices.size());

        // Copy staging buffer to vertex
        CommandBuffer* copyCommand = new CommandBuffer(commandPool);
        copyCommand->begin();
        copyCommand->copyBuffer(stagingBuffer, vertexBuffer, sizeof(vertices[0]) * vertices.size());
        copyCommand->end();
        Fence* fence = new Fence(logicalDevice);
        copyCommand->submit(logicalDevice->getGraphicsQueue(), {}, {}, fence);
        fence->wait();
        delete fence;
        delete stagingBuffer;
        delete copyCommand;

        // Create index buffer
        indexBuffer = new Buffer(
            logicalDevice, 
            sizeof(indices[0]) * indices.size(), 
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        // Write index data to staging buffer
        stagingBuffer = new Buffer(
            logicalDevice, 
            sizeof(indices[0]) * indices.size(), 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        stagingBuffer->write(indices.data(), sizeof(indices[0]) * indices.size());

        // Copy staging buffer to index
        copyCommand = new CommandBuffer(commandPool);
        copyCommand->begin();
        copyCommand->copyBuffer(stagingBuffer, indexBuffer, sizeof(indices[0]) * indices.size());
        copyCommand->end();
        fence = new Fence(logicalDevice);
        copyCommand->submit(logicalDevice->getGraphicsQueue(), {}, {}, fence);
        fence->wait();
        delete fence;
        delete stagingBuffer;
        delete copyCommand;
    }

    void createDescriptors() {
        descriptorPool = new DescriptorPool(logicalDevice, swapChain->getImageCount(), {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapChain->getImageCount()},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, swapChain->getImageCount()}
        });

        // Create uniform buffers
        for (size_t i = 0; i < swapChain->getImageCount(); i++) {
            Buffer* uniformBuffer = new Buffer(
                logicalDevice, 
                sizeof(UniformBufferObject), 
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );
            uniformBuffer->mapMemory();
            uniformBuffers.push_back(uniformBuffer);
        }

        // Bind uniform buffers and sampler
        for (size_t i = 0; i < swapChain->getImageCount(); i++) {
            DescriptorSet* descriptorSet = new DescriptorSet(descriptorPool, descriptorLayout);
            descriptorSets.push_back(descriptorSet);
            descriptorSet->update(uniformBuffers[i], 0);
            descriptorSet->update(textureImageView, textureSampler, 1);
        }
    }

    void createTextureImage() {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load("textures/viking_room.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        // Get image size
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        // Create staging buffer
        Buffer* stagingBuffer = new Buffer(
            logicalDevice, 
            imageSize, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        stagingBuffer->write(pixels, imageSize);
        stbi_image_free(pixels);

        // Choose number of mips
        uint32_t mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        // Create the image
        textureImage = new Image(
            logicalDevice, 
            texWidth, 
            texHeight, 
            mip_levels,
            VK_FORMAT_R8G8B8A8_SRGB, 
            VK_IMAGE_TILING_OPTIMAL, 
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_SAMPLE_COUNT_1_BIT
        );
        
        // Create command buffer and fence
        CommandBuffer* imageCopyCommand = new CommandBuffer(commandPool);
        Fence* fence = new Fence(logicalDevice);
        imageCopyCommand->begin();

        // Transition image to transfer destination optimal layout
        ImageBarrier* imageBarrier = new ImageBarrier(
            logicalDevice, 
            textureImage, 
            VK_IMAGE_LAYOUT_UNDEFINED, 
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            textureImage->getFormat(),
            0,
            mip_levels
        );
        imageCopyCommand->pipelineBarrier(imageBarrier);
        delete imageBarrier;
        
        // Copy buffer to image
        imageCopyCommand->copyBufferToImage(stagingBuffer, textureImage, texWidth, texHeight, 0, 0, 0, 0, 1);
        
        imageCopyCommand->end();
        imageCopyCommand->submit(logicalDevice->getGraphicsQueue(), {}, {}, fence);
        fence->wait();
        delete fence;
        delete imageCopyCommand;
        delete stagingBuffer;

        // Create texture image view
        textureImageView = new ImageView(
            textureImage,
            VK_IMAGE_VIEW_TYPE_2D,
            VK_IMAGE_ASPECT_COLOR_BIT,
            mip_levels
        );
    
        // Create texture sampler
        textureSampler = new Sampler(
            logicalDevice, 
            VK_FILTER_LINEAR, 
            VK_SAMPLER_MIPMAP_MODE_LINEAR, 
            VK_SAMPLER_ADDRESS_MODE_REPEAT, 
            static_cast<float>(mip_levels)
        );

        generateMipmaps(textureImage);
    }

    void generateMipmaps(Image* image) {
        CommandBuffer* commandBuffer = new CommandBuffer(commandPool);
        Fence* fence = new Fence(logicalDevice);
        commandBuffer->begin();

        ImageBarrier* barrier = new ImageBarrier(
            logicalDevice,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image->getFormat()
        );

        int32_t mipWidth = image->getWidth();
        int32_t mipHeight = image->getHeight();
        for (uint32_t i = 1; i < image->getMipLevels(); i++) {
            barrier->setBaseMipLevel(i - 1);
            barrier->setLevelCount(1);
            barrier->setOldLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            barrier->setNewLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
            barrier->setSrcAccessMask(VK_ACCESS_TRANSFER_WRITE_BIT);
            barrier->setDstAccessMask(VK_ACCESS_TRANSFER_READ_BIT);
            barrier->setSourceStage(VK_PIPELINE_STAGE_TRANSFER_BIT);
            barrier->setDestinationStage(VK_PIPELINE_STAGE_TRANSFER_BIT);
            commandBuffer->pipelineBarrier(barrier);

            commandBuffer->blitImage(
                image,
                image,
                ImageRegion(0, 0, 0, mipWidth, mipHeight, 1),
                ImageSubresource(VK_IMAGE_ASPECT_COLOR_BIT, i - 1, 1, 0, 1),
                ImageRegion(0, 0, 0, mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1),
                ImageSubresource(VK_IMAGE_ASPECT_COLOR_BIT, i, 1, 0, 1),
                VK_FILTER_LINEAR
            );

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        const uint32_t mipLevels = image->getMipLevels();
        if (mipLevels > 1) {
            barrier->setBaseMipLevel(0);
            barrier->setLevelCount(mipLevels - 1);
            barrier->setOldLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
            barrier->setNewLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            barrier->setSrcAccessMask(VK_ACCESS_TRANSFER_READ_BIT);
            barrier->setDstAccessMask(VK_ACCESS_SHADER_READ_BIT);
            barrier->setSourceStage(VK_PIPELINE_STAGE_TRANSFER_BIT);
            barrier->setDestinationStage(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            commandBuffer->pipelineBarrier(barrier);
        }

        barrier->setBaseMipLevel(mipLevels - 1);
        barrier->setLevelCount(1);
        barrier->setOldLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        barrier->setNewLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        barrier->setSrcAccessMask(VK_ACCESS_TRANSFER_READ_BIT);
        barrier->setDstAccessMask(VK_ACCESS_SHADER_READ_BIT);
        barrier->setSourceStage(VK_PIPELINE_STAGE_TRANSFER_BIT);
        barrier->setDestinationStage(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        commandBuffer->pipelineBarrier(barrier);

        commandBuffer->end();
        commandBuffer->submit(logicalDevice->getGraphicsQueue(), {}, {}, fence);
        fence->wait();
        delete fence;
        delete barrier;
        delete commandBuffer;
    }

    void createColorResources() {
        VkFormat colorFormat = swapChain->getSurfaceFormat().format;

        multisampleImage = new Image(
            logicalDevice,
            swapChain->getExtent().width,
            swapChain->getExtent().height,
            1,
            colorFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            msaaSamples
        );

        multisampleImageView = new ImageView(
            multisampleImage,
            VK_IMAGE_VIEW_TYPE_2D,
            VK_IMAGE_ASPECT_COLOR_BIT
        );
    }
    
    void pickPhysicalDevice() {
        // Get all physical devices
        std::vector<PhysicalDevice> physicalDevices = getPhysicalDevices(instance, surface);
        
        // Find device with best score
        unsigned int bestScore = 0;
        PhysicalDevice* bestDevice = nullptr;
        for (auto& device : physicalDevices) {
            if (!isDeviceSuitable(device)) continue;
            if (!bestDevice || device.getScore() > bestScore) {
                bestScore = device.getScore();
                bestDevice = &device;
            }
        }
        
        // If there is no suitable device, you lowk fucked and poor as well
        if (!bestDevice) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        // Create physical device object from best device
        physicalDevice = new PhysicalDevice(bestDevice->getHandle(), surface);
        std::cout << "Using device: " << physicalDevice->getName() << std::endl;
    }

    void createFramebuffers() {
        for (size_t i = 0; i < swapChain->getImageCount(); i++) {
            std::vector<VkImageView> attachments = {
                multisampleImageView->getHandle(),
                depthImageView->getHandle(),
                swapChain->getImageViews()[i]->getHandle()
            };
            swapChainFramebuffers.push_back(new Framebuffer(logicalDevice, renderPass, attachments, 1));
        }
    }

    void createCommandBuffers() {
        // Make buffers for each in flight frame
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            commandBuffers.push_back(new CommandBuffer(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY));
        }
    }

    void createSyncObjects() {
        // Create MAX_FRAMES_IN_FLIGHT imageAvailableSemaphores and inFlightFences
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            inFlightFences.push_back(new Fence(logicalDevice, true));
            imageAvailableSemaphores.push_back(new Semaphore(logicalDevice));
        }
        // Create swapChainImages.size() renderFinishedSemaphores
        for (size_t i = 0; i < swapChain->getImageCount(); i++) {
            renderFinishedSemaphores.push_back(new Semaphore(logicalDevice));
        }
    }

    void createDepthResources() {
        VkFormat format = physicalDevice->getDepthFormat();
        depthImage = new Image(
            logicalDevice,
            swapChain->getExtent().width,
            swapChain->getExtent().height,
            1,
            format,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            msaaSamples,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );
        VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) {
            aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        depthImageView = new ImageView(depthImage, VK_IMAGE_VIEW_TYPE_2D, aspectFlags);

        CommandBuffer* depthCommand = new CommandBuffer(commandPool);
        Fence* fence = new Fence(logicalDevice);
        ImageBarrier* barrier = new ImageBarrier(
            logicalDevice,
            depthImage,
            VK_IMAGE_LAYOUT_UNDEFINED, 
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            format
        );
        depthCommand->begin();
        depthCommand->pipelineBarrier(barrier);
        depthCommand->end();
        depthCommand->submit(logicalDevice->getGraphicsQueue(), {}, {}, fence);
        fence->wait();
        delete barrier;
        delete depthCommand;
        delete fence;
    }

    void loadModel() {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "models/viking_room.obj")) {
            throw std::runtime_error(err);
        }

        if (!warn.empty()) {
            std::cout << warn << std::endl;
        }

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};

                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };
                
                vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };
                
                vertex.color = {1.0f, 1.0f, 1.0f};
        
                vertices.push_back(vertex);
                indices.push_back(indices.size());
            }
        }
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window->getHandle())) {
            glfwPollEvents();
            drawFrame();
        }

        logicalDevice->waitIdle();
    }

    void drawFrame() {
        // Get the in flight fence, command buffer, and image available semaphore for the current frame
        Fence* inFlightFence = inFlightFences[currentFrame];

        
        // Make sure no other frame is rendering
        inFlightFence->wait();
        inFlightFence->reset();
        
        // Aquire Image from swap chain
        Semaphore* imageAvailableSemaphore = imageAvailableSemaphores[currentFrame];
        uint32_t imageIndex = swapChain->acquireImage(imageAvailableSemaphore);
       
        // Update the uniform buffer
        updateUniformBuffer(imageIndex);

        // Get the render finished semaphore and framebuffer for the current image
        Semaphore* renderFinishedSemaphore = renderFinishedSemaphores[imageIndex];
        Framebuffer* framebuffer = swapChainFramebuffers[imageIndex];
        
        // Record the command buffer
        CommandBuffer* commandBuffer = commandBuffers[currentFrame];
        commandBuffer->reset();
        commandBuffer->begin();
        commandBuffer->beginRenderPass(renderPass, framebuffer, {0.0f, 0.0f, 0.0f, 1.0f});
        commandBuffer->bindPipeline(graphicsPipeline);
        commandBuffer->setViewport(0.0f, 0.0f, static_cast<float>(swapChain->getExtent().width), static_cast<float>(swapChain->getExtent().height));
        commandBuffer->setScissor(0, 0, swapChain->getExtent().width, swapChain->getExtent().height);
        commandBuffer->bindVertexBuffer(vertexBuffer, 0, 0);
        commandBuffer->bindIndexBuffer(indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        commandBuffer->bindDescriptorSet(graphicsPipeline, descriptorSets[imageIndex], 0);
        commandBuffer->drawIndexed(static_cast<uint32_t>(indices.size()));
        commandBuffer->endRenderPass();
        commandBuffer->end();

        // Submit the command buffer
        commandBuffer->submit(logicalDevice->getGraphicsQueue(), {imageAvailableSemaphore}, {renderFinishedSemaphore}, inFlightFence);

        // Present the image to the screen
        logicalDevice->present(swapChain->getHandle(), imageIndex, {renderFinishedSemaphore});

        // Increment the current frame
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void updateUniformBuffer(uint32_t imageIndex) {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count() / 5.0;

        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), swapChain->getExtent().width / (float) swapChain->getExtent().height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        uniformBuffers[imageIndex]->write(&ubo, sizeof(UniformBufferObject));
    }

    void cleanup() {
        // If validation is enabled, destory the messenger
        if (enableValidationLayers) {
            delete debugger;
        }
        // Destroy synchronization objects
        for (size_t i = 0; i < swapChain->getImageCount(); i++) {
            delete renderFinishedSemaphores[i];
        }
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            delete inFlightFences[i];
            delete imageAvailableSemaphores[i];
        }
        // Destroy all descriptor sets
        for (auto descriptorSet : descriptorSets) {
            delete descriptorSet;
        }
        // Destroy all framebuffer (must happen before image pipeline destroy)
        for (auto framebuffer : swapChainFramebuffers) {
            delete framebuffer;
        }
        // Destroy all command buffers
        for (auto commandBuffer : commandBuffers) {
            delete commandBuffer;
        }
        // Destroy all uniform buffers
        for (auto uniformBuffer : uniformBuffers) {
            delete uniformBuffer;
        }

        delete multisampleImageView;
        delete multisampleImage;
        delete depthStencil;
        delete depthImageView;
        delete depthImage;
        delete textureSampler;
        delete textureImageView;
        delete textureImage;
        delete descriptorPool;
        delete descriptorLayout;
        delete vertexBuffer;
        delete indexBuffer;
        delete vertexInput;
        delete commandPool;
        delete graphicsPipeline;
        delete renderPass;
        delete swapChain;
        delete logicalDevice;
        delete physicalDevice;
        delete surface;
        delete window;
        delete instance;
    }
};

int main() {
    HelloTriangleApplication app;
    app.run();
}