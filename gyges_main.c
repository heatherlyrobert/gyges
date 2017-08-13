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
   if (rc >= 0)  rc = yURG_logger  (argc, argv);
   if (rc >= 0)  rc = PROG_init    (argc, argv);
   if (rc >= 0)  rc = yURG_urgs    (argc, argv);
   if (rc >= 0)  rc = PROG_args    (argc, argv);
   if (rc >= 0)  rc = PROG_begin   ();
   if (rc <  0)  {
      PROG_end     ();
      exit (-1);
   }
   /*---(post-initialization)------------*/
   if (rc == 0)  rc = CURS_begin ();
   if (rc != 0)  {
      PROG_end     ();
      exit (-1);
   }
   /*---(file and screen)----------------*/
   INPT_main (my.f_title);
   KEYS_basics (' ', 'r');
   KEYS_bcol (BCOL);
   CURS_col_head();
   KEYS_brow (BROW);
   CURS_row_head();
   SEQ_calc_full ();
   hist_active = 'y';
   /*---(process)------------------------*/
   MOVE_vert ('r');
   MOVE_horz ('r');
   sch = ' ';
   yVIKEYS_mode_mesg (my.message, g_command);
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
      x_savemode = yVIKEYS_mode_curr ();
      switch (yVIKEYS_mode_curr ()) {
         /*---(major)--------------------*/
      case MODE_GOD      : rc = MODE_god      (sch, cch); break;
      case MODE_MAP      : rc = MODE_map      (sch, cch); break;
      case MODE_VISUAL   : rc = VISU_mode     (sch, cch); break;
      case MODE_SOURCE   : rc = MODE_source   (sch, cch); break;
      case MODE_INPUT    : rc = MODE_input    (sch, cch); break;
      case MODE_COMMAND  : rc = MODE_command  (' ', cch); break;
         /*---(submodes)-----------------*/
      case SMOD_ERROR    : rc = SMOD_error    (sch, cch); break;
      case SMOD_SELECT   : rc = SELC_mode     (sch, cch); break;
      case SMOD_TEXTREG  : rc = TREG_mode     (sch, cch); break;
      case SMOD_REPLACE  : rc = SMOD_replace  (sch, cch); break;
      case SMOD_FORMAT   : rc = SMOD_format   (' ', cch); break;
      case SMOD_BUFFER   : rc = SMOD_buffer   (' ', cch); break;
      case SMOD_WANDER   : rc = SMOD_wander   (' ', cch); break;
      case SMOD_REGISTER : rc = REG_mode      (sch, cch); break;
      case SMOD_MARK     : rc = MARK_mode     (sch, cch); break;
      case SMOD_MENUS    : rc = SMOD_menus    (sch, cch); break;
      default            : rc = MODE_map      (sch, cch); break;
      }
      /*---(translate unprintable)-------*/
      if      (cch ==  0 )  snprintf (cmd,   9, " %c %c " , sch, G_CHAR_NULL );
      else if (cch == 10 )  snprintf (cmd,   9, " %c %c " , sch, G_CHAR_ENTER);
      else if (cch == 27 )  snprintf (cmd,   9, " %c %c " , sch, G_CHAR_ESC  );
      else if (cch == 32 )  snprintf (cmd,   9, " %c %c " , sch, G_CHAR_SPACE);
      else if (cch == 127)  snprintf (cmd,   9, " %c %c " , sch, G_CHAR_BS   );
      else if (cch <= 32 )  snprintf (cmd,   9, " %c %02x", sch, cch);
      else                  snprintf (cmd,   9, " %c %c " , sch, cch);
      /*---(setup for next keystroke)----*/
      if      (rc == 0)    sch = ' ';
      else if (rc >  0)    sch = rc;
      else               { sch = ' ';  sta_error = 'y'; }
      /*---(setup status line)-----------*/
      if   (x_savemode != yVIKEYS_mode_curr() || yVIKEYS_mode_curr() == MODE_COMMAND) {
         yVIKEYS_mode_mesg (my.message, g_command);
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
