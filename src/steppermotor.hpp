#pragma once
#include <Arduino.h>

class StepperMotor {
private:
    const int stepPin;
    const int dirPin;
    const int sleepPin;
    const int resetPin;

public:
    StepperMotor(int step, int dir, int sleep, int reset) 
      : stepPin(step), dirPin(dir), sleepPin(sleep), resetPin(reset) {
        pinMode(stepPin, OUTPUT);
        pinMode(dirPin, OUTPUT);
        pinMode(sleepPin, OUTPUT);
        pinMode(resetPin, OUTPUT);
    }

    void moveForward(int steps, int delay) {
        enableDriver();
        digitalWrite(dirPin, HIGH);
        for (int i = 0; i < steps; i++) {
            digitalWrite(stepPin, HIGH);
            delayMicroseconds(delay);
            digitalWrite(stepPin, LOW);
            delayMicroseconds(delay);
        }
        disableDriver();
    }

    void moveBackward(int steps, int delay) {
        enableDriver();
        digitalWrite(dirPin, LOW);
        for (int i = 0; i < steps; i++) {
            digitalWrite(stepPin, HIGH);
            delayMicroseconds(delay);
            digitalWrite(stepPin, LOW);
            delayMicroseconds(delay);
        }
        disableDriver();
    }

    void stopMotor() {
        disableDriver();
    }

private:
    void enableDriver() {
        digitalWrite(sleepPin, HIGH);
        digitalWrite(resetPin, HIGH);
    }

    void disableDriver() {
        digitalWrite(sleepPin, LOW);
        digitalWrite(resetPin, LOW);
    }
};
