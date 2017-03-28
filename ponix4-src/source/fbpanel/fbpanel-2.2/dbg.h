#define ERR(fmt, args...) fprintf(stderr, fmt, ## args)
#define DBG2(fmt, args...) fprintf(stderr, fmt, ## args)

#ifdef DEBUG

#define ENTER do { fprintf(stderr, "%s: ENTER at %d\n", __FUNCTION__, __LINE__); } while(0)
#define RET(args...)   do { fprintf(stderr, "%s: RETURN at %d\n", __FUNCTION__, __LINE__);\
return args; } while(0)
#define DBG(fmt, args...) fprintf(stderr, fmt, ## args)

#else


#define ENTER         do {  } while(0)
#define RET(args...)   return args; 
#define DBG(fmt, args...)   do {  } while(0)
 
#endif

