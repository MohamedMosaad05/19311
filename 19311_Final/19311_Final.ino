#include <WiFi.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

#define SECRET_SSID "MOSAAD"    // replace MySSID with your WiFi network name
#define SECRET_PASS "MOSAADMOSAADMOSAAD"  // replace MyPassword with your WiFi password
#define SECRET_CH_ID 1962721     // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "YFGI5BM2GB9GZLNZ"   // replace XYZ with your channel write API Key
#define mqPin 34                                  // Assign pin number 35 for MQ135
#define phPin 35                                  // Assign pin number 34 for pH Sensor

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
int statusCode = 0;
int co2lvl, SensorValue[10];
//int Calib_VAL = 1045;
char j, k;
long result;
float ph;
float Value=0;
String myStatus = "";

void setup()
{
  Serial.begin(115200);
  //delay(10);
  
  pinMode(mqPin, INPUT);
  pinMode(phPin,INPUT);
  
  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop()
{

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
  
  // Read the CO2 level
    for (j = 0; j < 10; j++) {
      SensorValue[j] = analogRead(mqPin);
      delay(20);
    }

  // We now have 10 readings, so average them
    result = 0;
    for (j = 0; j < 10; j++) {
      result = result + SensorValue[j]; // add them up
    }
  
    result = result / 10;               // this is our averaged result
    co2lvl = result - 400 ;
    Serial.print("CO2 LEVEL = ");
    Serial.print(co2lvl);
    Serial.println(" PPM");

  //  Read the pH value
    Value= analogRead(phPin);
    float voltage=Value*(3.3/4095.0);
    ph=(3.3*voltage);
    Serial.print(" | ph Val = ");
    Serial.println(ph);

  ThingSpeak.setStatus(myStatus);

  // set the fields with the values
  ThingSpeak.setField(1, co2lvl);
  ThingSpeak.setField(2, ph);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(5000); // Wait 20 seconds to update the channel again

}
