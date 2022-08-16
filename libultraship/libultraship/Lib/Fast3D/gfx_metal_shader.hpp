//
//  gfx_metal_shader.hpp
//  libultraship
//
//  Created by David Chavez on 16.08.22.
//

#ifdef ENABLE_METAL

#ifndef gfx_metal_shader_hpp
#define gfx_metal_shader_hpp

#include <stdio.h>
#include "gfx_cc.h"

MTL::VertexDescriptor* gfx_metal_build_shader(char buf[4096], size_t& num_floats, const CCFeatures& cc_features, bool three_point_filtering);

#endif /* gfx_metal_shader_hpp */
#endif
