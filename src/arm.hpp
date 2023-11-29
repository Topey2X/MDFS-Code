#pragma once
#include <Arduino.h>

#include "motor.hpp"
#include "button.hpp"
#include "servo.hpp"

class Arm
{  
public:
  Servo* _flap;
  Button* _limit_bottom;
  Button* _limit_top;
  Motor* _motor;

  const unsigned long _time_down;
  const unsigned long _time_up;
  const unsigned long _time_reverse;
  const unsigned long time_flap = 1000;
  
  Arm(Servo* flap, Button* limit_bottom, Button* limit_top, Motor* motor, int time_down = 350, int time_up = 800, int time_reverse = 1000);
  void Down();
  void Collect();
  void Up();
  void Deposit();
  void ESTOP();
};

Arm::Arm(Servo* flap, Button* limit_bottom, Button* limit_top, Motor* motor, int time_down, int time_up, int time_reverse)
: _flap(flap), _limit_bottom(limit_bottom), _limit_top(limit_top), _motor(motor), _time_down(time_down), _time_up(time_up), _time_reverse(time_reverse)
{
}

void Arm::Down() {
  _motor->Down();
  auto ms = millis();

  while(millis()-ms < _time_down) {
    continue;
  }
  ms = millis();

  _motor->Reverse();
      while(millis()-ms < _time_reverse) {
        continue;
      }

  _motor->Stop();
}

void Arm::Collect() {
  _flap->Close();
  delay(time_flap);
}

void Arm::Up() {
  _motor->Up();
  auto ms = millis();
  while(millis()-ms < _time_up) {
    continue;
  }
  _motor->Stop();
}

void Arm::Deposit() {
  _flap->Open();
  delay(time_flap);
}

void Arm::ESTOP() {
  _flap->Open();
  _motor->Stop();
}

