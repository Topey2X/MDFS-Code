#pragma once
#include <Arduino.h>

#include "motor.hpp"
#include "button.hpp"
#include "servo.hpp"

class Arm
{  
public:
  Arm(Servo* flap, Button* limit_bottom, Button* limit_top, Motor* motor);

  void Down();
  void Collect();
  void Up();
  void Deposit();
  void ESTOP();

  Servo* _flap;
  Button* _limit_bottom;
  Button* _limit_top;
  Motor* _motor;

  const int time_down = 350;
  const int time_reverse = 1000;
  const int time_up = 1000;
  const int time_flap = 1000;
};

Arm::Arm(Servo* flap, Button* limit_bottom, Button* limit_top, Motor* motor)
: _flap(flap), _limit_bottom(limit_bottom), _limit_top(limit_top), _motor(motor)
{
}

void Arm::Down() {
  _motor->Down();
  auto ms = millis();

  while((not _limit_bottom->checkPressed()) and (millis()-ms < time_down)) {
    continue;
  }

  _motor->Reverse();
      while((not _limit_bottom->checkPressed()) and (millis()-ms < time_reverse)) {
        continue;
      }

  _motor->Stop();
  ms = millis();
  while((not _limit_bottom->checkPressed()) and (millis()-ms < 1000)) {
    continue;
  }
}

void Arm::Collect() {
  _flap->Close();
  delay(time_flap);
}

void Arm::Up() {
  _motor->Up();
  auto ms = millis();
  while((not _limit_top->checkPressed()) and (millis()-ms < time_up)) {
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

