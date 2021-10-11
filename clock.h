/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Clock management
*
* Created by       :        Arnold Velzel
* Created on       :        28-09-2021
*
*******************************************************************************/
#include "avision_system.h"

#include <Wire.h>             // Wire library (required for I2C devices)
#include "RTClib.h"           // RTC library

#include "settings.h"
#include "RGBleds.h"
#include "display.h"

#ifndef __CLOCK_H__
#define __CLOCK_H__

namespace AVision
{
    struct clock_datetime {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
    };

    class clock
    {
        private:

            RGBleds *c_leds;
            bool ledsSet;
            dotmatrixDisplay *c_Display;
            bool displaySet;

            bool useNTP;

            bool useRTC;
            RTC_DS1307 rtc;
            DateTime   rtc_datetime;

            clock_datetime dt;
            String ts; // current Timestamp

            callbackFunctionString datetimeEvent;
            callbackFunctionInt yearEvent;
            callbackFunctionInt yearCountdownEvent;
            callbackFunctionInt hourEvent;
            callbackFunctionInt minuteEvent;
            callbackFunctionInt secondEvent;

            int countDownSeconds;
            bool h_LED_lit;
            bool show_RTC_led;
            bool digitalOn;

        public:

            void setDateTime(String ts);
            clock_datetime getDateTime();
            String getDateTimeString();

            void showTime(settings_struct clock_settings);
            void ticktock(bool on);

            void onDateTimeChange(callbackFunctionString event);
            void onYear(callbackFunctionInt event);
            void onYearCountdown(callbackFunctionInt event, int secondsPrior);
            void onHour(callbackFunctionInt event);
            void onMinute(callbackFunctionInt event);
            void onSecond(callbackFunctionInt event);

            void setLeds(RGBleds *leds);
            void setDisplay(dotmatrixDisplay *display);
            void displayOnOff();

            void loop();
            void init(clock_settings_struct clock_settings);

            clock();
            ~clock();
            
        };    

} // namespace AVision

#endif