#ifndef KATRA_KATRA_H
#define KATRA_KATRA_H

//Util
#include <katra/util/includes.h>
#include <katra/util/constants.h>

// Debug
#include <katra/debug/debugger.h>

// Context
#include <katra/context/glfw_context.h>

// Render
#include <katra/render/instance.h>
#include <katra/render/surface.h>
#include <katra/render/window.h>
#include <katra/render/swap_chain.h>
#include <katra/render/render_pass.h>
#include <katra/render/shader_module.h>
#include <katra/render/framebuffer.h>

// Pipeline
#include <katra/pipeline/graphics_pipeline.h>
#include <katra/pipeline/vertex_input.h>

// Device
#include <katra/device/physical_device.h>
#include <katra/device/logical_device.h>

// Command
#include <katra/command/command_pool.h>
#include <katra/command/command_buffer.h>

// Synchronization
#include <katra/synchronization/fence.h>
#include <katra/synchronization/semaphore.h>

// Buffer
#include <katra/buffer/buffer.h>

// Descriptor
#include <katra/descriptor/descriptor_pool.h>
#include <katra/descriptor/descriptor_set.h>
#include <katra/descriptor/descriptor_layout.h>

#endif