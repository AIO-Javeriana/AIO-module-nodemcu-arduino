#ifndef __SIO_COMMUNICATION_CHANNEL_H__
#define __SIO_COMMUNICATION_CHANNEL_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <string.h>
#include "../communicationChannel/CommunicationChannel.h"
#include "../communicationChannel/CommunicationEvents.h"
#include "SocketIO_Client.h"


#define SocketIO_Client SocketIO_Client
#define PRINT(...) Serial.printf(__VA_ARGS__);

using namespace CommunicationEvents;

class SIOCommnuicationChannel: public CommunicationChannel{
    private:
        SocketIO_Client _webSocket;
        char* _host;
        int _port;
        
        static void _onDisconnected(const char * payload, size_t length);
        void onDisconnected(const char * payload, size_t length);
        SIOCommnuicationChannel();
        //~SIOCommnuicationChannel(){}
    
    public:
        static SIOCommnuicationChannel& getInstance();
        static SIOCommnuicationChannel& getInstance(char * host,int port);
        
        bool syncWithAIO();
        bool syncWithAIO(int port);
        
        void setup(const char * module_info,
                    std::function < void (const char * payload, size_t length)> onREGISTRATION_REPLY,
                    std::function < void (const char * payload, size_t length)> onWORK_ASSIGNATION,
                    std::function < void (const char * payload, size_t length)> onWORK_STATUS,
                    std::function < void (const char * payload, size_t length)> onALL_BEGINS
                    );
        void loop();
        bool isConnected();
        void emitEvent(const char* event, const char * msg);

        char* getHost();
        int   getPort();
        void  setHost(char * host);
        void  setPort(int port);
        bool isDebugging();
	    void setDebugging(bool debug);
    
};

#endif