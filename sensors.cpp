/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Read sensors
*
* Created by       :        Arnold Velzel
* Created on       :        08-09-2021
*
*******************************************************************************/

#include "debug.h"
#include "sensors.h"

using namespace AVision;

void clockSensors::onFunctionKey(int timeout, callbackFunction callback)
{
    if (timeout && functionKeyActionIndex < MAXFUNCTION_KEY_ACTIONS)
    {
        functionKeyEvents[functionKeyActionIndex].timeout = timeout;
        functionKeyEvents[functionKeyActionIndex].callback = callback;
        functionKeyActionIndex++;
    }
}
void clockSensors::onFunctionKeyRelease(callbackFunction callback)
{
    releasedEvent = callback;
}
void clockSensors::onFunctionKeyDown(sensorCallbackFunction callback)
{
    pressedEvent = callback;
}

void clockSensors::onAmbientLightChange(int percent, sensorCallbackFunction callback)
{
    if (percent)
    {
        ambientLightEvent.change = percent;
        ambientLightEvent.callback = callback;
    }
}

void clockSensors::triggerAmbientEvent()
{
    _triggerAmbientEvent = true;
}

void clockSensors::loop()
{
    unsigned long m = millis();

    ambientLight = 100 * (MAX_AMBIANTLIGHT - analogRead(A0)) / MAX_AMBIANTLIGHT;
    if (_triggerAmbientEvent && ambientLightEvent.callback != nullptr) {
        _triggerAmbientEvent = false;
        lastAmbientLight = ambientLight;
        ambientLightEvent.callback(ambientLight);
    }
    if (ambientLight != lastAmbientLight)
    {
        if (ambientLightEvent.change && ambientLightEvent.callback != nullptr)
        {
            if (abs((int)ambientLight - (int)lastAmbientLight) > ambientLightEvent.change)
            {
                lastAmbientLight = ambientLight;
                ambientLightEvent.callback(ambientLight);
            }
        }
        else
        {
            lastAmbientLight = ambientLight;
        }
    }

    if (digitalRead(FUNCTION_KEY_INPUT) == LOW)
    {
        if (lastKeyPress == 0)
        {
            lastKeyPress = m;
            functionKeyPressedTrigger = FUNCTION_KEY_REPEAT_INTERVAL;
        }
        functionKeyPressed = m - lastKeyPress;
    }
    else
    {
        lastKeyPress = 0;
    }
    if (functionKeyPressed && (functionKeyPressed >= functionKeyPressedTrigger))
    {
        unsigned long functionKeyPressedBlock = (functionKeyPressed - (functionKeyPressed % FUNCTION_KEY_REPEAT_INTERVAL));
        functionKeyPressedTrigger = functionKeyPressedBlock + FUNCTION_KEY_REPEAT_INTERVAL;
        if (pressedEvent != nullptr)
        {
            pressedEvent(functionKeyPressedBlock);
        }
        for (size_t a = 0; a < functionKeyActionIndex; a++)
        {
            if (functionKeyPressedBlock == functionKeyEvents[a].timeout)
            {
                executeFunctionKeyActionIndex = a;
            }
        }
        if (functionKeyPressed >= 60000 && executeFunctionKeyActionIndex >= 0)
        {
            // Cancel
            executeFunctionKeyActionIndex = -1;
        }
    }

    if (lastKeyPress == 0)
    {
        // Button released
        functionKeyPressed = 0;
        if (releasedEvent != nullptr)
        {
            releasedEvent();
        }
    }

    if (lastKeyPress == 0 && executeFunctionKeyActionIndex >= 0 && functionKeyEvents[executeFunctionKeyActionIndex].callback != nullptr)
    {
        // Button released
        functionKeyEvents[executeFunctionKeyActionIndex].callback();
        executeFunctionKeyActionIndex = -1;
    }
}

void clockSensors::init()
{
    pinMode(FUNCTION_KEY_INPUT, INPUT_PULLUP);
    for (size_t f = 0; f < MAXFUNCTION_KEY_ACTIONS; f++)
    {
        functionKeyEvents[f].timeout = 0;
        functionKeyEvents[f].callback = nullptr;
    }
    dbgln("Sensors initialised");
}

clockSensors::clockSensors()
{
    lastAmbientLight = 0;
    ambientLight = 0;
    ambientLightEvent.change = 0;
    ambientLightEvent.callback = nullptr;
    _triggerAmbientEvent = false;

    lastKeyPress = 0;
    functionKeyPressed = 0;
    pressedEvent = nullptr;
    releasedEvent = nullptr;
    functionKeyActionIndex = 0;
    executeFunctionKeyActionIndex = -1;
}

clockSensors::~clockSensors()
{
}
