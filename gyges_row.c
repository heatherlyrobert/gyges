/*============================----beg-of-source---============================*/
#include   "gyges.h"



/*====================------------------------------------====================*/
/*===----                           row related                        ----===*/
/*====================------------------------------------====================*/
PRIV void  o___ROWS____________o () { return; }

char         /*-> clear all row customizations -------[ leaf   [ge.843.123.20]*/ /*-[01.0000.014.!]-*/ /*-[--.---.---.--]-*/
ROW_clear            (int a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_row       =   0;
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
   /*---(initialize rows)----------------*/
   DEBUG_LOCS   yLOG_snote   ("clear rows to defaults");
   DEBUG_LOCS   yLOG_svalue  ("MAX_ROWS"  , MAX_ROWS);
   for (x_row = 0; x_row < MAX_ROWS; ++x_row) {
      s_tabs [a_tab].rows [x_row].h = s_tabs [a_tab].deftall;
      s_tabs [a_tab].rows [x_row].y = 0;
      s_tabs [a_tab].rows [x_row].c = 0;
   }
   /*---(clear frozen rows)--------------*/
   DEBUG_LOCS   yLOG_snote   ("clear any frozen rows");
   s_tabs [a_tab].froz_row  = '-';
   s_tabs [a_tab].froz_brow = 0;
   s_tabs [a_tab].froz_erow = 0;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.330.215.40]*/ /*-[00.0000.084.H]-*/ /*-[--.---.---.--]-*/
ROW_valid            (int a_tab, int a_row)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (a_tab   < 0)                           return rce;
   --rce;  if (a_tab   >= MAX_TABS)                   return rce;
   --rce;  if (a_row   < 0)                           return rce;
   --rce;  if (a_row   >= MAX_ROWS)                   return rce;
   /*---(complete)-----------------------*/
   return 0;
}

int          /*-> return default row size ------------[ leaf   [gn.210.002.00]*/ /*-[00.0000.10#.6]-*/ /*-[--.---.---.--]-*/
ROW_defmax           (void)
{
   return DEF_ROWS;
}

int          /*-> return max row for tab -------------[ ------ [gn.210.113.11]*/ /*-[00.0000.604.D]-*/ /*-[--.---.---.--]-*/
ROW_max              (int a_tab)
{
   char rc = ROW_valid (a_tab, 0);
   if (rc < 0) return rc;
   return s_tabs [a_tab].nrow;
}

int          /*-> return the row used ----------------[ ------ [gn.210.213.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
ROW_used             (int a_tab, int a_row)
{
   char rc = ROW_valid (a_tab, 0);
   if (rc < 0) return rc;
   return s_tabs [a_tab].rows [a_row].c;
}

int          /*-> return the row ypos ----------------[ ------ [gn.210.213.11]*/ /*-[00.0000.603.!]-*/ /*-[--.---.---.--]-*/
ROW_ypos             (int a_tab, int a_row)
{
   char rc = ROW_valid (a_tab, 0);
   if (rc < 0) return rc;
   return s_tabs [a_tab].rows [a_row].y;
}

char         /*-> set a new row y-pos ----------------[ ------ [gc.210.312.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
ROW_yset             (int a_tab, int a_row, int a_pos)
{
   char rc = ROW_valid (a_tab, a_row);
   if (rc < 0) return rc;
   s_tabs [a_tab].rows [a_row].y = a_pos;
   return 0;
}

char         /*-> set the default width --------------[ ------ [gc.210.213.11]*/ /*-[00.0000.G03.!]-*/ /*-[--.---.---.--]-*/
ROW_defheight        (int a_tab, int a_size)
{
   char        rc          =    0;
   int         x_row       =    0;
   rc = TAB_valid (a_tab);
   if (rc < 0) return rc;
   if (a_size  < MIN_HEIGHT)   a_size = MIN_HEIGHT;
   if (a_size  > MAX_HEIGHT)   a_size = MAX_HEIGHT;
   for (x_row = 0; x_row < MAX_ROWS; ++x_row) {
      if (s_tabs [a_tab].rows [x_row].h != s_tabs [a_tab].deftall)  continue;
      if (s_tabs [a_tab].rows [x_row].c >  0)                       continue;
      s_tabs [a_tab].rows [x_row].h = a_size;
   }
   s_tabs [a_tab].deftall = a_size;
   return 0;
}

char         /*-> return height for a row ------------[ ------ [gc.210.213.11]*/ /*-[00.0000.604.!]-*/ /*-[--.---.---.--]-*/
ROW_height           (int a_tab, int a_row)
{
   char rc = ROW_valid (a_tab, a_row);
   if (rc < 0) return rc;
   return s_tabs [a_tab].rows [a_row].h;
}

char         /*-> change the row height --------------[ ------ [gc.320.312.31]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
ROW_heighten         (int a_tab, int a_row, int a_size)
{
   char rc = ROW_valid (a_tab, a_row);
   if (rc < 0) return rc;
   if (a_size  < MIN_HEIGHT)   a_size = MIN_HEIGHT;
   if (a_size  > MAX_HEIGHT)   a_size = MAX_HEIGHT;
   s_tabs [a_tab].rows [a_row].h = a_size;
   return 0;
}

char         /*-> change the frozen rows -------------[ ------ [gc.430.323.31]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
ROW_freeze           (int a_tab, int a_brow, int a_erow)
{
   char rc = ROW_valid (a_tab, a_brow);
   if (rc < 0) return rc;
   rc = ROW_valid (a_tab, a_erow);
   if (rc < 0) return rc;
   int    x_row;
   if (a_brow  >  a_erow) {
      x_row   = a_brow;
      a_brow  = a_erow;
      a_erow  = x_row;
   }
   s_tabs [a_tab].froz_row    = 'y';
   s_tabs [a_tab].froz_brow   = a_brow;
   s_tabs [a_tab].froz_erow   = a_erow;
   return 0;
}

char         /*-> clear the frozen rows --------------[ ------ [gc.320.112.11]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
ROW_unfreeze         (int a_tab)
{
   char rc = ROW_valid (a_tab, 0);
   if (rc < 0) return rc;
   s_tabs [a_tab].froz_row    = '-';
   s_tabs [a_tab].froz_brow   = 0;
   s_tabs [a_tab].froz_erow   = 0;
   return 0;
}

