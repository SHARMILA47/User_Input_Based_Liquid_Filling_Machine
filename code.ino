#include <Keypad.h>
const int ROW_NUM = 4;
const int COLUMN_NUM = 4;
#include <LiquidCrystal.h>
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
 
char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
 
byte pin_rows[ROW_NUM] = { 8, 9, 10, 11 };
byte pin_column[COLUMN_NUM] = { 4, 5, 6, 7 };
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
 
int sensorInterrupt = 0;
int sensorPin = 2;
int solenoidValve = 5;
unsigned int SetPoint = 400;
String code = "";
 
float calibrationFactor = 90;
 
volatile byte pulseCount = 0;
 
float flowRate = 0.0;
unsigned int flowMilliLitres = 0;
unsigned long totalMilliLitres = 0, volume = 0;
 
unsigned long oldTime;
const int relais_moteur = 3;
 
void setup() {
  totalMilliLitres = 0;
  pinMode(relais_moteur, OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Set Volume:");
  Serial.begin(9600);
  pinMode(solenoidValve, OUTPUT);
  digitalWrite(solenoidValve, HIGH);
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}
 
void loop() {
  char key = keypad.getKey();
 
  if (key) {
    code += key;
    lcd.setCursor(0, 1);
    lcd.print(code);
    delay(100);
  }
 
  if (key == 'D') {
    if (code.toInt() <= 1500) {
      volume = code.toInt();
    } else {
      lcd.clear();
 
      lcd.setCursor(0, 0);
      lcd.print("Set Volume:");
    }
    code = "";
  }
 
  if (totalMilliLitres < volume) {
    digitalWrite(relais_moteur, HIGH);
 
    if ((millis() - oldTime) > 1000) {
      detachInterrupt(sensorInterrupt);
 
      flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
      oldTime = millis();
      flowMilliLitres = (flowRate / 60) * 1000;
      totalMilliLitres += flowMilliLitres;
 
      unsigned int frac;
      Serial.print("Flow rate :-");
      Serial.print(flowMilliLitres, DEC);
      Serial.print("mL/Second");
      Serial.print("\t");
      lcd.clear();
 
      lcd.setCursor(0, 0);
      lcd.print("Speed :");
      lcd.print(flowMilliLitres);
      lcd.print(" ml/s");
      Serial.print("Output Liquid Quantity: ");
      Serial.print(totalMilliLitres, DEC);
 
      Serial.println("mL");
      Serial.print("\t");
      lcd.setCursor(0, 1);
      lcd.print("Filled:");
      lcd.print(totalMilliLitres);
      lcd.print(" ml");
      if (totalMilliLitres > 40) {
        SetSolinoidValve();
      }
      pulseCount = 0;
      attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
    }
  } else {
    digitalWrite(relais_moteur, LOW);
    volume = 0;
  }
}
 
void pulseCounter() {
 
  pulseCount++;
}
 
void SetSolinoidValve() {
  digitalWrite(solenoidValve, LOW);
}
