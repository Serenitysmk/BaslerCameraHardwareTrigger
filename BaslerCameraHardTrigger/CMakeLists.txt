cmake_minimum_required(VERSION 2.8)

project(BaslerCameraHardTrigger)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


#OpenCV (here we need is OpenCV3)
Message("Checking for OpenCV3 lib :")
#in case you did not install opencv in default directory, please specify exact location
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    Message("OpenCV has Found")
    message("OpenCV Version " ${OpenCV_VERSION})
    include_directories(${OpenCV_INCLUDE_DIRS})
else(OpenCV_FOUND)
    Message(FATAL_ERROR "OpenCV did not found")
endif(OpenCV_FOUND)


# Use Basler Pylon API for Basler camera
# The Basler Pylon API must be installed on your system
# The Path to Pylon should be :
# Include directory should be : /path/to/pylon5/include
# Libraries : /path/to/pylon5/lib64
MESSAGE("Trying to find Pylon API for Basler Cameras")
OPTION(USE_BASLER "If we use a Basler camera , we should find the Pylon API" ON)
#Pylon Libraries:A Basler Camera
if(USE_BASLER)
    set(BASLER_PYLON_INCLUDE_PATH /data/mshe/software/pylon5/include)
    set(BASLER_PYLON_LIBRARY -L/data/mshe/software/pylon5/lib64 -Wl,-E -lpylonbase -lpylonutility -lGenApi_gcc_v3_1_Basler_pylon_v5_1 -lGCBase_gcc_v3_1_Basler_pylon_v5_1)
    INCLUDE_DIRECTORIES(${BASLER_PYLON_INCLUDE_PATH})
endif(USE_BASLER)


#Basler camera grabber library
add_library(CameraGrabberBasler "CameraGrabberBasler.h" "CameraGrabberBasler.cpp")

add_executable(${PROJECT_NAME} "ImageGrabController.cpp" )
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} CameraGrabberBasler ${BASLER_PYLON_LIBRARY})
