#include "CommunicationEvents.h"

char* CommunicationEvents::toString(CommunicationEvents::CommunicationEventsTypes c){
    switch(c){
        case REGISTRATION: return ("REGISTRATION");
        case REGISTRATION_REPLY: return("REGISTRATION_REPLY");
        case WORK_ASSIGNATION:  return("WORK_ASSIGNATION");
        case WORK_ASSIGNATION_REPLY: return ("WORK_ASSIGNATION_REPLY");
        case WORK_STATUS: return ("WORK_STATUS");
        case WORK_STATUS_REPLY: return ("WORK_STATUS_REPLY");
        case ABORT_WORK: return ("ABORT_WORK");
        case ALL_BEGINS: return ("ALL_BEGINS");
        case ACTION_FINISHED: return ("ACTION_FINISHED");
        case GET_SENSOR_SERVICES: return ("GET_SENSOR_SERVICES");
        case GET_SENSOR_SERVICES_REPLY: return ("GET_SENSOR_SERVICES_REPLY");
        case SUBSCRIPTION_SENSOR_SERVICE: return ("SUBSCRIPTION_SENSOR_SERVICE");
        case SUBSCRIPTION_SENSOR_SERVICE_REPLY: return ("SUBSCRIPTION_SENSOR_SERVICE_REPLY");
        case SENSOR_SERVICE: return ("SENSOR_SERVICE");
        case SENSOR_SERVICE_REPLY: return ("SENSOR_SERVICE_REPLY");
        case EMOTIONAL_EVENT: return ("EMOTIONAL_EVENT");
        case PARTICIPATION_EVENT: return ("PARTICIPATION_EVENT");
        case NECESSITY_FOR_MODULE: return ("NECESSITY_FOR_MODULE");
        case FINISHED_EXECUTE_COMMAND:  return ("FINISHED_EXECUTE_COMMAND");
        case COMMANDS_ASSIGNATION: return ("COMMANDS_ASSIGNATION");
        case MODULE_REGISTRATION_SERVICE: return ("MODULE_REGISTRATION_SERVICE");
        case POSSIBLE_SUPPLIER: return ("POSSIBLE_SUPPLIER");
        case NOT_DEFINED: return ("NOT_DEFINED");
        default : return  "NOT_DEFINED";
    }
    return "";
}
    