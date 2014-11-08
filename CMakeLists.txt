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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wc++0x-compat -Wall -Wextra -pipe -fno-rtti -fPIC -fvisibility=hidden")

set(futil_COM_HEADERS
  include/com/hresult.h
  include/com/iunknown.h
  include/com/basecomponent.h
  include/com/ifactory.h
  include/com/componentfactory.h
)

set(futil_UTIL_HEADERS
  include/util/visibility.h
  include/util/bitutil.h
  include/util/singletone.h
  include/util/refobject.h
  include/util/patricia.h
  include/util/rlucache.h
)

set(futil_SOURCES
  src/com/iunknown.cpp
  src/com/ifactory.cpp
  src/com/basecomponent.cpp
  src/com/componentfactory.cpp
)

add_library(futil STATIC 
  ${futil_SOURCES} 
  ${futil_UTIL_HEADERS}
  ${futil_COM_HEADERS}
 )

include_directories(include)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

install(FILES ${futil_COM_HEADERS} DESTINATION include/futil/com)
install(FILES ${futil_UTIL_HEADERS} DESTINATION include/futil/util)
install (TARGETS futil DESTINATION lib)
