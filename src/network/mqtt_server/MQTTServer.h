//
// Created by PC on 8/25/2025.
//

#ifndef MQTTSERVER_H
#define MQTTSERVER_H
#include <string>
#include <lwip/apps/mqtt.h>

using namespace std;

struct MQTTServerState
{
    bool connected{false};
    mqtt_connect_client_info_t client_info{};
};

class MQTTServer
{
    mqtt_client_t* mqtt_client;
    MQTTServerState* state;

public:
    explicit MQTTServer(const ip_addr_t* ip_addr);
    ~MQTTServer();

    int publish_weather_data(double pressure, double temperature, double humidity) const;
};


#endif //MQTTSERVER_H
