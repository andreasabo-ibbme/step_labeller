# Generated from StepLabeller.pro.

cmake_minimum_required(VERSION 3.16)
project(StepLabeller VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Try to used passed in OPENCV_DIR, or ENV variable if not passed in
if(NOT DEFINED OPENCV_DIR)
    SET(OPENCV_DIR $ENV{OPENCV_DIR})
    if(NOT DEFINED OPENCV_DIR)
        message(FATAL_ERROR "OPENCV_DIR not found!
        Set the OPENCV_DIR environment variable or pass in to cmake with -DOPENCV_DIR={path_to_opencv_root_build_dir}")
     endif()
endif(NOT DEFINED OPENCV_DIR)

if(NOT DEFINED INSTALL_EXAMPLEDIR)
    set(INSTALL_EXAMPLEDIR "bin")
endif()


include(CMakePrintHelpers)
cmake_print_variables(INSTALL_EXAMPLEDIR)
cmake_print_variables(OPENCV_DIR)
cmake_print_variables(CMAKE_MODULE_PATH)
set(OpenCV_STATIC OFF)
find_package(OpenCV REQUIRED PATHS ${OPENCV_DIR} NO_DEFAULT_PATH)

# Project related
include_directories(include)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)
file(GLOB_RECURSE UI_FILES ui/*.ui)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES include/*.h)

find_package(QT NAMES Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Multimedia Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (UI_HEADERS)
  qt_wrap_ui(UI_HEADERS ${UI_FILES})
endif()

add_executable(${CMAKE_PROJECT_NAME}
  ${UI_FILES}
  ${SOURCE_FILES}
  ${HEADER_FILES}
)

include_directories( ${OpenCV_INCLUDE_DIRS} )
link_directories( ${OpenCV_LIB_DIR} )

cmake_print_variables(OpenCV_LIBS)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Widgets
    ${OpenCV_LIBS}
)


install(TARGETS StepLabeller
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
