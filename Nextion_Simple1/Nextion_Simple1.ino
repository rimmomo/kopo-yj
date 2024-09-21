/**
 * 시프트레지스터(74HC595D)를 이용한 릴레이 제어 입니다.
 * 8채널 릴레이는 LOW Level릴레이로써 HIGH:OFF, LOW:ON 으로 작동 됩니다.
 * 즉, 시프트레지스터에서 HIGH를 출력하면 릴레이가 OFF가 되고, LOW를 출력하면 ON 됩니다.
 * 
 * 필수 라이브러리 PublishClient
 * 
 */
 
#define numOfRegisterPins 8
#define SER_PIN 19
#define RCLK_PIN 5
#define SRCLK_PIN 4
#define DALLAS 32

// RUN CHECK LED
#define LED_CHECK 33
#define MQTT_KEEPALIVE 120

// Display용 디버깅 옵션
#define DEBUG_SERIAL_ENABLE

#include <WiFi.h>
#include <PubSubClient.h>

#include "HardwareSerial.h"
#include "Nextion.h"

#include <OneWire.h>
#include <DallasTemperature.h>

boolean registers [numOfRegisterPins];  // 시프트레지스터 변수 [릴레이 갯수]

// WiFi 설정
const char* ssid      = "";      // 연결가능한 WiFi 아이디로 교체
const char* password  = "";   // WiFi의 접속 비밀번호

// MQTT 설정
const char* mqtt_server   = "";    // MQTT 브로커 주소
const int mqtt_port       = 1883;                 // MQTT 포트 (기본: 1883)
const char* mqtt_user     = "";          // MQTT 사용자 이름 (필요 시)
const char* mqtt_password = "";          // MQTT 패스워드 (필요 시)

String subTopic = String(mqtt_user) + "/#";       // MQTT 구독용 토픽
String pubTopic = String(mqtt_user) + "/";        // MQTT 발행용 토픽

String clientId = "ESP32Client-" + String(random(0xfffffff), HEX);

unsigned long lastTime = 0;                       // 마지막으로 측정한 시간

byte dgInputPin[4] = {25, 26, 27, 35};              // 디지털 입력 핀맵
int lastButtonState[4] = {HIGH, HIGH, HIGH, HIGH};  // 입력 상태 (풀업 상태이므로, HIGH입니다.)
int buttonState[4] = {HIGH, HIGH, HIGH, HIGH};      // 현재 버튼 상태
unsigned long lastDebounceTime[4] = {0, 0, 0, 0};   // 마지막으로 디바운싱이 발생한 시간
unsigned long debounceDelay = 50;                   // 디바운싱 지연 시간 (밀리초)
unsigned long lastTempCheckTime = 0;                // 온도체크 마지막 시간

WiFiClient wifiClient;
PubSubClient client(wifiClient);
HardwareSerial HMISerial(2);

OneWire oneWire(DALLAS);
DallasTemperature sensors(&oneWire);

// Page0의 구성요소
NexText t0 = NexText(0, 1, "t0");
NexText t1 = NexText(0, 15, "t1");
NexPicture p0 = NexPicture(0, 10, "p0");     // WiFi 연결 아이콘
NexPicture inp1 = NexPicture(0, 11, "p1");  // 디지털입력1
NexPicture inp2 = NexPicture(0, 12, "p2");  // 디지털입력2
NexPicture inp3 = NexPicture(0, 13, "p3");  // 디지털입력3
NexPicture inp4 = NexPicture(0, 14, "p4");  // 디지털입력4

NexDSButton bt0 = NexDSButton(0, 2, "bt0");      // 버튼1
NexDSButton bt1 = NexDSButton(0, 3, "bt1");      // 버튼2
NexDSButton bt2 = NexDSButton(0, 4, "bt2");      // 버튼3
NexDSButton bt3 = NexDSButton(0, 5, "bt3");      // 버튼4
NexDSButton bt4 = NexDSButton(0, 6, "bt4");      // 버튼5
NexDSButton bt5 = NexDSButton(0, 7, "bt5");      // 버튼6
NexDSButton bt6 = NexDSButton(0, 8, "bt6");      // 버튼7
NexDSButton bt7 = NexDSButton(0, 9, "bt7");      // 버튼8

// 모든 페이지 구성요소 초기화

NexTouch *nex_listen_list[] = {
  &t0, &t1, &p0, &inp1, &inp2, &inp3, &inp4,
  &bt0, &bt1, &bt2, &bt3, &bt4, &bt5, &bt6, &bt7, NULL
};

void t0PopCallback(void *ptr);
void bt0PushCallback(void *ptr);
void bt1PushCallback(void *ptr);
void bt2PushCallback(void *ptr);
void bt3PushCallback(void *ptr);
void bt4PushCallback(void *ptr);
void bt5PushCallback(void *ptr);
void bt6PushCallback(void *ptr);
void bt7PushCallback(void *ptr);



