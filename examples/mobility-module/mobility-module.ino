//============================================================================//
// INCLUDE
//============================================================================//
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <vector>
#include <string.h>
#include <Servo.h>
#include <QTRSensors.h>
#include <AIOModule.h>

#define PARAMETERS JsonObject

//============================================================================//
// DEFINE STRUCTURES
//============================================================================//
enum MMState { FINISHED,FOLLOW_LINE};
class MobilityModuleState: public AIOState{
  private:
    MMState _mmstate;
  public:
    MobilityModuleState(char* module_id):AIOState(module_id){
        this->_mmstate = MMState::FINISHED;
    }
    MMState& getMMState(){
        return this->_mmstate;
    }
    void setMMState(MMState newMMState){
        this->_mmstate = newMMState;
    }
   
};

//============================================================================//
// DEFINE PINS
//============================================================================//
// Define rigth motor pin.
#define RIGTH_MOTOR                   D8
// Define left motor pin.
#define LEFT_MOTOR                    D7

#define EMITTER_PIN                   D0     // emitter is controlled by digital pin 
#define S0                            D6
#define S1                            D5
#define S2                            D4
#define S3                            D3
#define S4                            D2
#define S5                            D1
#define S6                            D9
#define S7                            D10

#define PIN_BATTERY_LVL               A0

//============================================================================//
// DEFINE CONSTANTS
//============================================================================//
#define NUM_SENSORS                   6     // number of sensors used
#define NUM_FRONT_SENSORS             2     // number of sensors used
#define TIMEOUT                       2500  // waits for 2500 microseconds for sensor outputs to go low
#define SENSITIVE_ERROR               200
#define EPSILON_LINE                  500
#define MAX_SPEED                     400
#define NORMAL_SPEED                  0
#define CALIBRATION_SPEED             400
//----------------------------------------------------------------------------//
// This is the maximum speed the motors will be allowed to  turn. (400 lets
// the motors go at top speed; decrease to impose a speed limit)
//----------------------------------------------------------------------------//
//============================================================================//
// DEFINE CALIBRATION CONSTANTS
//============================================================================//
// Define the cero for left motor speed.
const int SPEED_DEGREE_CERO_LM = 94;
// Define the cero for rigth motor speed.
const int SPEED_DEGREE_CERO_RM = 93;
// Define the proportional action of 1/6
const int PROPORTIONAL_ACTION = 1;
// Define de derivative action of 6.
const int DERIVATIVE_ACTION = 0;
//============================================================================//
// DEFINE GLOBAL VARIABLES
//============================================================================//

char* ssid = "AIO";//wifi network name
char* password = "";//wifi network password
bool isSync = false;

SIOCommnuicationChannel commnuicationChannel = SIOCommnuicationChannel::getInstance();
MobilityModuleState mmstate("NODEMCU-MOBILITY-MODULE");
AIOModule& module=AIOModule::getInstance();
//============================================================================//
// DEFINE GLOBAL CONTROLS
//============================================================================//
// Define the control for rigth motor.
Servo rigthServo;
// Define the control for left motor.
Servo leftServo;
// Define the control for array sensors.
QTRSensorsRC arraySensors((unsigned char[]) {S0, S1, S2, S3, S4, S5},
                          NUM_SENSORS, TIMEOUT, EMITTER_PIN);
// Define the array values sensor.
unsigned int sensorValues[NUM_SENSORS];
// Error for follow line algorithm.
int lastError = 0;
int integral = 0;
int position = 2500;
int direction = 0;

//============================================================================//
// DEFINE FUNCTIONS
//============================================================================//

/// int speedValue: Speed pf servo value(-400,400) 
void setSpeedMotor(Servo &servo,int speedValue, int speed_degree_cero,bool invested){
    // Check if speed more than 0.
    if (speedValue >= 0) {
      // Scale it to use it with the servo (value between 0 and 90 -> 90 to 180).
      servo.write((invested?-1:1)*map(speedValue, 0, 400, 0, 90) + speed_degree_cero);
    } else {
      // Scale it to use it with the servo (value between 0 and 90 -> 0 to 90).
      servo.write(-(invested?-1:1)*map( (speedValue * -1) , 0, 400, 0, 90) + speed_degree_cero);
    }
}
/// Set the speed to servos.
/// int leftSp: Speed pf left servo.
/// int rigthSp: Speed pf rigth servo.
void setSpeeds(int leftSp, int rigthSp) {
  setSpeedMotor(leftServo,leftSp,SPEED_DEGREE_CERO_LM,true);
  setSpeedMotor(rigthServo,rigthSp,SPEED_DEGREE_CERO_RM,false);
}

