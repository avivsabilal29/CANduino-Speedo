#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
#define LED_PIN1 19 // Pin LED 1
#define LED_PIN2 18 // Pin LED 2
#define LED_PIN3 5 // Pin LED 3
#define LED_PIN4 17 // Pin LED 4
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Alamat I2C dan ukuran LCD (20 kolom x 4 baris)

const int trigPin = 26;
const int echoPin = 25;
const int buzzer = 27; //buzzer to arduino pin 9
long duration;
int distance;

// Insert your network credentials
#define WIFI_SSID "Galaxy A21s4151"
#define WIFI_PASSWORD "avav293227"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBTPoD5A-8KDFBV0ycFKhJGHXyQxcUiMOU"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://parking-ezz-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

void setup(){
  lcd.begin(); // Menginisialisasi LCD
  lcd.backlight();  
  lcd.clear();
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); 
  pinMode(buzzer, OUTPUT);             // Aktifkan backlight LCD          // Posisi kursor di baris pertama, kolom pertama 
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  pinMode(LED_PIN4, OUTPUT);
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
     digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      // Sets the trigPin on HIGH state for 10 micro seconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(2);
      digitalWrite(trigPin, LOW);
      // Reads the echoPin, returns the sound wave travel time in microseconds
      duration = pulseIn(echoPin, HIGH);
      // Calculating the distance
      distance = duration * 0.034 / 2;
    // Write an Int number on the database path test/int
    Serial.print("Distance: ");
    Serial.println(distance);

    if (distance < 50) {
          if (Firebase.RTDB.setInt(&fbdo, "parthners/1415f07a-6514-444e-8cb2-aba67c551d1c/floor/5e73d19d-9318-48ca-8735-87d5c5124199/parking/-Ne52XLRzsVOjgNiNWG8/statusValue", 1)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        }
        else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }
        } else if (distance > 50) {
          if (Firebase.RTDB.setInt(&fbdo, "parthners/1415f07a-6514-444e-8cb2-aba67c551d1c/floor/5e73d19d-9318-48ca-8735-87d5c5124199/parking/-Ne52XLRzsVOjgNiNWG8/statusValue", 0)){
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        }
        }
    
