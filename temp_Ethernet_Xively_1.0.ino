#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>
#include "DHT.h"

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11  (AM2302)

// MAC address for your Ethernet shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Your Xively key to let you upload data
char xivelyKey[] = "Place Key Here";

// Define the strings for our datastream IDs
char sensorId[] = "temp01";
char sensorId2[] = "hum01";

XivelyDatastream datastreams[] = {
  XivelyDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT), 
  XivelyDatastream(sensorId2, strlen(sensorId2), DATASTREAM_FLOAT),
};
// Finally, wrap the datastreams into a feed
XivelyFeed feed(YOURFEEDHERE datastreams, 2 /* number of datastreams */);

EthernetClient client;
XivelyClient xivelyclient(client);

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
// NOTE: For working with a faster chip, like an Arduino Due or Teensy, you
// might need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// Example to initialize DHT sensor for Arduino Due:
//DHT dht(DHTPIN, DHTTYPE, 30);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  Serial.println("Starting single datastream upload to Xively...");
  Serial.println();
  
  dht.begin();
  
  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }
}

void loop() {
  
    // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  //int sensorValue = t;
  datastreams[0].setFloat(f);
  datastreams[1].setFloat(h);
  Serial.print("Read sensor value ");
  Serial.println(datastreams[0].getFloat());
  Serial.println(datastreams[1].getFloat());
  
  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);

  Serial.println();
  delay(15000);
}

