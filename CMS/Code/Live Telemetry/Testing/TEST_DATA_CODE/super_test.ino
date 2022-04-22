#include <Wire.h>
int aValue=0, bValue=0, cValue=0, x=0;
unsigned long start;

void setup() {
 Serial.begin(9600);
 start = millis();
 }

void loop() {
//Data to send to LabView:
    /*
    aValue = random(1, 1000) / 100.0;
    bValue = random(1001, 2000) / 100.0;
    cValue = random(2001, 3000) / 100.0;
    */
    
    aValue = sin(x)+5;
    
    if((millis() - start) >=1){
    bValue = ! bValue;
    //output_state *= 1023;
    start = millis();
  }
    
    Serial.print(aValue);
    Serial.print(",");
    Serial.print(bValue);
    //Serial.print(",");
    //Serial.print(cValue);
    Serial.println(",");
    x++;
    //delay(1);
}
