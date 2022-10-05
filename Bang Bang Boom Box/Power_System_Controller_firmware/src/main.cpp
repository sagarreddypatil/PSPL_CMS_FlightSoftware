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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);   //start coms with control computer 
  Serial1.begin(9600);  //start coms with Drok_1 
  Serial2.begin(9600);  //start coms with Drok_2
  Serial3.begin(9600);  //start coms with Drok_3
}

void loop() {
  // put your main code here, to run repeatedly:
  read_command();

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
