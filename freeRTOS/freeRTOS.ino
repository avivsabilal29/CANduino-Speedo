#include <Arduino_FreeRTOS.h>
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
// Definisi konstanta
const float voltageConversionFactor = 0.001885;
int voltaseMaksimal = 84;
int voltaseMinimal = 63;
void Task_Print1(void *param);
void Task_Print2(void *param);
TaskHandle_t Task_Handle1;
TaskHandle_t Task_Handle2;

void setup() {
  Serial.begin(115200);
  HMISerial.begin(9600);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS);
  mcp2515.setNormalMode();
  xTaskCreate(Task_Print1, "Task1", 100, NULL, 1, &Task_Handle1);
  xTaskCreate(Task_Print2, "Task2", 100, NULL, 2, &Task_Handle2);
}

void loop() {
  //Jangan letakkan code apapun di bawah sini
}

void Task_Print1(void *param)
{
  (void) param;

  while (1) // or for(;;)
  {
    if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
      if (canMsg.can_id == 0x80000926) {
        // Kode kecepatan
        for (int i = 0; i < 4; i++) {
          Serial.print(canMsg.data[i], HEX);
          Serial.print(" ");
        }
        long decimalValueSpeed = convertHexToDecimal(canMsg.data, 0, 4);
        speedRead = 32 * (decimalValueSpeed / 10) * voltageConversionFactor;
        Serial.print("Kecepatan (Km/h) : ");
        Serial.println(abs(speedRead));
        HMISerial.print("speed.val=");
        HMISerial.print(abs(speedRead));
        HMISerial.write(0xff);
        HMISerial.write(0xff);
        HMISerial.write(0xff);
      }
    }
    Serial.println("SpeedValue");
    vTaskDelay(10 / portTICK_PERIOD_MS); // Arduino akan melakukan print ke serial monitor tiap 1 detik sekali
  }
}

void Task_Print2(void *param)
{
  (void) param;

  while (1)
  {
    if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
      if (canMsg.can_id == 0x80001B26) {
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
        float persentase = ((voltageIn - voltaseMinimal) / (voltaseMaksimal - voltaseMinimal)) * 100;
        HMISerial.print("value.val=");
        HMISerial.print(72);
        HMISerial.write(0xff);
        HMISerial.write(0xff);
        HMISerial.write(0xff);
      }
    }
    Serial.println("Task Baca Sensor MPU");
    vTaskDelay(5000 / portTICK_PERIOD_MS); // Arduino akan melakukan print ke serial monitor tiap 1 detik sekali
  }
}

long convertHexToDecimal(byte hexData[], int startIndex, int endIndex) {
  String hexString = "";
  for (int i = startIndex; i < endIndex; i++) {
    hexString += String(hexData[i], HEX);
  }
  return strtol(hexString.c_str(), NULL, 16);
}
