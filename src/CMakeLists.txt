file(GLOB_RECURSE UI_FILES *.ui)
file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)

find_package(QT NAMES Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Multimedia Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# cmake_print_variables(UI_HEADERS)
# cmake_print_variables(SOURCE_FILES)
# cmake_print_variables(HEADER_FILES)
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
