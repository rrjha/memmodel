#include "includes.h"

inclusivecache::inclusivecache(uint32 cachesize, memory *lowerlevel) : cache(cachesize, lowerlevel)
{
    //ctor
}

inclusivecache::~inclusivecache()
{
    //dtor
}

void inclusivecache::handle_invalidate_req(uint32 set_val, uint32 tag_val) {
    uint32 index = 0;
    bool found = find_block(set_val, tag_val, &index);

    if(found)
        m_cachemem[set_val][index].state = EInvalid;
}

void inclusivecache::handle_cleanevict_req(uint32 set_val, uint32 tag_val){
    /* No implementation for inclusive? */
}

void inclusivecache::handle_dirtyevict_req(uint32 set_val, uint32 tag_val){
    /* This is a dirty block - Note it */
    uint32 index = 0;
    uint64 address = blk_addr(set_val, tag_val);
    bool found = find_block(set_val, tag_val, &index);

    if(found){
        //Data found - Update the last access
        m_cachemem[set_val][index].state = EDirty;
        m_cachemem[set_val][index].laccess = access_iter;
        m_writecount++;
    }
    else {
        /* Block is not cached (WB miss) - This is only likely if inclusivity is not enforced */
        trace("Writeback miss for block 0x%llX\n", address);
    }
}

/* We don't care about the data in simulation so just return true for inclusive */
bool inclusivecache::handle_read_req(uint32 set_val, uint32 tag_val) {
    uint32 index = 0;
    uint64 address = blk_addr(set_val, tag_val);
    bool found = find_block(set_val, tag_val, &index);

    if(found){
        //Data found - Update the last access
        m_cachemem[set_val][index].laccess = access_iter;
    }
    else {
        /* Block is not cached - send request to next level */
        // Assume that data is returned instantaneously
        m_lowerlevel->handle_request(address, EReadReq);

        // Now allocate space for block and cache
        // Do the needful if we are replacing block
        if(m_cachemem[set_val][index].state != EInvalid) {
            // Check if the to-be replaced block is dirty, if so, send a write back to next lower level cache
            // Note since we are not simulating L1 and thus no enforced inclusivity so there is a possiblity of WB miss
            if(m_cachemem[set_val][index].state == EDirty)
                m_lowerlevel->handle_request(address, EWbDirty);
            else // Eclean
                m_lowerlevel->handle_request(address, ECleanEvict);

            //Force inclusivity for L2 to L3
            for (uint32 i=0; i < m_highCount; i++)
                if(m_highlevel[i])
                    m_highlevel[i]->handle_request(address, EInvalidate);
        }
       //Now place the new block in the cache
        m_cachemem[set_val][index].state = EClean;
        m_cachemem[set_val][index].tag = tag_val;
        m_cachemem[set_val][index].laccess = access_iter;
        m_writecount++;
    }
    return true;
}
