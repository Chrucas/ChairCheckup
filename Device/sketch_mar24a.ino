#include <HCSR04.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

/*-------------------------------------------------------------------------------------------------------------------------------|
  This code allows you to know the disctance between an Ultrasonic sensor and an object                                           |     |
  ---------------------------------------------------------------------------------------------------------------------------------|*/
//Start :
//define the pins and authorizations.
//----------------------------------------------------------------------------------------------------------------------
#define trigPin1 D7                                   
#define echoPin1 D6                                                              
#define RESET_pin D3

#define FIREBASE_HOST "chaircheckup-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "aQifoi9T2VLjbryLTDmHMFlDEJXCCIJtYleqEb72"
#define WIFI_SSID "YOUR_WIFI_ID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
//----------------------------------------------------------------------------------------------------------------------

//used variables
//----------------------------------------------------------------------------------------------------------------------
long duration, distance; //we'll use these variable to store and generate data
int triggerPin = 7;
int echoPin = 6;
int count;
boolean counting;
char data;
String SerialData = "";

UltraSonicDistanceSensor distanceSensor(D7, D6);

FirebaseData firebaseData;
String path = "Data/OccupationTime";

int n = 0;
//----------------------------------------------------------------------------------------------------------------------

//Make the setup of your pins
//----------------------------------------------------------------------------------------------------------------------
void setup()
{ // START SETUP FUNCTION
  Serial.println("start setup");
  Serial.begin (115200);                              // we will use the serial data transmission to display the distance value on the serial monitor
  counting = false;


  // setup pins first sensor
  pinMode(trigPin1, OUTPUT);                        // from where we will transmit the ultrasonic wave
  pinMode(echoPin1, INPUT);                         //from where we will read the reflected wave
  pinMode(LED_BUILTIN, OUTPUT);                  // from where we will control the LED

  digitalWrite(RESET_pin, HIGH);                // this is set to making resetting the device easier. 
  digitalWrite(LED_BUILTIN, HIGH);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(firebaseData, path))
  {
    Serial.println("Could not begin stream");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  } else {
    Serial.println("Firebase stream started");
  }
}// END SETUP FUNCTION

void loop()
{
  // START THE LOOP FUNCTION
  if (!Firebase.readStream(firebaseData))
  {
    Serial.println();
    Serial.println("Can't read stream data");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }

  if (firebaseData.streamTimeout())
  {
    Serial.println();
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
  }
  
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration = pulseIn(echoPin1, HIGH);
  distance = duration * 0.031 / 2;
  if (distance <= 40 && !counting) {
    counting = true;
    count++;
    digitalWrite(LED_BUILTIN, LOW); 
  } else if (distance <= 40 && counting) {
    count++;
  } else if (distance >= 40 && counting) {
    counting = false;
      if (!Firebase.pushInt(firebaseData, path, count))
      {
        Serial.println();
        Serial.println("Can't push data");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println();
      }

    Serial.print("time sensed: ");
    Serial.print(count);
    Serial.println(" s");
    digitalWrite(LED_BUILTIN, HIGH);
    count = 0;
  }
  Serial.print("distance measured by the first measurement: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("distance measured by the second measurement: ");
  Serial.println(distanceSensor.measureDistanceCm(16));



  delay(1000);
}//END LOOP FUNTION



/****************************----------------------- END PROGRAM -----------------------****************************/
