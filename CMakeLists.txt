#----------------------------------------------------
# MatheoCrawler
# A small SFML 3.0 C++20 game

# Requirements:
#   A C++20 capable compiler
#   Git installed to fetch SFML via CMAKE FetchContent
#   If you are using linux, SFML requires a few other depedencies that u must install.
#   Please head to:
#       a) https://www.sfml-dev.org/tutorials/3.0/getting-started/build-from-source/#installing-dependencies
#       b) https://github.com/SFML/cmake-sfml-project?tab=readme-ov-file#command-line
#   For more information.

# How to build:
#   Navigate to the project root by command: 'cd /path/to/MatheoCrawler'
#   Create build directory: 'mkdir build'
#   Configure the build with: 'cmake -S . -B build'
#   Then build either for Debug or Release version:
#       cmake --build build --config Debug 
#       cmake --build build --config Release

# Assets folder will be automatically copied as well.
#----------------------------------------------------



cmake_minimum_required(VERSION 3.15...4.0)

project(MatheoCrawler VERSION 1.0
                      DESCRIPTION "MatheoCrawler"
                      LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#Required, otherwise there is an error saying
#'Compatibility with CMAKE < 3.5 has been removed from CMake'
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

#Fetch SFML from github
#It's from their official link: https://github.com/SFML/cmake-sfml-project/blob/master/CMakeLists.txt
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

#Collect .cpp files
file(GLOB SOURCES src/*.cpp)

add_executable(MatheoCrawler ${SOURCES})
target_compile_features(MatheoCrawler PRIVATE cxx_std_20)
target_include_directories(MatheoCrawler PRIVATE include)
target_link_libraries(MatheoCrawler PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System)
target_precompile_headers(MatheoCrawler PRIVATE include/pch.h)

#--------------------------------------------------------------------
# Hide the console window in visual studio projects - Release
#--------------------------------------------------------------------
if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

#Copy assets section
set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)
set(DEST_DIR $<TARGET_FILE_DIR:MatheoCrawler>)

add_custom_command(
    TARGET MatheoCrawler
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ARGS ${ASSETS_DIR} ${DEST_DIR}/assets
    COMMENT "Copying assets to $<CONFIG> directory.."
)