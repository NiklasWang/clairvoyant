#ifndef _IDENTIFIER_H_
#define _IDENTIFIER_H_

#include <string>

#include "Common.h"

namespace pandora {

typedef int32_t IdType;

struct Identifier {
public:
    IdType myId() const;
    const char *myName() const;
    const char *me() const;
    const char *whoamI() const;
    IdType id() const;
    const char *name() const;
    ModuleType getModule() const;
    void dump() const;

protected:
    IdType        mId;
    std::string   mName;
    ModuleType    mModule;
    static IdType mInstanceCnt;

public:
    Identifier(ModuleType module = MODULE_MAX_INVALID,
        const char *name = "default");
    virtual ~Identifier() = default;
    Identifier(const Identifier &rhs);
    Identifier &operator=(const Identifier &rhs);
};


template <typename T>
struct Type2ID;
template <int id>
struct ID2Type;

#define REGISTER_TYPE(type, n) \
    template <> \
    struct Type2ID<type> { char id[n]; }; \
    template <> \
    struct ID2Type<n>    { typedef type type_t; };

template <typename T>
Type2ID<T> encode(const T&);
template <typename T>
Type2ID<T> encode(T&);

#define type_of(...) \
    ID2Type<sizeof(encode((__VA_ARGS__)))>::type_t


};

#endif
