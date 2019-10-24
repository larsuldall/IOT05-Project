/*
 * Project firmwarePhoton
 * Description:
 * Author: Lars Juhl
 * Date: 19/10/2019
 */

Servo myservo;

int servoPin = D0;


void setup() {

    Particle.subscribe("TurnServoOn", ElectricKettleOn, MY_DEVICES);
    Particle.function("BoilWater", manuallyBoil);

    myservo.attach(servoPin);
    myservo.write(90); // move servo to 90 degree (Start position)
    
}

void loop() {

}


// Function to manually turn the electric kettle to boil from the ParticleAPP
int manuallyBoil(String command)   // when "manuallyBoil" is called from the cloud, it will
{                                  // be accompanied by a string.
    if(command == "boil")          // if the string is "boil", servo turn electric ketle on.
    {                            
        myservo.write(25);         // move servo to 25°
        delay(1500);               // wait 1.5 secund
        myservo.write(90);         // move servo to 90°
        return 1;                  // return a status of "1"
    }
    
}

// Function to boil water when "Smart morning routine" triggers.
void ElectricKettleOn (const char *event, const char *data){
  myservo.write(25);
  myservo.write(90);
}