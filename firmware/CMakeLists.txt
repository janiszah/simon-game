cmake_minimum_required(VERSION 3.13)

list(APPEND CMAKE_MODULE_PATH 
  ${CMAKE_SOURCE_DIR}/cmake
  ${CMAKE_SOURCE_DIR}/3rd_party)


set(ENV{ARMGCC_DIR} "/usr/local")
set(CMAKE_TOOLCHAIN_FILE "cmake/armgcc.cmake")

# ENABLE ASM
ENABLE_LANGUAGE(ASM)

# Start Project
project(SimonGame)

include(flags)

# Add subdirectories
add_subdirectory(src)