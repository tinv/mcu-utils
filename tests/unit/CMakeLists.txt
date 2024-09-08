
cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_C_FLAGS -m32)
set(CMAKE_CXX_FLAGS -m32)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_CXX_COMPILER /usr/bin/g++ CACHE PATH "" FORCE)
set(BOARD native_posix)
set(ZEPHYR_TOOLCHAIN_VARIANT zephyr)
set(TOOLCHAIN_ROOT ${ZEPHYR_BASE})
set(ZEPHYR_EXTRA_MODULES $ENV{ZEPHYR_BASE}/../mcu-utils/modules/googletest)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(mcu-utils-unit-tests)

# Necessary for google test
include_directories(AFTER SYSTEM /usr/include/c++/12 /usr/include/x86_64-linux-gnu/c++/12)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH PARENT_DIR)
cmake_path(GET PARENT_DIR PARENT_PATH PRJ_DIR)

# Add test sources
set(SOURCES
		${PRJ_DIR}/lib/codec/src/cobs.c
		${PRJ_DIR}/lib/utils/src/hsm.c
		main.cpp
        codec/CobsTests.cpp
		utils/HsmTests.cpp
)

include_directories(
		${PRJ_DIR}/include
		${CMAKE_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}/src)

# Add these sources to your target
target_sources(app PRIVATE ${SOURCES})

# Link libraries
target_link_libraries(app PUBLIC gmock gtest)

add_custom_target(all_tests ALL)
