#include <Arduino.h>
#include <WiFi.h>
#include <ADS1219.h>
#include "ModbusServerWiFi.h"
#include "EEPROM.h"

#define DRDY 4
ADS1219 ads(DRDY);

union {
  float asFloat;
  uint16_t asInt[2];
} flreg;

ModbusServerWiFi MBserver;


#include "HardwareSerial.h"
#include "ModbusServerRTU.h"
ModbusServerRTU MBserver_rtu(Serial2, 2000);

ModbusServerRTU MBserver_rtu2(Serial, 2000);

#define A 3.9083 * pow(10,-3)
#define B -5.775 * pow(10,-7)
#define C -4.183 * pow(10,-12)

#define RST 13
#define DAC1 25
#define PIN_A 14
#define PIN_B 27


//----------------------coils defination---------------
#define LAMP_PIN 15
#define EXIT_PIN 5
#define EXHAUST_PIN 18
const uint8_t RelayPins[3] = {LAMP_PIN, EXHAUST_PIN, EXIT_PIN}; //coil 0,1,2,3,4;

#define DECIMAL_BIT 5
#define TEST_START_BIT 6
#define CLEAR_DATA_BIT 7
//-------------------------------------------------------
//----------------------register defination--------------
//floats
#define SDR_REG 2
#define ABS_REG 4
#define TEMP_REG 6
#define VOLT_REG 8
#define ABS_TABLE_REG 10

//integer
#define DC_VOLTS_WRITE_REG 12
#define COUNTER_REG 13
#define STATUS_REG 14
#define FUNCTION_REG 15

#define MV0_REG 16
#define MV100_REG 17
#define MV_REG 18
#define BIT_REG 19
#define DURATION_REG 20   // test length in seconds; 0 = default 240 s
#define DEFAULT_TEST_SECONDS 240
//--------------------------------------------------------


const uint8_t MY_SERVER(1);  // server id = 1;

const uint8_t nCoils_byte = 2;
bool MBcoils[nCoils_byte * 8] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


const uint8_t nInputRegisters = 25;
uint16_t MBregisters[nInputRegisters];  // Allocate memory for registers

float para[4] = {1.5, 2.5, 3.5, 4.5};
float pre_res, pre_volts;
float _SDR = 0;

//const char* ssid     = "Railwire";
//const char* password = "12345678";

const char* ssid     = "ESP32_rehan";
const char* password = "12345678";

long int lt = 0, lt1 = 0 ;
bool writeEEPROM = false;
uint16_t testLen = DEFAULT_TEST_SECONDS;  // test length latched when STATUS is reset to 0

uint16_t requestedTestLen() {
  return MBregisters[DURATION_REG] ? MBregisters[DURATION_REG] : DEFAULT_TEST_SECONDS;
}


