#include <Arduino.h>

#define INPUT_SIZE 37  // Max number of bytes expected to come over in a command (1 char + 9 longs)
#define NUM_INPUTS 10  // Max number of different inputs we expect
#define INPUT_SIZE1 100  // Max number of bytes to come over from charger Rx

bool debugging = false;
String allStatus;
int cmdCount = 0;
char cmd = '0';
char* parsedCommand;
long cmd_parameter[NUM_INPUTS];
char input[INPUT_SIZE + 1];
char input1[INPUT_SIZE1 + 1];
byte size;  // Size of the incoming command as read from Serial

void read_command();
void Drok_off();

void setup() {

  Serial.begin(9600);   //start coms with control computer 
  Serial1.begin(9600);  //start coms with Drok_0 
  Serial2.begin(9600);  //start coms with Drok_1
  Serial3.begin(9600);  //start coms with Drok_2

  //Start with all Droks off 
  Serial1.println("awo0");
  delay(50);
  while (Serial1.available() > 0) {
    Serial1.read();
  }
  Serial2.println("awo0");
  delay(50);
  while (Serial2.available() > 0) {
    Serial2.read();
  }
  Serial3.println("awo0");
  delay(50);
  while (Serial3.available() > 0) {
    Serial3.read();
  }

}

void loop() {

  read_command();


  /* 
  cmd_parameter[subCommand]
  o,0 - Drok 0 OutPut on 
  o,1 - Drok 1 OutPut on 
  o,2 - Drok 2 OutPut on 
  o,3 - All Droks OutPut on 
  */
  if (cmd == 'o'){    //Turn on the output of droks 
    switch (cmd_parameter[0]){
    case 0:
        Serial1.println("awo1");
        delay(50);
        while (Serial1.available() > 0) {
          Serial1.read();
        }
      break;
    case 1:
        Serial2.println("awo1");
        delay(50);
        while (Serial2.available() > 0) {
          Serial2.read();
        }
    break;
    case 2:
        Serial3.println("awo1");
        delay(50);
        while (Serial3.available() > 0) {
          Serial3.read();
        }  
    break;
    case 3:
      Serial1.println("awo1");
      delay(50);
      while (Serial1.available() > 0) {
        Serial1.read();
      }
      Serial2.println("awo1");
      delay(50);
      while (Serial2.available() > 0) {
        Serial2.read();
      }
      Serial3.println("awo1");
      delay(50);
      while (Serial3.available() > 0) {
        Serial3.read();
      }
    break;

    default:
    // invalid 
    Serial.println("invalid argument");
      break;
    }
  }

  /* 
  cmd_parameter[subCommand]
  f,0 - Drok 0 OutPut off 
  f,1 - Drok 1 OutPut off 
  f,2 - Drok 2 OutPut off 
  f,3 - All Droks OutPut off 
  */
  if (cmd == 'f'){    //Turn off the output of droks 
    switch (cmd_parameter[0]){
    case 0:
        Serial1.println("awo0");
        delay(50);
        while (Serial1.available() > 0) {
          Serial1.read();
        }
      break;
    case 1:
        Serial2.println("awo0");
        delay(50);
        while (Serial2.available() > 0) {
          Serial2.read();
        }
    break;
    case 2:
        Serial3.println("awo0");
        delay(50);
        while (Serial3.available() > 0) {
          Serial3.read();
        }  
    break;
    case 3:
      Serial1.println("awo0");
      delay(50);
      while (Serial1.available() > 0) {
        Serial1.read();
      }
      Serial2.println("awo0");
      delay(50);
      while (Serial2.available() > 0) {
        Serial2.read();
      }
      Serial3.println("awo0");
      delay(50);
      while (Serial3.available() > 0) {
        Serial3.read();
      }
    break;

    default:
    // invalid 
    Serial.println("invalid argument");
      break;
    }
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
      Serial.println("Receive input control parameters");
      Serial.println("-------------------------------------");
      for (int i = 0; i < NUM_INPUTS; i++) {
        Serial.print(i);
        Serial.print(": ");
        Serial.println(cmd_parameter[i]);
      }
      Serial.println("-------------------------------------");
    }

    // Use this switch to ensure we recognize the command.
    //  Otherwise, default to 0
    switch (cmd) {

      case 'o': // OutPut On
      case 'f': // OutPut Off
      case 's': // Status 
      case 'v': // Set Voltage
      case 'c': // Set Current  
        if (debugging) {
          Serial.print("Receive control command: ");
          Serial.println(cmd);
          Serial.println("-------------------------------------");
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
