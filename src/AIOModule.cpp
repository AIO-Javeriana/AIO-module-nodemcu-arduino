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
    this->_debug = false;
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

void AIOModule::_onABORT_WORK(const char * payload, size_t length){
    AIOModule::getInstance().onABORT_WORK(payload,length);
}

void AIOModule::_onALL_BEGINS(const char * payload, size_t length){
    AIOModule::getInstance().onALL_BEGINS(payload,length);
}

void AIOModule::onREGISTRATION_REPLY(const char * payload, size_t length){
    if (this->_debug)
        PRINT("REGISTRATION_REPLY: %s\n", payload);
}

void AIOModule::onWORK_ASSIGNATION(const char * payload, size_t length){
    String replayJSON = String(payload);
    replayJSON.replace("\\","");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& data = jsonBuffer.parseObject(replayJSON.c_str());
    if (this->_debug)
        PRINT("WORK_ASSIGNATION: %s\n", payload);
    if (!data.success()){
        PRINT("ERROR:[WORK_ASSIGNATION] parseObject() failed:\n %s\n",payload);
        return;
    }
    GROUP_ID group_id = ((const char*)data["GROUP_ID"]);
    COMMAND_ID command_id = ((const char*)data["COMMAND_ID"]);
    COMMAND_NAME command_name =((const char*)data["COMMAND"]);
    PARAMETERS& parameters =(data["PARAMS"]);
    replayJSON = String("{\\\"MODULE_ID\\\":\\\"");
    replayJSON += this->_state.getModule_id();
    replayJSON += "\\\",\\\"EVENT_NAME\\\":\\\"";
    replayJSON += toString(CommunicationEventsTypes::WORK_ASSIGNATION_REPLY);
    replayJSON += "\\\",\\\"COMMAND_ID\\\":";
    replayJSON += command_id;
    replayJSON += ",\\\"GROUP_ID\\\":";
    replayJSON += group_id;
    replayJSON += ",\\\"REPLY\\\":\\\"";
    std::map<COMMAND_NAME, AIOActionCommand* >::iterator it = this->_actions.find(command_name);
    if (it != this->_actions.end()){
        if (it->second->getIsAcceptWork()(this->_state,parameters)){
            replayJSON +="ACCEPTED";
            this->_actionInQueue[group_id].push_back(std::pair<COMMAND_ID,COMMAND_INFO >(command_id,COMMAND_INFO(it->second,&parameters)));
        } else {
            replayJSON +="REFUSE";
        }
    } else {
        replayJSON +="REFUSE";
    }
    replayJSON += "\\\"}";
    this->_communicationChannel->emitEvent(toString(CommunicationEventsTypes::WORK_ASSIGNATION_REPLY),replayJSON.c_str());
}

void AIOModule::onWORK_STATUS(const char * payload, size_t length){
    String replayJSON = String(payload);
    replayJSON.replace("\\","");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& data = jsonBuffer.parseObject(replayJSON.c_str());
    if (this->_debug)
        PRINT("WORK_STATUS: %s\n", payload);
    if (!data.success()){
        PRINT("ERROR:[WORK_STATUS] parseObject() failed:\n %s\n",payload);
        return;
    }
    GROUP_ID group_id = ((const char*)data["GROUP_ID"]);
    COMMAND_ID command_id = ((const char*)data["COMMAND_ID"]);
    replayJSON = String("{\\\"MODULE_ID\\\":\\\"");
    replayJSON += this->_state.getModule_id();
    replayJSON += "\\\",\\\"EVENT_NAME\\\":\\\"";
    replayJSON += toString(CommunicationEventsTypes::WORK_STATUS_REPLY);
    replayJSON += "\\\",\\\"COMMAND_ID\\\":";
    replayJSON += command_id;
    replayJSON += ",\\\"GROUP_ID\\\":";
    replayJSON += group_id;
    replayJSON += ",\\\"STATUS\\\":\\\"";
    std::map<COMMAND_ID, COMMAND_INFO >::iterator it = this->_actionInExecution.find(command_id);
    if (it != this->_actionInExecution.end()){
        replayJSON +="WORKING";
    } else {
        replayJSON +="ERROR";
    }
    replayJSON += "\\\"}";
    this->_communicationChannel->emitEvent(toString(CommunicationEventsTypes::WORK_STATUS_REPLY),replayJSON.c_str());
}

