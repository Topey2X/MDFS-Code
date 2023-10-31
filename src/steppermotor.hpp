#pragma once
#include <Arduino.h>

class StepperMotor {
private:
    const int stepPin;
    const int dirPin;
    const int sleepPin;
    const int resetPin;
    const int speed; // In microseconds

public:
    StepperMotor(int step, int dir, int sleep, int reset, int speed = 800) 
      : stepPin(step), dirPin(dir), sleepPin(sleep), resetPin(reset), speed(speed) {
        pinMode(stepPin, OUTPUT);
        pinMode(dirPin, OUTPUT);
        pinMode(sleepPin, OUTPUT);
        pinMode(resetPin, OUTPUT);
    }

    void moveForward(int steps) {
        enableDriver();
        digitalWrite(dirPin, HIGH);
        for (int i = 0; i < steps; i++) {
            digitalWrite(stepPin, HIGH);
            delayMicroseconds(speed);
            digitalWrite(stepPin, LOW);
            delayMicroseconds(speed);
        }
        disableDriver();
    }

    void moveBackward(int steps) {
        enableDriver();
        digitalWrite(dirPin, LOW);
        for (int i = 0; i < steps; i++) {
            digitalWrite(stepPin, HIGH);
            delayMicroseconds(speed);
            digitalWrite(stepPin, LOW);
            delayMicroseconds(speed);
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
