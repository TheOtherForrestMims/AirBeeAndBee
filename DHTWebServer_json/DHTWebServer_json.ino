/*
We would like to Thank ARDUINO for providing us with example code for us to
start this project

  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 */

#include <SPI.h>
#include <Ethernet.h>

int val = 0; // Carbon Monoxide VAR
int val2 = 0; // Alcohol VAR
int val3 = 0; // Methane VAR

int PIN = 0; // Carbon Monoxide Analog Pin In
int PINPIN = 1; // Alcohol Analog Pin In
int PINPINPIN = 2; // Methan Analog Pin In

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"

#define DHTPIN 2     // what pin we're connected to
// for the TEmp and Humidty

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

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

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 120);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
//EthernetServer server(80);
// mitchilllz i change the port to 81!!!!!
EthernetServer server(81); 
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("DHTxx test!");

  dht.begin();


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  pinMode(PIN, INPUT); // (A0)
  pinMode(PINPIN, INPUT); // (A1)
  pinMode(PINPINPIN, INPUT); // (A2)
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();

  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);

  int val = analogRead(PIN);
  int val2 = analogRead(PINPIN);
  int val3 = analogRead(PINPINPIN);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  //float hi = dht.computeHeatIndex(f, h);


  ///////////////////////////////////////////
  // CHECKS AND POSTS VIA SERIAL
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("CO: ");
  Serial.print(val);
  Serial.print(" *F\t");
  Serial.print("Drinks: ");
  Serial.print(val2);
  Serial.print(" *F\t");
  Serial.print("Farts: ");
  Serial.print(val3);
  Serial.print(" *F\t");
  //  Serial.print("Heat index: ");
  //  Serial.print(hi);
  //  Serial.println(" *F");

/////////////////////////////////////////////
// ETHERNET HTML DATA 
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          // the connection will be closed after completion of the response
          client.println("Connection: close");  
          client.println();
          client.print("{");
          client.print("\n  \"tempc\": "); client.print(t);
          client.print(",\n  \"tempf\": "); client.print(f);
          client.print(",\n  \"rh\": "); client.print(h);
          client.print(",\n  \"co\": "); client.print(val);
          client.print(",\n  \"etoh\": "); client.print(val2);
          client.print(",\n  \"methane\": "); client.print(val3);
          client.print("\n}\n");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

