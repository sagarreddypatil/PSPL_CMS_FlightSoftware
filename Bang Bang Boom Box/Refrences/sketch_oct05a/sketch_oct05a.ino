#include <Wire.h>
#

#define INPUT_SIZE 37  // Max number of bytes expected to come over in a command (1 char + 9 longs)
#define NUM_INPUTS 10  // Max number of different inputs we expect
#define INPUT_SIZE1 100  // Max number of bytes to come over from charger Rx

//define LCD control pins
const int rs = A3, en = A5, d4 = A9, d5 = A10, d6 = A11, d7 = A12;
#include <LiquidCrystal.h>
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

char v_reading;
char i_reading;

bool debugging = false;

float battVoltageErrorThreshold = 140.0;
float battCurrentErrorThreshold = 1000.0;

int battVoltages[6] = {0, 0, 0, 0, 0, 0};
int battVoltagesPre[6] = {0, 0, 0, 0, 0, 0};
int priorbattVoltages[6] = {0, 0, 0, 0, 0, 0};
int priorpriorbattVoltages[6] = {0, 0, 0, 0, 0, 0};

bool isBattCharging[6] = {false, false, false, false, false, false};
bool isBattFull[6] = {false, false, false, false, false, false};
int battCurrents[6] = {0, 0, 0, 0, 0, 0};
int battCurrentsPre[6] = {0, 0, 0, 0, 0, 0};
int priorbattCurrents[6] = {0, 0, 0, 0, 0, 0};
int priorpriorbattCurrents[6] = {0, 0, 0, 0, 0, 0};
int numBatteries = 6;
String desiredCurrent = "1000";

String allStatus;
int cmdCount = 0;
char cmd = '0';
char* parsedCommand;
long cmd_parameter[NUM_INPUTS];
char input[INPUT_SIZE + 1];
char input1[INPUT_SIZE1 + 1];
byte size;  // Size of the incoming command as read from Serial


void setup() {
  // External comms over serial0, comms with chargers on serial1
  Serial.begin(115200);
  Serial1.begin(9600);
  delay(100);
  Serial.println("DroneHome Charger Starting");

  //set LCD Control pins
  pinMode(A14,OUTPUT);
  pinMode(A13,OUTPUT);
  pinMode(A4,OUTPUT);
  pinMode(A0,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A1,OUTPUT);

  //used for bread board not final product 
  digitalWrite(A14,LOW); 
  digitalWrite(A13,HIGH); 
  digitalWrite(A4,LOW); 
  digitalWrite(A0,LOW);
  digitalWrite(A1,HIGH);

//initialize the LCD
  lcd.begin(20, 4);

  // Prints Charging Bays on the LCD during startup
  lcd.setCursor (2,0);
  lcd.print("Battery Voltages");
  lcd.setCursor (0,1);
  lcd.print("B2:");
  lcd.setCursor (11,1);
  lcd.print("B3:");
  lcd.setCursor (0,2);
  lcd.print("B6:");
  lcd.setCursor (11,2);
  lcd.print("B7:");
  lcd.setCursor (0,3);
  lcd.print("B10:");
  lcd.setCursor (11,3);
  lcd.print("B11:");
  
  //pins for mux address
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);

  //Run through each address and initialize voltage and current limits
  for (int n = 0; n < 6; n++) {
    delay(50);
    assign_address(n);
    delay(50);
    Serial1.println("awo0");
    delay(50);
    write_settings();

    while (Serial1.available() > 0) {
      // read the incoming byte to clear the buffer
      Serial1.read();
    }
  }

  Serial.println("DroneHome Chargers Initialized");
}


