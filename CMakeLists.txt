cmake_minimum_required(VERSION 3.10)

project(TankGame)

# Set Boost root directory
set(BOOST_ROOT "C:/Users/andre/CPP_TTT/boost_1_86_0")

# Specify Boost include and lib directories
include_directories("${BOOST_ROOT}")
link_directories("${BOOST_ROOT}/stage/lib")

# Specify C++20 standard
set(CMAKE_CXX_STANDARD 20)

# First executable
add_executable(gameserver "gameserver.cpp")
target_link_libraries(gameserver 
    PRIVATE 
    "${BOOST_ROOT}/stage/lib/libboost_system-vc143-mt-x64-1_86.lib"  # Explicit Boost system library
    ws2_32
)

# Second executable
add_executable(gameclient "gameclient.cpp")
target_link_libraries(gameclient 
    PRIVATE 
    "${BOOST_ROOT}/stage/lib/libboost_system-vc143-mt-x64-1_86.lib"  # Explicit Boost system library
    ws2_32
)
