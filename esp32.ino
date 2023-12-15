#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h> 
#include <ArduinoJson.h>
#include <WebServer.h>

// AP credentials
#define AP_SSID   "ESP32_AP"
#define AP_PASS   "123456789"

// WiFi credentials
#define WIFI_SSID   "Zune"
#define WIFI_PASS   "08092002"

// Adafruit IO credentials
#define IO_USERNAME    "dangtna12020"
#define IO_KEY         "aio_rtGp863QFkradfYw2CtIeUV2QT4A"

// Adafruit IO MQTT server
#define MQTT_SERVER    "io.adafruit.com"
#define MQTT_SERVERPORT 1883 

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, IO_USERNAME, IO_KEY);

// Initialize MQTT feeds and subscribers
Adafruit_MQTT_Publish yourFeed = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/test");
Adafruit_MQTT_Subscribe yourSubscriber = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME "/feeds/test");

Adafruit_MQTT_Subscribe servoSubscriber = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME "/feeds/yolo-servo");
Adafruit_MQTT_Subscribe fanSubscriber = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME "/feeds/yolo-fan");

WebServer server(80); // Server will run on port 80

struct ControllerData {
    float Fan;
    float Servo;
} lastestData;

// Handling function for servo data
void handleServoData(char* data, uint16_t len) {
    lastestData.Servo = atof(data);
}

// Handling function for fan data
void handleFanData(char* data, uint16_t len) {
    lastestData.Fan = atof(data);
}

// Handling function for GET request to retrieve data
void handleGetData() {
    DynamicJsonDocument doc(256);
    doc["yolo-servo"] = lastestData.Servo;
    doc["yolo-fan"] = lastestData.Fan;

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

// Handling function for POST request to receive and publish data
void handlePostData() {
  if (server.hasArg("plain")) {
    String jsonData = server.arg("plain");

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, jsonData);

    if (error) {
      server.send(400, "text/plain", "JSON parsing failed");
      return;
    }

    for (JsonPair keyValue : doc.as<JsonObject>()) {
      const char* feedKey = keyValue.key().c_str();
      JsonVariant dataValue = keyValue.value();

      String topic = String(IO_USERNAME) + "/feeds/" + String(feedKey);
      Adafruit_MQTT_Publish mqttPublish(&mqtt, topic.c_str());

      // Check data type and publish accordingly
      if (dataValue.is<float>()) {
        float floatValue = dataValue.as<float>();
        if (!mqttPublish.publish(floatValue)) {
          Serial.println("MQTT Publish failed for float");
        }
      } else if (dataValue.is<const char*>()) {
        const char* stringValue = dataValue.as<const char*>();
        if (!mqttPublish.publish(stringValue)) {
          Serial.println("MQTT Publish failed for string");
        }
      } else {
        Serial.println("Unsupported data type");
      }
    }
    server.send(200, "text/plain", "Data received and published to feeds");
  } else {
    server.send(400, "text/plain", "Body not found");
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Set up access point mode
  WiFi.softAP(AP_SSID, AP_PASS);

  //Establish MQTT subscriptions
  mqtt.subscribe(&servoSubscriber);
  mqtt.subscribe(&fanSubscriber);

  // Define routes for handling GET and POST requests
  server.on("/getData", HTTP_GET, handleGetData);
  server.on("/postdata", HTTP_POST, handlePostData);

  // Connect to Adafruit IO MQTT
  mqtt.subscribe(&yourSubscriber);

  // Start the server
  server.begin();
}

void loop() {
  // Handle incoming client requests
  server.handleClient();

  // Reconnect if necessary and process MQTT packets
  if (!mqtt.connected()) {
    if (mqtt.connect()) {
      mqtt.subscribe(&yourSubscriber);
    } else {
      delay(5000);
    }
  } else {
    mqtt.processPackets(100);
    mqtt.ping();
  }

  // Read MQTT subscription and handle received data
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &servoSubscriber) {
      handleServoData((char *)servoSubscriber.lastread, servoSubscriber.datalen);
    } else if (subscription == &fanSubscriber) {
      handleFanData((char *)fanSubscriber.lastread, fanSubscriber.datalen);
    }
  }

  // Print connection status and latest data values
  static unsigned long lastPrint = 0;
  unsigned long now = millis();
  if (now - lastPrint > 10000) { // Print every 10 seconds
    lastPrint = now;

    Serial.print("Stations connected = ");
    Serial.println(WiFi.softAPgetStationNum());

    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());

    Serial.print("Latest Servo Value: ");
    Serial.println(lastestData.Servo);
    Serial.print("Latest Fan Value: ");
    Serial.println(lastestData.Fan);
  }
}
