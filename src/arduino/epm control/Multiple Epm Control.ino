#include <Adafruit_MPRLS.h>

// EPM Control Board Code 

////////////////// INSTRUCTIONS FOR USE ////////////////////////////

// Upload code to Arduino Nano Every
// Each EPM can be controlled using the following convention: EPM Group (Number on Central Board) / EPM Designation (Number on Peripheral Board) / Desired Direction / Pulse Duration 

// Each Serial Monitor command must be 6 characters long
// First character is a number 1-7 to designate the EPM Group you want to enable (corresponds to a number on the central board) 
// Second character is a number 1-5 to designate the specific EPM you want to enable (corresponds to a number on the peripheral board) 
// Third character is A (for magnetization) or B (for demagnetization) 
// Last three characters are pulse duration (maximum is 999 microseconds, minimum is 005 microseconds) 

// Example 1: I want to magnetize EPM 5 in Group 7 using a 300 microsecond pulse, so I would use command 75A300
// Example 2: I want to demagnetize EPM 4 in Group 3 using a 20 microsecond pulse, so I would use command 34B020

///////////////////////////////////////////////////////////////////

// Old Pin Assignments

//int EN_G1 = 21;
//int EN_G2 = 2;
//int EN_G3 = 16;
//int EN_G4 = 17;
//int EN_G5 = 18;
//int EN_G6 = 19;
//int EN_G7 = 20;

//int IN_1A = 11; 
//int IN_1B = 12;
//int IN_2A = 9; 
//int IN_2B = 10;
//int IN_3A = 7;
//int IN_3B = 8;
//int IN_4A = 5;
//int IN_4B = 6;
//int IN_5A = 3;
//int IN_5B = 4;

// New Pin Assignments

int EN_G1 = 16;
int EN_G2 = 17;
int EN_G3 = 18;
int EN_G4 = 19;
int EN_G5 = 20;
int EN_G6 = 21;
int EN_G7 = 12;

int IN_1A = 10;
int IN_1B = 11;
int IN_2A = 8;
int IN_2B = 9;
int IN_3A = 6;
int IN_3B = 7;
int IN_4A = 4;
int IN_4B = 5;
int IN_5A = 2;
int IN_5B = 3;

// Rise time compensation offset

int riseTime_compensation = 3; 

void setup() {

  // Initialize serial monitor 
  
  Serial.begin(9600);

  // Set pins as outputs

  pinMode(IN_1A, OUTPUT); 
  pinMode(IN_1B, OUTPUT); 
  pinMode(IN_2A, OUTPUT); 
  pinMode(IN_2B, OUTPUT); 
  pinMode(IN_3A, OUTPUT); 
  pinMode(IN_3B, OUTPUT); 
  pinMode(IN_4A, OUTPUT); 
  pinMode(IN_4B, OUTPUT);
  pinMode(IN_5A, OUTPUT); 
  pinMode(IN_5B, OUTPUT); 

  pinMode(EN_G1, OUTPUT); 
  pinMode(EN_G2, OUTPUT); 
  pinMode(EN_G3, OUTPUT); 
  pinMode(EN_G4, OUTPUT); 
  pinMode(EN_G5, OUTPUT); 
  pinMode(EN_G6, OUTPUT); 
  pinMode(EN_G7, OUTPUT); 

  digitalWrite(IN_1A, LOW); 
  digitalWrite(IN_1B, LOW); 
  digitalWrite(IN_2A, LOW); 
  digitalWrite(IN_2B, LOW); 
  digitalWrite(IN_3A, LOW); 
  digitalWrite(IN_3B, LOW); 
  digitalWrite(IN_4A, LOW); 
  digitalWrite(IN_4B, LOW);
  digitalWrite(IN_5A, LOW); 
  digitalWrite(IN_5B, LOW); 
}

