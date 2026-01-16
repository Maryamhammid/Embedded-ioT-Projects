/****************************************************
 * ESP32 + MQTT Subscriber + OLED
 * Subscribes:
 *   home/lab2/temp
 *   home/lab2/humidity
 ****************************************************/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- WiFi ----------
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// ---------- MQTT ----------
const char* mqtt_server = "192.168.56.1";
const int mqtt_port = 1883;

const char* TOPIC_TEMP1 = "home/lab2/mariha/temp"; // change to your actual topic
// const char* TOPIC_TEMP2 = "home/lab2/temp2"; // if multiple publishers
const char* TOPIC_HUM1  = "home/lab2/mariha/hum";
// const char* TOPIC_HUM2  = "home/lab2/hum2";

// ---------- OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- MQTT ----------
WiFiClient espClient;
PubSubClient mqtt(espClient);

// Latest values
String lastTemp = "--";
String lastHum  = "--";

// ---------- OLED Display ----------
void showData() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("MQTT Sensor Monitor");
  display.println("------------------");

  display.setCursor(0,20);
  display.print("Temp: ");
  display.print(lastTemp);
  display.println(" C");

  display.setCursor(0,35);
  display.print("Hum : ");
  display.print(lastHum);
  display.println(" %");

  display.display();
}
void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++)
    msg += (char)payload[i];
  msg.trim();

  String t = String(topic);

  if (t.endsWith("/temp")) {
    lastTemp = msg;
    Serial.print("Temp received: ");
    Serial.println(msg);
  }
  else if (t.endsWith("/hum")) {
    lastHum = msg;
    Serial.print("Humidity received: ");
    Serial.println(msg);
  }

  showData();
}


// ---------- WiFi ----------
void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

// ---------- MQTT ----------
void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqtt.connect("subscriber-esp32")) {
      Serial.println("connected");
      mqtt.subscribe(TOPIC_TEMP1);
      mqtt.subscribe(TOPIC_HUM1);
      showData();
    } else {
      Serial.print("failed rc=");
      Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // OLED (ESP32 default SDA=21, SCL=22)
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while (1);
  }

  display.clearDisplay();
  display.display();

  showData();
  connectWiFi();

  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(callback);
  connectMQTT();
}

void loop() {
  if (!mqtt.connected())
    connectMQTT();

  mqtt.loop();
}
