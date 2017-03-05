#ifndef PHYSICALMEMORY_H
#define PHYSICALMEMORY_H

#include "includes.h"


class physicalmemory : public memory
{
    public:
        physicalmemory() { /* Nothing to construct */ }
        ~physicalmemory() { /* Nothing to destroy */ }
        bool handle_request(uint64 address, access_type req_type);
};

#endif // PHYSICALMEMORY_H
