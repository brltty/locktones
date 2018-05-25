/* General type and procedure definitions. */
#include "lktMain.h"

/* Program version specification. */
const char *program_version = "0.6";

const char *program_path = "locktones"; // The full path to the executing binary.
const char *program_name = NULL; // The name (last path component) of the executing binary.

enum {
   play_active, // Play a persistent tone for each active lock.
   play_toggle  // Play an ascending (descending) tone sequence for each activated (deactivated) lock.
};

#define play_default play_toggle
#define poll_default 1 // The default lock state poll interval.
#define duration_default 2 // The default tone duration.
#define caps_default    300 // The default caps lock tone pitch.
#define insert_default  450 // The default insert mode tone pitch.
#define number_default  600 // The default number lock tone pitch.
#define scroll_default 1200 // The default scroll lock tone pitch.

static const int minimum_interval = 1; // The minimum specifiable time interval.
static const int maximum_interval = 10; // The maximum specifiable time interval.
#define interval_multiplier 100000 // The number of micro-seconds per specifiable interval unit.

static const int minimum_pitch = 30; // The minimum specifiable tone pitch.
static const int maximum_pitch = 10000; // The maximum specifiable tone pitch.

struct tone_entry { // The definition of a tone.
   int pitch; // The pitch (in Herz).
   const struct tone_entry *next; // The next tone (or NULL) in the sequence.
};
#define DEFINE_TONE(lock) static struct tone_entry lock##_tone = {lock##_default, NULL}
DEFINE_TONE(caps); // The default caps lock tone.
DEFINE_TONE(insert); // The default insert mode tone.
DEFINE_TONE(number); // The default number lock tone.
DEFINE_TONE(scroll); // The default scroll lock tone.

static int foreground_task = 0;
int show_changes = 0;
static int play_mode = play_default; // The way in which the tones are to be played.
static int poll_interval = poll_default * interval_multiplier; // Micro-seconds between each poll of the lock states.
static int initial_duration = 0; // The duration of the initial tones of a sequence (in micro-seconds).
static int final_duration = duration_default * interval_multiplier; // The duration of the last tone of a sequence (in micro-seconds).
static int lock_flags = 0; // The current states of the keyboard locks.

/* The lock table. */
typedef struct { // The definition of an entry in the lock table.
   const int flag; // The platform-independent flag bit for this lock.
   const struct tone_entry *tone; // The tone for this lock.
   const struct tone_entry *activated; // The tune for when this lock is activated.
   const struct tone_entry *deactivated; // The tune for when this lock is deactivated.
   unsigned int active:1; // The current state of the lock.
} lock_entry;
#define LOCK_ENTRY(lock) {lock##_flag, &lock##_tone, NULL, NULL, FALSE}
static lock_entry lock_table[] = { // The table of all of the supported locks.
   LOCK_ENTRY(caps),
   LOCK_ENTRY(insert),
   LOCK_ENTRY(number),
   LOCK_ENTRY(scroll)
};
static const int lock_count = sizeof(lock_table) / sizeof(lock_entry); // The number of locks in the table.
static int next_lock = 0; // The index into the lock table of the next lock to check.
static const struct tone_entry *next_tone = NULL; // The next tone to play.

/* Check the lock states, and take appropriate action if they have changed. */
static void check_active (int flags) {
   if (flags != lock_flags) {
      /* At least one lock state has changed. */
      int old_flags = lock_flags;
      if ((lock_flags = flags) == 0) {
	 /* No locks are active. */
	 stop_tone();
      } else if (old_flags == 0) {
	 /* No locks were active. */
	 select_tone();
      }
   }
}

/* Check the lock states, and take appropriate action if they have changed. */
static void check_toggle (int flags) {
   if (flags != lock_flags) {
      /* At least one lock state has changed. */
      lock_flags = flags;
      select_tone();
   }
}

/* If the lock is active then return its base tone. */
static const struct tone_entry *select_active (lock_entry *lock) {
   if (lock_flags & lock->flag) {
      /* This lock is active. */
      const struct tone_entry *tone = lock->tone;
      if (tone->pitch) {
	 /* Its pitch is not "off". */
	 return tone;
      }
   }
   return NULL;
}

