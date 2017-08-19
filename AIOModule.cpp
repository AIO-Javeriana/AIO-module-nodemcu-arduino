#include "AIOModule.h"

AIOModule& AIOModule::getInstance(){
    static AIOModule _module;
    return _module;
}

AIOModule& AIOModule::getInstance(AIOState& state,CommunicationChannel* const& communicationChannel){
    AIOModule& _module = AIOModule::getInstance();
    _module.setState(state);
    _module.setCommunicationChannel(communicationChannel);
    return _module;
}
        

AIOModule::AIOModule():_state("ARDUINO-MODULE"){

}

AIOState AIOModule::getState(){
    return this->_state;
}
void AIOModule::setState(AIOState& state){
    this->_state = state;
}

CommunicationChannel* AIOModule::getCommunicationChannel(){
    return this->_communicationChannel;
}
void AIOModule::setCommunicationChannel(CommunicationChannel* const& communicationChannel){
    this->_communicationChannel = communicationChannel;
}

void AIOModule::_onREGISTRATION_REPLY(const char * payload, size_t length){
    AIOModule::getInstance().onREGISTRATION_REPLY(payload,length);
}

void AIOModule::_onWORK_ASSIGNATION(const char * payload, size_t length){
    AIOModule::getInstance().onWORK_ASSIGNATION(payload,length);
}

void AIOModule::_onWORK_STATUS(const char * payload, size_t length){
    AIOModule::getInstance().onWORK_STATUS(payload,length);
}

void AIOModule::_onALL_BEGINS(const char * payload, size_t length){
    AIOModule::getInstance().onALL_BEGINS(payload,length);
}

void AIOModule::onREGISTRATION_REPLY(const char * payload, size_t length){
    PRINT("REGISTRATION_REPLY: %s\n", payload);
}

void AIOModule::onWORK_ASSIGNATION(const char * payload, size_t length){
    PRINT("WORK_ASSIGNATION: %s\n", payload);
    



}

void AIOModule::onWORK_STATUS(const char * payload, size_t length){
    PRINT("WORK_STATUS: %s\n", payload);
}

void AIOModule::onALL_BEGINS(const char * payload, size_t length){
    PRINT("ALL_BEGINS: %s\n", payload);
}

void AIOModule::setup(){
    String sJSON = String("{\\\"MODULE_ID\\\":\\\"");
    sJSON += this->_state.getModule_id();
    sJSON += "\\\",\\\"COMMANDS\\\":[";
    for (std::map<COMMAND_NAME, AIOActionCommand*>::iterator it = this->_actions.begin();it != this->_actions.end();++it){
        sJSON += "{\\\"COMMAND\\\":\\\"";
        sJSON += it->first;
        sJSON += "\\\",";
        if(it->second->isInterruptible()){
            sJSON += "\\\"INTERRUPTIBLE\\\":true,";
        }else{
            sJSON += "\\\"INTERRUPTIBLE\\\":false,";
        }
        sJSON += "\\\"SERVICE\\\":false,";
        sJSON += "\\\"PARAMS\\\":[";
        for (int i = 0;i < it->second->getParameters().size();i++ ){
            sJSON += "\\\"";
            sJSON += it->second->getParameters()[i];
            sJSON += "\\\",";
        }    
        sJSON += "]},";
                   
    }
    for (std::map<COMMAND_NAME, AIOServiceCommand*>::iterator it = this->_services.begin();it != this->_services.end();++it){
        sJSON += "{\\\"COMMAND\\\":\\\"";
        sJSON += it->first;
        sJSON += "\\\",";
        if(it->second->isInterruptible()){
            sJSON += "\\\"INTERRUPTIBLE\\\":true,";
        }else{
            sJSON += "\\\"INTERRUPTIBLE\\\":false,";
        }
        sJSON += "\\\"SERVICE\\\":false,";
        sJSON += "\\\"PARAMS\\\":[";
        for (int i = 0;i < it->second->getParameters().size();i++ ){
            sJSON += "\\\"";
            sJSON += it->second->getParameters()[i];
            sJSON += "\\\",";
        }    
        sJSON += "]},";
    }
    sJSON += "]}";
    this->_communicationChannel->setup(sJSON.c_str(),
                                        _onREGISTRATION_REPLY,
                                        _onWORK_ASSIGNATION,
                                        _onWORK_STATUS,
                                        _onALL_BEGINS
                                        );
    //*/
}   
void AIOModule::loop(){
    
    this->_communicationChannel->loop();

}

void AIOModule::addCommand(AIOActionCommand& command){
        this->_actions[command.getName()] = &command;
}

void AIOModule::addCommand(AIOServiceCommand& command){
        this->_services[command.getName()] = &command;
}

