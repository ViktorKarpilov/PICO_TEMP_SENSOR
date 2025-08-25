//
// Created by PC on 6/27/2025.
//

#ifndef SRC_H
#define SRC_H
#include "network/wifi_service/WifiService.h"
// static WifiService wifi_service = WifiService::instance();

void loop();
int init();
void setupUART();
WifiService& get_wifi_service();

#endif //SRC_H
