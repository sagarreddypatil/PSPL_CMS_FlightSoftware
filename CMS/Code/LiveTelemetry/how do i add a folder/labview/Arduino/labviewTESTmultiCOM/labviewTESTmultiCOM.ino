#include <SoftwareSerial.h>

SoftwareSerial portOne(10,11);
SoftwareSerial portTwo(8, 9);

void setup() {
  //Serial.begin(9600);
  portOne.begin(9600);
  portTwo.begin(9600);

  while (!Serial) {

    ; // wait for serial port to connect. Needed for native USB port only

  }
}

unsigned long dt = 10, prevTime = 0;
  
void loop() {
  double x;
  double otherX;
  portOne.listen();
  while (portOne.available() > 0) {
      while(micros() >= prevTime + dt){
      x = random(0, 1000);
      prevTime = micros();
      }
    }
    Serial.write(x);

  portTwo.listen();
  while (portTwo.available() > 0) {
      while(micros() >= prevTime + dt){
      otherX = random(0, 1000);
      prevTime = micros();
    }
    Serial.write(otherX);
  }
}
