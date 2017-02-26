#include <stdio.h>
#include "inclusivecache.h"

inclusivecache::inclusivecache(uint32 cachesize, memory *nextlevel) : cache(cachesize, nextlevel)
{
    //ctor - empty implementation
}

inclusivecache::~inclusivecache()
{
    //dtor
    // Nothing to do here base class releases all memory for cache
}

void inclusivecache::handle_cleanevict_req(uint32 set_val, uint32 tag_val){
    /* No implementation for inclusive? */
    /* Update for exclusive in next pass */
}

void inclusivecache::handle_dirtyevict_req(uint32 set_val, uint32 tag_val){
    /* This is a dirty block - Note it */
    uint32 index = 0;
    uint32 address = blk_addr(set_val, tag_val);
    bool found = find_block(set_val, tag_val, &index);

    if(found){
        //Data found - Update the last access
        m_cachemem[set_val][index].state = EDirty;
        m_cachemem[set_val][index].laccess = access_iter;
    }
    else {
        /* Block is not cached (WB miss) - This is only likely if inclusivity is not enforced */
        printf("Writeback miss for block 0x%X\n", address);
    }
}

/* We don't care about the data in simulation so no need to return anything */
void inclusivecache::handle_read_req(uint32 set_val, uint32 tag_val) {
    uint32 index = 0;
    uint32 address = blk_addr(set_val, tag_val);
    bool found = find_block(set_val, tag_val, &index);

    if(found){
        //Data found - Update the last access
        m_cachemem[set_val][index].laccess = access_iter;
    }
    else {
        /* Block is not cached - send request to next level */
        m_nextlevel->request(address, EReadReq);
        // Assume that data is returned instantaneously
        //Now place the block in the cache
        m_cachemem[set_val][index].state = EClean;
        m_cachemem[set_val][index].tag = tag_val;
        m_cachemem[set_val][index].laccess = access_iter;
    }
}
