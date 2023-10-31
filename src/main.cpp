#include <Arduino.h>

#include "servo.hpp"
#include "button.hpp"
#include "motor.hpp"
#include "arm.hpp"
#include "misc.hpp"

const int pin_Button = 10;

const int pin_Servo1 = 8;
const int pin_Servo2  = 9;

const int pin_Limit1_Up = 24;
const int pin_Limit1_Down = 22;
const int pin_Limit2_Up  = 28;
const int pin_Limit2_Down = 26;

const int pin_Motor1_PWM = 7;
const int pin_Motor1_In1 = 6;
const int pin_Motor1_In2 = 5;

const int pin_Motor2_PWM = 2;
const int pin_Motor2_In1 = 3;
const int pin_Motor2_In2 = 4;

Button button = Button(pin_Button);

// Setup Arms
Button* limit1_up = new Button(pin_Limit1_Up);
Button* limit1_down = new Button(pin_Limit1_Down);
Button* limit2_up = new Button(pin_Limit2_Up);
Button* limit2_down = new Button(pin_Limit2_Down);

Motor* motor1 = new Motor(pin_Motor1_In1,pin_Motor1_In2,pin_Motor1_PWM, 40);
Motor* motor2 = new Motor(pin_Motor2_In1,pin_Motor2_In2,pin_Motor2_PWM, 10);

Servo* servo1 = new Servo(pin_Servo1, false);
Servo* servo2 = new Servo(pin_Servo2, true);

Arm arm_tennis = Arm(servo1, limit1_down, limit1_up, motor1);
Arm arm_squash = Arm(servo2, limit2_down, limit2_up, motor2);

STATE state = STATE::Start;

auto ms = millis();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  switch(state) {
    case Start:
      while (not button.checkPressed()) {
        blink();
      }
      state = STATE::Arm_Tennis_Down;
      break;

    case Arm_Tennis_Down:
      arm_tennis.Down();
      state = STATE::Arm_Tennis_Collect;
      break;

    case Arm_Tennis_Collect:
      arm_tennis.Collect();
      state = STATE::Arm_Tennis_Up;
      break;

    case Arm_Tennis_Up:
      arm_tennis.Up();
      state = STATE::Arm_Tennis_Deposit;
      break;

    case Arm_Tennis_Deposit:
      arm_tennis.Deposit();
      state = STATE::Tennis_Deliver_PLACEHOLDER;
      break;

    case Tennis_Deliver_PLACEHOLDER:
      delay(1000);
      state = STATE::Arm_Squash_Down;

    case Arm_Squash_Down:
      arm_squash.Down();
      state = STATE::Arm_Squash_Collect;
      break;

    case Arm_Squash_Collect:
      arm_squash.Collect();
      state = STATE::Arm_Squash_Up;
      break;

    case Arm_Squash_Up:
      arm_squash.Up();
      state = STATE::Arm_Squash_Deposit;
      break;

    case Arm_Squash_Deposit:
      arm_squash.Deposit();
      state = STATE::Squash_Deliver_PLACEHOLDER;
      break;

    case Squash_Deliver_PLACEHOLDER:
      state = STATE::End;
      break;

    case End:
      state = STATE::Start;
      break;

    default:
      state = STATE::Start;
  }
  if (button.checkPressed()) {
    state = STATE::Start;
    arm_tennis.ESTOP();
    arm_squash.ESTOP();
  }
}