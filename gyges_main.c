/*============================----beg-of-source---============================*/


#include   "gyges.h"




int          /*-> tbd --------------------------------[ ------ [gn.842.232.99]*/ /*-[01.0000.000.!]-*/ /*-[--.---.---.--]-*/
main (int argc, char *argv[])
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
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
   DEBUG_PROG   yLOG_note     ("initial tab creation");
   TAB_new_in_abbr ('0', NULL, NULL);
   TAB_new_in_abbr ('®', NULL, NULL);
   TAB_new_in_abbr ('¯', NULL, NULL);
   TAB_switch      (0);
   /*---(main-loop)----------------------*/
   rc = yVICURSES_main  ("keys", "every", NULL);
   /*---(wrap-up)------------------------*/
   DRAW_wrap    ();
   PROG_end     ();
   /*---(complete)-----------------------*/
   return 0;
}



/*============================----end-of-source---============================*/
