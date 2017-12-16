include_directories(./)

file(GLOB_RECURSE SOURCES_CC *.cc)

add_library(socks5 STATIC ${SOURCES_CC} ${SOURCES_C} ${CUDA_OBJECTS})
target_link_libraries(socks5 ${COMMON_LIBRARIES})
