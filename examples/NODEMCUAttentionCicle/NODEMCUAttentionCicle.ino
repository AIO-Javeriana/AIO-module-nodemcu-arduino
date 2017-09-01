/*
 */
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <vector>
#include <AIOModule.h>

#define PARAMETERS JsonObject

char* ssid = "AIO";//wifi network name
char* password = "";//wifi network password
bool isSync = false;

SIOCommnuicationChannel commnuicationChannel = SIOCommnuicationChannel::getInstance();//Singleton
AIOState state("NODEMCU-ATTENTION_CYCLE");
AIOModule& module = AIOModule::getInstance();//Singleton

bool acceptWorkATTENTION_CYCLE(AIOState& state,PARAMETERS& parameters){
    Serial.println("[ATTENTION_CYCLE]:acceptWork");
    return true;
}

bool executeCommandATTENTION_CYCLE(AIOState& state,CommunicationChannel*& communicationChannel,PARAMETERS& parameters){
    Serial.println("[ATTENTION_CYCLE]:executeCommand");
    delay(2000);
    return true;
}

bool abortCommandATTENTION_CYCLE(AIOState& state,CommunicationChannel*& communicationChannel){
    Serial.println("[ATTENTION_CYCLE]:abortCommand");
    delay(2000);
    return true;
}

AIOActionCommand attention_cycle("ATTENTION_CYCLE",*(new std::vector<char*>()),false,acceptWorkATTENTION_CYCLE,executeCommandATTENTION_CYCLE,abortCommandATTENTION_CYCLE);

void setup() {
  Serial.begin(115200);
  Serial.println();
  commnuicationChannel.setDebugging(true);
  attention_cycle.getParameters().push_back("none");
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
}

void loop() {
  if (isSync){
     module.loop();
  }else {
    moduleSync();
  }
}
