/* General procedure and type definitions. */
#include "lktMain.h"

/* Unix-specific procedure and type definitions. */
#include "lktUnix.h"

/* Generic I/O controls. */
#include <sys/ioctl.h>

/* Linux-specific includes. */
#include <linux/kd.h>
#include <linux/vt.h>

static int console_descriptor = -1; // A handle to the console I/O subsystem.
static int console_number = -1;     /* The foreground console number */

/* Get a handle to the console I/O subsystem. */
static void
open_console (void) {
   if ((console_descriptor = open("/dev/tty0", O_WRONLY)) == -1) {
      /* The console device could not be opened. */
      system_error("console open");
   }
}

/* Get all of the needed system resources. */
void
get_resources (void) {
   open_console();
}

/* Start an asynchronous, endless tone. */
int
tone_on (int pitch) {
   return ioctl(console_descriptor, KIOCSOUND, (1190000 / pitch)) != -1;
}

/* Stop the current tone. */
void
tone_off (void) {
   ioctl(console_descriptor, KIOCSOUND, 0);
}

/* Return the current lock states. */
int
get_flags (void) {
   int flags = 0;
   unsigned char leds = 0;
   struct vt_stat state;

   if (console_descriptor < 0)
      return 0;

   if (ioctl(console_descriptor, VT_GETSTATE, &state) != -1 &&
       state.v_active != console_number) {
      /* Reopen the console to get to the foreground console */
      close(console_descriptor);
      open_console();
      console_number = state.v_active;
   }

   if (ioctl(console_descriptor, KDGKBLED, &leds) != -1) {
      /* The query of the lock states was successful. */

      if (leds & LED_CAP) {
	 /* The caps lock is active. */
         flags |= caps_flag;
      }

      if (leds & LED_NUM) {
	 /* The num lock is active. */
         flags |= num_flag;
      }

      if (leds & LED_SCR) {
	 /* The scroll lock is active. */
         flags |= scroll_flag;
      }
   }

   return flags;
}
