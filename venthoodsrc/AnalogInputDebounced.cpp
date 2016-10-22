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

#define DEBOUNCE_COUNT 20

template <typename T>
bool AnalogInputDebounced::checkInRange(const T& valueToCheck, const T& lowerBound, const T& upperBound) {
    return !(valueToCheck < lowerBound) && (valueToCheck <= upperBound);
}


int AnalogInputDebounced::analogVoltageValue(float inputVolt) {
    int analogResult = (int) ((4095 / 5.0) * inputVolt) ;
    return analogResult;
}

AnalogInputDebounced::AnalogInputDebounced(int pin, float targetVoltage) {
    m_pin = pin;
    m_prevState = false;
    m_inputState = false;
    m_count = 0;
    m_voltagePoint = analogVoltageValue(targetVoltage);
    m_toleranceVoltageRange = 300;
}

void AnalogInputDebounced::setVoltToleranceRange(int newBoundary) {
    m_toleranceVoltageRange = newBoundary;
}

void AnalogInputDebounced::updateInput(void) {
    int currentAnalogReading = analogRead(m_pin);
    int lowerBoundInclusive = m_voltagePoint - m_toleranceVoltageRange;
    int upperBoundInclusive = m_voltagePoint + m_toleranceVoltageRange;

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
    if (m_inputState == true && m_prevState != m_inputState) {
        m_prevState = m_inputState;
        Serial.println("Diving deep");
        return true;
    }
    return false;
}

bool AnalogInputDebounced::isActive(void) {
    return m_inputState;
}
