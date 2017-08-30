/*
 */
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <vector>
#include "AIOModule.h"

#define PARAMETERS JsonObject

char* ssid = "AIO";
char* password = "";
bool isSync = false;

SIOCommnuicationChannel commnuicationChannel = SIOCommnuicationChannel::getInstance();
AIOState state("NODEMCU");
AIOModule& module=AIOModule::getInstance();
bool acceptWorkATTENTION_CYCLE(AIOState& state,PARAMETERS& parameters){
    Serial.println(" acceptWorkATTENTION_CYCLE <<<<-----");
    return true;
}

bool executeServiceATTENTION_CYCLE(AIOState& state,CommunicationChannel*& communicationChannel,PARAMETERS& parameters){
    delay(1000);
    return true;
}

bool abortServiceATTENTION_CYCLE(AIOState& state,CommunicationChannel*& communicationChannel){
    delay(1000);
    return true;
}

std::vector<char*> params;
AIOActionCommand attention_cycle("ATTENTION_CYCLE",params,false,false,acceptWorkATTENTION_CYCLE,executeServiceATTENTION_CYCLE,abortServiceATTENTION_CYCLE);

void setup() {
  Serial.begin(115200);
  Serial.println();
  params.push_back("none");
  module.setState(state);
  module.setCommunicationChannel(&commnuicationChannel);
  module.addCommand(attention_cycle);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  moduleSync();
  //*/
}

void moduleSync(){
  if ((isSync = commnuicationChannel.syncWithAIO())){
      Serial.printf("Sync with %s:%d \n",commnuicationChannel.getHost(),commnuicationChannel.getPort());
      module.setup();
  } 
  //*/
}

void loop() {
  if (isSync){
     module.loop();
  }else {
    moduleSync();
  }
}