/* If the lock has changed state then return the appropriate tune. */
static const struct tone_entry *select_toggle (lock_entry *lock) {
   if (((lock_flags & lock->flag) != 0) != lock->active) {
      /* This lock has changed state. */
      return (lock->active = !lock->active) ? lock->activated : lock->deactivated;
   }
   return NULL;
}

typedef struct {
   const char *option; // The name of the option.
   void (*check) (int flags);
   const struct tone_entry * (*select) (lock_entry *lock);
} play_entry;
static const play_entry play_table[] = {
   {"active", check_active, select_active},
   {"toggle", check_toggle, select_toggle}
};

/* Start the next tone. */
void select_tone () {
   if (next_tone == NULL) {
      /* The current tone sequence has finished. */
      int loop_limit;
      for (loop_limit=lock_count; loop_limit>0; --loop_limit) {
	 /* Check the next lock. */
	 lock_entry *lock = &lock_table[next_lock];
	 next_lock = (next_lock + 1) % lock_count;
	 if ((next_tone = play_table[play_mode].select(lock))) {
	    break;
	 }
      }
   }
   if (next_tone) {
      /* There is a tone to play. */
      start_tone(next_tone->pitch, ((next_tone->next != NULL) ? initial_duration : final_duration));
      next_tone = next_tone->next;
   } else {
      /* There is nothing to play. */
      stop_tone();
   }
}

typedef struct {
   const char *name;
   unsigned char bit;
} flag_entry;

static const flag_entry flag_table[] = {
   { "caps"  , caps_flag  },
   { "number", number_flag   },
   { "scroll", scroll_flag},
   { "insert", insert_flag},
   { NULL }
};

/* Analyze the latest states of the locks and take appropriate action. */
void check_flags (int flags) {
   if (show_changes) {
      const flag_entry *flag = flag_table;

      while (flag->name) {
         const char *state = NULL;

         if ((flags & flag->bit) && !(lock_flags & flag->bit)) {
            state = "on";
         } else if (!(flags & flag->bit) && (lock_flags & flag->bit)) {
            state = "off";
         }

         if (state) printf("%s %s\n", flag->name, state);
         flag += 1;
      }

      fflush(stdout);
   }

   play_table[play_mode].check(flags);
}

/* Terminate the program with the conventional exit status used for syntax errors. */
void syntax_error () {
   exit(2);
}

/* Write a message to standard error describing the most recent system error (value of errno)
 * and then terminate the program with a non-zero exit status.
 */
void system_error (const char *action) {
   fprintf(stderr, "%s: %s error %d: %s\n", program_path, action, errno, strerror(errno));
   exit(3);
}

/* Allocate a block of storage; abort execution if it cannot be done. */
static void *allocate_memory (size_t size) {
   void *address = malloc(size);
   if (address == NULL) {
      system_error("memory allocation");
   }
   return address;
}

/* Determine the path to, and name of, the executing binary. */
static void identify_program (int argc, char **argv) {
   if (argv) {
      /* An argument vector has been supplied. */
      if (*argv) {
	 /* The path to the executing binary has been supplied. */
	 program_path = *argv;
      }
   }
   if ((program_name = strrchr(program_path, '/'))) {
      /* The path contains more than one component. */
      ++program_name;
   } else {
      /* The path contains just one component. */
      program_name = program_path;
   }
}

/* Write program version information to standard output. */
static void display_version () {
   printf("%s %s\n", program_name, program_version);
}

