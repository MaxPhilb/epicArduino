
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Joystick.h>


// constante
#define NB_INPUT 8
#define NB_CHIP 24
#define nbDigInput NB_CHIP *NB_INPUT
#define nbAnaInput 16


#define resolutionAnalog 1023


//#define DEBUG
//#define DEBUG_EXECUTION_TIME

struct STRUCT
{
  byte digInput[NB_CHIP];   // bool
  int anaInput[nbAnaInput]; // int
} message;

const uint8_t entete = 45;




#define JOYSTICK_COUNT 6

Joystick_ Joystick[JOYSTICK_COUNT] = {
    Joystick_(0x03, JOYSTICK_TYPE_GAMEPAD , 32, 0, true, true, true, true, false, false, false, false, false, false, false),
    Joystick_(0x04, JOYSTICK_TYPE_GAMEPAD , 32, 0, true, true, true, true, false, false, false, false, false, false, false),
    Joystick_(0x05, JOYSTICK_TYPE_GAMEPAD , 32, 0, true, true, true, true, false, false, false, false, false, false, false),
    Joystick_(0x06, JOYSTICK_TYPE_GAMEPAD , 32, 0, true, true, true, true, false, false, false, false, false, false, false),
    Joystick_(0x07, JOYSTICK_TYPE_GAMEPAD, 32, 0, false, false, false, false, false, false, false, false, false, false, false),
    Joystick_(0x08, JOYSTICK_TYPE_GAMEPAD, 32, 0, false, false, false, false, false, false, false, false, false, false, false)

};


// var global
int addrEEPROM = 0; // pour variable echo sur EEPROM
bool echoMode = false;

Adafruit_MCP23X17 digOutput1; // carte sorties digital
Adafruit_MCP23X17 digOutput2;

uint8_t addressMCP1 = 0x20;
uint8_t addressMCP2 = 0x21;

StaticJsonDocument<350> doc;
//DynamicJsonDocument doc(450);

/**
 *
 *          ConfJoy
 *
 *  transformer le message en provenance du mega vers les joysticks
 *  on recupere la trame de 32*Joystick_count et on affecte chaque bit des joysticks
 *
 *
 **/

