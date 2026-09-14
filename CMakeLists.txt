cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME STM32_DOG)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        # 这里不再直接添加源文件，改用 target_sources 添加（更清晰）
)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined library search paths
)

# ======================================================
# 添加用户源文件
# ======================================================
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        # 添加您的 Hardware 文件夹中的源文件
        Hardware/DogActions.c
        Hardware/MyIIC.c
        Hardware/OLED.c
        Hardware/MPU6050.c
        Hardware/Task/ActionTask.c
        Hardware/Task/CommandTask.c
        Hardware/Task/OLEDTask.c
        Hardware/Task/SensorTask.c
        # 如果有其他 .c 文件，也在这里添加
        # 例如：Hardware/OtherModule.c
)

# ======================================================
# 添加用户头文件路径
# ======================================================
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        # 添加 Hardware 文件夹，以便 #include "DogActions.h" 能找到
        Hardware
        Hardware/Task
        Hardware/Types
        # 如果还有其它自定义头文件目录，也加上
        # 例如：Hardware/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
        stm32cubemx

        # Add user defined libraries
)