/* Write program usage information to standard output. */
static void display_usage () {
   const static char *lines[] = {
      "",
      "Use the PC speaker to audibly indicate the states of the keyboard locks.",
      "",
      "{-t | --toggle}         Play an ascending (descending) tone sequence each time",
      "                           a lock or mode is activated (deactivated).",
      "{-a | --active}         Play a persistent tone for each active lock and mode.",
      "",
      "{-p | --poll} time      The lock state poll interval.",
      "{-d | --duration} time  The duration of each tone.",
      "",
      "{-c | --caps} pitch     The pitch of the caps lock tone.",
      "{-i | --insert} pitch   The pitch of the insert mode tone.",
      "{-n | --number} pitch   The pitch of the number lock tone.",
      "{-s | --scroll} pitch   The pitch of the scroll lock tone.",
      "",
      "{-f | --foreground}     Don't become a daemon.",
      "{-h | --help}           Write usage information to standard output and exit.",
      "{-v | --verbose}        Write state changes to standard output (implies -f).",
      "{-V | --version}        Write version information to standard output and exit.",
      "",
      NULL
   };
   const char **line = lines;
   printf("Usage: %s [option ...]\n", program_name);
   while (*line) {
      printf("%s\n", *line++);
   }
   printf("A \"time\" operand is an integer from %d through %d (tenths of a second).\n", minimum_interval, maximum_interval);
   printf("A \"pitch\" operand is an integer from %d through %d (Herz).\n", minimum_pitch, maximum_pitch);
   printf("\nThe defaults are: --caps=%d --insert=%d --number=%d --scroll=%d\n",
	  caps_default, insert_default, number_default, scroll_default);
   printf("                  --%s --poll=%d --duration=%d\n",
	  play_table[play_default].option, poll_default, duration_default);
}

/* Convert a string into an integer,
 * insuring that its value is within an acceptable range.
 * If it begins with either "0x" or "0X", then it's interpreted as a hexadecimal number;
 * if it begins with "0" then it's interpreted as an octal number;
 * if it begins with "1" through "9", then it's interpreted as a decimal number.
 */
static int to_integer (const char *string, const char *description, const int *minimum, const int *maximum) {
   if (*string) {
      /* There is at least one character. */
      if (!isspace(*string)) {
	 /* There are no leading space characters. */
	 char *end;
	 int integer = strtol(string, &end, 0);
	 if (*end == 0) {
	    /* No non-numeric character was encountered. */
	    int acceptable = TRUE;
	    if (minimum) {
	       /* A minimum value has been specified. */
	       if (integer < *minimum) {
		  /* The value is less than the specified minimum. */
		  acceptable = FALSE;
	       }
	    }
	    if (maximum) {
	       /* A maximum value has been specified. */
	       if (integer > *maximum) {
		  /* The value is greater than the specified maximum. */
		  acceptable = FALSE;
	       }
	    }
	    if (acceptable) {
	       /* The value is within an acceptable range. */
	       return integer;
	    }
	 }
      }
   }
   fprintf(stderr, "%s: invalid %s -- %s\n", program_path, description, string);
   syntax_error();
}

/* Process the positional arguments with which the program was invoked.
 * At present, no positional arguments are supported.
 */
static void process_parameters (int argc, char **argv) {
   get_parameters(&argc, &argv);
   if (argc > 0) {
      /* There remains at least one unprocessed positional argument. */
      fprintf(stderr, "%s: too many parameters.\n", program_path);
      syntax_error();
   }
}

/* Interpret the operand of the option currently being processed as a time duration in tenths of seconds. */
static int interval_operand (const char *description) {
   return to_integer(get_operand(), description, &minimum_interval, &maximum_interval) * interval_multiplier;
}

/* Interpret the operand of the option currently being processed as a tone pitch in Herz.
 * If it's "off", then return 0.
 */
static int pitch_operand (const char *description) {
   const char *operand = get_operand();
   if (strcmp(operand, "off") == 0) {
      return 0;
   }
   return to_integer(operand, description, &minimum_pitch, &maximum_pitch);
}

