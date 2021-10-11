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

#include "clock.h"

using namespace AVision;

void clock::displayOnOff()
{
    digitalOn = !digitalOn;
    dbgln("Clock display " + String(digitalOn ? "ON" : "OFF"));
}

void clock::showTime(settings_struct clock_settings)
{
    if (ledsSet)
    {
        uint8_t s_LED = dt.second * clock_settings.clock.LEDcount / 60;
        uint8_t m_LED = dt.minute * clock_settings.clock.LEDcount / 60;
        uint8_t h_LED = ((dt.hour % 12) * 60 + dt.minute) * clock_settings.clock.LEDcount / (12 * 60);
        uint8_t h_divider = clock_settings.clock.LEDcount / 12;
        if ((clock_settings.clock.LEDcount % 12) > 0)
        {
            h_divider = clock_settings.clock.LEDcount / 4;
        }
        if ((clock_settings.clock.LEDcount % 60) > 0)
        {
            clock_settings.clock.showSeconds = false;
        }
        for (size_t s = 0; s < clock_settings.clock.LEDcount; s++)
        {
            if (!h_LED_lit && s == h_LED)
            {
                if (s == m_LED)
                {
                    h_LED_lit = true; // Toggle between hour and minute LED when the same
                }
                c_leds->setLED(s, clock_settings.colors.hourColor);
            }
            else
            {
                if (s == h_LED)
                {
                    h_LED_lit = false; // Toggle between hour and minute LED when the same
                }
                if (s == m_LED)
                {
                    c_leds->setLED(s, clock_settings.colors.minuteColor);
                }
                else
                {
                    if (clock_settings.clock.showSeconds && (s == s_LED))
                    {
                        c_leds->setLED(s, clock_settings.colors.secondColor);
                    }
                    else
                    {
                        if (clock_settings.clock.showHourIndicators && (s % h_divider == 0))
                        {
                            c_leds->setLED(s, clock_settings.colors.hourIndicatorColor);
                        }
                        else
                        {
                            c_leds->setLED(s, clock_settings.colors.defaultColor);
                        }
                    }
                }
            }
        }
    }

    if (displaySet && digitalOn)
    {
        c_Display->text = " " + ts.substring(11, 16) + "\n " + ts.substring(8, 10) + "-" + ts.substring(5, 7);
    }
    else
    {
        c_Display->text = "";
    }
}
void clock::setLeds(RGBleds *leds)
{
    c_leds = leds;
    ledsSet = true;
}
void clock::setDisplay(dotmatrixDisplay *display)
{
    c_Display = display;
    displaySet = true;
}

void clock::onYear(callbackFunctionInt event)
{
    yearEvent = event;
}
void clock::onYearCountdown(callbackFunctionInt event, int secondsPrior)
{
    yearCountdownEvent = event;
    countDownSeconds = secondsPrior;
}

void clock::onDateTimeChange(callbackFunctionString event)
{
    datetimeEvent = event;
}
void clock::onHour(callbackFunctionInt event)
{
    hourEvent = event;
}
void clock::onMinute(callbackFunctionInt event)
{
    minuteEvent = event;
}
void clock::onSecond(callbackFunctionInt event)
{
    secondEvent = event;
}

clock_datetime clock::getDateTime()
{
    return dt;
}
String clock::getDateTimeString()
{
    return ts;
}
void clock::setDateTime(String ts)
{
    if (useRTC)
    {
        String date = ts.substring(4, 15);
        String time = ts.substring(16, 24);
        dbgln(date + " " + time);
        DateTime rts_dt = DateTime(date.c_str(), time.c_str());
        rtc.adjust(rts_dt);
    }
    else
    {
        //
    }
}

void clock::ticktock(bool on)
{
    if (on != show_RTC_led)
    {
        if (on)
        {
            rtc.writeSqwPinMode(DS1307_SquareWave1HZ);
        }
        else
        {
            rtc.writeSqwPinMode(DS1307_ON); // High output -> LED off
        }
        show_RTC_led = on;
    }
}