void loop() {

  read_command();

  //testing mode command 
  if (cmd == 't'){
    Serial.println("Entering Testing Mode! \nTo clear the testing mode power cycle, wait for test completion, or restart serial monitor.");
    Serial.println("-----------------------------------------------------------------------------------------");
  int fail = 0; 
  int count = 0;
  lcd.begin(20, 4);
  lcd.setCursor (4,0);
  lcd.print("Test Mode");
  lcd.setCursor (0,1);
  lcd.print("B2:");
  lcd.setCursor (11,1);
  lcd.print("B3:");
  lcd.setCursor (0,2);
  lcd.print("B6:");
  lcd.setCursor (11,2);
  lcd.print("B7:");
  lcd.setCursor (0,3);
  lcd.print("B10:");
  lcd.setCursor (11,3);
  lcd.print("B11:");
  
   while (fail < 5 ){
    count++;
      //check the voltage of each battery. if 0 then test fail 
      all_set_and_on();
      for (int n = 0; n < numBatteries; n++){
        assign_address(n);
        //Serial.write("[2J")
        delay(50);
        if (tesing(n) == 0){
          fail++ ;
        }
      }
      //resume_on();
      all_set_and_on();
      Serial.print("All Chargers Tested ");
      Serial.print(count);
      Serial.print(" Times.   Fails: ");
      Serial.println(fail);
   }
   all_off();
   
//initialize the LCD
  lcd.begin(20, 4);

  // Prints Charging Bays on the LCD during startup
  lcd.setCursor (2,0);
  lcd.print("Battery Voltages");
  lcd.setCursor (0,1);
  lcd.print("B2:");
  lcd.setCursor (11,1);
  lcd.print("B3:");
  lcd.setCursor (0,2);
  lcd.print("B6:");
  lcd.setCursor (11,2);
  lcd.print("B7:");
  lcd.setCursor (0,3);
  lcd.print("B10:");
  lcd.setCursor (11,3);
  lcd.print("B11:");
  
   Serial.println("Test Complete.....");
   Serial.print("The Total Iterations before 5 fails:  ");
   Serial.println(count);
   Serial.println("-----------------------------------------------------------------------------------------");
   Serial.println("Resuming Normal Operation.....");
   Serial.println("-----------------------------------------------------------------------------------------");
   
  }

  //status command
  if (cmd == 'a'){
    // Grab current while the chargers are on
    for (int n = 0; n < numBatteries; n++){
      assign_address(n);
      delay(50);
      read_current(n);
    }
    // Turn off all the chargers so that we can get valid readings
    all_off();
    delay(5000);
    for (int n = 0; n < numBatteries; n++){
      assign_address(n);
      delay(50);
      read_voltage(n);
    }

    getStatus();
    Serial.println(allStatus);
    resume_on();
  }

  // initialize command
  if (cmd == 'i'){
    if (cmd_parameter[0] > 0){
      if (cmd_parameter[0] < 1000){
        desiredCurrent = "0" + String(cmd_parameter[0]);
      } else {
        desiredCurrent = String(cmd_parameter[0]);
      }
    }
    set_all();
  }

  // on command (o,8 = all on    o,0 = bay 1 on ... o,5 = bay 6 on)
  if (cmd == 'o'){
    if (cmd_parameter[0] == 8){

      // cmd_parameter[1] holds the value desired for the current
      if (cmd_parameter[1] > 0){
        if (cmd_parameter[1] < 1000){
          desiredCurrent = "0" + String(cmd_parameter[1]);
        } else {
          desiredCurrent = String(cmd_parameter[1]);
        }
      }
      all_set_and_on();

    } else if (cmd_parameter[0] >= 0 && cmd_parameter[0] < numBatteries){
      assign_address(cmd_parameter[0]);
      delay(50);
      isBattCharging[cmd_parameter[0]] = true;
      if (isBattFull[cmd_parameter[0]] == false){
        voltage_on();
      } else {
        Serial.println("Battery Full");
      }

    } else {
      Serial.println("ERROR_99, NOT AN AVAILABLE BAY");
    }
  }

  // off command (f,8 = all off    f,0 = bay 1 off ... f,5 = bay 6 off)
  if (cmd == 'f'){
    if (cmd_parameter[0] == 8){
      all_off();
    for (int n = 0; n < numBatteries; n++){
        isBattCharging[n] = false;
      }
    } else if (cmd_parameter[0] >= 0 && cmd_parameter[0] < numBatteries){
      assign_address(cmd_parameter[0]);
      delay(50);
      voltage_off();
      isBattCharging[cmd_parameter[0]] = false;
    }
    else{
      Serial.println("ERROR_99, NOT AN AVAILABLE BAY");
    }
  }
}


