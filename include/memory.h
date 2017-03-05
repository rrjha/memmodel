#ifndef MEMORY_H
#define MEMORY_H

#include "includes.h"

extern uint64 access_iter;

typedef enum mem_access_type {
    EReadReq,
    ECleanEvict,
    EWbClean,
    EWbDirty,
    EInvalidate,
    EInvalidAccess
} access_type;

// Abstract memory interface
class memory
{
    public:
        virtual bool handle_request(uint64 address, access_type req_type) = 0;
};

#endif // MEMORY_H
