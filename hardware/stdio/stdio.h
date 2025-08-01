//
// Created by PC on 8/1/2025.
//

#ifndef STDIO_H
#define STDIO_H
#include <pico/stdio.h>
#include "pico/stdio_usb.h"

#define log(message) printf(message)

#define sensor_stdio_usb_connected() stdio_usb_connected()

#endif //STDIO_H
