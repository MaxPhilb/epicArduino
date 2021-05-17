#include "SerialTransfer.h"
#include <Adafruit_MCP23017.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

int addr = 0;
bool echoMode = false;

#define nbDigOutput 32
#define nbDigInput 192
#define nbAnaInput 16

Adafruit_MCP23017 digOutput1;
Adafruit_MCP23017 digOutput2;

StaticJsonDocument<256> doc;
SerialTransfer myTransfer;

struct STRUCT
{
  bool digInput[nbDigInput]; //bool
  int anaInput[nbAnaInput];  //int
} messageFromMega;

void printL()
{

  Serial.print("{\"cmd\": \"stateAnaInput\",\"data\":");
  Serial.print("[");
  for (int i = 0; i < nbAnaInput; i++)
  {
    Serial.print("{\"channel\"");
    Serial.print(":");
    Serial.print(i);
    Serial.print(",");
    Serial.print("\"state\"");
    Serial.print(":");
    Serial.print(messageFromMega.anaInput[i]);
    Serial.print("}");
  }
  Serial.println("]");
  Serial.println("}!");

  Serial.print("{\"cmd\": \"stateDigInput\",\"data\":");
  Serial.print("[");
  for (int i = 0; i < nbDigInput; i++)
  {
    Serial.print("{\"channel\"");
    Serial.print(":");
    Serial.print(i);
    Serial.print(",");
    Serial.print("\"state\"");
    Serial.print(":");
    Serial.print(messageFromMega.digInput[i]);
    Serial.print("}");
  }
  Serial.print("]");
  Serial.println("}!");
}
void initDigOutput()
{
  for (int i = 0; i < 16; i++)
  {
    digOutput1.pinMode(i, OUTPUT);
    digOutput2.pinMode(i, OUTPUT);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  myTransfer.begin(Serial1);

  EEPROM.get(addr, echoMode);

  digOutput1.begin(); // use default address 0
  digOutput2.begin(); // use default address 0
  initDigOutput();
}

void setOutput(int channel, bool state)
{
  if (channel >= 0 && channel <= 15)
  {
    digOutput1.digitalWrite(channel, state);
  }
  if (channel >= 16 && channel <= 31)
  {
    int tempChannel = channel - 15;
    digOutput2.digitalWrite(tempChannel, state);
  }
  /*
  //test local
  if (channel == 0)
  {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("builtin");
    digitalWrite(LED_BUILTIN, state);
  }
  */
}

void returnCorrectCommand()
{
  doc.clear();
  doc["cmd"] = "command_return";
  doc["data"] = "OK";
  serializeJsonPretty(doc, Serial);
}

void returnIncorretCommand(String desc)
{
  doc.clear();
  doc["cmd"] = "command_return";
  doc["data"] = "KO";
  doc["description"] = desc;
  serializeJsonPretty(doc, Serial);
}

void setEchoMode(bool state)
{
  EEPROM.put(addr, state);
  echoMode = state;
}

void interprete()
{
  JsonObject object = doc.as<JsonObject>();
  JsonVariant cmd = object["cmd"];

  if (!cmd.isNull())
  {

    if (cmd == "digOutput")
    {
      JsonArray data = object["data"].as<JsonArray>();
      if (!data.isNull())
      {

        for (JsonObject output : data)
        {

          JsonVariant channel = output["channel"];
          JsonVariant state = output["state"];

          if (channel.is<int>() && state.is<bool>())
          {
            setOutput(channel.as<int>(), state.as<bool>());
          }
        }

        returnCorrectCommand();
      }
      else
      {
        returnIncorretCommand(F("data non présente"));
      }
    }
    else if (cmd == "setEcho")
    {
      JsonVariant data = object["data"].as<JsonVariant>();
      if (!data.isNull())
      {

        if (data.is<bool>())
        {
          setEchoMode(data.as<bool>());
          returnCorrectCommand();
        }
        else
        {
          returnIncorretCommand(F("data n'est pas un bool"));
        }
      }
      else
      {
        returnIncorretCommand(F("data non présente"));
      }
    }
    else
    {
      returnIncorretCommand(F("La commande [cmd] n'est pas implementee"));
    }
  }
  else
  {
    returnIncorretCommand(F("La commande [cmd] est manquante "));
  }
}

void loop()
{
  if (myTransfer.available())
  {

    myTransfer.rxObj(messageFromMega);
    if (echoMode)
    {
      printL();
    }
  }

  if (Serial.available())
  {
    String messageFromUSB = Serial.readStringUntil('!');
    //Serial.println(messageFromUSB);
    deserializeJson(doc, messageFromUSB);
    interprete();
  }

  /*
  {
    "cmd" : "digOutput",
    "data" : [
      {
        "channel" : 1,
        "state" : false
      },
      {
        "channel" : "all",
        "state" : false
      }
    ]
  }!
  */
}