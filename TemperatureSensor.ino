#include <Homie.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN            2         // Pin which is connected to the DHT sensor.

#define FW_NAME "temp-firmware"
#define FW_VERSION "1.0.2"

// Uncomment the type of sensor in use:
//#define DHTTYPE           DHT11     // DHT 11
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE           DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;


const int TEMPERATURE_INTERVAL = 300;

unsigned long lastTemperatureSent = 0;

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */

HomieNode temperatureNode("temperature", "temperature");
HomieNode humidityNode("humidity", "humidity");

void setupHandler() {
  temperatureNode.setProperty("unit").send("F");
  dht.begin();
  temperatureNode.advertise("unit");
  temperatureNode.advertise("degrees");
  humidityNode.advertise("humidity");
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Homie.getLogger() << sensor.name << endl;
}

void loopHandler() {
  if (millis() - lastTemperatureSent >= TEMPERATURE_INTERVAL * 1000UL || lastTemperatureSent == 0) {
    // Print temperature sensor details.
    sensors_event_t event;
  //  dht.temperature(true).getEvent(&event);
//    dht.temperature().getEvent(&event);
 float temp_f = dht.getTemperature(true);
    if (isnan(temp_f)) {
      Homie.getLogger() << "Error getting Temp" << endl;
    }
    else {
//      float temp_f =  (event.temperature * 9 +2)/5+32;
      //float temp_f = event.temperature;
      Homie.getLogger() << "Temperature: " << temp_f << " °F" << endl;
      temperatureNode.setProperty("degrees").send(String(temp_f));
      lastTemperatureSent = millis();
    }
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Homie.getLogger() << "Error getting Humidity" << endl;
    }
    else {
      Homie.getLogger() << "Temperature: " << event.relative_humidity << " %" << endl;
      humidityNode.setProperty("humidity").send(String(event.relative_humidity));
    }

  }
}

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;
 // Homie_setFirmware("awesome-temperature", "1.0.0");
    Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);


  Homie.setup();
}

void loop() {
  Homie.loop();
}
