#define BLYNK_TEMPLATE_ID           "TMPL3FvRi7A0T"
#define BLYNK_TEMPLATE_NAME         "Smart Water Monitor"
#define BLYNK_AUTH_TOKEN            "TpAAZ-jIR6omR5DWxg0rtTK3JVuDyoU5"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


// ---------- Wi-Fi Credentials ----------
char ssid[] = "SANATH";
char pass[] = "12345678";
const char* flaskServer = "http://10.243.87.35:5000/upload";  


// ---------- UNO Communication ----------
SoftwareSerial arduinoSerial(D2, D3); // RX=D3, TX=D2 (connect UNO A2→D2, GND↔GND)
BlynkTimer timer;

// ---------- Gamification Variables ----------
int points = 0;
String badge = "🌱 New User";

// ---------- Function: Send Data to Blynk ----------
void sendToBlynk(String data) {
  float flow, level, temp, hum;
  int count = sscanf(data.c_str(), "%f,%f,%f,%f", &flow, &level, &temp, &hum);

  if (count == 4) {
    // Send live sensor data
    Blynk.virtualWrite(V0, flow);
    Blynk.virtualWrite(V1, level);
    Blynk.virtualWrite(V2, temp);
    Blynk.virtualWrite(V3, hum);

    // ---- Gamification Rules ----
    if (flow < 2.0 && level >= 30 && level <= 80) {
      points += 5;  // good usage
    } 
    else if (level > 90) {
      points -= 8;  // overflow
    } 
    else if (flow > 5.0) {
      points -= 5;  // possible leak
    }

    // Cap points between 0–500
    if (points < 0) points = 0;
    if (points > 500) points = 500;

    // Badge assignment
    if (points >= 100) badge = "🏅 Eco Hero";
    else if (points >= 50) badge = "💧 Saver";
    else badge = "🌱 New User";

    // Push gamification data to Blynk
    Blynk.virtualWrite(V4, points);
    Blynk.virtualWrite(V5, badge);

    // Serial feedback
    Serial.print("Flow: "); Serial.print(flow);
    Serial.print(" | Level: "); Serial.print(level);
    Serial.print(" | Points: "); Serial.print(points);
    Serial.print(" | Badge: "); Serial.println(badge);
    sendToFlask(flow, level, temp, hum, points);
  }
}

void sendToFlask(float flow, float level, float temp, float hum, int points) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, flaskServer);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"device_id\":\"home1\","
                     "\"flow\":" + String(flow, 2) +
                     ",\"level\":" + String(level, 2) +
                     ",\"temp\":" + String(temp, 1) +
                     ",\"hum\":" + String(hum, 1) +
                     ",\"points\":" + String(points) + "}";

    int httpResponseCode = http.POST(payload);
    Serial.print("Flask POST Response: ");
    Serial.println(httpResponseCode);

    http.end();
  }
}


// ---------- Function: Read CSV from UNO ----------
void readFromArduino() {
  if (arduinoSerial.available()) {
    String line = arduinoSerial.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      Serial.println("Received: " + line);
      sendToBlynk(line);
    }
  }
}

// ---------- Function: Daily Reset ----------
void dailyReset() {
  points = 0;
  badge = "🌱 New User";

  // Reset on dashboard
  Blynk.virtualWrite(V4, points);
  Blynk.virtualWrite(V5, badge);

  Serial.println("🔄 Daily Reset: Points and badge reset to default.");

  // Optional Blynk notification (set up 'daily_reset' event in Blynk)
  Blynk.logEvent("daily_reset", "Daily reset complete! Start fresh 🌞");
}

// ---------- Setup ----------
void setup() {
  Serial.begin(9600);
  arduinoSerial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Run every 2 seconds to read data from UNO
  timer.setInterval(2000L, readFromArduino);
  // Run once every 24 hours (86,400,000 ms)
  timer.setInterval(86400000L, dailyReset);

  Serial.println("\n--- NodeMCU Smart Water + Gamification + Daily Reset ---");
}

// ---------- Loop ----------
void loop() {
  Blynk.run();
  timer.run();
}
