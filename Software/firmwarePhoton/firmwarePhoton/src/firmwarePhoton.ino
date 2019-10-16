/*
 * Project firmwarePhoton
 * Description:
 * Author:
 * Date:
 */

void setup() {
    pinMode(D7, OUTPUT);
    Particle.subscribe("hook-response/temp", blinkLED, MY_DEVICES);
    Particle.subscribe("PhotonMotion", blinkLED, MY_DEVICES);
    Particle.subscribe("TurnServoOn", blinkLED, MY_DEVICES);
}

void loop() {


}




void blinkLED(const char *event, const char *data) {
  // Handle the integration response
    digitalWrite(D7, HIGH);
    delay(500);
    digitalWrite(D7, LOW);
    delay(100);
}
          
void turnServoOn(const char *event, const char *data){ // Add servo motor code here
    digitalWrite(D7, HIGH);
    delay(1500);
    digitalWrite(D7, LOW);
    delay(1500);
}
