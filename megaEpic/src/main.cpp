#include <Arduino.h>
#include "Wire.h"

#define NB_INPUT 8
#define NB_CHIP 24
#define nbDigInput NB_CHIP *NB_INPUT
#define nbAnaInput 16

#define DEBUG
//#define DEBUG_EXECUTION_TIME

const byte entete = 45;

const int anaPin[nbAnaInput] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};

// port L pour les DATA
const int dataPin[NB_INPUT] = {49, 48, 47, 46, 45, 44, 43, 42};

// liste des chip select
const int chipsSelect[NB_CHIP] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};

struct STRUCT
{
  byte digInput[NB_CHIP];
  int anaInput[nbAnaInput]; // int
} message;

void analogReadInput()
{
#ifdef DEBUG

  Serial.println("analogRead");

#endif

#ifdef DEBUG_EXECUTION_TIME

  Serial.print("\tRead analog Input start at ");
  Serial.println(millis());

#endif
  for (int i = 0; i < nbAnaInput; i++)
  {
    message.anaInput[i] = analogRead(anaPin[i]);
    // message.anaInput[i]=random(nbAnaInput);
#ifdef DEBUG

    Serial.print(message.anaInput[i]);
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

// reset chip select
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
  // DDRL = 0;
  for (int i = 0; i < NB_INPUT; i++)
  {
    pinMode(dataPin[i], INPUT_PULLUP);
  }
}

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

// chipselect->bas

void initMsg()
{
  int i = 0;

  for (i = 0; i < NB_CHIP; i++)
  {
    message.digInput[i] = 0;
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
  initchipselect();
  initInput();
  initMsg();
}

bool lastValue = false;

void loop()
{
#ifdef DEBUG_EXECUTION_TIME

  Serial.print("loop start at ");
  Serial.println(millis());

#endif

  digitalReadInput();
  analogReadInput();

  // Serial1.write(entete);
  byte Mess[NB_CHIP + 1];
  Mess[0] = entete;
  for (int i = 0; i < NB_CHIP; i++)
  {
    Mess[i + 1] = message.digInput[i];
  }
  Serial1.write(Mess, NB_CHIP + 1);

#ifdef DEBUG
  delay(1000);
#endif
#ifdef DEBUG_EXECUTION_TIME
  Serial.print("loop end at ");
  Serial.println(millis());
  Serial.println();
  delay(1000);
#endif
}