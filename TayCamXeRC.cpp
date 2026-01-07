#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //khai báo màn hình

#define potDanhlai 35 //để tạm
#define potSpeedDC 34 //để tạm
#define potMaxDC 33 //để tạm
#define potSteeringTrim 32 //để tạm
#define Congtac 36 //để tạm
uint8_t receiverAddr[] = {0xcc, 0x7b, 0x5c, 0x25, 0xbf, 0x20};

typedef struct struct_message{
  int button; //tắt hoặc bật chế độ tự hành
  int Steering; //0-180 (góc quay servo)
  int Speed;  //-100 - 100 (âm lùi, dương tiến)
  int SpeedLimit;//0-100
  int ServoTrim; // căn chỉnh servo để xe đi thẳng chắc vậy -15-15 
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

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddr, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if(esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Khong the bat cap");
    return;
  }

  Wire.begin(21, 22);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Khong tim thay OLED");
    while (true);
  }
  oled.setTextColor(SSD1306_WHITE); //pixel hiện sẽ có màu trắng
  oled.clearDisplay(); // xóa hết màn hình
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

  oled.clearDisplay();
  oled.setTextSize(1); //chỉnh size chữ gốc 5x7 pixels

  if(DataToSend.button == HIGH){
    oled.setCursor(4,4);
    oled.print("TU HANH");
  }
  else{
    oled.setCursor(4,3);
    oled.print("SPEED : ");
    oled.print(DataToSend.Speed);

    oled.setCursor(4,12);
    oled.print("SERVO ANGLE : ");
    oled.print(DataToSend.Steering);

    oled.setCursor(4,21);
    oled.print("MAX SPEED : ");
    oled.print(DataToSend.SpeedLimit);

    oled.setCursor(4,30);
    oled.print("SERVO TRIM : ");
    oled.print(DataToSend.ServoTrim);

  }

  oled.display(); // bắt buộc để oled hiện được chữ
}
