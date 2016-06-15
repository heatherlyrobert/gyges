/*============================----beg-of-source---============================*/

#include   "gyges.h"


struct cACCESSOR my;


int     save = 0;

int     done = 1;
char    contents[MAX_STR] = "";
char    saved   [MAX_STR] = "";
char    cmd[10] = "";
char    message [MAX_STR] = "";
char    command [MAX_STR] = "";
char    mode       = 'N';
char    special = 0;

char        empty       [200] = "                                                                                                                                                                                                       ";
char        dashes      [200] = "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";
char        equals      [200] = "=======================================================================================================================================================================================================";
char        unders      [200] = "_______________________________________________________________________________________________________________________________________________________________________________________________________";
char        dots        [200] = ".......................................................................................................................................................................................................";
char        pluses      [200] = "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";


/*===[[ local prototypes ]]=====================*/
char        KEYS__end          (char a_dir);
char        KEYS__horz         (char a_type, char a_move);



char          /* PURPOSE : clear out recent editing --------------------------*/
clear_input   (void)
{
   strncpy(saved   , contents, MAX_STR);
   strncpy(contents, ""      , MAX_STR);
   return 0;
}

char          /* PURPOSE : place recent input into the right cell ------------*/
save_saved    (void)
{
   strncpy(contents, saved, MAX_STR);
   CELL_change (CHG_INPUT, CTAB, CCOL, CROW, contents);
   return 0;
}




/*====================------------------------------------====================*/
/*===----                       ncurses related                        ----===*/
/*====================------------------------------------====================*/
PRIV void  o___ODD_MOVES_______o () { return; }

