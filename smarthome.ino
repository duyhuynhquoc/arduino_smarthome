//LIBRARY
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <RFID.h>

//Motor
#define IN1 28
#define IN2 26
#define ENA 8
#define IN3 24
#define IN4 22
#define ENB 9

// RFID
#define RST 5
#define MISO 50
#define MOSI 51
#define SCK 52
#define SDA 53
RFID myRFID(SDA, RST);
int readingCard[5]; //current card code
int validCard[5][5] = { //valid card code
                        //{48, 236, 247, 115, 88},
                        //{192, 33, 252, 115, 110},
                        {0, 206, 245, 115, 72},
                        {32, 13, 247, 115, 169}
                      };
bool doorState = 0; //door is closed

// SERVO
#define SERVO_PIN 2
Servo myServo;

// LCD
#define SDA_LCD 20
#define SCL_LCD 21
LiquidCrystal_I2C lcd(0x27, 16, 2);

// RAIN SENSOR
#define Rain 3
bool ngoai = 1;

// LED
#define LED1_PIN 32
#define LED2_PIN 34
bool LED1State = 0;
bool LED2State = 0;

char inChar;

// BUZZER
#define BUZZER_PIN 36

void LCDInit() {
  lcd.init();
  lcd.backlight();
}

void LEDInit() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED1_PIN, 0);
  digitalWrite(LED2_PIN, 0);
}

void BuzzerNoti(){
  tone(BUZZER_PIN,698,4);
  delay(50);
  noTone(BUZZER_PIN);
}

void LEDControl()
{
  if (inChar == '1')
  {
    LED1State = !LED1State;
    digitalWrite(LED1_PIN, LED1State);
    BuzzerNoti();
    inChar = -1;
  }
  else if (inChar == '2')
  {
    LED2State = !LED2State;
    digitalWrite(LED2_PIN, LED2State);
    BuzzerNoti();
    inChar = -1;
  }
}

void ServoInit() {
  myServo.attach(SERVO_PIN);
  myServo.write(90);
}

void DoorControl() {
  if (myRFID.isCard()) {
    if (myRFID.readCardSerial()) {
      for (int i = 0; i < 5; i++) readingCard[i] = myRFID.serNum[i];
  
      for (int i = 0; i < 5; i++){
        Serial.print(readingCard[i]);
        Serial.print(' ');
      }
      Serial.println();
      
      bool flag;
      for (int i = 0; i < 5; i++) {
        flag = true;
        for (int j = 0; j < 5; j++) {
          if (readingCard[j] != validCard[i][j]) {
            flag = false;
            break;
          }
        }
        if (flag) {
          break;
        }
      }
      
      if (flag) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" <3 Welcome <3  ");
        lcd.setCursor(0,1);
        if (readingCard[0]==0) lcd.print("    Honeyyyy    ");
        else lcd.print("    Babeeeee    ");
        
        if (doorState) myServo.write(180); else myServo.write(90);      

        doorState = !doorState;
      } else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Invalid ID Card");
      }
      BuzzerNoti();
      delay(1000);
    }
    
    myRFID.halt();
  }
}

void MotorInit(){
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(ENB,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENB,OUTPUT);
}

const int spe=100;

void M1Control(bool dir){
  analogWrite(ENA,240);
  if (dir){
    digitalWrite(IN1,0);
    digitalWrite(IN2,1);
    delay(spe);
    digitalWrite(IN1,0);
    digitalWrite(IN2,0);
  } else {
    digitalWrite(IN1,1);
    digitalWrite(IN2,0);
    delay(spe);
    digitalWrite(IN1,0);
    digitalWrite(IN2,0);
  }
}

void RainSSInit() {
  pinMode(Rain,INPUT);
}

void rainControl() {
  int value = digitalRead(Rain);
  Serial.print(value); Serial.println(ngoai);
  if (value == HIGH && !ngoai)
  {
    // Dang khong mua
    ngoai = 1;
    M1Control(1);
  }
  if (value == LOW && ngoai)
  {
    // Dang mua
    ngoai = 0;
    M1Control(0);
  }
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  LCDInit();
  ServoInit();
  LEDInit();
  myRFID.init();
  RainSSInit();
}

void loop() {
  LEDControl();
  DoorControl();
  rainControl();
}

void serialEvent(){
  while (Serial.available()) inChar = (char)Serial.read();
}

