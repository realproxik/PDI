cmake_minimum_required(VERSION 3.10)
project(pdi C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_VISIBILITY_PRESET hidden)

include_directories(include)

# --- core static/shared lib (the host-side registry) ---
add_library(pdi_core STATIC src/core/registry.c)
target_link_libraries(pdi_core PUBLIC dl)

# --- example plugin, built as a loadable shared object ---
add_library(example_plugin SHARED src/plugins/example_plugin.c)
set_target_properties(example_plugin PROPERTIES PREFIX "lib")

# --- host test binary ---
add_executable(pdi_host tests/host_main.c)
target_link_libraries(pdi_host PRIVATE pdi_core)
