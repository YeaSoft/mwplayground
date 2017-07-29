// app.cpp - Sample Application 1
//
// This small sample application built with
// the MeisterWerk Framework flashes the
// internal LED

// platform includes
#include <ESP8266WiFi.h>

// Let MeisterWerk output debugs on Serial
#define _MW_DEBUG 1

// framework includes
#include <MeisterWerk.h>
#include <thing/onoff-GPIO.h>
#include <thing/pushbutton-GPIO.h>
#include <util/dumper.h>
#include <util/messagespy.h>
#include <util/metronome.h>
#include <util/timebudget.h>

using namespace meisterwerk;

// led class
class MyLed : public core::entity {
    public:
    util::metronome ledBlinkIntervall = 500;
    uint8_t         pin               = BUILTIN_LED;
    bool            state             = false;

    MyLed( String name, uint8_t pin, unsigned long ledBlinkIntervallMs ) : core::entity( name, 50000 ), pin{pin} {
        ledBlinkIntervall = ledBlinkIntervallMs;
    }

    virtual void setup() {
        core::entity::setup();
        pinMode( pin, OUTPUT );
    }

    virtual void loop() {
        if ( ledBlinkIntervall.beat() ) {
            if ( state ) {
                state = false;
                digitalWrite( pin, HIGH );
            } else {
                state = true;
                digitalWrite( pin, LOW );
            }
        }
    }
    /*
        virtual void onGetState( JsonObject &request, JsonObject &response ) override {
            response["type"]  = "led";
            response["state"] = state;
        }

        virtual bool onSetState( JsonObject &request, JsonObject &response ) override {
            return false;
        }
    */
};

// application class
class MyApp : public core::baseapp {
    public:
    MyLed                  led1;
    util::messagespy       spy;
    util::dumper           dmp;
    util::metronome        beat;
    thing::pushbutton_GPIO btn1;
    thing::onoff_GPIO      relais1;

    public:
    MyApp()
        : core::baseapp( "app", 250000 ), led1( "led1", BUILTIN_LED, 500 ), dmp( "dmp", 0, "btn1" ),
          btn1( "btn1", D4, 1000, 3000 ), relais1( "relais1", D3 ) {
    }

    virtual void setup() override {
        // Initialize debug console
        Serial.begin( _MW_SERIAL_SPEED );

        subscribe( "btn1/short" );
        subscribe( "btn1/long" );
        subscribe( "btn1/extralong" );
        // perform action all 15 seconds
        beat = 15000;
    }

    void loop() override {
        if ( beat.beat() ) {
            // ask state and configuration
            publish( "spy/getstate" );
            publish( "dmp/getstate" );
            publish( "btn1/getstate" );
            publish( "relais1/getstate" );
        }
    }

    virtual void receive( const char *origin, const char *topic, const char *msg ) override {
        String t( topic );
        if ( t == "btn1/short" ) {
            publish( "relais1/toggle" );
        } else if ( t == "btn1/long" ) {
            publish( "relais1/on", "{\"duration\":5000}" );
        }
    }
};

// Application Instantiation
MyApp app;
