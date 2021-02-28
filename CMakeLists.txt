cmake_minimum_required(VERSION 3.0)

project(ExtendedEssay VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall")

set(EE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(EE_SRC "${EE_ROOT}/src")
set(EE_BIN "${EE_ROOT}/bin")
set(EE_DEPS "${EE_ROOT}/deps")
set(EE_ASSETS "${EE_ROOT}/assets")

set(EXECUTABLE_OUTPUT_PATH "${EE_BIN}")

# sources
set(sources
    "Main.cpp"
    "Utils.cpp"
    "camera/Camera.cpp"
    "camera/CameraController.cpp"
    "mesh/Planet.cpp"
    "render/Model.cpp"
    "render/Shader.cpp"
    "window/Window.cpp")

list(TRANSFORM sources PREPEND "${EE_SRC}/")

# dependencies
add_subdirectory("${EE_DEPS}/glm")
add_subdirectory("${EE_DEPS}/glfw")
add_subdirectory("${EE_DEPS}/glad")

# build
add_executable(ExtendedEssay ${sources})

target_include_directories(ExtendedEssay PRIVATE "${EE_ROOT}/include")
target_include_directories(ExtendedEssay PRIVATE "${EE_DEPS}/cspice/include" "${EE_DEPS}/glm/glm" "${EE_DEPS}/glfw/include" "${EE_DEPS}/glad/include")
target_link_libraries(ExtendedEssay pthread "${EE_DEPS}/cspice/lib/cspice.a" glfw glad)

# copy assets
add_custom_target(CopyAssets
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${EE_ASSETS} ${EE_BIN}/assets
  COMMENT "Copying assets"
)
add_dependencies(ExtendedEssay CopyAssets)