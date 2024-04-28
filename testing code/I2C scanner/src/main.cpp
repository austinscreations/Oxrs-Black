#include <Arduino.h>
#include <Wire.h>

/*--------------------------- Version ------------------------------------*/
#define FW_NAME       "OXRS-AC-ETH_TEST-ESP32-FW"
#define FW_SHORT_NAME "I2C TEST"
#define FW_MAKER      "Austin's Creations"
#define FW_VERSION    "0.0.1"

// Serial
#define SERIAL_BAUD_RATE  115200

// stack size counter (for determine used heap size on ESP8266)
char * g_stack_start;

/*--------------------------- I2C Ready Check -----------------*/
bool checkI2Cbus()
{
  bool ready = digitalRead(I2C_READY);
  return ready;
}

/*--------------------------- I2C Scanner -----------------*/

void runI2Cscan()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }

  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

void scanI2CBus()
{
  #if(I2C_PROTECTED)
    bool checkI2C = checkI2Cbus();
    if(!checkI2C)
    {
      Serial.println("I2C not ready\n");
    }
    else
    {
      Serial.println("I2C ready\n");
      runI2Cscan();
    }
  #else
    runI2Cscan();
  #endif

  delay(5000);
}

/*--------------------------- JSON builders -----------------*/

void initialiseSerial()
{
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);
  Serial.println();
  Serial.println(F("\n==============================="));
  Serial.println(FW_NAME);
  Serial.print  (F("            v"));
  Serial.println(FW_VERSION);
  Serial.println(F("==============================="));
}


void setup() {

  initialiseSerial();

  pinMode(I2C_READY, INPUT_PULLUP);

  Wire.begin(I2C_SDA,I2C_SCL,100000);

}

void loop() {
  
  scanI2CBus();

}