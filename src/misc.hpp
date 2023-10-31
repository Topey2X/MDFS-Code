#pragma once
#include <Arduino.h>

enum STATE {
  Start,
  Arm_Tennis_Down,
  Arm_Tennis_Collect,
  Arm_Tennis_Up,
  Arm_Tennis_Deposit,
  Tennis_Deliver_PLACEHOLDER,
  Arm_Squash_Down,
  Arm_Squash_Collect,
  Arm_Squash_Up,
  Arm_Squash_Deposit,
  Squash_Deliver_PLACEHOLDER,
  End,
};

void blink() {
  static auto ms = millis();
  if (millis()-ms > 500) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    ms = millis();
  }
}