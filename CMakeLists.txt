cmake_minimum_required(VERSION 3.20.0)

set(BOARD rpi_pico)

find_package(Zephyr)
project(morpheus_app)

target_sources(app PRIVATE src/main.cpp src/generated.cpp src/protocol.cpp src/morpheus-states.cpp)