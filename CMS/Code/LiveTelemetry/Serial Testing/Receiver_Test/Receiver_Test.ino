int x = 0;
void setup() {
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available()==0);
  int data = Serial.read() - '0';
  
  Serial.print(x);
  Serial.print(" ");
  Serial.println(data);
  x++;
}
