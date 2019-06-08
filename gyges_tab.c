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
   TAB_purge ();
   NTAB = DEF_TABS;
   /*---(add buffer commands)------------*/
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "buf"         , "bu"  , "c"    , TAB_switch_char            , "switch buffer"                        );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "btitle"      , "bt"  , "s"    , TAB_rename_curr            , "rename current buffer"                );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "bfirst"      , "bf"  , ""     , TAB_first                  , "goto the first buffer in list"        );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "bnext"       , "bn"  , ""     , TAB_next                   , "goto the next sequential buffer"      );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "bprev"       , "bp"  , ""     , TAB_prev                   , "goto the previous sequential buffer"  );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "blast"       , "bl"  , ""     , TAB_last                   , "goto the last buffer in list"         );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "bsize"       , "bs"  , "s"    , TAB_resize                 , "change a buffer size"                 );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "bmax"        , "bx"  , "i"    , TAB_setmax                 , "change count of available buffers"    );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "bbrowse"     , "bb"  , "a"    , TAB_browse                 , "find buffer by name"                  );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "defwide"     , ""    , "i"    , TAB_defwide                , "change default column width"          );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "deftall"     , ""    , "i"    , TAB_deftall                , "change default row height"            );
   /*---(add status options)-------------*/
   rc = yVIKEYS_view_option (YVIKEYS_STATUS, "buffer" , TAB_status_curr     , "details of current buffer"                  );
   /*---(add yparse specification)-------*/
   rc = yPARSE_handler (FILE_TABS    , "tab"       , 4.1, "NLLsssc-----", TAB_reader      , TAB_writer_all  , "------------" , "name,min,max,wid,tal,dep,t"        , "gyges tabs (v-axis)"      );
   /*---(complete)-----------------------*/
   return rc;
}

