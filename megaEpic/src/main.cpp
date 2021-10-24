#include <Arduino.h>
#include "Wire.h"


#define NB_INPUT 8
#define NB_CHIP 24
#define nbDigInput NB_CHIP *NB_INPUT
#define nbAnaInput 16


//#define DEBUG
//#define DEBUG_EXECUTION_TIME


const int anaPin[nbAnaInput] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};

// port L pour les DATA
const int dataPin[NB_INPUT] = {49, 48, 47, 46, 45, 44, 43, 42};

// liste des chip select    attention 18/19 serial      20/21 I2C
const int chipsSelect[NB_CHIP] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 28, 29, 30,31, 22, 23, 24, 25};



struct STRUCT
{
  byte digInput[NB_CHIP];
  byte anaInput[nbAnaInput*2]; // int
} message;

byte temp1[NB_CHIP];
byte temp2[NB_CHIP];
byte temp3[NB_CHIP];


unsigned long startTime;

/**
 *
 *          analogReadInput
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

//TODO

//Faire un vote sur 3 lectures


/**
 *
 *          initchipselect
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
 **/
void digitalReadInput(byte *table)
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

    table[i] = not readPort();
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
  //Serial1.setTimeout(3000);
}




/**
 *
 *          loop
 *
 **/
void loop()
{
#ifdef DEBUG_EXECUTION_TIME
startTime=millis();
#endif

digitalReadInput(temp1);
digitalReadInput(temp2);
digitalReadInput(temp3);


for(int i=0;i<NB_CHIP;i++){

   byte tmp1= temp1[i]; //1010
    byte tmp2=temp2[i]; //1100
    byte tmp3=temp3[i]; //1001
    
    int nbT=0;
    int nbF=0;
    byte res=0;
    for(int j=0;j<8;j++){
        bool st1=bitRead(tmp1,j);
        if(st1){nbT++;}else{nbF++;}
        bool st2=bitRead(tmp2,j);
        if(st2){nbT++;}else{nbF++;}
        bool st3=bitRead(tmp3,j);
        if(st3){nbT++;}else{nbF++;}
        if(nbF<nbT){
          bitWrite(res,j,true);
        }else{
           bitWrite(res,j,false);
        }
    }
    message.digInput[i]=res;
/*
byte exc12=tmp1^tmp2;   //0110
byte exc23=tmp2^tmp3;   //0101
byte exc13=tmp1^tmp3;   //0011

byte exc1223=exc12^exc23; //0011

    if(tmp1==tmp2){
        res+=1;
    }
    if(tmp1==tmp3){
      res+=2;
    }
    if(tmp2==tmp3){
      res+=4;
    }
    todo 
    message.digInput[i];
    */


}

#ifdef DEBUG
  delay(1000);
#endif
#ifdef DEBUG_EXECUTION_TIME
  unsigned long deltaTime=millis()-startTime;
Serial.print("execution time: ");
Serial.println(deltaTime);
  //delay(1000);
#endif
}