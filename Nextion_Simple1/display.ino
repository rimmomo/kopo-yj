void t0PopCallback(void *ptr)
{
    dbSerialPrintln("t0PopCallback");
    t0.setText("50");
}

// 버튼1번 == 릴레이 1번 제어
void bt0PushCallback(void *ptr) {
    uint32_t dual_state;
    NexDSButton *btn = (NexDSButton *)ptr;
    bt0.getValue(&dual_state);
    switch (dual_state) {
      case 0:
        Serial.println("1번 릴레이 OFF");
        setRegisterPin(0, HIGH);
        writeRegisters();
      break;
      case 1:
        Serial.println("1번 릴레이 ON");
        setRegisterPin(0, LOW);
        writeRegisters();
      break;
    }
}

// 버튼2번 == 릴레이 2번 제어
void bt1PushCallback(void *ptr) {
    uint32_t dual_state;
    NexDSButton *btn = (NexDSButton *)ptr;
    bt1.getValue(&dual_state);
    switch (dual_state) {
      case 0:
        Serial.println("2번 릴레이 OFF");
        setRegisterPin(1, HIGH);
        writeRegisters();
      break;
      case 1:
        Serial.println("2번 릴레이 ON");
        setRegisterPin(1, LOW);
        writeRegisters();
      break;
    }
}

// 버튼3번 == 릴레이 3번 제어
void bt2PushCallback(void *ptr) {
    uint32_t dual_state;
    NexDSButton *btn = (NexDSButton *)ptr;
    bt2.getValue(&dual_state);
    switch (dual_state) {
      case 0:
        Serial.println("3번 릴레이 OFF");
        setRegisterPin(2, HIGH);
        writeRegisters();
      break;
      case 1:
        Serial.println("3번 릴레이 ON");
        setRegisterPin(2, LOW);
        writeRegisters();
      break;
    }
}

// 버튼4번 == 릴레이 4번 제어
void bt3PushCallback(void *ptr) {
    uint32_t dual_state;
    NexDSButton *btn = (NexDSButton *)ptr;
    bt3.getValue(&dual_state);
    switch (dual_state) {
      case 0:
        Serial.println("4번 릴레이 OFF");
        setRegisterPin(3, HIGH);
        writeRegisters();
      break;
      case 1:
        Serial.println("4번 릴레이 ON");
        setRegisterPin(3, LOW);
        writeRegisters();
      break;
    }
}

// 버튼5번 == 릴레이 5번 제어
void bt4PushCallback(void *ptr) {
    uint32_t dual_state;
    NexDSButton *btn = (NexDSButton *)ptr;
    bt4.getValue(&dual_state);
    switch (dual_state) {
      case 0:
        Serial.println("5번 릴레이 OFF");
        setRegisterPin(4, HIGH);
        writeRegisters();
      break;
      case 1:
        Serial.println("5번 릴레이 ON");
        setRegisterPin(4, LOW);
        writeRegisters();
      break;
    }
}

// 버튼6번 == 릴레이 6번 제어
void bt5PushCallback(void *ptr) {
    uint32_t dual_state;
    NexDSButton *btn = (NexDSButton *)ptr;
    bt5.getValue(&dual_state);
    switch (dual_state) {
      case 0:
        Serial.println("6번 릴레이 OFF");
        setRegisterPin(5, HIGH);
        writeRegisters();
      break;
      case 1:
        Serial.println("6번 릴레이 ON");
        setRegisterPin(5, LOW);
        writeRegisters();
      break;
    }
}

// 버튼7번 == 릴레이 7번 제어
void bt6PushCallback(void *ptr) {
    uint32_t dual_state;
    NexDSButton *btn = (NexDSButton *)ptr;
    bt6.getValue(&dual_state);
    switch (dual_state) {
      case 0:
        Serial.println("7번 릴레이 OFF");
        setRegisterPin(6, HIGH);
        writeRegisters();
      break;
      case 1:
        Serial.println("7번 릴레이 ON");
        setRegisterPin(6, LOW);
        writeRegisters();
      break;
    }
}

// 버튼8번 == 릴레이 8번 제어
void bt7PushCallback(void *ptr) {
    uint32_t dual_state;
    NexDSButton *btn = (NexDSButton *)ptr;
    bt7.getValue(&dual_state);
    switch (dual_state) {
      case 0:
        Serial.println("8번 릴레이 OFF");
        setRegisterPin(7, HIGH);
        writeRegisters();
      break;
      case 1:
        Serial.println("8번 릴레이 ON");
        setRegisterPin(7, LOW);
        writeRegisters();
      break;
    }
}

void icon_wifi(bool actVal) {
    if (actVal) {
        p0.setPic(4);
    } else {
        p0.setPic(3);
    }
}