char         /*-> clean all tabs ---------------------[ ------ [fz.A52.021.03]*/ /*-[01.0000.023.!]-*/ /*-[--.---.---.--]-*/
TAB_purge            (void)
{  /*---(design notes)--------------------------------------------------------*/
   /* run CELL_wrap/purge before LOC_wrap/purge so all cells are unhooked     */
   /*---(locals)-----------+-----------+-*/
   int         x_tab       = 0;
   char        x_label     [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(initialize s_tabs)----------------*/
   for (x_tab = 0; x_tab < MAX_TABS; ++x_tab) {
      DEBUG_PROG   yLOG_value   ("x_tab"     , x_tab);
      /*---(main config)-----------------*/
      DEBUG_PROG   yLOG_note    ("reset naming");
      s_tabs [x_tab].tab     = x_tab;
      s_tabs [x_tab].type    = G_TAB_NORMAL;
      TAB_defname (x_tab, s_tabs [x_tab].name);
      /*---(size limits)-----------------*/
      DEBUG_PROG   yLOG_note    ("reset default size");
      DEBUG_PROG   yLOG_value   ("DEF_COLS"  , DEF_COLS);
      DEBUG_PROG   yLOG_value   ("DEF_ROWS"  , DEF_ROWS);
      str4gyges (x_tab, DEF_COLS - 1, DEF_ROWS - 1, 0, 0, x_label, YSTR_CHECK);
      TAB_resize (x_label);
      /*---(current position)------------*/
      DEBUG_PROG   yLOG_note    ("reset current position");
      s_tabs [x_tab].ccol    =    0;
      s_tabs [x_tab].crow    =    0;
      /*---(screen position)-------------*/
      DEBUG_PROG   yLOG_note    ("reset beginning and ending cells");
      s_tabs [x_tab].bcol    =    0;
      s_tabs [x_tab].brow    =    0;
      s_tabs [x_tab].ecol    =    0;
      s_tabs [x_tab].erow    =    0;
      /*---(col/row)---------------------*/
      s_tabs [x_tab].defwide = DEF_WIDTH;
      s_tabs [x_tab].deftall = DEF_HEIGHT;
      COL_clear         (x_tab);
      ROW_clear         (x_tab);
      /*---(frozen)----------------------*/
      s_tabs [x_tab].froz_col      = '-';
      s_tabs [x_tab].froz_bcol     =   0;
      s_tabs [x_tab].froz_ecol     =   0;
      s_tabs [x_tab].froz_row      = '-';
      s_tabs [x_tab].froz_brow     =   0;
      s_tabs [x_tab].froz_erow     =   0;
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
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
   --rce;  if (!VALID_tab (a_tab))                    return rce;
   --rce;  if (a_name  == NULL)                       return rce;
   sprintf (a_name, "tab_%02d", a_tab);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.320.223.21]*/ /*-[00.0000.404.A]-*/ /*-[--.---.---.--]-*/
TAB_name             (int a_tab, char *a_name)
{
   char        rce         =  -20;
   --rce;  if (!LEGAL_TAB (a_tab))                    return rce;
   --rce;  if (a_name  == NULL)                       return rce;
   strlcpy (a_name, s_tabs [a_tab].name, LEN_TERSE);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.330.225.41]*/ /*-[00.0000.104.C]-*/ /*-[--.---.---.--]-*/
TAB_rename           (int a_tab, char *a_name)
{
   char        rce         =  -20;
   --rce;  if (!LEGAL_TAB (a_tab))                    return rce;
   --rce;  if (a_name  == NULL)                       return rce;
   --rce;  if (a_name [0] == '\0')                    return rce;
   --rce;  if (strlen (a_name) >= LEN_TERSE)           return rce;
   strlcpy (s_tabs [a_tab].name, a_name, LEN_TERSE);
   return 0;
}

char  TAB_rename_curr      (char *a_name) { return TAB_rename (CTAB, a_name); }



/*====================------------------------------------====================*/
/*===----                          switching                           ----===*/
/*====================------------------------------------====================*/
static void  o___SWITCHING_______o () { return; }

char
TAB_retrieve            (void)
{
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(restore values)-----------------*/
   DEBUG_LOCS   yLOG_note    ("restore new tab values");
   DEBUG_LOCS   yLOG_value   ("CTAB"      , CTAB);
   /*---(switch tab)---------------------*/
   p_tab     = &s_tabs[CTAB];
   /*---(cols)---------*/
   /*> NCOL      = s_tabs [CTAB].ncol;                                                <* 
    *> CCOL      = s_tabs [CTAB].ccol;                                                <* 
    *> BCOL      = s_tabs [CTAB].bcol;                                                <* 
    *> ECOL      = s_tabs [CTAB].ecol;                                                <* 
    *> FR_COL    = s_tabs [CTAB].froz_col;                                            <* 
    *> FR_BCOL   = s_tabs [CTAB].froz_bcol;                                           <* 
    *> FR_ECOL   = s_tabs [CTAB].froz_ecol;                                           <*/
   /*---(rows)---------*/
   /*> NROW      = s_tabs [CTAB].nrow;                                                <* 
    *> CROW      = s_tabs [CTAB].crow;                                                <* 
    *> BROW      = s_tabs [CTAB].brow;                                                <* 
    *> EROW      = s_tabs [CTAB].erow;                                                <* 
    *> FR_ROW    = s_tabs [CTAB].froz_row;                                            <* 
    *> FR_BROW   = s_tabs [CTAB].froz_brow;                                           <* 
    *> FR_EROW   = s_tabs [CTAB].froz_erow;                                           <*/
   /*---(reposition)---------------------*/
   /*> g_bmap.gcur = CTAB;                                                            <* 
    *> g_xmap.gcur = CCOL;                                                            <* 
    *> g_ymap.gcur = CROW;                                                            <*/
   g_bmap.gcur = CTAB;
   g_xmap.gcur = 0;
   g_ymap.gcur = 0;
   g_zmap.gcur = 0;
   MAP_mapper (YVIKEYS_UPDATE);
   /*> yVIKEYS_jump (CTAB, CCOL, CROW, 0);                                            <*/
   yVIKEYS_jump (CTAB, 0, 0, 0);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return CTAB;
}

char
TAB_save               (void)
{
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(save values)--------------------*/
   DEBUG_LOCS   yLOG_note    ("save existing tab values");
   DEBUG_LOCS   yLOG_value   ("CTAB"      , CTAB);
   /*---(cols)---------*/
   /*> s_tabs [CTAB].ncol      = NCOL;                                                <* 
    *> s_tabs [CTAB].ccol      = CCOL;                                                <* 
    *> s_tabs [CTAB].bcol      = BCOL;                                                <* 
    *> s_tabs [CTAB].ecol      = ECOL;                                                <* 
    *> s_tabs [CTAB].froz_col  = FR_COL;                                              <* 
    *> s_tabs [CTAB].froz_bcol = FR_BCOL;                                             <* 
    *> s_tabs [CTAB].froz_ecol = FR_ECOL;                                             <*/
   /*---(rows)---------*/
   /*> s_tabs [CTAB].nrow      = NROW;                                                <* 
    *> s_tabs [CTAB].crow      = CROW;                                                <* 
    *> s_tabs [CTAB].brow      = BROW;                                                <* 
    *> s_tabs [CTAB].erow      = EROW;                                                <* 
    *> s_tabs [CTAB].froz_row  = FR_ROW;                                              <* 
    *> s_tabs [CTAB].froz_brow = FR_BROW;                                             <* 
    *> s_tabs [CTAB].froz_erow = FR_EROW;                                             <*/
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return CTAB;
}

char         /*-> tbd --------------------------------[ ------ [gc.E82.112.31]*/ /*-[02.0000.073.!]-*/ /*-[--.---.---.--]-*/
TAB_switch             (int a_tab)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(begin)--------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_value   ("a_tab"     , a_tab);
   /*---(defense)---------------------*/
   rc = LEGAL_TAB (a_tab);
   DEBUG_LOCS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <= 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save values)-----------------*/
   TAB_save     ();
   /*---(update tab)------------------*/
   CTAB      = a_tab;
   /*---(retrieve tab)-------------------*/
   TAB_retrieve ();
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return CTAB;
}

