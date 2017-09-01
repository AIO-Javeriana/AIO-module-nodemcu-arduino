#include "SIOCommnuicationChannel.h"


SIOCommnuicationChannel& SIOCommnuicationChannel::getInstance(){
    static SIOCommnuicationChannel _communicationChannel;
    return _communicationChannel;
}

SIOCommnuicationChannel& SIOCommnuicationChannel::getInstance(char * host,int port){
    SIOCommnuicationChannel& _communicationChannel = SIOCommnuicationChannel::getInstance();
    _communicationChannel.setHost(host);
    _communicationChannel.setPort(port);
    return _communicationChannel;
}
bool SIOCommnuicationChannel::syncWithAIO(){
    return this->syncWithAIO(9090);
}

bool SIOCommnuicationChannel::syncWithAIO(int port){
    WiFiUDP udp;
    udp.begin(1003);
    udp.beginMulticast(WiFi.localIP(), IPAddress(233,255,255,250),1003 );
    unsigned long startTime = millis();
    unsigned long timeLimit = 20000;
    while (millis() - startTime < timeLimit ) {
            if (udp.parsePacket()){
                this->setHost((char *)udp.remoteIP().toString().c_str());
                this->setPort(port);
                udp.stop();
                udp.stopAll();
                return true;
            }
    }
    udp.stop();
    udp.stopAll();
    return false;
}

SIOCommnuicationChannel::SIOCommnuicationChannel(void){

}

void SIOCommnuicationChannel::_onDisconnected(const char * payload, size_t length){
    SIOCommnuicationChannel::getInstance().onDisconnected(payload,length);
}

void SIOCommnuicationChannel::onDisconnected(const char * payload, size_t length){

}

void SIOCommnuicationChannel::setup(const char * module_info,
                    std::function < void (const char * payload, size_t length)> onREGISTRATION_REPLY,
                    std::function < void (const char * payload, size_t length)> onWORK_ASSIGNATION,
                    std::function < void (const char * payload, size_t length)> onWORK_STATUS,
                    std::function < void (const char * payload, size_t length)> onALL_BEGINS
                    ){
    this->_webSocket.on("Disconnected", _onDisconnected);
    this->_webSocket.on(toString(CommunicationEventsTypes::REGISTRATION_REPLY),onREGISTRATION_REPLY );
    this->_webSocket.on(toString(CommunicationEventsTypes::WORK_ASSIGNATION),onWORK_ASSIGNATION );
    this->_webSocket.on(toString(CommunicationEventsTypes::WORK_STATUS),onWORK_STATUS );
    this->_webSocket.on(toString(CommunicationEvents::ALL_BEGINS),onALL_BEGINS );
    this->_webSocket.begin(this->_host, this->_port, "/socket.io/?transport=websocket");
    this->_webSocket.emit(toString(CommunicationEvents::REGISTRATION),module_info);
}

void SIOCommnuicationChannel::loop(){
    this->_webSocket.loop();
}

bool SIOCommnuicationChannel::isConnected(){
    return this->_webSocket.isConnected();
}

void SIOCommnuicationChannel::emitEvent(const char* event, const char * msg){
    this->_webSocket.emit(event,msg);
}

char* SIOCommnuicationChannel::getHost(){
    return this->_host;
}

int   SIOCommnuicationChannel::getPort(){
    return this->_port;
}

void  SIOCommnuicationChannel::setHost(char * host){
    this->_host = new char[strlen(host)];
    strcpy(this->_host,host);
}

void  SIOCommnuicationChannel::setPort(int port){
    this->_port = port;
}

bool SIOCommnuicationChannel::isDebugging(){
    return this->_webSocket.isDebugging();
}

void SIOCommnuicationChannel::setDebugging(bool debug){
    this->_webSocket.setDebugging(debug);
}
