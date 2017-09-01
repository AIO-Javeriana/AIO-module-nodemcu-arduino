#ifndef __AIO_STATE_H__
#define __AIO_STATE_H__

#include <Arduino.h>

class AIOState {
    protected:
        char* _module_id;
    public:
         AIOState(char* module_id);
         char* getModule_id();
         void setModule_id(char* module_id); 
         virtual ~AIOState();     
};

#endif