/*
 * Project Smart home (Argon)
 * Description: IOT Project (Automatic morning routine)
 * Author: Lars Juhl  
 * Date: 16/10/2019
 */

// Declare variables
int pirPin = D0;                // choose the input pin (for PIR sensor)
int ledPin = D1;                // LED Pin
int tempRead = A0;              // LM35 Analog pin

int pirState = LOW;             // we start assuming no motion detected
int PIRval = 0;                 // variable for reading the PIR pin status
int calibrateTime = 5000;       // wait for the PIR sensor to calibrate
int analogTempValue = 0;        // Anolog temperature value
int adc_value = 0;
double tempVariable = 0;
int actualTime = 0;
int count = 0;
String temp;
int burglerAlarmStatus = 0;

int hour = 0; 

// Initial setup
void setup() {
    Serial.begin(9600);           // Starts serial communication (Used for testing)
    pinMode(ledPin, OUTPUT);      // declare LED as OUTPUT
    pinMode(pirPin, INPUT);       // declare PIR sensor as input
    pinMode(tempRead, INPUT);     // declare tempSensor as input

    Particle.variable("PIR", PIRval);         // Cloud variable of PIR value
    Particle.variable("Temperature", tempVariable);  // Cloud variable of temp 
    Particle.variable("Hour", hour);
    Particle.variable("count", count);

    Particle.function("BurglerAlarm", burglerAlarm);
}

// Program running
void loop() 
{
  hour = Time.hour() + 1; // Stores the current hour (UTC) + 1 for local time
  actualTime = millis()/1000; // Seconds that the program has run

  // if the sensor is calibrated PIR detects motion
  if (calibratedPIR()) 
    {
    // get the data from the sensor
    readThePIRSensor();

    // report it out, if the state has changed
    reportThePIRData();

      // Morning routine only run once a day
      if (hour >= 6 && hour < 9) //Events should only run once in timeinterval 6 to 9
        {
           if(PIRval == 1 && count < 1)
           {
           Particle.publish("TurnServoOn", "Initiate Servo", PRIVATE); // Event that the photon subscribes to
           // Her kommer webhook til at tænde Hue lys
           Particle.publish("TurnLightsOn", "Hue is starting morning routine", PRIVATE);
           // Her kommer webhook til at tænde radioen
           count = 1;
           }
 
        } else 
          {
          count = 0;  // Restarts count so it ONLY executes ones a day. (First time you get up)
          }
    }
  
  // Get temperature and Firealarm
  if (modulusTime())    // If True (That is every 10 seconds)
    {
      if ((actualTime*1000)-millis() == 0) // And if this is true read the temperature.
      {
          readTemperatureFunc();

          // Firealarm
          if(tempVariable > 26) // If temperature reach 26 or above, the alarm will go off via webhook
          {
            Particle.publish("triggerFireAlarm", "It's burning", PRIVATE);
          }
      }
    }

  // Burgler alarm
  if (burglerAlarmStatus == 1) // Activated with particle function in APP
    {
      if (PIRval == 1)        // When motion is detected
        {
          // Publish an event that the HUE webhook is 
          Particle.publish("triggerBurglerAlarm", "You have an intruder", PRIVATE);
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

void reportThePIRData() {
    if (PIRval == HIGH) {
        // the current state is no motion
        // i.e. it's just changed
        // announce this change by publishing an event
        if (pirState == LOW) {
          // we have just turned on
          Particle.publish("ArgonMotion", "Motion Detected", PRIVATE);
          
          // Update the current state
          pirState = HIGH;
          setLED(pirState);
        }
    } else {
        if (pirState == HIGH) {
          // we have just turned of
          // Update the current state
          Particle.publish("ArgonMotion", "Off", PRIVATE);
          pirState = LOW;
          setLED(pirState);
        }
    }
}

// LED lights up when motion is detected
void setLED(int state) {
    digitalWrite(ledPin, state);    
}

// Temperature sensor
void readTemperatureFunc(){
    adc_value = analogRead(tempRead);           // Reads the adc value (max 1024 = 5V) 10mV/1deg C
    float tempValue = (adc_value*0.80)/10;      // Calculate to Celsius
    tempVariable = tempValue;
    String tempC = String(tempValue);            // Convert the Celsius result to a string
    Particle.publish("temp", tempC, PRIVATE);    // Publish an event named "temp" with the data of tempC
}

bool modulusTime() {    
    return (millis()/1000) % 10 == 0;     // Takes millisecunds the program has run, into secunds, and divide with 10
                                          // so it is only true every 10 sekunds
}

// Set burgler alarm on/off
int burglerAlarm(String command) {

    if (command=="on") {
        burglerAlarmStatus = 1;
        return 1;
    }
    else if (command=="off") {
        burglerAlarmStatus = 0;
        return 0;
    }
    else {
        return -1;
    }

}