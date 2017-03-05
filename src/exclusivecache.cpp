#include "includes.h"

exclusivecache::exclusivecache(uint32 cachesize, memory* lowerlevel) : cache(cachesize, lowerlevel)
{
    //ctor
}

exclusivecache::~exclusivecache()
{
    //dtor
    m_phymem = NULL;
}

void exclusivecache::handle_cleanevict_req(uint32 set_val, uint32 tag_val){
    if(!m_phymem) {
        /* This is L3 - Allocate this block if not existing */
        uint32 index = 0;
        uint32 address = blk_addr(set_val, tag_val);
        bool found = find_block(set_val, tag_val, &index);
        if(!found) {
            // Check if we are replacing an old block
            if(m_cachemem[set_val][index].state != EInvalid) {
                // Check if the to-be replaced block is dirty, if so, send a write back to next lower level memory
                if(m_cachemem[set_val][index].state == EDirty)
                    m_lowerlevel->handle_request(address, EWbDirty);
                else // Eclean
                    m_lowerlevel->handle_request(address, ECleanEvict);
            }

           //Now place the new block in the cache
            m_cachemem[set_val][index].state = EClean;
            m_cachemem[set_val][index].tag = tag_val;
            // Update as MRU
            m_cachemem[set_val][index].laccess = access_iter;
            m_writecount++;
        }
    }
}

void exclusivecache::handle_dirtyevict_req(uint32 set_val, uint32 tag_val){
    /* This is a dirty block - Note it */
    uint32 index = 0;
    uint32 address = blk_addr(set_val, tag_val);
    bool found = find_block(set_val, tag_val, &index);

    if(found){
        //Data found - Update the last access
        m_cachemem[set_val][index].state = EDirty;
        // Now set the access for MRU if block found in L2 and LRU if in L3 (because L3 is exclusive)
        if(m_phymem)
            m_cachemem[set_val][index].laccess = access_iter;
        else
            m_cachemem[set_val][index].laccess = 0;
        //increment the write count irrespective of L2 or L3 as data is written back (mirrored)
        m_writecount++;
    }
    else if (m_phymem){
        /* Block is not cached */
        trace("Writeback miss in L2 for block 0x%X\n", address);
    }
    else {
            // Check if we are replacing an old block
        if(m_cachemem[set_val][index].state != EInvalid) {
            // Check if the to-be replaced block is dirty, if so, send a write back to next lower level memory
            if(m_cachemem[set_val][index].state == EDirty)
                m_lowerlevel->handle_request(address, EWbDirty);
            else // Eclean
                m_lowerlevel->handle_request(address, ECleanEvict);
        }

       //Now allocate this block with dirty state and set as MRU -
        m_cachemem[set_val][index].state = EDirty;
        m_cachemem[set_val][index].tag = tag_val;
        // Set as LRU
        m_cachemem[set_val][index].laccess = access_iter;
        m_writecount++;
    }
}

/* We don't care about the data in simulation */
bool exclusivecache::handle_read_req(uint32 set_val, uint32 tag_val) {
    uint32 index = 0;
    uint32 address = blk_addr(set_val, tag_val);
    bool found = find_block(set_val, tag_val, &index);
    bool retval = false;

    if(found){
        //Data found - Return the data to seeker
        retval = true;
        // Now set the access for MRU if block found in L2 and LRU if in L3 (because L3 is exclusive)
        if(m_phymem)
            m_cachemem[set_val][index].laccess = access_iter;
        else
            m_cachemem[set_val][index].laccess = 0;
    }
    /* Distinguish between L2 and L3 - L3 takes no ownership */
    /* L2 reads from lower level cache first and then if not found goes to mem */
    else if(m_phymem) {
        /* Block is not cached - send request to next level */
        retval = m_lowerlevel->handle_request(address, EReadReq);
        if(!retval) {
            m_phymem->handle_request(address, EReadReq);
            retval = true;
        }

        // Do the needful if we are replacing block
        if(m_cachemem[set_val][index].state != EInvalid) {
            // Check if the to-be replaced block is dirty, if so, send a write back to next lower level cache
            // Note since we are not simulating L1 and thus no enforced inclusivity so there is a possiblity of WB miss
            if(m_cachemem[set_val][index].state == EDirty)
                m_lowerlevel->handle_request(address, EWbDirty);
            else // EClean
                m_lowerlevel->handle_request(address, ECleanEvict);
        }
       //Now place the new block in the cache
        m_cachemem[set_val][index].state = EClean;
        m_cachemem[set_val][index].tag = tag_val;
        m_cachemem[set_val][index].laccess = access_iter;
        m_writecount++;
    }
    return retval;
}

