#include <Arduino.h>
#include "servo.hpp"
#include "button.hpp"
#include "motor.hpp"
#include "arm.hpp"
#include "misc.hpp"
#include "steppermotor.hpp"

const int pin_Button = 10;
const int pin_Button_GND = 12;
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
const int pin_Winch2_In1 = 29;
const int pin_Winch2_In2 = 28;

const int stepPin = A4; 
const int dirPin = A3;  
const int enPin = A0;   
const int resetPin = A1;
const int sleepPin = A2;
const int stepsPerRevolution = 200;

Button button = Button(pin_Button);

// Setup Arms
Button* squash_Limit_up = new Button(pin_Squash_Limit_Up);
Button* squash_Limit_down = new Button(pin_Squash_Limit_Down);
Button* tennis_limit_up = new Button(pin_Tennis_Limit_Up);
Button* tennis_limit_down = new Button(pin_Tennis_Limit_Down);

Motor* squash_motor = new Motor(pin_Squash_Motor_In1,pin_Squash_Motor_In2,pin_Squash_Motor_PWM, 40, 150, 230);
Motor* tennis_motor = new Motor(pin_Tennis_Motor_In1,pin_Tennis_Motor_In2,pin_Tennis_Motor_PWM);
Motor* winch1 = new Motor(pin_Winch1_In1,pin_Winch1_In2,pin_Winch1_PWM, 50, 50, 160);
Motor* winch2 = new Motor(pin_Winch2_In1, pin_Winch2_In2, pin_Winch2_PWM, 50, 50, 255);


Servo* squash_servo = new Servo(pin_Squash_Servo, false);
Servo* tennis_servo = new Servo(pin_Tennis_Servo, true);
Servo* depServo = new Servo(pin_servoDep, false);

Arm arm_squash = Arm(squash_servo, squash_Limit_down, squash_Limit_up, squash_motor);
Arm arm_tennis = Arm(tennis_servo, tennis_limit_down, tennis_limit_up, tennis_motor, 300, 700);
// StepperMotor nema17 = StepperMotor(pin_Stepper_Step, pin_Stepper_Dir, pin_Stepper_Sleep, pin_Stepper_Reset);

auto ms = millis();

void loop_manual();
void loop_auto();

byte state = 0;

void rotateStepper(int speed, double revolutions, bool clockwise) {
  digitalWrite(enPin, LOW); // Enable the stepper driver
  digitalWrite(dirPin, clockwise ? HIGH : LOW); // Set direction based on the parameter

  int delayTime = 1000000 / (speed * 2); // Calculate delay time in microseconds
  int totalSteps = floor(revolutions*stepsPerRevolution);
  for (int step = 0; step < totalSteps; step++) { // Assuming 200 steps per revolution
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayTime);
  }
  digitalWrite(enPin, HIGH);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pin_Button_GND, OUTPUT);
  digitalWrite(pin_Button_GND, LOW);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);  
  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, HIGH);
  pinMode(sleepPin, OUTPUT);
  digitalWrite(sleepPin, HIGH);
  
  // Initialize serial communication at 115200 baud.
  Serial.begin(115200);

  // Print the message to the serial monitor
  Serial.println("Waiting for command...");
}



void loop() {
  // manual
  //loop_manual();

  // autu
  loop_auto();


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
  } else if (command == "TUI") {
    const auto flap = 300;
    const auto slow = 100;
    const auto total = 700;
    arm_tennis._motor->Up(255);
    delay(slow);
    arm_tennis._motor->Up(100);
    delay(flap);
    arm_tennis._flap->Open();
    delay(total-flap-slow);
    arm_tennis._motor->Stop();
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
  } else if (command == "SUI") {
    arm_squash._motor->Up();
    delay(400);
    arm_squash._flap->Open();
    delay(300);
    arm_squash._motor->Stop();

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
      winch1->Up(120); // Move winch1 up
      delay(argument); // Delay for the specified time in the argument
      winch1->Stop(); // Stop winch1 movement
    }

    // Winch2 Commands
    else if (command == "WTD") {
      winch2->Down(); // Move winch2 down
      delay(argument); // Delay for the specified time in the argument
      winch2->Stop(); // Stop winch2 movement
    } else if (command == "WTU") {
      winch2->Up(255); // Move winch2 up
      delay(argument); // Delay for the specified time in the argument
      winch2->Stop(); // Stop winch2 movement
    }
    else if (command == "WRI") {
      rotateStepper(argument, 5, true);
    }
    else if (command == "WRO") {
      rotateStepper(argument, 5, false);
    } else {
      // Print an error message if an unknown command is received
      Serial.println("Unknown command");
    }
  }
}


void loop_auto() {
  switch (state++) {
    case 0: // Open both flaps
      while (!(button.checkPressed())) {
        blink();
        loop_manual();
      }
      arm_tennis.Deposit();
      arm_squash.Deposit();
      break;

    case 1: // Lower both arms
      arm_tennis.Down();
      arm_squash.Down();
      break;

    case 2: // Collect squash balls
      arm_squash.Collect();
      delay(1000);
      break;

    case 3: // Raise squash arm
      arm_squash._motor->Up();
      delay(400);
      arm_squash._flap->Open();
      delay(300);
      arm_squash._motor->Up(80);
      delay(300);
      arm_squash._motor->Stop();
      delay(2000);
      break;

    case 4: // Extend squash slider
      arm_squash.Down();
      delay(200);
      winch1->Up(200);
      delay(3000);
      winch1->Stop();
      delay(3000);
      break;

    case 5: // Collect tennis balls
      arm_tennis.Collect();
      delay(1000);
      break;
    
    case 6: // Raise tennis arm
      arm_tennis._motor->Up(255);
      delay(300);
      arm_tennis._motor->Up(100);
      delay(100);
      arm_tennis._flap->Open();
      delay(300);
      arm_tennis._motor->Stop();
      delay(2000);
      break;
    
    case 7: // Extend tennis slider
      arm_tennis.Down();
      delay(200);
      winch2->Up(255);
      delay(6000);
      winch2->Stop();
      break;

    case 8: // Retraction
      delay(3000);
      winch1->Down();
      winch2->Down();
      rotateStepper(200, 7, false);
      delay(2000);
      winch1->Stop();
      winch2->Stop();
      arm_tennis.Up();
      arm_squash.Up();

    default:
      state = 0;
    
  }
}