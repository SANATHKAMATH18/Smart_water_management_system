#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <SoftwareSerial.h>

#define FLOW_PIN 2
#define TRIG 9
#define ECHO 10
#define BUZZER 8
#define LED_PIN 6
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial nodeSerial(A2, A3); // TX=A2, RX=A3

volatile unsigned int pulseCount = 0;
float calibrationFactor = 7.5;
float tankHeightCm = 30.0;

void pulseCounter() {
  pulseCount++;
}

float readFlow() {
  pulseCount = 0;
  delay(1000);
  return (pulseCount / calibrationFactor);
}

float readDistance() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH, 30000);
  return (duration * 0.0343) / 2.0;
}

void setup() {
  Serial.begin(9600);
  nodeSerial.begin(9600);
  dht.begin();

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(FLOW_PIN, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), pulseCounter, RISING);
}

void loop() {
  float flow = readFlow();
  float distance = readDistance();
  float level = constrain(((tankHeightCm - distance) / tankHeightCm) * 100.0, 0, 100);
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  String data = String(flow, 2) + "," + String(level, 1) + "," + String(t, 1) + "," + String(h, 1);
  nodeSerial.println(data);  // Send to NodeMCU

  Serial.println("Sent: " + data);
  delay(3000);
}
