
include_directories(${ModbusMQTT-GW_SOURCE_DIR}/main/inc)

add_executable(ModbusMQTT-GW main.c configure.c log.c my_alloc.c modbus_runner.c)
target_link_libraries( ${PROJECT_NAME} mxml pthread modbus)