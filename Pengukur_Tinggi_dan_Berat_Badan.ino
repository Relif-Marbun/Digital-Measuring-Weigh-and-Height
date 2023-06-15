//HEADER
#include <HX711_ADC.h>            //library Loadcell
#include <EEPROM.h>               //library eeprom
#include <NewPing.h>              //library ultrasonik            
#include <Wire.h>                 //library komunikasi I2C
#include <LiquidCrystal_I2C.h>    //library LCD    

//deklarasi pin sensor ultrasonik di pin digital 4 dan 5
#define TRIGGER_PIN  4
#define ECHO_PIN     5
#define MAX_DISTANCE 200

//deklarasi pin Loadcell di pin 2 dan 3
const int HX711_dout = 2;
const int HX711_sck = 3;

//deklarasi variabel
int landasan, tinggi, tinggibadan, beratbadan; float i;

HX711_ADC LoadCell(HX711_dout, HX711_sck); NewPing ultrasonic (TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); LiquidCrystal_I2C lcd(0x27, 16, 2);

const int calVal_calVal_eepromAdress = 0;
unsigned long t = 0;

void setup() {
  Serial.begin(57600); delay(10);
  Serial.println();
  Serial.println("Starting...");
  float calibrationValue;
  calibrationValue = 21.41; //nilai kalibrasi sensor berat
  LoadCell.begin();
  unsigned long stabilizingtime = 2000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration factor (float)
    Serial.println("Startup is complete");
  }
  while (!LoadCell.update());
  Serial.print("Calibration value: ");
  Serial.println(LoadCell.getCalFactor());
  Serial.print("HX711 measured conversion time ms: ");
  Serial.println(LoadCell.getConversionTime());
  Serial.print("HX711 measured sampling rate HZ: ");
  Serial.println(LoadCell.getSPS());
  Serial.print("HX711 measured settlingtime ms: ");
  Serial.println(LoadCell.getSettlingTime());
  Serial.println("Note that the settling time may increase significantly if you use delay() in your sketch!");
  if (LoadCell.getSPS() < 7) {
    Serial.println("!!Sampling rate is lower than specification, check MCU>HX711 wiring and pin designations");
  }
  else if (LoadCell.getSPS() > 100) {
    Serial.println("!!Sampling rate is higher than specification, check MCU>HX711 wiring and pin designations");
  }
  //Memulai LCD
  lcd.init(); lcd.backlight();
  lcd.setCursor(0, 0); lcd.clear(); lcd.print("==WELCOME TO==");
  lcd.setCursor(2, 1); lcd.print("OUR SYSTEM");
  delay(2000);
}

void loop() {
  lcd.clear();
  static boolean newDataReady = 0;
  const int serialPrintInterval = 500; 
  if (LoadCell.update()) newDataReady = true;
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      i = LoadCell.getData();
      Serial.print("Load_cell output val: ");
      Serial.println(i);
      newDataReady = 0;
      t = millis();
    }
  }
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }

  landasan = 200;
  tinggi   = ultrasonic.ping_cm();
  tinggibadan = landasan - tinggi;
  int beratbadan = i / 1000;
  lcd.setCursor(0, 0); lcd.print("T.B(cm): "); lcd.setCursor(9, 0); lcd.print(tinggibadan);
  lcd.setCursor(0, 1); lcd.print("B.B(kg): "); lcd.setCursor(9, 1); lcd.print(beratbadan);
  Serial.print("T. B: "); Serial.print(tinggibadan); Serial.print("  ");
  Serial.print("B. B: "); Serial.println(beratbadan);
  delay(300);
}
