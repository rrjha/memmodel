#ifndef EXCLUSIVECACHE_H
#define EXCLUSIVECACHE_H

#include "includes.h"

//Implements L3 as victim cache thus keeping L2 and L3 exclusive

class exclusivecache : public cache
{
    public:
        exclusivecache(uint32 cachesize, memory* lowerlevel);
        virtual ~exclusivecache();

    protected:
        // Realization of virtual methods
        bool handle_read_req(uint32 set_val, uint32 tag_val);
        void handle_cleanevict_req(uint32 set_val, uint32 tag_val);
        void handle_dirtyevict_req(uint32 set_val, uint32 tag_val);
        void handle_invalidate_req(uint32 set_val, uint32 tag_val) {};
};

#endif // EXCLUSIVECACHE_H
