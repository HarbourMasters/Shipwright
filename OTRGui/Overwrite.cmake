include(CMakePrintHelpers)

string(REPLACE "\\ " " " sources_dir "${src_dir}")
string(REPLACE "\\ " " " destination_dir "${dst_dir}")

file(GLOB_RECURSE _file_list RELATIVE "${sources_dir}" "${sources_dir}/*")

foreach( each_file ${_file_list} )
  set(destinationfile "${destination_dir}/${each_file}")
  set(sourcefile "${sources_dir}/${each_file}")
  if(NOT EXISTS ${destinationfile} OR ${sourcefile} IS_NEWER_THAN ${destinationfile})
    get_filename_component(destinationdir ${destinationfile} DIRECTORY)
    file(COPY ${sourcefile} DESTINATION ${destinationdir})
  endif()
endforeach(each_file)
