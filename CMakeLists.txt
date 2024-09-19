cmake_minimum_required(VERSION 3.10)

# Check if the platform is Apple (macOS)
if(APPLE)
    # Set the compiler to use clang/clang++ from the specified path
    set(CMAKE_C_COMPILER /opt/homebrew/opt/llvm/bin/clang)
    set(CMAKE_CXX_COMPILER /opt/homebrew/opt/llvm/bin/clang++)
endif()

project(image-viewer)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include third-party libraries and source directory
add_subdirectory(third-party)
add_subdirectory(src)
