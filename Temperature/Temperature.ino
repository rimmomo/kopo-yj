/**
 * One-Wire 통신을 이용한 온도센서 예제 입니다
 * One-Wire의 상세설명 참조 : https://ko.wikipedia.org/wiki/1-Wire
 * 
 * One-Wire 단자대에 온도센서를 아래 순서로 연결 하세요
 * 적 - 노 - 흑
 * 
 * 필수 라이브러리 DallasTemperature
 * 
 */

// DS18B20
#define DALLAS 32

#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(DALLAS);
DallasTemperature sensors(&oneWire);

void setup() {
    Serial.begin(115200);

    // 온도센서 시작
    sensors.begin();
    
    Serial.println("DS18B20 온도 센서 준비완료");
}

void loop() {
    
    get_dallas_sensing();
    delay(10000);
}

void get_dallas_sensing() {
  
    float temp = 0;
    
    // 센서에게 온도 측정 명령 전송
    sensors.requestTemperatures(); // Send the command to get temperatures
    
    // 측정한 온도값 확인
    temp = sensors.getTempCByIndex(0);
    
    Serial.printf("현재 온도 :%.2f°C\n", temp);

}
