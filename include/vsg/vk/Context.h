#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <deque>
#include <memory>

#include <vsg/core/Object.h>
#include <vsg/core/ScratchMemory.h>
#include <vsg/nodes/Group.h>
#include <vsg/state/GraphicsPipeline.h>
#include <vsg/vk/BufferData.h>
#include <vsg/vk/CommandPool.h>
#include <vsg/vk/DescriptorPool.h>
#include <vsg/vk/Fence.h>
#include <vsg/vk/ImageData.h>
#include <vsg/vk/MemoryBufferPools.h>

#include <vsg/commands/Command.h>

namespace vsg
{
    class VSG_DECLSPEC BuildAccelerationStructureCommand : public Inherit<Command, BuildAccelerationStructureCommand>
    {
    public:
        BuildAccelerationStructureCommand(Device* device, VkAccelerationStructureInfoNV* info, const VkAccelerationStructureNV& structure, Buffer* instanceBuffer, Allocator* allocator = nullptr);

        void compile(Context&) override {}
        void record(CommandBuffer& commandBuffer) const override;

        ref_ptr<Device> _device;
        VkAccelerationStructureInfoNV* _accelerationStructureInfo;
        VkAccelerationStructureNV _accelerationStructure;
        ref_ptr<Buffer> _instanceBuffer;

        // scratch buffer set after compile traversal before record of build commands
        ref_ptr<Buffer> _scratchBuffer;
    };

    class VSG_DECLSPEC Context
    {
    public:
        Context(Device* in_device, BufferPreferences bufferPreferences = {});

        Context(const Context& context);

        virtual ~Context();

        // used by BufferData.cpp, ComputePipeline.cpp, Descriptor.cpp, Descriptor.cpp, DescriptorSet.cpp, DescriptorSetLayout.cpp, GraphicsPipeline.cpp, ImageData.cpp, PipelineLayout.cpp, ShaderModule.cpp
        const uint32_t deviceID = 0;
        ref_ptr<Device> device;

        // used by GraphicsPipeline.cpp
        ref_ptr<RenderPass> renderPass;

        // pipeline states that are usually not set in a scene, e.g.,
        // the viewport state, but might be set for some uses
        GraphicsPipelineStates defaultPipelineStates;

        // pipeline states that must be set to avoid Vulkan errors
        // e.g., MultisampleState.
        // XXX MultisampleState is complicated because the sample
        // number needs to agree with the renderpass attachement, but
        // other parts of the state, like alpha to coverage, belong to
        // the scene graph .
        GraphicsPipelineStates overridePipelineStates;

        // DescriptorSet.cpp
        ref_ptr<DescriptorPool> descriptorPool;

        // transfer data settings
        // used by BufferData.cpp, ImageData.cpp
        ref_ptr<Queue> graphicsQueue;
        ref_ptr<CommandPool> commandPool;
        ref_ptr<CommandBuffer> commandBuffer;
        ref_ptr<Fence> fence;
        ref_ptr<Semaphore> semaphore;
        ref_ptr<ScratchMemory> scratchMemory;

        std::vector<ref_ptr<Command>> commands;

        void record();
        void waitForCompletion();

        ref_ptr<CommandBuffer> getOrCreateCommandBuffer();

        ref_ptr<MemoryBufferPools> deviceMemoryBufferPools;
        ref_ptr<MemoryBufferPools> stagingMemoryBufferPools;

        // raytracing
        VkDeviceSize scratchBufferSize;
        std::vector<ref_ptr<BuildAccelerationStructureCommand>> buildAccelerationStructureCommands;
    };

} // namespace vsg
