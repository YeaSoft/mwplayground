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
#include <thing/pushbutton-GPIO.h>
#include <util/dumper.h>
#include <util/messagespy.h>
#include <util/timebudget.h>

using namespace meisterwerk;

// led class
class MyLed : public core::entity {
    public:
    unsigned long ledLastChange       = 0;
    unsigned long ledBlinkIntervallUs = 500000L;
    uint8_t       pin                 = BUILTIN_LED;
    bool          state               = false;

    MyLed( String name, uint8_t pin, unsigned long ledBlinkIntervallMs )
        : core::entity( name ), pin{pin} {
        ledBlinkIntervallUs = ledBlinkIntervallMs * 1000;
    }

    virtual void onSetup() {
        pinMode( pin, OUTPUT );
    }

    virtual void onLoop( unsigned long ticker ) {
        unsigned long micros = util::timebudget::delta( ledLastChange, ticker );
        if ( micros >= ledBlinkIntervallUs ) {
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
    MyLed                  led1;
    MyLed                  led2;
    util::messagespy       spy;
    util::dumper           dmp;
    thing::pushbutton_GPIO dbg;

    public:
    MyApp()
        : core::baseapp( "MyApp" ), led1( "led1", BUILTIN_LED, 500 ), led2( "led2", D3, 250 ),
          dmp( "dmp" ), dbg( "dbg", D4, 1000, 5000 ) {
    }

    virtual void onSetup() {
        // Debug console
        Serial.begin( 115200 );

        spy.registerEntity();
        dmp.registerEntity();
        dbg.registerEntity();
        led1.registerEntity( 50000 );
        led2.registerEntity( 50000 );
    }
};

// Application Instantiation
MyApp app;
