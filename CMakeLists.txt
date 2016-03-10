cmake_minimum_required(VERSION 2.8)
project(SafeSysUpdater)
enable_language(ASM)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(APP_ICON res/icon.png)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DKA_SUGGESTED_C_FLAGS}") # Use the devkitArm suggested flags. This is not mandatory.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")

# Note that you must copy the cmake folder and the DevkitArm3DS.cmake file in this directory
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake) # Add the cmake folder to the modules paths, so that we can use the tools and find_package for ctrulib
include(Tools3DS) # Include all the macros and tools needed for 3ds development.

find_package(CTRULIB REQUIRED) # Look for ctrulib

file(GLOB_RECURSE SOURCE_FILES
    source/*
)
file(GLOB FILTER_FILES
    source/SuperUserLib3DS/demo/source/*
    source/svchax/test.c
)
list(REMOVE_ITEM SOURCE_FILES
    ${FILTER_FILES}
)

add_executable(SafeSysUpdater ${SOURCE_FILES}) # Create the elf file

target_link_libraries(SafeSysUpdater ${LIBCTRU_LIBRARIES}) # Link ctrulib

target_include_directories(SafeSysUpdater PUBLIC ${LIBCTRU_INCLUDE_DIRS}) # Add the ctrulib include folder
target_include_directories(SafeSysUpdater PUBLIC "/opt/devkitpro/portlibs/armv6k/include")
target_include_directories(SafeSysUpdater PUBLIC source/sysUpdater/include)
target_include_directories(SafeSysUpdater PUBLIC source/sysUpdater/include/zip)

add_3dsx_target(SafeSysUpdater) # Generate the .3dsx and .smdh from the CtrManager target
add_netload_target(SafeSysUpdater_3dslink SafeSysUpdater)
add_cia_target(SafeSysUpdater res/app.rsf res/banner.png res/audio.wav)
add_citra_target(SafeSysUpdater_Citra SafeSysUpdater)
set_target_properties(SafeSysUpdater PROPERTIES COMPILE_FLAGS "-DCITRA")
