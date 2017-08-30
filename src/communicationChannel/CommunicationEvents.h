#ifndef __COMMUNICATION_EVENTS_H__
#define __COMMUNICATION_EVENTS_H__

#include <Arduino.h>

namespace CommunicationEvents{
    enum CommunicationEventsTypes {
        REGISTRATION,
        REGISTRATION_REPLY,
        WORK_ASSIGNATION,
        WORK_ASSIGNATION_REPLY,
        WORK_STATUS,
        WORK_STATUS_REPLY,
        ABORT_WORK,
        ALL_BEGINS,
        ACTION_FINISHED,
        GET_SENSOR_SERVICES,
        GET_SENSOR_SERVICES_REPLY,
        SUBSCRIPTION_SENSOR_SERVICE,
        SUBSCRIPTION_SENSOR_SERVICE_REPLY,
        SENSOR_SERVICE,
        SENSOR_SERVICE_REPLY,
        EMOTIONAL_EVENT,
        PARTICIPATION_EVENT,
        NECESSITY_FOR_MODULE,
        FINISHED_EXECUTE_COMMAND,
        COMMANDS_ASSIGNATION,
        MODULE_REGISTRATION_SERVICE,
        POSSIBLE_SUPPLIER,
        NOT_DEFINED

        
    };
    char* toString(CommunicationEventsTypes c);

}
    
#endif