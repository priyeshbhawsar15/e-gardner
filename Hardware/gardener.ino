// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
#include "DHT.h"
#define DHTTYPE DHT11
int DHTPIN = D4;
DHT dht(DHTPIN, DHTTYPE);


int sensorPin = A0; 
int mositValue;  
int limit = 300;
int enA = D5;
int in1 = D6;
int in2 = D7;



#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define WIFI_SSID "Gatloo"
#define WIFI_PASSWORD "17121972"
#define USER_EMAIL "priyeshbhawsar15@gmail.com"
#define USER_PASSWORD "123456778"
#define FIREBASE_HOST "https://gardener-2c47a-default-rtdb.firebaseio.com"
#define API_KEY "AIzaSyDjXf1DKnn_TV5Ay6VmXfOAyAtvEo1EjQU"
#define FIREBASE_AUTH "vSdnQ5L8qdcqq9rc377gCGfBJ8Geuq7fxkrMbuK0"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson updateData;
FirebaseJson json;


void setup() {
  Serial.begin(9600);
  dht.begin();
  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  // Turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    config.host = FIREBASE_HOST;
    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    fbdo.setResponseSize(4096);

    Firebase.reconnectWiFi(true);

    Firebase.begin(&config, &auth);
}
  

void loop() {

  moistValue = analogRead(sensorPin); 
  Serial.println("Analog Value : ");
  Serial.println(mositValue); 


  delay(500);
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
  if (isnan(humidity) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print(F(" Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temp);


  pumpwater();


  if (Firebase.getInt(fbdo, "/app/pwm")) {
  
      if (fbdo.dataType() == "int") {
        Serial.println(fbdo.intData());
        if (fbdo.intData()==255) {
          pumpwater();
          updateData.set("pwm", 0);
         }
  
    } else {
      Serial.println(fbdo.errorReason());
    }

    
  updateData.set("Humidity", humidity);
  updateData.set("Moisture", moistValue);
  updateData.set("Temp", temp);
  
  if (Firebase.updateNode(fbdo, "/app", updateData)) {
  
    Serial.println(fbdo.dataPath());
  
    Serial.println(fbdo.dataType());
  
    Serial.println(fbdo.jsonString()); 
  
  } else {
    Serial.println(fbdo.errorReason());
  }
  delay(2000);
}


void pumpwater() {
  // Set motors to maximum speed
  // For PWM maximum possible values are 0 to 255
  analogWrite(enA, 255);

  // Turn on motor A & B
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(5000);
  
  // Turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}
