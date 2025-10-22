#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---- OLED setup ----
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void drawPrimitives() {
  // Lines
  display.drawLine(0, 0, 127, 63, SSD1306_WHITE);
  display.drawLine(0, 63, 127, 0, SSD1306_WHITE);
  // Rectangles
  display.drawRect(2, 2, 40, 20, SSD1306_WHITE);
  display.fillRect(50, 2, 30, 20, SSD1306_WHITE);
  // Circles
  display.drawCircle(100, 16, 10, SSD1306_WHITE);
  display.fillCircle(100, 48, 10, SSD1306_WHITE);
  // Triangles
  display.drawTriangle(5, 60, 25, 30, 45, 60, SSD1306_WHITE);
}

void drawTextDemo() {
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Adafruit_GFX");
  display.setTextSize(2);
  display.setCursor(0, 18);
  display.println("ESP32");
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print("W:"); display.print(display.width());
  display.print(" H:"); display.println(display.height());
}

void setup() {
  Wire.begin(21, 22); // ESP32 default I2C pins (SDA=21, SCL=22)

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    // If it fails, check wiring and address (0x3C/0x3D)
    for (;;);
  }

  // Example 1: text formatting
  display.clearDisplay();
  drawTextDemo();
  display.display();
  delay(2000);

  // Example 2: shapes
  display.clearDisplay();
  drawPrimitives();
  display.display();
  delay(2000);

  // Example 3: rotation + inverse
  display.clearDisplay();
  display.setRotation(1); // 90 degrees
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Rotated 90");
  display.display();
  delay(1500);
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  display.setRotation(0); // back to normal
  
}

void loop() {
  // Simple marquee using getTextBounds
  static int x = 128;
  const char *msg = "Hello IoT!  ";
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(x, 26);
  display.print(msg);
  display.display();

  x -= 3;
  if (x < -((int)w)) x = display.width();
  delay(40);
}
