#include <stdio.h>
#include "physicalmemory.h"


void physicalmemory::request(uint32 address, access_type req_type) {
    switch(req_type) {
    case EReadReq:
        printf("Read request received\n");
        break;

    case ECleanEvict:
    case EWbClean:
        printf("Clean evict request received\n");
        break;

    case EWbDirty:
        printf("Dirty evict request received\n");
        break;

    default:
        printf("Error - Undefined request type\n");
    }
}
