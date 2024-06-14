
#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(10);

void setup() {
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS);
  mcp2515.setNormalMode();
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID          DLC   DATA");
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    if (canMsg.can_id == 0x80001B26) {
      Serial.print("ID: 80001B26 (Status 5) ");
      Serial.print("DLC: ");
      Serial.print(canMsg.can_dlc, HEX); // print DLC
      Serial.print(" DATA: ");
      for (int i = 0; i < canMsg.can_dlc; i++) {  // print the data
        Serial.print(canMsg.data[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  }
}
