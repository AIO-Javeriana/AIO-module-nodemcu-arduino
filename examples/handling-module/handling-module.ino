//============================================================================//
// INCLUDE
//============================================================================//
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <vector>
#include <Servo.h>
#include <AIOModule.h>

#define PARAMETERS JsonObject

//============================================================================//
// DEFINE PINS
//============================================================================//
// Define gripper pin. 
#define GRIPPER_MOTOR                   D4

//============================================================================//
// DEFINE GLOBAL CONTROLS
//============================================================================//

char* ssid = "AIO";//wifi network name
char* password = "";//wifi network password

bool isSync = false;
SIOCommnuicationChannel commnuicationChannel = SIOCommnuicationChannel::getInstance();
AIOState state("NODEMCU-HANDLING-MODULE");
AIOModule& module=AIOModule::getInstance();
// Define the control for gripper motor.
Servo gripperServo;

bool acceptWorkOPEN_GRIPPER(AIOState& state,PARAMETERS& parameters){
    Serial.println("[OPEN_GRIPPER]:acceptWork");
    return true;
}

bool executeCommandOPEN_GRIPPER(AIOState& state,CommunicationChannel*& communicationChannel,PARAMETERS& parameters){
    Serial.println("[OPEN_GRIPPER]:executeCommand");
    gripperServo.write(65);
    return true;
}

bool abortCommandOPEN_GRIPPER(AIOState& state,CommunicationChannel*& communicationChannel){
    Serial.println("[OPEN_GRIPPER]:abortCommand");
    return true;
}
bool acceptWorkCLOSE_GRIPPER(AIOState& state,PARAMETERS& parameters){
    Serial.println("[CLOSE_GRIPPER]:acceptWork");
    return true;
}

bool executeCommandCLOSE_GRIPPER(AIOState& state,CommunicationChannel*& communicationChannel,PARAMETERS& parameters){
    Serial.println("[CLOSE_GRIPPER]:executeCommand");
    gripperServo.write(0);
    return true;
}

bool abortCommandCLOSE_GRIPPER(AIOState& state,CommunicationChannel*& communicationChannel){
    Serial.println("[CLOSE_GRIPPER]:abortCommand");
    return true;
}

AIOActionCommand openCommand("OPEN_GRIPPER",*(new std::vector<char*>()),false,acceptWorkOPEN_GRIPPER,executeCommandOPEN_GRIPPER,abortCommandOPEN_GRIPPER);
AIOActionCommand closeCommand("CLOSE_GRIPPER",*(new std::vector<char*>()),false,acceptWorkCLOSE_GRIPPER,executeCommandCLOSE_GRIPPER,abortCommandCLOSE_GRIPPER);

void setup() {
  Serial.begin(115200);
  Serial.println();
  //------------------------------------------------------------------------//
  // Init servos.
  //------------------------------------------------------------------------//
  gripperServo.attach(GRIPPER_MOTOR);
  // Set init state (close).
  gripperServo.write(0);
  commnuicationChannel.setDebugging(false);
  module.setState(state);
  module.setCommunicationChannel(&commnuicationChannel);
  module.addCommand(openCommand);
  module.addCommand(closeCommand);
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
