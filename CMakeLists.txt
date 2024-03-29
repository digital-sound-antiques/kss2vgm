cmake_minimum_required(VERSION 3.0)
project(kss2vgm)

if(MSVC)
  set(CMAKE_C_FLAGS "/Ox /W3 /wd4996")
  set(configurations
    CMAKE_C_FLAGS_DEBUG
    CMAKE_C_FLAGS_MINSIZEREL
    CMAKE_C_FLAGS_RELEASE
    CMAKE_C_FLAGS_RELWITHDEBINFO
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_MINSIZEREL
    CMAKE_CXX_FLAGS_RELEASE
    CMAKE_CXX_FLAGS_RELWITHDEBINFO
  )
  foreach(flag_var ${configurations})
    if(${flag_var} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endif(${flag_var} MATCHES "/MD")
  endforeach()
else()
  set(CMAKE_C_FLAGS "-O3 -Wall")
endif(MSVC)

add_subdirectory(modules/libkss) 

include_directories(modules/libkss/src)
include_directories(modules/libkss/modules)

add_executable(kss2vgm src/main.c)

if(NOT CMAKE_COMPILER_IS_GNUCC)
	target_link_libraries(kss2vgm kss)
else()
	target_link_libraries(kss2vgm kss m)
endif()