void confJoy()
{

 for (int i = 0; i < JOYSTICK_COUNT; i++)
  {
    for (int j = 0; j < 32; j++)
    {
     
       int num=(i*32)+j;
       int numTab=num/NB_INPUT;
       int jit= num-(numTab*8);
       
       bool st=bitRead(message.digInput[numTab],jit);
      #ifdef DEBUG
      Serial.print("num ");
      Serial.print(num);
      Serial.print(" numTab ");
      Serial.print(numTab);
      Serial.print(" jit ");
      Serial.print(jit);
      Serial.print(" state ");
      Serial.println(st);
      #endif
      
      if (!st)
      {
        Joystick[i].pressButton(j);
      }
      else
      {
        Joystick[i].releaseButton(j);
      }
      //Joystick[i].sendState();
      //delay(500);
      //delayMicroseconds(5);
    }
    Joystick[i].sendState();
  }
   //Serial.println();

  
  
  
}


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
   Serial.print(F("{\"cmd\": \"stAnaIn\",\"data\":"));
  Serial.print(F("["));
  for (int i = 0; i < nbAnaInput; i++)
  {
    Serial.print(message.anaInput[i]);
     if(i!= nbAnaInput-1){
    Serial.print(F(","));
     }
  }
  Serial.print(F("]"));
  Serial.println(F("}"));

  Serial.print(F("{\"cmd\": \"stDigIn\",\"data\":"));
  Serial.print(F("["));
  for (int i = 0; i < nbDigInput; i++)
  {
    Serial.print(message.digInput[i]);
    if(i!= nbDigInput-1){
      Serial.print(F(","));
    }
    
  }
  Serial.print(F("]"));
  Serial.println(F("}"));
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
    /*
    Serial.print("dig1 ");
    Serial.print(channel);
    Serial.print(" state ");
    Serial.println(state);
    */
  }
  if (channel >= 16 && channel <= 31)
  {
    int tempChannel = channel - 16;
    digOutput2.digitalWrite(tempChannel, state);
    /*
    Serial.print("dig2 ");
   Serial.print(channel);
    Serial.print(" state ");
    Serial.println(state);
    */
  }
 
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

  /*
  Serial.println("set HIGH");

  for (int i = 0; i < 32; i++)
  {
      setOutput(i,HIGH);
      
  }

  delay(20000);
  for (int i = 0; i < 32; i++)
  {
      setOutput(i,LOW);
  }
  Serial.println("set LOW");
   delay(4000);
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
  EEPROM.put(addrEEPROM, state);
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

    //        {"cmd":"digOutput","data":[0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]}!
    //        {"cmd":"digOutput","data":[0,0,0,1,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0]}!
    //        {"cmd":"digOutput","data":[0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]}!
    //        {"cmd":"digOutput","data":[1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]}!
    // 0 sortie off
    // 1 sortie On
    // 7 ne change pas l'etat d'avant

    if (cmd == "digOutput") 
    {
      JsonArray data = object["data"].as<JsonArray>();
      //Serial.println(data.size());
      if (!data.isNull())
      {
        int channelNb=0;
        //Serial.print(" data size ");
        //Serial.println(data.size());
        for (JsonVariant state : data)
        {
          //1 ou 0 ou 7
          //Serial.print("state is int: ");
          //Serial.println(state.is<int>());
          if ( state.is<int>())
          {
            if(state.as<int>()==1){
              setOutput(channelNb, true);
            }
            if(state.as<int>()==0){
              setOutput(channelNb, false );
            }
            
            
          }else{
            Serial.println("state is not a int");
          }
          channelNb++;
        }

        returnCorrectCommand();
      }
      else
      {
        returnIncorretCommand(F("data non présente"));
      }
    }
    else if (cmd == "setEcho") // {"cmd":"setEcho","data":false}!
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
  Wire.begin();


  //myTransfer.begin(Serial1);
  EEPROM.get(addrEEPROM, echoMode); // lit dans leeprom si le mode echo est active
  
 
  /* initialise les analogiques des joysticks , on definit la resolution de 10 bits*/
  Joystick[0].setXAxisRange(0,resolutionAnalog);
  Joystick[0].setYAxisRange(0,resolutionAnalog);
  Joystick[0].setZAxisRange(0,resolutionAnalog);
  Joystick[0].setRxAxisRange(0,resolutionAnalog);


  Joystick[1].setXAxisRange(0,resolutionAnalog);
  Joystick[1].setYAxisRange(0,resolutionAnalog);
  Joystick[1].setZAxisRange(0,resolutionAnalog);
  Joystick[1].setRxAxisRange(0,resolutionAnalog);

  Joystick[2].setXAxisRange(0,resolutionAnalog);
  Joystick[2].setYAxisRange(0,resolutionAnalog);
  Joystick[2].setZAxisRange(0,resolutionAnalog);
  Joystick[2].setRxAxisRange(0,resolutionAnalog);

  Joystick[3].setXAxisRange(0,resolutionAnalog);
  Joystick[3].setYAxisRange(0,resolutionAnalog);
  Joystick[3].setZAxisRange(0,resolutionAnalog);
  Joystick[3].setRxAxisRange(0,resolutionAnalog);

  /* active les joystisks */
  for(int i=0;i<JOYSTICK_COUNT;i++){
    Joystick[i].begin(false);
  }

  /* initialise les cartes sorties bool */

  if (!digOutput1.begin_I2C(addressMCP1)) {
  //if (!mcp.begin_SPI(CS_PIN)) {
    Serial.println("Impossible de demarrer le module mcp1 de la carte sortie digital");
    /*
    while (1){
      //Serial.print(1);
    };
    */
  }

  
  if (!digOutput2.begin_I2C(addressMCP2)) {
  //if (!mcp.begin_SPI(CS_PIN)) {
    Serial.println("Impossible de demarrer le module mcp2 de la carte sortie digital");
    /*
    while (1){
      //Serial.print(2);
    };
    */
  }

  initDigOutput();
  
}

 /***
 * 
 * 
 *  Lecture des entrees dig
 * 
 * 
 */
