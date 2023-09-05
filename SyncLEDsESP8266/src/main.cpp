#include<ESP8266WiFi.h>
#include<PubSubClient.h>
#include <Arduino.h>
const char* ssid = "Hoang chuong";
const char* password = "0942104221";

#define MQTT_SERVER "broker.mqttdashboard.com"
#define MQTT_PORT 1883
#define MQTT_USER "KhanhDuy"
#define MQTT_PASSWORD "Khanhduy123"
#define MQTT_TOPIC "SyncLEDs"

const int BUTTON1 = 13;
const int BUTTON2 = 12;
const int LED1 = 14;
const int LED2 = 16;

bool state_led_1 = false;
bool state_led_2 = false;
int value_button_1 = 1;
int value_button_2 = 1;

WiFiClient wifi_client;
PubSubClient client(wifi_client);


void setup_WiFi() 
{
  Serial.print("Connecting to WiFi");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void connect_to_broker() 
{
  while(!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    char * clientID = "IoTDevice2";
    if(client.connect(clientID,MQTT_USER,MQTT_PASSWORD)) 
    {
      client.subscribe(MQTT_TOPIC);
      Serial.println("Successfully connect to broker");
    }
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void toggle(bool *present_state, const int pin) 
{
  if (*present_state) 
  {
    *present_state = false;
    digitalWrite(pin,LOW);
  }
  else 
  {
    *present_state = true;
    digitalWrite(pin,HIGH);
  }
}

void callback(char* topic, byte *payload, unsigned int length) 
{
  Serial.println("Message arrived");
  if(*payload == '1') toggle(&state_led_1,LED1);
  else if (*payload == '2') toggle(&state_led_2,LED2);
}

void send_data(char* led) 
{
  client.publish(MQTT_TOPIC,led);
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  setup_WiFi();
  client.setServer(MQTT_SERVER,MQTT_PORT);
  client.setCallback(callback);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
}

void loop() {
  client.loop();
  if (!client.connected()) 
  {
    connect_to_broker();
  }
  value_button_1 = digitalRead(BUTTON1);
  value_button_2 = digitalRead(BUTTON2);
  if (value_button_1 == 0) 
  {
    toggle(&state_led_1,LED1);
    send_data("3");
  }
  if (value_button_2 == 0) 
  {
    toggle(&state_led_2,LED2);
    send_data("4");
  }
}

