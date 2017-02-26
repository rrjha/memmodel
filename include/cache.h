#ifndef CACHE_H
#define CACHE_H

#include "memory.h"

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
        cache(uint32 cachesize, memory *nextlevel);
        virtual ~cache();
        void request(uint32 address, access_type req_type);

        uint32 getbits(uint32 num) {
            uint32 power = 0;
            while((num >>= 1))
                power++;
            return power;
        }


    protected:

        // Inline
        uint32 blk_addr(uint32 set_val, uint32 tag_val){
            return ((set_val << m_tagshift) | (set_val << m_setshift));
        }
        bool find_block(uint32 set_val, uint32 tag_val, uint32 *pindex);
        uint32 lru_evict(uint32 set_val, uint32 tag_val);

        // Virtual methods to be overridden by actual caches
        virtual void handle_read_req(uint32 set_val, uint32 tag_val) = 0;
        virtual void handle_cleanevict_req(uint32 set_val, uint32 tag_val) = 0;
        virtual void handle_dirtyevict_req(uint32 set_val, uint32 tag_val) = 0;

        // Data members - Kept protected so that deriver class can access these directly
        uint32 m_nsets;
        uint32 m_setshift, m_tagshift;
        uint32 m_setmask, m_tagmask;
        cacheblk **m_cachemem;
        memory *m_nextlevel;
};

#endif // CACHE_H
