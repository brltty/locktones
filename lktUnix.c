/* General procedure and type definitions. */
#include "lktMain.h"

/* Unix-specific procedure and type definitions. */
#include "lktUnix.h"

/* Install a signal handler. */
static void
install_handler (int signal, void (*handler)(int signal)) {
   struct sigaction sa;
   clear_variable(sa);
   sa.sa_handler = handler;
   sigaction(signal, &sa, NULL);
}

/* Handle the alarm signal. */
static void
alarm_handler (int signal) {
   select_tone();
}

/* Place the program into the background. */
static void
become_daemon (void) {
   fflush(stdout);
   fflush(stderr);
   {
      pid_t pid = fork();
      if (pid != 0) {
	 /* This is the parent process. */
	 if (pid == -1) {
	    /* An error occurred. */
	    system_error("process creation");
	 }
	 exit(0);
      }
   }
   fclose(stdin);
   fclose(stdout);
   fclose(stderr);
   setsid();
}

/* Fill in the fields of a "timeval" data structure.
 * Assume that all slack bits have previously been cleared.
 */
static void
set_timeval (struct timeval *tv, int micro_seconds) {
   tv->tv_sec = micro_seconds / 1000000;
   tv->tv_usec = micro_seconds % 1000000;
}

/* Delay program execution for the specified period of time. */
static void
delay_execution (int micro_seconds) {
   struct timeval tv;
   clear_variable(tv);
   set_timeval(&tv, micro_seconds);
   select(0, NULL, NULL, NULL, &tv);
}

/* Monitor the locks in the background. */
void
monitor_locks (int foreground_task, int poll_interval) {
   get_resources();
   install_handler(SIGALRM, alarm_handler);
   if (!foreground_task) become_daemon();

   /* Loop forever. */
   while (TRUE) {
      check_flags(get_flags());
      delay_execution(poll_interval);
   }
}

/* Schedule an alarm signal. */
static void
set_alarm (int interval) {
   struct itimerval itv;
   clear_variable(itv);
   set_timeval(&itv.it_value, interval);
   setitimer(ITIMER_REAL, &itv, NULL);
}

/* Cancel any pending alarm signal. */
static void
cancel_alarm (void) {
   struct itimerval itv;
   clear_variable(itv);
   setitimer(ITIMER_REAL, &itv, NULL);
}

/* Start the specified tone. */
void
start_tone (int pitch, int duration) {
   if (tone_on(pitch)) {
      /* The tone has been started. */
      set_alarm(duration);
   }
}

/* Stop the current tone. */
void
stop_tone (void) {
   cancel_alarm();
   tone_off();
}
