#ifndef __AIO_COMMAND_H__
#define __AIO_COMMAND_H__

#include <Arduino.h>
#include <functional>
#include <vector>
#include <ArduinoJson.h>
#include "../state/AIOState.h"
#include "../communicationChannel/CommunicationChannel.h"

#define PARAMETERS JsonObject
#define ACCEPT_WORK_FUNCTION std::function<bool (AIOState& state,PARAMETERS& parameters)>
#define EXECUTE_ACTION_COMMAND_FUNCTION std::function<bool (AIOState& state,CommunicationChannel*& communicationChannel,PARAMETERS& parameters)>
#define ABORT_ACTION_COMMAND_FUNCTION std::function<void (AIOState& state,CommunicationChannel*& communicationChannel)>
#define EXECUTE_SERVICE_FUNCTION std::function<char* (AIOState& state,CommunicationChannel*& communicationChannel)>


class AIOCommand {
    protected:
        char* _name;
        std::vector<char*>* _params;
        bool _interruptible;
    public:
         AIOCommand(char* name,std::vector<char*>& params,bool interruptible);
         char* getName();
         void setName(char* name);
         std::vector<char*>& getParameters();
         void setParameters(std::vector<char*>& params);
         bool isInterruptible();
         void setInterruptible(bool interruptible);
};

class AIOActionCommand: public AIOCommand{
    protected:
        ACCEPT_WORK_FUNCTION _acceptWork;
        EXECUTE_ACTION_COMMAND_FUNCTION _executeCommand;
        ABORT_ACTION_COMMAND_FUNCTION _abortCommand;
    public:
         AIOActionCommand(char* name,
                         std::vector<char*> &params,bool interruptible,
                         ACCEPT_WORK_FUNCTION acceptWork,
                         EXECUTE_ACTION_COMMAND_FUNCTION executeCommand,
                         ABORT_ACTION_COMMAND_FUNCTION abortCommand);
         ACCEPT_WORK_FUNCTION getIsAcceptWork();
         void setAcceptWork(ACCEPT_WORK_FUNCTION acceptWork);
         EXECUTE_ACTION_COMMAND_FUNCTION getExecuteCommand();
         void setExecuteCommand(EXECUTE_ACTION_COMMAND_FUNCTION executeCommand);
         ABORT_ACTION_COMMAND_FUNCTION getAbortCommand();
         void setAbortCommand(ABORT_ACTION_COMMAND_FUNCTION abortCommand);
         
};

class AIOServiceCommand: public AIOCommand{
    protected:
         EXECUTE_SERVICE_FUNCTION _executeService;
    public:
         AIOServiceCommand(char* name,std::vector<char*>& params,bool interruptible,EXECUTE_SERVICE_FUNCTION executeService);
         EXECUTE_SERVICE_FUNCTION getExecuteService();
         void setExecuteService(EXECUTE_SERVICE_FUNCTION executeService);        
};


#endif