if (Firebase.RTDB.getInt(&fbdo, "parthners/1415f07a-6514-444e-8cb2-aba67c551d1c/floor/5e73d19d-9318-48ca-8735-87d5c5124199/parking/-Ne52XLRzsVOjgNiNWG8/booked")) {
  if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_integer) {
    int booked = fbdo.to<int>();
    
    if (Firebase.RTDB.getInt(&fbdo, "parthners/1415f07a-6514-444e-8cb2-aba67c551d1c/floor/5e73d19d-9318-48ca-8735-87d5c5124199/parking/-Ne52XLRzsVOjgNiNWG8/statusValue")) {
      if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_integer) {
        int statusValue = fbdo.to<int>();

        if (Firebase.RTDB.getInt(&fbdo, "parthners/1415f07a-6514-444e-8cb2-aba67c551d1c/floor/5e73d19d-9318-48ca-8735-87d5c5124199/parking/-Ne52XLRzsVOjgNiNWG8/verifId")) {
      if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_integer) {
        int verifId = fbdo.to<int>();
        
        if (booked == 2 && statusValue == 1 && verifId == 1) {
          // booked == 1 dan statusValue == 1
          digitalWrite(LED_PIN1, LOW);
          digitalWrite(LED_PIN2, LOW);
          digitalWrite(LED_PIN3, HIGH);
          digitalWrite(LED_PIN4, LOW);
          digitalWrite(buzzer, LOW);
        } else if (booked == 1 && statusValue == 1 && verifId == 0) {
          // booked == 0 dan statusValue == 1
          digitalWrite(LED_PIN1, LOW);
          digitalWrite(LED_PIN2, HIGH);
          digitalWrite(LED_PIN3, LOW);
          digitalWrite(LED_PIN4, LOW);
          digitalWrite(buzzer, LOW);
        }
        else if ((booked == 1 || booked == 3) && statusValue == 1 && verifId == 2) {
          // booked == 0 dan statusValue == 1
          digitalWrite(LED_PIN1, LOW);
          digitalWrite(LED_PIN2, LOW);
          digitalWrite(LED_PIN3, LOW);
          digitalWrite(LED_PIN4, HIGH);
          digitalWrite(buzzer, HIGH);// Send 1KHz sound signal...
        }
        else if (booked == 0 && statusValue == 1 && verifId == 0) {
          // booked == 0 dan statusValue == 1
          digitalWrite(LED_PIN1, HIGH);
          digitalWrite(LED_PIN2, LOW);
          digitalWrite(LED_PIN3, LOW);
          digitalWrite(LED_PIN4, LOW);
          digitalWrite(buzzer, LOW);
        } else if (booked == 1 && statusValue == 0 && verifId == 0) {
          // booked == 1 dan statusValue == 0
          digitalWrite(LED_PIN1, LOW);
          digitalWrite(LED_PIN2, HIGH);
          digitalWrite(LED_PIN3, LOW);
          digitalWrite(LED_PIN4, LOW);
          digitalWrite(buzzer, LOW);
        } else {
          // Kasus lainnya
          digitalWrite(LED_PIN1, HIGH); // Sesuaikan dengan logika Anda
          digitalWrite(LED_PIN2, LOW); // Sesuaikan dengan logika Anda
          digitalWrite(LED_PIN3, LOW); // Sesuaikan dengan logika Anda
          digitalWrite(LED_PIN4, LOW); // Sesuaikan dengan logika Anda
          digitalWrite(buzzer, LOW);
        }
      } else {
        Serial.println("Data type is not an integer for statusValue.");
      }
    }

      } else {
        Serial.println("Data type is not an integer for statusValue.");
      }
    } else {
      Serial.println("Failed to get statusValue from Firebase.");
      Serial.println("Reason: " + fbdo.errorReason());
    }
  } else {
    Serial.println("Data type is not an integer for booked.");
  }
}

    
if (Firebase.RTDB.getString(&fbdo, "parthners/1415f07a-6514-444e-8cb2-aba67c551d1c/floor/5e73d19d-9318-48ca-8735-87d5c5124199/parking/-Ne52XLRzsVOjgNiNWG8/name")) {
  if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_string) {
    String user = fbdo.to<String>();
    
    if (Firebase.RTDB.getInt(&fbdo, "parthners/1415f07a-6514-444e-8cb2-aba67c551d1c/floor/5e73d19d-9318-48ca-8735-87d5c5124199/parking/-Ne52XLRzsVOjgNiNWG8/idParkir")) {
      if (fbdo.dataTypeEnum() ==firebase_rtdb_data_type_string) {
        String idParkir = fbdo.to<String>();

        if (Firebase.RTDB.getString(&fbdo, "parthners/1415f07a-6514-444e-8cb2-aba67c551d1c/floor/5e73d19d-9318-48ca-8735-87d5c5124199/parking/-Ne52XLRzsVOjgNiNWG8/title")) {
      if (fbdo.dataTypeEnum() == firebase_rtdb_data_type_string) {
        String place = fbdo.to<String>();
                lcd.setCursor(0, 0);
                lcd.print("Name: ");
                lcd.print(user);
                lcd.setCursor(0, 1);
                lcd.print("Token: ");
                lcd.print(idParkir);
                lcd.setCursor(0, 2);
                lcd.print("Tempat: ");
                lcd.print(place);
      } else {
        Serial.println("Data type is not an integer for statusValue.");
      }
    }

      } else {
        Serial.println("Data type is not an integer for statusValue.");
      }
    } else {
      Serial.println("Failed to get statusValue from Firebase.");
      Serial.println("Reason: " + fbdo.errorReason());
    }
  } else {
    Serial.println("Data type is not an integer for booked.");
  }
}
else {
  Serial.println("Failed to get booked from Firebase.");
  Serial.println("Reason: " + fbdo.errorReason());
}
                
    
  }

}
