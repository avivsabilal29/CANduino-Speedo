 #include <SPI.h>
#include <mcp2515.h>
#include <SoftwareSerial.h>
SoftwareSerial HMISerial(6, 7);

struct can_frame canMsg;
MCP2515 mcp2515(10);
int speedRead;

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
      for (int i = 0; i < 4; i++) {
        Serial.print(canMsg.data[i], HEX);
        Serial.print(" ");
      }
      // Convert hex data to decimal and print
      long decimalValue = convertHexToDecimal(canMsg.data);
      speedRead = 32*(decimalValue/10)*0.001885;
      Serial.print("Kecepatan (Km/h) : ");
      Serial.println(abs(speedRead));
      HMISerial.print("speed.val=");
      HMISerial.print(abs(speedRead));
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      HMISerial.write(0xff);
      delay(50);
    }
  }
}

long convertHexToDecimal(byte hexData[]) {
  String hexString = "";
  for (int i = 0; i < 4; i++) {
    hexString += String(hexData[i], HEX);
  }
  return strtol(hexString.c_str(), NULL, 16);
}
