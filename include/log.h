#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

//Pass -D TRACE flag to GCC to turn on debug messages
#ifdef TRACE
#define trace(fmt, ...) \
        do { fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)
#else
#define trace(fmt, ...) ((void)0)
#endif

#define LOG(component, fmt, ...) \
        do { fprintf(stdout, "%s: %s(): " fmt, component, __func__, ##__VA_ARGS__); } while(0)


#define l2tag "L2Cache"
#define l3tag "L3Cache"
#define memtag "MM"


#endif // LOG_H_INCLUDED
