const int laserPin = 6;

void setup() {
  pinMode(laserPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    Serial.println(msg);
    sendPreamble();
    sendData(msg);
  }
}

void sendPreamble() {
  String preamble = "1010101111111111";
  for (int i = 0; i < preamble.length(); i++) {
    digitalWrite(laserPin, preamble[i] == '1' ? HIGH : LOW);
    delay(10);
  }
}

void sendData(String msg) {
  for (int i = 0; i < msg.length(); i++) {
    char c = msg[i];
    for (int j = 0; j < 8; j++) {
      bool bit = (c >> j) & 0x01;
      digitalWrite(laserPin, bit ? HIGH : LOW);
      delay(10);
    }
  }
  digitalWrite(laserPin, LOW); // 비트 간 구분
  delay(20);  // 각 문자 사이에는 긴 지연
}
