/*
  - Install host software:
    - For Linux, install Avahi (http://avahi.org/).
    - For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
    - For Mac OSX and iOS support is built in through Bonjour already.
  - Point your browser to http://esp8266.local, you should see a response.

*/

#define SPEAKERPIN        D5 // Pin with the piezo speaker attached

int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
int addup = 0;
int count = 100;
int topCount = 100;

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
long lastReconnectAttempt = 0;

long buttonTimer = 0;


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
  client.setCallback(callback);
  pinMode(SPEAKERPIN, OUTPUT);

  Serial.print("all Setup");
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == 'a' && (char)payload[1] == 'r' && (char)payload[2] == 'f') {
    arf();
  }

  Serial.println();

  // Switch on the LED if an 1 was received as first character


}

boolean reconnect() {

  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if (client.connect(DeviceName)) {
    Serial.println("connected");
    //       Once connected, publish an announcement...
    client.publish(DeviceName, "Buttons");
    // ... and resubscribe
    client.subscribe(DeviceName);
    client.setCallback(callback);
  }
  return client.connected();
}


void loop() {
  if (analogRead(sensorPin) >= 200) {
    sensorValue += analogRead(sensorPin);
    // Serial.println(sensorValue);
    count--;
  }

  if (count == 0) {
    count = topCount;

    sensorValue /= (topCount);
    sensorValue = (sensorValue / 10 + (sensorValue % 10 > 5)) * 10;

    buttonSort(sensorValue);
    lastReconnectAttempt = millis();
  }

client.connected();


if (queuelevel() > 0) {
  if (!client.connected()) {
    Serial.print('.');
   // tickSound2();
    long now = millis();
    if (now - lastReconnectAttempt > 3000) {
      lastReconnectAttempt = now;
      reconnect();
      // Attempt to reconnect
      //      if (reconnect()) {
      //        lastReconnectAttempt = 0;
      //      }
    }
  } else {

    //lastReconnectAttempt = 0;
    // Client connected
    
      Serial.println("queLength");
      Serial.println(queuelevel());
      char buf[4];

      Serial.println("connection established, sending from queue");
      byte bb = dequeue();
      Serial.println(bb);
      client.publish(DeviceName, itoa(bb, buf, 10) );
    }

  }
  client.loop();

}

void buttonSort(int inVal) {

  byte ID = 0;
  if (inVal == 1030) {
    sendPress(1);
  } else if (inVal == 930 || inVal == 940) {
    sendPress(2);
  } else if (inVal == 850 || inVal == 860) {
    sendPress(3);
  } else if (inVal == 790 || inVal == 800 || inVal == 780) {
    sendPress(4);
  } else if (inVal == 730) {
    sendPress(5);
  } else if (inVal == 680) {
    sendPress(6);
  } else if (inVal == 650 || inVal == 640 || inVal == 630) {
    sendPress(7);
  } else if (inVal == 600) {
    sendPress(8);
  } else if (inVal == 580 || inVal == 570  || inVal == 560) {
    sendPress(9);
  } else if (inVal == 550 || inVal == 540) {
    sendPress(10);
  } else if (inVal == 510 || inVal == 520) {
    sendPress(11);
  } else if (inVal == 500 || inVal == 490) {
    sendPress(12);
  } else if (inVal > 30) {
    Serial.println("not found ");
    Serial.println(inVal);
  }
}

void sendPress(byte id) {

  long now = millis();
  if (now - buttonTimer > 100) {

    enqueue(id);
    buttonTimer = now;
    // delay(100);
  }
}

const int ringsize = 8;
int head = 0;
int tail = 0;
byte ring[ringsize];
// Put something into the buffer. Returns 0 when the buffer was full,
// 1 when the stuff was put sucessfully into the buffer
int enqueue (byte val) {
  int newtail = (tail + 1) % ringsize;
  if (newtail == head) {
    // Buffer is full, do nothing
    return 0;
  }
  else {
    ring[tail] = val;
    tail = newtail;
    return 1;
  }
}

// Return number of elements in the queue.
int queuelevel () {
  return tail - head + (head > tail ? ringsize : 0);
}

// Get something from the queue. 0 will be returned if the queue
// is empty
byte dequeue () {
  if (head == tail) {
    return 0;
  }
  else {
    byte val = ring[head];
    head  = (head + 1) % ringsize;
    return val;
  }
}
