#include <Arduino.h>

// --- Pin Configuration ---
#define BUZZER_PIN  27     // Buzzer connected to GPIO27
#define LED_PIN     18     // LED connected to GPIO18
#define BUZZER_CH   0      // PWM channel for buzzer
#define LED_CH      1      // PWM channel for LED
#define FREQ_BUZZER 1000   // Base buzzer frequency (was 2000)
#define FREQ_LED    5000   // LED PWM frequency
#define RESOLUTION  8      // 8-bit resolution (0–255)

// --- Function: LED brightness fade ---
void fadeLED() {
  // Fade In
  for (int d = 0; d <= 255; d++) {
    ledcWrite(LED_CH, d);
    delay(5);
  }
  // Fade Out
  for (int d = 255; d >= 0; d--) {
    ledcWrite(LED_CH, d);
    delay(5);
  }
}

// --- Function 1: Simple beep pattern ---
void playBeepPattern() {
  for (int i = 0; i < 3; i++) {
    ledcWrite(LED_CH, 255);                // LED full brightness
    ledcWriteTone(BUZZER_CH, 1000 + i * 500); // Beeps at 1000, 1500, 2000 Hz
    delay(200);
    ledcWrite(BUZZER_CH, 0);
    ledcWrite(LED_CH, 0);
    delay(150);
  }
}

// --- Function 2: Frequency sweep ---
void playFrequencySweep() {
  for (int f = 500; f <= 4000; f += 200) {  // Sweeps from 500Hz to 4kHz
    ledcWriteTone(BUZZER_CH, f);
    ledcWrite(LED_CH, (f / 16)); // LED brightness changes with frequency
    delay(25);
  }
  ledcWrite(BUZZER_CH, 0);
  ledcWrite(LED_CH, 0);
  delay(400);
}

// --- Function 3: Short melody ---
void playMelody() {
  // Frequencies for notes (C D E F G A B C)
  int melody[] = {262, 330, 392, 523, 587, 659, 784, 988}; 
  for (int i = 0; i < 8; i++) {
    ledcWrite(LED_CH, 255);              // LED on
    ledcWriteTone(BUZZER_CH, melody[i]);
    delay(300);
    ledcWrite(LED_CH, 0);                // LED off
    delay(80);
  }
  ledcWrite(BUZZER_CH, 0);
}

// --- Setup ---
void setup() {
  // Setup buzzer PWM
  ledcSetup(BUZZER_CH, FREQ_BUZZER, RESOLUTION);
  ledcAttachPin(BUZZER_PIN, BUZZER_CH);

  // Setup LED PWM
  ledcSetup(LED_CH, FREQ_LED, RESOLUTION);
  ledcAttachPin(LED_PIN, LED_CH);
}

// --- Loop ---
void loop() {
  playBeepPattern();
  fadeLED();

  playFrequencySweep();
  fadeLED();

  playMelody();
  fadeLED();

  delay(500);
}
