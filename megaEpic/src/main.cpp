#include <Arduino.h>
#include "Wire.h"
#include "SerialTransfer.h"

#define NB_CHIP 24
#define nbDigInput 192
#define nbAnaInput 16

SerialTransfer myTransfer;

const int anaPin[nbAnaInput] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};

//port L pour les DATA
const int dataPin[8] = {49, 48, 47, 46, 45, 44, 43, 42};

//liste des chip select
const int chipsSelect[NB_CHIP] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};

struct STRUCT
{
  bool digInput[nbDigInput]; //bool
  int anaInput[nbAnaInput];  //int
} message;

void analogReadInput()
{
  Serial.println("analogRead");
  for (int i = 0; i < nbAnaInput; i++)
  {
    message.anaInput[i] = analogRead(anaPin[i]);
    //message.anaInput[i]=random(nbAnaInput);
    Serial.print(message.anaInput[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void digitalReadInput()
{
  for (int i = 0; i <= nbDigInput; i++)
  {
  }
}

//chipselect->bas

void initMsg()
{
  int i = 0;

  for (i = 0; i < nbDigInput; i++)
  {
    message.digInput[i] = false;
  }

  for (i = 0; i < nbAnaInput; i++)
  {
    message.anaInput[i] = -1;
  }
}

//reset chip select
void resetchipselect()
{
  for (int i = 0; i < NB_CHIP; i++)
  {
    digitalWrite(chipsSelect[i], HIGH);
  }
}

// chip select
void initchipselect()
{
  for (int i = 0; i < NB_CHIP; i++)
  {
    pinMode(chipsSelect[i], OUTPUT);
  }
  resetchipselect();
}

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  myTransfer.begin(Serial1);
  initchipselect();
  //initMsg();
}

bool lastValue = false;
void loop()
{
  digitalReadInput();

  //pour test
  lastValue = !lastValue;
  message.digInput[0] = lastValue;
  // f

  analogReadInput();
  myTransfer.sendDatum(message);
  delay(1000);
}