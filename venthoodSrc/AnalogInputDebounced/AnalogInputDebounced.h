/**
 * Copyright (c) 2016 FirstBuild
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "application.h"

#ifndef ANALOG_INPUT_DEBOUNCED_H
#define ANALOG_INPUT_DEBOUNCED_H

#define DEBOUNCE_COUNT 2
#define DEBOUNCE_TIME 10
#define VOLTAGE_TOLERANCE_RANGE 100

class AnalogInputDebounced {
  public:
    AnalogInputDebounced(void);
    AnalogInputDebounced(int pin, int targetVoltage);
    void updateInput(void);
    void setLongPressedTimeout(unsigned long timeLimit);
    bool onLongPressed(void);
    bool onPress(void);
    bool onRelease(void);

  private:
    bool m_prevRawState;
    bool m_inputRawState;
    bool m_currentKnownState;
    int m_count;
    int m_pin;
    int m_voltagePoint;
    unsigned long m_longPressTimeout;
    unsigned long m_lastDebounceTime;
    bool uniquelyPressed;
    bool uniquelyReleased;
    bool uniquelyLongPressed;

    template <typename T>
      bool checkInRange(const T& valueToCheck, const T& lowerBound, const T& upperBound);

    bool validateButtonVoltage();

};

#endif
