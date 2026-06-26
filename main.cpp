#include <katra/katra.h>

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

const std::vector<const char*> REQUIRED_DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
const std::vector<const char*> PREFERED_DEVICE_EXTENSIONS = {
    "VK_KHR_portability_subset"
};

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

bool isDeviceSuitable(PhysicalDevice& device) {
    return  device.checkExtensionSupport(REQUIRED_DEVICE_EXTENSIONS) &&
            device.supportsSwapChain() &&
            device.hasGraphicsFamily() && 
            device.hasPresentFamily();
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

    Buffer* vertexBuffer;
    Buffer* indexBuffer;
    std::vector<Buffer*> uniformBuffers;

    DescriptorPool* descriptorPool;
    std::vector<DescriptorSet*> descriptorSets;

    // VkDescriptorSetLayout descriptorSetLayout;
    // VkPipelineLayout pipelineLayout;


    void initVulkan() {
        instance = new Instance("Katra", enableValidationLayers, VK_API_VERSION_1_2);
        if (enableValidationLayers) { debugger = new Debugger(instance); }
        window = new Window("Katra", 800, 600);
        surface = new Surface(instance, window);

        pickPhysicalDevice();
        logicalDevice = new LogicalDevice(physicalDevice, REQUIRED_DEVICE_EXTENSIONS, PREFERED_DEVICE_EXTENSIONS);
                
        swapChain = new SwapChain(logicalDevice, surface);
        renderPass = new RenderPass(swapChain);
        vertexInput = new VertexInput(logicalDevice, 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX, {
            {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos)},
            {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)}
        });
        descriptorLayout = new DescriptorLayout(logicalDevice, {
            {0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT}
        });
        graphicsPipeline = new GraphicsPipeline(renderPass, "shaders/shader.vert.spv", "shaders/shader.frag.spv", vertexInput, {descriptorLayout});
        createFramebuffers();

        commandPool = new CommandPool(logicalDevice, logicalDevice->getGraphicsFamilyIndex());
        createCommandBuffers();
        createSyncObjects();

        createBuffers();
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
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapChain->getImageCount()}
        });

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

        for (size_t i = 0; i < swapChain->getImageCount(); i++) {
            DescriptorSet* descriptorSet = new DescriptorSet(descriptorPool, descriptorLayout);
            descriptorSets.push_back(descriptorSet);
            descriptorSet->update(uniformBuffers[i], 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        }
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
        // Loop through images and create a framebuffer
        for (size_t i = 0; i < swapChain->getImageCount(); i++) {
            std::vector<VkImageView> attachments = { swapChain->getImageViews()[i] };
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
        commandBuffer->bindIndexBuffer(indexBuffer);
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
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

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