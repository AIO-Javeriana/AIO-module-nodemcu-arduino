#ifndef __AIO_MODULE_ARDUINO_H__
#define __AIO_MODULE_ARDUINO_H__

#include <Arduino.h>
#include <functional>
#include <memory>
#include <vector>
#include <map>
#include <ArduinoJson.h>

#include "./state/AIOState.h"
#include "./command/AIOCommand.h"
#include "./communicationChannel/CommunicationChannel.h"
#include "./communicationChannel/CommunicationEvents.h"
#include "./socketIOClient/SIOCommnuicationChannel.h"

#define PRINT(...) Serial.printf(__VA_ARGS__);
#define PRINTLN(...) Serial.println(__VA_ARGS__);
#define COMMAND_NAME String
#define GROUP_ID String
#define COMMAND_ID String
#define PARAMETERS JsonObject
#define BUFFER_CAPACITY 500
#define COMMAND_INFO std::pair<AIOActionCommand*,PARAMETERS*>
using namespace CommunicationEvents;

class AIOModule {
    private:
        AIOState _state;
        CommunicationChannel* _communicationChannel;
        std::map<COMMAND_NAME, AIOActionCommand*> _actions;
        std::map<COMMAND_NAME, AIOServiceCommand*> _services;
        std::map<GROUP_ID, std::vector<std::pair<COMMAND_ID, COMMAND_INFO > > > _actionInQueue;
        std::map<COMMAND_ID, COMMAND_INFO > _actionInExecution;
        bool _debug;
        static void _onREGISTRATION_REPLY(const char * payload, size_t length);
        static void _onWORK_ASSIGNATION(const char * payload, size_t length);
        static void _onWORK_STATUS(const char * payload, size_t length);
        static void _onABORT_WORK(const char * payload, size_t length);
        static void _onALL_BEGINS(const char * payload, size_t length);
        
        void onREGISTRATION_REPLY(const char * payload, size_t length);
        void onWORK_ASSIGNATION(const char * payload, size_t length);
        void onWORK_STATUS(const char * payload, size_t length);
        void onABORT_WORK(const char * payload, size_t length);
        void onALL_BEGINS(const char * payload, size_t length);
        AIOModule();
        
    public:
        static AIOModule& getInstance();
        static AIOModule& getInstance(AIOState& state,CommunicationChannel* const& communicationChannel);
        void setup();
        void loop();
        
        AIOState getState();
        void setState(AIOState& state);
        CommunicationChannel* getCommunicationChannel();
        void setCommunicationChannel(CommunicationChannel* const&  communicationChannel);
        bool isDebugging();
        void setDebugging(bool debug);
        void addCommand(AIOActionCommand& command);
        void addCommand(AIOServiceCommand& command);
};

#endif