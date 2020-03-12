#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

#include <menu.h>
#include <menuIO/lcdOut.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include "encoderIn.h"
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>

using namespace Menu;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address

#define encBtn 4 // button
#define encA 5
#define encB 6

Encoder encoder(5, 6);
EncoderStream encStream(encoder, 4);

// Todo: Implement ClickEncoderStream
keyMap encBtn_map[] = {{-encBtn, defaultNavCodes[enterCmd].ch}};
keyIn<1> encButton(encBtn_map);

#define LEDPIN 13
#define MAX_DEPTH 1

unsigned int timeOn = 1000;
unsigned int timeOff = 500;

MENU(mainMenu,
     "LED blink menu",
     Menu::doNothing,
     Menu::noEvent,
     Menu::wrapStyle,
     FIELD(timeOn,
           "On",
           "ms",
           0,
           1000,
           10,
           1,
           Menu::doNothing,
           Menu::noEvent,
           Menu::noStyle),
     FIELD(timeOff,
           "Off",
           "ms",
           0,
           10000,
           10,
           1,
           Menu::doNothing,
           Menu::noEvent,
           Menu::noStyle),
     EXIT("Exit"));

serialIn serial(Serial);
MENU_INPUTS(in, &serial, &encStream, &encButton);

MENU_OUTPUTS(
    out,
    MAX_DEPTH, SERIAL_OUT(Serial),
    LCD_OUT(lcd, {0, 0, 16, 2}));

NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);

void setup()
{
    pinMode(LEDPIN, OUTPUT);
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println("Menu 4.x");
    Serial.println("Use keys + - * /");
    Serial.println("to control the menu navigation");
    lcd.begin(16, 2); // initialize the lcd
}

bool blink(int timeOn, int timeOff) { return millis() % (unsigned long)(timeOn + timeOff) < (unsigned long)timeOn; }

void loop()
{
    nav.poll();
    digitalWrite(LEDPIN, blink(timeOn, timeOff));
}
