
cmake_minimum_required(VERSION 3.5)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

# ------------------------------------------------------------------------------------------ #
# Configure build paths
# ------------------------------------------------------------------------------------------ #

set(dir ${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir})
set(CMAKE_BUILD_FILES_DIRECTORY ${dir})
set(CMAKE_BUILD_DIRECTORY ${dir})
set(CMAKE_BINARY_DIR  ${dir})
set(EXECUTABLE_OUTPUT_PATH ${dir})
set(LIBRARY_OUTPUT_PATH ${dir}lib)
set(CMAKE_CACHEFILE_DIR ${dir})

# ------------------------------------------------------------------------------------------ #
# Configure optimization flags
# ------------------------------------------------------------------------------------------ #

string(CONCAT CMAKE_CXX_FLAGS
      "-Wall "
      "-Wextra "
      "-Wdouble-promotion "
      "-Wformat=2 "
      "-pthread "
      "-std=c++17 "
      "-mtune=native "
      "-march=native "
      )
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -Werror -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O1 -g -Wno-error=unused-parameter -Wno-error=unused-variable")

# ------------------------------------------------------------------------------------------ #
# Configure external libraries
# ------------------------------------------------------------------------------------------ #
# {
# REQUIRED
find_package(Boost COMPONENTS filesystem iostreams REQUIRED )
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND COMMON_LIBRARIES ${Boost_LIBRARIES})

find_package(GFlags REQUIRED)
include_directories(${GFLAGS_INCLUDE_DIRS})
link_directories(${GFLAGS_LIBRARYRARY_DIRS})
list(APPEND COMMON_LIBRARIES ${GFLAGS_LIBRARIES})

find_package(Glog REQUIRED)
include_directories(${GLOG_INCLUDE_DIRS})
list(APPEND COMMON_LIBRARIES ${GLOG_LIBRARY})

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
list(APPEND COMMON_LIBRARIES ${PROTOBUF_LIBRARIES})
# }


# ------------------------------------------------------------------------------------------ #
# Build
# ------------------------------------------------------------------------------------------ #

add_subdirectory(libsocks5)

