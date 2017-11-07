//
// Created by 汤申亮 on 17/4/11.
//

#ifndef CONTROLLER_SERVICE_JOYSTICKDATABASE_H
#define CONTROLLER_SERVICE_JOYSTICKDATABASE_H

#include <stdint.h>

class JoystickDataBase {

public:
    virtual ~JoystickDataBase() {};
    
protected:
    void ResetData(char* bytes);
    int32_t ReadInt(int32_t nBits, bool bSignBit);

private:
    char* buffer;
    int32_t nPosition;
};


#endif //CONTROLLER_SERVICE_JOYSTICKDATA_H