char         /*-->-move-to-the-very-edge-of-used-space----[-leaf---[--------]-*/
MOVE__edge         (
      /*----------+-----------+-----------------------------------------------*/
      char        a_dir)      /* direction code (see design notes)            */
{  /*---(design notes)--------------------------------------------------------*/
   /*
    *   a very powerful movement to quickly see the extent of the populated
    *   space and to get to any one of the four edges, alone or in combination.
    *   maximum extent for any row or col is shown with up, down, left, right.
    *   maximum extent in curr row or col is shown with  K  J     H     L
    */
   /*---(locals)-----------+-----------+-*/
   int         y_fin       = 0;
   int         x_fin       = 0;
   char        rce         = -10;                /* return code for errors    */
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_char    ("a_dir"     , a_dir);
   /*---(defense)------------------------*/
   if (strchr ("tbseKJHL", a_dir) == 0)                         return rce;
   /*---(prepare)------------------------*/
   y_fin = CROW;
   x_fin = CCOL;
   /*---(execute)------------------------*/
   switch (a_dir) {
   case 't' : case 'K' :
      /*---(top)-------------------------*/
      for (y_fin = 0; y_fin < NROW; ++y_fin) {
         if (a_dir == 't' && tab->rows [y_fin].c == 0)          continue;
         if (a_dir == 'K' && tab->sheet[CCOL][y_fin] == NULL)   continue;
         break;
      }
      break;
   case 'b' : case 'J' :
      /*---(bottom)----------------------*/
      for (y_fin = NROW - 1; y_fin >= 0; --y_fin) {
         if (a_dir == 'b' && tab->rows [y_fin].c == 0)          continue;
         if (a_dir == 'J' && tab->sheet[CCOL][y_fin] == NULL)   continue;
         break;
      }
      break;
   case 's' : case 'H' :
      /*---(left)------------------------*/
      for (x_fin = 0; x_fin < NCOL; ++x_fin) {
         if (a_dir == 's' && tab->cols [x_fin].c == 0)          continue;
         if (a_dir == 'H' && tab->sheet[x_fin][CROW] == NULL)   continue;
         break;
      }
      break;
   case 'e' : case 'L' :
      /*---(bottom)----------------------*/
      for (x_fin = NCOL - 1; x_fin >= 0; --x_fin) {
         if (a_dir == 'e' && tab->cols [x_fin].c == 0)          continue;
         if (a_dir == 'L' && tab->sheet[x_fin][CROW] == NULL)   continue;
         break;
      }
      break;
   }
   /*---(fix overruns)-------------------*/
   if (y_fin <  0   )  y_fin = 0;
   if (y_fin >= NROW)  y_fin = NROW - 1;
   if (x_fin <  0   )  x_fin = 0;
   if (x_fin >= NCOL)  x_fin = NCOL - 1;
   /*---(update)-------------------------*/
   DEBUG_USER  yLOG_value   ("x_fin"     , x_fin);
   DEBUG_USER  yLOG_value   ("y_fin"     , y_fin);
   CCOL = x_fin;
   CROW = y_fin;
   /*---(complete)-----------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
KEYS__end          (char a_dir)
{
   /*---(locals)-----------+-----------+-*/
   char        x_type      = '-';      /* '-' = empty, 'f' = filled           */
   int         x_done      = '-';      /* complete indicator                  */
   int         x_cur       = 0;
   int         x_inc       = 0;
   int         x_fin       = 0;
   int         y_cur       = 0;
   int         y_inc       = 0;
   int         y_fin       = 0;
   char        rce         = -10;           /* return code for error          */
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_char    ("a_dir"     , a_dir);
   /*---(set up direction)---------------*/
   switch (a_dir) {
   case 'r' :  x_inc =  1;  y_inc =  0; break;
   case 'l' :  x_inc = -1;  y_inc =  0; break;
   case 'd' :  x_inc =  0;  y_inc =  1; break;
   case 'u' :  x_inc =  0;  y_inc = -1; break;
   default  :
               DEBUG_USER  yLOG_note    ("invalid move code passed to function");
               DEBUG_USER  yLOG_exit    (__FUNCTION__);
               return rce;
   }
   DEBUG_USER  yLOG_value   ("x_inc"     , x_inc);
   DEBUG_USER  yLOG_value   ("y_inc"     , y_inc);
   /*---(initialize)---------------------*/
   x_fin = x_cur = CCOL;
   y_fin = y_cur = CROW;
   DEBUG_USER  yLOG_value   ("x_cur"     , x_cur);
   DEBUG_USER  yLOG_value   ("y_cur"     , y_cur);
   /*---(check current)------------------*/
   if (tab->sheet[x_cur][y_cur] != NULL) {
      x_type = 'f';
      if (tab->sheet[x_cur + x_inc][y_cur + y_inc] == NULL)  x_type = '-';
   }
   DEBUG_USER  yLOG_char    ("x_type"    , x_type);
   /*---(process cells)------------------*/
   while (1) {
      /*---(update)----------------------*/
      x_cur += x_inc;
      y_cur += y_inc;
      DEBUG_USER  yLOG_value   ("x_cur"     , x_cur);
      DEBUG_USER  yLOG_value   ("y_cur"     , y_cur);
      /*---(check limits)----------------*/
      if (x_cur <  0   )  { x_fin = 0       ; break; }
      if (x_cur >= NCOL)  { x_fin = NCOL - 1; break; }
      if (y_cur <  0   )  { y_fin = 0       ; break; }
      if (y_cur >= NROW)  { y_fin = NROW - 1; break; }
      /*---(test)------------------------*/
      switch (x_type) {
      case  '-' :
         if (tab->sheet[x_cur][y_cur] != NULL) {
            DEBUG_USER  yLOG_note    ("found a occupied cell");
            x_fin  = x_cur;
            y_fin  = y_cur;
            x_done = 'y';
         }
         break;
      case  'f' :
         if (tab->sheet[x_cur][y_cur] == NULL) {
            DEBUG_USER  yLOG_note    ("found a null cell");
            x_fin  = x_cur - x_inc;
            y_fin  = y_cur - y_inc;
            x_done = 'y';
         }
         break;
      }
      if (x_done == 'y')  break;
   }
   /*---(update)-------------------------*/
   DEBUG_USER  yLOG_value   ("x_fin"     , x_fin);
   DEBUG_USER  yLOG_value   ("y_fin"     , y_fin);
   CCOL = x_fin;
   CROW = y_fin;
   /*---(complete)-----------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> move left or right by columns ---------[--------[--------]-*/
KEYS__horz         (char a_type, char a_move)
{
   /*---(locals)-------------------------*/
   int         i           = 0;             /* iterator -- horizontal pos     */
   int         x_target    = 0;
   int         x_cum       = 0;
   int         x_col       = 0;
   char        rce         = -10;           /* return code for error          */
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_char    ("a_type"    , a_type);
   DEBUG_USER  yLOG_char    ("a_move"    , a_move);
   /*---(set target)---------------------*/
   --rce;
   DEBUG_USER  yLOG_value   ("my.x_avail", my.x_avail);
   x_target = my.x_left;
   switch (a_move) {
   case 's' :  x_target += 0;                        break;
   case 'h' :  x_target += my.x_avail / 4;           break;
   case ',' :  x_target += my.x_avail / 2;           break;
   case 'l' :  x_target += (my.x_avail / 4) * 3;     break;
   case 'e' :  x_target += my.x_avail;               break;
   default  :
               DEBUG_USER  yLOG_note    ("invalid move code passed to function");
               DEBUG_USER  yLOG_exit    (__FUNCTION__);
               return rce;
   }
   DEBUG_USER  yLOG_value   ("x_target"  , x_target);
   /*---(find column containing target)--*/
   if (a_type == 'g') {
      x_col = 0;
      for (i = BCOL; i <= ECOL; ++i) {
         DEBUG_USER  yLOG_complex ("checking"  , "col %3d at %3d", i, tab->cols[i].x);
         if (tab->cols[i].x <= x_target)  {
            x_col = i;
            continue;
         }
         DEBUG_USER  yLOG_note    ("just passed");
         break;
      }
      CCOL = x_col;
   }
   if (a_type == 'z') {
      if      (a_move == 's')  BCOL = CCOL;
      else if (a_move == 'e')  ECOL = CCOL;
      else {
         BCOL = CCOL;
         x_cum   = x_target - (tab->cols[CCOL].w / 2);
         x_col   = 0;
         for (i = CCOL - 1; i >= 0; --i) {
            x_cum -= tab->cols[i].w;
            DEBUG_USER  yLOG_complex ("checking"  , "col %3d wid %3d cum %3d", i, tab->cols[i].w, x_cum);
            if (x_cum > 0) {
               x_col = i;
               continue;
            }
            DEBUG_USER  yLOG_note    ("just passed");
            break;
         }
         BCOL = x_col;
      }
   }
   /*---(update current colunn)----------*/
   DEBUG_USER  yLOG_value   ("my.bcol"   , BCOL);
   DEBUG_USER  yLOG_value   ("tab->ccol" , CCOL);
   DEBUG_USER  yLOG_value   ("my.ecol"   , ECOL);
   /*---(complete)-----------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
KEYS_col           (char *a_keys)
{
   /*---(locals)-------------------------*/
   int         s_beg       = 0;             /* save the beginning col         */
   int         s_end       = 0;             /* save the ending col            */
   char        x_mod       = ' ';           /* modifier, such as 'g' or 'z'   */
   char        x_key       = ' ';           /* actual key                     */
   char        rce         = -10;           /* return code for error          */
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_point   ("a_keys"    , a_keys);
   /*---(defense: keystrokes)------------*/
   --rce;  // null input string (maximum dangerous)
   if (a_keys == NULL) {
      DEBUG_USER  yLOG_note    ("null key string passed to function");
      DEBUG_USER  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER  yLOG_info    ("a_keys"    , a_keys);
   --rce;  // wrong number of characters (tight input)
   if (strlen (a_keys) != 2) {
      DEBUG_USER  yLOG_note    ("wrong length key string passed to function");
      DEBUG_USER  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   x_mod   = a_keys[0];
   x_key   = a_keys[1];
   DEBUG_USER  yLOG_complex ("key combo" , "modifier %c (%3d), key %c (%3d)", x_mod, x_mod, x_key, x_key);
   /*---(defense: modifier)--------------*/
   --rce;  // illegal value for modifier (tight input)
   if (strchr (" cgze", x_mod) == NULL) {
      DEBUG_USER  yLOG_note    ("unrecognized modifier passed to function");
      DEBUG_USER  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(init)------------------------*/
   s_beg   = BCOL;
   s_end   = ECOL;
   /*---(adjust curr)-----------------*/
   --rce;  // bad basic request
   if (x_mod == ' ') switch (x_key) {
   case '0' : CCOL = 0;                     break;
   case 'H' : CCOL -= 5;                    break;
   case 'h' : --CCOL;                       break;
   case 'l' : ++CCOL;                       break;
   case 'L' : CCOL += 5;                    break;
   case '$' : CCOL = NCOL - 1;              break;
   case 'o' : /* see above           */  break;    /* add                     */
   case 'x' : /* see above           */  break;    /* delete                  */
   case 'w' : /* see above           */  break;    /* delete                  */
   case 'r' : if (CCOL > ECOL)
                 ECOL = CCOL;
              else if (CCOL < BCOL)
                 BCOL = CCOL;
              else
                 s_beg = -1;
              break;
   default  :
              DEBUG_USER  yLOG_complex ("ERROR"     , "normal mode key %c (%3d) not handled", x_key, x_key);
              DEBUG_USER  yLOG_exit    (__FUNCTION__);
              return rce;
              break;
   }
   /*---(page moves)---------------------*/
   --rce;  // bad page request
   if (x_mod == 'c') switch (x_key) {
   case 's' : ECOL = CCOL = BCOL;          break;
   case 'h' : KEYS__horz ('c', 'h');       break;
   case 'l' : KEYS__horz ('c', 'l');       break;
   case 'e' : BCOL = CCOL = ECOL;          break;
   default  :
              DEBUG_USER  yLOG_complex ("ERROR"     , "control mode key %c (%3d) not handled", x_key, x_key);
              DEBUG_USER  yLOG_exit    (__FUNCTION__);
              return rce;
              break;
   }
   /*---(screen moves)-------------------*/
   --rce;  // bad goto request
   if (x_mod == 'g') switch (x_key) {
   case 's' : KEYS__horz (x_mod, x_key);              break;
   case 'h' : KEYS__horz (x_mod, x_key);              break;
   case ',' : KEYS__horz (x_mod, x_key);              break;
   case 'l' : KEYS__horz (x_mod, x_key);              break;
   case 'e' : KEYS__horz (x_mod, x_key);              break;
   default  :
              DEBUG_USER  yLOG_complex ("ERROR"     , "goto mode key %c (%3d) not handled", x_key, x_key);
              DEBUG_USER  yLOG_exit    (__FUNCTION__);
              return rce;
              break;
   }
   /*---(scrolling)----------------------*/
   --rce;  // bad scroll request
   if (x_mod == 'z') switch (x_key) {
   case 's' : KEYS__horz (x_mod, x_key);              break;
   case 'h' : KEYS__horz (x_mod, x_key);              break;
   case ',' : KEYS__horz (x_mod, x_key);              break;
   case 'l' : KEYS__horz (x_mod, x_key);              break;
   case 'e' : KEYS__horz (x_mod, x_key);              break;
   default  :
              DEBUG_USER  yLOG_complex ("ERROR"     , "scroll mode key %c (%3d) not handled", x_key, x_key);
              DEBUG_USER  yLOG_exit    (__FUNCTION__);
              return rce;
              break;
   }
   /*---(end)----------------------------*/
   --rce;  // bad end request
   if (x_mod == 'e') switch (x_key) {
   case 'h' : KEYS__end  ('l');                       break;
   case 'l' : KEYS__end  ('r');                       break;
   case 'H' : MOVE__edge ('H');                       break;
   case 's' : MOVE__edge ('s');                       break;
   case 'L' : MOVE__edge ('L');                       break;
   case 'e' : MOVE__edge ('e');                       break;
   default  :
              DEBUG_USER  yLOG_complex ("ERROR"     , "end mode key %c (%3d) not handled", x_key, x_key);
              DEBUG_USER  yLOG_exit    (__FUNCTION__);
              return rce;
              break;
   }
   /*---(check lock)------------------*/
   if (tab->froz_col == 'y') {
      DEBUG_USER  yLOG_note    ("correct for locked areas");
      if (BCOL <= tab->froz_ecol)   BCOL   = tab->froz_ecol + 1;
      if (CCOL <= tab->froz_ecol)   CCOL   = tab->froz_ecol + 1;
      if (ECOL <= tab->froz_ecol)   ECOL   = tab->froz_ecol + 1;
   }
   /*---(check min/max)---------------*/
   DEBUG_USER  yLOG_note    ("correct for min/max violations");
   if (CCOL <  0        )    CCOL =    0;
   if (CCOL >= NCOL)    CCOL = NCOL - 1;
   /*---(check bcol)------------------*/
   DEBUG_USER  yLOG_note    ("correct for beginning violations");
   if (BCOL <  0        )      BCOL   =    0;
   if (BCOL >= NCOL)      BCOL   = NCOL - 1;
   /*---(check ecol)------------------*/
   DEBUG_USER  yLOG_note    ("correct for ending violations");
   if (ECOL <  0        )      ECOL   =    0;
   if (ECOL >= NCOL)      ECOL   = NCOL - 1;
   /*---(adjust screen)---------------*/
   DEBUG_USER  yLOG_note    ("adust screen if beg or end changed");
   if (CCOL < BCOL)       BCOL = CCOL;
   if (CCOL > ECOL)       ECOL = CCOL;
   /*---(update contents)-------------*/
   DEBUG_USER  yLOG_note    ("update current contents");
   if (tab->sheet[CCOL][CROW] != NULL && tab->sheet[CCOL][CROW]->s != NULL) {
      strncpy (contents, tab->sheet[CCOL][CROW]->s, MAX_STR);
   } else {
      strncpy (contents, ""                  , MAX_STR);
   }
   /*---(check for selection)---------*/
   DEBUG_USER  yLOG_note    ("update selection if necessary");
   if (x_key   != 'w')  SEL_update   (CTAB, CCOL, CROW);
   /*---(check for update)------------*/
   DEBUG_USER  yLOG_note    ("update column headings if necessary");
   if      (BCOL != s_beg) { KEYS_bcol (BCOL); CURS_colhead(); }
   else if (ECOL != s_end) { KEYS_ecol (ECOL); CURS_colhead(); }
   /*---(complete)--------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

int          /*--> firgure out size of locked columns --- [ ------ [ ------ ]-*/
KEYS_lcol          (void)
{
   /*---(locals)-------------------------*/
   int       i         = 0;                    /* iterator -- cols            */
   int       w         = 0;                    /* col width                   */
   int       cw        = 0;                    /* cumulative col width        */
   /*---(process cols)-------------------*/
   if (tab->froz_col != 'y')  return 0;
   for (i = tab->froz_bcol; i <= tab->froz_ecol; ++i) {
      w               = tab->cols[i].w;
      if (cw + w >  my.x_avail)   break;
      tab->cols[i].x  = cw + my.x_left;
      cw             += w;
      if (cw == my.x_avail)       break;
   }
   /*---(complete)-----------------------*/
   return cw;
}

char         /*--> layout screen starting from left ----- [ ------ [ ------ ]-*/
KEYS_bcol          (int a_beg)
{
   /*---(locals)-------------------------*/
   int       i         = 0;                    /* iterator -- cols            */
   int       j         = 0;                    /* iterator -- cols            */
   int       w         = 0;                    /* col width                   */
   int       cw        = 0;                    /* cumulative col width        */
   char      x_done    = '-';
   int       x_1st     = 0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_value   ("a_beg"     , a_beg);
   /*---(defense)------------------------*/
   BCOL = a_beg;
   if (BCOL <     0       )  BCOL = 0;
   if (tab->froz_col == 'y' && BCOL <= tab->froz_ecol)  BCOL = tab->froz_ecol + 1;
   if (BCOL >= NCOL)  BCOL = NCOL - 1;
   ECOL = BCOL;
   DEBUG_USER  yLOG_value   ("tab->bcol"   , BCOL);
   DEBUG_USER  yLOG_value   ("tab->ccol" , CCOL);
   DEBUG_USER  yLOG_value   ("tab->ecol"   , ECOL);
   DEBUG_USER  yLOG_value   ("avail"     , my.x_avail);
   /*---(process cols)-------------------*/
   if (tab->froz_col == 'y')  x_1st = tab->froz_ecol + 1;
   for (i = BCOL; i >= x_1st; --i) {
      for (j = 0; j < NCOL; ++j) tab->cols[j].x = 0;
      cw = KEYS_lcol ();
      for (j = i; j < NCOL; ++j) {
         w          = tab->cols[j].w;
         DEBUG_USER  yLOG_complex ("compare"   , "beg %3d, end %3d, cw %3d, w %3d", i, j, cw, w);
         if (cw + w >  my.x_avail) {
            DEBUG_USER  yLOG_note    ("hit the max");
            x_done = 'y';
            break;
         }
         tab->cols[j].x  = cw + my.x_left;
         cw        += w;
         ECOL    = j;
         if (cw == my.x_avail) {
            DEBUG_USER  yLOG_note    ("exact match");
            x_done = 'y';
            break;
         }
      }
      if (x_done == 'y') {
         BCOL = i;
         break;
      }
   }
   DEBUG_USER  yLOG_value   ("tab->bcol"   , BCOL);
   DEBUG_USER  yLOG_value   ("tab->ccol" , CCOL);
   DEBUG_USER  yLOG_value   ("tab->ecol"   , ECOL);
   /*---(complete)-----------------------*/
   clear ();
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> layout screen starting from right -----[--------[--------]-*/
KEYS_ecol          (int a_end)
{
   /*---(locals)-----------------------------*/
   int       i         = 0;                    /* iterator -- rows            */
   int       j         = 0;                    /* iterator -- rows            */
   int       w         = 0;                    /* col width                   */
   int       cw        = 0;                    /* cumulative col width        */
   char      x_done    = '-';
   int       x_1st     = 0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_value   ("a_end"     , a_end);
   /*---(defense)----------------------------*/
   ECOL = a_end;
   if (ECOL <     0) ECOL = 0;
   if (ECOL >= NCOL) ECOL = NCOL - 1;
   BCOL = ECOL;
   DEBUG_USER  yLOG_value   ("tab->bcol"   , BCOL);
   DEBUG_USER  yLOG_value   ("tab->ccol" , CCOL);
   DEBUG_USER  yLOG_value   ("tab->ecol"   , ECOL);
   DEBUG_USER  yLOG_value   ("avail"     , my.x_avail);
   /*---(process cols)-----------------------*/
   if (tab->froz_col == 'y')  x_1st = tab->froz_ecol + 1;
   for (i = ECOL; i < NCOL; ++i) {
      for (j = x_1st; j < NCOL; ++j) tab->cols[j].x = 0;
      cw = KEYS_lcol ();
      for (j = i; j >= x_1st; --j) {
         w  = tab->cols[j].w;
         DEBUG_USER  yLOG_complex ("compare"   , "beg %3d, end %3d, cw %3d, w %3d", j, i, cw, w);
         if (cw + w >  my.x_avail) {
            DEBUG_USER  yLOG_note    ("hit the max");
            x_done = 'y';
            break;
         }
         cw        += w;
         tab->cols[j].x  = my.x_avail - cw + my.x_left;
         BCOL       = j;
         if (cw == my.x_avail) {
            DEBUG_USER  yLOG_note    ("exact match");
            x_done = 'y';
            break;
         }
      }
      if (x_done == 'y') {
         ECOL = i;
         break;
      }
   }
   DEBUG_USER  yLOG_value   ("tab->bcol"   , BCOL);
   DEBUG_USER  yLOG_value   ("tab->ccol" , CCOL);
   DEBUG_USER  yLOG_value   ("tab->ecol"   , ECOL);
   /*---(fix x values)-----------------------*/
   cw = my.x_left + KEYS_lcol ();
   for (i = BCOL; i <= ECOL; ++i) {
      tab->cols[i].x  = cw;
      cw += tab->cols[i].w;
   }
   /*---(test end)---------------------------*/
   for (i = ECOL + 1; i < NCOL; ++i) {
      w  = tab->cols[i].w;
      if (cw + w >  my.x_avail)  break;
      DEBUG_USER  yLOG_value   ("can fit"   , i);
      tab->cols[i].x  = cw;
      cw += tab->cols[i].w;
      ECOL = i;
   }
   DEBUG_USER  yLOG_value   ("tab->ecol"   , ECOL);
   /*---(complete)---------------------------*/
   clear ();
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
KEYS_pcol          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i;
   char        x_name      [100]       = "";
   char        rce         = -10;
   FILE       *x_file      = NULL;
   /*---(open)---------------------------*/
   snprintf (x_name, 95, "%s.cols", f_title);
   x_file = fopen(x_name, "w");
   --rce;
   if (x_file == NULL)      return rce;
   /*---(print)--------------------------*/
   fprintf (x_file, "details of column settings\n\n");
   fprintf (x_file, "lcol  = %c\n" , tab->froz_col);
   fprintf (x_file, "lbcol = %3d\n", tab->froz_bcol);
   fprintf (x_file, "lecol = %3d\n", tab->froz_ecol);
   fprintf (x_file, "bcol  = %3d\n", BCOL);
   fprintf (x_file, "ecol  = %3d\n", ECOL);
   fprintf (x_file, "ncol  = %3d\n", NCOL);
   fprintf (x_file, "avail = %3d\n", my.x_avail);
   fprintf (x_file, "ccol  = %3d\n", CCOL);
   for (i = 0; i < NCOL; ++i) {
      if (i % 5 == 0)  fprintf (x_file, "\ncol   label---   -c-   -w-   -x-\n");
      fprintf (x_file, "%3d   %-8s   %3d   %3d   %3d\n", i, tab->cols[i].l, tab->cols[i].c, tab->cols[i].w, tab->cols[i].x);
   }
   fprintf (x_file, "\ndone\n");
   /*---(close)--------------------------*/
   fclose (x_file);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> move up or down by rows ---------------[--------[--------]-*/
KEYS__vert         (char a_type, char a_move)
{
   /*---(locals)-------------------------*/
   int         i           = 0;             /* iterator -- horizontal pos     */
   int         y_target    = 0;
   int         y_cum       = 0;
   int         y_row       = 0;
   char        rce         = -10;           /* return code for error          */
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_char    ("a_type"    , a_type);
   DEBUG_USER  yLOG_char    ("a_move"    , a_move);
   /*---(set target)---------------------*/
   --rce;
   DEBUG_USER  yLOG_value   ("my.y_avail", my.y_avail);
   y_target = row_main;
   switch (a_move) {
   case 't' :  y_target += 0;                        break;
   case 'k' :  y_target += my.y_avail / 4;           break;
   case '.' :  y_target += (my.y_avail / 2) - 1;     break;
   case 'j' :  y_target += (my.y_avail / 4) * 3;     break;
   case 'b' :  y_target += my.y_avail;               break;
   default  :
               DEBUG_USER  yLOG_note    ("invalid move code passed to function");
               DEBUG_USER  yLOG_exit    (__FUNCTION__);
               return rce;
   }
   DEBUG_USER  yLOG_value   ("y_target"  , y_target);
   /*---(find column containing target)--*/
   if (a_type == 'g') {
      y_row = 0;
      for (i = BROW; i <= EROW; ++i) {
         DEBUG_USER  yLOG_complex ("checking"  , "row %3d at %3d", i, tab->rows[i].y);
         if (tab->rows[i].y <= y_target)  {
            y_row = i;
            continue;
         }
         DEBUG_USER  yLOG_note    ("just passed");
         break;
      }
      CROW = y_row;
   }
   if (a_type == 'z') {
      if      (a_move == 't')  BROW = CROW;
      else if (a_move == 'b')  EROW = CROW;
      else {
         BROW = CROW;
         y_cum   = y_target - (tab->rows[CROW].h / 2);
         y_row   = 0;
         for (i = CROW - 1; i >= 0; --i) {
            y_cum -= tab->rows[i].h;
            DEBUG_USER  yLOG_complex ("checking"  , "row %3d hei %3d cum %3d", i, tab->rows[i].h, y_cum);
            if (y_cum > 0) {
               y_row = i;
               continue;
            }
            DEBUG_USER  yLOG_note    ("just passed");
            break;
         }
         BROW = y_row;
      }
   }
   /*---(update current colunn)----------*/
   DEBUG_USER  yLOG_value   ("tab->brow"   , BROW);
   DEBUG_USER  yLOG_value   ("tab->crow" , CROW);
   DEBUG_USER  yLOG_value   ("tab->erow"   , EROW);
   /*---(complete)-----------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

char             /* [------] handle row movement input -----------------------*/
KEYS_row           (char *a_keys)
{
   /*---(locals)-------------------------*/
   char        x_mod       = ' ';           /* modifier, such as 'g' or 'z'   */
   char        x_key       = ' ';           /* actual key                     */
   int         s_beg       = BROW;       /* save the beginning row         */
   int         s_end       = EROW;       /* save the ending row            */
   int         s_cur       = CROW;          /* save the current row           */
   char        rce         = -10;           /* return code for error          */
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_point   ("a_keys"    , a_keys);
   /*---(defenses)-----------------------*/
   --rce;  // null input string (maximum dangerous)
   if (a_keys == NULL) {
      DEBUG_USER  yLOG_note    ("null key string passed to function");
      DEBUG_USER  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER  yLOG_info    ("a_keys"    , a_keys);
   --rce;  // wrong number of characters (tight input)
   if (strlen (a_keys) != 2) {
      DEBUG_USER  yLOG_note    ("wrong length key string passed to function");
      DEBUG_USER  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   x_mod   = a_keys[0];
   x_key   = a_keys[1];
   DEBUG_USER  yLOG_complex ("key combo" , "modifier %c (%3d), key %c (%3d)", x_mod, x_mod, x_key, x_key);
   /*---(defense: modifier)--------------*/
   --rce;  // illegal value for modifier (tight input)
   if (strchr (" cgze", x_mod) == NULL) {
      DEBUG_USER  yLOG_note    ("unrecognized modifier passed to function");
      DEBUG_USER  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check adds)------------------*/
   --rce;  // bad add request
   if        (x_key == 'o') {
      ++NROW;
      if (NROW <     1) NROW =    1;
      if (NROW >  MAX_ROWS) NROW = MAX_ROWS;
      s_end = -1;
      CROW  = NROW;
   } else if (x_key == 'x') {
      --NROW;
      if (NROW <     1) NROW =    1;
      if (NROW >  MAX_ROWS) NROW = MAX_ROWS;
      s_beg = -1;
      CROW  = 0;
   }
   /*---(adjust curr)-----------------*/
   --rce;  // bad basic request
   if (x_mod == ' ') switch (x_key) {
   case '_' : CROW = 0;                     break;    /* line moves              */
   case 'K' : CROW -= 5;                    break;
   case 'k' : --CROW;                       break;
   case 'j' : ++CROW;                       break;
   case 'J' : CROW += 5;                    break;
   case 'G' : CROW = NROW - 1;         break;
   case 'o' : /* see above           */          break;    /* add                     */
   case 'x' : /* see above           */          break;    /* delete                  */
   case 'r' : if (CROW > EROW)
                 EROW = CROW;
              else if (CROW < BROW)
                 BROW = CROW;
              else
                 s_beg = -1;
              break;
   default  :
              DEBUG_USER  yLOG_complex ("ERROR"     , "normal mode key %c (%3d) not handled", x_key, x_key);
              DEBUG_USER  yLOG_exit    (__FUNCTION__);
              return rce;
              break;
   }
   /*---(screen moves)-------------------*/
   --rce;  // bad goto request
   if (x_mod == 'g') switch (x_key) {
   case 't' : KEYS__vert (x_mod, x_key);              break;
   case 'k' : KEYS__vert (x_mod, x_key);              break;
   case '.' : KEYS__vert (x_mod, x_key);              break;
   case 'j' : KEYS__vert (x_mod, x_key);              break;
   case 'b' : KEYS__vert (x_mod, x_key);              break;
   default  :
              DEBUG_USER  yLOG_complex ("ERROR"     , "goto mode key %c (%3d) not handled", x_key, x_key);
              DEBUG_USER  yLOG_exit    (__FUNCTION__);
              return rce;
              break;
   }
   /*---(scrolling)----------------------*/
   --rce;  // bad scroll request
   if (x_mod == 'z') switch (x_key) {
   case 't' : KEYS__vert (x_mod, x_key);              break;
   case 'k' : KEYS__vert (x_mod, x_key);              break;
   case '.' : KEYS__vert (x_mod, x_key);              break;
   case 'j' : KEYS__vert (x_mod, x_key);              break;
   case 'b' : KEYS__vert (x_mod, x_key);              break;
   default  :
              DEBUG_USER  yLOG_complex ("ERROR"     , "scroll mode key %c (%3d) not handled", x_key, x_key);
              DEBUG_USER  yLOG_exit    (__FUNCTION__);
              return rce;
              break;
   }
   /*---(control keys)---------s---------*/
   --rce;  // bad control request
   if (x_mod == 'c') switch (x_key) {
   case 't' : EROW =  CROW = BROW;         break;
   case 'b' : BROW =  CROW = EROW;         break;
   default  :
              DEBUG_USER  yLOG_complex ("ERROR"     , "control mode key %c (%3d) not handled", x_key, x_key);
              DEBUG_USER  yLOG_exit    (__FUNCTION__);
              return rce;
              break;
   }
   /*---(control keys)---------s---------*/
   --rce;  // bad control request
   if (x_mod == 'e') switch (x_key) {
   case 'k' : KEYS__end  ('u');                       break;
   case 'j' : KEYS__end  ('d');                       break;
   case 'K' : MOVE__edge ('K');                       break;
   case 't' : MOVE__edge ('t');                       break;
   case 'J' : MOVE__edge ('J');                       break;
   case 'b' : MOVE__edge ('b');                       break;
   default  :
              DEBUG_USER  yLOG_complex ("ERROR"     , "end mode key %c (%3d) not handled", x_key, x_key);
              DEBUG_USER  yLOG_exit    (__FUNCTION__);
              return rce;
              break;
   }
   /*---(check lock)------------------*/
   if (tab->froz_row == 'y') {
      DEBUG_USER  yLOG_note    ("correct for locked areas");
      if (BROW <= tab->froz_erow)   BROW   = tab->froz_erow + 1;
      if (CROW <= tab->froz_erow)   CROW   = tab->froz_erow + 1;
      if (EROW <= tab->froz_erow)   EROW   = tab->froz_erow + 1;
   }
   /*---(check min/max)---------------*/
   DEBUG_USER  yLOG_note    ("correct for min/max violations");
   if (CROW <     0     )    CROW =    0;
   if (CROW >= NROW)    CROW = NROW - 1;
   /*---(check brow)------------------*/
   DEBUG_USER  yLOG_note    ("correct for beginning violations");
   if (BROW <     0     )      BROW   =    0;
   if (BROW >= NROW)      BROW   = NROW - 1;
   /*---(check erow)------------------*/
   DEBUG_USER  yLOG_note    ("correct for ending violations");
   if (EROW <     0     )      EROW   =    0;
   if (EROW >= NROW)      EROW   = NROW - 1;
   /*---(adjust screen)---------------*/
   DEBUG_USER  yLOG_note    ("adust screen if beg or end changed");
   if (CROW < BROW)       BROW   = CROW;
   if (CROW > EROW)       EROW   = CROW;
   /*---(update contents)-------------*/
   DEBUG_USER  yLOG_note    ("update current contents");
   if (tab->sheet[CCOL][CROW] != NULL && tab->sheet[CCOL][CROW]->s != NULL) {
      strncpy(contents, tab->sheet[CCOL][CROW]->s, MAX_STR);
   } else {
      strncpy(contents, ""                  , MAX_STR);
   }
   /*---(check for selection)---------*/
   DEBUG_USER  yLOG_note    ("update selection if necessary");
   SEL_update  (CTAB, CCOL, CROW);
   /*---(check for update)------------*/
   DEBUG_USER  yLOG_note    ("update row headings if necessary");
   if      (BROW != s_beg) { KEYS_brow (BROW); CURS_rowhead(); }
   else if (EROW != s_end) { KEYS_erow (EROW); CURS_rowhead(); }
   /*> else if (CROW != s_cur) CURS_rowhead();                                   <*/
   /*---(complete)--------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

int          /*--> firgure out size of locked rows ------ [ ------ [ ------ ]-*/
KEYS_lrow          (void)
{
   /*---(locals)-------------------------*/
   int         i           = 0;             /* iterator -- rows            */
   int         h           = 1;             /* row height                  */
   int         ch          = 0;             /* cumulative row height       */
   /*---(process cols)-------------------*/
   if (tab->froz_row != 'y')  return 0;
   for (i = tab->froz_brow; i <= tab->froz_erow; ++i) {
      h               = tab->rows[i].h;
      if (ch + h >  my.y_avail)   break;
      tab->rows[i].y  = ch + row_main;
      ch             += h;
      if (ch == my.y_avail)       break;
   }
   /*---(complete)-----------------------*/
   return ch;
}

char               /* PURPOSE : find the ending row given the beginning one   */
KEYS_brow          (int a_beg)
{
   /*---(locals)-----------+-------------*/
   int         i           = 0;             /* iterator -- rows            */
   int         j           = 0;             /* iterator -- rows            */
   int         h           = 1;             /* row height                  */
   int         ch          = 0;             /* cumulative row height       */
   char        x_done      = '-';
   int         x_1st       = 0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_value   ("a_beg"     , a_beg);
   /*---(defense)------------------------*/
   BROW = a_beg;
   if (BROW <     0) BROW = 0;
   if (tab->froz_row == 'y' && BROW <= tab->froz_erow)  BROW = tab->froz_erow + 1;
   if (BROW >= NROW) BROW = NROW - 1;
   EROW = BROW;
   DEBUG_USER  yLOG_value   ("tab->brow"   , BROW);
   DEBUG_USER  yLOG_value   ("tab->crow" , CROW);
   DEBUG_USER  yLOG_value   ("tab->erow"   , EROW);
   DEBUG_USER  yLOG_value   ("y_avail"   , my.y_avail);
   /*---(process beginning row)----------*/
   if (tab->froz_row == 'y')  x_1st = tab->froz_erow + 1;
   for (i = BROW; i >= x_1st; --i) {
      /*---(initialize row positions)----*/
      for (j = 0; j < NROW; ++j) tab->rows[j].y = 0;
      ch = KEYS_lrow ();
      /*---(check for ending row)--------*/
      for (j = i; j < NROW; ++j) {
         /*---(prepare)------------------*/
         h  = tab->rows[j].h;
         DEBUG_USER  yLOG_complex ("compare"   , "beg %3d, end %3d, ch %3d, h %3d", i, j, ch, h);
         /*---(check for too far)--------*/
         if (ch + h >  my.y_avail) {
            DEBUG_USER  yLOG_note    ("hit the max");
            x_done = 'y';
            break;
         }
         /*---(update)-------------------*/
         tab->rows[j].y  = ch + row_main;
         ch        += h;
         EROW    = j;
         /*---(check for just right)-----*/
         if (ch == my.y_avail) {
            DEBUG_USER  yLOG_note    ("exact match");
            x_done = 'y';
            break;
         }
         /*---(bottom)-------------------*/
      }
      /*---(check for done)--------------*/
      if (x_done == 'y') {
         BROW = i;
         break;
      }
      /*---(bottom)----------------------*/
   }
   /*---(show values)------------------------*/
   DEBUG_USER  yLOG_value   ("tab->brow"   , BROW);
   DEBUG_USER  yLOG_value   ("tab->crow" , CROW);
   DEBUG_USER  yLOG_value   ("tab->erow"   , EROW);
   /*---(complete)---------------------------*/
   clear ();
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

char               /* PURPOSE : find the beginning row given the ending one   */
KEYS_erow          (int a_end)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;             /* iterator -- rows            */
   int         j           = 0;             /* iterator -- rows            */
   int         h           = 1;             /* row height                  */
   int         ch          = 0;             /* cumulative row height       */
   char        x_done      = '-';
   int         x_1st       = 0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_value   ("a_end"     , a_end);
   /*---(defense)----------------------------*/
   EROW = a_end;
   if (EROW <     0) EROW = 0;
   if (EROW >= NROW) EROW = NROW - 1;
   BROW = EROW;
   DEBUG_USER  yLOG_value   ("tab->brow"   , BROW);
   DEBUG_USER  yLOG_value   ("tab->crow" , CROW);
   DEBUG_USER  yLOG_value   ("tab->erow"   , EROW);
   DEBUG_USER  yLOG_value   ("y_avail"   , my.y_avail);
   /*---(process ending row)-------------*/
   if (tab->froz_row == 'y')  x_1st = tab->froz_erow + 1;
   for (i = EROW; i < NROW; ++i) {
      /*---(initialize row positions)----*/
      for (j = 0; j < NROW; ++j) tab->rows[j].y = 0;
      ch = KEYS_lrow ();
      /*---(check for beginning row)-----*/
      for (j = i; j >= x_1st; --j) {
         /*---(prepare)------------------*/
         h  = tab->rows[j].h;
         DEBUG_USER  yLOG_complex ("compare"   , "beg %3d, end %3d, ch %3d, h %3d", j, i, ch, h);
         /*---(check for too far)--------*/
         if (ch + h >  my.y_avail) {
            DEBUG_USER  yLOG_note    ("hit the max");
            x_done = 'y';
            break;
         }
         /*---(update)-------------------*/
         ch        += h;
         tab->rows[j].y  = my.y_avail - ch + row_main;
         BROW       = j;
         /*---(check for just right)-----*/
         if (ch == my.x_avail) {
            DEBUG_USER  yLOG_note    ("exact match");
            x_done = 'y';
            break;
         }
         /*---(bottom)-------------------*/
      }
      /*---(check for done)--------------*/
      if (x_done == 'y') {
         EROW = i;
         break;
      }
      /*---(bottom)----------------------*/
   }
   /*---(show values)------------------------*/
   DEBUG_USER  yLOG_value   ("tab->brow"   , BROW);
   DEBUG_USER  yLOG_value   ("tab->crow" , CROW);
   DEBUG_USER  yLOG_value   ("tab->erow"   , EROW);
   /*---(fix y values)-----------------------*/
   ch = KEYS_lrow ();
   for (i = BROW; i <= EROW; ++i) {
      tab->rows[i].y  = ch + row_main;
      ch += tab->rows[i].h;
   }
   /*---(test end)---------------------------*/
   for (i = EROW + 1; i < NROW; ++i) {
      h  = tab->rows[i].h;
      if (ch + h >  my.y_avail)  break;
      DEBUG_USER  yLOG_value   ("can fit"   , i);
      tab->rows[i].y  = ch + row_main;
      ch += tab->rows[i].h;
      EROW = i;
   }
   DEBUG_USER  yLOG_value   ("tab->erow"   , EROW);
   /*---(complete)---------------------------*/
   clear ();
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
KEYS_prow          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i;
   char        x_name      [100]       = "";
   char        rce         = -10;
   FILE       *x_file      = NULL;
   /*---(open)---------------------------*/
   snprintf (x_name, 95, "%s.rows", f_title);
   x_file = fopen(x_name, "w");
   --rce;
   if (x_file == NULL)      return rce;
   /*---(print)--------------------------*/
   fprintf (x_file, "details of row settings\n\n");
   fprintf (x_file, "lrow  = %c\n" , tab->froz_row);
   fprintf (x_file, "lbrow = %3d\n", tab->froz_brow);
   fprintf (x_file, "lerow = %3d\n", tab->froz_erow);
   fprintf (x_file, "brow  = %3d\n", BROW);
   fprintf (x_file, "erow  = %3d\n", EROW);
   fprintf (x_file, "nrow  = %3d\n", NROW);
   fprintf (x_file, "avail = %3d\n", my.y_avail);
   fprintf (x_file, "crow  = %3d\n", CROW);
   for (i = 0; i < NROW; ++i) {
      if (i % 5 == 0)  fprintf (x_file, "\nrow   -c-   -h-   -y-\n");
      fprintf (x_file, "%3d   %3d   %3d   %3d\n", i, tab->rows[i].c, tab->rows[i].h, tab->rows[i].y);
   }
   fprintf (x_file, "\ndone\n");
   /*---(close)--------------------------*/
   fclose (x_file);
   /*---(complete)-----------------------*/
   return 0;
}

char       /*----: move forward by a word ------------------------------------*/
word_fore          (void)
{
   /*---(locals)-------------------------*/
   int    i;
   /*---(get to end of word)-------------*/
   for (i = my.cpos; i < my.npos; ++i) {
      if (contents[i]     == '\0')  break;
      my.cpos = i;
      if (contents[i]     != ' ' )  continue;
      break;
   }
   /*---(get to end of gap)--------------*/
   for (i = my.cpos; i < my.npos; ++i) {
      if (contents[i]     == ' ' )  continue;
      if (contents[i]     == '\0')  break;
      my.cpos = i;
      break;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char               /* PURPOSE : debugging row number assignments -------------*/
row_print          (void)
{
   /*---(locals)-----------------------------*/
   int       i         = 0;                    /* iterator -- rows            */
   /*---(process rows)-----------------------*/
   for (i = BROW; i <= EROW; ++i) {
      printf("display %3d = row %3d %s\n", tab->rows[i].y, i, (i - BROW + 2 == tab->rows[i].y) ? "good" : "FAILED");
   }
   /*---(complete)---------------------------*/
   return 0;
}

char       /*----: move back by a word ---------------------------------------*/
word_back          (void)
{
   /*---(locals)-------------------------*/
   int    i;
   int    save = my.cpos;
   /*---(get to beg of word)-------------*/
   /*> printf("start 1 at %2d (%c)\n", save, contents[save]);                         <*/
   for (i = save - 1; i >= 0; --i) {
      if (contents[i]     == ' ' )  continue;
      save = i;
      break;
   }
   /*---(get to end of gap)--------------*/
   /*> printf("start 2 at %2d (%c)\n", save, contents[save]);                         <*/
   for (i = save; i >= 0; --i) {
      if (i == 0 && contents[i] != ' ') {
         my.cpos = 0;
         break;
      }
      if (contents[i]     != ' ' )  continue;
      my.cpos = i + 1;
      break;
   }
   /*> printf("done    at %2d (%c)\n", cpos, contents[cpos]);                         <*/
   /*---(complete)-----------------------*/
   return 0;
}

char       /*----: move forward to next end of a word ------------------------*/
word_end           (void)
{
   /*---(locals)-------------------------*/
   int    i;
   int    save = my.cpos;
   /*---(get to end of gap)--------------*/
   for (i = save + 1; i < my.npos; ++i) {
      if (contents[i]     == ' ' )  continue;
      if (contents[i]     == '\0')  break;
      save = i;
      break;
   }
   /*---(get to end of word)-------------*/
   for (i = save; i < my.npos; ++i) {
      if (i == my.npos - 1 && contents[i] != ' ') {
         my.cpos = i;
         break;
      }
      if (contents[i]     != ' ' )  continue;
      if (contents[i]     == '\0')  break;
      my.cpos = i - 1;
      break;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
pos_move           (char a_dir)
{
   DEBUG_S  printf("pos_move       :: begin\n");
   /*---(locals)-----------------------------*/
   int       i    = 0;                       /* loop iterator                 */
   int  s_beg = my.bpos;                /* save the beginning pos                */
   int  s_end = my.epos;                /* save the ending pos                   */
   int  s_cur = my.cpos;                /* save the current pos                  */
   int  avail = my.apos;                /* available positions                   */
   int  half  = avail / 2;           /* half the viewable positions           */
   int  qtr   = avail / 4;           /* quarter the viewable positions        */
   /*---(adjust curr)-----------------*/
   my.apos     = my.x_full - 25 - 9 - 3;
   my.npos     = strlen(contents);
   switch (a_dir) {

   case '0' : my.bpos  = my.cpos = 0;          break;    /* pos moves               */
   case 'l' : my.cpos -= 5;                 break;
   case '-' : --my.cpos;                    break;
   case '+' : ++my.cpos;                    break;
   case 'm' : my.cpos += 5;                 break;
   case '$' : my.cpos  = my.npos - 1; my.bpos = my.epos - my.apos;   break;

   case 'T' : my.cpos  = my.bpos;              break;    /* screen moves            */
   case 't' : my.cpos  = my.bpos + qtr;        break;
   case '.' : my.cpos  = my.bpos + half;       break;
   case 'b' : my.cpos  = my.bpos + half + qtr; break;
   case 'B' : my.cpos  = my.bpos + avail - 1;  break;

   case '^' : my.bpos  = my.cpos;              break;    /* scrolling               */
   case 'k' : my.bpos  = my.cpos - qtr;        break;
   case ',' : my.bpos  = my.cpos - half;       break;
   case 'j' : my.bpos  = my.cpos - half - qtr; break;
   case 'v' : my.bpos  = my.cpos - avail + 1;  break;

   case 'w' : word_fore ();                    break;
   case 'W' : word_back ();                    break;
   case 'e' : word_end  ();                    break;

   case 'r' : s_beg = -1;                break;    /* size change only        */
   default  : return 0;                  break;
   }
   my.epos = my.bpos + my.apos - 1;
   /*---(check min/max)---------------*/
   if (my.cpos <   0      )    my.cpos = 0;
   if (my.cpos >=  my.npos)    my.cpos = my.npos - 1;
   /*---(check end)-------------------*/
   if (my.cpos <   my.bpos)    my.bpos = my.cpos;
   if (my.cpos >   my.epos)  { my.epos = my.cpos; my.bpos = my.epos - my.apos + 1; }
   if (my.bpos <   0      )    my.bpos = 0;
   /*> if (epos >= npos)  { epos = npos - 1; bpos = epos - apos + 1; }                <*/
   my.epos = my.bpos + my.apos - 1;
   /*---(set boundaries)--------------*/
   if (my.npos < my.apos) {
      my.bpos = 0;
      my.epos = my.bpos + my.npos - 1;
   }
   /*---(adjust screen)---------------*/
   /*> if (cpos < bpos)     cpos = bpos;                                              <* 
    *> if (cpos > epos)     epos = cpos;                                              <*/
   /*---(complete)--------------------*/
   DEBUG_S  printf("pos_move       :: end\n");
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    unit testing accessor                     ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;}

char*            /* [------] unit test accessor ------------------------------*/
move_unit          (char *a_question, int a_num)
{
   /*---(prepare)------------------------*/
   strcpy  (unit_answer, "s_move unit      : question not understood");
   /*---(defenses)-----------------------*/
   if      (a_num < 0) {
      snprintf (unit_answer, LEN_TEXT, "s_move error     : tab ref (%d) is negative", a_num);
   }
   else if (a_num >= NTAB) {
      snprintf (unit_answer, LEN_TEXT, "s_move error     : tab ref (%d vs %d) is too high", a_num, NTAB);
   }
   /*---(screen location)----------------*/
   else if (strcmp(a_question, "row_stats"     ) == 0) {
      snprintf(unit_answer, LEN_TEXT, "s_move row stat  : num=%4d, scr=%4d, beg=%4d, cur=%4d, end=%4d", NROW, my.y_avail, BROW, CROW, EROW);
   }
   else if (strcmp(a_question, "col_stats"     ) == 0) {
      snprintf(unit_answer, LEN_TEXT, "s_move col stat  : num=%4d, scr=%4d, beg=%4d, cur=%4d, end=%4d", NCOL, my.x_avail, BCOL, CCOL, ECOL);
   }
   /*---(current position)---------------*/
   else if (strcmp(a_question, "tab_cur"       ) == 0) {
      snprintf(unit_answer, LEN_TEXT, "s_move tab cur   : tab=%4d, col=%4d, row=%4d", CTAB, CCOL, CROW);
   }
   else if (strcmp(a_question, "tab_beg"       ) == 0) {
      snprintf(unit_answer, LEN_TEXT, "s_move tab beg   : tab=%4d, col=%4d, row=%4d", a_num, tabs [a_num].bcol, tabs [a_num].brow);
   }
   else if (strcmp(a_question, "tab_pos"       ) == 0) {
      snprintf(unit_answer, LEN_TEXT, "s_move tab pos   : tab=%4d, col=%4d, row=%4d", a_num, tabs [a_num].ccol, tabs [a_num].crow);
   }
   else if (strcmp(a_question, "tab_max" )       == 0) {
      snprintf(unit_answer, LEN_TEXT, "s_move tab max   : tab=%4d, col=%4d, row=%4d", a_num, tabs [a_num].ncol, tabs [a_num].nrow);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



/*============================----end-of-source---============================*/
