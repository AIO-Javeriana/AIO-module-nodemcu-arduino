#ifndef __AIO_MODULE_ARDUINO_H__
#define __AIO_MODULE_ARDUINO_H__

#include <Arduino.h>
#include <functional>
#include <memory>
#include <vector>
#include <map>
#include <ArduinoJson.h>

#include "AIOState.h"
#include "AIOCommand.h"
#include "CommunicationChannel.h"

#define PRINT(...) Serial.printf(__VA_ARGS__);
#define COMMAND_NAME char*
#define GROUP_ID char*
#define COMMAND_ID char*

class AIOModule {
    private:
        AIOState _state;
        CommunicationChannel* _communicationChannel;
        StaticJsonBuffer<200> _jsonBuffer;
        std::map<COMMAND_NAME, AIOActionCommand*> _actions;
        std::map<COMMAND_NAME, AIOServiceCommand*> _services;
        std::map<GROUP_ID, std::vector<std::pair<COMMAND_ID, AIOActionCommand*> > > _actionInQueue;
        std::map<COMMAND_ID, AIOActionCommand*> _actionInExecution;

        static void _onREGISTRATION_REPLY(const char * payload, size_t length);
        static void _onWORK_ASSIGNATION(const char * payload, size_t length);
        static void _onWORK_STATUS(const char * payload, size_t length);
        static void _onALL_BEGINS(const char * payload, size_t length);
        
        void onREGISTRATION_REPLY(const char * payload, size_t length);
        void onWORK_ASSIGNATION(const char * payload, size_t length);
        void onWORK_STATUS(const char * payload, size_t length);
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
        void addCommand(AIOActionCommand& command);
        void addCommand(AIOServiceCommand& command);
};

#endif