#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "physicalmemory.h"
#include "inclusivecache.h"
#include "exclusivecache.h"

#define L2SIZE (1 << 20)
#define L3SIZE (L2SIZE << 3)

// Our trace has L2 as shared so wb are having core as -1
// Hack this to set the core to 0
#define L2_SHARED_HACK


// Global to keep track of relative access clock
uint64 access_iter = -1;

void get_opt(int num_args, char* args[], bool &inclusive, char *filename, int &ncore)
{
    int opt;
    if((num_args < 2) || (num_args > 5)) {
        /* Invalid number of arguments. Displaye usage */
        printf("Incorrect number of arguments.. exiting..\n");
        printf("Usage: %s [-i/-e] [-n <number>] tracefilename\n", args[0]);
        exit(1);
    }

    if(num_args >= 3) {
        while ((opt = getopt(num_args, args, "ien:")) != -1) {
            if('i' == opt) {
                /* Inclusive cache */
                inclusive = true;
            } else if('e' == opt){
                /* Exclusive cache */
                inclusive = false;
            } else if('n' == opt) {
                ncore = atoi(optarg);
            } else {
                /* We don't know why we are here - dump the option and throw error */
                trace("%c\n", opt);
                printf("Usage: %s [-i/-e] [-n <number>] tracefilename\n", args[0]);
                exit(1);
            }
        }
        if(optind >= num_args) {
            printf("Expected trace file name after options\n");
            exit(1);
        }
        strcpy(filename, args[optind]);
    } else {
        strcpy(filename, args[1]);
        inclusive = true;
    }
}

void parse_request (int &core, access_type &curr_req, uint32 &curr_addr, const char *access_str) {
    char *copy_str = strdup(access_str);
    char *token = NULL;

    char *temp = copy_str;

    // Remove unecessary tokens of timestamp and name
    token = strsep(&temp, ":");
    temp++;
    token = strsep(&temp, ":");
    temp++;

    // Remove "Core-" and get the core.
    temp += 5;
    token = strsep(&temp, " ");
    core = strtol(token, NULL, 10);

    // Remove string "address=" string and get the address
    temp += 8;
    token = strsep(&temp, " ");
    curr_addr = strtoul(token, NULL, 16);

    // Get request type. Remove "cmd="
    temp += 4;
    switch(temp[0]) {
        case 'R':
            curr_req = EReadReq;
        break;

        case 'W':
#ifdef L2_SHARED_HACK
            core = (core == -1) ? 0 : core;
#endif
            temp += 10;
            if(!strncmp(temp, "WritebackDirty", 14)) {
                curr_req = EWbDirty;
                break;
            }
            // Fall through for wbclean

        case 'N':
            curr_req = ECleanEvict;
        break;

        default:
            // Don't handle other requests for now just return invalid
            curr_req = EInvalidAccess;
    }
    free(copy_str);
}


int main(int argc, char *argv[]) {
    // Initialize all memory hierarchy elements

    // Main memory
    physicalmemory mem;
    bool inclusive = false;
    char tracefile[256], line[512];
    int ncore = 1, core;
    access_type curr_req = EInvalidAccess;
    uint32 curr_addr = 0;

    get_opt(argc, argv, inclusive, tracefile, ncore);
    FILE *fin = fopen(tracefile, "r");
    if(NULL == fin) {
        perror("Program exiting due to error.. \n");
        exit(1);
    }

    // Caches - Shared L3 and private L2
    cache **l2cache = NULL, *l3cache = NULL;
    l2cache = new cache*[ncore];

    if(inclusive) {
        l3cache = new inclusivecache(L3SIZE, &mem);
        for (int i=0; i<ncore; i++) {
            l2cache[i] = new inclusivecache(L2SIZE, l3cache);
            l2cache[i]->set_higherlevel(NULL);
        }
        // Note for ncore > 1 we need to have coherency protocols to initiate back trigger e.g, backward invlaidation
    if (ncore == 1)
        l3cache->set_higherlevel(l2cache[0]);
    } else {
        l3cache = new exclusivecache(L3SIZE, &mem);
        for (int i=0; i<ncore; i++) {
            l2cache[i] = new exclusivecache(L2SIZE, l3cache);
            l2cache[i]->set_phymem(&mem);
        }
    // To Check - May not be required as we don't communicate with higher levels in victim cache
    if (ncore == 1)
        l3cache->set_phymem(NULL);
    }

    /* Read file line by line and process */
    while (fgets(line, sizeof(line), fin)) {
        access_iter++;
        //remove the trailing \n
        if (line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';
        //And remove the trailing \r for dos format input files
        if (line[strlen(line)-1] == '\r')
            line[strlen(line)-1] = '\0';
        trace("read line %s\n", line);
        parse_request(core, curr_req, curr_addr, line);
        if((core > -1) &&(EInvalidAccess != curr_req))
            l2cache[core]->handle_request(curr_addr, curr_req);
    }

    for (int i=0; i<ncore; i++)
        delete l2cache[i];
    delete l3cache;

    fclose(fin);

    return 0;
}
