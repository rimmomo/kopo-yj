/**
 * 디지털 입력 예제 입니다.
 * IN1 ~ IN4 번까지의 입력 단자에 3.3v ~ 5v의 HIGH신호를 입력해주세요. 
 */

// Digtial Input
#define DIN1 25
#define DIN2 26
#define DIN3 27
#define DIN4 35

void setup() {
    Serial.begin(115200);
    
    pinMode(DIN1, INPUT);
    pinMode(DIN2, INPUT);
    pinMode(DIN3, INPUT);
    pinMode(DIN4, INPUT);

    Serial.println("Digital 입력 준비완료");
    Serial.println("IN1 ~ 5까지 입력 단자에 3.3v~ 5v 의 HIGH 신호가 입력되도록 하세요.");
}

void loop() {

    if (digitalRead(DIN1) == LOW) {
        Serial.println("DIN1 입력신호 감지");
    }
    
    if (digitalRead(DIN2) == LOW) {
        Serial.println("DIN2 입력신호 감지");
    }
    if (digitalRead(DIN3) == LOW) {
        Serial.println("DIN3 입력신호 감지");
    }
    if (digitalRead(DIN4) == LOW) {
        Serial.println("DIN4 입력신호 감지");
    }
    
}
