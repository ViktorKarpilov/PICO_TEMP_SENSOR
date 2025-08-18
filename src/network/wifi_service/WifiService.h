//
// Created by PC on 7/19/2025.
//

#ifndef WIFISERVICE_H
#define WIFISERVICE_H
#include <cstdint>
#include <string>

#include "src/config.h"

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
    WifiService();
public:
    static WifiService& instance() {
        static WifiService wifi_service;
        return wifi_service;
    }

    ~WifiService();

    wifi_mode mode;

    int discover_identifiers();
    int turn_on_captive_portal();
    int get_ssids(uint8_t ssids[][CONFIG::SSID_MAX_SIZE], int max_count) const;
    int connect_user_network(const std::string& ssid, const std::string& password);
    void ping() const;
};



#endif //WIFISERVICE_H
