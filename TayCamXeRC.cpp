#include <WiFi.h>
#include <esp_now.h>

#define potDanhlai 35
#define potSpeedDC 34
#define potMaxDC 33
#define potSteeringTrim 32
#define Congtac 36
uint8_t receiverAddr[] = {0xcc, 0x7b, 0x5c, 0x25, 0xbf, 0x20};

typedef struct struct_message{
  int button; //tắt hoặc bật chế độ tự hành
  int Steering; //0-180 (góc quay servo)
  int Speed;  //-100 - 100 (âm lùi, dương tiến)
  int SpeedLimit;//0-100
  int ServoTrim; // căn chỉnh servo để xe đi thẳng chắc vậy
} struct_message;

struct_message DataToSend;

void setup() {
  pinMode(Congtac, INPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if(esp_now_init() != ESP_OK){
    Serial.println("Khong the thuc hien giao thuc ESP NOW");
    return;
  }

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverAddr, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if(esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Khong the bat cap");
    return;
  }
}

void loop() {
  int value1 = analogRead(potDanhlai);
  int value2 = analogRead(potSpeedDC);
  int value3 = analogRead(potMaxDC);
  int value4 = analogRead(potSteeringTrim);
  DataToSend.button = digitalRead(Congtac);
  DataToSend.Steering = map(value1, 0, 4095, 0, 180);
  DataToSend.Speed = map(value2, 0, 4095, -100, 100);
  DataToSend.SpeedLimit = map(value3, 0, 4095, 0 , 100);
  DataToSend.ServoTrim = map(value4, 0, 4095, -15, 15);
  esp_now_send(receiverAddr, (uint8_t *) &DataToSend, sizeof(DataToSend));
  delay(100);
}
