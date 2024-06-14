#include <SPI.h>
#include <mcp2515.h>
#include <SoftwareSerial.h>

SoftwareSerial HMISerial(7, 6);
struct can_frame canMsg;
MCP2515 mcp2515(10);

int speedRead = 0;
int tempValue = 0;
int currentValue = 0;
int voltageIn = 0;

// Definisi konstanta
const float voltageConversionFactor = 0.001885;
int voltaseMaksimal = 84;
int voltaseMinimal = 63;

void setup() {
  Serial.begin(115200);
  HMISerial.begin(9600);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    if (canMsg.can_id == 0x80000926) {
      // Kode kecepatan
      for (int i = 0; i < 4; i++) {
        Serial.print(canMsg.data[i], HEX);
        Serial.print(" ");
      }
      long decimalValueSpeed = convertHexToDecimal(canMsg.data, 0,4);
      speedRead = 32 * (decimalValueSpeed / 10) * voltageConversionFactor;
      Serial.print("Kecepatan (Km/h) : ");
      Serial.println(abs(speedRead));
      HMISerial.print("speed.val=");
      HMISerial.print(abs(speedRead));
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      HMISerial.write(0xff);
    } else if (canMsg.can_id == 0x80001B26) {
      // Kode Voltage
      for (int i = 4; i < 6; i++) {
        Serial.print(canMsg.data[i], HEX);
        Serial.print(" ");
      }
      long decimalValueVoltage = convertHexToDecimal(canMsg.data, 4, 6);
      voltageIn = decimalValueVoltage / 10;
      Serial.print("Voltage (V) : ");
      Serial.println(abs(voltageIn));
      HMISerial.print("voltage.val=");
      HMISerial.print(abs(voltageIn));
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      int atas = (voltageIn - voltaseMinimal) * 100;
      int bawah = voltaseMaksimal - voltaseMinimal;
      int percent= atas/bawah;
      HMISerial.print("value.val=");
      HMISerial.print(abs(percent));
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      HMISerial.write(0xff);
    }else if (canMsg.can_id == 0x80001026) {
      // Kode suhu
      for (int i = 0; i < 2; i++) {
        Serial.print(canMsg.data[i], HEX);
        Serial.print(" ");
      }
      long decimalValueTemp = convertHexToDecimal(canMsg.data, 0,2);
      tempValue = decimalValueTemp;
      Serial.print("Temperature (Celcius) : ");
      Serial.println(abs(tempValue));
      HMISerial.print("temp.val=");
      HMISerial.print(abs(tempValue));
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      HMISerial.write(0xff);

      // Kode Current
      for (int i = 4; i < 6; i++) {
        Serial.print(canMsg.data[i], HEX);
        Serial.print(" ");
      }
      long decimalValueCurrent = convertHexToDecimal(canMsg .data, 4, 6);
      currentValue = decimalValueCurrent;
      Serial.print("Current (A) : ");
      Serial.println(abs(currentValue));
      HMISerial.print("current.val=");
      HMISerial.print(abs(currentValue));
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      HMISerial.write(0xff);
    }
    delay(10);
  }
}

long convertHexToDecimal(byte hexData[], int startIndex, int endIndex) {
  String hexString = "";
  for (int i = startIndex; i < endIndex; i++) {
    hexString += String(hexData[i], HEX);
  }
  return strtol(hexString.c_str(), NULL, 16);
}
