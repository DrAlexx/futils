cmake_minimum_required(VERSION 3.9)
project(futil)

set(CMAKE_VERBOSE_MAKEFILE ON)
message(STATUS "System: " ${CMAKE_SYSTEM_NAME} " " ${CMAKE_SYSTEM_VERSION})
message(STATUS "Processor: " ${CMAKE_HOST_SYSTEM_PROCESSOR})
message(STATUS "CMake generates: " ${CMAKE_GENERATOR})
message(STATUS "Build type:" ${CMAKE_BUILD_TYPE})

#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
#list(REMOVE_DUPLICATES CMAKE_MODULE_PATH)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wc++17-compat -Wall -Wextra -pipe -fno-rtti -fvisibility=hidden")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pipe -fvisibility=hidden")

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -O0 -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS_DEBUG "-ggdb -O0 -fno-omit-frame-pointer")

set(CMAKE_LINKER_FLAGS_RELEASE "${CMAKE_LINKER_FLAGS_RELEASE} -s ")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -s ")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s ")

if(WIN32)
  add_definitions(-D_WIN32)
  set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O2 -march=pentium4 -mtune=pentium4 ")
  set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O2 -march=pentium4 -mtune=pentium4 ")
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

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIR})

set(futil_UTIL_HEADERS
  include/util/visibility.h
  include/util/bitutil.h
  include/util/rlucache.h
  include/util/shardmap.h
  include/util/scoped_profiler.h

  include/util/patricia_tree.h
#  include/util/patricia_set.h
#  include/util/patricia_map.h

  include/util/binary_tree.h
  include/util/avl_tree_balancer.h
)

#set(futil_UTIL_SOURCES src/scoped_profiler.cpp)

#add_executable(trie_test test/main.cpp test/test.cpp ${futil_UTIL_HEADERS} ${futil_UTIL_SOURCES})
#target_link_libraries(trie_test ${Boost_LIBRARIES})

add_executable(shardmap_test test/main.cpp test/shardmap.cpp ${futil_UTIL_HEADERS} ${futil_UTIL_SOURCES})
target_link_libraries(shardmap_test ${Boost_LIBRARIES})

add_executable(avl_tree_test test/main.cpp test/avl_tree.cpp ${futil_UTIL_HEADERS} ${futil_UTIL_SOURCES})
target_link_libraries(avl_tree_test ${Boost_LIBRARIES})

#add_library(futil STATIC
#    ${futil_UTIL_SOURCES}
#    ${futil_UTIL_HEADERS}
# )
#install(FILES ${futil_UTIL_HEADERS} DESTINATION include/futil/util)
#install (TARGETS futil DESTINATION lib)
