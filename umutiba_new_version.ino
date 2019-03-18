#include "HX711.h"
#include "ESP8266WiFi.h"
float calibration_factor = 190077; // this calibration factor is adjusted according to my load cell
float units;
float ounces;
// Define our data and clock pins
#define DOUT 14 // D2 maps to GPIO4
#define CLK 12 // D1 maps to GPIO5
// Initialize our HX711 interface
HX711 scale(DOUT, CLK);
// WiFi network name and password
const char* ssid = "FABLAB_2.4G"; // your wifi SSID name
const char* password = "innovate";// wifi pasword
const char* server = "api.thingspeak.com";
WiFiClient client;
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());
  scale.set_scale();
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {
  scale.set_scale(calibration_factor); //Adjust to this calibration factor

  Serial.print("Reading: ");
  units = scale.get_units(), 10;
  if (units < 0)
  {
    units = 0.00;
  }
  ounces = units * 0.035274;
  //  Serial.print(units);
  //  Serial.print(" grams");


  float data = units;
  Serial.println(data);
  Serial.print(" grams");
  Serial.println();
  sendWeight(data);

  //delay(1000);
}
void sendWeight(float data) {
  if (client.connect(server, 80)) {
    String apiKey = "7O63U4GNTKJ8JBDP";

    String body = "field1=" + String(data, 2);

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(body.length());
    client.print("\n\n");
    client.print(body);
    client.print("\n\n");
    //Serial.println("DONE");
  }
  client.stop();
  Serial.println("Waiting...");
  delay(3000);
}
