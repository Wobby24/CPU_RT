cmake_minimum_required(VERSION 3.16)
project(CPURaytracer VERSION 1.0 LANGUAGES CXX C)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Output binary location
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Project directories (based on current repo layout)
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")  # keep forward slashes and keep on one line
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/inc")
set(EXTERNAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ext")

# List Source files
set(PROJECT_SOURCES
    "${SRC_DIR}/main.cpp"
)

# Executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Post-build copy: only valid after the target exists
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
 #   COMMAND ${CMAKE_COMMAND} -E copy_directory
  #  "${CMAKE_SOURCE_DIR}/rt_output"
   # "$<TARGET_FILE_DIR:${PROJECT_NAME}>/rt_output"
#)

# Includes
target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${INCLUDE_DIR}"
        "${EXTERNAL_INCLUDE_DIR}"
)

# Debug macro
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG_BUILD>
)
