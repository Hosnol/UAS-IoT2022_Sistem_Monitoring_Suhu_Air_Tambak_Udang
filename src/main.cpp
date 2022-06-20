#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// variable untuk menyimpan nilai ssid, password dan mqtt_server
const char *ssid = "Galaxy A52";  // sesuaikan dengan username wifi
const char *password = "irdh4560";  // sesuaikan dengan password wifi
const char *mqtt_server = "broker.hivemq.com"; // isikan server broker

// Temperature MQTT Topics
#define MQTT_PUB_TEMP "esp/ds18b20/temperature"

// GPIO where the DS18B20 is connected to
const int oneWireBus = D4;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);
// Temperature value
float temp;

//deklarasi lcd
LiquidCrystal_I2C lcd(0x3F, 16, 2);

WiFiClient espClient;
PubSubClient client(espClient);

long now = millis();
long lastMeasure = 0;
String macAddr = "";

// fungsi untuk connect wifi
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
  macAddr = WiFi.macAddress();
  Serial.println(macAddr);
}

// fungsi untuk connect mqtt
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(macAddr.c_str()))
    {
      Serial.println("connected");
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

// setup awal
void setup()
{
  Serial.begin(115200);
  lcd.init(); // initialize the lcd
  lcd.backlight(); //menyalakan backlight
  lcd.clear(); //menghapus tampilan LCD
  lcd.home();
  Serial.println("Mqtt Node-RED");
  setup_wifi();
  sensors.begin(); //Mulai sensor suhu
  client.setServer(mqtt_server, 1883);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  if (!client.loop())
  {
    client.connect(macAddr.c_str());
  }
  now = millis();
  if (now - lastMeasure > 5000)
  {
    lastMeasure = now;
    sensors.requestTemperatures(); 
    // Temperature in Celsius degrees
    temp = sensors.getTempCByIndex(0);

    // Publish an MQTT message on topic esp/ds18b20/temperature
    uint16_t packetIdPub1 = client.publish(MQTT_PUB_TEMP, String(temp).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1 ", MQTT_PUB_TEMP, packetIdPub1);
    Serial.printf("Message: %.2f \n", temp);
  }
   //Tampilan LCD
    lcd.setCursor(0,0);
	  lcd.print("Suhu Air ");
    lcd.setCursor(0,1);
	  lcd.print(temp);
	  lcd.print("*C");
}