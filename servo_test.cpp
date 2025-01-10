#include <Servo.h>

Servo myServo; // Create a Servo object
const int servoPin = 6; // Pin where the servo is connected
String inputString = ""; // String to hold incoming data
boolean stringComplete = false; // Flag to indicate data reception

void setup() {
  myServo.attach(servoPin); // Attach the servo to the specified pin
  myServo.write(90); // Initialize the servo to 90 degrees (neutral position)
  Serial.begin(9600); // Begin serial communication at 9600 baud
  inputString.reserve(200); // Reserve 200 bytes for the input string
  Serial.println("Send target angle (0-180):");
}

void loop() {
  // Check if a complete string has been received
  if (stringComplete) {
    // Parse and apply the servo angle
    int targetAngle = inputString.toInt(); // Convert string to integer

    // Validate the angle range (0 to 180)
    if (targetAngle >= 0 && targetAngle <= 180) {
      myServo.write(targetAngle); // Move servo to the target angle
      Serial.print("Servo set to: ");
      Serial.println(targetAngle);
    } else {
      Serial.println("Error: Angle out of range (0-180)");
    }

    // Clear the input string and reset the flag
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); // Read the next character

    if (inChar == '\n') {
      // Newline character indicates end of input
      stringComplete = true;
      break; // Exit the loop to process the complete input
    } else {
      // Append the character to the input string
      inputString += inChar;
    }
  }
}