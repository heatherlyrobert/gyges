/*============================----beg-of-source---============================*/


#include   "gyges.h"




int
main (int argc, char *argv[])
{
   /*---(locals)-----------+-----------+-*/
   int         updates     = 0;
   int         cch         = 0;        /* current keystroke                   */
   int         sch         = 0;        /* saved keystroke                     */
   char        rc          = 0;
   char        x_savemode  = '-';
   /*---(initialize)---------------------*/
   if (rc == 0)  rc = PROG_logger  (argc, argv);
   if (rc == 0)  rc = PROG_init    ();
   if (rc == 0)  rc = PROG_urgs    (argc, argv);
   if (rc == 0)  rc = PROG_args    (argc, argv);
   if (rc == 0)  rc = PROG_begin   ();
   if (rc != 0)  {
      PROG_end     ();
      exit (-1);
   }
   /*---(post-initialization)------------*/
   if (rc == 0)  rc = CURS_begin ();
   if (rc != 0)  {
      PROG_end     ();
      exit (-1);
   }
   if (strcmp (f_name, FILE_BLANK) != 0)  FILE_read (f_title);
   hist_active = 'y';
   /*---(process)------------------------*/
   KEYS_col (" r");
   KEYS_row (" r");
   sch = ' ';
   MODE_message (my.mode);
   /*---(main-loop)----------------------*/
   DEBUG_TOPS   yLOG_note    ("entering main processing loop");
   DEBUG_TOPS   yLOG_break   ();
   while (done) {
      /*---(show screen)-----------------*/
      cch = CURS_main ();
      KEYS_record (cch);
      /*---(log)-------------------------*/
      ++updates;
      DEBUG_LOOP  yLOG_complex ("update"    , "loop = %6d, val = %4d, cch = %c", updates, cch, (cch > 32) ? cch : '-');
      /*---(create command display)------*/
      snprintf (cmd,   9, "%02x", cch);
      /*> if      (cch == '%') snprintf (cmd,   9, "  %% ");                          <* 
       *> else if (sch == '%') snprintf (cmd,   9, "%%%c ", cch);                     <* 
       *> else                 snprintf (cmd,   9, "%c%c ", sch, cch);                <*/
      /*---(handle keystroke)------------*/
      x_savemode = my.mode;
      switch (my.mode) {
         /*---(major)--------------------*/
      case MODE_GOD      : rc = MODE_god      (sch, cch); break;
      case MODE_MAP      : rc = MODE_map      (sch, cch); break;
      case MODE_VISUAL   : rc = MODE_map      (sch, cch); break;
      case MODE_SOURCE   : rc = KEYS_source   (sch, cch); break;
      case MODE_INPUT    : rc = KEYS_input    (sch, cch); break;
      case MODE_COMMAND  : rc = KEYS_command  (' ', cch); break;
         /*---(submodes)-----------------*/
      case MODE_FORMAT   : rc = KEYS_format   (' ', cch); break;
      case MODE_BUFFER   : rc = KEYS_buffer   (' ', cch); break;
      case MODE_WANDER   : rc = KEYS_wander   (' ', cch); break;
      case MODE_REGISTER : rc = REG_keys      (sch, cch); break;
      case MODE_MARK     : rc = MARK_mode     (sch, cch); break;
      default            : rc = MODE_map      (sch, cch); break;
      }
      /*---(setup for next keystroke)----*/
      if      (rc == 0)    sch = ' ';
      else if (rc >  0)    sch = cch;
      else               { sch = ' ';  sta_error = 'y'; }
      /*---(setup status line)-----------*/
      if   (x_savemode != my.mode || my.mode == MODE_COMMAND) {
         MODE_message (my.mode);
      }
      /*---(translate unprintable)-------*/
      if      (cch ==  0) strcpy  (cmd,   "n/a");
      else if (cch == 10) strcpy  (cmd,   "ret");
      else if (cch == 27) strcpy  (cmd,   "esc");
      /*---(done)------------------------*/
   }
   DEBUG_TOPS  yLOG_break   ();
   DEBUG_TOPS  yLOG_note    ("exiting main processing loop");
   /*---(wrap-up)------------------------*/
   /*> FILE_write   (f_title);                                                        <*/
   CURS_end     ();
   PROG_end     ();
   /*---(complete)-----------------------*/
   return 0;
}



/*============================----end-of-source---============================*/
