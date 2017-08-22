#include "AIOCommand.h"

AIOCommand::AIOCommand(char* name,std::vector<char*> params,bool interruptible,bool service){
    this->_name = name;
    this->_params = params;
    this->_interruptible = interruptible;
    this->_service = service;
}

char* AIOCommand::getName(){
    return this->_name;
}

void AIOCommand::setName(char* name){
    this->_name = name;
}

std::vector<char*>& AIOCommand::getParameters(){
    return this->_params;
}

void AIOCommand::setParameters(std::vector<char*>& params){
    this->_params = params;
}

bool AIOCommand::isInterruptible(){
    return this->_interruptible;
}

void AIOCommand::setInterruptible(bool interruptible){
    this->_interruptible = interruptible;
}

bool AIOCommand::isService(){
    return this->_service;
}

void AIOCommand::setService(bool service){
    this->_service = service;
}

AIOActionCommand::AIOActionCommand(char* name,std::vector<char*>& params,bool interruptible,bool service,std::function<bool (AIOState& state)> acceptWork,std::function<bool (AIOState& state,CommunicationChannel*& communicationChannel)> executeService)
                :AIOCommand(name,params,interruptible,service){
     this->_acceptWork = acceptWork;
     this->_executeService = executeService;
}
        

std::function<bool (AIOState& state)> AIOActionCommand::getIsAcceptWork(){
    return this->_acceptWork;
}

void AIOActionCommand::setAcceptWork(std::function<bool (AIOState& state)> acceptWork){
    this->_acceptWork = acceptWork;

}

std::function<bool (AIOState& state,CommunicationChannel*& communicationChannel)> AIOActionCommand::getExecuteService(){
    return this->_executeService;
}

void AIOActionCommand::setExecuteService(std::function<bool (AIOState& state,CommunicationChannel*& communicationChannel)> executeService){
    this->_executeService = executeService;
}


AIOServiceCommand::AIOServiceCommand(char* name,std::vector<char*> params,bool interruptible,bool service,std::function<char* (AIOState& state,CommunicationChannel*& communicationChannel)> executeService)
                                    :AIOCommand(name,params,interruptible,service){
    this->_executeService = executeService;
}

std::function<char* (AIOState& state,CommunicationChannel*& communicationChannel)> AIOServiceCommand::getExecuteService(){
    return this->_executeService;
}

void AIOServiceCommand::setExecuteService(std::function<char* (AIOState& state,CommunicationChannel*& communicationChannel)> executeService){
    this->_executeService = executeService;
}
