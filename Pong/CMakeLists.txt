cmake_minimum_required(VERSION 3.15)
project(PongGame)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2 packages
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Add source files
set(SOURCES
    SRC/Main.cpp
    SRC/game.cpp
    SRC/game_logic.cpp
    SRC/game_loops.cpp
    SRC/init.cpp
    SRC/input.cpp
    SRC/config.cpp
    SRC/rendering.cpp
    SRC/network.cpp
    SRC/network_manager.cpp
    SRC/texture_manager.cpp
)

# Add header files
set(HEADERS
    HEADER/config.h
    HEADER/constants.h
    HEADER/game.h
    HEADER/game_logic.h
    HEADER/game_loops.h
    HEADER/init.h
    HEADER/input.h
    HEADER/network.h
    HEADER/network_manager.h
    HEADER/rendering.h
    HEADER/texture_manager.h
)

# Create executable
add_executable(PongGame ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(PongGame PRIVATE HEADER)

# Link libraries
target_link_libraries(PongGame PRIVATE 
    SDL2::SDL2 
    SDL2_ttf::SDL2_ttf
    ws2_32
)

# Copy font file to build directory (works correctly for multi-config generators)
add_custom_command(TARGET PongGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/FONT/ARCADE_I.TTF $<TARGET_FILE_DIR:PongGame>
)

# Set output directory
set_target_properties(PongGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
