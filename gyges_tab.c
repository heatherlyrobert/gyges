/*============================----beg-of-source---============================*/
#include   "gyges.h"


/*
 *  future...
 *     -- tab models such as mini (one) "0®¯", small (five) "01234®¯", etc
 *
 *
 *
 */


static  char  *s_valids = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ®¯"; 
static  int    s_nvalid = 38;



/*====================------------------------------------====================*/
/*===----                         program level                        ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
TAB_init                (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(count buffer labels)------------*/
   s_nvalid = strlen (s_valids);
   /*---(add buffer commands)------------*/
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFER, "buf"         , "bu"  , "c"    , TAB_switch_char            , "switch buffer"                        );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFER, "btitle"      , "bt"  , "s"    , TAB_rename_curr            , "rename current buffer"                );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFER, "bfirst"      , "bf"  , ""     , TAB_first                  , "goto the first buffer in list"        );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFER, "bnext"       , "bn"  , ""     , TAB_next                   , "goto the next sequential buffer"      );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFER, "bprev"       , "bp"  , ""     , TAB_prev                   , "goto the previous sequential buffer"  );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFER, "blast"       , "bl"  , ""     , TAB_last                   , "goto the last buffer in list"         );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFER, "bsize"       , "bs"  , "s"    , TAB_resize                 , "change a buffer size"                 );
   /*---(complete)-----------------------*/
   return rc;
}



/*====================------------------------------------====================*/
/*===----                         support functions                    ----===*/
/*====================------------------------------------====================*/
static void  o___SUPPORT_________o () { return; }

char         /*-> tbd --------------------------------[ leaf   [ge.320.113.20]*/ /*-[00.0000.184.I]-*/ /*-[--.---.---.--]-*/
TAB_valid            (int a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (a_tab   < 0)                           return rce;
   --rce;  if (a_tab   >= MAX_TABS)                   return rce;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.320.113.20]*/ /*-[00.0000.184.I]-*/ /*-[--.---.---.--]-*/
TAB_label            (int  a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_label     =  '-';
   /*---(defense)------------------------*/
   --rce;  if (a_tab <  0       )  return rce;
   --rce;  if (a_tab >= s_nvalid)  return rce;
   /*---(convert to index)---------------*/
   x_label = s_valids [a_tab];
   /*---(complete)-----------------------*/
   return x_label;
}

char         /*-> tbd --------------------------------[ leaf   [ge.320.113.20]*/ /*-[00.0000.184.I]-*/ /*-[--.---.---.--]-*/
TAB_index            (char  a_abbr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char       *p           = NULL;
   int         x_index     =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_abbr == 0   )  return rce;
   p = strchr (s_valids, a_abbr);
   --rce;  if (p      == NULL)  return rce;
   /*---(convert to index)---------------*/
   x_index = (int) (p - s_valids);
   /*---(complete)-----------------------*/
   return x_index;
}



/*====================------------------------------------====================*/
/*===----                          tab naming                          ----===*/
/*====================------------------------------------====================*/
static void  o___NAMES___________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.320.223.21]*/ /*-[00.0000.10#.4]-*/ /*-[--.---.---.--]-*/
TAB_defname          (int a_tab, char *a_name)
{
   char        rce         =  -20;
   char        rc          =    0;
   rc = TAB_valid (a_tab);
   if (rc < 0) return rc;
   --rce;  if (a_name  == NULL)                       return rce;
   sprintf (a_name, "tab_%02d", a_tab);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.320.223.21]*/ /*-[00.0000.404.A]-*/ /*-[--.---.---.--]-*/
TAB_name             (int a_tab, char *a_name)
{
   char        rce         =  -20;
   char rc = TAB_valid (a_tab);
   if (rc < 0) return rc;
   --rce;  if (a_name  == NULL)                       return rce;
   strlcpy (a_name, s_tabs [a_tab].name, LEN_ABBR);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.330.225.41]*/ /*-[00.0000.104.C]-*/ /*-[--.---.---.--]-*/
TAB_rename           (int a_tab, char *a_name)
{
   char        rce         =  -20;
   char rc = TAB_valid (a_tab);
   if (rc < 0) return rc;
   --rce;  if (a_name  == NULL)                       return rce;
   --rce;  if (a_name [0] == '\0')                    return rce;
   --rce;  if (strlen (a_name) >= LEN_ABBR)           return rce;
   strlcpy (s_tabs [a_tab].name, a_name, LEN_ABBR);
   return 0;
}

char  TAB_rename_curr      (char *a_name) { return TAB_rename (CTAB, a_name); }



/*====================------------------------------------====================*/
/*===----                          switching                           ----===*/
/*====================------------------------------------====================*/
static void  o___SWITCHING_______o () { return; }

