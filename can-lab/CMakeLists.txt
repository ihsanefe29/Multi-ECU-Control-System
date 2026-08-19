cmake_minimum_required(VERSION 3.16)
project(QtCanBusService LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core SerialBus)

qt_add_executable(QtCanBusService
    src/main.cpp
    src/CanFrame.h
    src/CanChannelWorker.h
    src/CanChannelWorker.cpp
    src/CommunicationCore.h
    src/CommunicationCore.cpp
)

target_link_libraries(QtCanBusService PRIVATE
    Qt6::Core
    Qt6::SerialBus
)
