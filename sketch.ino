#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

Servo servos[6];
const int servoPins[6] = {13, 12, 14, 27, 26, 25};

void callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload, length);

  if (error) return;

  int goc1 = doc["goc1"];
  int goc2 = doc["goc2"];
  int goc3 = doc["goc3"];

  // Ánh xá 3 biến trở cho 6 Servo (Ví dụ: mỗi biến trở điều khiển cặp 2 servo)
  servos[0].write(goc1);
  servos[1].write(180 - goc1); // Đảo chiều servo thứ 2

  servos[2].write(goc2);
  servos[3].write(180 - goc2);

  servos[4].write(goc3);
  servos[5].write(180 - goc3);
}

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32_Receiver_" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe("khoaluan/robot/control");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Khởi tạo 6 Servo
  for (int i = 0; i < 6; i++) {
    servos[i].attach(servoPins[i], 500, 2400);
  }

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}