char         /*-> tbd --------------------------------[ ------ [gc.E82.112.31]*/ /*-[02.0000.073.!]-*/ /*-[--.---.---.--]-*/
TAB_switch             (int a_tab)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(begin)--------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_value   ("a_tab"     , a_tab);
   /*---(defense)---------------------*/
   rc = TAB_valid (a_tab);
   DEBUG_LOCS   yLOG_value   ("rc"        , rc);
   if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(non-init run)-------------------*/
   if (a_tab != CTAB) {
      /*---(save values)-----------------*/
      DEBUG_LOCS   yLOG_note    ("save existing tab values");
      /*---(cols)---------*/
      s_tabs [CTAB].ncol      = NCOL;
      s_tabs [CTAB].ccol      = CCOL;
      s_tabs [CTAB].bcol      = BCOL;
      s_tabs [CTAB].ecol      = ECOL;
      s_tabs [CTAB].froz_col  = FR_COL;
      s_tabs [CTAB].froz_bcol = FR_BCOL;
      s_tabs [CTAB].froz_ecol = FR_ECOL;
      /*---(rows)---------*/
      s_tabs [CTAB].nrow      = NROW;
      s_tabs [CTAB].crow      = CROW;
      s_tabs [CTAB].brow      = BROW;
      s_tabs [CTAB].erow      = EROW;
      s_tabs [CTAB].froz_row  = FR_ROW;
      s_tabs [CTAB].froz_brow = FR_BROW;
      s_tabs [CTAB].froz_erow = FR_EROW;
      /*---(update tab)------------------*/
      CTAB      = a_tab;
   }
   DEBUG_LOCS   yLOG_value   ("CTAB"      , CTAB);
   /*---(switch tab)---------------------*/
   p_tab     = &s_tabs[CTAB];
   /*---(restore values)-----------------*/
   DEBUG_LOCS   yLOG_note    ("restore new tab values");
   /*---(cols)---------*/
   NCOL      = s_tabs [CTAB].ncol;
   CCOL      = s_tabs [CTAB].ccol;
   BCOL      = s_tabs [CTAB].bcol;
   ECOL      = s_tabs [CTAB].ecol;
   FR_COL    = s_tabs [CTAB].froz_col;
   FR_BCOL   = s_tabs [CTAB].froz_bcol;
   FR_ECOL   = s_tabs [CTAB].froz_ecol;
   /*---(rows)---------*/
   NROW      = s_tabs [CTAB].nrow;
   CROW      = s_tabs [CTAB].crow;
   BROW      = s_tabs [CTAB].brow;
   EROW      = s_tabs [CTAB].erow;
   FR_ROW    = s_tabs [CTAB].froz_row;
   FR_BROW   = s_tabs [CTAB].froz_brow;
   FR_EROW   = s_tabs [CTAB].froz_erow;
   /*---(reposition)---------------------*/
   g_xmap.gcur = CCOL;
   g_ymap.gcur = CROW;
   g_zmap.gcur = CTAB;
   MAP_mapper (YVIKEYS_UPDATE);
   yVIKEYS_jump (CCOL, CROW, CTAB);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return CTAB;
}

char         /*-> tbd --------------------------------[ ------ [gc.220.112.31]*/ /*-[00.0000.103.!]-*/ /*-[--.---.---.--]-*/
TAB_switch_char        (char a_tab)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_tab       =    0;
   /*---(relative tabs)------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_char    ("a_tab"     , a_tab);
   DEBUG_LOCS   yLOG_value   ("a_tab"     , a_tab);
   if (a_tab > 0 && strchr ("[<>]", a_tab) != NULL) {
      switch (a_tab) {
      case '[' :  x_tab = 0;             break;
      case '<' :  x_tab = CTAB - 1;      break;
      case '>' :  x_tab = CTAB + 1;      break;
      case ']' :  x_tab = s_nvalid - 3;  break;
      }
   }
   /*---(absolute tabs)------------------*/
   else {
      x_tab = TAB_index (a_tab);
      if (x_tab < 0) {
         DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
         return x_tab;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return TAB_switch (x_tab);
}

char        TAB_first            (void)  { return TAB_switch_char ('['); }
char        TAB_prev             (void)  { return TAB_switch_char ('<'); }
char        TAB_next             (void)  { return TAB_switch_char ('>'); }
char        TAB_last             (void)  { return TAB_switch_char (']'); }



/*====================------------------------------------====================*/
/*===----                        updating sizing                       ----===*/
/*====================------------------------------------====================*/
static void  o___SIZING__________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.320.223.22]*/ /*-[00.0000.00#.D]-*/ /*-[--.---.---.--]-*/
TAB_size             (int a_tab, char *a_max)
{
   char        rce         =  -20;
   char rc = TAB_valid (a_tab);
   if (rc < 0) return rc;
   --rce;  if (a_max   == NULL)                       return rce;
   LOC_ref (a_tab, s_tabs[a_tab].ncol - 1, s_tabs[a_tab].nrow - 1, 0, a_max);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.632.153.22]*/ /*-[01.0000.104.U]-*/ /*-[--.---.---.--]-*/
