# Normalize Android ARM64 heap pointers before libultraship's OTR signature
# address-range check. Android may tag heap pointers in their top byte; without
# stripping that tag, valid pointers to OTR path strings copied into game-state
# buffers look like non-canonical addresses and are treated as raw texture data.
if(NOT DEFINED INTERPRETER_FILE OR NOT EXISTS "${INTERPRETER_FILE}")
    message(FATAL_ERROR "INTERPRETER_FILE must name libultraship/src/fast/interpreter.cpp")
endif()

file(READ "${INTERPRETER_FILE}" interpreter)
string(FIND "${interpreter}" "Android ARM64 tagged-pointer normalization" marker_position)
if(NOT marker_position EQUAL -1)
    return()
endif()

set(old_code [=[int32_t gfx_check_image_signature(const char* imgData) {
    uintptr_t i = (uintptr_t)(imgData);

    if ((i & 1) == 1) {]=])
set(new_code [=[int32_t gfx_check_image_signature(const char* imgData) {
    uintptr_t i = (uintptr_t)(imgData);
#if defined(__ANDROID__) && defined(__aarch64__)
    // Android ARM64 tagged-pointer normalization: heap pointers may use the top byte as a memory tag.
    // Strip it before the canonical-address filter and signature read below.
    i &= 0x00FFFFFFFFFFFFFFull;
    imgData = reinterpret_cast<const char*>(i);
#endif

    if ((i & 1) == 1) {]=])

string(FIND "${interpreter}" "${old_code}" old_position)
if(old_position EQUAL -1)
    message(FATAL_ERROR "Could not find the expected gfx_check_image_signature code in ${INTERPRETER_FILE}")
endif()

string(REPLACE "${old_code}" "${new_code}" patched_interpreter "${interpreter}")
if(patched_interpreter STREQUAL interpreter)
    message(FATAL_ERROR "Failed to normalize Android ARM64 tagged pointers in ${INTERPRETER_FILE}")
endif()

file(WRITE "${INTERPRETER_FILE}" "${patched_interpreter}")
