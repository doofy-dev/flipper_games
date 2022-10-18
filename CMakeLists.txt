project('app')
cmake_minimum_required(VERSION 3.16)

include_directories(../unleashed/furi/)
include_directories(../unleashed/applications/services/)
include_directories(../unleashed/lib/)
include_directories(../unleashed/build/f7-firmware-D/assets/compiled/)
include_directories(../unleashed/lib/flipper_application/elf/)
include_directories(../unleashed/furi/core/)
include_directories(.)

FILE(GLOB SOURCES blackjack/*.c)

ADD_EXECUTABLE(app ${SOURCES} ${EXTRA})