#include <DHT.h>
#define BLYNK_TEMPLATE_ID "TMPL3Ul8Ak1Ak"
#define BLYNK_TEMPLATE_NAME "Air quality detection"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = "F6qHig0MNL4VkjhDTDdocncn_T6jLU0d";
char ssid[] = "V2036";
char pass[] = "smartphone";

#define DHT_PIN 13
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// ----- MQ Sensor Analog Pins -----
#define MQ2_PIN 32
#define MQ6_PIN 33
#define MQ135_PIN 34

// ----- LED Indicator Pins -----
#define MQ2_LED 2
#define MQ6_LED 4
#define MQ135_LED 19

// ----- Threshold Values -----
// Note: ESP32 analogRead default range ~0-4095. Adjust thresholds accordingly.
int mq2_threshold = 500;
int mq6_threshold = 1000;
int mq135_threshold = 4000;

// ----- Alert / debounce settings -----
const unsigned long ALERT_COOLDOWN_MS = 5UL * 60UL * 1000UL; // 5 minutes
const int HYSTERESIS = 50;

// State for alerts
bool mq2_alerted = false;
bool mq6_alerted = false;
bool mq135_alerted = false;

unsigned long mq2_lastAlert = 0;
unsigned long mq6_lastAlert = 0;
unsigned long mq135_lastAlert = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting MQ Gas Detection System (MQ2, MQ6, MQ135) with Blynk.logEvent alerts...");
  Blynk.begin(auth, ssid, pass);

  dht.begin();

  // Analog pins don't need pinMode but harmless to set
  pinMode(MQ2_PIN, INPUT);
  pinMode(MQ6_PIN, INPUT);
  pinMode(MQ135_PIN, INPUT);

  pinMode(MQ2_LED, OUTPUT);
  pinMode(MQ6_LED, OUTPUT);
  pinMode(MQ135_LED, OUTPUT);
}

void loop() {
  Blynk.run(); // keep Blynk alive

  // --- Read DHT ---
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  bool dht_ok = true;
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT_ERROR");
    dht_ok = false;
  }

  // --- Read MQ sensors ---
  int mq2_value   = analogRead(MQ2_PIN);
  int mq6_value   = analogRead(MQ6_PIN);
  int mq135_value = analogRead(MQ135_PIN);

  // --- Print one clean line for logging/parsing ---
  // Format: MQ2:220, MQ6:310, MQ135:455, Temp:25.4, Hum:60
  Serial.print("MQ2:");
  Serial.print(mq2_value);
  Serial.print(", MQ6:");
  Serial.print(mq6_value);
  Serial.print(", MQ135:");
  Serial.print(mq135_value);

  if (dht_ok) {
    Serial.print(", Temp:");
    Serial.print(temperature);
    Serial.print(", Hum:");
    Serial.print(humidity);
  } else {
    Serial.print(", Temp:NaN, Hum:NaN");
  }
  Serial.println(); // <-- important newline so each reading is on its own line

  // --- Send to Blynk ---
  if (dht_ok) {
    Blynk.virtualWrite(V0, temperature); // Temperature
    Blynk.virtualWrite(V1, humidity);    // Humidity
  }
  Blynk.virtualWrite(V2, mq2_value);
  Blynk.virtualWrite(V4, mq6_value);
  Blynk.virtualWrite(V5, mq135_value);

  // --- LED logic reset ---
  digitalWrite(MQ2_LED, LOW);
  digitalWrite(MQ6_LED, LOW);
  digitalWrite(MQ135_LED, LOW);

  unsigned long now = millis();

  // --- MQ2 logic ---
  if (mq2_value > mq2_threshold) {
    digitalWrite(MQ2_LED, HIGH);
    if (!mq2_alerted && (now - mq2_lastAlert > ALERT_COOLDOWN_MS)) {
      mq2_alerted = true;
      mq2_lastAlert = now;
      Serial.println("⚠ MQ2 ALERT! sending event...");
      Blynk.logEvent("mq2_alert", "⚠ MQ2: high gas level detected! Value: " + String(mq2_value));
    }
  } else if (mq2_alerted && mq2_value < (mq2_threshold - HYSTERESIS)) {
    mq2_alerted = false;
    Serial.println("MQ2 back to normal.");
    Blynk.logEvent("mq_normal", "MQ2 back to normal. Value: " + String(mq2_value));
  }

  // --- MQ6 logic ---
  if (mq6_value > mq6_threshold) {
    digitalWrite(MQ6_LED, HIGH);
    if (!mq6_alerted && (now - mq6_lastAlert > ALERT_COOLDOWN_MS)) {
      mq6_alerted = true;
      mq6_lastAlert = now;
      Serial.println("⚠ MQ6 ALERT! sending event...");
      Blynk.logEvent("mq6_alert", "⚠ MQ6: high gas level detected! Value: " + String(mq6_value));
    }
  } else if (mq6_alerted && mq6_value < (mq6_threshold - HYSTERESIS)) {
    mq6_alerted = false;
    Serial.println("MQ6 back to normal.");
    Blynk.logEvent("mq_normal", "MQ6 back to normal. Value: " + String(mq6_value));
  }

  // --- MQ135 logic ---
  if (mq135_value > mq135_threshold) {
    digitalWrite(MQ135_LED, HIGH);
    if (!mq135_alerted && (now - mq135_lastAlert > ALERT_COOLDOWN_MS)) {
      mq135_alerted = true;
      mq135_lastAlert = now;
      Serial.println("⚠ MQ135 ALERT! sending event...");
      Blynk.logEvent("mq135_alert", "⚠ MQ135: poor air quality detected! Value: " + String(mq135_value));
    }
  } else if (mq135_alerted && mq135_value < (mq135_threshold - HYSTERESIS)) {
    mq135_alerted = false;
    Serial.println("MQ135 back to normal.");
    Blynk.logEvent("mq_normal", "MQ135 back to normal. Value: " + String(mq135_value));
  }

  delay(2000);
}







