#include "SerialTransfer.h"
#include <Adafruit_MCP23017.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

//constante
#define nbDigOutput 32
#define nbDigInput 192
#define nbAnaInput 16

#define addressMCP1 0x0
#define addressMCP2 0x1

//var global
int addr = 0;
bool echoMode = false;

Adafruit_MCP23017 digOutput1;
Adafruit_MCP23017 digOutput2;

StaticJsonDocument<256> doc;
SerialTransfer myTransfer;

struct STRUCT
{
  bool digInput[nbDigInput]; //bool
  int anaInput[nbAnaInput];  //int
} messageFromMega;

/**
 * 
 *          PrintL
 * 
 *  afficher sur le port serie USB l'etat des entrees au format Json
 * 
 * 
 **/
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

/**
 * 
 * 
 *    initDigOutput
 * 
 * 
 *  initialiser les ports des composants MCP23017 en sortie
 * 
 * 
 **/
void initDigOutput()
{
  for (int i = 0; i < 16; i++)
  {
    digOutput1.pinMode(i, OUTPUT);
    digOutput2.pinMode(i, OUTPUT);
  }
}

/**
 * 
 * 
 *    setOutput
 * 
 * 
 *  changer l'etat d'un port des MCP23017
 * 
 * 
 **/
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

/**
 * 
 * 
 *    returnCorrectCommand
 * 
 * 
 *  retourne un message sur le port serie USB pour signaler que le message a bien ete compris
 * 
 * 
 **/
void returnCorrectCommand()
{
  doc.clear();
  doc["cmd"] = "command_return";
  doc["data"] = "OK";
  serializeJsonPretty(doc, Serial);
}

/**
 * 
 * 
 *    returnIncorretCommand
 * 
 * 
 *  retourne un message sur le port serie USB pour signaler un probleme avec le message recu
 * 
 * 
 **/
void returnIncorretCommand(String desc)
{
  doc.clear();
  doc["cmd"] = "command_return";
  doc["data"] = "KO";
  doc["description"] = desc;
  serializeJsonPretty(doc, Serial);
}

/**
 * 
 * 
 *    setEchoMode
 * 
 * 
 *  Permet de changer l'etat de l'eeprom pour activer ou desactiver le retour des entrees sur le port serie
 * 
 * 
 **/
void setEchoMode(bool state)
{
  EEPROM.put(addr, state);
  echoMode = state;
}

/**
 * 
 * 
 *    interprete
 * 
 * 
 *  decode le message recu sur le port serie USB
 * 
 * 
 **/
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

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  myTransfer.begin(Serial1);

  EEPROM.get(addr, echoMode); //lit dans leeprom si le mode echo est activé

  digOutput1.begin(addressMCP1);
  digOutput2.begin(addressMCP2);
  initDigOutput();
}

void loop()
{
  if (myTransfer.available())
  {

    myTransfer.rxObj(messageFromMega); //lit le retour en provenance de larduino Mega
    if (echoMode)
    {
      printL();
    }
  }

  if (Serial.available()) //lit les informations sur le port serie USB
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