char         /*-> tbd --------------------------------[ ------ [gc.220.112.31]*/ /*-[00.0000.103.!]-*/ /*-[--.---.---.--]-*/
TAB_switch_char        (char a_tab)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   static int  x_last      =    0;
   static int  x_tab       =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_char    ("a_tab"     , a_tab);
   DEBUG_LOCS   yLOG_value   ("a_tab"     , a_tab);
   DEBUG_LOCS   yLOG_value   ("x_tab"     , x_tab);
   DEBUG_LOCS   yLOG_value   ("x_last"    , x_last);
   /*---(previous tabs)------------------*/
   if (a_tab == ',') {
      rc     = x_tab;
      x_tab  = x_last;
      x_last = rc;
   }
   /*---(relative tabs)------------------*/
   else if (a_tab > 0 && strchr ("[<>]", a_tab) != NULL) {
      x_last = x_tab;
      switch (a_tab) {
      case '[' :  x_tab = 0;             break;
      case '<' :  x_tab = CTAB - 1;      break;
      case '>' :  x_tab = CTAB + 1;      break;
      case ']' :  x_tab = NTAB - 1;      break;
      }
   }
   /*---(absolute tabs)------------------*/
   else {
      x_last = x_tab;
      x_tab = INDEX_tab (a_tab);
      if (x_tab < 0) {
         DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
         return x_tab;
      }
   }
   /*---(done)---------------------------*/
   DEBUG_LOCS   yLOG_value   ("x_tab"     , x_tab);
   DEBUG_LOCS   yLOG_value   ("x_last"    , x_last);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return TAB_switch (x_tab);
}

char        TAB_first            (void)  { return TAB_switch_char ('['); }
char        TAB_prev             (void)  { return TAB_switch_char ('<'); }
char        TAB_next             (void)  { return TAB_switch_char ('>'); }
char        TAB_last             (void)  { return TAB_switch_char (']'); }

