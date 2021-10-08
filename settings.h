/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Clock settings
*
* Created by       :        Arnold Velzel
* Created on       :        28-09-2021
*
*******************************************************************************/
#include "avision_system.h"

#define ARDUINOJSON_USE_LONG_LONG 1

#include <ArduinoJson.h>

#include "debug.h"
#include "files.h"

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define MAX_IR_ACTIONS 50

namespace AVision {

    struct clock_settings_struct {
        bool useWiFi;
        bool showSeconds;
        bool showHourIndicators;
        bool showDigital;
        bool autoBrightness;
        int orientation;
        uint16_t LEDtype;
        uint8_t LEDcount;
        bool LEDreversed;
    };
    struct clock_alarm_struct {
        char time[8];
        bool active;
    };
    struct clock_colors_struct {
        uint32_t defaultColor;
        uint32_t hourIndicatorColor;
        uint32_t hourColor;
        uint32_t minuteColor;
        uint32_t secondColor;
    };
    struct settings_struct {
        struct clock_settings_struct clock;
        struct clock_alarm_struct alarm;
        struct clock_colors_struct colors;
    };

    struct settings_ir_actions {
        uint64_t code;
        char * action;
    };

    class clockSettings
    {
        private:

            files drive;
            StaticJsonDocument<2048> json_data;
            bool dirty;

            struct settings_struct clock_settings;
            struct settings_ir_actions ir_actions[MAX_IR_ACTIONS];
            unsigned int ir_actions_count;
            void setDefaults();
            
        public:

            void save(String json);
            void load();
            void loadIRcodes();

            clock_settings_struct getClockSettings();
            clock_alarm_struct getAlarmSettings();
            clock_colors_struct getColorsSettings();
            settings_struct getAllSettings();

            settings_ir_actions* getIRactions();

            void loop();
            void init();

            clockSettings(files systemDrive);
            ~clockSettings();
    };

}

#endif
