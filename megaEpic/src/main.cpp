#include <Arduino.h>
#include "Wire.h"
//#include "SerialTransfer.h"
//SerialTransfer myTransfer;

//#include "I2CTransfer.h"
//I2CTransfer myTransfer;

#define NB_INPUT 8
#define NB_CHIP 24
#define nbDigInput NB_CHIP *NB_INPUT
#define nbAnaInput 16


//#define DEBUG
//#define DEBUG_EXECUTION_TIME

const byte entete = 45;

bool lastValue = false;

const int anaPin[nbAnaInput] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};

// port L pour les DATA
const int dataPin[NB_INPUT] = {49, 48, 47, 46, 45, 44, 43, 42};

// liste des chip select    attention 18/19 serial      20/21 I2C
const int chipsSelect[NB_CHIP] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 28, 29, 30,31, 22, 23, 24, 25};

const int timeoutTransfer=20;

#define synchroPinFromLeo 26
#define synchroPinToLeo 27



struct STRUCT
{
  byte digInput[NB_CHIP];
  byte anaInput[nbAnaInput*2]; // int
} message;

/**
 *
 *          analogReadInput
 *
 *  
 *
 *
 **/

void analogReadInput()
{
#ifdef DEBUG

  Serial.println("analogRead");

#endif

#ifdef DEBUG_EXECUTION_TIME

  Serial.print("\tRead analog Input start at ");
  Serial.println(millis());

#endif
int index=0;
  for (int i = 0; i < nbAnaInput; i++)
  {
    int val=analogRead(anaPin[i]);
    message.anaInput[index] = val;
    index++;
     message.anaInput[index] = (val>>8);
     index++;
  
#ifdef DEBUG

    Serial.print(val);
    Serial.print(" ");

#endif
  }
#ifdef DEBUG_EXECUTION_TIME

  Serial.print("\tRead analog Input end at ");
  Serial.println(millis());
  Serial.println();
#endif
#ifdef DEBUG

  Serial.println();

#endif
}


/**
 *
 *          resetchipselect
 *
 *  
 *
 *
 **/

void resetchipselect()
{
  for (int i = 0; i < NB_CHIP; i++)
  {
    digitalWrite(chipsSelect[i], HIGH);
  }
}
byte readPort()
{
  return PINL;
}


/**
 *
 *          initchipselect

 *
 *  
 *
 *
 **/
void initchipselect()
{
  for (int i = 0; i < NB_CHIP; i++)
  {
    pinMode(chipsSelect[i], OUTPUT);
  }
  resetchipselect();
}

/**
 *
 *          initInput

 *
 *  
 *
 *
 **/
void initInput()
{
  // DDRL = 0;
  for (int i = 0; i < NB_INPUT; i++)
  {
    pinMode(dataPin[i], INPUT_PULLUP);
  }
}

/**
 *
 *          digitalReadInput

 *
 *  
 *
 *
 **/
void digitalReadInput()
{
#ifdef DEBUG_EXECUTION_TIME

  Serial.print("\tRead digital Input start at ");
  Serial.println(millis());
#endif
  resetchipselect();
  delayMicroseconds(2);
  for (int i = 0; i < NB_CHIP; i++)
  {
    if (i > 0)
    {
      digitalWrite(chipsSelect[i - 1], HIGH);
    }
    digitalWrite(chipsSelect[i], LOW);
    delayMicroseconds(5);

    message.digInput[i] = readPort();
    delayMicroseconds(5);
#ifdef DEBUG
    Serial.print(" ");
    Serial.print(message.digInput[i], BIN);
#endif
  }
#ifdef DEBUG
  Serial.println();
  // delay(1000);
#endif

  digitalWrite(chipsSelect[NB_CHIP - 1], LOW);

#ifdef DEBUG_EXECUTION_TIME

  Serial.print("\tRead digital Input end at ");
  Serial.println(millis());
  Serial.println();
#endif
}

/**
 *
 *          initMsg

 *
 *  
 *
 *
 **/
void initMsg()
{
  int i = 0;

  for (i = 0; i < NB_CHIP; i++)
  {
    message.digInput[i] = 0;
  }

  for (i = 0; i < nbAnaInput*2; i++)
  {
    message.anaInput[i] = -1;
  }
}

uint8_t operation=0;

// function that executes whenever data is received from master
void receiveEvent(int size) {

  Serial.print("expected size ");
   Serial.println(size);
   if(size==1){
     operation=Wire.read();
     Serial.println("operation "+String(operation));
   }
   
}

void requestEvent(){

  digitalReadInput();
  analogReadInput();

 
 if(operation==1){
   Wire.write(message.digInput,NB_CHIP );
   operation=0;
 }

 if(operation==2){
   Wire.write(message.anaInput,(nbAnaInput*2) );
   operation=0;
 }

   
}

/**
 *
 *          setup

 *
 *  
 *
 *
 **/
void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  //myTransfer.begin(Serial1);

   
   Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //myTransfer.begin(Wire);

  initchipselect();
  initInput();
  initMsg();
  pinMode(synchroPinFromLeo,INPUT_PULLUP);
  pinMode(synchroPinToLeo,OUTPUT);
  digitalWrite(synchroPinToLeo,HIGH);
  //Serial1.setTimeout(3000);
}




/**
 *
 *          loop

 *
 *  
 *
 *
 **/
void loop()
{
#ifdef DEBUG_EXECUTION_TIME

  Serial.print("loop start at ");
  Serial.println(millis());

#endif

 

 
 

#ifdef DEBUG
  delay(1000);
#endif
#ifdef DEBUG_EXECUTION_TIME
  Serial.print("loop end at ");
  Serial.println(millis());
  Serial.println();
  //delay(1000);
#endif
}