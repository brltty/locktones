/* General type and procedure definitions. */
#include "lktMain.h"

/* Getopt-related type and procedure definitions. */
#include "lktGopt.h"

/* The single-character option table. */
extern const char *character_options;

/* Get the next program invocation option. */
char get_option (int argc, char **argv) {
   return getopt(argc, argv, character_options);
}
