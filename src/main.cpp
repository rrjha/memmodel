#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "physicalmemory.h"
#include "inclusivecache.h"
#include "exclusivecache.h"

#define L2SIZE (1 << 20)
#define L3SIZE (L2SIZE << 3)


// Global to keep track of relative access clock
uint64 access_iter = 0;

void get_opt(int num_args, char* args[], bool &inclusive, char *filename)
{
    if((num_args < 2) || (num_args > 3)) {
        /* Invalid number of arguments. Displaye usage */
        printf("Incorrect number of arguments.. exiting..\n");
        printf("Usage: %s [-i/-e] tracefilename\n", args[0]);
        exit(1);
    }
    if(num_args == 3) {
        int opt = getopt(num_args, args, "ie");
        if('i' == opt) {
            /* Inclusive cache */
            inclusive = true;
        } else if('e' == opt){
            /* Exclusive cache */
            inclusive = false;
        } else {
            /* We don't know why we are here - dump the option and throw error */
            printf("%c\n", opt);
            printf("Usage: %s [-i/-e] tracefilename\n", args[0]);
            exit(1);
        }
        strcpy(filename, args[2]);
    } else {
        strcpy(filename, args[1]);
        inclusive = true;
    }
}

void parse_request (access_type &curr_req, uint32 &curr_addr, const char *access_str) {
}


int main(int argc, char *argv[]) {
    // Initialize all memory hierarchy elements

    // Main memory
    physicalmemory mem;
    bool inclusive = false;
    char tracefile[256], line[512];
    access_type curr_req = EInvalidAccess;
    uint32 curr_addr = 0;

    get_opt(argc, argv, inclusive, tracefile);
    FILE *fin = fopen(tracefile, "r");
    if(NULL == fin) {
        perror("Program exiting due to error.. \n");
        exit(1);
    }

    // Caches
    cache *l2cache = NULL, *l3cache = NULL;

    if(inclusive) {
        l2cache = new inclusivecache(L2SIZE, l3cache, NULL);
        l3cache = new inclusivecache(L3SIZE, &mem, l2cache);
    } else {
        l2cache = new exclusivecache(L2SIZE, l3cache, &mem);
        l3cache = new exclusivecache(L3SIZE, &mem, NULL);
    }

    /* Read file line by line and process */
    while (fgets(line, sizeof(line), fin)) {
        //remove the trailing \n
        if (line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';
        //And remove the trailing \r for dos format input files
        if (line[strlen(line)-1] == '\r')
            line[strlen(line)-1] = '\0';
        printf("read line %s\n", line);
        parse_request(curr_req, curr_addr, line);
        if(EInvalidAccess != curr_req)
            l2cache->handle_request(curr_addr, curr_req);
    }

    delete l2cache;
    delete l3cache;

    fclose(fin);

    return 0;
}
