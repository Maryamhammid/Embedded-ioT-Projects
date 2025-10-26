/*
  Project Title : Single Button with Press-Type Detection and OLED Display
  Name          : Maryam Hamid
  Reg No        : 23-NTU-CS-1046
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------------- Pin Configuration ----------------------
const int BUTTON_PIN = 32;   // Push button input pin (active LOW)
const int LED_PIN = 16;      // LED output pin
const int BUZZER_PIN = 27;   // Buzzer output pin


// ---------------------- OLED Configuration ----------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------------------- Timing Constants ----------------------
const unsigned long LONG_PRESS_THRESHOLD = 1500;  // Duration (ms) for a long press
const unsigned long DEBOUNCE_DELAY = 50;          // Debounce delay (ms)

// ---------------------- State Variables ----------------------
unsigned long buttonPressStartTime = 0;  // Timestamp of when button was pressed
bool buttonPressed = false;               // Flag indicating if button is currently pressed
bool ledState = false;                    // Tracks current LED state (ON/OFF)
bool longPressTriggered = false;          // Indicates if long press has been detected


void displayMessage(const char *message) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(message);
  display.display();
}


void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Use internal pull-up resistor
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Wire.begin(21, 22);  // SDA = GPIO21, SCL = GPIO22 for ESP32
  Serial.begin(115200);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ OLED initialization failed!");
    while (true);  // Halt execution if OLED not detected
  }

  // Display initial message
  displayMessage("Ready");

  // Initialize outputs
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);

  Serial.println("System initialized. Waiting for button input...");
}


void loop() {
  int currentButtonState = digitalRead(BUTTON_PIN);
  unsigned long currentTime = millis();

  // ---------------------- Button Press Detection ----------------------
  if (currentButtonState == LOW && !buttonPressed) {
    // Debounce check
    delay(DEBOUNCE_DELAY);
    if (digitalRead(BUTTON_PIN) == LOW) {
      buttonPressed = true;
      buttonPressStartTime = currentTime;
      longPressTriggered = false;
      Serial.println("Button press detected...");
    }
  }

  // ---------------------- Long Press Handling ----------------------
  if (buttonPressed && currentButtonState == LOW) {
    if (!longPressTriggered && (currentTime - buttonPressStartTime > LONG_PRESS_THRESHOLD)) {
      // Long press detected
      longPressTriggered = true;
      tone(BUZZER_PIN, 1000);   // Activate buzzer at 1kHz
      displayMessage("BUZZER");
      Serial.println("Long press detected - buzzer activated");
    }
  }

  // ---------------------- Button Release Handling ----------------------
  if (buttonPressed && currentButtonState == HIGH) {
    // Debounce check
    delay(DEBOUNCE_DELAY);
    if (digitalRead(BUTTON_PIN) == HIGH) {
      noTone(BUZZER_PIN);  // Stop buzzer immediately

      if (!longPressTriggered) {
        // --- Short Press Action: Toggle LED ---
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        displayMessage(ledState ? "LED ON" : "LED OFF");
        Serial.println(ledState ? "Short press: LED turned ON" : "Short press: LED turned OFF");
      } else {
        // --- Long Press Release Action ---
        displayMessage("Stopped");
        Serial.println("Long press released - buzzer stopped");
      }
      // Reset press flags
      buttonPressed = false;
      longPressTriggered = false;
    }
  }
}
