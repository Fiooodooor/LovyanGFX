# See how to install sdl2: https://wiki.libsdl.org/SDL2/Installation
cmake_minimum_required (VERSION 3.8)
project(LGFX_SDL)

add_definitions(-DLGFX_SDL)

# Specify the files to include in the build;
# If you change the relative position of LovyanGFX, adjust the relative path descriptions to match your environment;
file(GLOB Target_Files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS 
    *.cpp
    ../../../LovyanGFX/src/lgfx/Fonts/efont/*.c
    ../../../LovyanGFX/src/lgfx/Fonts/IPA/*.c
    ../../../LovyanGFX/src/lgfx/utility/*.c
    ../../../LovyanGFX/src/lgfx/v1/*.cpp
    ../../../LovyanGFX/src/lgfx/v1/misc/*.cpp
    ../../../LovyanGFX/src/lgfx/v1/panel/Panel_Device.cpp
    ../../../LovyanGFX/src/lgfx/v1/panel/Panel_FrameBufferBase.cpp
    ../../../LovyanGFX/src/lgfx/v1/platforms/sdl/*.cpp
    )
add_executable (LGFX_SDL ${Target_Files})

# Specify the path of the files to include;
# If you change the relative position of LovyanGFX, adjust the relative path descriptions to match your environment;
target_include_directories(LGFX_SDL PUBLIC "../../../LovyanGFX/src/")

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")

# For Linux environments;
# Set up SDL2_INCLUDE_DIRS and SDL2_LIBRARIES in advance;
    find_package(SDL2 REQUIRED SDL2)
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(LGFX_SDL -lpthread ${SDL2_LIBRARIES})

ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

# For Windows environments;
# Specify the include path within the directory where SDL2 is installed;
    target_include_directories(LGFX_SDL PUBLIC "C:/SDL2/include")

# Specify the path to the .lib files within the directory where SDL2 is installed;
    target_link_libraries(LGFX_SDL PUBLIC "C:/SDL2/lib/x64/SDL2.lib")


    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>")

ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")

# For MacOS (Darwin) environments;
    find_package(SDL2 REQUIRED COMPONENTS SDL2)
    target_link_libraries(LGFX_SDL PRIVATE SDL2::SDL2)

ENDIF ()

target_compile_features(LGFX_SDL PUBLIC cxx_std_17)
