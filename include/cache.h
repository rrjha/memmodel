#ifndef CACHE_H
#define CACHE_H

#include "includes.h"

typedef enum cache_state {
    EInvalid,
    EClean,
    EDirty
} cstate;

typedef struct cacheblock {
    uint32 tag;
    uint64 laccess;
    cstate state;
} cacheblk;

class cache : public memory
{
    public:
        cache(uint32 cachesize, memory *lowerlevel);
        virtual ~cache();
        bool handle_request(uint64 address, access_type req_type);

        uint32 getbits(uint32 num) {
            uint32 power = 0;
            while((num >>= 1))
                power++;
            return power;
        }
        uint64 getWriteCount() { return m_writecount; }

        void set_higherlevel(cache **highlevel, uint32 highCount) { m_highlevel = highlevel; m_highCount = highCount; }
        void set_phymem(memory *phymem) { m_phymem = phymem; }


    protected:

        // Inline
        uint64 blk_addr(uint32 set_val, uint32 tag_val){
            return ((tag_val << m_tagshift) | (set_val << m_setshift));
        }
        bool find_block(uint32 set_val, uint32 tag_val, uint32 *pindex);
        uint32 find_lru(uint32 set_val);
        uint32 find_mru(uint32 set_val);

        // Virtual methods to be overridden by actual caches
        virtual bool handle_read_req(uint32 set_val, uint32 tag_val) = 0;
        virtual void handle_cleanevict_req(uint32 set_val, uint32 tag_val) = 0;
        virtual void handle_dirtyevict_req(uint32 set_val, uint32 tag_val) = 0;
        virtual void handle_invalidate_req(uint32 set_val, uint32 tag_val) = 0;

        // Data members - Kept protected so that deriver class can access these directly
        uint32 m_nsets;
        uint32 m_setshift, m_tagshift;
        uint64 m_setmask, m_tagmask;
        cacheblk **m_cachemem;
        memory *m_lowerlevel;
        cache **m_highlevel;
        uint32 m_highCount;
        memory *m_phymem;
        uint64 m_writecount;
};

#endif // CACHE_H
