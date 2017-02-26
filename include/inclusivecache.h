#ifndef INCLUSIVECACHE_H
#define INCLUSIVECACHE_H

#include "cache.h"


class inclusivecache : public cache
{
    public:
        inclusivecache(uint32 cachesize, memory *nextlevel);
        virtual ~inclusivecache();

    protected:
        // Realization of virtual methods
        void handle_read_req(uint32 set_val, uint32 tag_val);
        void handle_cleanevict_req(uint32 set_val, uint32 tag_val);
        void handle_dirtyevict_req(uint32 set_val, uint32 tag_val);
};

#endif // INCLUSIVECACHE_H
