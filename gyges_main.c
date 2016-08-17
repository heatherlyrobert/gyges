/*============================----beg-of-source---============================*/


#include   "gyges.h"




int
main (int argc, char *argv[])
{
   /*---(locals)-----------+-----------+-*/
   int         updates     = 0;
   int         cch         = ' ';      /* current keystroke                   */
   int         sch         = ' ';      /* saved keystroke                     */
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
   if (strcmp (my.f_name, FILE_BLANK) != 0)  INPT_main (my.f_title);
   hist_active = 'y';
   /*---(process)------------------------*/
   MOVE_vert ('r');
   MOVE_horz ('r');
   sch = ' ';
   MODE_message ();
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
      /*---(handle keystroke)------------*/
      x_savemode = MODE_curr ();
      switch (MODE_curr ()) {
         /*---(major)--------------------*/
      case MODE_GOD      : rc = MODE_god      (sch, cch); break;
      case MODE_MAP      : rc = MODE_map      (sch, cch); break;
      case MODE_VISUAL   : rc = VISU_mode     (sch, cch); break;
      case MODE_SOURCE   : rc = MODE_source   (sch, cch); break;
      case MODE_INPUT    : rc = MODE_input    (sch, cch); break;
      case MODE_COMMAND  : rc = MODE_command  (' ', cch); break;
         /*---(submodes)-----------------*/
      case SMOD_SELECT   : rc = SELC_mode     (sch, cch); break;
      case SMOD_TEXTREG  : rc = TREG_mode     (sch, cch); break;
      case SMOD_REPLACE  : rc = SMOD_replace  (sch, cch); break;
      case SMOD_FORMAT   : rc = SMOD_format   (' ', cch); break;
      case SMOD_BUFFER   : rc = SMOD_buffer   (' ', cch); break;
      case SMOD_WANDER   : rc = SMOD_wander   (' ', cch); break;
      case SMOD_REGISTER : rc = REG_mode      (sch, cch); break;
      case SMOD_MARK     : rc = MARK_mode     (sch, cch); break;
      default            : rc = MODE_map      (sch, cch); break;
      }
      /*---(translate unprintable)-------*/
      if      (cch ==  0 )  snprintf (cmd,   9, " %c %c " , sch, 216);
      else if (cch == 10 )  snprintf (cmd,   9, " %c %c " , sch, 182);
      else if (cch == 27 )  snprintf (cmd,   9, " %c %c " , sch, 234);
      else if (cch == 32 )  snprintf (cmd,   9, " %c %c " , sch, 223);
      else if (cch == 127)  snprintf (cmd,   9, " %c %c " , sch, 171);
      else if (cch <= 32 )  snprintf (cmd,   9, " %c %02x", sch, cch);
      else                  snprintf (cmd,   9, " %c %c " , sch, cch);
      /*---(setup for next keystroke)----*/
      if      (rc == 0)    sch = ' ';
      else if (rc >  0)    sch = rc;
      else               { sch = ' ';  sta_error = 'y'; }
      /*---(setup status line)-----------*/
      if   (x_savemode != MODE_curr() || MODE_curr() == MODE_COMMAND) {
         MODE_message ();
      }
      /*---(done)------------------------*/
   }
   DEBUG_TOPS  yLOG_break   ();
   DEBUG_TOPS  yLOG_note    ("exiting main processing loop");
   /*---(wrap-up)------------------------*/
   /*> FILE_write   (my.f_title);                                                        <*/
   CURS_end     ();
   PROG_end     ();
   /*---(complete)-----------------------*/
   return 0;
}



/*============================----end-of-source---============================*/
