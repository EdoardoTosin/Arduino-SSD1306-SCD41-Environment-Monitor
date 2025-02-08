// Include necessary libraries
#include <Wire.h>                 // I2C communication library
#include <Adafruit_GFX.h>          // Graphics library for SSD1306 OLED display
#include <Adafruit_SSD1306.h>      // SSD1306 OLED display driver
#include "SparkFun_SCD4x_Arduino_Library.h"  // SparkFun SCD4x sensor library

// ===================== CONSTANTS =====================

// OLED display configuration
#define SCREEN_WIDTH       128   // OLED display width in pixels
#define SCREEN_HEIGHT      32    // OLED display height in pixels
#define OLED_RESET         -1    // Reset pin for OLED (-1 if using Arduino reset pin)
#define SCREEN_ADDRESS     0x3C  // I2C address for the OLED display

// ===================== PINOUT DETAILS =====================

/*
  SCD41 Sensor Pinout (using hardware I2C):
  VCC      -> 5V (Arduino Micro)        Power for the sensor
  GND      -> GND (Arduino Micro)       Ground for the sensor
  SDA      -> Pin 2 (Arduino Micro)     SDA (data) pin for I2C communication
  SCL      -> Pin 3 (Arduino Micro)     SCL (clock) pin for I2C communication

  SSD1306 OLED Display Pinout (using software I2C):
  VCC      -> 5V (Arduino Micro)        Power for the display
  GND      -> GND (Arduino Micro)       Ground for the display
  SDA      -> Pin 2 (Arduino Micro)     SDA (data) pin for I2C communication
  SCL      -> Pin 3 (Arduino Micro)     SCL (clock) pin for I2C communication

  Note: Both the SSD1306 OLED and the SCD41 sensor communicate over I2C, sharing the SDA and SCL lines.
        Ensure your I2C address for the display is set to 0x3C and confirm the sensor is correctly connected.
*/

// ===================== OBJECT INSTANCES =====================

// Create OLED display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Create SCD4x sensor object
SCD4x mySensor;

// ===================== SETUP =====================

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  
  // Start I2C communication
  Wire.begin();

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true); // Halt execution if the display doesn't initialize
  }

  // Clear the display and show initial splash screen
  display.clearDisplay();
  display.display();
  delay(2000); // Wait for 2 seconds

  // Initialize the SCD4x sensor
  if (!mySensor.begin()) {
    Serial.println(F("Sensor not detected. Please check wiring."));
    while (true); // Halt execution if the sensor isn't detected
  }

  // Stop periodic measurement if it was previously running
  if (mySensor.stopPeriodicMeasurement()) {
    Serial.println(F("Periodic measurement is disabled!"));
  }

  // Enable low power periodic measurement mode
  if (mySensor.startLowPowerPeriodicMeasurement()) {
    Serial.println(F("Low power mode enabled!"));
  }
}

// ===================== MAIN LOOP =====================

void loop() {
  // Check if new data is available from the sensor
  if (mySensor.readMeasurement()) {
    // Retrieve CO2, temperature, and humidity readings
    float co2 = mySensor.getCO2();
    float temperature = mySensor.getTemperature();
    float humidity = mySensor.getHumidity();

    // Print sensor data to the Serial Monitor
    Serial.print(F("CO2 (ppm): "));
    Serial.print(co2);
    Serial.print(F("\tTemperature (°C): "));
    Serial.print(temperature, 1);
    Serial.print(F("\tHumidity (%RH): "));
    Serial.println(humidity, 1);

    // Update the OLED display with the sensor data
    display.clearDisplay();
    display.setTextSize(1);           // Set text size to normal (1:1 pixel scale)
    display.setTextColor(SSD1306_WHITE);  // Set text color to white
    display.setCursor(0, 0);          // Set text cursor at top-left corner

    display.print(F("CO2 (ppm): "));
    display.println(co2);

    display.print(F("Temp (°C): "));
    display.println(temperature, 1);

    display.print(F("Humidity (%): "));
    display.println(humidity, 1);

    display.display();               // Update the OLED display
  } else {
    // If no new data is available, print a dot for debugging
    Serial.print(F("."));
  }

  // Delay between sensor data updates
  delay(1000);  // Wait for 1 second before updating the data
}
