/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Dotmatrix display management
*
* Created by       :        Arnold Velzel
* Created on       :        08-09-2021
*
*******************************************************************************/
#include "avision_system.h"

// https://github.com/MajicDesigns/MD_MAX72XX
#include <SPI.h>
#include <MD_MAX72xx.h>

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

// https://pjrp.github.io/MDParolaFontEditor
#include "font_with_tiny_numbers.h"

// Define the number of devices we have in the chain and the hardware interface
#ifndef HARDWARE_TYPE
#define HARDWARE_TYPE MD_MAX72XX::DR1CR0RR0_HW
#endif

#define CLK_PIN         D5  // or SCK
#define DATA_PIN        D7  // or MOSI
#define CS_PIN          D8  // or CS

#define CHAR_SPACING    1 // pixels between characters

namespace AVision
{
    class clockDisplay
    {
        private:

            MD_MAX72XX *display;
            uint8_t display_brightness;

            void showText(uint8_t start, uint8_t end, char* text);
            int display_rows;
            int display_columns;

        public:
            String text; // Use ▼ to use tiny numbers and ▲ to use normal sized numbers

            void showDot(uint8_t row, uint16_t column);
            void hideDot(uint8_t row, uint16_t column);
            void flashDot(uint8_t row, uint16_t column, int on, int off);
            void invert(uint8_t start, uint8_t end);
            void shiftLeft();

            void setIntensity(uint8_t intensity, uint8_t start, uint8_t end);
            void setIntensity(uint8_t intensity); // whole display
            void setFont(MD_MAX72XX::fontType_t *f);
            void loop();
            void init(int columns, int rows);

            clockDisplay();
            ~clockDisplay();
    };

}

#endif
