//
// Created by PC on 7/19/2025.
//

#ifndef WIFISERVICE_H
#define WIFISERVICE_H
#include <cstdint>
#include <string>

struct wifi_state;

enum wifi_mode
{
    Offline = 0,
    CaptivePortalMode,
    StationMode,
    WifiScanningMode,
};

class WifiService {
    wifi_state *state;
public:
    WifiService();
    ~WifiService();

    wifi_mode mode;

    int discover_identifiers();
    int turn_on_captive_portal();
    int get_ssids(uint8_t ssids[][32], int max_count) const;
    int connect_user_network(std::string ssid, std::string password);
};



#endif //WIFISERVICE_H
