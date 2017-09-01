#include "AIOState.h"

AIOState::AIOState(char* module_id){
    this->_module_id = module_id;
}

char* AIOState::getModule_id(){
    return this->_module_id;
}

void AIOState::setModule_id(char* module_id){
    this->_module_id =  module_id;
}

AIOState::~AIOState(){
    
}