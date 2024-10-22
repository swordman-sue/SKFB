#ifndef LOCATION_TOOLS_H_
#define LOCATION_TOOLS_H_

#include "zqcommon/typedef.h"
#include <string>
#include "luaengine/luaengine.h"

class LocationTools
{
public:
    static void StartLocation(const int timeout);
    static void StopLocation();
};
#endif
