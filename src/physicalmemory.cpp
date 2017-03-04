#include <stdio.h>
#include "physicalmemory.h"


bool physicalmemory::handle_request(uint32 address, access_type req_type) {
    switch(req_type) {
    case EReadReq:
        trace("Read request received - Read data and return\n");
        break;

    case ECleanEvict:
    case EWbClean:
        trace("Clean evict request received - Don't care\n");
        break;

    case EWbDirty:
        trace("Dirty evict request received - Write data and return\n");
        break;

    default:
        trace("Error - Undefined request type\n");
    }
    return true;
}
