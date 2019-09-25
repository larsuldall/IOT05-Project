/*
 * Project firmware
 * Description: IOT Project (Automatic morning routine)
 * Author: Lars Juhl  
 * Date:
 */

// Declare variables
int pirPin = D0;                // choose the input pin (for PIR sensor)
int ledPin = D1;                // LED Pin
int tempRead = A5;              // LM35 Analog pin

int pirState = LOW;             // we start assuming no motion detected
int PIRval = 0;                 // variable for reading the PIR pin status
int calibrateTime = 5000;       // wait for the PIR sensor to calibrate
int analogTempValue = 0;        // Anolog temperature value
int adc_value = 0;
double tempVariable = 0;
int actualTime = 0;
String temp;

// Initial setup
void setup() {
    pinMode(ledPin, OUTPUT);      // declare LED as OUTPUT
    pinMode(pirPin, INPUT);       // declare PIR sensor as input
    pinMode(tempRead, INPUT);     // declare tempSensor as input

    Particle.variable("PIR", PIRval);         // Cloud variable of PIR value
    Particle.variable("Temperature", tempVariable);  // Cloud variable of temp 
    Particle.variable("Time", actualTime);
}

// Program running
void loop() 
{
  actualTime = millis()/1000; // Secunds that the program has run

  // if the sensor is calibrated PIR detects motion
  if (calibratedPIR()) 
    {
    // get the data from the sensor
    readThePIRSensor();

    // report it out, if the state has changed
    reportThePIRData();
    }
  
  // Get temperature
  if (modulusTime())    // If True (That is every 10 secunds)
    {
      if ((actualTime*1000)-millis() == 0) // And if this is true read the temperature.
      {
          readTemperatureFunc();
      }
    }
}


// FUNCTIONS
//

// PIR Motion detector
void readThePIRSensor() {
    PIRval = digitalRead(pirPin);   // Reads 0 or 1
}

bool calibratedPIR() {
    return millis() - calibrateTime > 0;  // When the calibration time is finished its True
}

void setLED(int state) {
    digitalWrite(ledPin, state);    
}

void reportThePIRData() {
    if (PIRval == HIGH) {
        // the current state is no motion
        // i.e. it's just changed
        // announce this change by publishing an event
        if (pirState == LOW) {
          // we have just turned on
          Particle.publish("PhotonMotion", "Motion Detected", PRIVATE);
          
          // Update the current state
          pirState = HIGH;
          setLED(pirState);
        }
    } else {
        if (pirState == HIGH) {
          // we have just turned of
          // Update the current state
          Particle.publish("PhotonMotion", "Off", PRIVATE);
          pirState = LOW;
          setLED(pirState);
        }
    }
}

// Temperature sensor
void readTemperatureFunc(){
    adc_value = analogRead(tempRead);           // Reads the adc value (max 1024 = 5V) 10mV/1deg C
    float tempValue = (adc_value*0.80)/10;      // Calculate to Celsius
    tempVariable = tempValue;
    String temp = String(tempValue);            // Convert the Celsius result to a string
    Particle.publish("temp", temp, PRIVATE);    // Publish an event named "temp" with the data of temp
}

bool modulusTime() {    
    return (millis()/1000) % 60 == 0;     // Takes millisecunds the program has run, into sekunds, and divide with 60
                                          // so it is only true every 60 sekunds
}