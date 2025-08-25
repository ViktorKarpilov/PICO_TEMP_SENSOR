add_library(pico_lwip_mqtt INTERFACE)
target_sources(pico_lwip_mqtt INTERFACE
        ${PICO_LWIP_PATH}/src/apps/mqtt/mqtt.c
)
target_compile_definitions(pico_lwip_mqtt INTERFACE
        LWIP_MQTT=1
)