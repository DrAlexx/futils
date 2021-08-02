cmake_minimum_required(VERSION 3.9)
project(futil)

set(CMAKE_VERBOSE_MAKEFILE ON)
enable_testing()

message(STATUS "System: " ${CMAKE_SYSTEM_NAME} " " ${CMAKE_SYSTEM_VERSION})
message(STATUS "Processor: " ${CMAKE_HOST_SYSTEM_PROCESSOR})
message(STATUS "CMake generates: " ${CMAKE_GENERATOR})
message(STATUS "Build type:" ${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wc++20-compat -Wall -Wextra -pipe -fno-rtti -fvisibility=hidden")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pipe -fvisibility=hidden")

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -O0 -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS_DEBUG "-ggdb -O0 -fno-omit-frame-pointer")

set(CMAKE_LINKER_FLAGS_RELEASE "${CMAKE_LINKER_FLAGS_RELEASE} -s ")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -s ")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s ")

if(WIN32)
  add_definitions(-D_WIN32)
  set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O2 ")
  set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O2 ")
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} ")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
else(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pthread")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -pthread")

  set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O2 -march=native -mtune=native -flto")
  set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O2 -march=native -mtune=native -flto")
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -flto ")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto ")

  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -Wl,-z,defs")

  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS} -fsanitize=address")

  set(USE_BINUTILS_WORKAROUND ON CACHE BOOL "use gcc-ar and gcc-ranlib instead ar and ranlib (is needed for LTO)")
  if(USE_BINUTILS_WORKAROUND)
    set(CMAKE_AR gcc-ar)
    set(CMAKE_RANLIB gcc-ranlib)
  endif(USE_BINUTILS_WORKAROUND)
endif(WIN32)

option(BUILD_DOC "Build documentation" ON)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules (JEMALLOC jemalloc)
pkg_search_module(JEMALLOC REQUIRED jemalloc)
include_directories(${JEMALLOC_INCLUDE_DIRS})

set(futil_HEADERS
    include/binary_tree/base.h
    include/binary_tree/binary_tree.h
    include/binary_tree/avl_balancer.h

    include/util/profiler.h
    include/util/const_pool.h
    include/util/visibility.h
    include/util/type_utils.h
    include/util/rlu_map.h
    include/util/shardmap.h
    include/util/allocator.h

#  include/util/bitutil.h
#  include/patricia_trie/patricia_trie.h
)

#set(futil_UTIL_SOURCES src/scoped_profiler.cpp)

#add_executable(trie_test test/main.cpp test/test.cpp ${futil_UTIL_HEADERS} ${futil_UTIL_SOURCES})
#target_link_libraries(trie_test ${Boost_LIBRARIES})

#add_executable(shardmap_test test/main.cpp test/shardmap.cpp ${futil_UTIL_HEADERS} ${futil_UTIL_SOURCES})
#target_link_libraries(shardmap_test ${Boost_LIBRARIES})

add_executable(avl_performance_test test/avl_performance_test.cpp ${futil_HEADERS} ${futil_SOURCES})
target_link_libraries(avl_performance_test ${Boost_LIBRARIES}  ${JEMALLOC_LIBRARIES})
#add_test(avl_tree ./avl_tree_test)

add_executable(avl_tree_test  test/avl_tree.cpp ${futil_HEADERS} ${futil_SOURCES})
target_link_libraries(avl_tree_test ${Boost_LIBRARIES})
add_test(avl_tree ./avl_tree_test)

add_executable(profiler_test  test/profiler.cpp ${futil_HEADERS} ${futil_SOURCES})
target_link_libraries(profiler_test ${Boost_LIBRARIES})
add_test(profiler ./profiler_test)

add_executable(const_pool_test test/const_pool.cpp ${futil_HEADERS} ${futil_SOURCES})
target_link_libraries(const_pool_test ${Boost_LIBRARIES})
add_test(const_pool_test ./const_pool_test)

add_executable(rlu_map_test test/rlu_map.cpp ${futil_HEADERS} ${futil_SOURCES})
target_link_libraries(rlu_map_test ${Boost_LIBRARIES})
add_test(rlu_map_test ./rlu_map_test)

#add_library(futil STATIC
#    ${futil_UTIL_SOURCES}
#    ${futil_UTIL_HEADERS}
# )
#install(FILES ${futil_UTIL_HEADERS} DESTINATION include/futil/util)
#install (TARGETS futil DESTINATION lib)
