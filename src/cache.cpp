#include <stdio.h>
#include "cache.h"

#define BLKSIZE 64
#define TAGSIZE (1<<29)
#define NWAY 32

cache::cache(uint32 cachesize, memory *nextlevel)
{
    //ctor
    //allocate space for cache
    m_nsets = cachesize/(NWAY * BLKSIZE);
    m_setshift = getbits(BLKSIZE);
    m_tagshift = getbits(m_nsets) + m_setshift;

    m_setmask = (m_nsets - 1) << m_setshift;
    m_tagmask = (TAGSIZE - 1) << m_setshift;
    m_cachemem = new cacheblk* [m_nsets];
    for (uint32 i=0; i < m_nsets; i++)
        m_cachemem[i] = new cacheblk [NWAY];
    m_nextlevel = nextlevel;
}

cache::~cache()
{
    //dtor
    for(uint32 i=0; i < m_nsets; i++)
        delete [] m_cachemem[i];
    delete [] m_cachemem;
    m_cachemem = NULL;
    m_nextlevel = NULL;
}


uint32 cache::lru_evict(uint32 set_val, uint32 tag_val) {
    int i=0;
    uint32 lru_index = 0, min_atime = m_cachemem[set_val][0].laccess;
    uint32 address = blk_addr(set_val, tag_val);

    for (i=0; i < NWAY; i++) {
        if (m_cachemem[set_val][i].laccess < min_atime){
            lru_index = i;
            min_atime = m_cachemem[set_val][i].laccess;
        }
    }
    // Now check if the to-be replaced block is dirty, if so, send a write back to LLC
    // Note since we are not simulating L1 and thus no enforced inclusivity so there is a possiblity of WB miss
    if(m_cachemem[set_val][lru_index].state == EDirty)
        m_nextlevel->request(address, EWbDirty);
    else // Eclean
        m_nextlevel->request(address, ECleanEvict);
    return lru_index;
}

/*  Helper function - finds if the block exists in this set_val.                     *
 *  Returns true if block found and sets index to index of block.                *
 *  Else returns false and index of first empty index is returned in out param   */
bool cache::find_block(uint32 set_val, uint32 tag_val, uint32 *pindex) {
    bool found = false;
    int i = 0, empty_index = -1;
    /* Offset to this set and see if the tag_val exists */
    for (i=0; i < NWAY; i++) {
        if((m_cachemem[set_val][i].state != EInvalid) &&
            (m_cachemem[set_val][i].tag == tag_val)) {
            found = true;
            break;
        }
        else if ((empty_index == -1) && (m_cachemem[set_val][i].state == EInvalid))
            empty_index = i; //remember the first empty location for allocation if required
    }
    if(found)
        *pindex = i;
    else if (empty_index < 0)
        // No place - Evict LRU
        *pindex = lru_evict(set_val, tag_val);
    else
        *pindex = empty_index;
    return found;
}

void cache::request(uint32 address, access_type req_type) {
    uint32 set_val = (address & m_setmask) >> m_setshift;
    uint32 tag_val = (address && m_tagmask) >> m_tagshift;

    switch(req_type) {
    case EReadReq:
        handle_read_req(set_val, tag_val);
        break;

    case ECleanEvict:
    case EWbClean:
        handle_cleanevict_req(set_val, tag_val);
        break;

    case EWbDirty:
        handle_dirtyevict_req(set_val, tag_val);
        break;

    default:
        printf("Error - Undefined request type - %u\n", req_type);
    }
}
