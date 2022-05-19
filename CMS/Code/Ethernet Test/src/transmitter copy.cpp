#define TRANSMITTER
#ifdef TRANSMITTER

#include "common.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

char mac[] = "eclair";
IPAddress ip(10, 4, 26, 110);

void setup()
{
    Ethernet.init();
    Ethernet.begin((uint8_t *)mac, ip, gateway, subnetMask);
}

void loop()
{
}

#endif // TRANSMITTER