// assign_address will set the muxer for the desired charger
void assign_address(int n) {

  if (n == 0) {
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    lcd.setCursor (4,1);  // character zero, line 1
  }

  if (n == 1) {
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    lcd.setCursor (15,1);  // character 11, line 1
  }

  if (n == 2) {
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
    lcd.setCursor (4,2);  // character zero, line 2
  }

  if (n == 3) {
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
    lcd.setCursor (15,2);  // character 11, line 2
  }

  if (n == 4) {
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
    lcd.setCursor (4,3);  // character zero, line 3
  }

  if (n == 5) {
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
    lcd.setCursor (15,3);  // character 11, line 3
  }
}


void read_command () {
   if (Serial.available()) {
    memset(cmd_parameter, 10, sizeof(cmd_parameter)); // Reset the cmd_parameter array to be 0s
    size = Serial.readBytesUntil('$', input, INPUT_SIZE);
    // Add the final 0 to end the string
    input[size] = 0;
    parsedCommand = strtok(input, ",");
    cmd = *parsedCommand;
    parsedCommand = strtok(0, ",");
    cmdCount = 0;
    while (parsedCommand != 0 ) {
      cmd_parameter[cmdCount] = atol(parsedCommand);
      cmdCount++;
      parsedCommand = strtok(0, ",");
    }

    if (debugging) {
      for (int i = 0; i < NUM_INPUTS; i++) {
        Serial.print(i);
        Serial.print(": ");
        Serial.println(cmd_parameter[i]);
      }
    }

    // Use this switch to ensure we recognize the command.
    //  Otherwise, default to 0
    switch (cmd) {

      case 't': //testing
      case 'a': // Status
      case 'o': // Voltage on
      case 'f': // Voltage off
      case 'i': // Set all voltage/current limits
        if (debugging) {
          Serial.println(cmd);
        }
        break;
      default:
        // Unknown command.
        cmd = '0';
        break;
    }
  } else {
    cmd = '0';
  }
}


// Returns the String allStatus. Format:
// status,<numBatteries>,<BAY0 STATUS THRU BAY11 STATUS>,<BAY0 VOLTAGE THRU BAY11 VOLTAGE>
String getStatus () {
  allStatus = "status,6,";
  // Add the latest battery voltages to the status message
  for (int j=0; j<numBatteries; j++){
    allStatus += String(battVoltages[j]);
    if (j != numBatteries-1){
      allStatus += ",";
    }
  }
  allStatus += ",";

  // Add the latest battery currents to the status message
  for (int j=0; j<numBatteries; j++){
    allStatus += String(battCurrents[j]);
    if (j != numBatteries-1){
      allStatus += ",";
    }
  }
  return allStatus;
}


// voltage_off turns off the current charger
void voltage_off () {
  Serial1.println("awo0");
  delay(50);
  while (Serial1.available() > 0) {
    // read the incoming byte to clear the buffer
    Serial1.read();
  }
}


// voltage_on turns on the current charger
void voltage_on () {
  Serial1.println("awo1");
  delay(50);
  while (Serial1.available() > 0) {
    // read the incoming byte to clear the buffer
    Serial1.read();
  }
}


// all_off loops through each charger and turns it off
void all_off (){
  for (int n = 0; n < numBatteries; n++){
      assign_address(n);
      delay(50);
      voltage_off();
  }
}


// all_on loops through each charger and turns them on
void all_on (){
  for (int n = 0; n < numBatteries; n++){
      assign_address(n);
      delay(50);
      voltage_on();
      isBattCharging[n] = true;
  }
}


// resume_on will resume charging any batteries that should be charging but
// were turned off to read voltage
void resume_on (){
  for (int n = 0; n < numBatteries; n++){
    if (isBattCharging[n] == true && isBattFull[n] == false){
      assign_address(n);
      delay(50);
      voltage_on();
    }
  }
}


// Write the voltage and current settings to the currently assigned
// charger
void write_settings (){
  //noramly awu2520
  Serial1.println("awu1000");
  delay(50);
  Serial1.println("awi" + desiredCurrent);
  delay(50);
}


