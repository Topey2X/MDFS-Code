#pragma once
#include <Arduino.h>

class Button {
  private:
    bool latch = false;
    bool triggered = false;
    unsigned long ms = millis();
    int _pin;

  public:
    Button(int pin) : _pin(pin) {pinMode(_pin, INPUT_PULLUP);};

    bool checkPressed() {
      bool button_state = !digitalRead(_pin);
      if (button_state) {
        if (not triggered) {
          if (not latch) {
            latch = true;
            ms = millis();
          }
          if (millis() - ms > 20) {
            triggered = true;
            return true;
          }
        }
      } else {
        latch = false;
        triggered = false;
      }
      return false;
    }
};