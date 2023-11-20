#include <Arduino.h>
#include "servo.hpp"
#include "button.hpp"
#include "motor.hpp"
#include "arm.hpp"
#include "misc.hpp"
#include "steppermotor.hpp"

const int pin_Button = 10;
const int pin_Squash_Servo = 9;
const int pin_Tennis_Servo  = 8;
const int pin_servoDep = 11;

const int pin_Squash_Limit_Up = 24;
const int pin_Squash_Limit_Down = 22;
const int pin_Tennis_Limit_Up  = 28;
const int pin_Tennis_Limit_Down = 26;

const int pin_Squash_Motor_PWM = 2; // Squash is correct. 
const int pin_Squash_Motor_In1 = 39;
const int pin_Squash_Motor_In2 = 37;

const int pin_Tennis_Motor_PWM = 3; // tennis correct 
const int pin_Tennis_Motor_In1 = 38;
const int pin_Tennis_Motor_In2 = 36;

const int pin_Winch1_PWM = 4; // winch 1 squash correct 
const int pin_Winch1_In1 = 30;
const int pin_Winch1_In2 = 31;

const int pin_Winch2_PWM = 5; // winch 2 tennis correct 
const int pin_Winch2_In1 = 28;
const int pin_Winch2_In2 = 29;

Button button = Button(pin_Button);

// Setup Arms
Button* squash_Limit_up = new Button(pin_Squash_Limit_Up);
Button* squash_Limit_down = new Button(pin_Squash_Limit_Down);
Button* tennis_limit_up = new Button(pin_Tennis_Limit_Up);
Button* tennis_limit_down = new Button(pin_Tennis_Limit_Down);

Motor* squash_motor = new Motor(pin_Squash_Motor_In1,pin_Squash_Motor_In2,pin_Squash_Motor_PWM, 40, 150, 200);
Motor* tennis_motor = new Motor(pin_Tennis_Motor_In1,pin_Tennis_Motor_In2,pin_Tennis_Motor_PWM, 10);
Motor* winch1 = new Motor(pin_Winch1_In1,pin_Winch1_In2,pin_Winch1_PWM, 50, 50, 150);
Motor* winch2 = new Motor(pin_Winch2_In1, pin_Winch2_In2, pin_Winch2_PWM, 50, 50, 150);


Servo* squash_servo = new Servo(pin_Squash_Servo, false);
Servo* tennis_servo = new Servo(pin_Tennis_Servo, true);
Servo* depServo = new Servo(pin_servoDep, false);

Arm arm_squash = Arm(squash_servo, squash_Limit_down, squash_Limit_up, squash_motor);
Arm arm_tennis = Arm(tennis_servo, tennis_limit_down, tennis_limit_up, tennis_motor, 350, 800);
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
  // Check if there is data available to read from the serial port
  if (Serial.available()) {
    // Read the incoming command line until a newline character is encountered
    String commandLine = Serial.readStringUntil('\n');
    // Remove any leading and trailing whitespace or newline characters
    commandLine.trim();
    // Convert the command line to uppercase for consistent command handling
    commandLine.toUpperCase();

    // Find the position of the left parenthesis '(' in the command line
    int leftParen = commandLine.indexOf('(');
    // Find the position of the right parenthesis ')' in the command line
    int rightParen = commandLine.indexOf(')');
    // Initialize 'command' with the full command line
    String command = commandLine;
    // Set a default argument value for delay, used if no argument is provided
    int argument = 200;
    // Check if both parentheses are found in the command line
    if (leftParen != -1 && rightParen != -1) {
      // Extract the command part (before the left parenthesis)
      command = commandLine.substring(0, leftParen);
      // Extract the argument part (between the parentheses) and convert it to an integer
      argument = commandLine.substring(leftParen + 1, rightParen).toInt();
    }

    // Parse the received command and execute the corresponding function
if (command == "TU") {
  arm_tennis.Up();
} else if (command == "TD") {
  arm_tennis.Down();
} else if (command == "TC") {
  arm_tennis.Collect();
} else if (command == "TR") {
  arm_tennis.Deposit();
} else if (command == "TE") {
  arm_tennis.ESTOP();
} else if (command == "SU") {
  arm_squash.Up();
} else if (command == "SUL") {
  arm_squash.Deposit();
  arm_squash.Up();
} else if (command == "SD") {
  arm_squash.Down();
} else if (command == "SC") {
  arm_squash.Collect();
} else if (command == "SR") {
  arm_squash.Deposit();
} else if (command == "SE") {
  arm_squash.ESTOP();
} else if (command == "DC") {
    depServo->Close(); 
} else if (command == "DO"){
    depServo->Open(); 
} else if (command == "WSD") {
      winch1->Down(); // Move winch1 down
      delay(argument); // Delay for the specified time in the argument
      winch1->Stop(); // Stop winch1 movement
    } else if (command == "WSU") {
      winch1->Up(); // Move winch1 up
      delay(argument); // Delay for the specified time in the argument
      winch1->Stop(); // Stop winch1 movement
    }

    // Winch2 Commands
    else if (command == "WTD") {
      winch2->Down(); // Move winch2 down
      delay(argument); // Delay for the specified time in the argument
      winch2->Stop(); // Stop winch2 movement
    } else if (command == "WTU") {
      winch2->Up(); // Move winch2 up
      delay(argument); // Delay for the specified time in the argument
      winch2->Stop(); // Stop winch2 movement
    } else {
      // Print an error message if an unknown command is received
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