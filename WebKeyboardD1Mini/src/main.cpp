#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include "ESPAsyncWebServer.h"

#define KEY_DOWN   0b00000001
#define KEY_UP     0b00000010
#define MOUSE_MOVE 0b00000100
#define MOUSE_DOWN 0b00001000
#define MOUSE_UP   0b00010000
#define FOCUS      0b00100000

struct t_event {
    uint8_t action;
    uint8_t code;
    uint16_t x, y;
    uint16_t crc;
};

t_event event;
WiFiServer wifiServer( 2300 );

void setup() {
    WiFi.disconnect();
    WiFi.mode( WIFI_AP );
    WiFi.softAPConfig( IPAddress( 192, 168, 10, 1 ), 0, IPAddress( 255, 255, 255, 0 ) );
    WiFi.softAP( "WebKeyboard" );

    Serial1.begin( 115200 ); // RX = D4, TX = None
    Serial.begin( 9600 );

    wifiServer.begin();

    delay( 1000 );
    Serial.printf( "Packet Size %d\n", sizeof( event ) );
}

void loop() {
    WiFiClient client = wifiServer.available();
    if (client && client.connected()) {
        client.setTimeout( 1 );
        if (client.readBytes( (uint8_t*)(&event), sizeof( event ) ) == sizeof( event )) {
            Serial.printf( "recv %d %d %d %d %d\n", event.action, event.code, event.x, event.y, event.crc );
            Serial1.write( (uint8_t*)(&event), sizeof( event ) );
        } else {
            Serial.printf( "client no data" );
        }
    }
}