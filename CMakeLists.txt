
cmake_minimum_required(VERSION 3.10)


project(Chaos VERSION 0.95)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find SFML package
# This requires you to have SFML installed in a standard location
# or to have the SFML_DIR environment variable set.
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Gather all source files from the src directory
aux_source_directory(src SRC_FILES)

# Add the executable target
add_executable(Chaos ${SRC_FILES})

# Enable warnings for better code quality
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Chaos PRIVATE -Wall -Wextra)
endif()

# Link SFML libraries to the executable
target_link_libraries(Chaos PRIVATE sfml-graphics sfml-window sfml-system)

# Set output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Add an install rule (optional, but good practice)
install(TARGETS Chaos DESTINATION bin) 