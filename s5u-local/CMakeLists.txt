include_directories(./)

file(GLOB_RECURSE SOURCES_CC *.cc)
add_executable(s5u_local ${SOURCES_CC})

target_link_libraries(s5u_local ${COMMON_LIBRARIES})
