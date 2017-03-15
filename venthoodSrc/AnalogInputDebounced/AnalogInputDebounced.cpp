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

AnalogInputDebounced::AnalogInputDebounced(void) : m_prevRawState(false), m_inputRawState(false), m_currentKnownState(false), m_pin(-1), m_voltagePoint(-1), m_longPressTimeout(2000), m_lastDebounceTime(millis()), uniquelyPressed(false), uniquelyReleased(false), uniquelyLongPressed(false) {}

AnalogInputDebounced::AnalogInputDebounced(int pin, int targetVoltage) : m_prevRawState(false), m_inputRawState(false), m_currentKnownState(false), m_pin(pin), m_voltagePoint(targetVoltage), m_longPressTimeout(2000), m_lastDebounceTime(millis()), uniquelyPressed(false), uniquelyReleased(false), uniquelyLongPressed(false) {}

bool AnalogInputDebounced::validateButtonVoltage() {
  int currentAnalogReading = analogRead(m_pin);
  int lowerBoundInclusive = m_voltagePoint - VOLTAGE_TOLERANCE_RANGE;
  int upperBoundInclusive = m_voltagePoint + VOLTAGE_TOLERANCE_RANGE;

  bool result = checkInRange(currentAnalogReading, lowerBoundInclusive, upperBoundInclusive);

  return result;
}

void AnalogInputDebounced::updateInput(void) {
  unsigned long currentMilliTime = millis();
  m_inputRawState = validateButtonVoltage();

  if (m_inputRawState != m_prevRawState) {
    m_lastDebounceTime = currentMilliTime;
  } 
    
  if ((currentMilliTime - m_lastDebounceTime) > DEBOUNCE_TIME) {
    m_currentKnownState = m_inputRawState;
  }

  //if (m_inputRawState) {
  //  if (m_count < DEBOUNCE_COUNT) {
  //    m_count++;
  //  } else {
  //    m_inputRawState = true;
  //  }
  //} else {
  //  if (m_count > 0) {
  //    m_count--;
  //  } else {
  //    m_inputRawState = false;
  //  }
  //}
  
  m_prevRawState = m_inputRawState;
}

void AnalogInputDebounced::setLongPressedTimeout(unsigned long timeLimit) {
  m_longPressTimeout = timeLimit;
}

bool AnalogInputDebounced::onPress(void) {
  unsigned long currentMilliTime = millis();
  if (m_currentKnownState && !uniquelyPressed) {
    return uniquelyPressed = true;
  } else if (!m_currentKnownState && uniquelyPressed) {
    uniquelyPressed = false;
  }
  return false;
}

bool AnalogInputDebounced::onRelease(void) {
  unsigned long currentMilliTime = millis();
  if (!m_currentKnownState  && !uniquelyReleased) {
      return uniquelyReleased = true;
  } else if (m_currentKnownState && uniquelyReleased) {
    uniquelyReleased = false;
  }
  return false;
}

bool AnalogInputDebounced::onLongPressed(void) {
  static unsigned long timertest = millis();
  if ((millis() - timertest) > 3000) {
    Serial.println("Starting state");
    Serial.println(m_currentKnownState);
    Serial.println((millis() - m_lastDebounceTime) > m_longPressTimeout);
    timertest = millis();
  }
  if (m_currentKnownState && !uniquelyLongPressed && ((millis() - m_lastDebounceTime) > m_longPressTimeout)) {
    return uniquelyLongPressed = true;
  } else if (!m_currentKnownState && uniquelyLongPressed) {
    uniquelyLongPressed = false;
  }
  return false;
}
