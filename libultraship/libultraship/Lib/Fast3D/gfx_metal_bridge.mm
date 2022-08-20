//
//  gfx_metal_bridge.mm
//  libultraship
//
//  Created by David Chavez on 16.08.22.
//

#ifdef ENABLE_METAL

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

void set_layer_pixel_format(void* layer)
{
    CAMetalLayer* metalLayer = (__bridge CAMetalLayer*) layer;
    metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
}

void set_layer_drawable_size(void* layer, uint32_t width, uint32_t height)
{
    CAMetalLayer* metalLayer = (__bridge CAMetalLayer*) layer;
    metalLayer.drawableSize = CGSizeMake(width, height);
}

MTL::Device* get_layer_device(void* layer)
{
    CAMetalLayer* metalLayer = (__bridge CAMetalLayer*) layer;
    return (__bridge MTL::Device*)metalLayer.device;
}

CA::MetalDrawable* get_layer_next_drawable(void* layer)
{
  CAMetalLayer* metalLayer = (__bridge CAMetalLayer*) layer;
  id <CAMetalDrawable> metalDrawable = [metalLayer nextDrawable];
  CA::MetalDrawable* metalCppDrawable = (__bridge CA::MetalDrawable*)metalDrawable;
  return metalCppDrawable;
}

void set_command_buffer_label(MTL::CommandBuffer* command_buffer, int framebuffer_index) {
    id<MTLCommandBuffer> metalCommandBuffer = (__bridge id<MTLCommandBuffer>) command_buffer;
    metalCommandBuffer.label = [NSString stringWithFormat:@"FrameBuffer (%d) Command Buffer", framebuffer_index];
}

void set_command_encoder_label(MTL::RenderCommandEncoder* encoder, int framebuffer_index) {
    id<MTLRenderCommandEncoder> metalEncoder = (__bridge id<MTLRenderCommandEncoder>) encoder;
    metalEncoder.label = [NSString stringWithFormat:@"FrameBuffer (%d) Command Encoder", framebuffer_index];
}

#endif
