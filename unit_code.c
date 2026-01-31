/*============================----beg-of-source---============================*/
#include   "gyges.h"


char         /*-> set up programgents/debugging ------[ light  [uz.320.011.05]*/ /*-[00.0000.00#.#]-*/ /*-[--.---.---.--]-*/
PROG__unit_quiet     (void)
{
   char        rce         =  -10;
   char        rc          =    0;
   int         x_argc      =    1;
   char       *x_args [1]  = { "gyges" };
   /*---(urgents)------------------------*/
   rc = PROG_urgents  (x_argc, x_args);
   DEBUG_PROG   yLOG_value    ("urgents"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(startup)------------------------*/
   rc = PROG_startup  (x_argc, x_args);
   DEBUG_PROG   yLOG_value    ("startup"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> set up programgents/debugging ------[ light  [uz.320.011.05]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
PROG__unit_loud      (void)
{
   char        rce         =  -10;
   char        rc          =    0;
   int         x_argc      =  4;
   char       *x_args [10] = { "gyges_unit", "@@kitchen", "@@ycalc", ""      };
   /*---(urgents)------------------------*/
   rc = PROG_urgents  (x_argc, x_args);
   DEBUG_PROG   yLOG_value    ("urgents"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   yURG_name   ("tops"  , YURG_ON);
   yURG_name   ("prog"  , YURG_ON);
   yURG_name   ("cell"  , YURG_ON);
   yURG_name   ("regs"  , YURG_ON);
   /*> yURG_name   ("map"   , YURG_ON);                                               <*/
   yURG_name   ("deps"  , YURG_ON);
   /*> yURG_name   ("ystr"  , YURG_ON);                                               <*/
   /*> yURG_name   ("args"  , YURG_ON);                                               <*/
   /*> yURG_name   ("apis"  , YURG_ON);                                               <*/
   /*> yURG_name   ("ycalc" , YURG_ON);                                               <*/
   /*> yURG_name   ("exec"  , YURG_ON);                                               <*/
   /*> yURG_name   ("adjs"  , YURG_ON);                                               <*/
   yURG_name   ("srcp"  , YURG_ON);
   yURG_name   ("locs"  , YURG_ON);
   /*> yURG_name   ("yparse", YURG_ON);                                               <*/
   yURG_name   ("ymode" , YURG_ON);
   yURG_name   ("ykeys" , YURG_ON);
   yURG_name   ("ymacro", YURG_ON);
   yURG_name   ("ycmd"  , YURG_ON);
   yURG_name   ("ysrc"  , YURG_ON);
   /*> yURG_name   ("srch"  , YURG_ON);                                               <*/
   yURG_name   ("ymap"  , YURG_ON);
   /*> yURG_name   ("yview" , YURG_ON);                                               <*/
   /*> yURG_name   ("hist"  , YURG_ON);                                               <*/
   /*> yURG_name   ("sort"  , YURG_OFF);                                              <*/
   /*> yURG_name   ("ysort" , YURG_ON);                                               <*/
   /*---(startup)------------------------*/
   rc = PROG_startup  (x_argc, x_args);
   DEBUG_PROG   yLOG_value    ("startup"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> set up program urgents/debugging ---[ light  [uz.210.001.01]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
PROG__unit_end       (void)
{
   char        rce         =  -10;
   char        rc          =    0;
   rc = PROG_shutdown  ();
   DEBUG_PROG   yLOG_value    ("shutdown"  , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
PROG__unit_cleanse   (void)
{
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   PROG_cleanse ();
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}
