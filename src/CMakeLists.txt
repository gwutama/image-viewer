# Find OpenGL
find_package(OpenGL REQUIRED)

# Find wxWidgets, including the OpenGL (gl) component
find_package(wxWidgets REQUIRED COMPONENTS core base gl)

if(wxWidgets_USE_FILE) # not defined in CONFIG mode
    include(${wxWidgets_USE_FILE})
endif()

# Define the source files
set(SRC_FILES
        main.mm
        ImageCanvas.cpp
        ImageEditor.cpp
)

# Include directories
include_directories(${CMAKE_BINARY_DIR}) # for generated opencv_modules.hpp
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/calib3d/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/core/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/dnn/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/features2d/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/flann/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/highgui/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/imgcodecs/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/imgproc/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/ml/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/objdetect/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/photo/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/stitching/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/video/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/videoio/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv_contrib/modules/xphoto/include)

# wxWidgets include directories
include_directories(${wxWidgets_INCLUDE_DIRS})

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        opencv_core opencv_imgproc opencv_imgcodecs
        ${wxWidgets_LIBRARIES}
        OpenGL::GL    # Ensure OpenGL is linked
)
