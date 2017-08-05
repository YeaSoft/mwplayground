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
#include <base/i2cbus.h>
#include <thing/onoff-GPIO.h>
#include <thing/pushbutton-GPIO.h>
#include <thing/temp-hum-DHT.h>
#include <util/dumper.h>
#include <util/messagespy.h>
#include <util/metronome.h>
#include <util/timebudget.h>

using namespace meisterwerk;

// led class
class MyLed : public core::jentity {
    public:
    util::metronome ledBlinkIntervall = 500;
    uint8_t         pin               = BUILTIN_LED;
    bool            state             = false;

    MyLed( String name, uint8_t pin, unsigned long ledBlinkIntervallMs ) : core::jentity( name, 50000 ), pin{pin} {
        ledBlinkIntervall = ledBlinkIntervallMs;
    }

    virtual void setup() {
        core::jentity::setup();
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

    virtual void onGetValue( String value, JsonObject &params, JsonObject &data ) override {
        if ( value == "info" ) {
            data["type"]  = "led";
            data["state"] = state;
            notify( value, data );
        }
    }
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
    base::i2cbus           i2c;
    thing::dht             dht1;

    public:
    MyApp()
        : core::baseapp( "app", 250000 ), led1( "led1", BUILTIN_LED, 500 ), dmp( "dmp", 0, "btn1" ),
          btn1( "btn1", D4, 1000, 3000 ), relais1( "relais1", D3 ), i2c( "i2cbus", D2, D1 ),
          dht1( "dht", "AM2302", D5 ) {
        dht1.bOnlyValidTime = false;
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
            // ask info
            // publish( "info/get" );
            // publish( "temperature/get" );
            // publish( "humidity/get" );
        }
    }

    virtual void receive( const char *origin, const char *topic, const char *msg ) override {
        String t( topic );
        if ( t == "btn1/short" ) {
            publish( "relais1/toggle" );
        } else if ( t == "btn1/long" ) {
            publish( "relais1/on", "{\"duration\":5000}" );
            publish( "info/get" );
        }
    }
};

// Application Instantiation
MyApp app;
