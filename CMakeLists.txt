cmake_minimum_required(VERSION 2.8)
project(futil)

set(CMAKE_VERBOSE_MAKEFILE ON)
message(STATUS "System: " ${CMAKE_SYSTEM_NAME} " " ${CMAKE_SYSTEM_VERSION})
message(STATUS "Processor: " ${CMAKE_HOST_SYSTEM_PROCESSOR})
message(STATUS "CMake generates: " ${CMAKE_GENERATOR})
message(STATUS "Build type:" ${CMAKE_BUILD_TYPE})

#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
#list(REMOVE_DUPLICATES CMAKE_MODULE_PATH)

#Enable C++0x
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wc++11-compat -Wall -Wextra -pipe -fvisibility=hidden")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -flto -march=native -mtune=native")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIR})

set(futil_UTIL_HEADERS
  include/util/visibility.h
  include/util/bitutil.h
  include/util/patricia_tree.h
  include/util/patricia_set.h
  include/util/patricia_map.h
  include/util/rlucache.h
)

add_executable(trie_test test/main.cpp test/test.cpp ${futil_UTIL_HEADERS})
target_link_libraries(trie_test ${Boost_LIBRARIES})

#add_library(futil STATIC
#  ${futil_UTIL_HEADERS}
# )

include_directories(include)

install(FILES ${futil_UTIL_HEADERS} DESTINATION include/futil/util)
#install (TARGETS futil DESTINATION lib)
