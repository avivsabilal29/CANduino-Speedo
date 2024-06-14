
#include <SPI.h>
#include <mcp2515.h>
#include <SoftwareSerial.h>
SoftwareSerial HMISerial(6, 7);

struct can_frame canMsg;
MCP2515 mcp2515(10);
int voltageIn;
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
    if (canMsg.can_id == 0x80001B26) {
      for (int i = 4; i < 6; i++) {
        Serial.print(canMsg.data[i], HEX);
        Serial.print(" ");
      }
      // Convert hex data to decimal and print
      long decimalValue = convertHexToDecimal(canMsg.data);
      voltageIn = decimalValue/10;
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
      Serial.print("Percent (%) : ");
      Serial.println(percent);
      HMISerial.print("value.val=");
      HMISerial.print(abs(percent));
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      delay(50);
    }
  }
}

long convertHexToDecimal(byte hexData[]) {
  String hexString = "";
  for (int i = 4; i < 6; i++) {
    hexString += String(hexData[i], HEX);
  }
  return strtol(hexString.c_str(), NULL, 16);
}