// set_all loops through each charger to set desired charging voltage
// and current
void set_all (){
  for (int n = 0; n < numBatteries; n++){
    assign_address(n);
    delay(50);
    write_settings();
  }
}


// This method will loop through each charger and set the voltage and
// current before turning it on.
void all_set_and_on () {
  for (int n = 0; n < numBatteries; n++){
    assign_address(n);
    delay(50);
    write_settings();
    isBattCharging[n] = true;
    if (isBattFull[n] == false) {
      voltage_on();
    }
  }
}


void read_voltage (int bay) {
  String voltage = "";
  int volt = 0;

  priorpriorbattVoltages[bay] = priorbattVoltages[bay];
  priorbattVoltages[bay] = battVoltages[bay];

  Serial1.println("");
  delay(50);
  Serial1.println("aru");
  delay(50);
  while (Serial1.available() > 0) {
    // read the incoming byte:
    v_reading = Serial1.read();
    voltage.concat(v_reading);
  }
  volt = voltage.substring(10,14).toInt();

  if (volt < 1500) {
    volt = 0;
    isBattFull[bay] = false;
    lcd.print("00.00");
  }
 

  // If the voltage is above 25.1 Volts, charger turns off for battery safety
  // 25.10 to account for difference in voltage between what the charger reads and what the actual voltage is

  // normaly set to else if (volt > 2510)
  else if (volt > 2510) {
    voltage_off();
    isBattFull[bay] = true;
    lcd.print(voltage.substring(10,12).toInt());
    lcd.print(".");
    if (voltage.substring(12,14).toInt() < 10) {
      lcd.print("0");
    }
    lcd.print(voltage.substring(12,14).toInt());
  }

  else if (isBattFull[bay] == true) {
    // If the bay isn't charging and the battery is tracked as full,
    // check the current voltage to decide if it's discharged enough
    // to warrant turning the charger back on.
    if (volt < 2500){
      isBattFull[bay] = false;
    }

    lcd.print(voltage.substring(10,12).toInt());
    lcd.print(".");
    if (voltage.substring(12,14).toInt() < 10) {
      lcd.print("0");
    }
    lcd.print(voltage.substring(12,14).toInt());
  }

  else {
    // If we're in this condition then we know the battery isn't full and that
    // we can just print the voltage to the LCD screen.
    isBattFull[bay] = false;

    // Print values left of decimal
    lcd.print(voltage.substring(10,12).toInt());
    lcd.print(".");

    // Zero padding if needed
    if (voltage.substring(12,14).toInt() < 10) {
      lcd.print("0");
    }

    // Print values to the right of decimal
    lcd.print(voltage.substring(12,14).toInt());
  }

  battVoltages[bay] = volt;
}


void read_current (int bay) {
  String current = "";
  int Amp = 0;

  priorpriorbattCurrents[bay] = priorbattCurrents[bay];
  priorbattCurrents[bay] = battCurrents[bay];

  delay(50);
  Serial1.println("ari");
  delay(50);
  while (Serial1.available() > 0) {
    // read the incoming byte:
    i_reading = Serial1.read();
    current.concat(i_reading);
  }

  Amp = current.substring(10,14).toInt();

  battCurrents[bay] = Amp;
}

int tesing (int bay) {
  String voltage = "";
  int volt = 0;

  priorpriorbattVoltages[bay] = priorbattVoltages[bay];
  priorbattVoltages[bay] = battVoltages[bay];

  Serial1.println("");
  delay(50);
  Serial1.println("aru");
  delay(50);
  while (Serial1.available() > 0) {
    // read the incoming byte:
    v_reading = Serial1.read();
    voltage.concat(v_reading);
  }
  volt = voltage.substring(10,14).toInt();


  
  voltage_off();
  isBattFull[bay] = true;

  if (volt == 0){
    lcd.print("Error");
  }
  else{
    lcd.print(voltage.substring(10,12).toInt());
    lcd.print(".");
    if (voltage.substring(12,14).toInt() < 10) {
       lcd.print("0");
    }
    lcd.print(voltage.substring(12,14).toInt());
  }

  //battVoltages[bay] = volt;
  isBattFull[bay] = false;
  //Serial.println(volt);
  return volt;
}
