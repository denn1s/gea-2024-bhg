cmake_minimum_required(VERSION 3.29)
project(game)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(
  game
  ${SOURCE_FILES}
)

find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
set(BOX2D_INCLUDE_DIR "/usr/include/box2d")
set(BOX2D_LIBRARY "/usr/lib/libbox2d.so")
set(FMOD_INCLUDE_DIR "/usr/include/fmod")
set(FMOD_LIBRARY "/usr/lib/libfmod.so")

target_include_directories(game  
  PUBLIC src
  PUBLIC ${SDL2_INCLUDE_DIRS}
  PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS}
  PUBLIC ${BOX2D_INCLUDE_DIRS}
  PUBLIC ${FMOD_INCLUDE_DIRS}
)

target_link_libraries(game
  ${SDL2_LIBRARIES}
  ${SDL2_IMAGE_LIBRARIES}
  ${BOX2D_LIBRARY}
  ${FMOD_LIBRARY}
)
