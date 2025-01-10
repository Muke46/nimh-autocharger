#include <Arduino.h>
#include <Servo.h>

// Battery voltage pin
#define BATTERY_VOLTAGE_PIN A0

// Charger status pin
#define CHARGER_STATUS_PIN A5

// New battery switch pin
#define NEW_BATTERY_SWITCH_PIN 3

// Charged battery switch pin
#define CHARGED_BATTERY_SWITCH_PIN 4

// Analog reference voltage
#define ANALOG_REFERENCE_VOLTAGE 5

// Charger enabler pin
#define CHARGER_ENABLER_PIN 5

// Servo pin
#define SERVO_PIN 6

// Load angle
#define LOAD_ANGLE 40

// Charge angle
#define CHARGE_ANGLE 90

// Unload angle
#define UNLOAD_ANGLE 120


// Battery voltage threshold
#define BATTERY_VOLTAGE_THRESHOLD 0.5

// Charger status threshold
#define CHARGER_STATUS_THRESHOLD 400

// Create a Servo object
Servo myServo;

int servo_position;

// Battery measure function
float measureBatteryVoltage() {
  return (float) analogRead(BATTERY_VOLTAGE_PIN) * ANALOG_REFERENCE_VOLTAGE / 1023;
}

// Moves smoothly the servo
void moveServo(int angle) {
  if (angle > servo_position) {
    for (int i = servo_position; i <= angle; i++) {
      myServo.write(i);
      delay(10);
    }
  }
  else {
    for (int i = servo_position; i >= angle; i--) {
      myServo.write(i);
      delay(10);
    }
  }
  servo_position = angle;
}

// Battery load function
void loadBattery() {
  // myServo.write(0);
  moveServo(LOAD_ANGLE);
  delay(1000);
  moveServo(CHARGE_ANGLE);
  delay(1000);
}

// Battery unload function
void unloadBattery() {
  // myServo.write(20);
  myServo.write(UNLOAD_ANGLE);
  delay(2000);
}

void setup() {
  
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Starting...");

  // Set pins directions
  pinMode(BATTERY_VOLTAGE_PIN, INPUT);
  pinMode(CHARGER_STATUS_PIN, INPUT);
  pinMode(NEW_BATTERY_SWITCH_PIN, INPUT);
  pinMode(CHARGED_BATTERY_SWITCH_PIN, INPUT);
  pinMode(CHARGER_ENABLER_PIN, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);

  // Setting switches pull-ups
  pinMode(NEW_BATTERY_SWITCH_PIN, INPUT_PULLUP);
  pinMode(CHARGED_BATTERY_SWITCH_PIN, INPUT_PULLUP);

  // Attach the servo to the pin
  myServo.attach(SERVO_PIN);
  myServo.write(0);
  servo_position = 0;


}

void loop() {

  while(true){

  // TODO add a check if a battery is already loaded (in case of crash or power down during the charge)

  // Wait for a new battery
  Serial.println("Waiting for a new battery...");
  while (digitalRead(NEW_BATTERY_SWITCH_PIN) == HIGH) {
    delay(1000);
  }

  // New battery detected
  Serial.println("New battery detected!");

  // If there is still a charged battery, wait until it is removed
  if(digitalRead(CHARGED_BATTERY_SWITCH_PIN) == HIGH){
    Serial.println("Waiting until the charged battery is removed...");  
  }
  while (digitalRead(CHARGED_BATTERY_SWITCH_PIN) == LOW) {
    delay(1000);
  }

  // Load the battery
  Serial.println("Loading the battery...");
  loadBattery();

  // Check battery presence
  Serial.println("Checking battery presence...");
  
  // Disable the charger
  digitalWrite(CHARGER_ENABLER_PIN, LOW);
  
  // Measure battery voltage
  float batteryVoltage = measureBatteryVoltage();

  if (batteryVoltage < BATTERY_VOLTAGE_THRESHOLD) {
    Serial.println("Battery not present!");
    // TODO do something smart about this, like counting the errors and stop after n retries
    // There may be a dead cell? How to deal with that?
    continue;
  }

  // Battery detected
  Serial.print ("Battery detected with voltage:");
  Serial.print(batteryVoltage);
  Serial.println(" V");

  // Start charging
  Serial.println("Starting charging...");

  // Enable the charger
  digitalWrite(CHARGER_ENABLER_PIN, HIGH);

  // Wait untile the charging starts
  unsigned int timer = 0;
  while (analogRead(CHARGER_STATUS_PIN) > CHARGER_STATUS_THRESHOLD && timer < 5) {
    delay(1000);
    timer++;
  }
  if (timer >= 5) {
    Serial.println("Charging failed to start!");
    unloadBattery();
    continue;
  }


  Serial.println("Charging started!");

  unsigned int charge_timer = 0;
  timer = 0;
  while (true) {
    if (timer > 5) {
      break;
    }
    if (analogRead(CHARGER_STATUS_PIN) > CHARGER_STATUS_THRESHOLD) {
      timer++;
    }
    else {
      timer = 0;
    }
    delay(1000);
    charge_timer++;
    // Serial.print("Charging: Voltage ");
    // Serial.print(measureBatteryVoltage());
    // Serial.print(" V, Charger status: ");
    // Serial.print(analogRead(CHARGER_STATUS_PIN));
    // Serial.print(", Timer: ");
    // Serial.print(charge_timer);
    // Serial.println(" s");
  
  }

  // Charging finished
  Serial.print("Charging finished!");

  // Print stats like charge time and capacity charged?
  Serial.print(" Charged in ");
  Serial.print(charge_timer);
  Serial.println(" s");  

  // Check if the battery can be removed
  if(digitalRead(CHARGED_BATTERY_SWITCH_PIN) == HIGH){
    Serial.println("Waiting until there is space to remove the battery...");  
  }
  while (digitalRead(CHARGED_BATTERY_SWITCH_PIN) == LOW) {
    delay(1000);
  }

  // Unload the battery
  Serial.println("Unloading the battery...");
  unloadBattery();

  }

}