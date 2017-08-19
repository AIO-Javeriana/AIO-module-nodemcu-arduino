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
        ALL_BEGINS,
        ACTION_FINISHED,
        GET_SENSOR_SERVICES,
        SUBSCRIPTION_SENSOR_SERVICE,
        SENSOR_SERVICE,
        EMOTIONAL_EVENT,
        PARTICIPATION_EVENT,
        SUBSCRIPTION_SENSOR_SERVICE_REPLY,
        NECESSITY_FOR_MODULE,
        FINISHED_EXECUTE_COMMAND,
        SENSOR_SERVICE_REPLY,
        GET_SENSOR_SERVICES_REPLY,
        COMMANDS_ASSIGNATION,
        NOT_DEFINED
    };
    char* toString(CommunicationEventsTypes c);

}
    
#endif