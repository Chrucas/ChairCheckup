#include <HCSR04.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

/*-------------------------------------------------------------------------------------------------------------------------------|
  This code allows you to know the disctance between an Ultrasonic sensor and an object                                           |     |
  ---------------------------------------------------------------------------------------------------------------------------------|*/
//Start :
//define the pins that we will use for the first ultrasonic sensor
//----------------------------------------------------------------------------------------------------------------------
#define trigPin1 D7                                   //pin number 9 in arduino MEGA2560
#define echoPin1 D6                                   // we'll use this pin to read the signal from the first sensor
#define LED_first_ping D2                            // I/O digital or analogique ( we will use pin 22 to command an LED (on/off))
#define RESET_pin D3

#define FIREBASE_HOST "chaircheckup-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "aQifoi9T2VLjbryLTDmHMFlDEJXCCIJtYleqEb72"
#define WIFI_SSID "babelfish3"
#define WIFI_PASSWORD "10916109161091610916"
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
  pinMode(LED_first_ping, OUTPUT);                  // from where we will control the LED

  //inisialize LED status
  digitalWrite(LED_first_ping, LOW);

  digitalWrite(RESET_pin, HIGH);

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

//write the code in the loop function
void loop()
{
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
  // START THE LOOP FUNCTION
  digitalWrite(LED_first_ping, HIGH);
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration = pulseIn(echoPin1, HIGH);
  distance = duration * 0.031 / 2;
  if (distance <= 90 && !counting) {
    counting = true;
    count++;
  } else if (distance <= 90 && counting) {
    count++;
  } else if (distance >= 90 && counting) {
    counting = false;
//    if (firebaseData.streamAvailable())
//    {
      if (!Firebase.pushInt(firebaseData, path, count))
      {
        Serial.println();
        Serial.println("Can't push data");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println();
      }
      //      Firebase.pushInt(firebaseData, path, count);
      //      Serial.println("Time pushed to database");
//      Serial.println("Stream available");
//    } else {
//        Serial.println();
//        Serial.println("stream not available");
//        Serial.println("REASON: " + firebaseData.errorReason());
//        Serial.println();
//    }

    Serial.print("time sensed: ");
    Serial.print(count);
    Serial.println(" s");
    count = 0;
    //TODO:send data
  }
  Serial.print("distance measured by the first measurement: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("distance measured by the second measurement: ");
  Serial.println(distanceSensor.measureDistanceCm(16));



  delay(1000);
}//END LOOP FUNTION



/****************************----------------------- END PROGRAM -----------------------****************************/