void setup() {
  Serial.begin(115200);
  HMISerial.begin(9600,SERIAL_8N1, 14, 15);

  nexInit();
  
  pinMode(SER_PIN, OUTPUT);
  pinMode(RCLK_PIN, OUTPUT);
  pinMode(SRCLK_PIN, OUTPUT);
  pinMode(LED_CHECK, OUTPUT);

  // 시프트레지스터 HIGH.
  setRegistersPinAll(HIGH);
  
  // 릴레이 OFF 작동
  writeRegisters();

  // 와이파이 연결
  wifiConnect();
  // 연결된 와이파이 정보 출력
  wifiInfoPrint();

  if (WiFi.status() == WL_CONNECTED) {
        
        // MQTT 서버 연결 설정
        client.setServer(mqtt_server, mqtt_port);
        // MQTT 서버 연결 완료후 콜백 함수 설정
        client.setCallback(callback);
        
        connectToMQTT();
        Serial.printf("%s 구독 설정 완료\n", subTopic);

        // 메시지 발행
        // pubTopic을 만들어서 발행
        pubMQTT(pubTopic + "Res", "릴레이 MQTT 준비완료");

  }

  t0.attachPush(t0PopCallback);             // 온도 텍스트 박스
  bt0.attachPush(bt0PushCallback, &bt0);    // 릴레이 버튼1
  bt1.attachPush(bt1PushCallback, &bt1);    // 릴레이 버튼2
  bt2.attachPush(bt2PushCallback, &bt2);    // 릴레이 버튼3
  bt3.attachPush(bt3PushCallback, &bt3);    // 릴레이 버튼4
  bt4.attachPush(bt4PushCallback, &bt4);    // 릴레이 버튼5
  bt5.attachPush(bt5PushCallback, &bt5);    // 릴레이 버튼6
  bt6.attachPush(bt6PushCallback, &bt6);    // 릴레이 버튼7
  bt7.attachPush(bt7PushCallback, &bt7);    // 릴레이 버튼8

  // Display의 버튼을 모두 0으로 초기화
  bt0.setValue(0);
  bt1.setValue(0);
  bt2.setValue(0);
  bt3.setValue(0);
  bt4.setValue(0);
  bt5.setValue(0);
  bt6.setValue(0);
  bt7.setValue(0);

  for (byte i = 0; i < sizeof(dgInputPin) / sizeof(dgInputPin[0]); i++) {
      pinMode(dgInputPin[i], INPUT);
  }

  // 온도센서 시작
  sensors.begin();

  Serial.println("Nextion Display & 릴레이 MQTT 준비완료");
  Serial.printf("사용방법 : %srelay/릴레이번호(1~8) 토픽으로 1:ON 0:OFF 값으로 발행 하세요\n", pubTopic);
}

void loop(){

    check_digital_input();
    
    nexLoop(nex_listen_list);

    get_dallas_sensing();

    // 만약 연결이 해제되면, 연결 재시도
    if (!client.connected()) {
        connectToMQTT();
    }
    client.loop();
}

void get_dallas_sensing() {

    if ((millis() - lastTempCheckTime) > 5000) {
        float temp = 0;
    
        // 센서에게 온도 측정 명령 전송
        sensors.requestTemperatures(); // Send the command to get temperatures
        
        // 측정한 온도값 확인
        temp = sensors.getTempCByIndex(0);
    
        t0.setText( String(temp).c_str() );
        
        Serial.printf("현재 온도 :%.2f°C\n", temp);

        lastTempCheckTime = millis();
    }
}

void check_digital_input() {
  
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
                            inp1.setPic(5);
                            break;
                        case 1:
                            Serial.println("DIN2 입력 OFF 감지");
                            inp2.setPic(5);
                            break;
                        case 2:
                            Serial.println("DIN3 입력 OFF 감지");
                            inp3.setPic(5);
                            break;
                        case 3:
                            Serial.println("DIN4 입력 OFF 감지");
                            inp4.setPic(5);
                            break;
                    }
                } else if (buttonState[i] == LOW) {
                    switch (i) {
                        case 0:
                            Serial.println("DIN1 입력 ON 감지");
                            inp1.setPic(6);
                            break;
                        case 1:
                            Serial.println("DIN2 입력 ON 감지");
                            inp2.setPic(6);
                            break;
                        case 2:
                            Serial.println("DIN3 입력 ON 감지");
                            inp3.setPic(6);
                            break;
                        case 3:
                            Serial.println("DIN4 입력 ON 감지");
                            inp4.setPic(6);
                            break;
                    }
                }
            }
        }

        // 다음 루프를 위해 현재 상태를 기록합니다.
        lastButtonState[i] = reading;
    }
}