void readDigIN(){
    
    Wire.beginTransmission(8);
    Wire.write(1); // send register to read, the 'result' register
    Wire.endTransmission();
    int nbbyte=Wire.requestFrom(8,NB_CHIP);
    #ifdef DEBUG
    Serial.print("nb byte ");
    Serial.println(nbbyte);
    #endif
    Wire.readBytes(message.digInput,NB_CHIP);

    #ifdef DEBUG
    Serial.print("message  ");
    for(int i=0;i<=NB_CHIP;i++){
        Serial.print(message.digInput[i],BIN);
        Serial.print(" ");
    }
     Serial.println();
    
    #endif

    confJoy();

}



/***
* 
* 
*  Lecture des entrees ana
* 
* 
*/
void readAnaIN(){
    
    byte Mess[(nbAnaInput*2)];
    Wire.beginTransmission(8);
    Wire.write(2); 
    Wire.endTransmission();
    int nbbyte=Wire.requestFrom(8,(nbAnaInput*2));
    #ifdef DEBUG
    //Serial.print("nb byte Ana ");
    //Serial.println(nbbyte);
    #endif
    Wire.readBytes(Mess,(nbAnaInput*2));
    int i=-1;
    for(int j=0;j<nbAnaInput;j++)
    {
      i++;
      byte byt1=Mess[i];
      i++;
      byte byt2=Mess[i];
      int val=byt1+(byt2<<8);
      message.anaInput[j]=val;
    }

     // analogique 0 à 7
  
  Joystick[0].setXAxis(message.anaInput[0]);
  Joystick[0].setYAxis(message.anaInput[1]);
  Joystick[0].setZAxis(message.anaInput[2]);
  Joystick[0].setRxAxis(message.anaInput[3]);
  Joystick[0].sendState();


  Joystick[1].setXAxis(message.anaInput[4]);
  Joystick[1].setYAxis(message.anaInput[5]);
  Joystick[1].setZAxis(message.anaInput[6]);
  Joystick[1].setRxAxis(message.anaInput[7]);
  Joystick[1].sendState();

  // analogique 8 à 15

  Joystick[2].setXAxis(message.anaInput[8]);
  Joystick[2].setYAxis(message.anaInput[9]);
  Joystick[2].setZAxis(message.anaInput[10]);
  Joystick[2].setRxAxis(message.anaInput[11]);
  Joystick[2].sendState();


  Joystick[3].setXAxis(message.anaInput[12]);
  Joystick[3].setYAxis(message.anaInput[13]);
  Joystick[3].setZAxis(message.anaInput[14]);
  Joystick[3].setRxAxis(message.anaInput[15]);
  Joystick[3].sendState();
}

unsigned long startTime;
void loop()
{
 
#ifdef DEBUG_EXECUTION_TIME
startTime=millis();
#endif

readDigIN();
readAnaIN();

delay(5);




 
  if (echoMode)
  {
    printL();
  }
  


  if (Serial.available()) // lit les informations sur le port serie USB
  {
    String messageFromUSB = Serial.readStringUntil('!');
    // Serial.println(messageFromUSB);
    deserializeJson(doc, messageFromUSB);
    interprete();
  }
  #ifdef DEBUG_EXECUTION_TIME
unsigned long deltaTime=millis()-startTime;
Serial.print("execution time: ");
Serial.println(deltaTime);
#endif
}