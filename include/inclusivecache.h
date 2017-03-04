#ifndef INCLUSIVECACHE_H
#define INCLUSIVECACHE_H

#include "cache.h"


class inclusivecache : public cache
{
    public:
        inclusivecache(uint32 cachesize, memory *lowerlevel);
        virtual ~inclusivecache();
        void set_higherlevel(memory *higherlevel) { m_higherlevel = higherlevel; }
        void set_phymem(memory *phymem) {}

    protected:
        // Realization of virtual methods
        bool handle_read_req(uint32 set_val, uint32 tag_val);
        void handle_cleanevict_req(uint32 set_val, uint32 tag_val);
        void handle_dirtyevict_req(uint32 set_val, uint32 tag_val);
        void handle_invalidate_req(uint32 set_val, uint32 tag_val);

    private:
        memory *m_higherlevel;
};

#endif // INCLUSIVECACHE_H
