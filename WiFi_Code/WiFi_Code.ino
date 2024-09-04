/**
 * WiFi 접속 예제
 * 다양한 예제는 파일 > 예제 > ESP32 Dev Module > Wifi를 참조 하세요
 */

// RUN CHECK LED
#define LED_CHECK 33

#include <WiFi.h>

const char* ssid      = "";      // 연결가능한 WiFi 아이디로 교체
const char* password  = "";   // WiFi의 접속 비밀번호

WiFiClient wifiClient;

void setup() {
    Serial.begin(115200);
    pinMode(LED_CHECK, OUTPUT);

    wifiConnect();
    wifiInfoPrint();
}

void loop() {
    
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
