#include <AccelStepper.h>
#include <Wire.h>
#include <RTClib.h>

// Define the pins for the stepper motor
#define STEPPER_PIN1 12
#define STEPPER_PIN2 11
#define STEPPER_PIN3 10
#define STEPPER_PIN4 9

// Define the pins for the limit switch
#define LIMIT_SWITCH_PIN 8

// Define the time to wait before moving the stepper motor
#define WAIT_TIME 120000 // 2 minutes in milliseconds

// Create an instance of the AccelStepper library for the stepper motor
AccelStepper stepper(AccelStepper::FULL4WIRE, STEPPER_PIN1, STEPPER_PIN2, STEPPER_PIN3, STEPPER_PIN4);

// Create an instance of the RTC library for the real-time clock
RTC_DS3231 rtc;

void setup() {
  // Initialize the limit switch pin as an input
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);

  // Initialize the stepper motor
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);

  // Initialize the serial communication for debugging purposes
  Serial.begin(9600);

  // Initialize the real-time clock
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set the time to the compile time
}

void loop() {
  // Check if the limit switch is pressed
  if (digitalRead(LIMIT_SWITCH_PIN) == LOW) {
    Serial.println("Limit switch pressed. Motor stopped.");
    stepper.stop();
  } else {
    // Get the current time from the real-time clock
    DateTime now = rtc.now();

    // Check if the time has passed the waiting period
    if (now.unixtime() >= (rtc.now().unixtime() + WAIT_TIME / 1000)) {
      Serial.println("Waiting period over. Motor moving.");
      stepper.moveTo(1000);

      // Move the motor until the limit switch is pressed
      while (digitalRead(LIMIT_SWITCH_PIN) == HIGH) {
        stepper.run();
      }

      // Stop the motor when the limit switch is pressed
      stepper.stop();
      Serial.println("Limit switch pressed. Motor stopped.");
    }
  }
}