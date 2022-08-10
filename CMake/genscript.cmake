file(READ ${CMAKE_CURRENT_SOURCE_DIR}/extract_assets.py filedata)
string(REGEX REPLACE "zapd_exe = .*exec_cmd =" "zapd_exe = \"${program}\"\n    exec_cmd =" filedata "${filedata}")
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/extract_assets_cmake2.py" "${filedata}")
file(CHMOD "${CMAKE_CURRENT_SOURCE_DIR}/extract_assets_cmake2.py" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

