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



// Initial setup
void setup() {
    pinMode(ledPin, OUTPUT);      // declare LED as OUTPUT
    pinMode(pirPin, INPUT);       // declare PIR sensor as input
    pinMode(tempRead, INPUT);     // declare tempSensor as input

    Particle.variable("PIR", PIRval);                       // Cloud variable of PIR value
    Particle.variable("analogTempValue", analogTempValue);  // Cloud variable of temp 
}

// Program running
void loop() {

  // if the sensor is calibrated
  if (calibratedPIR()) {
    // get the data from the sensor
    readThePIRSensor();

    // report it out, if the state has changed
    reportThePIRData();
    }
    delay(1000);
  // Get temperature
  readTemperatureFunc();
}


// Functions

// PIR Motion detector
void readThePIRSensor() {
    PIRval = digitalRead(pirPin);   // Reads 0 or 1
}

bool calibratedPIR() {
    return millis() - calibrateTime > 0;
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
    int adc_value = analogRead(tempRead);     // Reads the adc value (max 1024 = 5V) 10mV/1deg C
    float tempVal = (adc_value*0.80)/10;      // Calculate to Celsius
    String temp = String(tempVal);            // Convert the Celsius result to a string
    Particle.publish("temp", temp, PRIVATE);  // Publish an event named "temp" with the data of temp
}