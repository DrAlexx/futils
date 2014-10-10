#pragma once

#include <stdint.h>

namespace COM {

enum ErrorCode{
    UNKNOWN = 0,
    NO_INTERFACE = 1
};

class HResult
{
public:
   inline HResult()
       :value(0){}

   inline HResult(int32_t err_code){
        value = err_code;
        value |= (31<<1);
    }

    inline bool fail(){
        return (value|(1<<31)) == 0? false : true;
    }

    inline bool success(){
        return !fail();
    }

    inline int32_t code(){
        return value & ~(1<<31);
    }

private:
    int32_t value;
};

}
