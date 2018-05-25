/* General type and procedure definitions. */
#include "lktMain.h"

/* Getopt-related type and procedure definitions. */
#include "lktGopt.h"

/* Support for long options. */
#include <getopt.h>

/* The full-word option table. */
const struct option word_options[] = {
   {"active"    , no_argument      , NULL, 'a'},
   {"caps"      , required_argument, NULL, 'c'},
   {"duration"  , required_argument, NULL, 'd'},
   {"foreground", no_argument      , NULL, 'f'},
   {"help"      , no_argument      , NULL, 'h'},
   {"insert"    , required_argument, NULL, 'i'},
   {"num"       , required_argument, NULL, 'n'},
   {"poll"      , required_argument, NULL, 'p'},
   {"scroll"    , required_argument, NULL, 's'},
   {"toggle"    , no_argument      , NULL, 't'},
   {"verbose"   , no_argument      , NULL, 'v'},
   {"version"   , no_argument      , NULL, 'V'},
   {NULL        , 0                , NULL, 0  }
};

/* Get the next program invocation option. */
char get_option (int argc, char **argv) {
   return getopt_long(argc, argv, character_options, word_options, NULL);
}
