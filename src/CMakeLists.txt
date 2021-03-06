cmake_minimum_required (VERSION 2.8.11)

project(BlackBox)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")

find_package(pigpio REQUIRED)
find_package(Boost 1.45 REQUIRED COMPONENTS program_options QUIET)


set(PIFDR_LIB_SRCS
FXAS21002C.cxx 
FXOS8700CQ.cxx 
PIIORaw.cxx
PIIOD.cxx
Lamp.cxx
Switch.cxx
Video.cxx
FDR.cxx)  


add_library(pifdr_lib ${PIFDR_LIB_SRCS})

target_include_directories(pifdr_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_executable(FXAS_Test FXAS_Test.cxx)

target_include_directories(FXAS_Test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${pigpio_INCLUDE_DIRS})
target_link_libraries(FXAS_Test LINK_PUBLIC pifdr_lib ${pigpio_LIBRARY} ${pigpiod_if2_LIBRARY})

add_executable(FXOS_Test FXOS_Test.cxx)

target_include_directories(FXOS_Test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${pigpio_INCLUDE_DIRS})
target_link_libraries(FXOS_Test LINK_PUBLIC pifdr_lib ${pigpio_LIBRARY} ${pigpiod_if2_LIBRARY})


add_executable(SW SW_Test.cxx)

target_include_directories(SW PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${pigpio_INCLUDE_DIRS})
target_link_libraries(SW LINK_PUBLIC pifdr_lib ${pigpio_LIBRARY} ${pigpiod_if2_LIBRARY})

add_executable(Shutdown_Test Shutdown_Test.cxx)

add_executable(Util_Test Util_Test.cxx Utils.cxx)

add_executable(FDR_Main FDR_Main.cxx)
target_include_directories(FDR_Main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${pigpio_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(FDR_Main LINK_PUBLIC pifdr_lib ${pigpio_LIBRARY} ${pigpiod_if2_LIBRARY} ${Boost_LIBRARIES})

add_executable(FXOS_Exp FXOS_Exp.cxx)
target_include_directories(FXOS_Exp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${pigpio_INCLUDE_DIRS})
target_link_libraries(FXOS_Exp LINK_PUBLIC pifdr_lib ${pigpio_LIBRARY} ${pigpiod_if2_LIBRARY})
