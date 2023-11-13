#pragma once
#include <Arduino.h>

class Motor {
  private:
    int _in1;
    int _in2;
    int _pwm;


    int _speed_reverse = 50;
    int _speed_down = 150;
    int _speed_up = 255;

  public:
    Motor(int in1, int in2, int pwm, int speed_reverse = 50, int speed_down = 150, int speed_up = 255)
    : _in1(in1), _in2(in2), _pwm(pwm), _speed_reverse(speed_reverse), _speed_down(speed_down), _speed_up(speed_up) {
      pinMode(_in1, OUTPUT);
      pinMode(_in2, OUTPUT);
      pinMode(_pwm, OUTPUT);

      digitalWrite(_in1, LOW);
      digitalWrite(_in2, LOW);
      analogWrite(_pwm, 0);
    };

    void Up() {
      digitalWrite(_in1, LOW);
      digitalWrite(_in2, HIGH);
      analogWrite(_pwm, _speed_up);
    };

    void Down() {
      digitalWrite(_in1, HIGH);
      digitalWrite(_in2, LOW);
      analogWrite(_pwm, _speed_down);
    };

    void Reverse() {
      digitalWrite(_in1, LOW);
      digitalWrite(_in2, HIGH);
      analogWrite(_pwm, _speed_reverse);
    }

    void Stop() {
      digitalWrite(_in1, LOW);
      digitalWrite(_in2, LOW);
      analogWrite(_pwm, 0);
    };
};