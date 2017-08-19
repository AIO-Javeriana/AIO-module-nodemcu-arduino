#ifndef __AIO_COMMAND_H__
#define __AIO_COMMAND_H__

#include <Arduino.h>
#include <functional>
#include <vector>
#include "AIOState.h"
#include "CommunicationChannel.h"

class AIOCommand {
    protected:
        char* _name;
        std::vector<char*> _params;
        bool _interruptible;
        bool _service;
    public:
         AIOCommand(char* name,std::vector<char*> params,bool interruptible,bool service);
         char* getName();
         void setName(char* name);
         std::vector<char*>& getParameters();
         void setParameters(std::vector<char*>& params);
         bool isInterruptible();
         void setInterruptible(bool interruptible);
         bool isService();
         void setService(bool service);
};

class AIOActionCommand: public AIOCommand{
    protected:
        std::function<bool (AIOState& state)> _acceptWork;
        std::function<bool (AIOState& state,CommunicationChannel*& communicationChannel)> _executeService;
    public:
         AIOActionCommand(char* name,std::vector<char*> params,bool interruptible,bool service,std::function<bool (AIOState& state)> acceptWork,std::function<bool (AIOState& state,CommunicationChannel*& communicationChannel)> executeService);
         std::function<bool (AIOState& state)> getAcceptWork();
         void setAcceptWork(std::function<bool (AIOState& state)> acceptWork);
         std::function<bool (AIOState& state,CommunicationChannel*& communicationChannel)> getESxecuteService();
         void setExecuteService(std::function<bool (AIOState& state,CommunicationChannel*& communicationChannel)> executeService);
};

class AIOServiceCommand: public AIOCommand{
    protected:
        std::function<char* (AIOState& state,CommunicationChannel*& communicationChannel)> _executeService;
    public:
         AIOServiceCommand(char* name,std::vector<char*> params,bool interruptible,bool service,std::function<char* (AIOState& state,CommunicationChannel*& communicationChannel)> executeService);
         std::function<char* (AIOState& state,CommunicationChannel*& communicationChannel)> getESxecuteService();
         void setExecuteService(std::function<char* (AIOState& state,CommunicationChannel*& communicationChannel)> executeService);        
};


#endif