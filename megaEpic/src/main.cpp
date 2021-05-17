#include <Arduino.h>


#include "SerialTransfer.h"

#define nbDigOutput 32
#define nbDigInput 192
#define nbAnaInput 16

SerialTransfer myTransfer;

const int anaPin[nbAnaInput] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};

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

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  myTransfer.begin(Serial1);
  //initMsg();
}

bool lastValue = false;
void loop()
{
  lastValue = !lastValue;
  message.digInput[0] = lastValue;
  //digitalReadInput();
  analogReadInput();
  myTransfer.sendDatum(message);
  delay(1000);
}