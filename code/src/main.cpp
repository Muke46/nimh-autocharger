#include <Arduino.h>

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

// Battery voltage threshold
#define BATTERY_VOLTAGE_THRESHOLD 0.5

// Charger status threshold
#define CHARGER_STATUS_THRESHOLD 400

// Battery measure function
float measureBatteryVoltage() {
  return (float) analogRead(BATTERY_VOLTAGE_PIN) * ANALOG_REFERENCE_VOLTAGE / 1023;
}

// Battery load function
void loadBattery() {
  // TODO
}

// Battery unload function
void unloadBattery() {
  // TODO
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

  // Setting switches pull-ups
  pinMode(NEW_BATTERY_SWITCH_PIN, INPUT_PULLUP);
  pinMode(CHARGED_BATTERY_SWITCH_PIN, INPUT_PULLUP);

}

void loop() {

  while(true){

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
  while (analogRead(CHARGER_STATUS_PIN) > CHARGER_STATUS_THRESHOLD) {
    delay(1000);
    // TODO put a watchdog timer here!
  }

  Serial.println("Charging started!");

  while (analogRead(CHARGER_STATUS_PIN) < CHARGER_STATUS_THRESHOLD) {
    delay(1000);
    Serial.print("Charging: Voltage ");
    Serial.print(measureBatteryVoltage());
    Serial.print(" V, Charger status: ");
    Serial.println(analogRead(CHARGER_STATUS_PIN));
  }

  // Charging finished
  Serial.println("Charging finished!");
  // Print stats like charge time and capacity charged?

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