char
TAB_browse              (char *a_entry)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         x_first     =   -1;
   int         x_matches   =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(defense)--------------------s---*/
   DEBUG_LOCS   yLOG_point   ("a_entry"   , a_entry);
   --rce;  if (a_entry == NULL || a_entry [0] == 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_delim   ("a_entry"   , a_entry);
   /*---(compile search)-----------------*/
   rc = yREGEX_comp (a_entry);
   DEBUG_SRCH   yLOG_value   ("comp"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_SRCH   yLOG_note    ("could not compile search");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check all tabs)-----------------*/
   for (i = 0; i < NTAB; ++i) {
      DEBUG_INPT   yLOG_info    ("tab"       , s_tabs [i].name);
      rc = yREGEX_exec (s_tabs [i].name);
      DEBUG_INPT   yLOG_value   ("exec"      , rc);
      if (rc <= 0)   continue;
      ++x_matches;
      if (x_matches == 1)  x_first = i;
   }
   DEBUG_LOCS   yLOG_value   ("x_matches" , x_matches);
   --rce;  if (x_matches != 1) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(make switch)--------------------*/
   TAB_switch (x_first);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        updating sizing                       ----===*/
/*====================------------------------------------====================*/
static void  o___COUNTS__________o () { return; }

int          /*-> return default col size ------------[ leaf   [gn.210.002.00]*/ /*-[00.0000.10#.6]-*/ /*-[--.---.---.--]-*/
TAB_defmax           (void)
{
   return DEF_TABS;
}

int          /*-> return max col for tab -------------[ ------ [gn.210.113.11]*/ /*-[00.0000.704.D]-*/ /*-[--.---.---.--]-*/
TAB_max              (void)
{
   return NTAB;
}

int          /*-> indicate if tab is used ------------[ ------ [gn.210.212.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
TAB_used             (int a_tab)
{
   if (!LEGAL_TAB (a_tab))   return -1;
   return s_tabs [a_tab].c;
}

int          /*-> find largest used tab --------------[ ------ [gn.210.212.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
TAB_maxused          (void)
{
   int         i           =    0;
   int         x_max       =   -1;
   for (i = 0; i < NTAB; ++i) {
      if (s_tabs [i].c > 0)  x_max   = i;
   }
   return x_max;
}

int          /*-> update the tab count ---------------[ ------ [gn.210.213.11]*/ /*-[00.0000.503.!]-*/ /*-[--.---.---.--]-*/
TAB_setmax           (int a_count)
{
   int         x_max       =    0;
   x_max = TAB_maxused ();
   if (x_max >= a_count)  a_count = x_max + 1;
   if (a_count < 1)  a_count = 1;
   if (!VALID_tab (a_count - 1))  return -2;
   if (a_count >  MAX_TABS)       return -3;
   NTAB = a_count;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        updating sizing                       ----===*/
/*====================------------------------------------====================*/
static void  o___SIZING__________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.320.223.22]*/ /*-[00.0000.00#.D]-*/ /*-[--.---.---.--]-*/
TAB_size             (int a_tab, char *a_max)
{
   char        rce         =  -20;
   char rc = VALID_tab (a_tab);
   if (rc == 0) return rc;
   --rce;  if (a_max   == NULL)                       return rce;
   str4gyges (a_tab, s_tabs[a_tab].ncol - 1, s_tabs[a_tab].nrow - 1, 0, 0, a_max, YSTR_CHECK);
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
   char        x_label     [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(handle default sizing)----------*/
   if (a_max != NULL && strlen (a_max) == 1) {
      rc = x_tab  = INDEX_tab (a_max [0]);
      if (rc >= 0) {
         x_col = DEF_COLS - 1;
         x_row = DEF_ROWS - 1;
      }
   }
   /*---(handle full)--------------------*/
   else {
      rc = str2gyges  (a_max, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_CHECK);
   }
   /*---(check error)--------------------*/
   DEBUG_LOCS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(adjust tab)---------------------*/
   rc = str4gyges (x_tab, x_col, x_row, 0, 0, x_label, YSTR_EXACT);
   DEBUG_LOCS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*> if (x_tab == CTAB) {                                                           <* 
    *>    NCOL = x_col + 1;                                                           <* 
    *>    NROW = x_row + 1;                                                           <* 
    *> }                                                                              <*/
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  TAB_colwide      (int a_tab) { return s_tabs [a_tab].defwide; }
char  TAB_rowtall      (int a_tab) { return s_tabs [a_tab].deftall; }

char  TAB_defwide      (int a_size) { return COL_defwidth  (CTAB, a_size); }
char  TAB_deftall      (int a_size) { return ROW_defheight (CTAB, a_size); }


/*====================------------------------------------====================*/
/*===----                         other characteristics                ----===*/
/*====================------------------------------------====================*/
static void  o___CHARS___________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.220.122.11]*/ /*-[00.0000.10#.!]-*/ /*-[--.---.---.--]-*/
TAB_type             (int a_tab)
{
   char        rce         =  -20;
   char        rc          =    0;
   rc = VALID_tab (a_tab);
   if (rc == 0) return rc;
   return s_tabs [a_tab].type;
}

char         /*-> tbd --------------------------------[ ------ [ge.320.222.11]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
TAB_retype           (int a_tab, char a_type)
{
   char        rce         =  -20;
   char        rc          =    0;
   rc = VALID_tab (a_tab);
   if (rc == 0) return rc;
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
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_tab       =  '0';
   char        x_size      [LEN_LABEL] = "";
   char        x_cur       [LEN_LABEL] = "";
   char        x_max       [LEN_LABEL] = "";
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
   /*---(convert tab)-----------------*/
   x_tab = LABEL_tab (a_tab);
   --rce;  if (x_tab < 0 && x_tab > -20) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(generate stats)--------------*/
   sprintf (x_size, "%dx%d", s_tabs [a_tab].ncol, s_tabs [a_tab].nrow);
   rc = str4gyges  (a_tab, s_tabs [a_tab].ccol, s_tabs [a_tab].crow, 0, 0, x_cur, YSTR_CHECK);
   rc = str4gyges  (a_tab, s_tabs [a_tab].ncol - 1, s_tabs [a_tab].nrow - 1, 0, 0, x_max, YSTR_CHECK);
   if      (s_tabs [a_tab].c ==   0)  sprintf (x_count, "  -");
   else if (s_tabs [a_tab].c <  999)  sprintf (x_count, "%3d", s_tabs [a_tab].c);
   else                               sprintf (x_count, "+++");
   /*---(finish)----------------------*/
   snprintf (a_list, LEN_FULL, "%c %-12.12s %-10.10s %3s %c %-8.8s %-8.8s %2d %2d",
         x_tab, s_tabs [a_tab].name, x_size, x_count,
         s_tabs [a_tab].type, x_cur, x_max,
         TAB_colwide (a_tab), TAB_rowtall (a_tab));
   /*---(complete)--------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.632.233.40]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
TAB_status         (char a_tab, char *a_list)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        t           [LEN_FULL]   = "";
   /*---(beginning)---------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(get info)--------------------*/
   rc = TAB_line (a_tab, t);
   if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(generate stats)--------------*/
   sprintf (a_list, "buffer  %s", t);
   /*---(complete)--------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  TAB_status_curr    (char *a_list) { return TAB_status (CTAB, a_list); }



/*====================------------------------------------====================*/
/*===----                    yparse record handling                    ----===*/
/*====================------------------------------------====================*/
static void  o___YPARSE__________o () { return; }

char
TAB_reader           (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   char        x_verb      [LEN_LABEL];
   char        x_name      [LEN_LABEL];
   char        x_label     [LEN_LABEL];
   int         x_len       =    0;
   int         x_tab       =    0;
   int         x_size      =    0;
   char        x_type      =  '-';
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(get verb)-----------------------*/
   rc = yPARSE_popstr (x_verb);
   DEBUG_INPT   yLOG_value   ("pop verb"  , rc);
   DEBUG_INPT   yLOG_info    ("x_verb"    , x_verb);
   --rce;  if (strcmp ("tab", x_verb) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get name)-----------------------*/
   rc = yPARSE_popstr (x_name);
   DEBUG_INPT   yLOG_value   ("pop name"  , rc);
   DEBUG_INPT   yLOG_info    ("x_name"    , x_name);
   x_len = strlen (x_name);
   DEBUG_INPT   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <= 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(min)----------------------------*/
   rc = yPARSE_popstr (x_label);
   DEBUG_INPT   yLOG_value   ("pop min"   , rc);
   DEBUG_INPT   yLOG_info    ("min"       , x_label);
   rc = str2gyges (x_label, NULL, NULL, NULL, NULL, NULL, 0, YSTR_ADAPT);
   DEBUG_INPT   yLOG_value   ("str2gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(max)----------------------------*/
   rc = yPARSE_popstr (x_label);
   DEBUG_INPT   yLOG_value   ("pop max"   , rc);
   DEBUG_INPT   yLOG_info    ("max"       , x_label);
   rc = str2gyges (x_label, &x_tab, NULL, NULL, NULL, NULL, 0, YSTR_ADAPT);
   DEBUG_INPT   yLOG_value   ("str2gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_value   ("tab"       , x_tab);
   /*---(switch to tab)------------------*/
   rc = TAB_switch (x_tab);
   DEBUG_INPT   yLOG_value   ("TAB_switch", rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update tab)---------------------*/
   rc = TAB_rename (x_tab, x_name);
   DEBUG_INPT   yLOG_value   ("TAB_rename", rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = TAB_resize (x_label);
   DEBUG_INPT   yLOG_value   ("TAB_resize", rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(default sizes)------------------*/
   rc = yPARSE_popint  (&x_size);
   DEBUG_INPT   yLOG_value   ("pop col"   , rc);
   if (x_size > 0)  COL_defwidth  (x_tab, x_size);
   rc = yPARSE_popint  (&x_size);
   DEBUG_INPT   yLOG_value   ("pop row"   , rc);
   if (x_size > 0)  ROW_defheight (x_tab, x_size);
   rc = yPARSE_popint  (&x_size);
   DEBUG_INPT   yLOG_value   ("pop depth" , rc);
   /*---(type)---------------------------*/
   rc = yPARSE_popchar (&x_type);
   DEBUG_INPT   yLOG_value   ("pop type"  , rc);
   DEBUG_INPT   yLOG_char    ("x_type"    , x_type);
   rc = TAB_retype (x_tab, x_type);
   DEBUG_INPT   yLOG_value   ("TAB_retype", rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 1;
}

char
TAB_writer            (char a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   char        x_type      =  '-';
   int         x_cols      =    0;
   int         x_rows      =    0;
   int         x_wide      =    0;
   int         x_tall      =    0;
   char        x_name      [LEN_LABEL];
   char        x_min       [LEN_LABEL];
   char        x_max       [LEN_LABEL];
   /*---(prepare)------------------------*/
   yPARSE_outclear  ();
   /*---(defense)------------------------*/
   if (VALID_tab (a_tab) == 0)  return -1;
   if (TAB_used  (a_tab) <= 0)  return  0;
   /*---(prepare)------------------------*/
   TAB_name    (a_tab, x_name);
   x_cols = COL_max  (a_tab) - 1;
   x_rows = ROW_max  (a_tab) - 1;
   x_wide = TAB_colwide (a_tab);
   x_tall = TAB_rowtall (a_tab);
   x_type = TAB_type (a_tab);
   str4gyges (a_tab , 0     , 0    , 0, 0, x_min, YSTR_LEGAL);
   str4gyges (a_tab, x_cols, x_rows, 0, 0, x_max, YSTR_LEGAL);
   /*---(write)--------------------------*/
   rc = yPARSE_fullwrite ("tab", x_name, x_min, x_max, x_wide, x_tall, 1, x_type);
   if (rc < 0)   return rc;
   /*---(complete)-----------------------*/
   return 1;
}

char
TAB_writer_all          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   int         i           =    0;
   char        c           =    0;
   /*---(walk)---------------------------*/
   yPARSE_verb_begin ("tab");
   for (i = 0; i < MAX_TABS; ++i) {
      rc = TAB_writer   (i);
      if (rc <  1)  continue;
      ++c;
      yPARSE_verb_break (c);
   }
   yPARSE_verb_end   (c);
   /*---(complete)-----------------------*/
   return c;
}



/*====================------------------------------------====================*/
/*===----                      unit testing                            ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }


char*        /*-> unit test accessor -----------------[ light  [us.B60.2A3.F2]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
TAB__unit          (char *a_question, int a_tab)
{
   /*---(locals)-------------------------*/
   char        rc          =    0;
   int         i           =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   char        x_abs       =    0;
   uchar       x_label     =  '-';
   char        x_beg       [LEN_LABEL]   = "";
   char        x_end       [LEN_LABEL]   = "";
   char        x_cur       [LEN_LABEL]   = "";
   char        x_max       [LEN_LABEL]   = "";
   /*---(parse location)-----------------*/
   strcpy  (unit_answer, "TAB              : label could not be parsed");
   /*---(prepare data)-------------------*/
   strcpy  (unit_answer, "TAB              : locations could not be prepared");
   if (strcmp (a_question, "curr") == 0)  a_tab = CTAB;
   if (rc >= 0)  rc = str4gyges  (a_tab, s_tabs [a_tab].bcol, s_tabs [a_tab].brow, 0, 0, x_beg, YSTR_CHECK);
   if (rc >= 0)  rc = str4gyges  (a_tab, s_tabs [a_tab].ecol, s_tabs [a_tab].erow, 0, 0, x_end, YSTR_CHECK);
   if (rc >= 0)  rc = str4gyges  (a_tab, s_tabs [a_tab].ccol, s_tabs [a_tab].crow, 0, 0, x_cur, YSTR_CHECK);
   if (rc >= 0)  rc = str4gyges  (a_tab, s_tabs [a_tab].ncol - 1, s_tabs [a_tab].nrow - 1, 0, 0, x_max, YSTR_CHECK);
   if (rc <  0)  return unit_answer;
   x_label = LABEL_tab (a_tab);
   /*---(overall)------------------------*/
   strcpy  (unit_answer, "TAB              : question not understood");
   if      (strcmp(a_question, "count"         ) == 0) {
      snprintf(unit_answer, LEN_FULL, "TAB count        : %d", NTAB);
   }
   else if (strcmp(a_question, "info"          ) == 0) {
      if (!LEGAL_TAB (a_tab))  snprintf(unit_answer, LEN_FULL, "TAB info     (%c) : %-12.12s -       -       -       -       -", x_label, s_tabs [a_tab].name);
      else                     snprintf(unit_answer, LEN_FULL, "TAB info     (%c) : %-12.12s %-7.7s %-7.7s %-7.7s %-7.7s %d", x_label, s_tabs [a_tab].name, x_beg, x_end, x_cur, x_max, s_tabs [a_tab].c);
   }
   else if (strcmp(a_question, "curr"          ) == 0) {
      snprintf(unit_answer, LEN_FULL, "TAB curr     (%c) : %-12.12s %-7.7s %-7.7s %-7.7s %-7.7s %d", x_label, s_tabs [a_tab].name, x_beg, x_end, x_cur, x_max, s_tabs [a_tab].c);
   }
   else if (strcmp(a_question, "defs"          )  == 0) {
      snprintf (unit_answer, LEN_FULL, "TAB defaults     : col=%2d, row=%2d", s_tabs[a_tab].defwide, s_tabs[a_tab].deftall);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}
