// app.cpp - Sample Application 1
//
// This small sample application built with
// the MeisterWerk Framework flashes the
// internal LED

// platform includes
#include <ESP8266WiFi.h>

// Let MeisterWerk output debugs on Serial
#define DEBUG 1

// framework includes
#include <MeisterWerk.h>

using namespace meisterwerk::core;

// led class
class MyLed : public entity {
    public:
    unsigned long ledLastChange       = 0;
    unsigned long ledBlinkIntervallMs = 500;
    bool          state               = false;

    MyLed() : entity( "led1" ) {
        pinMode( LED_BUILTIN, OUTPUT );
    }

    virtual void onLoop( unsigned long ticker ) {
        unsigned long millis = ( ticker - ledLastChange ) / 1000L;
        if ( millis >= ledBlinkIntervallMs ) {
            ledLastChange = ticker;
            if ( state ) {
                state = false;
                digitalWrite( LED_BUILTIN, HIGH );
            } else {
                state = true;
                digitalWrite( LED_BUILTIN, LOW );
            }
        }
    }
};

// application class
class MyApp : public baseapp {
    public:
    MyLed led1;

    MyApp() : baseapp( "MyApp" ) {
    }

    virtual void onSetup() {
        // Debug console
        Serial.begin( 115200 );

        led1.registerEntity( 50000, scheduler::PRIORITY_NORMAL );
    }
};

// Application Instantiation
MyApp app;
