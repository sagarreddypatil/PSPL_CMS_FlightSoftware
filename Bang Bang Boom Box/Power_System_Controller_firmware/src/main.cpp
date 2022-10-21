#include <Arduino.h>

#define INPUT_SIZE 37  // Max number of bytes expected to come over in a command (1 char + 9 longs)
#define NUM_INPUTS 10  // Max number of different inputs we expect
#define INPUT_SIZE1 100  // Max number of bytes to come over from charger Rx
#define BUAD 4800

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
//void Drok_off();

void setup() {

  Serial.begin(9600);   //start coms with control computer 
  Serial1.begin(BUAD);  //start coms with Drok_0 
  Serial2.begin(BUAD);  //start coms with Drok_1
  Serial3.begin(BUAD);  //start coms with Drok_2

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

  //add some system safety checks in here before allowing power up of the system 

  Serial.println("System start");

}

void loop() {

  read_command();

  //unknown command handeling 
  //if (cmd == 0){Serial.println("unknonw command registerd");}

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
        Serial.println("Drok_0 on");
        Serial1.println("awo1");
        delay(50);
        while (Serial1.available() > 0) {
          Serial1.read();
        }
      break;
    case 1:
        Serial.println("Drok_1 on");
        Serial2.println("awo1");
        delay(50);
        while (Serial2.available() > 0) {
          Serial2.read();
        }
    break;
    case 2:
        Serial.println("Drok_2 on");
        Serial3.println("awo1");
        delay(50);
        while (Serial3.available() > 0) {
          Serial3.read();
        }  
    break;
    case 3:
      Serial.println("All Droks on");
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
        Serial.println("Drok_0 off");
        Serial1.println("awo0");
        delay(50);
        while (Serial1.available() > 0) {
          Serial1.read();
        }
      break;
    case 1:
        Serial.println("Drok_1 off");
        Serial2.println("awo0");
        delay(50);
        while (Serial2.available() > 0) {
          Serial2.read();
        }
    break;
    case 2:
        Serial.println("Drok_2 off");
        Serial3.println("awo0");
        delay(50);
        while (Serial3.available() > 0) {
          Serial3.read();
        }  
    break;
    case 3:
      Serial.println("All Droks off");
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

  /* 
  cmd_parameter[Drok #, set volatge]
  v,0,xxxx - Drok 0 voltage set to xxxx
  v,1,xxxx - Drok 1 voltage set to xxxx
  v,2,xxxx - Drok 2 voltage set to xxxx
  */
  if (cmd == 'v' && (cmd_parameter[1] < 3500) ){    //set output voltage
    switch (cmd_parameter[0]){
    case 0:
        Serial.println("Drok 0 Set to: " + String(cmd_parameter[1] / 100.0) + "V");
        Serial1.println("awu" + String(cmd_parameter[1]) );
      break;
    case 1:
        Serial.println("Drok 1 Set to: " + String(cmd_parameter[1] / 100.0) + "V");
        Serial2.println("awu" + String(cmd_parameter[1]) );
    break;
    case 2:
        Serial.println("Drok 2 Set to: " + String(cmd_parameter[1] / 100.0) + "V");
        Serial3.println("awu" + String(cmd_parameter[1]) );
    break;

    default:
    // invalid 
    Serial.println("invalid argument");
      break;
    }
  }

  /* 
  cmd_parameter[Drok #, set current]
  i,0,xxxx - Drok 0 current set to xxxx
  i,1,xxxx - Drok 1 current set to xxxx
  i,2,xxxx - Drok 2 current set to xxxx
  */
  if (cmd == 'i' && (cmd_parameter[1] < 1000) ){    //set output current
    switch (cmd_parameter[0]){
    case 0:
        Serial.println("Drok 0 Set to: " + String(cmd_parameter[1] / 100.0) + "A");
        Serial1.println("awi" + String(cmd_parameter[1]) );
      break;
    case 1:
        Serial.println("Drok 1 Set to: " + String(cmd_parameter[1] / 100.0) + "A");
        Serial2.println("awi" + String(cmd_parameter[1]) );
    break;
    case 2:
        Serial.println("Drok 2 Set to: " + String(cmd_parameter[1] / 100.0) + "A");
        Serial3.println("awi" + String(cmd_parameter[1]) );
    break;

    default:
    // invalid 
    Serial.println("invalid argument");
      break;
    }
  }

  /* 
  cmd_parameter[Drok #, read or voltage]
  s,0,0 - Drok 0 reading volatge 
  s,0,1 - Drok 0 reading current 
  s,1,0 - Drok 1 reading volatge 
  s,1,1 - Drok 1 reading current 
  s,2,0 - Drok 2 reading volatge
  s,2,1 - Drok 2 reading current  
  */
  if (cmd == 's'){    //Read the output currernt and voltage
  String reading_s = "";
  int reading_i;
  char reading_c; 

    switch (cmd_parameter[1])   //cuurent or volatge 
    {
    case 0:
      switch (cmd_parameter[0])  //for each drok 
      {
      case 0: 
        delay(50);
        Serial1.println("ari");
        delay(50);
        while (Serial1.available() > 0) {
          // read the incoming byte:
          reading_c = Serial1.read();
          reading_s.concat(reading_c);
        }
        reading_i = reading_s.substring(10,14).toInt();
        break;

        case 1: 
        delay(50);
        Serial2.println("ari");
        delay(50);
        while (Serial2.available() > 0) {
          // read the incoming byte:
          reading_c = Serial2.read();
          reading_s.concat(reading_c);
        }
        reading_i = reading_s.substring(10,14).toInt();
        break;

        case 2: 
        delay(50);
        Serial3.println("ari");
        delay(50);
        while (Serial3.available() > 0) {
          // read the incoming byte:
          reading_c = Serial3.read();
          reading_s.concat(reading_c);
        }
        reading_i = reading_s.substring(10,14).toInt();
        break;
      
      default:
        break;
      }
      Serial.println("Drok: " + String(cmd_parameter[0]) + " is outputing " + String(reading_i / 100.0) + "Amps");
    break;
    
    case 1:
      switch (cmd_parameter[0])  //for each drok 
      {
      case 0: 
        delay(50);
        Serial1.println("aru");
        delay(50);
        while (Serial1.available() > 0) {
          // read the incoming byte:
          reading_c = Serial1.read();
          reading_s.concat(reading_c);
        }
        reading_i = reading_s.substring(10,14).toInt();
        break;

        case 1: 
        delay(50);
        Serial2.println("aru");
        delay(50);
        while (Serial2.available() > 0) {
          // read the incoming byte:
          reading_c = Serial2.read();
          reading_s.concat(reading_c);
        }
        reading_i = reading_s.substring(10,14).toInt();
        break;

        case 2: 
        delay(50);
        Serial3.println("aru");
        delay(50);
        while (Serial3.available() > 0) {
          // read the incoming byte:
          reading_c = Serial3.read();
          reading_s.concat(reading_c);
        }
        reading_i = reading_s.substring(10,14).toInt();
        break;
      
      default:
        break;
      }
      Serial.println("Drok: " + String(cmd_parameter[0]) + " is outputing " + String(reading_i / 100.0) + "Volts");
    break;

    default:
    // invalid 
    Serial.println("invalid argument");
      break;
    }
  }

/* 
  note this is an expermental function 
  cmd_parameter[Drok #] 
  s,0 - Drok 0 power reading
  s,1 - Drok 1 power reading 
  s,2 - Drok 2 power reading
  */
  if (cmd == 'p'){    //Read the power output of the drok
  String reading_s = "";
  int reading_i;
  char reading_c; 

      switch (cmd_parameter[0])  //for each drok 
      {
      case 0: 
        delay(50);
        Serial1.println("ari");
        delay(50);
        while (Serial1.available() > 0) {
          // read the incoming byte:
          reading_c = Serial1.read();
          reading_s.concat(reading_c);
        }
        reading_i = reading_s.substring(10,14).toInt();
        break;

        case 1: 
        delay(50);
        Serial2.println("ari");
        delay(50);
        while (Serial2.available() > 0) {
          // read the incoming byte:
          reading_c = Serial2.read();
          reading_s.concat(reading_c);
        }
        reading_i = reading_s.substring(10,14).toInt();
        break;

        case 2: 
        delay(50);
        Serial3.println("ari");
        delay(50);
        while (Serial3.available() > 0) {
          // read the incoming byte:
          reading_c = Serial3.read();
          reading_s.concat(reading_c);
        }
        reading_i = reading_s.substring(10,14).toInt();
        break;
      
      default:
        break;
      }
      Serial.println("Drok: " + String(cmd_parameter[0]) + " is outputing" + String(reading_i / 100.0) + "");
    
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
      case 'v': // Set Voltage
      case 'i': // Set Current
      case 's': // Status
      case 'p': // Power output 
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