void followLine(int position){
    // Our "error" is how far we are away from the center of the line, which
    // corresponds to position 2500.
    //------------------------------------------------------------------------//
    int error = position - 2500;
    //------------------------------------------------------------------------//
    // Here we are using a proportional constant of 1/6 and a derivative
    // constant of 6 from algorithm.
    //------------------------------------------------------------------------//
    int speedDifference = error / PROPORTIONAL_ACTION + DERIVATIVE_ACTION * (error - lastError);
    // Keep the current error.
    lastError = error;
    //------------------------------------------------------------------------//
    // Get individual motor speeds.  The sign of speedDifference
    // determines if the robot turns left or right.
    //------------------------------------------------------------------------//
    int m1Speed = NORMAL_SPEED + speedDifference;
    int m2Speed = NORMAL_SPEED - speedDifference;
    //------------------------------------------------------------------------//
    // Here we constrain our motor speeds to be between 0 and MAX_SPEED.
    // Generally speaking, one motor will always be turning at MAX_SPEED
    // and the other will be at MAX_SPEED-|speedDifference| if that is
    // positive, else it will be stationary.  For some applications, you
    // might want to allow the motor speed to go negative so that it can spin
    // in reverse.
    //------------------------------------------------------------------------//
    if (m1Speed < 0)
        m1Speed = 0;
    if (m2Speed < 0)
        m2Speed = 0;
    if (m1Speed > MAX_SPEED)
        m1Speed = MAX_SPEED;
    if (m2Speed > MAX_SPEED)
        m2Speed = MAX_SPEED;
    // Set the speeds.
    setSpeeds(m2Speed, m1Speed);
}

bool isLine(unsigned int sensorValue){
  return sensorValue > EPSILON_LINE;
}

bool isThereIntersection(unsigned int sensorValues[]){
    int sensors=0;
    for (int i = 0; i < NUM_SENSORS; i++){
       if (isLine(sensorValues[i])){
          sensors++;
        }else if (i+1 < NUM_SENSORS && isLine(sensorValues[i+1]) && i-1>=0 && !isLine(sensorValues[i-1]) && sensors >0){
            //return true;        
          }
      }
    //*/
    if (sensors==6){
      return true;
    }
    return false;
}

bool move(int &direction){
    position = arraySensors.readLine(sensorValues);
    if ((isThereIntersection(sensorValues)) && direction == 0){
            setSpeeds(0, 0);
            return true;                                                              
    }
    if (position>2000 && position<3000 && isLine(sensorValues[2]) && isLine(sensorValues[3])&& !isLine(sensorValues[1])&& !isLine(sensorValues[4])){
        direction=0;
    }
    switch (direction){
                      case (1): 
                                position=5000;
                                for(int i=NUM_SENSORS-1;i>=0;i--){
                                        if(isLine(sensorValues[i])){
                                                position=i*1000;
                                                break;            
                                        }
                                }
                                break;
                      case (2): 
                                position=0;
                                for(int i=0;i<NUM_SENSORS;i++){
                                        if(isLine(sensorValues[i])){
                                                position=i*1000;
                                                break;            
                                        }
                                }
                                break;
      };
    followLine(position);
    return false;
}

void automaticCalibration() {
  unsigned long startTime = millis();
  unsigned long timeLimit = 300;
  for (int i = 0; i < 2; i++) {
    startTime = millis();
    setSpeeds(CALIBRATION_SPEED, -CALIBRATION_SPEED);
    while (millis() - startTime < timeLimit ) {
      arraySensors.calibrate();
    }
    setSpeeds(0, 0);
    delay(10);
    startTime = millis();
    setSpeeds(-CALIBRATION_SPEED, CALIBRATION_SPEED);
    while (millis() - startTime < timeLimit ) {
      arraySensors.calibrate();
    }
    setSpeeds(0, 0);
    delay(10);
    startTime = millis();
    setSpeeds(-CALIBRATION_SPEED, CALIBRATION_SPEED);
    while (millis() - startTime < timeLimit ) {
      arraySensors.calibrate();
    }
    setSpeeds(0, 0);
    delay(10);
    startTime = millis();
    setSpeeds(CALIBRATION_SPEED, -CALIBRATION_SPEED);
    while (millis() - startTime < timeLimit ) {
      arraySensors.calibrate();
    }
    setSpeeds(0, 0);
    delay(10);
    
  }

}

