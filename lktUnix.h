/* Unix-specific includes. */
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>

/* Flavour-specific procedures. */
extern void get_resources (void);
extern int tone_on (int pitch);
extern void tone_off (void);
extern int get_flags (void);
