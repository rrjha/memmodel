#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "physicalmemory.h"
#include "inclusivecache.h"
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
        }
        else if('e' == opt){
            /* Exclusive cache */
            inclusive = false;
        }
        else {
            /* We don't know why we are here - dump the option and throw error */
            printf("%c\n", opt);
            printf("Usage: %s [-i/-e] tracefilename\n", args[0]);
            exit(1);
        }
        strcpy(filename, args[2]);
    }
    else {
        strcpy(filename, args[1]);
        inclusive = true;
    }
}


int main(int argc, char *argv[]) {
    // Initialize all memory hierarchy elements

    // Main memory
    physicalmemory mem;
    bool inclusive = false;
    char tracefile[256];

    get_opt(argc, argv, inclusive, tracefile);
    FILE *fin = fopen(tracefile, "r");
    if(NULL == fin) {
        perror("Program exiting due to error.. \n");
        exit(1);
    }

    // Caches
    cache *l3cache = NULL;
    cache *l2cache = new inclusivecache(L2SIZE, l3cache);

    if(inclusive) {
        l3cache = new inclusivecache(L3SIZE, &mem);
    }

    /* Read file line by line and process */

    delete l2cache;
    delete l3cache;

    return 0;
}
