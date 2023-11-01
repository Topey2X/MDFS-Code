#include <Arduino.h>
#include "servo.hpp"
#include "button.hpp"
#include "motor.hpp"
#include "arm.hpp"
#include "misc.hpp"
#include "steppermotor.hpp"

const int pin_Button = 10;
const int pin_Servo1 = 8;
const int pin_Servo2  = 9;
const int pin_servoDep = 11;

const int pin_Limit1_Up = 24;
const int pin_Limit1_Down = 22;
const int pin_Limit2_Up  = 28;
const int pin_Limit2_Down = 26;

const int pin_Motor1_PWM = 3;
const int pin_Motor1_In1 = 38;
const int pin_Motor1_In2 = 39;

const int pin_Motor2_PWM = 2;
const int pin_Motor2_In1 = 36;
const int pin_Motor2_In2 = 37;

const int pin_Winch1_PWM = 4;
const int pin_Winch1_In1 = 44;
const int pin_Winch1_In2 = 45;

Button button = Button(pin_Button);

// Setup Arms
Button* limit1_up = new Button(pin_Limit1_Up);
Button* limit1_down = new Button(pin_Limit1_Down);
Button* limit2_up = new Button(pin_Limit2_Up);
Button* limit2_down = new Button(pin_Limit2_Down);

Motor* motor1 = new Motor(pin_Motor1_In1,pin_Motor1_In2,pin_Motor1_PWM, 40);
Motor* motor2 = new Motor(pin_Motor2_In1,pin_Motor2_In2,pin_Motor2_PWM, 10);
Motor* winch1 = new Motor(pin_Winch1_In1,pin_Winch1_In2,pin_Winch1_PWM);

Servo* servo1 = new Servo(pin_Servo1, false);
Servo* servo2 = new Servo(pin_Servo2, true);
Servo* depServo = new Servo(pin_servoDep, false);

Arm arm_squash = Arm(servo1, limit1_down, limit1_up, motor1);
Arm arm_tennis = Arm(servo2, limit2_down, limit2_up, motor2);
// StepperMotor nema17 = StepperMotor(pin_Stepper_Step, pin_Stepper_Dir, pin_Stepper_Sleep, pin_Stepper_Reset);


STATE state = STATE::Start;

auto ms = millis();

void loop_manual();
void loop_auto();

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialize serial communication at 9600 baud.
  Serial.begin(9600);

  // Print the message to the serial monitor
  Serial.println("Waiting for command...");
}



void loop() {

  loop_manual();
  // loop_auto();

  // The emergency stop functionality based on button press can remain if you want
  if (button.checkPressed()) {
    arm_tennis.ESTOP();
    arm_squash.ESTOP();
  }
}

void loop_manual() {
  // Check if data is available to read from the serial port
  if (Serial.available()) {
    // Read the incoming byte
    String command = Serial.readStringUntil('\n');

    // Remove any whitespace or newline characters
    command.trim();
    command.toUpperCase();

    // Parse the received command and execute the corresponding function
if (command == "TU") {
  arm_tennis.Up();
} else if (command == "TD") {
  arm_tennis.Down();
} else if (command == "TC") {
  arm_tennis.Collect();
} else if (command == "TL") {
  arm_tennis.Deposit();
} else if (command == "TE") {
  arm_tennis.ESTOP();
} else if (command == "SU") {
  arm_squash.Up();
} else if (command == "SD") {
  arm_squash.Down();
} else if (command == "SC") {
  arm_squash.Collect();
} else if (command == "SL") {
  arm_squash.Deposit();
} else if (command == "SE") {
  arm_squash.ESTOP();
// }else if (command == "NF") {
//     nema17.moveForward(500,4000);
// } else if (command == "NB") {
//     nema17.moveBackward(500,4000);
// } else if (command == "NS") {
//     nema17.stopMotor();
} else if (command == "DC") {
    depServo->Close(); 
} else if (command == "DO"){
    depServo->Open(); 
} else if (command == "W1I") {
  winch1->Up();
  delay(200);
  winch1->Stop();
} else if (command == "W1O") {
  winch1->Down();
  delay(200);
  winch1->Stop();
} else if (command == "W1R") {
  winch1->Up();
  delay(1000);
  winch1->Stop();
} else if (command == "W1E") {
  winch1->Down();
  delay(1000);
  winch1->Stop();
}


 else {
  Serial.println("Unknown command");
}
  }
}

void loop_auto() {
  switch (state) {
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
}