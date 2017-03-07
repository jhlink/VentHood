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

#include "AnalogInputDebounced.h"

template <typename T>
bool AnalogInputDebounced::checkInRange(const T& valueToCheck, const T& lowerBound, const T& upperBound) {
  return (valueToCheck >= lowerBound) && (valueToCheck <= upperBound);
}

AnalogInputDebounced::AnalogInputDebounced(void) : m_prevState(false), m_inputState(false), m_count(0), m_pin(-1), m_voltagePoint(-1), longPressTimeout(0) {}

AnalogInputDebounced::AnalogInputDebounced(int pin, int targetVoltage) : m_prevState(false), m_inputState(false), m_count(0), m_pin(pin), m_voltagePoint(targetVoltage), longPressTimeout(2000)
{}

AnalogInputDebounced::AnalogInputDebounced(int pin, int targetVoltage, long longPressLength) : m_prevState(false), m_inputState(false), m_count(0), m_pin(pin), m_voltagePoint(targetVoltage), longPressTimeout(longPressLength)
{}

void AnalogInputDebounced::updateInput(void) {
  static unsigned long prevTime = millis();

  if ((millis() - prevTime) < UPDATETIME) {
    return;
  } else {
    prevTime = millis();
  }

  int currentAnalogReading = analogRead(m_pin);
  int lowerBoundInclusive = m_voltagePoint - VOLTAGE_TOLERANCE_RANGE;
  int upperBoundInclusive = m_voltagePoint + VOLTAGE_TOLERANCE_RANGE;

  if (checkInRange(currentAnalogReading, lowerBoundInclusive, upperBoundInclusive)) {
    if (m_count < DEBOUNCE_COUNT) {
      m_count++;
    } else {
      m_inputState = true;
    }
  } else {
    if (m_count > 0) {
      m_count--;
    } else {
      m_inputState = false;
    }
  }
}

bool AnalogInputDebounced::isUniquelyActive(void) {
  if ((m_inputState == true) && (m_prevState != m_inputState)) {
    m_prevState = m_inputState;
    longPressTimeout = millis();
    return true;
  } else if ((m_inputState == false) && (m_prevState != m_inputState)) {
    m_prevState = m_inputState;
  }
  return false;
}

bool AnalogInputDebounced::isLongPressed(void) {
  unsigned long pastTime = 0;
  if ((m_inputState == true) && (m_prevState != m_inputState) &&
      ((millis() - pastTime) > LONG_PRESS_TIMEOUT)) {
      m_prevState = m_inputState;
    return true;
  } else {
      longPressTimeout = millis();
  }
  return false;


  //if ((millis() - prevTime) < UPDATETIME) {
  //  return;
  //} else {
  //  prevTime = millis();
  //}
}

bool AnalogInputDebounced::isActive(void) {
  return m_inputState;
}
