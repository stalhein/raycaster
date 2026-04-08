cmake_policy(VERSION 3.21)
cmake_minimum_required(VERSION 3.16...3.28)
project(Raycaster VERSION 1.0 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get dependencies
include(FetchContent)

message(STATUS "Fetching SDL2...")
FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.30.0
)
set(SDL_PIPEWIRE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL2)


file(GLOB_RECURSE SRC_FILES "src/*.c")

add_executable(Raycaster ${SRC_FILES})

target_include_directories(Raycaster PRIVATE
  src
  include
)

target_link_libraries(Raycaster PRIVATE
  SDL2::SDL2
  SDL2::SDL2main
  m
)

if (WIN32)
  target_link_libraries(Raycaster PRIVATE OpenGL32)
endif()

add_custom_target(run
  COMMAND Raycaster
  DEPENDS Raycaster
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
