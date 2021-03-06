/*  ViReX-E Project Version Alpha 1
    Last Edited on February
     27, 2019 | 10:17 PM by JMBA of ViReX-E Development Group

    Log : 01/26/19 | 10:17 PM
          Start of the Codes.

    Log : 02/17/19 | 09:41 PM
          Updates: Plenty of functions made. Function regarding UDS control and the Foot Trackers is done.
                   Speed Control is also done

    Log : 02/22/19 | 12:56 AM
          Bluetooth Plugin update. Also Radio alternating Data sending is functional.

    Log : 02/27/19 | 08:36 AM
          Removed some impossible features:
              I2C for 4th Microcontroller with the sensor UDS for User Tracker
                    -> This feature causes hang on the Arduino Mega
              Hall Sensor for Arduino Mega
                    -> This needs an Interrupt


    Good Day! This is the Leader JMBA in charged of the Hardware and Software of this project.
    I am particularly involved in the designs of the whole thing, and also in charged of the
    Programming the Microcontrollers and basically in charged of making the whole thing work.

    And as you can see, this is the code for the mainboard, pertaining to the Main Microcontroller
    Arduino Mega 2560 ADK. All of the specific explanations will be in the Thesis Document. Check it
    out in our Hackaday Project Post, *Hackaday Website URL

    All of the codes in this section are made by me, with a few exceptions.

    The idea of this project came from jeanot1314's "BeYourHERO" https://github.com/jeanot1314/BeYourHero/
    And Matthew Hallberg's Unity with Arduino Tutorial Videos.

*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     INITIALIZATIONS    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// PIN DEFINITIONS:
// SENSORS
#define UDS_TRIGGER 2
#define UDS_ECHO 3
#define HALL_SENSOR A0
// Treadmill Board Control
#define CTRL_SPEED 4
#define CTRL_RELAY 5
// COMMUNICATION
#define NRF24L01_CE 48
#define NRF24L01_CSN 49
// Emergency Button
#define EMERGENCY_BTN 9

RF24 radio(NRF24L01_CE, NRF24L01_CSN);

// GLOBAL VARIABLES:
int HALL_SENSOR_VALUE;
int SPEED_READING = 0;
int SPEED_CONVERT = 0;
bool USER_DETECT = 0;

//Condition for opening the relay for transmission to happen
bool RLY_CONDITION = false;
bool RELAY_MODE = false;
bool SERIAL_CONDITION = false;
bool RESPONSE_CONDITION = false;

const uint64_t rAddress[] = {0x7878787878LL, 0xB3B4B5B6F1LL};

//Variable containers for Foot Tracker Coordinates
int ax1, gz1;
int ax2, gz2;

//Variables for the speed
int Speed_Value;


// Interchangeable values for treadmill Adjustment
int ACCEL_RANGE = 17000;// MAX is 32767
int ADJ_RANGE = 255; // MAX is 255
int SPEED_LIMIT = 120;
int BT_SPEED_RANGE = 180;



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MAIN CODE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void setup() {
  //Preliminary definitions for Serial connections
  Wire.begin(); // I2C definition for top pcb connections
  Serial.begin(115200); // This is for serial when connecting to a Computer
  Serial1.begin(115200); // This is for the bluetooth Communication

  Serial.println("~~~~~~~~~~~  ViReX-E Mainboard Serial Monitor  ~~~~~~~~~~~");
  Serial.println("Brought to you by ViReX-E Development Group");
  Serial.println("");

  // PINMODE INITIALIZATIONS
  //D2 to D5t
  for (int x = 2; x < 6; x++) {
    if (x != 3) pinMode(x, OUTPUT);
    if (x == 3) pinMode(x, INPUT);
  }
  //A0
  pinMode(HALL_SENSOR, INPUT);

  pinMode(EMERGENCY_BTN, INPUT_PULLUP);
  
  // Radio Definitions
  radio.begin();
  radio.setChannel(108);
  radio.setDataRate(RF24_250KBPS);//set data rate to 250kbps
  radio.enableAckPayload();
}

void loop() {
  EMERGENCY_BUTTON_PRESSED(); 
  RELAY_CONDITION_CHECK();
  RADIO_DATA_RECEIVE();
  
  Serial.println("              
  
 ");
  }

void EMERGENCY_BUTTON_PRESSED(){
  int State = digitalRead(EMERGENCY_BTN);
    //Serial.println(State);
  while(State == 1){
    //analogWrite(CTRL_SPEED, 0);
    
  }
}

void RELAY_CONDITION_CHECK() {
  if (RLY_CONDITION == 0 and RELAY_MODE == 0) {
    digitalWrite(CTRL_RELAY, LOW);
    RELAY_MODE = 1;
  }
  if (RLY_CONDITION == 1 and RELAY_MODE == 1) {
    SPEED_DETECT(0,"X");
    digitalWrite(CTRL_RELAY, HIGH);
    RELAY_MODE = 0;
  }
}

void RADIO_DATA_RECEIVE() {
  int Data[10];
  bool Trigger = 1;
  int ACK1 = 0;
  int ACK2 = 0;

  for (int x = 0; x < 2; x++) {
    radio.openWritingPipe(rAddress[x]);
    if (radio.write( &Trigger, sizeof(Trigger) )) {
      if (radio.isAckPayloadAvailable()) {
        radio.read( &Data, sizeof(Data) );
        if (Data[6] == 4214) {
          gz1 = abs(map(Data[4], -ACCEL_RANGE, ACCEL_RANGE, -ADJ_RANGE, ADJ_RANGE));
          Serial.print("GZ1: ");
          Serial.print(gz1); Serial.print("  ");
          SPEED_DETECT (gz1, 'A');
          ACK1 = Data[6];
          
        }

        if (Data[6] == 0510) {
          gz2 = abs(map(Data[4], -ACCEL_RANGE, ACCEL_RANGE, -ADJ_RANGE, ADJ_RANGE));
          Serial.print("GZ2: ");
          Serial.print(gz2); Serial.print("  ");
          SPEED_DETECT (gz2, 'B');
          ACK2 = Data[6];
          
        }

        if (ACK1 == 4214 and ACK2 == 0510) {
          RLY_CONDITION = 1;
          
        }    
      }
    }
  }
}

void SPEED_DETECT(int Speed, char Mode) {

  if (RLY_CONDITION) {

    Speed_Value = Speed;

    if (Speed >= SPEED_LIMIT) {
      Speed = SPEED_LIMIT;
    }
    if (Speed < SPEED_LIMIT) {
      Speed = Speed;
    }

        for(int i = Speed; i > 0; i--){
        int x = 5;
        
            while(i = Speed and x != 0){
              analogWrite(CTRL_SPEED, i);
              x--;
            }
          Speed = Speed - 7;
          if(Speed > 0)  analogWrite(CTRL_SPEED, abs(Speed));
                    
      }
    
    if(Mode != ""){
      Serial.print("\t"); 
      
      Serial.print("Mode: "); 
      Serial.print(Mode);
      Serial.print("\t"); 
      
      Serial.print("Speed: "); 
      Serial.print(Speed);
      Serial.print("\t"); 
      
      int Speed_Data = map(Speed_Value, -ADJ_RANGE, ADJ_RANGE, -BT_SPEED_RANGE, BT_SPEED_RANGE);

      Serial1.print(Mode);
      Serial1.println(abs(Speed));
    }
  }
}

void BT_SEND(){
  
}
