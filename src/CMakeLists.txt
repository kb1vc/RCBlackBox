cmake_minimum_required (VERSION 2.8.11)

project(BlackBox)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")

find_package(pigpio REQUIRED)

add_library(gyro FXAS21002C.cxx FXBase.cxx PI_IO.cxx)

target_include_directories(gyro PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_link_libraries(gyro LINK_PUBLIC ${pigpio_LIBRARY})

add_executable(FT FXAS_Test.cxx)

target_include_directories(FT PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${pigpio_INCLUDE_DIRS})
target_link_libraries(FT LINK_PUBLIC gyro ${pigpio_LIBRARY})
