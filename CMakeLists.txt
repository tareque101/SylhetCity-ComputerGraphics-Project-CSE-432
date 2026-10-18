cmake_minimum_required(VERSION 3.14)
project(SylhetCity CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)   # lets CMake 4.x accept older dependency files

# --- GLFW + GLM are downloaded automatically on first configure ---------------
include(FetchContent)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4 GIT_SHALLOW TRUE)
FetchContent_Declare(glm  GIT_REPOSITORY https://github.com/g-truc/glm.git  GIT_TAG 1.0.1 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glfw glm)

add_executable(SylhetCity
  external/glad/src/glad.c
  src/main.cpp
  src/Batch.cpp
  src/Props.cpp
  src/RoadNet.cpp
  src/Vehicles.cpp
  src/Boats.cpp
  src/Walkers.cpp
  src/Landmarks.cpp
  src/Campus.cpp
  src/World.cpp)

target_include_directories(SylhetCity PRIVATE external/glad/include src)
target_link_libraries(SylhetCity PRIVATE glfw glm::glm)

if(MSVC)
  target_compile_options(SylhetCity PRIVATE /utf-8)
elseif(WIN32)
  target_link_options(SylhetCity PRIVATE -static -static-libgcc -static-libstdc++)
endif()

# copy shaders next to the exe so it runs from anywhere
add_custom_command(TARGET SylhetCity POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:SylhetCity>/shaders)
