
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
    Joystick_(0x03, JOYSTICK_TYPE_GAMEPAD , 32, 0, false, false, false, false, false, false, false, false, false, false, false),
    Joystick_(0x04, JOYSTICK_TYPE_GAMEPAD , 32, 0, false, false, false, false, false, false, false, false, false, false, false),
    Joystick_(0x05, JOYSTICK_TYPE_GAMEPAD , 32, 0, false, false, false, false, false, false, false, false, false, false, false),
    Joystick_(0x06, JOYSTICK_TYPE_GAMEPAD , 32, 0, false, false, false, false, false, false, false, false, false, false, false),
    Joystick_(0x07, JOYSTICK_TYPE_GAMEPAD, 32, 0, true, true, true, true, true, true, true, true, false, false, false),
    Joystick_(0x08, JOYSTICK_TYPE_GAMEPAD, 32, 0, true, true, true, true, true, true, true, true, false, false, false)

};


// var global
int addrEEPROM = 0; // pour variable echo sur EEPROM
bool echoMode = false;

Adafruit_MCP23X17 digOutput1;
Adafruit_MCP23X17 digOutput2;

uint8_t addressMCP1 = 0x20;
uint8_t addressMCP2 = 0x21;

StaticJsonDocument<350> doc;

/**
 *
 *          ConfJoy
 *
 *  transformer le message en provenance du mega vers les joysticks
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
       /*
      Serial.print("num ");
      Serial.print(num);
      Serial.print(" numTab ");
      Serial.print(numTab);
      Serial.print(" jit ");
      Serial.print(jit);
      Serial.print(" state ");
      Serial.println(st);
      */
      if (st)
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
  Serial.println(F("}!"));

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
  Serial.println(F("}!"));
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
    Serial.print("dig2 ");
   Serial.print(channel);
    Serial.print(" state ");
    Serial.println(state);
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

    if (cmd == "digOutput") //{"cmd":"digOutput","data":[1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,x,0,0,0,0,0,0,0,0,0,0]}!
    {
      JsonArray data = object["data"].as<JsonArray>();
      Serial.println(data.size());
      if (!data.isNull())
      {
        int channelNb=0;
        //Serial.print(" data size ");
        //Serial.println(data.size());
        for (JsonVariant state : data)
        {
          //1 ou 0 ou x
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
  
  Joystick[4].setXAxisRange(0,resolutionAnalog);
  Joystick[4].setYAxisRange(0,resolutionAnalog);
  Joystick[4].setZAxisRange(0,resolutionAnalog);
  Joystick[4].setRxAxisRange(0,resolutionAnalog);
  Joystick[4].setRyAxisRange(0,resolutionAnalog);
  Joystick[4].setRzAxisRange(0,resolutionAnalog);
  Joystick[4].setRudderRange(0,resolutionAnalog);
  Joystick[4].setThrottleRange(0,resolutionAnalog);

  Joystick[5].setXAxisRange(0,resolutionAnalog);
  Joystick[5].setYAxisRange(0,resolutionAnalog);
  Joystick[5].setZAxisRange(0,resolutionAnalog);
  Joystick[5].setRxAxisRange(0,resolutionAnalog);
  Joystick[5].setRyAxisRange(0,resolutionAnalog);
  Joystick[5].setRzAxisRange(0,resolutionAnalog);
  Joystick[5].setRudderRange(0,resolutionAnalog);
  Joystick[5].setThrottleRange(0,resolutionAnalog);

  for(int i=0;i<JOYSTICK_COUNT;i++){
    Joystick[i].begin(false);
  }

   // uncomment appropriate mcp.begin
  if (!digOutput1.begin_I2C(addressMCP1)) {
  //if (!mcp.begin_SPI(CS_PIN)) {
    Serial.println("Error. mcp1");
    while (1){
      Serial.print(1);
    };
  }
   // uncomment appropriate mcp.begin
  if (!digOutput2.begin_I2C(addressMCP2)) {
  //if (!mcp.begin_SPI(CS_PIN)) {
    Serial.println("Error. mcp2");
    while (1){
      Serial.print(2);
    };
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
    Serial.print("nb byte ");
    Serial.println(nbbyte);
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
  
  Joystick[4].setXAxis(message.anaInput[0]);
  Joystick[4].setYAxis(message.anaInput[1]);
  Joystick[4].setZAxis(message.anaInput[2]);
  Joystick[4].setRxAxis(message.anaInput[3]);
  Joystick[4].setRyAxis(message.anaInput[4]);
  Joystick[4].setRzAxis(message.anaInput[5]);
  Joystick[4].setRudder(message.anaInput[6]);
  Joystick[4].setThrottle(message.anaInput[7]);
  Joystick[4].sendState();

  // analogique 8 à 15

  Joystick[5].setXAxis(message.anaInput[8]);
  Joystick[5].setYAxis(message.anaInput[9]);
  Joystick[5].setZAxis(message.anaInput[10]);
  Joystick[5].setRxAxis(message.anaInput[11]);
  Joystick[5].setRyAxis(message.anaInput[12]);
  Joystick[5].setRzAxis(message.anaInput[13]);
  Joystick[5].setRudder(message.anaInput[14]);
  Joystick[5].setThrottle(message.anaInput[15]);

  
  Joystick[5].sendState();
}

unsigned long startTime;
void loop()
{
 
#ifdef DEBUG_EXECUTION_TIME
startTime=millis();
#endif

//readDigIN();
//readAnaIN();

//delay(5);

#ifdef DEBUG_EXECUTION_TIME
unsigned long deltaTime=millis()-startTime;
Serial.print("execution time: ");
Serial.println(deltaTime);
#endif


 
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
  
}