void automaticCalibration1() {
  for (int i = 0; i < 80; i++) {
    if ((i > 10 && i <= 30) || (i > 50 && i <= 70)) {
      setSpeeds(200, 200);
    } else {
      setSpeeds(-200, -200);
    }
    arraySensors.calibrate();
    //frontSensors.calibrate();
    // Since our counter runs to 80, the total delay will be
    // 80*20 = 1600 ms.
    delay(10);
  }
  setSpeeds(0, 0);
}

void manualCalibration() {
  unsigned long startTime = millis();
  unsigned long timeLimit = 1000;
  for (int i = 0; i < 10 ; i++) {
    startTime = millis();
    timeLimit = 1000;
    while (millis() - startTime < timeLimit ) {
      arraySensors.calibrate();
    }
    delay(10);
  }
}

bool acceptWorkMOVE(AIOState& state,PARAMETERS& parameters){
    Serial.println("[MOVE]:acceptWork");
    setSpeeds(0, 0);
    return true;
}

bool executeCommandMOVE(AIOState& state,CommunicationChannel*& communicationChannel,PARAMETERS& parameters){
    Serial.println("[MOVE]:executeCommand");
    //MobilityModuleState& _state = dynamic_cast<MobilityModuleState&>(state); //ERROR: 'dynamic_cast' not permitted with -fno-rtti
    if (mmstate.getMMState() == MMState::FINISHED){
        String d =((const char *)parameters["DIRECTION"]);//NONE, TURN_RIGHT,TURN_LEFT 
        direction = 0;
        if (d.equals("TURN_RIGHT")){
            direction = 1;
        }else if (d.equals("TURN_LEFT")){
            direction = 2;
        }
        mmstate.setMMState(MMState::FOLLOW_LINE);
    }
    if (move(direction)){
        mmstate.setMMState(MMState::FINISHED);
        return true;
    }
    return false;
}

bool abortCommandMOVE(AIOState& state,CommunicationChannel*& communicationChannel){
    Serial.println("[MOVE]:abortCommand");
    setSpeeds(0, 0);
    mmstate.setMMState(MMState::FINISHED);
    return true;
}
char* executeService(AIOState& state,CommunicationChannel*& communicationChannel){
    String mlvl = String("{\\\"BATTERY_LVL\\\":"); 
    double lvl =((double)analogRead(PIN_BATTERY_LVL)/1023)*100;
    mlvl +=lvl;
    mlvl +="}";
    char * clvl =  new char[mlvl.length() + 1 ];
    strcpy(clvl, mlvl.c_str());
    return clvl;
}

AIOActionCommand moveCommand("MOVE",*(new std::vector<char*>()),false,acceptWorkMOVE,executeCommandMOVE,abortCommandMOVE);
AIOServiceCommand batterySensorService("BATTERY_LVL",*(new std::vector<char*>()),false,executeService);

void setup() {
  Serial.begin(115200);
  Serial.println();
  leftServo.attach(LEFT_MOTOR);
  rigthServo.attach(RIGTH_MOTOR);
  pinMode(PIN_BATTERY_LVL,INPUT);
  setSpeeds(0,0);
  //manualCalibration();
  //automaticCalibration();
  //automaticCalibration1();
  commnuicationChannel.setDebugging(true);
  moveCommand.getParameters().push_back("DIRECTION");//NONE, TURN_RIGHT,TURN_LEFT 
  
  Serial.printf("size -> %d",moveCommand.getParameters().size());
  module.setState(mmstate);
  module.setCommunicationChannel(&commnuicationChannel);
  module.addCommand(moveCommand);
  //module.addCommand(batterySensorService);
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
