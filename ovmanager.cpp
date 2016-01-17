#include "ovmanager.h"

std::auto_ptr<OvManager> OvManager::_inst;
OvManager *OvManager::Instance(){
    if (!_inst.get())
        _inst.reset(new OvManager());
    return _inst.get();
}

void OvManager::cleanup()
{
    if (_inst.get() != NULL)
        delete _inst.release();
}

OvManager::OvManager():
    seeded(false) {
}

void OvManager::doSeed(){
    if (!seeded){
        srand(time(NULL));
        seeded = true;
    }
}

OvManager::~OvManager(){
    /*if (_th_oldip.get() != NULL){
        if (_th_oldip->isRunning())
            _th_oldip->terminate();
        delete _th_oldip.get();
    }*/
}

uint64_t OvManager::getRnd64(){
    doSeed();
    uint64_t v = 0
        | ((uint64_t)rand() << 49)
        | ((uint64_t)rand() << 34)
        | ((uint64_t)rand() << 19)
        | ((uint64_t)rand() & 0xf);
    return v;
}


