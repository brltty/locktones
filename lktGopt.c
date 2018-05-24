/* General type and procedure definitions. */
#include "lktMain.h"

/* Getopt-related type and procedure definitions. */
#include "lktGopt.h"

/* The single-character option table. */
const char *character_options = "+ac:d:fhi:n:p:s:tvV";

/* Get the operand for the current program invocation option. */
const char *get_operand () {
   return optarg;
}

/* Adjust argc and argv so that they only reference the positional arguments. */
void get_parameters (int *argcp, char ***argvp) {
   *argcp -= optind; // The positional argument count.
   *argvp += optind; // The address of the first positional argument.
}
