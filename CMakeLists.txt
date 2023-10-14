cmake_minimum_required(VERSION 3.1...3.27)

project(
    SeverinEngine
    VERSION 1.0
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(RUNTIME_INCLUDE runtime/include/)
set(RUNTIME_SRC runtime/src/)
set(RUNTIME_SHADERS runtime/shaders/)
set(RUNTIME_TESTS_SRC tests/)

add_subdirectory(deps/entt)
set(ENTT_INCLUDE_DIR deps/entt/src/)
add_subdirectory(deps/fmt)
set(FMT_INCLUDE_DIR deps/fmt/include/)
set(FMT_LIB fmt)
add_subdirectory(deps/SDL/)
set(SDL_INCLUDE_DIR deps/SDL/include/)
set(SDL_LIB SDL3::SDL3)
add_subdirectory(deps/metal-cpp/)
set(METAL_INCLUDE_DIR deps/metal-cpp/)
set(METAL_LIB MetalCPP "-framework Metal" "-framework QuartzCore" "-framework Foundation")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
include(metal)

file(GLOB RUNTIME_SRC_FILES ${RUNTIME_SRC}/*)
add_compiled_metal_sources(RUNTIME_SRC_FILES ${RUNTIME_SHADERS}/triangle.metal)

add_library(SeverinEngineRuntime ${RUNTIME_SRC_FILES})
target_compile_options(SeverinEngineRuntime PRIVATE "-Wall -Wextra -Werror")
target_include_directories(SeverinEngineRuntime
    PUBLIC
        ${RUNTIME_INCLUDE}
        ${ENTT_INCLUDE_DIR}
        ${FMT_INCLUDE_DIR}
        ${METAL_INCLUDE_DIR}
        ${SDL_INCLUDE_DIR}
        ${RUNTIME_SHADERS}
        ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(SeverinEngineRuntime
    PUBLIC
        ${FMT_LIB}
        ${METAL_LIB}
        ${SDL_LIB})

target_compile_definitions(SeverinEngineRuntime
    PUBLIC
        $<$<CONFIG:Debug>:DEBUG>
)

file(GLOB RUNTIME_TESTS_SRC_FILES ${RUNTIME_TESTS_SRC}/*)
add_executable(SeverinEngineRuntimeTests ${RUNTIME_TESTS_SRC_FILES})
target_include_directories(SeverinEngineRuntimeTests PRIVATE ${RUNTIME_INCLUDE})
target_link_libraries(SeverinEngineRuntimeTests PRIVATE SeverinEngineRuntime)
