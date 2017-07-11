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
#include <thing/gpiopushbutton.h>
#include <util/dumper.h>
#include <util/messagespy.h>

using namespace meisterwerk;

// led class
class MyLed : public core::entity {
    public:
    unsigned long ledLastChange       = 0;
    unsigned long ledBlinkIntervallMs = 500;
    uint8_t       pin                 = BUILTIN_LED;
    bool          state               = false;

    MyLed( String _name, uint8_t _pin ) : core::entity( _name ) {
        pin = _pin;
    }

    virtual void onSetup() {
        pinMode( pin, OUTPUT );
    }

    virtual void onLoop( unsigned long ticker ) {
        unsigned long millis = ( ticker - ledLastChange ) / 1000L;
        if ( millis >= ledBlinkIntervallMs ) {
            ledLastChange = ticker;
            if ( state ) {
                state = false;
                digitalWrite( pin, HIGH );
            } else {
                state = true;
                digitalWrite( pin, LOW );
            }
        }
    }
};

// application class
class MyApp : public core::baseapp {
    public:
    MyLed                 led1;
    util::messagespy      spy;
    util::dumper          dmp;
    thing::gpiopushbutton dbg;

    public:
    MyApp()
        : core::baseapp( "MyApp" ), led1( "led1", BUILTIN_LED ), dmp( "dmp" ),
          dbg( "dbg", D4, 1000, 5000 ) {
    }

    virtual void onSetup() {
        // Debug console
        Serial.begin( 115200 );

        spy.registerEntity();
        dmp.registerEntity();
        dbg.registerEntity();
        led1.registerEntity( 50000, core::scheduler::PRIORITY_NORMAL );
    }
};

// Application Instantiation
MyApp app;
