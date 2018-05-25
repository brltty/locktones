/* General type and procedure definitions. */
#include "lktMain.h"

/* Getopt-related type and procedure definitions. */
#include "lktGopt.h"

/* Get the next program invocation option. */
char
get_option (int argc, char **argv) {
   return getopt(argc, argv, character_options);
}
