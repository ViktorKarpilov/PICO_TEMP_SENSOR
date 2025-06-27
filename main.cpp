#include <stdio.h>
#include "pico/stdlib.h"
#include "src/src.h"

[[noreturn]] int main() {
    init();
    
    while (true)
    {
        loop();
    }
}