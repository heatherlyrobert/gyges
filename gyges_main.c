/*============================----beg-of-source---============================*/


#include   "gyges.h"




int          /*-> tbd --------------------------------[ ------ [gn.842.232.99]*/ /*-[01.0000.000.!]-*/ /*-[--.---.---.--]-*/
main (int argc, char *argv[])
{
   /*---(locals)-----------+-----------+-*/
   int         x_loop      = 0;
   int         x_ch        = ' ';      /* current keystroke                   */
   uchar       x_key       = ' ';      /* current keystroke                   */
   char        rc          = 0;
   tTSPEC      x_dur;
   /*---(initialize)---------------------*/
   if (rc >= 0)  rc = yURG_logger  (argc, argv);
   if (rc >= 0)  rc = yURG_urgs    (argc, argv);
   if (rc >= 0)  rc = PROG_init    (argc, argv);
   if (rc >= 0)  rc = PROG_args    (argc, argv);
   if (rc >= 0)  rc = PROG_begin   ();
   if (rc >= 0)  rc = PROG_final   ();
   if (rc <  0) {
      PROG_end ();
      return -1;
   }
   /*---(for timer)------------------------*/
   /*> x_dur.tv_sec    = 0;                                                           <* 
    *> x_dur.tv_nsec   = 0.5 * 1000000;                                               <*/
   /*---(main-loop)----------------------*/
   yVIKEYS_main_curses ("10ms", "10cs");
   /*> DEBUG_TOPS   yLOG_note    ("entering main processing loop");                   <*/
   /*> DEBUG_TOPS   yLOG_break   ();                                                  <*/
   /*> while (g_done) {                                                               <* 
    *>    x_ch = getch ();                                                            <* 
    *>    if (x_ch == KEY_RESIZE)  yVIKEYS_view_resize (0, 0, 0);                     <* 
    *>    if (x_ch < 0)  x_key = 0;                                                   <* 
    *>    else           x_key = x_ch;                                                <* 
    *>    DEBUG_GRAF  yLOG_value   ("x_key"     , x_key);                             <* 
    *>    x_key = yVIKEYS_main_input  (RUN_USER, x_key);                              <* 
    *>    yVIKEYS_main_handle (x_key);                                                <* 
    *>    if (yVIKEYS_quit ())  break;                                                <* 
    *>    ++x_loop;                                                                   <* 
    *>    if ((x_loop % 20) == 0)  yVIKEYS_view_all (0.0);                            <* 
    *>    /+---(sleeping)--------------------+/                                       <* 
    *>    nanosleep    (&x_dur, NULL);                                                <* 
    *>    /+---(done)------------------------+/                                       <* 
    *> }                                                                              <*/
   /*> DEBUG_TOPS  yLOG_break   ();                                                   <*/
   /*> DEBUG_TOPS  yLOG_note    ("exiting main processing loop");                     <*/
   /*---(wrap-up)------------------------*/
   DRAW_wrap    ();
   PROG_end     ();
   /*---(complete)-----------------------*/
   return 0;
}



/*============================----end-of-source---============================*/
