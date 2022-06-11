double aValue;
double bValue;
double cValue;
double x;

void setup() {
 Serial.begin(9600);
}

void loop() {
  x = x + .1;
  //aValue = random(0, 1000) / 10.0;
  //bValue = random(1001, 2000) / 10.0;
  //cValue = random(2001, 3000) / 10.0;
  aValue = sin(x);
  bValue = sin(x)*sin(x);
  cValue = sin(x)*sin(x)*sin(x);
  Serial.print("a");
  Serial.println(aValue);
  Serial.print("b");
  Serial.println(bValue);
  Serial.print("c");
  Serial.println(cValue);
  delay(1);
}
