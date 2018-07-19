/*============================----beg-of-source---============================*/
#include   "gyges.h"



/*====================------------------------------------====================*/
/*===----                           col related                        ----===*/
/*====================------------------------------------====================*/
PRIV void  o___COLUMNS_________o () { return; }

char         /*-> clear all column customizations ----[ leaf   [ge.A53.123.40]*/ /*-[02.0000.014.!]-*/ /*-[--.---.---.--]-*/
COL_clear            (int a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_col       =   0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_senter  (__FUNCTION__);
   DEBUG_LOCS   yLOG_svalue  ("a_tab"     , a_tab);
   /*---(defense)------------------------*/
   --rce;  if (a_tab <  0) {
      DEBUG_LOCS   yLOG_snote   ("tab too small");
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_tab >= MAX_TABS) {
      DEBUG_LOCS   yLOG_snote   ("tab too big");
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(initialize columns)-------------*/
   DEBUG_LOCS   yLOG_snote   ("clear columns to defaults");
   DEBUG_LOCS   yLOG_svalue  ("MAX_COLS"  , MAX_COLS);
   for (x_col = 0; x_col < MAX_COLS; ++x_col) {
      /*---(characteristics)-------------*/
      s_tabs [a_tab].cols [x_col].w       = s_tabs [a_tab].defwide;
      s_tabs [a_tab].cols [x_col].x       = 0;
      s_tabs [a_tab].cols [x_col].c       = 0;
      /*---(labels)----------------------*/
      if        (x_col < 26)  {
         s_tabs [a_tab].cols [x_col].l[0] = '-';
         s_tabs [a_tab].cols [x_col].l[1] = x_col + 'a';
      } else  {
         s_tabs [a_tab].cols [x_col].l[0] = (x_col / 26) - 1 + 'a';
         s_tabs [a_tab].cols [x_col].l[1] = (x_col % 26) + 'a';
      }
      s_tabs [a_tab].cols [x_col].l[2] = '\0';
      /*---(done)------------------------*/
   }
   /*---(clear frozen cols)--------------*/
   DEBUG_LOCS   yLOG_snote   ("clear any frozen columns");
   s_tabs [a_tab].froz_col  = '-';
   s_tabs [a_tab].froz_bcol = 0;
   s_tabs [a_tab].froz_ecol = 0;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.330.215.40]*/ /*-[00.0000.194.I]-*/ /*-[--.---.---.--]-*/
COL_valid            (int a_tab, int a_col)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (a_tab   < 0)                           return rce;
   --rce;  if (a_tab   >= MAX_TABS)                   return rce;
   --rce;  if (a_col   < 0)                           return rce;
   --rce;  if (a_col   >= MAX_COLS)                   return rce;
   /*---(complete)-----------------------*/
   return 0;
}

int          /*-> return default col size ------------[ leaf   [gn.210.002.00]*/ /*-[00.0000.10#.6]-*/ /*-[--.---.---.--]-*/
COL_defmax           (void)
{
   return DEF_COLS;
}

int          /*-> return max col for tab -------------[ ------ [gn.210.113.11]*/ /*-[00.0000.704.D]-*/ /*-[--.---.---.--]-*/
COL_max              (int a_tab)
{
   char rc = COL_valid (a_tab, 0);
   if (rc < 0) return rc;
   return s_tabs [a_tab].ncol;
}

int          /*-> indicate if column is used ---------[ ------ [gn.210.212.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
COL_used             (int a_tab, int a_col)
{
   char rc = COL_valid (a_tab, a_col);
   if (rc < 0) return rc;
   return s_tabs [a_tab].cols [a_col].c;
}

int          /*-> return the col xpos ----------------[ ------ [gn.210.213.11]*/ /*-[00.0000.503.!]-*/ /*-[--.---.---.--]-*/
COL_xpos             (int a_tab, int a_col)
{
   char rc = COL_valid (a_tab, a_col);
   if (rc < 0) return rc;
   return s_tabs [a_tab].cols [a_col].x;
}

