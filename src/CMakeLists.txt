file(GLOB_RECURSE UI_FILES *.ui)
file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Multimedia)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

qt_wrap_ui(UI_HEADERS ${UI_FILES})

add_executable(${CMAKE_PROJECT_NAME}
  ${UI_FILES}
  ${SOURCE_FILES}
  ${HEADER_FILES}
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${OPENCV_DIR}/install/include
)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Multimedia
    Qt::Widgets
    ${OPENCV_DIR}/bin/libopencv_world451.dll
)
