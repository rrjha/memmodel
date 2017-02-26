#include <stdio.h>
#include "physicalmemory.h"


bool physicalmemory::handle_request(uint32 address, access_type req_type) {
    switch(req_type) {
    case EReadReq:
        printf("Read request received - Read data and return\n");
        break;

    case ECleanEvict:
    case EWbClean:
        printf("Clean evict request received - Don't care\n");
        break;

    case EWbDirty:
        printf("Dirty evict request received - Write data and return\n");
        break;

    default:
        printf("Error - Undefined request type\n");
    }
    return true;
}
