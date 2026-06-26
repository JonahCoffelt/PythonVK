#ifndef KATRA_DESCRIPTOR_SET_H
#define KATRA_DESCRIPTOR_SET_H

// Includes
#include <katra/util/includes.h>
#include <katra/device/logical_device.h>
#include <katra/descriptor/descriptor_pool.h>
#include <katra/descriptor/descriptor_layout.h>
#include <katra/buffer/buffer.h>


// Class Declaration
class DescriptorSet {
    private:
        LogicalDevice* device;
        DescriptorPool* pool;
        DescriptorLayout* layout;

        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        VkDescriptorSetAllocateInfo allocateInfo{};
        
        void setAllocateInfo();
        
    public:
        DescriptorSet(DescriptorPool* pool, DescriptorLayout* layout);
        ~DescriptorSet();
        DescriptorSet(const DescriptorSet&) = delete;
        DescriptorSet& operator=(const DescriptorSet&) = delete;

        void update(Buffer* buffer, uint32_t binding, VkDescriptorType descriptorType);

        inline const VkDescriptorSet& getHandle() const { return descriptorSet; }
        inline DescriptorPool* getPool() { return pool; }
        inline DescriptorLayout* getLayout() { return layout; }
        inline const VkDescriptorSetAllocateInfo& getAllocateInfo() const { return allocateInfo; }
};

#endif