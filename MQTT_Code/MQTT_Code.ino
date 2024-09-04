/**
 * MQTT 접속 예제
 * 
 * 필수 라이브러리 PublishClient
 */

// RUN CHECK LED
#define LED_CHECK 33
#define MQTT_KEEPALIVE 120

#include <WiFi.h>
#include <PubSubClient.h>

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

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
    Serial.begin(115200);
    pinMode(LED_CHECK, OUTPUT);

    wifiConnect();
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
        pubMQTT(pubTopic + "Res", "MQTT 발행 테스트");
    }
}

void loop() {
  
    // 만약 연결이 해제되면, 연결 재시도
    if (!client.connected()) {
        connectToMQTT();
    }
    
    client.loop();
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
    while (!client.connected()) {
        // 고유한 클라이언트 ID 사용
        Serial.printf("Client ID:%s\n", clientId.c_str());
        
        Serial.print("MQTT 서버에 접속중...");
                
        // MQTT 브로커에 연결 시도
        if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
            Serial.println("연결완료");
            
            // 연결 후 토픽을 구독
            client.subscribe(subTopic.c_str());
        } else {
          
            Serial.print("연결 실패! rc=");
            Serial.print(client.state());
            Serial.println(" 5초후, 연결 재시도");
      
            // 연결 실패 시 5초 후 재시도
            delay(5000);
        }
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
}
