//
// Created by PC on 7/19/2025.
//

#ifndef WIFISERVICE_H
#define WIFISERVICE_H
#include <cstdint>
#include <string>
#include <vector>

#include "src/config.h"

struct wifi_state;

enum wifi_mode
{
    Offline = 0,
    CaptivePortalMode,
    StationMode,
    WifiScanningMode,
    ScheduleStationMode,
    RecoveringMode,
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
    [[nodiscard]] uint32_t get_scan_start_time() const;
    void force_scan_completion();

    wifi_mode mode;

    int discover_identifiers();
    int turn_on_captive_portal();
    [[nodiscard]] std::vector<std::string> get_ssids() const;
    int connect_user_network();
    int schedule_connect_user_network(const std::string& ssid, const std::string& password);

    void ping();
};



#endif //WIFISERVICE_H
