#include <Wire.h>                         // I2C 통신을 위한 라이브러리
#include <LiquidCrystal_I2C.h>             // LCD 1602 I2C용 라이브러리
#include <Keypad.h>                        // 키패드 라이브러리

// 키패드 설정
const byte ROW_NUM    = 4;               // 키패드의 행 수
const byte COLUMN_NUM = 4;               // 키패드의 열 수

char keys[ROW_NUM][COLUMN_NUM] = {       // 키패드 버튼 설정
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6};    // 키패드의 행 핀 연결
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; // 키패드의 열 핀 연결

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);  // 키패드 객체 생성

LiquidCrystal_I2C lcd(0x27, 16, 2);      // LCD 설정

// 비밀번호 관련 변수
String correctPassword = "";             // 설정된 비밀번호
String inputPassword = "";               // 입력된 비밀번호
bool settingPassword = true;             // 비밀번호 설정 모드 (true: 설정, false: 일반 모드)
int solenoidPin = 12;                    // 솔레노이드 제어 핀

void setup() {
  lcd.init();                            // LCD 초기화
  lcd.backlight();                       // 백라이트 켬
  
  pinMode(solenoidPin, OUTPUT);          // 솔레노이드 핀을 출력으로 설정
  digitalWrite(solenoidPin, HIGH);       // 솔레노이드 잠금 상태 (HIGH로 잠금)
  lcd.setCursor(0, 0);
  lcd.print("Set Password:");            // 비밀번호 설정 안내
}

void loop() {
  char key = keypad.getKey();            // 키패드에서 눌린 키 값 읽기

  if (key) {                             // 키가 눌렸으면
    if (key == '#') {                    // #이 눌리면 동작 처리
      if (settingPassword) {             // 비밀번호 설정 모드일 때
        correctPassword = inputPassword; // 설정된 비밀번호 저장
        settingPassword = false;         // 설정 모드 종료
        inputPassword = "";              // 입력 초기화
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Password Set!");
        delay(1000);                     // 설정 후 잠시 대기
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Locked");
        digitalWrite(solenoidPin, HIGH);  // 잠금 상태로 설정
      } else {                           // 비밀번호 입력 모드일 때
        if (inputPassword == correctPassword) { // 비밀번호가 맞으면
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Unlocking...");
          digitalWrite(solenoidPin, LOW);  // 솔레노이드 잠금 해제 (LOW로 열기)
          delay(5000);                     // 5초 동안 열기
          digitalWrite(solenoidPin, HIGH); // 잠금 (HIGH로 잠그기)
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Access Granted");
          delay(1000);                    // "Access Granted" 후 잠시 대기
          
          // 비밀번호 초기화 및 다시 설정 모드로 돌아가기
          inputPassword = "";
          settingPassword = true;           // 비밀번호 설정 모드로 전환
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Set Password:");      // 설정 모드로 안내
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Incorrect PW");
          delay(2000);                     // 2초 대기 후 초기화
          inputPassword = "";             // 비밀번호 초기화
        }
      }
    } else if (key == '*') {             // '*'이 눌리면 입력 취소
      inputPassword = "";                // 입력된 값 초기화
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Input Cleared");
      delay(1000);                        // 1초 대기 후 리셋
      lcd.clear();                        // 다시 LCD 화면 지우기
    } else {
      inputPassword += key;              // 입력된 키를 문자열에 추가
      lcd.clear();                       // LCD 화면 지우기
      lcd.setCursor(0, 0);               // 첫 번째 줄로 커서 이동
      if (settingPassword) {
        lcd.print("Set PW: ");            // "Set PW: " 출력
      } else {
        lcd.print("Enter PW: ");          // "Enter PW: " 출력
      }
      lcd.print(inputPassword);           // 입력된 비밀번호 출력
    }
  }
}
