#include <WiFiNINA.h>
#include <PubSubClient.h>  // MQTT library

// WiFi Credentials
#define WIFI_SSID "Telstra1AA87A"
#define WIFI_PASSWORD "rqp2vsnk44b7unk4"

// MQTT Broker Credentials
#define MQTT_SERVER "broker.emqx.io"
#define MQTT_PORT 1883
#define MQTT_WAVE_TOPIC "SIT210/wave"
#define MQTT_PAT_TOPIC "SIT210/pat"

#define name "Chhay Hout Lay"

#define trigger 4
#define echo 2
#define blue 7

long lastmessage = 0;
long now;
double distance,duration;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void connectMQTT() {
   while (!mqttClient.connected()) {
       Serial.println("Connecting to MQTT...");
       if (mqttClient.connect("ArduinoClient")) {
           Serial.println("Connected to MQTT Broker!");
            mqttClient.subscribe(MQTT_WAVE_TOPIC);
            mqttClient.subscribe(MQTT_PAT_TOPIC);
       } else {
           Serial.print("Failed, rc=");
           Serial.print(mqttClient.state());
           Serial.println(" Retrying in 5 seconds...");
           delay(5000);
       }
   }

}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message: ");
  Serial.print(topic);
  Serial.print(" - ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if(strcmp(topic, "SIT210/wave") == 0)
  {
    for(int i=0; i < 3; i++)
    {
      digitalWrite(blue, HIGH);
      delay(200);
      digitalWrite(blue,LOW);
      delay(200);
    }
  }
  else if(strcmp(topic, "SIT210/pat") == 0)
  {
  for(int i=0; i < 5; i++)
  {
    digitalWrite(blue, HIGH);
    delay(200);
    digitalWrite(blue,LOW);
    delay(200);
  }
  }
}

void setup() {
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(blue, OUTPUT);
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  mqttClient.setCallback(callback);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  connectMQTT();
}


void loop() {
  if(!mqttClient.connected())
  {
    connectMQTT();
  }
  mqttClient.loop();
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration * 0.0343 / 2;

  now = millis();
  if(distance > 20 && distance < 50 && now - lastmessage > 10000)
  {
    if(mqttClient.publish(MQTT_WAVE_TOPIC, name))
    {
      Serial.println("message sent!");
      Serial.println(distance);
    }
    else
    {
      Serial.println("Message is not sent!");
    }
    lastmessage = now;
  }
  else if (distance >= 50 && distance < 100 && now - lastmessage > 10000)
  {
    if(mqttClient.publish(MQTT_PAT_TOPIC, name))
    {
      Serial.println("message sent!");
      Serial.println(distance);
    }
    else
    {
      Serial.println("Message is not sent!");
    }
    lastmessage = now;
  }
  delay(200);
}
