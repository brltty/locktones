/* General procedure and type definitions. */
#include "lktMain.h"

/* Unix-specific procedure and type definitions. */
#include "lktUnix.h"

/* Linux-specific includes. */
#include <linux/kd.h>

static int console_descriptor = -1; // A handle to the console I/O subsystem.

/* Get a handle to the console I/O subsystem. */
static void open_console () {
   if ((console_descriptor = open("/dev/tty0", O_WRONLY)) == -1) {
      /* The console device could not be opened. */
      system_error("console open");
   }
}

/* Get all of the needed system resources. */
void get_resources () {
   open_console();
}

/* Start an asynchronous, endless tone. */
int tone_on (pitch) {
   return ioctl(console_descriptor, KIOCSOUND, (1190000 / pitch)) != -1;
}

/* Stop the current tone. */
void tone_off () {
   ioctl(console_descriptor, KIOCSOUND, 0);
}

/* Return the current lock states. */
int get_flags () {
   int flags = 0;
   int leds = 0;
   if (ioctl(console_descriptor, KDGETLED, &leds) != -1) {
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
