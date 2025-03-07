cmake_minimum_required(VERSION 3.20)

# Disable in-source build.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source build is not allowed! Please specify a build folder.\n\tex:cmake -B build")
endif()

#**********************
# Get path to XCore SDK
#**********************
set(XCORE_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}")
cmake_path(GET XCORE_SDK_PATH PARENT_PATH XCORE_SDK_PATH)
cmake_path(GET XCORE_SDK_PATH PARENT_PATH XCORE_SDK_PATH)
cmake_path(GET XCORE_SDK_PATH PARENT_PATH XCORE_SDK_PATH)

## This app only supports the XCORE-AI-EXPLORER board
set(BOARD XCORE-AI-EXPLORER)

## Import XMOS configurations
include("${XCORE_SDK_PATH}/tools/cmake_utils/xmos_platform.cmake")

## Define project
project(explorer_board)

## Enable languages for project
enable_language(CXX C ASM)

## Set XMOS application variables
set(APP_COMPILER_FLAGS
    "-Os"
    "-g"
    "-report"
    "-fxscope"
    "-mcmodel=large"
    "-Wno-xcore-fptrgroup"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/config.xscope"
    "${CMAKE_CURRENT_SOURCE_DIR}/${BOARD}.xn"
)

set(APP_SOURCES
    "src/main.c"
    "src/audio_pipeline/audio_pipeline.c"
    "src/platform/app_pll_ctrl.c"
    "src/platform/aic3204.c"
    "src/platform/mic_support.c"
    "src/platform/tile_support.c"
    "src/platform/platform_init_tile0.c"
    "src/platform/platform_init_tile1.c"
    "src/demos/spi_demo.c"
    "src/demos/flash_demo.c"
    "src/demos/gpio_server.c"
)

set(APP_INCLUDES
    "src"
    "src/audio_pipeline"
    "src/platform"
    "src/misc"
    "src/demos"
)

set(APP_COMPILE_DEFINITIONS
    "XCORE_UTILS_DEBUG_PRINTF_REMAP=1"
    "DEBUG_PRINT_ENABLE=1"
    "MIC_DUAL_ENABLED=1"
    "PLATFORM_USES_TILE_0=1"
    "PLATFORM_USES_TILE_1=1"
)

set(APP_LINK_OPTIONS
    "-lquadspi"
)


#**********************
# targets
#**********************
set(TARGET_NAME ${PROJECT_NAME})

add_executable(${TARGET_NAME})
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${TARGET_NAME}.xe)

target_sources(${TARGET_NAME} PRIVATE ${APP_SOURCES} ${XMOS_PLATFORM_SOURCES})
target_include_directories(${TARGET_NAME} PRIVATE ${APP_INCLUDES} ${XMOS_PLATFORM_INCLUDES})

target_compile_definitions(${TARGET_NAME} PRIVATE ${APP_COMPILE_DEFINITIONS})
target_compile_options(${TARGET_NAME} PRIVATE ${APP_COMPILER_FLAGS})
target_link_options(${TARGET_NAME} PRIVATE ${APP_COMPILER_FLAGS} ${APP_LINK_OPTIONS})

#**********************
# install
#**********************
set(INSTALL_DIR "${PROJECT_SOURCE_DIR}/bin")
install(TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_DIR})
