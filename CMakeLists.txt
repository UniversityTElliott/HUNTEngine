cmake_minimum_required(VERSION 3.24)
project(HUNTEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# SDL3
# We want to build SDL3 as a shared library (DLL), and we don't need the static library, tests, or examples.
set(SDL_SHARED ON CACHE BOOL "Build shared SDL3" FORCE)
set(SDL_STATIC OFF CACHE BOOL "Do not build static SDL3" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "Do not build SDL3 test support" FORCE)
set(SDL_TESTS OFF CACHE BOOL "Do not build SDL3 tests" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "Do not build SDL3 examples" FORCE)

# FetchContent will download SDL3 from the specified Git repository and tag, and make it available for linking in our project.
# Give it a meaningful name, tell us where to find it and which version, and don’t get the full commit history
FetchContent_Declare(SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG release-3.4.18
	GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3)

#Custom Engine stuff below
#Librarys is basically anything that has a header file
add_library(Engine STATIC
    Engine/src/Engine.cpp
    Engine/src/Renderer.cpp
 "Engine/include/HUNT/Graphics/MeshData.h")

add_library(Game STATIC Game/src/Game.cpp "Engine/include/HUNT/Graphics/MeshData.h")

#Anything that has a main
add_executable(EditorApp EditorApp/src/HUNTEditor.cpp)
add_executable(HUNTGame GameApp/src/HUNTGame.cpp)


target_include_directories(Engine PUBLIC Engine/include)
target_include_directories(Game PUBLIC Game/include)

target_link_libraries(Engine PRIVATE SDL3::SDL3)
target_link_libraries(Game PUBLIC Engine)

target_link_libraries(HUNTGame PRIVATE Game)
target_link_libraries(EditorApp PRIVATE Game)



if(WIN32)
add_custom_target(StageRuntime
COMMAND ${CMAKE_COMMAND} -E copy_if_different
$<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:HUNTGame>
VERBATIM)
add_dependencies(HUNTGame StageRuntime)
add_dependencies(EditorApp StageRuntime)
endif()

#https://github.com/TechnicJelle/GPUForBeginners/blob/ab63d5d11df06466a67ec46a5e8cd51723629203/CMakeLists.txt#L35-L68 adapted from here


# Shader Compilation CLEAN UP A TAD


macro(COMPILE_SHADERS)
set(SHADERS_DIR
    "${CMAKE_SOURCE_DIR}/Engine/include/HUNT/Graphics/Shaders"
)

# Put shaders directly beside HUNTGame.exe
set(SHADERS_OUTPUT_DIR
    "$<TARGET_FILE_DIR:HUNTGame>/shaders"
)
#Find shadercross
find_program(SC_EXECUTABLE
    shadercross
    REQUIRED
    HINTS
        "${CMAKE_SOURCE_DIR}/Engine/Tools/ShaderCross"
)
#get all hlsl files
file(GLOB_RECURSE SHADER_FILES
    CONFIGURE_DEPENDS
    "${SHADERS_DIR}/*.hlsl"
)


set(COMPILED_SHADERS)

foreach(SHADER ${SHADER_FILES})

    get_filename_component(SHADER_NAME
        "${SHADER}"
        NAME
    )
    #Create all the final files 
    string(REPLACE ".hlsl" ".spv"
        SPV_NAME
        "${SHADER_NAME}"
    )

    string(REPLACE ".hlsl" ".msl"
        MSL_NAME
        "${SHADER_NAME}"
    )

    string(REPLACE ".hlsl" ".dxil"
        DXIL_NAME
        "${SHADER_NAME}"
    )

    set(SPV
        "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SPV_NAME}"
    )

    set(MSL
        "${CMAKE_CURRENT_BINARY_DIR}/shaders/${MSL_NAME}"
    )

    set(DXIL
        "${CMAKE_CURRENT_BINARY_DIR}/shaders/${DXIL_NAME}"
    )
    #Command to run shadercross on all the types
    add_custom_command(
        OUTPUT
            "${SPV}"
            "${MSL}"
            "${DXIL}"

        COMMAND
            "${CMAKE_COMMAND}" -E make_directory
            "${CMAKE_CURRENT_BINARY_DIR}/shaders"

        COMMAND
            "${SC_EXECUTABLE}"
            "${SHADER}"
            -o "${SPV}"

        COMMAND
            "${SC_EXECUTABLE}"
            "${SHADER}"
            -o "${MSL}"

        COMMAND
            "${SC_EXECUTABLE}"
            "${SHADER}"
            -o "${DXIL}"

        DEPENDS
            "${SHADER}"

        VERBATIM #This exists so cmake listens to me
    )
    #Add all the shaders to list called COMPILED_SHADERS
    list(APPEND COMPILED_SHADERS
        "${SPV}"
        "${MSL}"
        "${DXIL}"
    )

endforeach()

add_custom_target(
    HUNTEngine_shaders
    ALL
    DEPENDS
        ${COMPILED_SHADERS}
)

add_dependencies(
   HUNTGame
   HUNTEngine_shaders
)
endmacro()
COMPILE_SHADERS()



