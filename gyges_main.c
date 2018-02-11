/*============================----beg-of-source---============================*/


#include   "gyges.h"




int          /*-> tbd --------------------------------[ ------ [gn.842.232.99]*/ /*-[01.0000.000.!]-*/ /*-[--.---.---.--]-*/
main (int argc, char *argv[])
{
   /*---(locals)-----------+-----------+-*/
   int         x_loop      = 0;
   uchar       x_key       = ' ';      /* current keystroke                   */
   char        rc          = 0;
   /*---(initialize)---------------------*/
   if (rc >= 0)  rc = yURG_logger  (argc, argv);
   if (rc >= 0)  rc = PROG_init    (argc, argv);
   if (rc >= 0)  rc = yURG_urgs    (argc, argv);
   if (rc >= 0)  rc = PROG_args    (argc, argv);
   if (rc >= 0)  rc = PROG_begin   ();
   if (rc >= 0)  rc = PROG_final   ();
   if (rc <  0) {
      PROG_end ();
      return -1;
   }
   /*---(main-loop)----------------------*/
   DEBUG_TOPS   yLOG_note    ("entering main processing loop");
   DEBUG_TOPS   yLOG_break   ();
   while (g_done) {
      x_key = yVIKEYS_main_input  (RUN_USER, x_key);
      yVIKEYS_main_handle (x_key);
      if (yVIKEYS_quit ())  break;
      ++x_loop;
      x_key = CURS_main  ();
      /*---(done)------------------------*/
   }
   DEBUG_TOPS  yLOG_break   ();
   DEBUG_TOPS  yLOG_note    ("exiting main processing loop");
   /*---(wrap-up)------------------------*/
   CURS_end     ();
   PROG_end     ();
   /*---(complete)-----------------------*/
   return 0;
}



/*============================----end-of-source---============================*/
