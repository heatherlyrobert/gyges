/*============================----beg-of-source---============================*/


#include   "gyges.h"




int          /*-> tbd --------------------------------[ ------ [gn.842.232.99]*/ /*-[01.0000.000.!]-*/ /*-[--.---.---.--]-*/
main                    (int a_argc, char *a_argv [])
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   /*---(debugging)----------------------*/
   rc = PROG_urgents (a_argc, a_argv);
   DEBUG_PROG   yLOG_value    ("urgents"   , rc);
   if (rc <  0) { PROG_shutdown (); return -1; }
   /*---(initialization)-----------------*/
   rc = PROG_startup (a_argc, a_argv);
   DEBUG_PROG   yLOG_value    ("startup"   , rc);
   if (rc <  0) { PROG_shutdown (); return -2; }
   /*---(visual setup)-------------------*/
   rc = PROG_dawn    ();
   DEBUG_PROG   yLOG_value    ("dawn"      , rc);
   if (rc <  0) { PROG_shutdown (); return -3; }
   /*---(main-loop)----------------------*/
   rc = yVICURSES_main  ("keys", "every", NULL);
   DEBUG_PROG   yLOG_value    ("main"      , rc);
   rc = PROG_dusk     ();
   DEBUG_PROG   yLOG_value    ("dusk"      , rc);
   /*---(wrap-up)------------------------*/
   rc = PROG_shutdown ();
   /*---(complete)-----------------------*/
   return 0;
}



/*============================----end-of-source---============================*/