TAB_resize           (char *a_max)
{
   char        rce         = -10;
   char        rc          =   0;
   int         x_tab       =   0;
   int         x_col       =   0;
   int         x_row       =   0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(handle default sizing)----------*/
   if (a_max != NULL && strlen (a_max) == 1) {
      rc = x_tab  = TAB_index (a_max [0]);
      if (rc >= 0) {
         x_col = DEF_COLS - 1;
         x_row = DEF_ROWS - 1;
      }
   }
   /*---(handle full)--------------------*/
   else {
      rc = LOC_parse  (a_max, &x_tab, &x_col, &x_row, NULL);
   }
   /*---(check error)--------------------*/
   DEBUG_LOCS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(adjust tab)---------------------*/
   rc = LOC_legal  (x_col, x_row, x_tab, CELL_EXACT);
   DEBUG_LOCS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  TAB_colwide      (int a_tab) { return s_tabs [a_tab].defwide; }
char  TAB_rowtall      (int a_tab) { return s_tabs [a_tab].deftall; }



/*====================------------------------------------====================*/
/*===----                         other characteristics                ----===*/
/*====================------------------------------------====================*/
static void  o___CHARS___________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.220.122.11]*/ /*-[00.0000.10#.!]-*/ /*-[--.---.---.--]-*/
TAB_type             (int a_tab)
{
   char        rce         =  -20;
   char        rc          =    0;
   rc = TAB_valid (a_tab);
   if (rc < 0) return rc;
   return s_tabs [a_tab].type;
}

int          /*-> indicate if tab is used ------------[ ------ [gn.210.212.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
TAB_used             (int a_tab)
{
   char rc = TAB_valid (a_tab);
   if (rc < 0) return rc;
   return s_tabs [a_tab].c;
}

char         /*-> tbd --------------------------------[ ------ [ge.320.222.11]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
TAB_retype           (int a_tab, char a_type)
{
   char        rce         =  -20;
   char        rc          =    0;
   rc = TAB_valid (a_tab);
   if (rc < 0) return rc;
   s_tabs [a_tab].type = a_type;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        status information                    ----===*/
/*====================------------------------------------====================*/
static void  o___STATUS__________o () { return; }

char         /*-> tbd --------------------------------[ leaf   [ge.632.233.70]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
TAB_line           (char a_tab, char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_tab       = '0';
   char        x_count     [LEN_LABEL] = "";
   /*---(beginning)----------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_value   ("a_tab"     , a_tab);
   DEBUG_REGS   yLOG_point   ("a_list"    , a_list);
   /*---(defenses)--------------------*/
   if (a_list  == NULL) {
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return -1;     /* then no point                       */
   }
   /*---(walk the list)---------------*/
   if      (a_tab >= 0  && a_tab <= 9 )   x_tab = a_tab + '0';
   else if (a_tab >= 10 && a_tab <= 36)   x_tab = a_tab - 10 + 'A';
   else                                   return -2;
   if      (s_tabs [a_tab].c ==   0)  sprintf (x_count, "  -");
   else if (s_tabs [a_tab].c <  999)  sprintf (x_count, "%3d", s_tabs [a_tab].c);
   else                               sprintf (x_count, "+++");
   sprintf (a_list, " %c %-15.15s %3d %4d %3s %c ", x_tab, s_tabs [a_tab].name, s_tabs [a_tab].ncol, s_tabs [a_tab].nrow, x_count, s_tabs [a_tab].type);
   /*---(complete)--------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.632.233.40]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
TAB_status         (char a_tab, char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_tab       = '0';
   char        x_size      [LEN_LABEL] = "";
   /*---(beginning)----------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_value   ("a_tab"     , a_tab);
   DEBUG_REGS   yLOG_point   ("a_list"    , a_list);
   /*---(defenses)--------------------*/
   if (a_list  == NULL) {
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return -1;     /* then no point                       */
   }
   /*---(walk the list)---------------*/
   if      (a_tab >= 0  && a_tab <= 9 )   x_tab = a_tab + '0';
   else if (a_tab >= 10 && a_tab <= 36)   x_tab = a_tab - 10 + 'A';
   else                                   return -2;
   sprintf (x_size, "%dx%d", NCOL, NROW);
   snprintf (a_list, LEN_STR, "[ buffer %d %c %s %-30.30s ]", a_tab, x_tab, x_size, s_tabs [a_tab].name);
   /*---(complete)--------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}


