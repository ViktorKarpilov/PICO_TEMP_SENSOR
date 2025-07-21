//
// Created by PC on 7/19/2025.
//

#ifndef WIFISERVICE_H
#define WIFISERVICE_H
#include <cstdint>
#include <string>

struct wifi_state;

class WifiService {
private:
    wifi_state *state;
public:
    WifiService();
    ~WifiService();

    int discover_identifiers() const;
    int turn_on_captive_portal() const;
    int connect_user_network(std::string ssid, std::string password);
};



#endif //WIFISERVICE_H