/* Process the optional arguments with which the program was invoked. */
static void process_options (int argc, char **argv) {
   int current_option;
   int exit_immediately = FALSE;
   int help_specified = FALSE;
   int verbose_specified = FALSE;
   int version_specified = FALSE;
   while ((current_option = get_option(argc, argv)) != EOF) {
      switch (current_option) {
	 default: // A new option has been added to the table without adding a corresponding case clause below.
	    fprintf(stderr, "%s: option not implemented -- %c\n", program_path, current_option);
	 case '?': // An unknown option has been specified (error already displayed).
	 case ':': // A required operand has not been supplied (error already displayed).
	    syntax_error();
         case 'a': // --active: Play persistent tone when lock is active.
	    play_mode = play_active;
	    break;
         case 'c': // --caps: Specify the pitch of the caps lock tone.
	    caps_tone.pitch = pitch_operand("caps lock pitch");
	    break;
         case 'd': // --duration: Specify the duration of the last tone of a sequence.
	    final_duration = interval_operand("tone duration");
	    break;
         case 'v': // --verbose: Write lock state changes to standard output.
	    show_changes = TRUE;
            /* fall through */
         case 'f': // foreground: Don't become a daemon.
            foreground_task = 1;
            break;
         case 'h': // --help: Write usage information to standard output, and then exit.
	    help_specified = TRUE;
	    break;
         case 'i': // --insert: Specify the pitch of the insert mode tone.
	    insert_tone.pitch = pitch_operand("insert mode pitch");
	    break;
         case 'n': // --number: Specify the pitch of the number lock tone.
	    number_tone.pitch = pitch_operand("number lock pitch");
	    break;
         case 'p': // --poll: Specify the interval at which the lock states are checked.
	    poll_interval = interval_operand("lock state poll interval");
	    break;
         case 's': // --scroll: Specify the pitch of the scroll lock tone.
	    scroll_tone.pitch = pitch_operand("scroll lock pitch");
	    break;
         case 't': // --toggle: Play activation/deactivation tunes when lock state changes.
	    play_mode = play_toggle;
	    break;
         case 'V': // --version: Write version information to standard output, and then exit.
	    version_specified = TRUE;
	    break;
      }
   }
   if (version_specified) {
      /* The "version" option has been specified. */
      display_version();
      exit_immediately = TRUE;
   }
   if (help_specified) {
      /* The "help" option has been specified. */
      display_usage();
      exit_immediately = TRUE;
   }
   if (exit_immediately) {
      /* At least one of the "information only" options has been specified. */
      exit(0);
   }
}

/* Assign default values to those options which have not been specified on the command line. */
static void assign_defaults () {
   if (initial_duration == 0) {
      initial_duration = final_duration / 2;
   }
}

/* Construct a tune which plays the tones of the supplied tune in the reverse order. */
static const struct tone_entry *invert_tune (const struct tone_entry *tune) {
   struct tone_entry *enut = NULL;
   while (tune) {
      struct tone_entry *tone = allocate_memory(sizeof(*tone));
      *tone = *tune;
      tone->next = enut;
      enut = tone;
      tune = tune->next;
   }
   return enut;
}

/* Construct the lock activation tune. */
static const struct tone_entry *activated_tune (int pitch) {
   struct tone_entry *tune = NULL;
   static const int multiplier_base = 100;
   static const int multiplier_table[] = {0, 25, 50, 100};
   static const int multiplier_count = sizeof(multiplier_table) / sizeof(multiplier_table[0]);
   int multiplier_index;
   for (multiplier_index=multiplier_count; multiplier_index>0;) {
      struct tone_entry *tone = allocate_memory(sizeof(*tone));
      clear_variable(*tone);
      tone->pitch = pitch * (multiplier_base + multiplier_table[--multiplier_index]) / multiplier_base;
      tone->next = tune;
      tune = tone;
   }
   return tune;
}

/* Compose all of the lock activation and deactivation tunes. */
static void compose_tunes () {
   int lock_index;
   for (lock_index=0; lock_index<lock_count; ++lock_index) {
      lock_entry *lock = &lock_table[lock_index];
      int pitch = lock->tone->pitch;
      if (pitch) {
	 /* The pitch for this lock is not "off". */
	 lock->activated = activated_tune(pitch);
	 lock->deactivated = invert_tune(lock->activated);
      }
   }
}

/* The main program. */
int main (int argc, char **argv) {
   identify_program(argc, argv);
   process_options(argc, argv);
   process_parameters(argc, argv);
   assign_defaults();
   compose_tunes();
   monitor_locks(foreground_task, poll_interval);
}
