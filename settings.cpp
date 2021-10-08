/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Clock settings
*
* Created by       :        Arnold Velzel
* Created on       :        08-09-2021
*
*******************************************************************************/

#include "debug.h"
#include "settings.h"

using namespace AVision;

void clockSettings::setDefaults()
{
    clock_settings.clock.useWiFi = false;
    clock_settings.clock.showSeconds = true;
    clock_settings.clock.showHourIndicators = true;
    clock_settings.clock.showDigital = false;
    clock_settings.clock.autoBrightness = true;
    clock_settings.clock.orientation = 2;
    clock_settings.clock.LEDtype = 0;
    clock_settings.clock.LEDcount = 0;
    clock_settings.clock.LEDreversed = false;

    strcpy(clock_settings.alarm.time, "07:00");
    clock_settings.alarm.active = false;

    clock_settings.colors.defaultColor = 0x010101;
    clock_settings.colors.hourIndicatorColor = 0x808080;
    clock_settings.colors.hourColor = 0x01FF01;
    clock_settings.colors.minuteColor = 0xFF0101;
    clock_settings.colors.secondColor = 0x010180;    
}

void clockSettings::save(String json)
{
    drive.save("/settings.json", json);
    delay(100);
    load();
}
void clockSettings::loadIRcodes()
{
    for (size_t a = 0; a < ir_actions_count; a++)
    {
        ir_actions[a].code = 0;
        free(ir_actions[a].action);
        ir_actions[a].action = nullptr;
    }
    ir_actions_count = 0;

    String settingsJSON = drive.load("/settings.json");
    if (settingsJSON != "")
    {
        DeserializationError error = deserializeJson(json_data, (char *)settingsJSON.c_str());
        if (error)
        {
            dbgln(error.f_str());
        }
        else
        {
            JsonArray IRcodes = json_data["ir_codes"].as<JsonArray>();
            if (!IRcodes.isNull())
            {
                for (JsonVariant IRcode : IRcodes)
                {
                    JsonObject object = IRcode.as<JsonObject>();
                    if (!object.isNull())
                    {
                        if (ir_actions_count < MAX_IR_ACTIONS) {
                            uint64_t code = (unsigned long long)object["code"];
                            const char *action = (const char *)object["action"];

                            ir_actions[ir_actions_count].code = code;
                            ir_actions[ir_actions_count].action = (char*)malloc(strlen(action)+1);
                            strcpy(ir_actions[ir_actions_count].action, action);

                            ir_actions_count++;
                        }
                    }
                }
            }
        }
    }

}
void clockSettings::load()
{
    setDefaults();

    String settingsJSON = drive.load("/settings.json");
    if (settingsJSON != "")
    {
        DeserializationError error = deserializeJson(json_data, (char *)settingsJSON.c_str());
        if (error)
        {
            dbgln(error.f_str());
        }
        else
        {
            JsonObject clock = json_data["clock"].as<JsonObject>();
            if (!clock.isNull())
            {
                clock_settings.clock.useWiFi = (bool)clock["useWiFi"];
                clock_settings.clock.showSeconds = (bool)clock["showSeconds"];
                clock_settings.clock.showHourIndicators = (bool)clock["showHourIndicators"];
                clock_settings.clock.showDigital = (bool)clock["showDigital"];
                clock_settings.clock.autoBrightness = (bool)clock["autoBrightness"];
                clock_settings.clock.orientation = (int)clock["orientation"];
                clock_settings.clock.LEDtype = (uint16_t)clock["LEDtype"];
                clock_settings.clock.LEDcount = (uint8_t)clock["LEDcount"];
                clock_settings.clock.LEDreversed = (uint8_t)clock["LEDreversed"];
            }

            JsonObject alarm = json_data["alarm"].as<JsonObject>();
            if (!alarm.isNull())
            {
                strcpy(clock_settings.alarm.time, (const char *)alarm["time"]);
                clock_settings.alarm.active = (bool)alarm["active"];
            }

            JsonObject colors = json_data["colors"].as<JsonObject>();
            if (!colors.isNull())
            {
                clock_settings.colors.defaultColor = (unsigned long)colors["defaultColor"];
                clock_settings.colors.hourIndicatorColor = (unsigned long)colors["hourIndicatorColor"];
                clock_settings.colors.hourColor = (unsigned long)colors["hourColor"];
                clock_settings.colors.minuteColor = (unsigned long)colors["minuteColor"];
                clock_settings.colors.secondColor = (unsigned long)colors["secondColor"];
            }

            loadIRcodes();
        }
    }
}

clock_settings_struct clockSettings::getClockSettings()
{
    return clock_settings.clock;
}
clock_alarm_struct clockSettings::getAlarmSettings()
{
    return clock_settings.alarm;
}
clock_colors_struct clockSettings::getColorsSettings()
{
    return clock_settings.colors;
}
settings_struct clockSettings::getAllSettings()
{
    return clock_settings;
}
settings_ir_actions* clockSettings::getIRactions()
{
    return ir_actions;
}

void clockSettings::loop()
{
}

void clockSettings::init()
{
    load();
    dirty = false;
    dbgln("Settings initialised");
}

clockSettings::clockSettings(files systemDrive)
{
    ir_actions_count = 0;
    drive = systemDrive;
    dirty = false;

    for (size_t a = 0; a < MAX_IR_ACTIONS; a++)
    {
        ir_actions[a].code = 0;
        ir_actions[a].action = nullptr;
    }
}

clockSettings::~clockSettings()
{
}