void loop() {
  // User enters EPM Group, Order, Polarity, and Pulse Duration
  
  String command = Serial.readString();

  // For multiple EPM inputs, make sure to write the 6-char commands separated by commas - NO SPACES
  // Do not include a comma in the last input or it will not pass
  // Ex: 11A500,12A500,13A500

  actuate(command);

  if (command == "R") { // pressure blocking test
    for (int j = 0; j < 1; j++) {
      actuate("15B400"); //turn EPM on
      delay(30*1000); //microseconds (30 seconds delay)
      actuate("15A400"); //turn EPM off
      //delay(10*1000);
    };
  }

  if (command == "T") { // test code to see if the EPMs turn on/off simultaneously
    for (int j = 0; j < 1; j++) {
      //testing one by one actuation - is it simultaneous?
      actuate("23A400");
      actuate("24A400");
      actuate("25A400");
      delay(10*1000);
      actuate("23B400");
      actuate("24B400");
      actuate("25B400");
    };
  }

  if (command == "I") { // initialization - turn all EPMs on
    for (int j = 0; j < 1; j++) {
      actuate("23A750"); //turn EPM 1 on
      actuate("24A750"); //turn EPM 2 on
      actuate("25A750"); //turn EPM 3 on
      actuate("13A750"); //turn EPM 4 on (pressure)
      actuate("14A750"); //turn EPM 5 on (vacuum)
      actuate("15A750");
    };
  }

  if (command == "O") { // opposite - turn all EPMs off
    for (int j = 0; j < 1; j++) {
      actuate("23B750"); //turn EPM 1 off
      actuate("24B750"); //turn EPM 2 off
      actuate("25B750"); //turn EPM 3 off
      actuate("13B750"); //turn EPM 4 off (pressure)
      actuate("14B750"); //turn EPM 5 off (vacuum)
      actuate("15B750");
    };
  }

 if (command == "V") { // vacuum?
    for (int j = 0; j < 1; j++) {
      actuate("23B900"); //turn EPM 1 off
      actuate("24B900"); //turn EPM 2 off
      actuate("25B900"); //turn EPM 3 off
      actuate("15B900"); //turn EPM 4 off (pressure)
      delay(5*1000);
      actuate("15A900"); //turn EPM 4 on (pressure)
      actuate("14B900"); //turn EPM 5 off 
    };
  }

  if (command == "G") { // grab!
    for (int j = 0; j < 1; j++) {
      //all inflate
      actuate("15B900"); //turn EPM 4 off (pressure)
      actuate("23B900"); //turn EPM 1 off
      //delay(2*1000);
      actuate("24B900"); //turn EPM 2 off
      actuate("25B900"); //turn EPM 3 off
      delay(3*1000);
      //col 3 stops inflating
      actuate("25A900");
      delay(5*1000);
      //all stop inflating
      actuate("23A900");
      actuate("24A900");
      actuate("15A900");
      

    };
  }

  if (command == "J") {
    for (int j = 0; j < 1; j++) {
      // LOOP 1 //
      //all inflate - jump! (or stand? quickly?)
      actuate("15B900");//turn EPM 4 off (pressure)
      actuate("23B900"); //turn EPM 1 off
      actuate("24B900"); //turn EPM 2 off
      actuate("25B900"); //turn EPM 3 off
      delay(0.5*1000);
      //all deflate
      actuate("15A900");
      actuate("14B900");
      delay(10*1000);
      actuate("14A900");

      // LOOP 2 //
      //all inflate - jump! (or stand? quickly?)
      actuate("15B900");//turn EPM 4 off (pressure)
      actuate("23B900"); //turn EPM 1 off
      actuate("24B900"); //turn EPM 2 off
      actuate("25B900"); //turn EPM 3 off
      delay(0.5*1000);
      //all deflate
      actuate("15A900");
      actuate("14B900");
      delay(15*1000);
      actuate("14A900");
     
     // LOOP 3 //
      //all inflate - jump! (or stand? quickly?)
      actuate("15B900");//turn EPM 4 off (pressure)
      actuate("23B900"); //turn EPM 1 off
      actuate("24B900"); //turn EPM 2 off
      actuate("25B900"); //turn EPM 3 off
      delay(0.5*1000);
      //all deflate
      actuate("15A900");
      actuate("14B900");
      delay(15*1000);
      actuate("14A900");

    };
  }

  if (command == "L") { // locamote!
    for (int j = 0; j < 1; j++) {
      // //LEGS FORWARD//
      // //all inflate
      // actuate("15B900"); //turn EPM 4 off (pressure)
      // actuate("23B900"); //turn EPM 1 off
      // actuate("24B900"); //turn EPM 2 off
      // actuate("25B900"); //turn EPM 3 off
      // delay(6*1000);
      // //col 3 stops inflating
      // actuate("25A900");
      // delay(10*1000);
      // //all stop inflating
      // actuate("15A900");
      // //vacuum
      // actuate("25B900");
      // actuate("14B900");
      // delay(5*1000);
      //LEGS BACKWARD//

      // LOOP 1 //
      //all inflate
      actuate("15B900");//turn EPM 4 off (pressure)
      actuate("23B900"); //turn EPM 1 off
      actuate("24B900"); //turn EPM 2 off
      actuate("25B900"); //turn EPM 3 off
      delay(2*1000);
      //col 1 (? or 2?) stops inflating
      actuate("14B900");
      actuate("24A900"); //might have to change this
      delay(2*1000);
      //all stop inflating
      actuate("15A900");
      //vacuum
      actuate("24B900"); //might have to change this
      actuate("14B900");
      delay(20*1000); //longer because the loop starts again after this

      actuate("25A900");
      actuate("23A900");
      actuate("24A900");
      actuate("14A900");
      actuate("15B900");

      Serial.println("Loop done");
      //inflate 1
      actuate("23B900");
      delay(4*1000);
      // release 1
      actuate("23A900");
      actuate("14B900");
      delay(3*1000);

      Serial.println("Loop done");
      //inflate 1
      actuate("14A900");
      actuate("23B900");
      delay(4*1000);
      // release 1
      actuate("23A900");
      actuate("14B900");
      delay(3*1000);

      Serial.println("Loop done");
      //inflate 1
      actuate("14A900");
      actuate("23B900");
      delay(4*1000);
      // release 1
      actuate("23A900");
      actuate("14B900");
      delay(3*1000);

      Serial.println("Loop done");
      //inflate 1
      actuate("14A900");
      actuate("23B900");
      delay(4*1000);
      // release 1
      actuate("23A900");
      actuate("15A900");
      delay(3*1000);
      Serial.println("complete");
    };
  }


   if (command == "X") { // Inflation only demo - one col at a time @ 50 kPa
    for (int j = 0; j < 1; j++) {
      //column 1 inflates
      actuate("23B300"); //EPM 1 off
      actuate("13B300"); //EPM 4 off (pressure on)
      delay(3*1000);
      actuate("23A300"); //EPM 1 on
      //column 2 inflates 
      actuate("24B300"); //EPM 2 off
      delay(3*1000);
      actuate("24A300"); //EPM 2 on
      //column 3 inflates
      actuate("25B300"); //EPM 2 off
      delay(3*1000);
      actuate("25A300"); //EPM 2 on
      //turn off pressure
      actuate("13A300");
    };
  }

  if (command == "Y") { // Inflation only demo - cols inflate at different times @ 50 kPa
    for (int j = 0; j < 1; j++) {
      //All EPMs on valve turn off
      actuate("23B300"); //EPM 1 off
      actuate("24B300"); //EPM 2 off
      actuate("25B300"); //EPM 3 off
      actuate("13B300"); //EPM 4 off (pressure on)
      delay(3*1000);
      //stop EPM 3
      actuate("25A300");
      delay(3*1000);
      //stop EPM 2
      actuate("24A300");
      delay(3*1000);
      //stop EPM 1
      actuate("23A300");
      //turn off pressure
      actuate("13A300");
    };
  }
  
  if (command == "a") { // old inflation demo A - timing test
    for (int j = 0; j < 1; j++) {
      //only column 1
      actuate("23B900"); //EPM 1 off
      actuate("15B900"); //EPM 4 off (pressure on)
      delay(2*1000); //inflation
      actuate("15A900"); //EPM 4 on (pressure off)
      delay(2*1000);
      actuate("14B900"); //EPM 5 off (vacuum on)
      delay(5*1000); //deflation
      actuate("14A900"); //EPM 5 on (vacuum off)
      actuate("23A900"); //EPM 1 on
      //only column 2
      actuate("24B900"); //EPM 2 off
      actuate("15B900"); //EPM 4 off (pressure on)
      delay(2*1000); //inflation
      actuate("15A900"); //EPM 4 on (pressure off)
      delay(2*1000);
      actuate("14B900"); //EPM 5 off (vacuum on)
      delay(5*1000); //deflation
      actuate("14A900"); //EPM 5 on (vacuum off)
      actuate("24A900"); //EPM 2 on
      //only column 3
      actuate("25B900"); //EPM 3 off
      actuate("15B900"); //EPM 4 off (pressure on)
      delay(2*1000); //inflation
      actuate("15A900"); //EPM 4 on (pressure off)
      delay(2*1000);
      actuate("14B900"); //EPM 5 off (vacuum on)
      delay(5*1000); //deflation
      actuate("14A900"); //EPM 5 on (vacuum off)
      actuate("25A900"); //EPM 3 on
      //columns 1 & 2
      actuate("23B900"); //EPM 1 off
      actuate("24B900"); //EPM 2 off
      actuate("15B900"); //EPM 4 off (pressure on)
      delay(2*1000); //inflation
      actuate("15A900"); //EPM 4 on (pressure off)
      delay(2*1000);
      actuate("14B900"); //EPM 5 off (vacuum on)
      delay(5*1000); //deflation
      actuate("14A900"); //EPM 5 on (vacuum off)
      actuate("23A900"); //EPM 1 on
      actuate("24A900"); //EPM 2 on
      //columns 2 & 3
      actuate("24B900"); //EPM 2 off
      actuate("25B900"); //EPM 3 off
      actuate("15B900"); //EPM 4 off (pressure on)
      delay(2*1000); //inflation
      actuate("15A900"); //EPM 4 on (pressure off)
      delay(2*1000);
      actuate("14B900"); //EPM 5 off (vacuum on)
      delay(5*1000); //deflation
      actuate("14A900"); //EPM 5 on (vacuum off)
      actuate("24A900"); //EPM 2 on
      actuate("25A900"); //EPM 3 on
      //columns 3 & 1
      actuate("25B900"); //EPM 3 off
      actuate("23B900"); //EPM 1 off
      actuate("15B900"); //EPM 4 off (pressure on)
      delay(2*1000); //inflation
      actuate("15A900"); //EPM 4 on (pressure off)
      delay(2*1000);
      actuate("14B900"); //EPM 5 off (vacuum on)
      delay(5*1000); //deflation
      actuate("14A900"); //EPM 5 on (vacuum off)
      actuate("25A900"); //EPM 3 on
      actuate("23A900"); //EPM 1 on
      //all
      actuate("23B900"); //EPM 1 off
      actuate("24B900"); //EPM 2 off
      actuate("25B900"); //EPM 3 off
      actuate("15B900"); //EPM 4 off (pressure on)
      delay(2*1000); //inflation
      actuate("15A900"); //EPM 4 on (pressure off)
      delay(2*1000);
      actuate("14B900"); //EPM 5 off (vacuum on)
      delay(5*1000); //deflation
      actuate("14A900"); //EPM 5 on (vacuum off)
      actuate("23A900"); //EPM 1 on
      actuate("24A900"); //EPM 2 on
      actuate("24A900"); //EPM 3 off
    };
  }

  if (command == "b") { // old inflation demo B - timing test
    for (int j = 0; j < 1; j++) {
      //column 1 inflates
      actuate("23B300"); //EPM 1 off
      actuate("13B300"); //EPM 4 off (pressure on)
      delay(1*1000);
      actuate("23A300"); //EPM 1 on
      //column 2 inflates 
      actuate("24B300"); //EPM 2 off
      delay(1*1000);
      actuate("24A300"); //EPM 2 on
    };
  }

  if (command == "S") {
    for (int i = 0; i < 120; i++){
    actuate("23A900"); 
    delay(10*1000);
    actuate("23B900");
    delay(10*1000);
    };
  }

  if (command == "T") {
    actuate("15B900"); //turn on pressure
    actuate("24B900"); // open channel 1
    delay(4*1000); 
    actuate("24A900"); // close channel 1
    
    actuate("23B900"); 
    delay(4*1000);
    actuate("23A900");

    actuate("25B900");
    delay(4*1000);
    actuate("25A900");

    actuate("15A900"); // turn off pressure
    actuate("14B900"); // turn on vacuum

    actuate("24B900"); 
    delay(20*1000);
    actuate("23B900");
    delay(20*1000);
    actuate("25B900");
    delay(20*1000);
  }

  if (command == "W") { 
          //all inflate
      actuate("15B900");//turn EPM 4 off (pressure)
      actuate("23B900"); //turn EPM 1 off
      actuate("24B900"); //turn EPM 2 off
      actuate("25B900"); //turn EPM 3 off
      delay(1*1000);
      // col 2 stop inflating
      actuate("24A900"); //might have to change this
      delay(2*1000);
      Serial.println("Done inflation");
      //all stop inflating & hold
      actuate("15A900");
      actuate("23A900"); //might have to change this
      actuate("25A900");
      delay(1*1000); //longer because the loop starts again after this
      //exhaust - drop item
      actuate("14B900");
      actuate("23B900");
      actuate("24B900");
      actuate("25B900");
  }

 
  
  int command_length = command.length();
  if (command_length > 6 && command_length % 7 == 6) {
    int num_command_inputs = round(command_length / 7) + 1;
    for (int i = 0; i < num_command_inputs; i++) {
      int start_index = 7*i;
      int end_index = 7*(i+1) - 1;
      actuate(command.substring(start_index,end_index));
    }
  } else if (command.length() == 6) {
    actuate(command);
  }
}

