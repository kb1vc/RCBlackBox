cmake_minimum_required (VERSION 2.8.11)

project(BlackBox)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")

find_package(pigpio REQUIRED)

add_library(gyro FXAS21002C.cxx FXBase.cxx FXOS8700CQ.cxx PI_IO.cxx)

target_include_directories(gyro PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_link_libraries(gyro LINK_PUBLIC ${pigpio_LIBRARY})

add_executable(FG FXAS_Test.cxx)


target_include_directories(FG PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${pigpio_INCLUDE_DIRS})
target_link_libraries(FG LINK_PUBLIC gyro ${pigpio_LIBRARY})

add_executable(FC FXOS_Test.cxx)

target_include_directories(FC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${pigpio_INCLUDE_DIRS})
target_link_libraries(FC LINK_PUBLIC gyro ${pigpio_LIBRARY})


add_executable(SW SW_Test.cxx)

target_include_directories(SW PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${pigpio_INCLUDE_DIRS})
target_link_libraries(SW LINK_PUBLIC gyro ${pigpio_LIBRARY})
