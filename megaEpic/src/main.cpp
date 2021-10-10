#include <Arduino.h>
#include "Wire.h"
#include "SerialTransfer.h"

#define NB_INPUT 8
#define NB_CHIP 24
#define nbDigInput NB_CHIP *NB_INPUT
#define nbAnaInput 16

#define DEBUG false
#define DEBUG_EXECUTION_TIME true

SerialTransfer myTransfer;

const int anaPin[nbAnaInput] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};

//port L pour les DATA
const int dataPin[NB_INPUT] = {49, 48, 47, 46, 45, 44, 43, 42};

//liste des chip select
const int chipsSelect[NB_CHIP] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};

struct STRUCT
{
  bool digInput[nbDigInput]; //bool
  int anaInput[nbAnaInput];  //int
} message;

void analogReadInput()
{
  if (DEBUG)
  {
    Serial.println("analogRead");
  }
  if (DEBUG_EXECUTION_TIME)
  {
    Serial.print("\tRead analog Input start at ");
    Serial.println(millis());
  }
  for (int i = 0; i < nbAnaInput; i++)
  {
    message.anaInput[i] = analogRead(anaPin[i]);
    //message.anaInput[i]=random(nbAnaInput);
    if (DEBUG)
    {
      Serial.print(message.anaInput[i]);
      Serial.print(" ");
    }
  }
  if (DEBUG_EXECUTION_TIME)
  {
    Serial.print("\tRead analog Input end at ");
    Serial.println(millis());
    Serial.println();
  }
  if (DEBUG)
  {
    Serial.println();
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

void initInput()
{
  for (int i = 0; i < NB_INPUT; i++)
  {
    pinMode(dataPin[i], INPUT_PULLUP);
  }
}

void digitalReadInput()
{
  if (DEBUG_EXECUTION_TIME)
  {
    Serial.print("\tRead digital Input start at ");
    Serial.println(millis());
  }
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
    for (int j = 0; j < 8; j++)
    {
      int selectedInput = (i * 8) + j;
      message.digInput[selectedInput] = digitalRead(dataPin[j]);
      if (DEBUG)
      {
        Serial.print(" ");
        Serial.print(message.digInput[selectedInput]);
      }
    }
    if (DEBUG)
    {
      Serial.println();
      //delay(1000);
    }
  }
  digitalWrite(chipsSelect[NB_CHIP - 1], LOW);

  if (DEBUG_EXECUTION_TIME)
  {
    Serial.print("\tRead digital Input end at ");
    Serial.println(millis());
    Serial.println();
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
  initchipselect();
  initInput();
  //initMsg();
}

bool lastValue = false;

void loop()
{
  if (DEBUG_EXECUTION_TIME)
  {
    Serial.print("loop start at ");
    Serial.println(millis());
  }
  digitalReadInput();
  analogReadInput();
  myTransfer.sendDatum(message);

  if (DEBUG)
  {
    delay(1000);
  }
  if (DEBUG_EXECUTION_TIME)
  {
    Serial.print("loop end at ");
    Serial.println(millis());
    Serial.println();
    delay(1000);
  }
}