char         /*-> set a new col x-pos ----------------[ ------ [gc.210.312.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
COL_xset             (int a_tab, int a_col, int a_pos)
{
   char rc = COL_valid (a_tab, a_col);
   if (rc < 0) return rc;
   s_tabs [a_tab].cols [a_col].x = a_pos;
   return 0;
}

char         /*-> return the col xpos ----------------[ ------ [gc.210.313.21]*/ /*-[00.0000.204.!]-*/ /*-[--.---.---.--]-*/
COL_label            (int a_tab, int a_col, char *a_label)
{
   char rc = COL_valid (a_tab, a_col);
   if (rc < 0) return rc;
   if (a_label != NULL)  strlcpy (a_label, s_tabs [a_tab].cols [a_col].l, LEN_LABEL);
   return 0;
}

char         /*-> set the default width --------------[ ------ [gc.210.213.11]*/ /*-[00.0000.G03.!]-*/ /*-[--.---.---.--]-*/
COL_defwidth         (int a_tab, int a_size)
{
   char        rc          =    0;
   int         x_col       =    0;
   rc = TAB_valid (a_tab);
   if (rc < 0) return rc;
   if (a_size  < MIN_WIDTH)    a_size = MIN_WIDTH;
   if (a_size  > MAX_WIDTH)    a_size = MAX_WIDTH;
   for (x_col = 0; x_col < MAX_ROWS; ++x_col) {
      if (s_tabs [a_tab].cols [x_col].w != s_tabs [a_tab].defwide)  continue;
      if (s_tabs [a_tab].cols [x_col].c >  0)                       continue;
      s_tabs [a_tab].cols [x_col].w = a_size;
   }
   s_tabs [a_tab].defwide = a_size;
   return 0;
}

char         /*-> return the col width ---------------[ ------ [gc.210.213.11]*/ /*-[00.0000.G03.!]-*/ /*-[--.---.---.--]-*/
COL_width            (int a_tab, int a_col)
{
   char rc = COL_valid (a_tab, a_col);
   if (rc < 0) return rc;
   return s_tabs [a_tab].cols [a_col].w;
}

char         /*-> change the col width ---------------[ ------ [gc.320.312.31]*/ /*-[00.0000.404.5]-*/ /*-[--.---.---.--]-*/
COL_widen            (int a_tab, int a_col, int a_size)
{
   char rc = COL_valid (a_tab, a_col);
   if (rc < 0) return rc;
   if (a_size  < MIN_WIDTH)    a_size = MIN_WIDTH;
   if (a_size  > MAX_WIDTH)    a_size = MAX_WIDTH;
   s_tabs [a_tab].cols [a_col].w = a_size;
   return 0;
}

char         /*-> change the frozen cols -------------[ ------ [gc.430.323.31]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
COL_freeze           (int a_tab, int a_bcol, int a_ecol)
{
   char rc = COL_valid (a_tab, a_bcol);
   if (rc < 0) return rc;
   rc = COL_valid (a_tab, a_ecol);
   if (rc < 0) return rc;
   int  x_col;
   if (a_bcol  >  a_ecol) {
      x_col   = a_bcol;
      a_bcol  = a_ecol;
      a_ecol  = x_col;
   }
   s_tabs [a_tab].froz_col    = 'y';
   s_tabs [a_tab].froz_bcol   = a_bcol;
   s_tabs [a_tab].froz_ecol   = a_ecol;
   return 0;
}

char         /*-> clear the frozen cols --------------[ ------ [gc.320.112.11]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
COL_unfreeze         (int a_tab)
{
   char rc = COL_valid (a_tab, 0);
   if (rc < 0) return rc;
   s_tabs [a_tab].froz_col    = '-';
   s_tabs [a_tab].froz_bcol   = 0;
   s_tabs [a_tab].froz_ecol   = 0;
   return 0;
}
