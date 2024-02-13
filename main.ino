/* Arduino code for Panic Attack Monitor and Control (P.A.Mo.C.) device */

/* LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K potentiometer: ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */

#include <LiquidCrystal.h>				// includes the LiquidCrystal Library
#include <PulseSensorPlayground.h>		// includes the PulseSensorPlayground Library
#define USE_ARDUINO_INTERRUPTS true		// set-up low-level interrupts for most acurate BPM math

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int pulse = 0;		// signal pin connected to A0
int threshold = 120;		// set threshold for pulse

// reference voltage used in calculating the real voltage value
float ARef = 1.1;

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

int motorPin = 6; 	// motor transistor is connected to pin 6

int row_excel = 0; 	// initialize number of lines in excel

/*------------------------------------------------------------------------------*/

/* Setup section used to initialize variables. pin modes, etc. */
void setup() {
	// set up the LCD's number of columns and rows
	lcd.begin(8,2);
	// print a message to the LCD
	lcd.print("Welcome!");
	
	// initialize the serial communication
	Serial.begin(9600);
	
	// configure the PulseSensor object
	pulseSensor.analogInput(pulse);   
	pulseSensor.setThreshold(threshold);
	
	// use the internal reference
	analogReference(INTERNAL);
	
	lcd.begin();
	
	// create characters for battery bars and heart illustration
	lcd.createChar(0,Bar);
	lcd.createChar(1,Heart);
	
	// set the pin for motor tranistor as output
	pinMode(motorPin, OUTPUT);
}

/*------------------------------------------------------------------------------*/

/* Loop section loops consecutively, allowing program to change and respond */
void loop() {

/* Display battery bars accoding to the battery voltage */

	// read the voltage on the A0 pin
	float raw = analogRead(A0);
	
	// convert this value to voltage
	float x = raw/1023;
	float voltage = x*ARef*5;
	
	if (voltage > 0 && voltage < 1){
		// one bar on sixth column, line 0
		// (note: line 0 is the first row, since counting begins with 0)
		lcd.setCursor(7,0);
		lcd.write(byte(0));
	}else if (voltage > 1 && voltage < 2.35){
		// two bars
		lcd.setCursor(7,0);
		lcd.write(byte(0));
		lcd.setCursor(6,0);
		lcd.write(byte(0));
	}else if (voltage > 2.35 && voltage < 3.7){
		// three bars
		lcd.setCursor(7,0);
		lcd.write(byte(0));
		lcd.setCursor(6,0);
		lcd.write(byte(0));
		lcd.setCursor(5,0);
		lcd.write(byte(0));
	}else{
		// four bars - fully charged
		lcd.setCursor(7,0);
		lcd.write(byte(0));
		lcd.setCursor(6,0);
		lcd.write(byte(0));
		lcd.setCursor(5,0);
		lcd.write(byte(0));
		lcd.setCursor(4,0);
		lcd.write(byte(0));
	}
	
/*------------------------------------------------------------------------------*/

/* Dispay heart graphic and pulse sensor output from Serial port to LCD monitor */
	
	// set the cursor to column 0, line 1 (second line) and display heart
	lcd.setCursor(0,1);
	lcd.write(byte(1));
	
	int bpm = pulseSensor.getBeatsPerMinute();	// calculate BPM
	
	// constantly test to see if beat is found
	if (pulseSensor.sawStartOfBeat()) {
		Serial.print("BPM: ");                  // print phrase "BPM: " 
		Serial.println(bpm);                    // print the BPM value
	}
	
	// when characters arrive over the serial port...
	if (Serial.available()) {
		// wait a bit for the entire message to arrive
		delay(100);
		
		// set the cursor to column 2, line 1
		lcd.setCursor(2,1);
		
		// read all the available characters
		while (Serial.available() > 0) {
			// display each character to the LCD
			lcd.write(Serial.read());
		}
	}

	// display items and text on LCD
	lcd.display();

/*------------------------------------------------------------------------------*/

/* Motor vibrations to guide user during breathing technique */

	// check if pulse is over threshold
	if (bpm > threshold) {
		
		// repeat 10 times (10 breaths in total)
		for (int i=0; i<10; i++)
		{
			// set cursor to column 2, line 1 and give user command
			lcd.setCursor(2,1);
			lcd.print("Inhale");
		
			// vibrate for four seconds (inhale)
			digitalWrite(motorPin, HIGH);
			delay(4000);
	
			// set cursor to column 2, line 1 and give user command
			lcd.setCursor(2,1);
			lcd.print("Hold");
			
			// stop vibrating for two seconds (hold breath)
			digitalWrite(motorPin, LOW);
			delay(2000); 
	
			// set cursor to column 2, line 1 and give user command
			lcd.setCursor(2,1);
			lcd.print("Exhale");
			
			// vibrate for four seconds (exhale)
			digitalWrite(motorPin, HIGH);
			delay(4000);

			// set cursor to column 2, line 1 and give user command
			lcd.setCursor(2,1);
			lcd.print("Hold");
			
			// stop vibrating for two seconds (until next breath)
			digitalWrite(motorPin, LOW);
			delay(2000);
		}
	}
	
/*------------------------------------------------------------------------------*/
	
	delay(1000);	// 1 second delay
}