

void setup()
{
  Serial.begin(9600);

}

unsigned long dt = 15, prevTime = 0;
  
void loop()
{
  while(micros() >= prevTime + dt){
    double x = random(0, 1000);
    Serial.println(x);
      
    prevTime = micros();
    }
}
