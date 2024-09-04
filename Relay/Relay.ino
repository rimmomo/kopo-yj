/**
 * 시프트레지스터(74HC595D)를 이용한 릴레이 제어 입니다.
 * 8채널 릴레이는 LOW Level릴레이로써 HIGH:OFF, LOW:ON 으로 작동 됩니다.
 * 즉, 시프트레지스터에서 HIGH를 출력하면 릴레이가 OFF가 되고, LOW를 출력하면 ON 됩니다.
 */
 
#define numOfRegisterPins 8
#define SER_PIN 19
#define RCLK_PIN 5
#define SRCLK_PIN 4

boolean registers [numOfRegisterPins];  // 시프트레지스터 변수 [릴레이 갯수]

void setup() {
  Serial.begin(115200);
  
  pinMode(SER_PIN, OUTPUT);
  pinMode(RCLK_PIN, OUTPUT);
  pinMode(SRCLK_PIN, OUTPUT);

  // 시프트레지스터 HIGH.
  setRegistersPinAll(HIGH);
  
  // 릴레이 OFF 작동
  writeRegisters();
  
  Serial.println("릴레이 준비완료");
  Serial.println("시리얼 모니터를 이용하여 1~8까지의 값을 입력 하세요. 토글형식으로 작동됩니다.");
  Serial.println("0:모든 릴레이 OFF 1:모든 릴레이 ON");
}

void loop(){
  readSerialPort();
}

// 시리얼 입력 읽기
void readSerialPort() {
  while (Serial.available()) {
    delay(10);
    if (Serial.available() > 0) {
      int relay_num = Serial.parseInt();
      Serial.print(" <<< ");
      Serial.println(relay_num);

      if (relay_num == 0) {
        // 리셋 초기화 
        setRegistersPinAll(HIGH);
        Serial.println("모든 릴레이를 OFF 합니다.");
      } else if (relay_num == 9) {
        Serial.println("모든 릴레이를 ON 합니다.");
        setRegistersPinAll(LOW);
      } else {
        setRegisterPin( (relay_num - 1), (!registers[(relay_num-1)]) );  
      }
      writeRegisters();
    }
  }
}

void setRegisterPin(int index, int value) {
  registers[index] = value;
}

void setRegistersPinAll(bool val){
  for(int i = numOfRegisterPins-1; i >=  0; i--) {
    registers[i] = val;
  }
}

void writeRegisters(){/* function writeRegisters */ 
  digitalWrite(RCLK_PIN, LOW);
  for(int i = numOfRegisterPins-1; i >=  0; i--) {
    digitalWrite(SRCLK_PIN, LOW);
    int val = registers[i];
    digitalWrite(SER_PIN, val);
    digitalWrite(SRCLK_PIN, HIGH);
  }
  digitalWrite(RCLK_PIN, HIGH);
}
