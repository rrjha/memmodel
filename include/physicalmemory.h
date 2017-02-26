#ifndef PHYSICALMEMORY_H
#define PHYSICALMEMORY_H

#include "memory.h"


class physicalmemory : public memory
{
    public:
        physicalmemory() { /* Nothing to construct */ }
        ~physicalmemory() { /* Nothing to destroy */ }
        bool handle_request(uint32 address, access_type req_type);
};

#endif // PHYSICALMEMORY_H
