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

// RUN CHECK LED
#define LED_CHECK 33
#define MQTT_KEEPALIVE 120

#include <WiFi.h>
#include <PubSubClient.h>

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

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(115200);
  
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
  
  Serial.println("릴레이 MQTT 준비완료");
  Serial.printf("사용방법 : %srelay/릴레이번호(1~8) 토픽으로 1:ON 0:OFF 값으로 발행 하세요\n", pubTopic);
}

void loop(){

    // 만약 연결이 해제되면, 연결 재시도
    if (!client.connected()) {
        connectToMQTT();
    }
    client.loop();
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
    } else {
        Serial.println("[연결 성공!]");
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
