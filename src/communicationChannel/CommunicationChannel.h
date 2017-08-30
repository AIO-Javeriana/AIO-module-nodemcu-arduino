#ifndef __COMMUNICATION_CHANNEL_H__
#define __COMMUNICATION_CHANNEL_H__

#include <Arduino.h>
#include <functional>

class CommunicationChannel {
    private:

    public:
        
        virtual void setup(const char * module_info,
                            std::function < void (const char * payload, size_t length)> onREGISTRATION_REPLY,
                            std::function < void (const char * payload, size_t length)> onWORK_ASSIGNATION,
                            std::function < void (const char * payload, size_t length)> onWORK_STATUS,
                            std::function < void (const char * payload, size_t length)> onALL_BEGINS
                            )= 0;
        virtual void loop() = 0;
        virtual bool isConnected() = 0;
        virtual void emitEvent(const char* event, const char * msg) = 0; 
        //*/
};

#endif