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
   char        x_macro     [LEN_RECD] = "llljjhs123\n";
   char        x_mpos      = 0;
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
      cch = PROG_main_input (RUN_USER, '-');
      if (cch == -1) continue;
      /*---(log)-------------------------*/
      ++updates;
      DEBUG_LOOP  yLOG_complex ("update"    , "loop = %6d, val = %4d, cch = %c", updates, cch, (cch > 32) ? cch : '-');
      /*---(handle keystroke)------------*/
      rc = PROG_main_handle (cch);
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
