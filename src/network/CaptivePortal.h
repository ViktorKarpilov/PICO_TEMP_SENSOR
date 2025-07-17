//
// Created by PC on 7/12/2025.
//
#ifndef CAPTIVEPORTAL_H
#define CAPTIVEPORTAL_H
#include "access_point/dhcpserver/DhcpServer.h"
#include "access_point/dnsserver/DnsServer.h"
#include "http_server/HttpServer.h"


class CaptivePortal
{
private:
    DhcpServer dhcpServer;
    DnsServer dnsServer;
    HttpServer httpServer;
    
    public:
        CaptivePortal();
        int initiate();
};

#endif //CAPTIVEPORTAL_H
