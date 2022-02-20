#include <Arduino.h>

#define NB_OUTPUT 8
#define NB_CHIP 24
#define nbDigInput NB_CHIP *NB_OUTPUT

// port L pour les DATA
const int dataPin[NB_OUTPUT] = {49, 48, 47, 46, 45, 44, 43, 42};

// liste des chip select    attention 18/19 serial      20/21 I2C
const int chipsSelect[NB_CHIP] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 22, 23, 24, 25, 26, 27, 28,29};




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
    pinMode(chipsSelect[i], INPUT_PULLUP);
  }
 
}

/**
 *
 *          initOutput
 *
 *
 **/
void initOutput()
{
  // DDRL = 0;
  for (int i = 0; i < NB_OUTPUT; i++)
  {
    pinMode(dataPin[i], OUTPUT);
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  
  //initialisation des I/O
  initchipselect();
  initOutput();

}

int8_t lastActivated=-1;
void loop() {
  // put your main code here, to run repeatedly:
  uint8_t isActivated=45;
  for(int i=0;i<NB_CHIP;i++){
     if(!digitalRead(chipsSelect[i])){
        isActivated=i;
     }
  }
  
  if(isActivated<NB_CHIP && isActivated!=lastActivated){
    Serial.print(" isactivated "+String(isActivated)+"\n");
    lastActivated=isActivated;
    for(int i=0;i<8;i++){
      bool bittemp=bitRead(isActivated,i);
      //bool bittemp=HIGH;
      Serial.print(bittemp);
      digitalWrite(dataPin[i],bittemp);
    }
    Serial.println();
  }
  




 
}