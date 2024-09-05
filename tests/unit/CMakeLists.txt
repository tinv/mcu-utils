
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

include_directories(AFTER SYSTEM /usr/include/c++/12 /usr/include/x86_64-linux-gnu/c++/12)

# Add test sources
set(SOURCES
		../../lib/codec/src/cobs.c
		main.cpp
        codec/CobsTests.cpp
)

# Add these sources to your target
target_sources(app PRIVATE ${SOURCES})

# Link libraries
target_link_libraries(app PUBLIC gmock gtest)



# zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(
		${CMAKE_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}/src)

add_custom_target(all_tests ALL)

# Mocks

list(APPEND SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/mocks/device_mock.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/mocks/gpio_mock.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/mocks/uptime_mock.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/mocks/hsm_mock.cpp)

# Test lib

set(LIB_TEST_NAME ${APP_CORE_LIB}-test)
set(TINV_TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/codec/CobsTests.cpp
	PARENT_SCOPE)

# add_library(${LIB_TEST_NAME} STATIC ${LIB_TEST_SOURCES})


# Various tests

function(ADD_MU_TEST TEST_NAME TEST_SOURCES)
	add_executable(${TEST_NAME} ${TEST_SOURCES})
	target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	target_link_libraries(${TEST_NAME}
		${LIB_TEST_NAME})
	add_test(NAME ${TEST_NAME} COMMAND
		$<TARGET_FILE:${TEST_NAME}> --gtest_output=xml:${PROJECT_BINARY_DIR}/${TEST_NAME}.xml)
	add_dependencies(all_tests ${TEST_NAME})
endfunction()
	
# ADD_MU_TEST(cobs cobs/CobsTests.cpp)
# ADD_MU_TEST(hsm hsm/HsmTests.cpp)
# ADD_MU_TEST(comm comm/CommTests.cpp)

#if(${RUN_UNIT_TESTS})
#    add_custom_command(TARGET all_tests
#            COMMAND echo "Running tests" && ${CMAKE_CTEST_COMMAND} --output-on-failures
#            POST_BUILD)
#endif()


