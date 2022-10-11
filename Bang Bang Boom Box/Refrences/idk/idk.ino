void setup() {
  // put your setup code here, to run once:
  Serial.begin(4800);
  Serial1.begin(115200);

   Serial.println("off");
   Serial1.println("awl0");
   delay(50);
   while (Serial1.available() > 0) {
     Serial1.read();
   }

}

void loop() {
   delay(1000);  // put your main code here, to run repeatedly:
   Serial.println("hi");
   Serial1.println("awo0");
   delay(50);
   while (Serial1.available() > 0) {
     Serial1.read();
   }

   delay(1000);
      
   Serial1.println("awo1");
   delay(50);
   while (Serial1.available() > 0) {
     Serial1.read();
   }
        
}
