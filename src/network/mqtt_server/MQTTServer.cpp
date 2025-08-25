//
// Created by PC on 8/25/2025.
//

#include "MQTTServer.h"

#include <cstring>
#include <format>

#include "src/config.h"
#include <lwip/apps/mqtt.h>

#include "pico/cyw43_arch.h"

static void mqtt_connection_cb(mqtt_client_t* client, void* arg, mqtt_connection_status_t status)
{
    const auto state = static_cast<MQTTServerState*>(arg);
    
    if (!client || !state) {
        printf("ERROR: NULL pointer in mqtt_connection_cb\n");
        return;
    }
    
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("MQTT connection accepted\n");
        state->connected = true;  // Make sure this field exists
    } else {
        printf("MQTT connection failed: %d\n", status);
        state->connected = false;
    }
}

MQTTServer::MQTTServer(const ip_addr_t* ip_addr)
{
    state = new MQTTServerState;

    mqtt_client = mqtt_client_new();

    memset(&state->client_info, 0, sizeof(state->client_info));
    state->client_info.client_id = CONFIG::MQTT_CLIENT;

    auto err = mqtt_client_connect(mqtt_client, ip_addr, CONFIG::MQTT_CONNECTION_PORT, mqtt_connection_cb, state, &state->client_info);

    if (err != ERR_OK) {
        printf("mqtt_client_connect failed: %d\n", err);
        return;
    }
    
    printf("Waiting for MQTT connection...\n");
    
    for (int i = 0; i < 150; i++) {  // 5 seconds timeout
        sleep_ms(100);
        cyw43_arch_poll();
        
        if (mqtt_client_is_connected(mqtt_client)) {
            printf("MQTT connected!\n");
            break;
        }
    }
    
    if (!mqtt_client_is_connected(mqtt_client)) {
        printf("MQTT connection timeout\n");
    }

    printf("MQTT client connected\n");
}

MQTTServer::~MQTTServer()
{
    mqtt_disconnect(mqtt_client);
}

static void mqtt_pub_request_cb(void* arg, err_t result)
{
    if (result != ERR_OK)
    {
        printf("Published weather, result: %d\n", result);
    }
}


int MQTTServer::publish_weather_data(double pressure, double temperature, double humidity) const
{
    if (!this->state->connected)
    {
        printf("MQTT client is not connected\n");
        return 0;
    }

    // Store the string, not the pointer!
    const auto payload_str = format("{{"
                                    "\"temperature\":{},"
                                    "\"humidity\":{},"
                                    "\"pressure\":{}}}",
                                    temperature, humidity, pressure);

    if (mqtt_client == nullptr)
    {
        printf("Failed to create MQTT client!\n");
        return 1;
    }

    if (!mqtt_client_is_connected(mqtt_client))
    {
        printf("MQTT client not connected!\n");
        return 1;
    }

    mqtt_publish(mqtt_client, CONFIG::MQTT_TOPIC,
                 payload_str.c_str(), payload_str.length(),
                 0, 1, mqtt_pub_request_cb, this->state);

    return ERR_OK;
}
