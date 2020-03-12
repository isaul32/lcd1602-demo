#ifndef ENCODER_IN_H
#define ENCODER_IN_H

#include <Arduino.h>

#ifndef ARDUINO_SAM_DUE

#include <Encoder.h>

#include <menuDefs.h>

namespace Menu
{

class EncoderStream : public menuIn
{
public:
    Encoder &enc;
    int8_t sensivity;
    int oldPos;
    int pos;

    inline void update()
    {
        pos = enc.read();
    }

    EncoderStream(Encoder &enc, int sensivity)
        : enc(enc),
          sensivity(sensivity),
          oldPos(0),
          pos(0)
    {
        pos = enc.read();
    }

    inline void setSensivity(int s)
    {
        sensivity = s;
    }

    int available(void)
    {
        return peek() != -1;
    }

    int peek(void)
    {
        update();
        int d = pos - oldPos;
        if (d <= -sensivity)
            return options->navCodes[upCmd].ch;
        if (d >= sensivity)
            return options->navCodes[downCmd].ch;
        return -1;
    }

    int read()
    {
        int ch = peek();
        if (ch == options->navCodes[downCmd].ch)
            oldPos += sensivity;
        else if (ch == options->navCodes[upCmd].ch)
            oldPos -= sensivity;
        return ch;
    }

    void flush()
    {
        update();
        oldPos = pos;
    }

    size_t write(uint8_t v)
    {
        oldPos = v;
        return 1;
    }
};
} // namespace Menu

#endif

#endif
