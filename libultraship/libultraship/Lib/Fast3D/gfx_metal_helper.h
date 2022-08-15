#ifdef ENABLE_METAL

#ifndef GFX_METAL_HELPER_H
#define GFX_METAL_HELPER_H

void set_layer_pixel_format(void* layer);
void set_layer_drawable_size(void* layer, uint32_t width, uint32_t height);

MTL::Device* get_layer_device(void* layer);
CA::MetalDrawable* get_layer_next_drawable(void* layer);

NS::String* string_with_format(const char* format, ...);

#endif
#endif