void actuate(String input) {
  // Parse through serial command 
    
  int EPM_Group = input[0] - '0'; 
  int EPM_Designation = input[1] - '0'; 
  
  char polarity = input[2];
  
  int duration_hundreds = input[3] - '0';
  int duration_tens = input[4] - '0';
  int duration_ones = input[5] - '0';
  int duration = duration_hundreds * 100 + duration_tens * 10 + duration_ones;  

  if (duration > 0){ 

    // Output user's selection

    Serial.print("Group ");
    Serial.print(EPM_Group); 
    Serial.print(" selected");
      
    Serial.println();
  
    Serial.print("Designation ");
    Serial.print(EPM_Designation); 
    Serial.print(" selected");
  
    Serial.println();
  
    Serial.print("Direction ");
    Serial.print(polarity); 
    Serial.print(" selected");
  
    Serial.println();
  
    Serial.print("Duration = ");
    Serial.print(duration); 
    Serial.print(" microseconds");
  
    Serial.println();
    Serial.println();

    // Activate enable pin of a desired EPM group
  
    switch (EPM_Group) {
      case 1:
        digitalWrite(EN_G1, HIGH); 
        break;
      case 2:
        digitalWrite(EN_G2, HIGH); 
        break;
      case 3:
        digitalWrite(EN_G3, HIGH); 
        break;
      case 4:
        digitalWrite(EN_G4, HIGH); 
        break;
      case 5:
        digitalWrite(EN_G5, HIGH); 
        break;
      case 6:
        digitalWrite(EN_G6, HIGH); 
        break;
      case 7:
        digitalWrite(EN_G7, HIGH); 
        break;
    }

    // Send signal to desired EPM (order) in desired direction
  
    switch (EPM_Designation) {
      case 1:
        if (polarity == 'A') {
          digitalWrite(IN_1A, HIGH);
          delayMicroseconds(duration - riseTime_compensation);
          digitalWrite(IN_1A, LOW);
        }
        else if (polarity == 'B') {
          digitalWrite(IN_1B, HIGH);
          delayMicroseconds(duration - riseTime_compensation);
          digitalWrite(IN_1B, LOW);
        }
        break;
      case 2:
        if (polarity == 'A') {
          digitalWrite(IN_2A, HIGH);
          delayMicroseconds(duration - riseTime_compensation);
          digitalWrite(IN_2A, LOW);
        }
        else if (polarity == 'B') {
          digitalWrite(IN_2B, HIGH);
          delayMicroseconds(duration - riseTime_compensation);
          digitalWrite(IN_2B, LOW);
        }
      case 3:
        if (polarity == 'A') {
          digitalWrite(IN_3A, HIGH);
          delayMicroseconds(duration - riseTime_compensation);
          digitalWrite(IN_3A, LOW);
        }
        else if (polarity == 'B') {
          digitalWrite(IN_3B, HIGH);
          delayMicroseconds(duration - riseTime_compensation);
          digitalWrite(IN_3B, LOW);
        }
        break;
      case 4:
        if (polarity == 'A') {
          digitalWrite(IN_4A, HIGH);
          delayMicroseconds(duration - riseTime_compensation);
          digitalWrite(IN_4A, LOW);
        }
        else if (polarity == 'B') {
          digitalWrite(IN_4B, HIGH);
          delayMicroseconds(duration - riseTime_compensation);
          digitalWrite(IN_4B, LOW);
        }
        break;
      case 5:
        if (polarity == 'A') {
          digitalWrite(IN_5A, HIGH);
          delayMicroseconds(duration - riseTime_compensation);
          digitalWrite(IN_5A, LOW); 
        }
        else if (polarity == 'B') {
          digitalWrite(IN_5B, HIGH);
          delayMicroseconds(duration - riseTime_compensation);
          digitalWrite(IN_5B, LOW);
        } 
        break;
    } 

    // This delay is necessary to prevent a parasitic signal artifact
    
    delay(100); 

    // De-activate enable pin of desired EPM group
   
    digitalWrite(EN_G1, LOW); 
    digitalWrite(EN_G2, LOW); 
    digitalWrite(EN_G3, LOW); 
    digitalWrite(EN_G4, LOW); 
    digitalWrite(EN_G5, LOW); 
    digitalWrite(EN_G6, LOW); 
    digitalWrite(EN_G7, LOW);   
  } 
}