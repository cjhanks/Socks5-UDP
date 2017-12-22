include_directories(./)

file(GLOB_RECURSE SOURCES_CC *.cc)
add_executable("s5u-remote" ${SOURCES_CC})

target_link_libraries("s5u-remote" ${COMMON_LIBRARIES})
