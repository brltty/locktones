/* General procedure and type definitions. */
#include "lktMain.h"

/* DJGPP-specific procedure and type definitions. */
#include "lktDjgpp.h"

int _stklen = 0X2000; // The minimum size of the run-time stack.
int _crt0_startup_flags = _CRT0_FLAG_USE_DOS_SLASHES| // Preserve backslashes in argv[0].
                          _CRT0_FLAG_PRESERVE_UPPER_CASE| // Preserve case of argv[0].
			  _CRT0_FLAG_LOCK_MEMORY; // Lock all memory as it is allocated.

static volatile int poll_length = 0; // Lock state poll interval in ticks.
static volatile int poll_left = 0; // Ticks left until next lock state poll.

static volatile int tone_left = 0; // Ticks left for current tone.

/* Return the current lock states. */
static int get_flags () {
   int flags = 0;
   int status = _bios_keybrd(_KEYBRD_SHIFTSTATUS);
   if (status & 0X10) {
      /* The scroll lock is active. */
      flags |= scroll_flag;
   }
   if (status & 0X20) {
      /* The num lock is active. */
      flags |= num_flag;
   }
   if (status & 0X40) {
      /* The caps lock is active. */
      flags |= caps_flag;
   }
   if (status & 0X80) {
      /* Insert mode is active. */
      flags |= insert_flag;
   }
   return flags;
}

/* Handle clock ticks. */
static void tick_handler () {
   if (poll_left) {
      /* The poll interval has not yet expired. */
      --poll_left;
   } else {
      /* The poll interval has expired. */
      poll_left = poll_length - 1;
      check_flags(get_flags());
   }
   if (tone_left) {
      /* The tone timer is active. */
      if (--tone_left == 0) {
	 /* The current tone has expired. */
	 select_tone();
      }
   }
}

/* Install an interrupt handler. */
static void install_handler (int interrupt, void (*handler)(void)) {
   _go32_dpmi_seginfo si;
   clear_variable(si);
   si.pm_selector = _my_cs();
   si.pm_offset = (unsigned)handler;
   _go32_dpmi_chain_protected_mode_interrupt_vector(interrupt, &si);
}

/* Terminate the program but leave it resident. */
static void stay_resident () {
   __dpmi_regs regs;
   clear_variable(regs);
   regs.x.ax = 0x3100; // INT21[AX]: TSR=0X31, PgmRetCode=0X00
   regs.x.dx = (256) / 16; // INT21[DX]: Paragraphs (16-byte blocks) to keep.
   printf("Installing TSR %s %s.\n", program_path, program_version);
   fclose(stdin);
   fclose(stdout);
   fclose(stderr);
   __djgpp_exception_toggle();
   __dpmi_int(0x21, &regs);
}

/* Monitor the locks in the background. */
void monitor_locks (int foreground_task, int poll_interval) {
   poll_length = tick_count(poll_interval);
   install_handler(8, &tick_handler);
   stay_resident();
}

/* Start the specified tone. */
void start_tone (int pitch, int duration) {
   sound(pitch);
   tone_left = tick_count(duration);
}

/* Stop the current tone. */
void stop_tone () {
   tone_left = 0;
   nosound();
}
