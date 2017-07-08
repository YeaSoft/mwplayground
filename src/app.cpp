// app.cpp - Sample Application 1
//
// This small sample application built with
// the MeisterWerk Framework flashes the
// internal LED

// platform includes
#include <ESP8266WiFi.h>

// framework includes
#include <MeisterWerk.h>

using namespace meisterwerk;

// application class
class MyApp : public core::baseapp {
    public:
    MyApp() {
    }

    ~MyApp() {
    }

    void onSetup() {
        pinMode( LED_BUILTIN, OUTPUT );
        delay( 1000 );
    }

    void onLoop() {
        delay( 500 );
        digitalWrite( LED_BUILTIN, LOW );
        delay( 200 );
        digitalWrite( LED_BUILTIN, HIGH );
    }
};

// Application Instantiation
MyApp app;
