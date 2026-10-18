# Add explicit fragment precision defaults to the Fast3D GLSL template before
# it is packed into soh.o2r. GLES fragment shaders otherwise default to
# mediump, which can lose texture-coordinate and texture-size precision on
# mobile GPUs. The template's opengles condition keeps desktop GL unchanged.
if(NOT DEFINED SHADER_FILE OR NOT EXISTS "${SHADER_FILE}")
    message(FATAL_ERROR "SHADER_FILE must name an existing Fast3D GLSL template")
endif()

file(READ "${SHADER_FILE}" shader)
string(FIND "${shader}" "precision highp float;" precision_position)
if(NOT precision_position EQUAL -1)
    return()
endif()

set(shader_version_token "@{GLSL_VERSION}")
string(FIND "${shader}" "${shader_version_token}" token_position)
if(token_position EQUAL -1)
    message(FATAL_ERROR "Fast3D GLSL template is missing ${shader_version_token}")
endif()

set(shader_version_with_gles_precision [=[@{GLSL_VERSION}

@if(opengles)
    precision highp float;
    precision highp int;
@end]=])
string(REPLACE "${shader_version_token}" "${shader_version_with_gles_precision}" patched_shader "${shader}")
if(patched_shader STREQUAL shader)
    message(FATAL_ERROR "Failed to add GLES precision defaults to ${SHADER_FILE}")
endif()

file(WRITE "${SHADER_FILE}" "${patched_shader}")
