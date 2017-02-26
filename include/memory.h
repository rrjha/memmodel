#ifndef MEMORY_H
#define MEMORY_H

#include "datatypes.h"

extern uint64 access_iter;

typedef enum mem_access_type {
    EReadReq,
    ECleanEvict,
    EWbClean,
    EWbDirty,
    EInvalidAccess
} access_type;

// Abstract memory interface
class memory
{
    public:
        virtual void request(uint32 address, access_type req_type) = 0;
};

#endif // MEMORY_H
