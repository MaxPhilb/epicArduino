#include <Arduino.h>
#include "Wire.h"



#define NB_INPUT 8
#define NB_CHIP 24
#define nbDigInput NB_CHIP *NB_INPUT
#define nbAnaInput 16


//#define DEBUG   //permet d'afficher les traces
#define DEBUG_EXECUTION_TIME //permet d'afficher les temps d'execution


const int anaPin[nbAnaInput] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};

// port L pour les DATA
const int dataPin[NB_INPUT] = {49, 48, 47, 46, 45, 44, 43, 42};

// liste des chip select    attention 18/19 serial      20/21 I2C
const int chipsSelect[NB_CHIP] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 22, 23, 24, 25, 26, 27, 28,29};

uint8_t operation=0;

struct STRUCT
{
  byte digInput[NB_CHIP];
  byte anaInput[nbAnaInput*2]; // int
} message;

byte temp1[NB_CHIP];
byte temp2[NB_CHIP];
byte temp3[NB_CHIP];


unsigned long startTime;

bool reading=false;

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
    message.anaInput[index] = val; //decompose la lecture des ana en 2 byte
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

/*
*
*
*   Lecture du port L en entier
*
*/
byte readPort()
{
  return PINL;
}




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
  //delayMicroseconds(2);
  for (int i = 0; i < NB_CHIP; i++)
  {
    if (i > 0)
    {
      digitalWrite(chipsSelect[i - 1], HIGH);
    }
    digitalWrite(chipsSelect[i], LOW);
    delayMicroseconds(5);

    table[i] = ~readPort();
    //delayMicroseconds(5);
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



/*
*
*
*   ReceiveEvent
*
*   recoit un mot pour definir l'element que l'on souahaite lire avec la commande request voir fct requestEvent
*   si operation =1 => demande lecture entree num
*   si operation =2 => demande lecture entree analogique
*
*/
void receiveEvent(int size) {

  
   if(size==1){
     operation=Wire.read();
   }
   
}

/*
*
*
*   Lire les entrees num avec debounce
*   
*   3 lectures consecutives et vote
*
*/
void readDebounceInput(){
  

    digitalReadInput(temp1); //lecture 1
    digitalReadInput(temp2);  //lecture 2
    digitalReadInput(temp3); //lecture 3

    //reading=true;
  for(int i=0;i<NB_CHIP;i++){

        byte tmp1= temp1[i]; //1010
        byte tmp2=temp2[i]; //1100
        byte tmp3=temp3[i]; //1001
        
        int nbT=0;
        int nbF=0;
        byte res=0b00000000;

        for(int j=0;j<8;j++){
            nbF=0;
            nbT=0;
            bool st1=bitRead(tmp1,j);
            if(st1){nbT++;}else{nbF++;}

            bool st2=bitRead(tmp2,j);
            if(st2){nbT++;}else{nbF++;}

            bool st3=bitRead(tmp3,j);
            if(st3){nbT++;}else{nbF++;}


            /*
            Serial.print("nbT:");
            Serial.print(nbT);
            Serial.print(" nbF:");
            Serial.print(nbF);
            Serial.println();
            */
            if(nbF<nbT){
              bitWrite(res,j,true);
            }else{
              bitWrite(res,j,false);
            }
           
        }
        
      
        message.digInput[i]=res; //sauvegarde la valeur apres vote


  }

 
}

/*
*
*
* Recoit une demande de lecture
* on recupere le numéro "operation" recu juste avant
*
*
*/
void requestEvent(){

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
   
  Wire.begin(8);                    //declare le device en slave "I2C"
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //initialisation des I/O
  initchipselect();
  initInput();
  initMsg();
}




/**
 *
 *          loop
 *
 * 
 *      lecture en continue des I/O
 * 
 **/
void loop()
{

  #ifdef DEBUG_EXECUTION_TIME
  startTime=millis();
  #endif

   analogReadInput();
  //digitalReadInput(message.digInput);
  readDebounceInput();


   #ifdef DEBUG
    delay(1000);
  #endif
  #ifdef DEBUG_EXECUTION_TIME
    unsigned long deltaTime=millis()-startTime;
  Serial.print("execution time Debounce input: ");
  Serial.println(deltaTime);
    //delay(1000);
  #endif
}
