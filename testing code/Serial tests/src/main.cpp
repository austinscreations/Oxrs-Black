#include <Arduino.h>
#include <ArduinoJson.h>

/*--------------------------- Version ------------------------------------*/
#define FW_NAME       "OXRS-AC-SERIAL_TEST-ESP32-FW"
#define FW_SHORT_NAME "SERIAL TEST"
#define FW_MAKER      "Austin's Creations"
#define FW_VERSION    "0.0.1"

// Serial
#define SERIAL_BAUD_RATE  115200
#define RS485_BAUD_RATE 9600
#define KNX_BAUD_RATE 9600

// stack size counter (for determine used heap size on ESP8266)
char * g_stack_start;

/*--------------------------- Instantiate Global Objects -----------------*/
// SoftwareSerial RS485Serial(RXPin, TXPin); // RX, TX
HardwareSerial &RS485 = Serial1;
HardwareSerial &KNX = Serial2;

/*--------------------------- Variables -----------------*/

// tracks the progress of messages back and forth to test all RX / TX modes
uint8_t messageStage = 0;

/*--------------------------- JSON builders -----------------*/

void initialiseSerial()
{
  Serial.begin(SERIAL_BAUD_RATE);
  RS485.begin(RS485_BAUD_RATE, SERIAL_8N1, RS485_RX, RS485_TX);
  KNX.begin(KNX_BAUD_RATE, SERIAL_8N1, KNX_RX, KNX_TX);
  delay(1000);
  Serial.println();
  Serial.println(F("\n==============================="));
  Serial.println(FW_NAME);
  Serial.print  (F("            v"));
  Serial.println(FW_VERSION);
  Serial.println(F("==============================="));

  #if(MODE)
    Serial.println(F("\n[MAIN ] starting up..."));
  #else
    Serial.println(F("\n[SEC ] starting up..."));
  #endif
  }

void transmitRS485Test()
{
  #if(MODE)
      delay(2500);
      digitalWrite(RS485_COMMUNICATION_CONTROL_PIN, RS485_TX_PIN_VALUE); // prepare to transmit
      StaticJsonDocument<200> doc1;
      doc1["message"] = "Hello RS485";
      serializeJson(doc1, RS485);
      delay(100);
      messageStage = 1;
  #else
    digitalWrite(RS485_COMMUNICATION_CONTROL_PIN, RS485_RX_PIN_VALUE); // Init receive
      if (RS485.available())
      {
        StaticJsonDocument<400> doc;
        DeserializationError err = deserializeJson(doc, RS485);
        if (err == DeserializationError::Ok)
        {
          // Print the values
          // (we must use as<T>() to resolve the ambiguity)
          Serial.print("\n json = ");
          serializeJson(doc, Serial);
          Serial.println("\n");
          delay(100);
          messageStage = 1;
        }
      }
  #endif
}

void recieveRS485Test()
{
  #if(MODE)
      digitalWrite(RS485_COMMUNICATION_CONTROL_PIN, RS485_RX_PIN_VALUE); // Init receive
      if (RS485.available())
      {
        StaticJsonDocument<400> doc;
        DeserializationError err = deserializeJson(doc, RS485);
        if (err == DeserializationError::Ok)
        {
          // Print the values
          // (we must use as<T>() to resolve the ambiguity)
          Serial.print("\n json = ");
          serializeJson(doc, Serial);
          Serial.println("\n");
          delay(100);
          messageStage = 2;
        }
      }
  #else
      delay(2500);
      digitalWrite(RS485_COMMUNICATION_CONTROL_PIN, RS485_TX_PIN_VALUE); // prepare to transmit
      StaticJsonDocument<200> doc1;
      doc1["message"] = "Bye RS485";
      serializeJson(doc1, RS485);
      delay(100);
      messageStage = 2;
  #endif
}

void transmitKnxTest()
{
  #if(MODE)
      delay(2500);
      StaticJsonDocument<200> doc1;
      doc1["message"] = "Hello KNX";
      serializeJson(doc1, KNX);
      Serial.println("KNX sent");
      delay(100);
      messageStage = 3;
  #else
      if (KNX.available())
      {
        Serial.println("KNX recieve");
        StaticJsonDocument<400> doc;
        DeserializationError err = deserializeJson(doc, KNX);
        if (err == DeserializationError::Ok)
        {
          // Print the values
          // (we must use as<T>() to resolve the ambiguity)
          Serial.print("\n json = ");
          serializeJson(doc, Serial);
          Serial.println("\n");
          delay(100);
          messageStage = 3;
        }
      }
  #endif
}

void recieveKnxTest()
{
  #if(MODE)
      if (KNX.available())
      {
        StaticJsonDocument<400> doc;
        DeserializationError err = deserializeJson(doc, KNX);
        if (err == DeserializationError::Ok)
        {
          // Print the values
          // (we must use as<T>() to resolve the ambiguity)
          Serial.print("\n json = ");
          serializeJson(doc, Serial);
          Serial.println("\n");
          delay(100);
          messageStage = 5;
        }
      }
  #else
      delay(2500);
      StaticJsonDocument<200> doc1;
      doc1["message"] = "Bye KNX";
      serializeJson(doc1, KNX);
      delay(100);
      messageStage = 5;
  #endif
}


void setup() {

  initialiseSerial();

  pinMode(RS485_COMMUNICATION_CONTROL_PIN, OUTPUT);
  digitalWrite(RS485_COMMUNICATION_CONTROL_PIN, RS485_RX_PIN_VALUE);

}

void loop() {

  if(messageStage == 0)
  {
    transmitRS485Test();
  }
  if(messageStage == 1)
  {
    recieveRS485Test();
  }
  if(messageStage == 2)
  {
    transmitKnxTest();
  }
  if(messageStage == 3)
  {
    recieveKnxTest();
  }

  
}