void clock::loop()
{
    String currentTs = "";
    if (useRTC)
    {
        rtc_datetime = rtc.now();
        if (rtc_datetime.day() < 32) {
            dt.year = rtc_datetime.year();
            dt.month = rtc_datetime.month();
            dt.day = rtc_datetime.day();
            dt.hour = rtc_datetime.hour();
            dt.minute = rtc_datetime.minute();
            dt.second = rtc_datetime.second();
            currentTs = rtc_datetime.timestamp();
        } else {
            useRTC = false;
        }
    }
    else
    {
        time_t now = time(nullptr);
        struct tm *nowtime = localtime(&now);
        dt.year = nowtime->tm_year + 1900;
        dt.month = nowtime->tm_mon + 1;
        dt.day = nowtime->tm_mday;
        dt.hour = nowtime->tm_hour;
        dt.minute = nowtime->tm_min;
        dt.second = nowtime->tm_sec;
        char strBuffer[25];
        sprintf(strBuffer, "%04d-%02d-%02dT%02d:%02d:%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
        currentTs = String(strBuffer);
    }

    //2020-04-16T18:34:56
    //0000000000111111111
    //0123456789012345678

    if (ts != "")
    {
        if ((yearEvent != nullptr) && (currentTs.substring(0, 4) != ts.substring(0, 4)))
        {
            yearEvent(dt.year);
        }
        if ((hourEvent != nullptr) && (currentTs.substring(11, 13) != ts.substring(11, 13)))
        {
            hourEvent(dt.hour);
        }
        if ((minuteEvent != nullptr) && (currentTs.substring(14, 16) != ts.substring(14, 16)))
        {
            minuteEvent(dt.minute);
        }
        if ((currentTs.substring(17, 19) != ts.substring(17, 19)))
        {
            if ((yearCountdownEvent != nullptr) && (dt.month == 12) && (dt.day == 31) && (dt.hour == 23) && (dt.second >= (60 - countDownSeconds)))
            {
                yearCountdownEvent(60 - dt.second);
            }
            if (secondEvent != nullptr)
            {
                secondEvent(dt.second);

                if (!useRTC)
                {
                    if (!rtc.begin())
                    { // initialize RTC chip
                        dbgln("\n\nRTC Chip init error!\n\n");
                    }
                    else
                    {
                        useRTC = true;
                        if (!rtc.isrunning())
                        {
                            rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
                        }
                    }
                }
            }
        }
    }

    if (ts != currentTs)
    {
        ts = currentTs;
        if (datetimeEvent != nullptr)
        {
            datetimeEvent(ts);
        }
    }
}
void clock::init(clock_settings_struct clock_settings)
{
    delay(100);
    Wire.begin();
    delay(500);
    int max_retries = 5;
    dbg("Initialise RTC ");
    while (!useRTC && max_retries)
    {
        useRTC = rtc.begin();
        delay(250);
        max_retries--;
        dbg(".");
    }
    if (useRTC)
    {
        ticktock(true);
        if (!rtc.isrunning())
        {
            rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        }
        dbgln(" SUCCESS");
    }
    else
    {
        dbgln(" FAILED");
    }

    digitalOn = clock_settings.showDigital;

    int timezone = 2 * 3600; // CET
    configTime(timezone, 0, "pool.ntp.org", "time.nist.gov");
    dbgln("Clock initialised");
}

clock::clock()
{
    ledsSet = false;
    displaySet = false;
    useNTP = false;
    useRTC = false;
    show_RTC_led = false;
    h_LED_lit = false;
    ts = "";

    yearEvent = nullptr;
    yearCountdownEvent = nullptr;
    datetimeEvent = nullptr;
    hourEvent = nullptr;
    minuteEvent = nullptr;
    secondEvent = nullptr;

    countDownSeconds = 0;
    digitalOn = false;
}

clock::~clock()
{
}
