#pragma once
#include <Arduino.h>

void blink() {
  static auto ms = millis();
  if (millis()-ms > 500) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    ms = millis();
  }
}
