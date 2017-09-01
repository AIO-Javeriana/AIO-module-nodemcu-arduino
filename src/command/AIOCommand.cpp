#include "AIOCommand.h"

AIOCommand::AIOCommand(char* name,std::vector<char*>& params,bool interruptible){
    this->_name = name;
    this->_params = &params;
    this->_interruptible = interruptible;
}

char* AIOCommand::getName(){
    return this->_name;
}

void AIOCommand::setName(char* name){
    this->_name = name;
}

std::vector<char*>& AIOCommand::getParameters(){
    return *this->_params;
}

void AIOCommand::setParameters(std::vector<char*>& params){
    this->_params = &params;
}

bool AIOCommand::isInterruptible(){
    return this->_interruptible;
}

void AIOCommand::setInterruptible(bool interruptible){
    this->_interruptible = interruptible;
}

AIOActionCommand::AIOActionCommand(char* name,std::vector<char*>& params,bool interruptible,ACCEPT_WORK_FUNCTION acceptWork, EXECUTE_ACTION_COMMAND_FUNCTION executeCommand, ABORT_ACTION_COMMAND_FUNCTION abortCommand)
                :AIOCommand(name,params,interruptible){
     this->_acceptWork = acceptWork;
     this->_executeCommand = executeCommand;
     this->_abortCommand = abortCommand;
}
        

ACCEPT_WORK_FUNCTION AIOActionCommand::getIsAcceptWork(){
    return this->_acceptWork;
}

void AIOActionCommand::setAcceptWork(ACCEPT_WORK_FUNCTION acceptWork){
    this->_acceptWork = acceptWork;

}

EXECUTE_ACTION_COMMAND_FUNCTION AIOActionCommand::getExecuteCommand(){
    return this->_executeCommand;
}

void AIOActionCommand::setExecuteCommand(EXECUTE_ACTION_COMMAND_FUNCTION executeCommand){
    this->_executeCommand = executeCommand;
}

ABORT_ACTION_COMMAND_FUNCTION AIOActionCommand::getAbortCommand(){
    return this->_abortCommand;
}

void AIOActionCommand::setAbortCommand(ABORT_ACTION_COMMAND_FUNCTION abortCommand){
    this->_abortCommand =  abortCommand;
}
         

AIOServiceCommand::AIOServiceCommand(char* name,std::vector<char*>& params,bool interruptible,EXECUTE_SERVICE_FUNCTION executeService)
                                    :AIOCommand(name,params,interruptible){
    this->_executeService = executeService;
}

EXECUTE_SERVICE_FUNCTION AIOServiceCommand::getExecuteService(){
    return this->_executeService;
}

void AIOServiceCommand::setExecuteService(EXECUTE_SERVICE_FUNCTION executeService){
    this->_executeService = executeService;
}
