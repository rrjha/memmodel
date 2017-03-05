#include "includes.h"


bool physicalmemory::handle_request(uint64 address, access_type req_type) {
    switch(req_type) {
    case EReadReq:
        trace("Read request received for address=0x%llX - Read data and return\n", address);
        break;

    case ECleanEvict:
    case EWbClean:
        trace("Clean evict request received for address=0x%llX - Don't care\n", address);
        break;

    case EWbDirty:
        trace("Dirty evict request received for address=0x%llX - Write data and return\n", address);
        break;

    default:
        trace("Error - Undefined request type\n");
    }
    return true;
}
