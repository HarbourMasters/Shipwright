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

// Takes var args and returns NSString with concatenated args.
NS::String* string_with_format(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    NSString* string = [[NSString alloc] initWithFormat:@(format) arguments:args];
    va_end(args);
    return (__bridge_retained NS::String*)string;
}

#endif