void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);

  ads.begin();
  pinMode(PIN_A, OUTPUT);           // to handle ADC multiple channel
  pinMode(PIN_B, OUTPUT);           // to handle ADC multiple channel

  pinMode(DRDY, INPUT_PULLUP);
  pinMode(RST, OUTPUT);
  digitalWrite(RST, LOW);
  delay(200);
  digitalWrite(RST, HIGH);
  delay(200);
  Wire.begin();
  ads.setVoltageReference(REF_INTERNAL);

  //setup initl vaues

  EEPROM.begin(512);
  EEPROM.get(2 * MV0_REG, MBregisters[MV0_REG]);
  EEPROM.get(2 * MV100_REG, MBregisters[MV100_REG]);
  EEPROM.get(2 * DC_VOLTS_WRITE_REG, MBregisters[DC_VOLTS_WRITE_REG]);


  for (int i = 0; i < 3; i++) {
    pinMode(RelayPins[i], OUTPUT);
    digitalWrite(RelayPins[i], !bitRead(MBregisters[BIT_REG], i));
  }

  //MBcoils[TEST_START_BIT] = 0;
  testLen = requestedTestLen();
  MBregisters[COUNTER_REG] = testLen;
  dacWrite(DAC1, MBregisters[DC_VOLTS_WRITE_REG]);

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  //  Serial.print("AP IP address: ");
  //  Serial.println(myIP);

  //    Serial.println();
  //    Serial.print("Connecting to ");
  //    Serial.println(ssid);
  //
  //    WiFi.begin(ssid, password);
  //
  //    while (WiFi.status() != WL_CONNECTED) {
  //        delay(500);
  //        Serial.print(".");
  //    }
  //
  //    Serial.println("");
  //    Serial.println("WiFi connected");
  //    Serial.println("IP address: ");
  //    Serial.println(WiFi.localIP());




  MBserver.registerWorker(MY_SERVER, READ_HOLD_REGISTER, &FC03);
  // Register the worker function again for another FC
  MBserver.registerWorker(MY_SERVER, READ_INPUT_REGISTER, &FC03);
  // And write holding registers
  MBserver.registerWorker(MY_SERVER, WRITE_HOLD_REGISTER, &FC06);

  MBserver.registerWorker(MY_SERVER, WRITE_MULT_REGISTERS, &FC10);

  //  MBserver.registerWorker(MY_SERVER, READ_COIL, &FC01);
  //  MBserver.registerWorker(MY_SERVER, WRITE_COIL, &FC05);

  // Port number 502, maximum of 4 clients in parallel, 10 seconds timeout
  MBserver.start(502, 4, 0);


  MBserver_rtu.registerWorker(MY_SERVER, READ_HOLD_REGISTER, &FC03);
  // Register the worker function again for another FC
  MBserver_rtu.registerWorker(MY_SERVER, READ_INPUT_REGISTER, &FC03);
  // And write holding registers
  MBserver_rtu.registerWorker(MY_SERVER, WRITE_HOLD_REGISTER, &FC06);

  MBserver_rtu.registerWorker(MY_SERVER, WRITE_MULT_REGISTERS, &FC10);

  //  MBserver_rtu.registerWorker(MY_SERVER, READ_COIL, &FC01);
  //  MBserver_rtu.registerWorker(MY_SERVER, WRITE_COIL, &FC05);

  MBserver_rtu.start();


  MBserver_rtu2.registerWorker(MY_SERVER, READ_HOLD_REGISTER, &FC03);
  // Register the worker function again for another FC
  MBserver_rtu2.registerWorker(MY_SERVER, READ_INPUT_REGISTER, &FC03);
  // And write holding registers
  MBserver_rtu2.registerWorker(MY_SERVER, WRITE_HOLD_REGISTER, &FC06);

  MBserver_rtu2.registerWorker(MY_SERVER, WRITE_MULT_REGISTERS, &FC10);
  //  MBserver_rtu2.registerWorker(MY_SERVER, READ_COIL, &FC01);
  //  MBserver_rtu2.registerWorker(MY_SERVER, WRITE_COIL, &FC05);
  MBserver_rtu2.start();

  //put test values;
  flreg.asFloat = para[0];
  MBregisters[SDR_REG] = flreg.asInt[0];
  MBregisters[SDR_REG + 1] = flreg.asInt[1];

  flreg.asFloat = para[1];
  MBregisters[ABS_REG] = flreg.asInt[0];
  MBregisters[ABS_REG + 1] = flreg.asInt[1];

  flreg.asFloat = para[2];
  MBregisters[TEMP_REG] = flreg.asInt[0];
  MBregisters[TEMP_REG + 1] = flreg.asInt[1];

  flreg.asFloat = para[3];
  MBregisters[VOLT_REG] = flreg.asInt[0];
  MBregisters[VOLT_REG + 1] = flreg.asInt[1];

}

void loop() {
  // put your main code here, to run repeatedly:

  if (millis() - lt1 > 1000) {
    lt1 = millis();
    //    Serial.print(MBserver.activeClients());
    //    Serial.println(" clients running");

    if (MBregisters[STATUS_REG] == 1) {
      if (MBregisters[COUNTER_REG] == 0) {
        MBregisters[STATUS_REG] = 2;
      } else {
        MBregisters[COUNTER_REG]--;
      }

      if ((testLen - MBregisters[COUNTER_REG]) % 15 == 0) {
        MBregisters[ABS_TABLE_REG] = MBregisters[ABS_REG];
        MBregisters[ABS_TABLE_REG + 1] = MBregisters[ABS_REG + 1];
      }
    }
  }

  if (millis() - lt > 500) {
    lt = millis();

    if (writeEEPROM) {
      writeEEPROM = false;
      EEPROM.put(2 * DC_VOLTS_WRITE_REG, MBregisters[DC_VOLTS_WRITE_REG]);
      EEPROM.put(2 * MV0_REG, MBregisters[MV0_REG]);
      EEPROM.put(2 * MV100_REG, MBregisters[MV100_REG]);
      EEPROM.commit();
    }

    //Serial.println(getABS());

    flreg.asFloat = getABS();
    MBregisters[ABS_REG] = flreg.asInt[0];
    MBregisters[ABS_REG + 1] = flreg.asInt[1];

    flreg.asFloat = _SDR;
    MBregisters[SDR_REG] = flreg.asInt[0];
    MBregisters[SDR_REG + 1] = flreg.asInt[1];

    flreg.asFloat = resToTemp(getRT());
    MBregisters[TEMP_REG] = flreg.asInt[0];
    MBregisters[TEMP_REG + 1] = flreg.asInt[1];

    float v = getSupplyVolts();
    flreg.asFloat = v;
    MBregisters[VOLT_REG] = flreg.asInt[0];
    MBregisters[VOLT_REG + 1] = flreg.asInt[1];

    //Serial.println(v,3);


    //    flreg.asInt[0] = MBregisters[SDR_REG];
    //    flreg.asInt[1] = MBregisters[SDR_REG + 1];
    //
    //    Serial.println(flreg.asFloat);

  }

}
