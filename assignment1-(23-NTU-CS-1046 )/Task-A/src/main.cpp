/*
Project Title : LED Control using Two Push Buttons and Display on OLED
Name          : Maryam Hamid
Reg No        : 23-NTU-CS-1046
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------------------- Pin Definitions ----------------------------
#define LED_RED     16      // LED 1 - Red
#define LED_GREEN   17      // LED 2 - Green
#define LED_BLUE    18      // LED 3 - Blue
#define BTN_MODE    32      // Push button to cycle through modes
#define BTN_RESET   33      // Push button to reset LEDs

// ---------------------------- OLED Display Setup -------------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 oledDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------------------------- LED Control Variables ----------------------
int ledMode = 0;             // Current LED mode (0 = off)
int fadeBrightness = 0;      // Brightness value for fade mode
int fadeStep = 5;            //  step for fade effect
int blinkIndex = 0;          // Index for alternate blink sequence

// ---------------------------- Timing Variables --------------------------
unsigned long prevBlinkTime = 0;
unsigned long prevFadeTime = 0;
unsigned long prevModeButtonTime = 0;
unsigned long prevResetButtonTime = 0;

const long BLINK_INTERVAL   = 200;   // milliseconds
const long FADE_INTERVAL    = 15;    // milliseconds
const long BUTTON_DEBOUNCE  = 300;   // debounce for mode button
const long RESET_DEBOUNCE   = 200;   // debounce for reset button
const unsigned long PWM_PERIOD = 10; // software PWM period (~100Hz)

// ------------------------------------------------------------------------
// FUNCTION: showCurrentMode()
// Purpose : Displays the current LED mode text on the OLED
// ------------------------------------------------------------------------
void showCurrentMode(const char *modeText) {
  oledDisplay.clearDisplay();
  oledDisplay.setTextSize(1);
  oledDisplay.setTextColor(SSD1306_WHITE);
  oledDisplay.setCursor(0, 25);
  oledDisplay.print("Mode: ");
  oledDisplay.println(modeText);
  oledDisplay.display();
}

// ------------------------------------------------------------------------
// FUNCTION: resetAllLEDs()
// Purpose : Turns off all LEDs and resets mode to "OFF"
// ------------------------------------------------------------------------
void resetAllLEDs() {
  ledMode = 0;
  fadeBrightness = 0;
  fadeStep = 5;
  blinkIndex = 0;

  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  
  showCurrentMode("All OFF (Reset)");
}

// SETUP: Initialize OLED, LEDs, and Button Pins

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);

  // Initialize OLED
  if (!oledDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1); // halt if display initialization fails
  }

  // Initial display and LED state
  showCurrentMode("All OFF");
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
}

// LOOP: Main execution cycle - handles buttons and LED modes

void loop() {
  unsigned long currentTime = millis();

  // --- RESET BUTTON HANDLER ---
  if (digitalRead(BTN_RESET) == LOW && (currentTime - prevResetButtonTime >= RESET_DEBOUNCE)) {
    prevResetButtonTime = currentTime;
    resetAllLEDs();
    delay(50);  // short delay to avoid retriggering
    return;     // exit loop to immediately apply reset
  }

  if (digitalRead(BTN_MODE) == LOW && (currentTime - prevModeButtonTime >= BUTTON_DEBOUNCE)) {
    prevModeButtonTime = currentTime;
    ledMode = (ledMode + 1) % 4; // cycle through 4 modes (0–3)

    // Reset LED states when changing modes
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
    fadeBrightness = 0;
    fadeStep = 5;
    blinkIndex = 0;
    prevBlinkTime = currentTime;
    prevFadeTime = currentTime;

    // Update OLED display
    switch (ledMode) {
      case 0: showCurrentMode("All OFF"); break;
      case 1: showCurrentMode("Alternate Blink"); break;
      case 2: showCurrentMode("All ON"); break;
      case 3: showCurrentMode("PWM Fade"); break;
    }
    delay(50); // small debounce gap
  }

  // --- LED MODE CONTROL ---
  switch (ledMode) {

    // MODE 0: All LEDs OFF
    case 0:
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, LOW);
      break;

    // MODE 1: Alternate Blink between 3 LEDs
    case 1:
      if (currentTime - prevBlinkTime >= BLINK_INTERVAL) {
        prevBlinkTime = currentTime;

        // Turn all off first
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_BLUE, LOW);

        // Light up one LED in sequence
        if (blinkIndex == 0) digitalWrite(LED_RED, HIGH);
        else if (blinkIndex == 1) digitalWrite(LED_GREEN, HIGH);
        else if (blinkIndex == 2) digitalWrite(LED_BLUE, HIGH);

        blinkIndex = (blinkIndex + 1) % 3;
      }
      break;

    // MODE 2: All LEDs ON
    case 2:
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_BLUE, HIGH);
      break;

    // MODE 3: PWM Fade (Software-based)
    case 3:
      if (currentTime - prevFadeTime >= FADE_INTERVAL) {
        prevFadeTime = currentTime;
        fadeBrightness += fadeStep;

        if (fadeBrightness <= 0) {
          fadeBrightness = 0;
          fadeStep = abs(fadeStep);
        } 
        else if (fadeBrightness >= 255) {
          fadeBrightness = 255;
          fadeStep = -abs(fadeStep);
        }
      }

      // Compute software PWM duty cycle
      {
        unsigned long phase = currentTime % PWM_PERIOD;
        unsigned int onTime = (unsigned long)fadeBrightness * PWM_PERIOD / 255u;
        bool ledOn = (phase < onTime);

        digitalWrite(LED_RED, ledOn);
        digitalWrite(LED_GREEN, ledOn);
        digitalWrite(LED_BLUE, ledOn);
      }
      break;

    default:
      break;
  }
}
