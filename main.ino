/*
 */
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <vector>
//StaticJsonBuffer<500> jsonBuffer;
//#include "./libs/socket.io-client/SocketIO_Client.h"
#include "SIOCommnuicationChannel.h"
#include "AIOState.h"
#include "AIOModule.h"
#include "AIOCommand.h"


char* ssid = "BERMEOA";
char* password = "Meje015$";

char* host = "192.168.0.8";
int port = 9090;

SIOCommnuicationChannel commnuicationChannel = SIOCommnuicationChannel::getInstance(host,port);
AIOState state("NODEMCU");
AIOModule& module=AIOModule::getInstance();
bool acceptWorkATTENTION_CYCLE(AIOState& state){
    Serial.println(" acceptWorkATTENTION_CYCLE <<<<-----");
    return true;
}

bool executeServiceATTENTION_CYCLE(AIOState& state,CommunicationChannel*& communicationChannel){
    return true;
}

std::vector<char*> params;
AIOActionCommand attention_cycle("ATTENTION_CYCLE",params,false,false,acceptWorkATTENTION_CYCLE,executeServiceATTENTION_CYCLE);

void setup() {
  params.push_back("none");
  Serial.begin(115200);
  Serial.println();
  DynamicJsonBuffer jsonBuffer;
  String nn = String((const char*)"{\\\"EVENT_NAME\\\":\\\"WORK_ASSIGNATION\\\",\\\"COMMAND_ID\\\":8,\\\"COMMAND\\\":\\\"ATTENTION_CYCLE\\\",\\\"PARAMS\\\":{\\\"EMOTIONAL_VALUE\\\":1},\\\"GROUP_ID\\\":8}");
  nn.replace("\\","");
  Serial.println(nn);
  JsonObject& data = jsonBuffer.parseObject(nn.c_str());
  if (!data.success()){
        Serial.println("parseObject() failed");
        return;
  }
  Serial.print("->>>>>>>>> ");
  Serial.print((const char*)data["EVENT_NAME"]);
  Serial.println(" <<<<-----");
  
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
  module.setup();
  //*/
}

void loop() {
  module.loop();
}
