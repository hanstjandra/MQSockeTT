#include <WiFi.h>
#include <PubSubClient.h>

// =====================
// Wi-Fi Credentials
// =====================
const char* ssid = "CHL";
const char* password = "G4G9o5_CD8";

// =====================
// MQTT Broker Settings
// =====================
const char* mqtt_server = "192.168.68.75";  // Orange Pi MQTT broker
const char* mqtt_topic = "home/ssr";        // Topic ESP32 subscribes to

// =====================
// SSR Pins
// =====================
#define RELAY1_PIN 27  // Left SSR 1
#define RELAY2_PIN 32  // Right SSR 2
#define RELAY3_PIN 16  // Left SSR 3
#define RELAY4_PIN 19  // Right SSR 4

WiFiClient espClient;
PubSubClient client(espClient);

// =====================
// Setup
// =====================
void setup() {
  Serial.begin(115200);

  // Initialize SSR pins as OUTPUT
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);

  // Start with all SSRs OFF (HIGH = OFF for most SSRs)
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);
  digitalWrite(RELAY4_PIN, HIGH);

  // Connect to Wi-Fi
  setup_wifi();

  // Setup MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);
}

// =====================
// Wi-Fi Connection
// =====================
void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
}

// =====================
// MQTT Message Callback
// =====================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message received: ");
  Serial.println(message);

  // Individual SSR control
  if (message == "SSR1_ON") digitalWrite(RELAY1_PIN, LOW);
  if (message == "SSR1_OFF") digitalWrite(RELAY1_PIN, HIGH);

  if (message == "SSR2_ON") digitalWrite(RELAY2_PIN, LOW);
  if (message == "SSR2_OFF") digitalWrite(RELAY2_PIN, HIGH);

  if (message == "SSR3_ON") digitalWrite(RELAY3_PIN, LOW);
  if (message == "SSR3_OFF") digitalWrite(RELAY3_PIN, HIGH);

  if (message == "SSR4_ON") digitalWrite(RELAY4_PIN, LOW);
  if (message == "SSR4_OFF") digitalWrite(RELAY4_PIN, HIGH);
}

// =====================
// Reconnect to MQTT
// =====================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_SSR")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic); // Subscribe to SSR control topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" — retrying in 5 seconds");
      delay(5000);
    }
  }
}

// =====================
// Main Loop
// =====================
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
