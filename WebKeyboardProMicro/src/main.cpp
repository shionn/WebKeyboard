#include <Arduino.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <MouseTo.h>

#define KEY_DOWN       0b00000001
#define KEY_UP         0b00000010
#define MOUSE_MOVE 0b00000100
#define MOUSE_DOWN 0b00001000
#define MOUSE_UP   0b00010000
#define FOCUS      0b00100000

/* faire cette modification dans KeyboardLayout_en_US.cpp
    0x32,          // < 0x36|SHIFT
    0x32|SHIFT,    // > 0x37|SHIFT
*/

struct t_event {
    uint8_t action;
    uint8_t code;
    uint16_t x, y;
    uint16_t crc;
};
t_event event;

void setup() {
    Keyboard.begin();
    Mouse.begin();
    MouseTo.setScreenResolution( 5120, 1440 );
    // MouseTo.setMaxJump( 100 );
    event.action = 0;

    Serial.begin( 9600 );
    Serial1.begin( 115200 ); // TX0 , RX1

    delay( 2000 );
    MouseTo.setTarget( 0, 0, true );
    while (!MouseTo.move());

    Serial.print( "Packet size " );
    Serial.println( sizeof( event ) );
}

uint8_t toKey( uint8_t k ) {
    switch (k) {
    default: return k;
    }
}

void loop() {
    if (Serial1.available() >= sizeof( event )) {
        Serial1.readBytes( (uint8_t*)(&event), sizeof( event ) );
        Serial.print( "rcv " );
        Serial.print( event.action );
        Serial.print( " " );
        Serial.print( event.code );
        Serial.print( " " );
        Serial.print( event.x );
        Serial.print( " " );
        Serial.print( event.y );
        Serial.print( " " );
        Serial.println( event.crc );
        if (event.crc == 255) {
            switch (event.action) {
            case KEY_DOWN: Keyboard.press( toKey( event.code ) ); break;
            case KEY_UP: Keyboard.release( toKey( event.code ) ); break;
            case MOUSE_MOVE:
                MouseTo.setTarget( event.x, event.y, false );
                while (!MouseTo.move());
                break;
            case MOUSE_DOWN: Mouse.press( event.code ); break;
            case MOUSE_UP: Mouse.release( event.code ); break;
            }
        } else {
            Keyboard.releaseAll();
            while (Serial1.read() != -1);
            // Serial1.flush();
        }
        // Serial1.write( (uint8_t)1 );
    }
}
