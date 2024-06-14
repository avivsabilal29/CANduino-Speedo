#include <SPI.h>
#include <mcp2515.h>
#include <SoftwareSerial.h>

SoftwareSerial HMISerial(6, 7);
struct can_frame canMsg;
MCP2515 mcp2515(10);

int speedRead = 0;
int tempValue = 0;
int currentValue = 0;
int voltageIn = 0;
int voltaseMaksimal = 84;
int voltaseMinimal = 63;


void setup() {
  Serial.begin(115200);
  HMISerial.begin(9600);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS);
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
      // Convert hex data to decimal and print
      long decimalValueSpeed = convertHexToDecimalSpeed(canMsg.data);
      speedRead = 32 * (decimalValueSpeed / 10) * 0.001885;
      Serial.print("Kecepatan (Km/h) : ");
      Serial.println(abs(speedRead));
      HMISerial.print("speed.val=");
      HMISerial.print(abs(speedRead));
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      HMISerial.write(0xff);
    } else if (canMsg.can_id == 0x80001026) {
      // Kode suhu
      for (int i = 0; i < 2; i++) {
        Serial.print(canMsg.data[i], HEX);
        Serial.print(" ");
      }
      long decimalValueTemp = convertHexToDecimalTemp(canMsg.data);
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
      long decimalValueCurrent = convertHexToDecimalCurrent(canMsg.data);
      currentValue = decimalValueCurrent;
      Serial.print("Current (A) : ");
      Serial.println(abs(currentValue));
      HMISerial.print("current.val=");
      HMISerial.print(abs(currentValue));
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      HMISerial.write(0xff);
    } else if (canMsg.can_id == 0x80001B26) {
      // Kode Voltage
      for (int i = 4; i < 6; i++) {
        Serial.print(canMsg.data[i], HEX);
        Serial.print(" ");
      }
      // Convert hex data to decimal and print
      long decimalValueVoltage = convertHexToDecimalVoltage(canMsg.data);
      voltageIn = decimalValueVoltage / 10;
      Serial.print("Voltage (V) : ");
      Serial.println(abs(voltageIn));
      HMISerial.print("voltage.val=");
      HMISerial.print(abs(voltageIn));
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      float persentase = ((voltageIn - voltaseMinimal) / (voltaseMaksimal - voltaseMinimal)) * 100;
      HMISerial.print("value.val=");
      HMISerial.print(abs(persentase));
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      HMISerial.write(0xff);
    }
  }
}
long convertHexToDecimalSpeed(byte hexData[]) {
  String hexString = "";
  for (int i = 0; i < 4; i++) {
    hexString += String(hexData[i], HEX);
  }
  return strtol(hexString.c_str(), NULL, 16);
}


long convertHexToDecimalTemp(byte hexData[]) {
  String hexString = "";
  for (int i = 0; i < 2; i++) {
    hexString += String(hexData[i], HEX);
  }
  return strtol(hexString.c_str(), NULL, 16);
}

long convertHexToDecimalCurrent(byte hexData[]) {
  String hexString = "";
  for (int i = 4; i < 6; i++) {
    hexString += String(hexData[i], HEX);
  }
  return strtol(hexString.c_str(), NULL, 16);
}

long convertHexToDecimalVoltage(byte hexData[]) {
  String hexString = "";
  for (int i = 4; i < 6; i++) {
    hexString += String(hexData[i], HEX);
  }
  return strtol(hexString.c_str(), NULL, 16);
}
