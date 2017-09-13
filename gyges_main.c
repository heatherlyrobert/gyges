/*============================----beg-of-source---============================*/


#include   "gyges.h"




int
main (int argc, char *argv[])
{
   /*---(locals)-----------+-----------+-*/
   int         updates     = 0;
   char        cch         = ' ';      /* current keystroke                   */
   char        sch         = ' ';      /* saved keystroke                     */
   char        x_play      = ' ';      /* playback keystroke                  */
   char        rc          = 0;
   char        x_savemode  = '-';
   char        x_macro     [LEN_RECD] = "llljjhs123\n";
   char        x_mpos      = 0;
   struct timespec x_delay;
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
   INPT_main     ();
   KEYS_basics   (' ', 'r');
   KEYS_bcol     (BCOL);
   CURS_col_head ();
   KEYS_brow     (BROW);
   CURS_row_head ();
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
      DEBUG_LOOP   yLOG_note    ("top of main event loop");
      DEBUG_LOOP   yLOG_char    ("mode"      , my.mode_operating);
      DEBUG_LOOP   yLOG_char    ("delay"     , my.macro_delay);
      DEBUG_LOOP   yLOG_value   ("pos"       , my.macro_pos);
      switch (my.mode_operating) {
      case MACRO_OFF        :
         DEBUG_LOOP   yLOG_note    ("run_normal");
         cch = CURS_main  ();
         break;
      case MACRO_RECORD     :
         DEBUG_LOOP   yLOG_note    ("run_recording");
         cch = CURS_main  ();
         break;
      case MACRO_RUN        :
      case MACRO_DELAY      :
      case MACRO_PLAYBACK   :
         DEBUG_LOOP   yLOG_note    ("run_macro, run_playback, or run_delay");
         KEYS_macro_get  ();
         DEBUG_LOOP   yLOG_note    ("read macro keystroke");
         cch = KEYS_macro_curr ('-');
         DEBUG_LOOP   yLOG_value   ("cch"       , cch);
         if (cch <  0) {
            switch (256 + cch) {
            case G_CHAR_WAIT    : sleep (1);                           break;
            case G_CHAR_BREAK   : my.mode_operating = MACRO_PLAYBACK;  break;
            case G_CHAR_HALT    : KEYS_macro_reset (); cch = 0;        break;
            case G_CHAR_DISPLAY : CURS_screen_reset ();                break;
            default             : KEYS_macro_reset (); cch = 0;        break;
            }
         }
         if (cch == 0) {
            x_savemode = -1;
            cch        =  0;
            break;
         }
         DEBUG_LOOP   yLOG_note    ("show screen");
         CURS_main  ();
         IF_MACRO_DELAY {
            DEBUG_LOOP   yLOG_note    ("process a delay");
            switch (my.macro_delay) {
            case '9' : x_delay.tv_sec = 3; x_delay.tv_nsec =         0; break;
            case '8' : x_delay.tv_sec = 2; x_delay.tv_nsec =         0; break;
            case '7' : x_delay.tv_sec = 1; x_delay.tv_nsec = 750000000; break;
            case '6' : x_delay.tv_sec = 1; x_delay.tv_nsec = 500000000; break;
            case '5' : x_delay.tv_sec = 1; x_delay.tv_nsec = 250000000; break;
            case '4' : x_delay.tv_sec = 1; x_delay.tv_nsec =         0; break;
            case '3' : x_delay.tv_sec = 0; x_delay.tv_nsec = 750000000; break;
            case '2' : x_delay.tv_sec = 0; x_delay.tv_nsec = 500000000; break;
            case '1' : x_delay.tv_sec = 0; x_delay.tv_nsec = 250000000; break;
            case '0' : x_delay.tv_sec = 0; x_delay.tv_nsec =         0; break;
            default  : x_delay.tv_sec = 0; x_delay.tv_nsec =         0; break;
            }
            nanosleep (&x_delay, NULL);
         }
         DEBUG_LOOP   yLOG_note    ("read playback keystroke");
         x_play = CURS_playback ();
         DEBUG_LOOP   yLOG_value   ("x_play"    , x_play);
         switch (x_play) {
         case '.'      :
            DEBUG_LOOP   yLOG_note    ("user entered dot (.)");
            if      (my.mode_operating == MACRO_PLAYBACK)  {
               DEBUG_LOOP   yLOG_note    ("change playback to delay");
               my.mode_operating = MACRO_DELAY;
            }
            IF_MACRO_DELAY {
               DEBUG_LOOP   yLOG_note    ("change delay to playback");
               my.mode_operating = MACRO_PLAYBACK;
               continue;
            }
            break;
         case '<'      :
            if (my.macro_delay > '0')  --my.macro_delay;
            continue;
            break;
         case '>'      :
            if (my.macro_delay < '9')  ++my.macro_delay;
            continue;
            break;
         case K_ESCAPE :
            DEBUG_LOOP   yLOG_note    ("user entered escape");
            KEYS_macro_reset ();
            cch = 0;
            break;
         case K_RETURN :
            DEBUG_LOOP   yLOG_note    ("user entered return");
            my.mode_operating = MACRO_RUN;
            break;
         }
         break;
      }
      DEBUG_LOOP   yLOG_note    ("handle keystroke normally");
      KEYS_record (cch);
      /*---(log)-------------------------*/
      ++updates;
      DEBUG_LOOP  yLOG_complex ("update"    , "loop = %6d, val = %4d, cch = %c", updates, cch, (cch > 32) ? cch : '-');
      /*---(handle keystroke)------------*/
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
      case SMOD_MACRO    : rc = SMOD_macro    (sch, cch); break;
      default            : rc = MODE_map      (sch, cch); break;
      }
      /*---(translate unprintable)-------*/
      if      (cch == 0       )  snprintf (cmd,   9, " %c %c " , sch, G_CHAR_NULL  );
      else if (cch == K_RETURN)  snprintf (cmd,   9, " %c %c " , sch, G_CHAR_RETURN);
      else if (cch == K_ESCAPE)  snprintf (cmd,   9, " %c %c " , sch, G_CHAR_ESCAPE);
      else if (cch == K_TAB   )  snprintf (cmd,   9, " %c %c " , sch, G_CHAR_TAB   );
      else if (cch == K_BS    )  snprintf (cmd,   9, " %c %c " , sch, G_CHAR_BS    );
      else if (cch == K_SPACE )  snprintf (cmd,   9, " %c %c " , sch, G_CHAR_SPACE );
      else if (cch <= K_SPACE )  snprintf (cmd,   9, " %c %02x", sch, cch);
      else                       snprintf (cmd,   9, " %c %c " , sch, cch);
      /*---(setup for next keystroke)----*/
      if      (rc == 0)    sch = ' ';
      else if (rc >  0)    sch = rc;
      else               { sch = ' ';  sta_error = 'y'; }
      /*---(setup status line)-----------*/
      if   (x_savemode != yVIKEYS_mode_curr() || yVIKEYS_mode_curr() == MODE_COMMAND) {
         yVIKEYS_mode_mesg (my.message, g_command);
      }
      IF_MACRO_PLAYBACK {
         strlcpy (my.message, "[M@] RUN : macro execution mode", LEN_DESC);
      }
      x_savemode = yVIKEYS_mode_curr ();
      IF_MACRO_ON  ++my.macro_pos;
      /*---(done)------------------------*/
   }
   DEBUG_TOPS  yLOG_break   ();
   DEBUG_TOPS  yLOG_note    ("exiting main processing loop");
   /*---(wrap-up)------------------------*/
   /*> FILE_write   ();                                                        <*/
   CURS_end     ();
   PROG_end     ();
   /*---(complete)-----------------------*/
   return 0;
}



/*============================----end-of-source---============================*/
