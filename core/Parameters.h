#ifndef _PARAMETERS_H
#define _PARAMETERS_H

#include <map>

using namespace std;

namespace pandora {

class Parameters
{
public:
    Parameters();
    ~Parameters();

    void set(const char *key, const char *value);
    void set(const char *key, int value);
    void set(const char *key, float value);
    const char* get(const char *key) const;
    int getInt(const char *key) const;
    float getFloat(const char *key) const;

private:
    std::map<std::string, std::string> mMap;
};

};

#endif
