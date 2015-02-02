/* DJGPP-specific includes. */
#include <crt0.h>
#include <bios.h>
#include <pc.h>
#include <dpmi.h>
#include <go32.h>
#include <sys/exceptn.h>

/* Define the number of micro-seconds per tick (2^16 per hour). */
#define tick_interval (3600000000UL / 0X10000)

/* Calculate the number of ticks per time interval (in micro-seconds). */
#define tick_count(micro_seconds) (((micro_seconds) + (tick_interval - 1)) / tick_interval)
