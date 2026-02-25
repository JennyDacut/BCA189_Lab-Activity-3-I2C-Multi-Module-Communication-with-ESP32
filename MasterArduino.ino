#include <Wire.h>               // We use this for the I2C communication protocol
#include <LiquidCrystal_I2C.h>  // We use this to control our 16x2 LCD screen

// We initialize our LCD at address 0x27. 
// Note: If our screen doesn't show text, we might need to change 0x27 to 0x3F.
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Wire.begin();        // We join the I2C bus as the Master controller
  lcd.init();          // We wake up the LCD
  lcd.backlight();     // We turn on the backlight so we can see the text
  lcd.clear();         // We clear the screen to remove any startup glitches
  Serial.begin(9600);  // We start the Serial monitor so we can debug on our PC

  // --- INITIALIZING OUR RTC MODULE ---
  // We found that the DS1307 starts in a 'Halted' state by default.
  // We send a 0 to the first register to 'kickstart' the internal clock.
  Wire.beginTransmission(0x68); // 0x68 is our RTC's unique I2C address
  Wire.write(0x00);             // We point to the Seconds Register
  Wire.write(0x00);             // We write 0 to the CH bit to start the oscillator
  Wire.endTransmission();       // We finish our setup command
}

/**
 * We created this function to convert Binary Coded Decimal (BCD) 
 * from the sensor into regular Decimal numbers for our display.
 */
byte bcdToDec(byte val) {
  return ((val / 16 * 10) + (val % 16));
}

void loop() {
  // 1. We tell the RTC to start reading from the very beginning (Register 0)
  Wire.beginTransmission(0x68);
  Wire.write(0); 
  Wire.endTransmission();

  // 2. We request 7 bytes of data from our RTC module
  // This gives us: Seconds, Minutes, Hours, Day, Date, Month, and Year
  Wire.requestFrom(0x68, 7);

  if (Wire.available()) {
    // We read each byte and convert it immediately using our function.
    // We use '&' to mask out unnecessary status bits from the RTC.
    int second = bcdToDec(Wire.read() & 0x7f); 
    int minute = bcdToDec(Wire.read());
    int hour   = bcdToDec(Wire.read() & 0x3f); 
    Wire.read();                               // We skip the Day of the Week byte
    int day    = bcdToDec(Wire.read());
    int month  = bcdToDec(Wire.read());
    int year   = bcdToDec(Wire.read());

    // We print the time to our Serial Monitor to double-check our logic
    Serial.print("System Time: "); 
    Serial.print(hour); Serial.print(":"); Serial.println(minute);

    // 3. We format and display our Date on the top row
    lcd.setCursor(0, 0);
    lcd.print("Date: ");
    if(day < 10) lcd.print('0');   lcd.print(day);   lcd.print("/");
    if(month < 10) lcd.print('0'); lcd.print(month); lcd.print("/20");
    lcd.print(year);

    // 4. We format and display our Time on the bottom row
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    if(hour < 10) lcd.print('0');   lcd.print(hour);   lcd.print(":");
    if(minute < 10) lcd.print('0'); lcd.print(minute); lcd.print(":");
    if(second < 10) lcd.print('0'); lcd.print(second);
  }

  delay(1000); // We set our refresh rate to 1 second
}
