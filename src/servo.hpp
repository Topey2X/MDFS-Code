#pragma once
#include <Arduino.h>

class Servo {
  private:
    int _pin;
    const byte open = 240;
    const byte closed = 60;
    byte _open;
    byte _closed;

  public:
    Servo(int pin, bool reversed) : _pin(pin) {
      _open = reversed ? closed : open;
      _closed = reversed ? open : closed;
      pinMode(_pin, OUTPUT);
      // Open();
      Close();
    };

    void Open() {
      analogWrite(_pin, _open);
    };

    void Close() {
      analogWrite(_pin, _closed);
    };
};