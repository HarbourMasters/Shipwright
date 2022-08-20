//
//  gfx_metal_bridge.hpp
//  libultraship
//
//  Created by David Chavez on 16.08.22.
//

#ifdef ENABLE_METAL

#ifndef gfx_metal_bridge_hpp
#define gfx_metal_bridge_hpp

void set_layer_pixel_format(void* layer);
void set_layer_drawable_size(void* layer, uint32_t width, uint32_t height);
void set_command_buffer_label(MTL::CommandBuffer* command_buffer, int framebuffer_index);
void set_command_encoder_label(MTL::RenderCommandEncoder* encoder, int framebuffer_index);

MTL::Device* get_layer_device(void* layer);
CA::MetalDrawable* get_layer_next_drawable(void* layer);

#endif
#endif
