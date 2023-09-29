#include <Wire.h>
#include <LiquidCrystal_I2C.h>  //LCD 를 사용할 수 있게 해주는 라이브러리

LiquidCrystal_I2C lcd(0x27, 20, 4); // 0x27은 LCD의 I2C 주소입니다. 다를 수 있으니 확인이 필요합니다.

const int solarPin = A0; // 태양광 패널 연결 핀
const int threshold = 250; // HIGH와 LOW를 구분하는 아날로그 값(캘리브레이션)값
const int bitDelay = 10; // 비트 간의 딜레이 시간 (ms)

String incomingBits = "";  // 수신된 비트 저장
String receivedMessage = ""; // 수신된 메시지 저장

void setup() {
  pinMode(solarPin, INPUT); //태양광 패널 핀 모드 설정
  Serial.begin(9600);       //시리얼 모니터 설정
  lcd.begin();              //lcd 설정
  lcd.backlight();          //==
  lcd.setCursor(0, 0);      //==
  lcd.print("Receiving Data:");//==
}

void loop() {
  if (waitForPreamble()) {
    receiveData();
  }
}

bool waitForPreamble() {
  String preamble = "1010101111111111";   //암호화 같은거라고 생각하시면 됩니다. 수신측과 송신측이 같은 암호를 가지고 있어야 합니다. 암호화 이진수 뒤에 보내는 데이터가 들어간다고 생각하시면 됩니다.
  int matchedCount = 0;                   //몇개가 맞는지 확인해주는 변수

  while (true) {
    int sensorValue = analogRead(solarPin);
    char bit = (sensorValue > threshold) ? '1' : '0';
    
    if (bit == preamble[matchedCount]) {
      matchedCount++;
    } else {
      matchedCount = 0;  // 일치하지 않으면 카운트 리셋
    }

    if (matchedCount == preamble.length()) {
      return true;  // 전체 preamble이 일치하면 true 반환
    }
    delay(bitDelay);
  }
}

void receiveData() {
  incomingBits = "";  // 초기화
  while (true) {
    char receivedChar = 0;
    for (int i = 0; i < 8; i++) {
      int sensorValue = analogRead(solarPin);
      char bit = (sensorValue > threshold) ? '1' : '0';
      incomingBits += bit;

      if (bit == '1') {
        receivedChar |= (1 << i);
      }
      delay(bitDelay);
    }

    receivedMessage += receivedChar;
    if (receivedMessage.length() >= 10) {  // 메시지 길이가 10자를 넘어서면 출력
      Serial.println(receivedMessage);
      lcd.setCursor(0, 1);  // 다음 줄로 이동
      lcd.print(receivedMessage);
      receivedMessage = "";
    }
  }
}
