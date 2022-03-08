#include <ArduinoJson.h>



// constante
#define NB_INPUT 8
#define NB_CHIP 24
#define nbDigInput NB_CHIP *NB_INPUT

// port L pour les DATA
const int dataPin[NB_INPUT] = {49, 48, 47, 46, 45, 44, 43, 42};

// liste des chip select    attention 18/19 serial      20/21 I2C
const int chipsSelect[NB_CHIP] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 22, 23, 24, 25, 26, 27, 28,29};


 bool digInput[nbDigInput];


//#define DEBUG
//#define DEBUG_EXECUTION_TIME



bool echoMode=false;

StaticJsonDocument<750> doc;


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
  

  Serial.print(F("{\"cmd\": \"stDigIn\",\"data\":"));
  Serial.print(F("["));
  for (int i = 0; i < nbDigInput; i++)
  {
    Serial.print(digInput[i]);
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
 *    initDigOutput
 *
 *
 *  initialiser les ports des composants MCP23017 en sortie
 *
 *
 **/
void initDigOutput()
{
  for (int i = 0; i < NB_INPUT; i++)
  {
    pinMode(dataPin[i], OUTPUT);
   
  }

  Serial.println("set HIGH");

  for (int i = 0; i < NB_INPUT; i++)
  {
      digitalWrite(dataPin[i],HIGH);
      
  }

  delay(20000);
  Serial.println("set LOW");
   for (int i = 0; i < NB_INPUT; i++)
  {
      digitalWrite(dataPin[i],LOW);
      
  }
  
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
                digInput[channelNb]=true;
            }
            if(state.as<int>()==0){
              digInput[channelNb]=false;
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
          echoMode=data.as<bool>();
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




void setup()
{
  Serial.begin(115200);

  initchipselect();
    initDigOutput();
  
}

unsigned long startTime;
int8_t lastActivated=45;
void loop()
{
 
#ifdef DEBUG_EXECUTION_TIME
startTime=millis();
#endif


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
      bool bittemp=isActivated*8+i;
      //bool bittemp=HIGH;
      Serial.print(bittemp);
      Serial.print(" ");
      Serial.print(digInput[bittemp]);
      digitalWrite(dataPin[i],digInput[bittemp]);
    }
    Serial.println();
  }


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