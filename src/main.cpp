#include <Arduino.h>
#include <Homie.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 2 // Pin which is connected to the DHT sensor.

#define FW_NAME "comfort-sensor"
#define FW_VERSION "1.0.0"
#define DHTTYPE DHT22 // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

const int TEMPERATURE_INTERVAL = 300;

unsigned long lastTemperatureSent = 0;

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */

HomieNode comfortNode("comfort", "comfort", "sensor");

void setupHandler()
{
  dht.begin();
  comfortNode.advertise("degrees")
    .setDatatype("float")
    .setUnit("ºF");
  comfortNode.advertise("humidity")
    .setDatatype("float")
    .setUnit("%");
}

void loopHandler()
{
  if (millis() - lastTemperatureSent >= TEMPERATURE_INTERVAL * 1000UL || lastTemperatureSent == 0)
  {
    // Print temperature sensor details.
    float temp_f = dht.readTemperature(true);
    if (isnan(temp_f))
    {
      Homie.getLogger() << "Error getting Temp" << endl;
    }
    else
    {
      Homie.getLogger() << "Temperature: " << temp_f << " °F" << endl;
      comfortNode.setProperty("degrees").send(String(temp_f));
    }
    float humidity_f = dht.readHumidity();
    if (isnan(humidity_f))
    {
      Homie.getLogger() << "Error getting Humidity" << endl;
    }
    else
    {
      Homie.getLogger() << "Humidity: " << humidity_f << "" << endl;
      comfortNode.setProperty("humidity").send(String(humidity_f));
    }
    lastTemperatureSent = millis();
  }
}

void setup()
{
  Serial.begin(115200);
  Serial << endl
         << endl;

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

  Homie.setup();
}

void loop()
{
  Homie.loop();
}