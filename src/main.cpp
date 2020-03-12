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

// LCD
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address

// Rotary encoder
#define ENC_BUTTON 4 // button
#define ENC_A 5
#define ENC_B 6
#define ENC_SENSIVITY 4

Encoder encoder(ENC_A, ENC_B);
EncoderStream encStream(encoder, ENC_SENSIVITY);

// Todo: Implement ClickEncoderStream for click
keyMap encBtn_map[] = {{-ENC_BUTTON, defaultNavCodes[enterCmd].ch}};
keyIn<1> encButton(encBtn_map);

// Piezo buzzer
#define PIN_BEEP 7
#define BEEP_DURATION 125
#define BEEP_PAUSE 60

#define LEDPIN 13
#define MAX_DEPTH 1

void beep(int count, int freq)
{
    for (int i = 0; i < count; i++)
    {
        tone(PIN_BEEP, freq);
        delay(BEEP_DURATION);
        noTone(PIN_BEEP);
        delay(BEEP_PAUSE);
    }
}

// LED variables
unsigned int timeOn = 1000;
unsigned int timeOff = 500;

result showEvent(eventMask e, navNode &nav, prompt &item)
{
    beep(1, 1976);
    return proceed;
}

MENU(mainMenu,
     "LED blink menu",
     showEvent,           // Menu::doNothing,
     Menu::selFocusEvent, // Menu::noEvent,
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

bool blink(int timeOn, int timeOff)
{
    return millis() % (unsigned long)(timeOn + timeOff) < (unsigned long)timeOn;
}

void loop()
{
    nav.poll();
    digitalWrite(LEDPIN, blink(timeOn, timeOff));
}
