/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        RGB LEDs Clock management
*
* Created by       :        Arnold Velzel
* Created on       :        08-09-2021
*
*******************************************************************************/

#include "debug.h"
#include "RGBleds.h"

using namespace AVision;

uint8_t RGBleds::secondToLEDindex(int second)
{
    if (LEDreversed) {
        return ((60-second) + firstLED) % LEDcount;
    } else {
        return (second + firstLED) % LEDcount;
    }
}

uint32_t RGBleds::colorBrightness(uint32_t color, uint8_t brightness)
{
    uint32_t LEDcolor = color;

    uint8_t w = ((LEDcolor >> 24) & 0xFF);
    uint8_t r = ((LEDcolor >> 16) & 0xFF);
    uint8_t g = ((LEDcolor >> 8) & 0xFF);
    uint8_t b = ((LEDcolor >> 0) & 0xFF);

    w = (w * brightness) / 100;
    r = (r * brightness) / 100;
    g = (g * brightness) / 100;
    b = (b * brightness) / 100;

    return (w << 24) + (r << 16) + (g << 8) + b;
}

bool RGBleds::setLED(int second, uint32_t color)
{
    int index = secondToLEDindex(second);
    if (CLOCK_LEDS[index] != color)
    {
        CLOCK_LEDS[index] = color;
        UPDATE_LEDS[index] = true;
        return true;
    }
    return false;
}
void RGBleds::setAll(uint32_t color)
{
    for (size_t s = 0; s < LEDcount; s++)
    {
        setLED(s, color);
    }
    update(true);
}
void RGBleds::setAll(uint32_t color, int brightness)
{
    setAll(color);
    setBrightness(brightness);
}

void RGBleds::update(bool force = false)
{
    bool show = false;
    for (size_t i = 0; i < LEDcount; i++)
    {
        if (force || UPDATE_LEDS[i])
        {
            uint32_t color = colorBrightness(CLOCK_LEDS[i], currentBrightness);
            leds->setPixelColor(i, color);
            show = true;
        }
        UPDATE_LEDS[i] = false;
    }
    if (show)
    {
        leds->show();
    }
}

void RGBleds::setBrightness(int brightness)
{
    if (brightness != currentBrightness)
    {
        currentBrightness = brightness;
        update(true);
    }
}

void RGBleds::rotate(bool on)
{
    if (on)
    {
        setBrightness(10);
    }
    //dbg(on ? " -> " : " <- ");
    for (size_t i = 0; i < LEDcount; i++)
    {
        if (on)
        {
            setLED(i, leds->Color(255, 255, 255, 255));
        }
        else
        {
            setLED(i, leds->Color(0, 0, 0, 0));
        }
        update();
        //dbg(String(i) + " ");
        delay(500 / LEDcount);
    }
    //dbgln("");
    if (!on)
    {
        setBrightness(0);
    }
}

void RGBleds::party(bool on)
{
    if (on)
    {
        for (size_t i = 0; i < 10; i++)
        {
            uint8_t r = random(255);
            uint8_t g = random(255);
            uint8_t b = random(255);
            setAll((r << 16) + (g << 8) + b, 100);
            delay(100 + random(500));
            yield();
        }
    }
    else
    {
        setAll(0);
    }
}

void RGBleds::setOrientation(int orientation)
{
    firstLED = orientationFirstLED[orientation];
    update(true);
}

void RGBleds::setLEDcount(uint8_t count)
{
    if (count != LEDcount) {
        LEDcount = count;
        orientationFirstLED[0] = 0 * LEDcount / 4;
        orientationFirstLED[1] = 1 * LEDcount / 4;
        orientationFirstLED[2] = 2 * LEDcount / 4;
        orientationFirstLED[3] = 3 * LEDcount / 4;
        leds->updateLength(LEDcount);
    }
}
void RGBleds::setLEDtype(uint16_t type)
{
    leds->updateType(type);
}
void RGBleds::setLEDreversed(bool on)
{
    LEDreversed = on;
}

void RGBleds::loop()
{
    update();
}

void RGBleds::init()
{
    leds->begin();
    leds->show();
    for (size_t i = 0; i < LEDcount; i++)
    {
        setLED(i, 0);
    }
    update(true);
    initialised = true;

    dbgln("RGBleds initialised");
}

RGBleds::RGBleds()
{
    LEDreversed = LED_REVERSED;
    LEDcount = LED_COUNT;
    leds = new Adafruit_NeoPixel(LEDcount, PIN, LED_TYPE + NEO_KHZ800);
    currentBrightness = 0;
}

RGBleds::~RGBleds()
{
}
