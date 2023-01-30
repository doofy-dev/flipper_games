project('app')
cmake_minimum_required(VERSION 3.16)

include_directories(firmware/unleashed/furi/)
include_directories(firmware/unleashed/applications/services/)
include_directories(firmware/unleashed/lib/)
include_directories(firmware/unleashed/build/f7-firmware-D/assets/compiled/)
include_directories(firmware/unleashed/lib/flipper_application/elf/)
include_directories(firmware/unleashed/furi/core/)
include_directories(firmware/unleashed/firmware/targets/f7/furi_hal)
include_directories(faker)
include_directories(.)

FILE(GLOB SOURCES blackjack/*.c
        solitaire/*.c solitaire/*.h
        synth/*.c synth/*.h
        jumper/*.c jumper/*.h
        common/*.c common/*.h
        common/engine/*.c common/engine/*.h
        )

ADD_EXECUTABLE(app ${SOURCES} ${EXTRA})