cmake_minimum_required (VERSION 2.8.11)

project(BlackBox)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")

find_package(pigpio REQUIRED)

set(PIFDR_LIB_SRCS
FXAS21002C.cxx 
FXOS8700CQ.cxx 
PIIORaw.cxx
PIIOD.cxx
Lamp.cxx
Switch.cxx)  


add_library(pifdr_lib ${PIFDR_LIB_SRCS})

target_include_directories(pifdr_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_executable(FG FXAS_Test.cxx)

target_include_directories(FG PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${pigpio_INCLUDE_DIRS})
target_link_libraries(FG LINK_PUBLIC pifdr_lib ${pigpio_LIBRARY} ${pigpiod_if2_LIBRARY})

add_executable(FC FXOS_Test.cxx)

target_include_directories(FC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${pigpio_INCLUDE_DIRS})
target_link_libraries(FC LINK_PUBLIC pifdr_lib ${pigpio_LIBRARY} ${pigpiod_if2_LIBRARY})


add_executable(SW SW_Test.cxx)

target_include_directories(SW PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${pigpio_INCLUDE_DIRS})
target_link_libraries(SW LINK_PUBLIC pifdr_lib ${pigpio_LIBRARY} ${pigpiod_if2_LIBRARY})

add_executable(Shutdown_Test Shutdown_Test.cxx)

add_executable(Util_Test Util_Test.cxx Utils.cxx)
