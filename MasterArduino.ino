#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear(); // Clear any existing garbage characters
  Serial.begin(9600);

  // START THE CLOCK (Crucial for new DS1307)
  Wire.beginTransmission(0x68);
  Wire.write(0x00); // Point to the seconds register
  Wire.write(0x00); // Write 0 to the 7th bit (CH) to start the clock
  Wire.endTransmission();
}

byte bcdToDec(byte val) {
  return ((val / 16 * 10) + (val % 16));
}

void loop() {
  Wire.beginTransmission(0x68);
  Wire.write(0); 
  Wire.endTransmission();

  Wire.requestFrom(0x68, 7);

  if (Wire.available()) {
    int second = bcdToDec(Wire.read() & 0x7f);
    int minute = bcdToDec(Wire.read());
    int hour   = bcdToDec(Wire.read() & 0x3f);
    Wire.read(); 
    int day    = bcdToDec(Wire.read());
    int month  = bcdToDec(Wire.read());
    int year   = bcdToDec(Wire.read());

    // Print to Serial Monitor to verify the Arduino is actually reading data
    Serial.print("Time: "); Serial.print(hour); Serial.print(":"); Serial.println(minute);

    lcd.setCursor(0, 0);
    lcd.print("Date: ");
    if(day < 10) lcd.print('0'); lcd.print(day); lcd.print("/");
    if(month < 10) lcd.print('0'); lcd.print(month); lcd.print("/20");
    lcd.print(year);

    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    if(hour < 10) lcd.print('0'); lcd.print(hour); lcd.print(":");
    if(minute < 10) lcd.print('0'); lcd.print(minute); lcd.print(":");
    if(second < 10) lcd.print('0'); lcd.print(second);
  }

  delay(1000);
}