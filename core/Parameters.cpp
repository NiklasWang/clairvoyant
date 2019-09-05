#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "Parameters.h"
#include "Logs.h"
#include "Common.h"

namespace pandora {

Parameters::Parameters():mMap()
{
}

Parameters::~Parameters()
{
}

void Parameters::set(const char *key, const char *value)
{
	mMap[key] = value;
}

void Parameters::set(const char *key, int value)
{
    char str[16];
    sprintf(str, "%d", value);
    set(key, str);
}

void Parameters::set(const char *key, float value)
{
    char str[16];
    snprintf(str, sizeof(str), "%g", value);
    set(key, str);
}

const char* Parameters::get(const char *key) const
{
    if (mMap.find(key) != mMap.end()) {
        return mMap.find(key)->second.c_str();
    }
    return NULL;
}

int Parameters::getInt(const char *key) const
{
    const char *v = get(key);
    if (v == 0) {
        return -1;
    }
    return strtol(v, 0, 0);
}

float Parameters::getFloat(const char *key) const
{
    const char *v = get(key);
    if (v == 0) {
        return -1;
    }
    return strtof(v, 0);
}


};
