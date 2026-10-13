# 2ship's asset tools, included from the root CMakeLists.txt when BUILD_2SHIP is on.
# Only needs torch, so a SHIP_TOOLS_ONLY configure gets these without libultraship.
# The root project is SoH's, so 2ship keeps its own version and build name here.
set(TWOSHIP_VERSION 5.0.1)
set(TWOSHIP_BUILD_NAME "Battler Bravo")
set(TWOSHIP_TEAM "github.com/2ship2harkinian")

# Build-time ROM extraction, same as soh-torch but calling MMTorch::Extract
add_executable(2ship-torch EXCLUDE_FROM_ALL
    ${CMAKE_SOURCE_DIR}/2s2h/assets/tools/torch-cli/main.cpp
    ${CMAKE_SOURCE_DIR}/2s2h/2s2h/Extractor/TorchExtract.cpp
)
target_include_directories(2ship-torch PRIVATE ${CMAKE_SOURCE_DIR}/2s2h/2s2h/Extractor)
target_link_libraries(2ship-torch PRIVATE torch)

add_executable(2ship-o2r-packer EXCLUDE_FROM_ALL
    ${CMAKE_SOURCE_DIR}/2s2h/assets/tools/2ship-o2r-packer/main.cpp
    ${CMAKE_SOURCE_DIR}/2s2h/assets/tools/2ship-o2r-packer/PngTexture.cpp
)
target_link_libraries(2ship-o2r-packer PRIVATE torch)

if(MSVC)
    set_target_properties(2ship-torch 2ship-o2r-packer PROPERTIES
        MSVC_RUNTIME_LIBRARY "$<IF:$<CONFIG:Debug>,MultiThreadedDebug,MultiThreaded>")
endif()

# Target to generate OTRs. MM_ROM_PATH takes roms and/or directories of roms; torch names the
# archive (mm.o2r) from config.yml. 2ship.o2r comes from Generate2ShipOtr, chained below.
# roms/mm, not roms: extraction fails on a rom it doesn't know, so OoT and MM roms can't share a folder.
set(MM_ROM_PATH "${CMAKE_SOURCE_DIR}/roms/mm" CACHE STRING "Majora's Mask roms, or directories of roms, to extract")
add_custom_target(
    ExtractAssets2Ship
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_BINARY_DIR}/2s2h/mm.o2r
    COMMAND $<TARGET_FILE:2ship-torch>
            --src ${CMAKE_SOURCE_DIR}/2s2h/assets/yml
            --dest ${CMAKE_BINARY_DIR}/2s2h
            --version ${TWOSHIP_VERSION}
            ${MM_ROM_PATH}
    # torch caches extraction state next to the archive; drop it so the game's directory stays clean
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_BINARY_DIR}/2s2h/torch.hash.yml
    COMMENT "Running asset extraction..."
    DEPENDS 2ship-torch
    BYPRODUCTS ${CMAKE_BINARY_DIR}/2s2h/mm.o2r
)

# Torch emits no MM asset headers either, see ExtractAssetHeadersSoh
add_custom_target(
    ExtractAssetHeaders2Ship
    COMMAND ${CMAKE_COMMAND} -E echo "ExtractAssetHeaders2Ship is currently unavailable: torch does not emit MM asset headers yet."
    COMMAND ${CMAKE_COMMAND} -E echo "The checked-in headers under 2s2h/assets are unaffected."
    COMMAND ${CMAKE_COMMAND} -E false
)

# 2ship.o2r is rebuilt only when its inputs change, and is part of ALL like soh.o2r.
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/2s2h)
file(GLOB_RECURSE TWOSHIP_O2R_ASSETS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/2s2h/assets/custom/*)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/2s2h/2ship.o2r
    COMMAND $<TARGET_FILE:2ship-o2r-packer>
            ${CMAKE_SOURCE_DIR}/2s2h/assets/custom
            ${CMAKE_BINARY_DIR}/2s2h/2ship.o2r
            ${TWOSHIP_VERSION}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/2s2h/2ship.o2r ${CMAKE_SOURCE_DIR}/2ship.o2r
    COMMENT "Generating 2ship.o2r..."
    DEPENDS 2ship-o2r-packer ${TWOSHIP_O2R_ASSETS}
    BYPRODUCTS ${CMAKE_SOURCE_DIR}/2ship.o2r
    VERBATIM
)
add_custom_target(Generate2ShipOtr ALL DEPENDS ${CMAKE_BINARY_DIR}/2s2h/2ship.o2r)

# ExtractAssets produced 2ship.o2r as well as mm.o2r, so keep doing that.
add_dependencies(ExtractAssets2Ship Generate2ShipOtr)
add_dependencies(ExtractAssets ExtractAssets2Ship)
add_dependencies(ExtractAssetHeaders ExtractAssetHeaders2Ship)
