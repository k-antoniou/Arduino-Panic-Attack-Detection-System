/* Arduino code for Panic Attack Monitor and Control (P.A.Mo.C.) device */
#include <PulseSensorPlayground.h>
#include <LiquidCrystal.h>

#define USE_ARDUINO_INTERRUPTS true

// Variables
PulseSensorPlayground pulseSensor;
const int PulseWire = 0;
int Threshold = 550;
int bpm_tresh = 120;

int motorPin = 11; 

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
// user-defined graphics: bar
byte Bar[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

// user-defined graphics: heart
byte Heart[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
};

void setup() {
  Serial.begin(9600); // For Serial Monitor
  lcd.begin(16,2);
  // create characters for battery bars and heart illustration
  lcd.createChar(0,Bar);
  lcd.createChar(1,Heart);
  // setup pulse sensor
  pulseSensor.setThreshold(Threshold);
  // set the pin for motor as output
  pinMode(motorPin, OUTPUT);

  if (pulseSensor.begin()) {
    lcd.setCursor(3,0);
    lcd.print("Welcome to");
    lcd.setCursor(3,1);
    lcd.print("P.A.M.O.C!");
    delay(500);
    lcd.clear();
  }
}

void loop() {
  
  int bpm = pulseSensor.getBeatsPerMinute();
  if (pulseSensor.sawStartOfBeat()) 
  { 
      // check if pulse is over bpm threshold
      if (is_panicked(bpm))
      {
       panic_attack_control_response(); //Breathing exercises
      }
      else
      {
        //Print happy message
        lcd.setCursor(0,0);
        lcd.print("Everything");
        lcd.setCursor(0,1);
        lcd.print("is ok");
        lcd.setCursor(15,1);
        lcd.write(byte(1));
      }
  }

  //Print battery level
  battery_lvl_display();
  delay(1000);  // 1 second delay
  lcd.clear();

}
/* Motor vibrations to guide user during breathing technique */
void panic_attack_control_response()
{
    lcd.clear();
    // repeat 10 times (10 breaths in total)
    for (int i=0; i<10; i++)
    {
      // set cursor to column 2, line 1 and give user command
      lcd.setCursor(5,1);
      lcd.print("Inhale");
      
      // vibrate for four seconds (inhale)
      digitalWrite(motorPin, HIGH);
      delay(4000);
      lcd.clear();
      // set cursor to column 2, line 1 and give user command
      lcd.setCursor(6,1);
      lcd.print("Hold");
      
      // stop vibrating for two seconds (hold breath)
      digitalWrite(motorPin, LOW);
      delay(2000); 
      lcd.clear();
      // set cursor to column 2, line 1 and give user command
      lcd.setCursor(5,1);
      lcd.print("Exhale");
      
      // vibrate for four seconds (exhale)
      digitalWrite(motorPin, HIGH);
      delay(4000);
      lcd.clear();
      // set cursor to column 2, line 1 and give user command
      lcd.setCursor(2,1);
      lcd.print("You are safe");
      
      // stop vibrating for two seconds (until next breath)
      digitalWrite(motorPin, LOW);
      delay(2000);
      lcd.clear();
    }
}

/* Check if user is panicked */
bool is_panicked(int bpm)
{
  if (bpm > bpm_tresh) 
  {
    return true;

  }
  else
  {
    return false;
  }
}

/* Display battery bars according to the battery voltage */
void battery_lvl_display()
{

  // read the voltage on the A1 pin
  float x = analogRead(A1);
  
  // convert this value to voltage
  float voltage = x*5/1023;
  
  if (voltage > 0 && voltage < 1){
    // one bar
    lcd.setCursor(15,0);
    lcd.write(byte(0));
  }else if (voltage > 1 && voltage < 2.35){
    // two bars
    lcd.setCursor(15,0);
    lcd.write(byte(0));
    lcd.setCursor(14,0);
    lcd.write(byte(0));
  }else if (voltage > 2.35 && voltage < 3.7){
    // three bars
    lcd.setCursor(15,0);
    lcd.write(byte(0));
    lcd.setCursor(14,0);
    lcd.write(byte(0));
    lcd.setCursor(13,0);
    lcd.write(byte(0));
  }else{
    // four bars - fully charged
    lcd.setCursor(15,0);
    lcd.write(byte(0));
    lcd.setCursor(14,0);
    lcd.write(byte(0));
    lcd.setCursor(13,0);
    lcd.write(byte(0));
    lcd.setCursor(12,0);
    lcd.write(byte(0));
  }
}
