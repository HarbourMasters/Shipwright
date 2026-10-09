include("${CMAKE_CURRENT_LIST_DIR}/Default.cmake")

set_config_specific_property("OUTPUT_DIRECTORY" "${CMAKE_SOURCE_DIR}$<$<NOT:$<STREQUAL:${CMAKE_VS_PLATFORM_NAME},Win32>>:/${CMAKE_VS_PLATFORM_NAME}>/${PROPS_CONFIG}")

if(MSVC)
    create_property_reader("DEFAULT_CXX_EXCEPTION_HANDLING")
    create_property_reader("DEFAULT_CXX_DEBUG_INFORMATION_FORMAT")

    set_target_properties("${PROPS_TARGET}" PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    set_config_specific_property("DEFAULT_CXX_EXCEPTION_HANDLING" "/EHsc")

    # /Zi funnels every parallel cl.exe through one mspdbsrv.exe writing a shared PDB, and is not
    # cacheable by ccache/sccache. /Z7 keeps the debug info in the .obj instead.
    if (CMAKE_C_COMPILER_LAUNCHER MATCHES "ccache|sccache" OR NOT CMAKE_GENERATOR MATCHES "Visual Studio")
        set_config_specific_property("DEFAULT_CXX_DEBUG_INFORMATION_FORMAT" "/Z7")
    else()
        set_config_specific_property("DEFAULT_CXX_DEBUG_INFORMATION_FORMAT" "/Zi")
    endif()
endif()
