project(flipper-games)
cmake_minimum_required(VERSION 3.16)

set(FIRMWARE_PATH ../flipperzero-firmware)

include_directories(${FIRMWARE_PATH}/furi/)
include_directories(${FIRMWARE_PATH}/applications/services/)
include_directories(${FIRMWARE_PATH}/lib/)
include_directories(${FIRMWARE_PATH}/build/f7-firmware-D/assets/compiled/)
include_directories(${FIRMWARE_PATH}/lib/flipper_application/elf/)
include_directories(${FIRMWARE_PATH}/furi/core/)
include_directories(${FIRMWARE_PATH}/firmware/targets/f7/furi_hal)

FILE(GLOB SOURCES *.c *.h */*.c */*.h)

ADD_EXECUTABLE(flipper-games ${SOURCES} ${EXTRA})