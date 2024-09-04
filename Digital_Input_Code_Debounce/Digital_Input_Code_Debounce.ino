/**
 * 디지털 입력 예제입니다.
 * IN1 ~ IN4 번까지의 입력 단자에 3.3v ~ 5v의 HIGH 신호를 입력해주세요.
 * 
 * 채터링(Chattering) 방지를 위한 Debouncing 코드가 추가 되었습니다.
 */

byte dgInputPin[4] = {25, 26, 27, 35};              // 디지털 입력 핀맵

int lastButtonState[4] = {HIGH, HIGH, HIGH, HIGH};  // 입력 상태 (풀업 상태이므로, HIGH입니다.)
int buttonState[4] = {HIGH, HIGH, HIGH, HIGH};      // 현재 버튼 상태
unsigned long lastDebounceTime[4] = {0, 0, 0, 0};   // 마지막으로 디바운싱이 발생한 시간
unsigned long debounceDelay = 50;                   // 디바운싱 지연 시간 (밀리초)

void setup() {
    Serial.begin(115200);

    for (byte i = 0; i < sizeof(dgInputPin) / sizeof(dgInputPin[0]); i++) {
        pinMode(dgInputPin[i], INPUT);
    }
    
    Serial.println("Digital 입력 준비 완료");
    Serial.println("IN1 ~ IN4까지 입력 단자에 3.3v~ 5v 의 HIGH 신호가 입력되도록 하세요.");
}

void loop() {

    // 4개의 입력핀의 상태를 조회합니다.
    for (byte i = 0; i < 4; i++) {

        // 현재 핀의 상태를 읽습니다.
        int reading = digitalRead(dgInputPin[i]);

        // 이전 입력 상태와 현재 상태가 다르면 (즉, 입력이 들어온 상태 또는 끊어진 상태)
        if (reading != lastButtonState[i]) {
            // 마지막으로 변경된 시간 기록
            lastDebounceTime[i] = millis();
        }

        // 디바운싱 지연 시간만큼 지난 후에 다시 체크
        if ((millis() - lastDebounceTime[i]) > debounceDelay) {
          
            // 버튼의 상태가 변경되었는지 확인
            if (reading != buttonState[i]) {
                buttonState[i] = reading;
  
                // 버튼 상태가 LOW에서 HIGH로 변경된 경우 (신호 입력)
                if (buttonState[i] == HIGH) {
                    switch (i) {
                        case 0:
                            Serial.println("DIN1 입력 OFF 감지");
                        break;
                        case 1:
                            Serial.println("DIN2 입력 OFF 감지");
                        break;
                        case 2:
                            Serial.println("DIN3 입력 OFF 감지");
                        break;
                        case 3:
                            Serial.println("DIN4 입력 OFF 감지");
                        break;
                    }
                } else if (buttonState[i] == LOW) {
                    switch (i) {
                        case 0:
                            Serial.println("DIN1 입력 ON 감지");
                        break;
                        case 1:
                            Serial.println("DIN2 입력 ON 감지");
                        break;
                        case 2:
                            Serial.println("DIN3 입력 ON 감지");
                        break;
                        case 3:
                            Serial.println("DIN4 입력 ON 감지");
                        break;
                    }
                }
            }
        }

        // 다음 루프를 위해 현재 상태를 기록합니다.
        lastButtonState[i] = reading;
    }

}
