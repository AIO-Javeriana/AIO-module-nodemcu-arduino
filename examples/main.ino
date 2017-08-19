/*
 */
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <vector>
StaticJsonBuffer<200> jsonBuffer;
#include "SIOCommnuicationChannel.h"
#include "AIOState.h"
#include "AIOModule.h"
#include "AIOCommand.h"


char* ssid = "....";
char* password = "....";

char* host = "192.168.0.8";
int port = 9090;

SIOCommnuicationChannel commnuicationChannel = SIOCommnuicationChannel::getInstance(host,port);
AIOState state("NODEMCU");
AIOModule& module=AIOModule::getInstance();
bool acceptWorkATTENTION_CYCLE(AIOState& state){
    return true;
}

bool executeServiceATTENTION_CYCLE(AIOState& state,CommunicationChannel*& communicationChannel){
    return true;
}

void setup() {
  std::vector<char*> params;
  params.push_back("none");
  AIOActionCommand attention_cycle("ATTENTION_CYCLE",params,false,false,acceptWorkATTENTION_CYCLE,executeServiceATTENTION_CYCLE);
  module.setState(state);
  module.setCommunicationChannel(&commnuicationChannel);
  module.addCommand(attention_cycle);
  Serial.begin(115200);
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
  module.setup();
  
}

void loop() {
  module.loop();
}
