/* General includes. */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

/* Boolean constants. */
#define TRUE (0 == 0)
#define FALSE (!TRUE)

/* Clear (set to zero) an entire data structure to insure that all slack bits have predictable values. */
#define clear_variable(variable) (memset(&(variable), 0, sizeof((variable))))

/* Platform-independent procedures. */
extern void syntax_error (void);
extern void system_error (const char *action);
extern void select_tone (void);
extern void check_flags (int flags);

/* Platform-dependent procedures. */
extern void monitor_locks (int foreground_task, int poll_interval);
extern void start_tone (int pitch, int duration);
extern void stop_tone (void);
extern char get_option (int argc, char **argv);
extern const char *get_operand (void);
extern void get_parameters (int *argcp, char ***argvp);

/* A platform-independent flag bit for each lock. */
#define caps_flag   0X1
#define number_flag 0X2
#define scroll_flag 0X4
#define insert_flag 0X8

extern const char *program_path; // The full path to the executing binary.
extern const char *program_name; // The name (last path component) of the executing binary.
extern const char *program_version; // The version of the executing program.
extern int show_changes;
