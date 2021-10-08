/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Sensors
*
* Created by       :        Arnold Velzel
* Created on       :        24-09-2021
*
*******************************************************************************/
#include "avision_system.h"

#ifndef __SENSORS_H__
#define __SENSORS_H__

#define FUNCTION_KEY_INPUT              D6
#define FUNCTION_KEY_REPEAT_INTERVAL    100
#define MAXFUNCTION_KEY_ACTIONS         8
#define MAX_AMBIANTLIGHT                1024

typedef void (*sensorCallbackFunction)(int);
struct ambientLightEventStruct {
    int change;
    sensorCallbackFunction callback;
};
struct functionKeyEventStruct {
    int timeout;
    callbackFunction callback;
};

//ambientLightChangeEvent
namespace AVision {

    class clockSensors
    {
        private:
            unsigned int lastAmbientLight;
            unsigned int ambientLight;        // Value 0 - 100%

            unsigned long lastKeyPress;
            unsigned long functionKeyPressed; // Number of ms the functionkey is pressed
            unsigned long functionKeyPressedTrigger;

            ambientLightEventStruct ambientLightEvent;
            functionKeyEventStruct functionKeyEvents[MAXFUNCTION_KEY_ACTIONS]; // Max 8 functionkey actions
            sensorCallbackFunction pressedEvent;
            callbackFunction releasedEvent;
            int functionKeyActionIndex;
            int executeFunctionKeyActionIndex;

            bool _triggerAmbientEvent;

        public:

            // Triggered when ambient light changes XX percent
            void onAmbientLightChange(int percent, sensorCallbackFunction callback);
            void triggerAmbientEvent();

            // Generic event fired every 100ms while the functionkey is pressed
            void onFunctionKeyDown(sensorCallbackFunction callback);
            // Generic event when function key is released (note! specific timeout event is triggered first!)
            void onFunctionKeyRelease(callbackFunction callback);
            // Event when function key release after [timeout] milliseconds
            void onFunctionKey(int timeout, callbackFunction callback);

            void loop();
            void init();

            clockSensors();
            ~clockSensors();

    };
    
}

#endif