// MQTT 메시지 콜백 함수
void callback(char* topic, byte* message, unsigned int length) {
  
    Serial.print("메세지 도착: ");
    Serial.print(topic);
    Serial.print(" 내용: ");
    String messageTemp;
    
    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    // 토픽을 문자열로 변환 (토픽 예시 : 아이디/relay/1)
    String topicStr = String(topic);
    
    // 토픽의 2번째 부분과 3번째 부분 가져오기
    String secondPart = getTopicPart(topicStr, 2);
    String thirdPart = getTopicPart(topicStr, 3);

    if (secondPart == "relay") {
        // 만약 릴레이 제어이면 실행
        int activeValue = messageTemp.toInt();

        // 시프트레지스터에 릴레이값 등록
        setRegisterPin( (thirdPart.toInt() - 1), activeValue == 1 ? LOW : HIGH);
        writeRegisters();

        switch( thirdPart.toInt() - 1 ) {
          case 0:
            bt0.setValue(activeValue);
            break;
          case 1:
            bt1.setValue(activeValue);
            break;
          case 2:
            bt2.setValue(activeValue);
            break;
          case 3:
            bt3.setValue(activeValue);
            break;
          case 4:
            bt4.setValue(activeValue);
            break;
          case 5:
            bt5.setValue(activeValue);
            break;
          case 6:
            bt6.setValue(activeValue);
            break;
          case 7:
            bt7.setValue(activeValue);
            break;
        }
    }
}

// 토픽에서 원하는 부분을 추출하여 반환하는 함수
String getTopicPart(const String& topic, int partNumber) {
    int startIndex = 0;
    int endIndex = topic.indexOf('/');
    int currentPart = 1;  // 부분 인덱스는 1부터 시작
    
    while (endIndex != -1) {
        if (currentPart == partNumber) {
            // 원하는 부분을 추출하여 반환
            return topic.substring(startIndex, endIndex);
        }
        startIndex = endIndex + 1;
        endIndex = topic.indexOf('/', startIndex);
        currentPart++;
    }
    
    // 마지막 부분 확인
    if (currentPart == partNumber) {
        return topic.substring(startIndex);
    }
    
    // 원하는 부분이 없을 경우 빈 문자열 반환
    return "";
}

// 메시지 게시
void pubMQTT(String sendTopic, String msg) {
    client.publish(sendTopic.c_str(), msg.c_str());
}

/**
 * WiFi 연결
 */
void wifiConnect() {

    Serial.printf("%s로 WiFi 연결 시도중", ssid);
    
    WiFi.begin(ssid, password);
    // 자동 연결 설정
    WiFi.setAutoConnect(true);            // 자동 접속 설정
    // 자동 재 연결 설정
    WiFi.setAutoReconnect(true);          // 자동 재접속 설정
    // 접속 완료결과 수신 대기
    WiFi.waitForConnectResult();
    
    for (int i = 0; i < 10; i++) {
        if ( WiFi.status() != WL_CONNECTED ) {
          delay (100);
          Serial.print(".");
        }
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[연결 실패!]");

        // 와이파이 연결 실패 디스플레이의 아이콘 변경
        icon_wifi(false);

        t1.setText( "WiFi Disconnected!" );
        
    } else {
        Serial.println("[연결 성공!]");

        // 와이파이 연결 성공 디스플레이의 아이콘 변경
        icon_wifi(true);
    }
    
}

/**
 * 연결된 WiFi의 IP주소 및 Mac Address 출력
 */
void wifiInfoPrint() {
  
    if (WiFi.status () == WL_CONNECTED) {
        Serial.print("IP : ");
        Serial.print(WiFi.localIP());
        Serial.print(" MAC : ");
        Serial.println(WiFi.macAddress());

        String myIp = WiFi.localIP().toString();

        t1.setText( myIp.c_str() );

        digitalWrite(LED_CHECK, HIGH);
        delay(300);
        digitalWrite(LED_CHECK, LOW);
        delay(300);
        digitalWrite(LED_CHECK, HIGH);
        delay(300);
        digitalWrite(LED_CHECK, LOW);
        delay(300);
        digitalWrite(LED_CHECK, HIGH);
        delay(300);
        digitalWrite(LED_CHECK, LOW);        
    }
    
}

// MQTT 연결 함수
void connectToMQTT() {

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Restart");
        delay(2000);
        ESP.restart();
    }

    if (!client.connected()) {
        Serial.print("MQTT 서버에 재 접속중...");

        for (int i = 0; i < 20; i++) {
            client.connect(clientId.c_str(), mqtt_user, mqtt_password);
            Serial.print(".");
            delay(1000);

            if (client.connected()) {
                Serial.println(" 연결 완료");
                // 연결 후 토픽을 구독
                client.subscribe(subTopic.c_str());
                break;
            }
        }
    }

    if (!client.connected()) {
        Serial.println(" 연결 실패 [재부팅]");
        ESP.restart();
    }
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