void AIOModule::onABORT_WORK(const char * payload, size_t length){
    String replayJSON = String(payload);
    replayJSON.replace("\\","");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& data = jsonBuffer.parseObject(replayJSON.c_str());
    if (this->_debug)
        PRINT("ABORT_WORK: %s\n", payload);
    if (!data.success()){
        PRINT("ERROR:[ABORT_WORK] parseObject() failed:\n %s\n",payload);
        return;
    }
    GROUP_ID group_id = ((const char*)data["GROUP_ID"]);
    COMMAND_ID command_id = ((const char*)data["COMMAND_ID"]);
    std::map<COMMAND_ID, COMMAND_INFO >::iterator it = this->_actionInExecution.find(command_id);
    if (it != this->_actionInExecution.end()){
        it->second.first->getAbortCommand()(this->_state,this->_communicationChannel);
        this->_actionInExecution.erase (it); 
    }
}

void AIOModule::onALL_BEGINS(const char * payload, size_t length){
    String replayJSON = String(payload);
    replayJSON.replace("\\","");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& data = jsonBuffer.parseObject(replayJSON.c_str());
    if (this->_debug)
        PRINT("ALL_BEGINS: %s\n", payload);
    if (!data.success()){
        PRINT("ERROR:[ALL_BEGINS] parseObject() failed:\n %s\n",payload);
        return;
    }
    GROUP_ID group_id = ((const char*)data["GROUP_ID"]);
    std::map<GROUP_ID, std::vector<std::pair<COMMAND_ID, COMMAND_INFO > > >::iterator it = this->_actionInQueue.find((group_id));
    if (it != this->_actionInQueue.end()){
        for (int i = 0; i < it->second.size();++i ){
            this->_actionInExecution[it->second[i].first]=it->second[i].second;
        }
    }
}

/*
{"MODULE_ID":"NOMBRE_DEL_MODULO",
 "EVENT_NAME":"REGISTRATION",
 "COMMANDS":[
            {"COMMAND":"NOMBRE_DEL_COMANDO",
            "INTERRUPTIBLE":false,
            "SERVICE":false,
            "PARAMS":["PARAM1","PARAM2"]
            },
            ]
}//*/
void AIOModule::setup(){
    String sJSON = String("{\\\"MODULE_ID\\\":\\\"");
    sJSON += this->_state.getModule_id();
    sJSON += "\\\",\\\"EVENT_NAME\\\":\\\"";
    sJSON += toString(CommunicationEventsTypes::REGISTRATION);
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
        for (int i = 0;i < it->second->getParameters().size();++i ){
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
        sJSON += "\\\"SERVICE\\\":true,";
        sJSON += "\\\"PARAMS\\\":[";
        for (int i = 0;i < it->second->getParameters().size();++i ){
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
    for (std::map<COMMAND_ID, COMMAND_INFO >::iterator it = this->_actionInExecution.begin(); it != this->_actionInExecution.end();){
        if (it->second.first->getExecuteCommand()(this->_state,this->_communicationChannel,*(it->second.second))){
            String replayJSON = String("{\\\"MODULE_ID\\\":\\\"");
            replayJSON += this->_state.getModule_id();
            replayJSON += "\\\",\\\"EVENT_NAME\\\":\\\"";
            replayJSON += toString(CommunicationEventsTypes::ACTION_FINISHED);
            replayJSON += "\\\",\\\"COMMAND_ID\\\":";
            replayJSON += it->first;
            replayJSON += ",\\\"STATUS\\\":\\\"DONE\\\",\\\"FINISH_MESSAGE\\\":\\\"DONE\\\",\\\"ERROR_MESSAGE\\\":\\\"\\\"}";
            this->_communicationChannel->emitEvent(toString(CommunicationEventsTypes::ACTION_FINISHED),replayJSON.c_str());        
            this->_actionInExecution.erase(it++);
        }else{
            ++it;
        }
    }
    for (std::map<COMMAND_NAME, AIOServiceCommand*>::iterator it = this->_services.begin();it != this->_services.end();++it){
        String replayJSON = String("{\\\"MODULE_ID\\\":\\\"");
        replayJSON += this->_state.getModule_id();
        replayJSON += "\\\",\\\"EVENT_NAME\\\":\\\"";
        replayJSON += toString(CommunicationEventsTypes::SENSOR_SERVICE);
        replayJSON += "\\\",\\\"SENSOR_SERVICE_NAME\\\":\\\"";
        replayJSON += it->first;
        replayJSON += "\\\",\\\"DATA\\\":";
        replayJSON += it->second->getExecuteService()(this->_state,this->_communicationChannel);
        replayJSON += "}";
        this->_communicationChannel->emitEvent(toString(CommunicationEventsTypes::SENSOR_SERVICE),replayJSON.c_str());        
    }
    
}

bool AIOModule::isDebugging(){
    return this->_debug;
}

void AIOModule::setDebugging(bool debug){
    this->_debug =  debug;
}

void AIOModule::addCommand(AIOActionCommand& command){
        this->_actions[command.getName()] = &command;
}

void AIOModule::addCommand(AIOServiceCommand& command){
        this->_services[command.getName()] = &command;
}


        
