cmake_minimum_required(VERSION 3.0)

project(ExtendedEssay VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall")

set(EE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(EE_SRC "${EE_ROOT}/src")
set(EE_BIN "${EE_ROOT}/bin")
set(EE_DEPS "${EE_ROOT}/deps")

set(EXECUTABLE_OUTPUT_PATH "${EE_BIN}")

# sources
include_directories("${EE_ROOT}/include")
set(sources "Main.cpp" "Sim.cpp" "Launch.cpp" "Utils.cpp")

list(TRANSFORM sources PREPEND "${EE_SRC}/")

# build
add_executable(ExtendedEssay ${sources})

target_include_directories(ExtendedEssay PRIVATE "${EE_DEPS}/cspice/include")
target_link_libraries(ExtendedEssay pthread "${EE_DEPS}/cspice/lib/cspice.a")