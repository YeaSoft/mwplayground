// app.cpp - Sample Application 1
//
// This small sample application built with
// the MeisterWerk Framework flashes the
// internal LED

// platform includes
#include <ESP8266WiFi.h>

#include <ArduinoJson.h>

// Let MeisterWerk output debugs on Serial
#define _MW_DEBUG 1

// framework includes
#include <MeisterWerk.h>
#include <base/i2cbus.h>
#include <base/net.h>
#include <core/topic.h>
#include <thing/luminosity-TSL2561.h>
#include <thing/mqtt.h>
#include <thing/onoff-GPIO.h>
#include <thing/pushbutton-GPIO.h>
#include <thing/temp-hum-DHT.h>
#include <util/dumper.h>
#include <util/messagespy.h>
#include <util/metronome.h>
#include <util/timebudget.h>

extern "C" {
#include "pwm.h"
#include "user_interface.h"
}

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
    base::i2cbus           i2c;
    base::net              net;
    util::messagespy       spy;
    util::dumper           dmp;
    util::metronome        beat;
    thing::mqtt            mqtt;
    thing::pushbutton_GPIO btn1;
    thing::onoff_GPIO      relais1;
    thing::dht             dht;
    thing::tsl2561         tsl;

    int  level      = 0;
    bool bdirection = false;
    int  bglide     = false;
    //    const uint16_t PWM_duty_cycle[] = {0,  1,   2,   3,   4,   6,   9,   13,  19,   28,   40,   58,
    //                                       83, 118, 168, 238, 339, 482, 686, 978, 1382, 1996, 2874, 4095};

    public:
    MyApp()
        : core::baseapp( "app", 50000 ), led1( "led1", BUILTIN_LED, 500 ), dmp( "dmp", 0, "btn1" ),
          btn1( "btn1", D4, 1000, 3000 ), relais1( "relais1", D3 ), i2c( "i2cbus", D2, D1 ), dht( "dht", "AM2302", D5 ),
          tsl( "tsl", 57 ) {
        dht.bOnlyValidTime = false;
        tsl.bOnlyValidTime = false;
    }

    virtual void setup() override {
        // Initialize debug console
        Serial.begin( _MW_SERIAL_SPEED );

        subscribe( "btn1/short" );
        subscribe( "btn1/long" );
        subscribe( "btn1/extralong" );

        subscribe( "digital/value/set" );
        subscribe( "analog/value/set" );
        subscribe( "analog/range/set" );
        subscribe( "analog/frequency/set" );

        subscribe( "pulse/on" );
        subscribe( "pulse/off" );
        // perform action all 15 seconds
        beat = 15000;
    }

    void loop() override {
        if ( bglide ) {
            if ( bdirection ) {
                level = level + 8;
                if ( level >= 1023 ) {
                    DBG( "Pulse Down" );
                    level      = 1023;
                    bdirection = false;
                }
            } else {
                level = level - 8;
                if ( level <= 0 ) {
                    DBG( "Pulse Up" );
                    level      = 0;
                    bdirection = true;
                }
            }
            noInterrupts();
            analogWrite( D6, level );
            interrupts();
        }
        if ( beat.beat() ) {
            // ask info
            // publish( "info/get" );
            // publish( "temperature/get" );
            // publish( "humidity/get" );
        }
    }

    virtual void receive( const char *origin, const char *tpc, const char *msg ) override {
        DynamicJsonBuffer paramBuffer( 256 );
        DynamicJsonBuffer mdataBuffer( 256 );
        JsonObject &      param = paramBuffer.parseObject( msg );
        JsonObject &      mdata = mdataBuffer.createObject();
        core::Topic       topic = tpc;
        if ( topic == "btn1/short" ) {
            publish( "relais1/toggle" );
        } else if ( topic == "btn1/long" ) {
            publish( "relais1/on", "{\"duration\":5000}" );
            publish( "info/get" );
        } else if ( topic == "digital/value/set" ) {
            DBGF( "Digital Write D6: %d\n", param["value"].as<bool>() ? LOW : HIGH );
            digitalWrite( D6, param["value"].as<bool>() ? LOW : HIGH );
        } else if ( topic == "analog/value/set" ) {
            DBGF( "Analog Write D6: %d\n", param["value"].as<int>() );
            analogWrite( D6, param["value"].as<int>() );
        } else if ( topic == "analog/range/set" ) {
            DBGF( "Analog range set: %d\n", param["range"].as<uint32_t>() );
            analogWriteFreq( param["range"].as<uint32_t>() );
        } else if ( topic == "analog/frequency/set" ) {
            DBGF( "Analog frequency set: %d\n", param["frequency"].as<uint32_t>() );
            analogWriteRange( param["frequency"].as<uint32_t>() );
        } else if ( topic == "pulse/on" ) {
            level      = 0;
            bdirection = true;
            bglide     = true;
            analogWrite( D6, 0 );
        } else if ( topic == "pulse/off" ) {
            bglide = false;
            analogWrite( D6, 0 );
        }
    }
};

// Application Instantiation
MyApp app;
