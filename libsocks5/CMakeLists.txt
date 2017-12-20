include_directories(./)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SOURCES_CC *.cc)


protobuf_generate_cpp(
	PROTO_SOURCES
	PROTO_HEADERS
	wire.proto
	)

add_library(socks5 STATIC ${SOURCES_CC} ${SOURCES_C} ${CUDA_OBJECTS} ${PROTO_SOURCES})
target_link_libraries(socks5 ${COMMON_LIBRARIES})
