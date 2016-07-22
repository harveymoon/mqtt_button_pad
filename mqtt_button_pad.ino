/*
  - Install host software:
    - For Linux, install Avahi (http://avahi.org/).
    - For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
    - For Mac OSX and iOS support is built in through Bonjour already.
  - Point your browser to http://esp8266.local, you should see a response.

*/
int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
int addup = 0;
int count = 10;
int topCount = 10;

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const char* DeviceName = "wemos_buttons_01";

const char* mqtt_server = "iot.unanything.com";
//const char* mqtt_server = "10.1.10.96";

void setup(void)
{

  delay(500);
  Serial.begin(115200);

  // Connect to WiFi network
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration

  wifiManager.autoConnect(DeviceName);
  client.setServer(mqtt_server, 5556);// for iot.unanything.com
  // client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
  Serial.print("all Setup");

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(DeviceName)) {
      Serial.println("connected");
      //       Once connected, publish an announcement...
      // client.publish(DeviceName, "Pressed");
      // ... and resubscribe
      client.subscribe(DeviceName);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

    count--;
    sensorValue += analogRead(sensorPin);
  if (count == 0) {
    count = topCount;
     
     sensorValue/=topCount;
     sensorValue = (sensorValue / 10 + (sensorValue % 10 > 5)) * 10;
     buttonSort(sensorValue);
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}




void buttonSort(int inVal) {

  byte ID = 0;
  if (inVal == 1020) {
    sendPress(1);
  } else if (inVal == 930) {
    sendPress(2);
  } else if (inVal == 850) {
    sendPress(3);
  } else if (inVal == 780) {
    sendPress(4);
  } else if (inVal == 730) {
    sendPress(5);
  } else if (inVal == 680) {
    sendPress(6);
  } else if (inVal == 630 || inVal == 640) {
    sendPress(7);
  } else if (inVal == 600 || inVal == 590) {
    sendPress(8);
  } else if (inVal == 570) {
    sendPress(9);
  } else if (inVal == 540) {
    sendPress(10);
  } else if (inVal == 510) {
    sendPress(11);
  } else if (inVal == 490) {
    sendPress(12);
  } else if (inVal > 30) {
    Serial.println("not found ");
    Serial.println(inVal);
  }
}

void sendPress(byte id) {
    char buf[4];
  Serial.println(id);
  client.publish(DeviceName, itoa(id, buf, 10) );
  delay(1000);
}
