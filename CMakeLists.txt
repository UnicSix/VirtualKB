cmake_minimum_required(VERSION 3.30)

project(virtual_kb VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set( CMAKE_C_COMPILER gcc )
# set( CMAKE_CXX_COMPILER g++ )
set(CMAKE_CXX_FLAGS
  "-Wall -Wextra -O3 -std=c++20"
)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

string(ASCII 27 Esc)
set(ColourReset "${Esc}[0m")
set(Blue        "${Esc}[34m")
set(Magenta     "${Esc}[35m")

file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)
CPMAddPackage("gh:nlohmann/json#55f9368")

if( NOT WIN32)
  message(FATAL_ERROR "This project only supports Windows platform")
endif()

if(NOT MSVC)
  message(STATUS "Not MSVC")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")
else()
  message(STATUS "MSVC Version: %{MSVC_VERSION}")
endif()

find_library(USER32_LIB User32)
find_library(GDI32_LIB Gdi32)
find_package(fmt REQUIRED)
if(fmt_FOUND)
  message(STATUS "fmt found")
endif()
find_package(spdlog REQUIRED)
if (spdlog_FOUND)
  message( STATUS "spdlog Found")
  message(STATUS "Spdlog include dir : ${spdlog_DIR}")
else()
  message( STATUS "spdlog Not Found")
endif()

find_package(SDL3 REQUIRED SDL3-static)
find_package(SDL3_ttf REQUIRED SDL3_ttf-static)
find_package(SDL3_image REQUIRED SDL3_image-static)
message(STATUS "${Magenta}SDL3 Version: ${ColourReset}${SDL3_VERSION}")
message(STATUS "${Magenta}SDL3_ttf Version: ${ColourReset}${SDL3_ttf_VERSION}")
message(STATUS "${Magenta}SDL3_image Version: ${ColourReset}${SDL3_image_VERSION}")
message(STATUS "${Blue}SDL3 Dir: ${ColourReset}${SDL3_DIR}")
message(STATUS "${Blue}SDL3_ttf Dir: ${ColourReset}${SDL3_ttf_DIR}")
message(STATUS "${Blue}SDL3_image Dir: ${ColourReset}${SDL3_image_DIR}")

# find_package(nlohmann_json REQUIRED)
# if (nlohmann_json_FOUND)
#   message( STATUS "nlohmann json Found")
#   message( STATUS "nlohmann json include dir : ${nlohmann_json_DIR}")
# else()
#   message( STATUS "nlohmann json Not Found")
# endif()


# Set source directory
set( PROJECT_SRC src/*.cpp )

# Collect source files
file( GLOB_RECURSE PROJECT_SRC "src/*.cpp" )

# Add executable
add_executable(${PROJECT_NAME} ${PROJECT_SRC})

if(WIN32)
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
elseif(MINGW)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "-mwindows"
    )
endif()
endif()

# Add include directory
message(${PROJECT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME}
  PRIVATE ${PROJECT_SOURCE_DIR}
)

include_directories( "src/" )

target_link_libraries(${PROJECT_NAME}
  PRIVATE ${USER32_LIB}
  PRIVATE ${GDI32_LIB}
  PRIVATE SDL3::SDL3
  PRIVATE SDL3_ttf::SDL3_ttf
  PRIVATE SDL3_image::SDL3_image
  PRIVATE nlohmann_json::nlohmann_json
  PRIVATE spdlog
  PRIVATE fmt::fmt
)
# Install executable
install(TARGETS virtual_kb DESTINATION bin)

# Install entire texture directory
install(DIRECTORY "${PROJECT_SOURCE_DIR}/res/textures/"
        DESTINATION res/textures
)

# Pack necessary dlls
set( MINGW_BIN_DIR "C:/msys64/mingw64/bin" )
set( INSTALL_DLLS
    libgcc_s_seh-1.dll
    libwinpthread-1.dll
    libstdc++-6.dll
    libfmt-11.dll
    libpng16-16.dll
    zlib1.dll
    SDL3.dll
    SDL3_image.dll
)
foreach(DLL ${INSTALL_DLLS})
  install(FILES "${MINGW_BIN_DIR}/${DLL}" DESTINATION bin)
endforeach()

# NSIS settings
set(CPACK_GENERATOR "NSIS")
set(CPACK_NSIS_PACKAGE_NAME "VirtualKB")
set(CPACK_NSIS_MODIFY_PATH ON)

set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

include(CPack)

set(CPACK_PACKAGE_NAME "Virtual KB")
set(CPACK_PACKAGE_VENDOR "Unic Six")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Pepe reacts upon your keyboard input")
set(CPACK_PACKAGE_VERSION "1.0.1")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
