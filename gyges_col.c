/*============================----beg-of-source---============================*/
#include   "gyges.h"




/*====================------------------------------------====================*/
/*===----                         program level                        ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
COL_init             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(add buffer commands)------------*/
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFER, "colwide"     , ""    , "sii"  , COL_resize                 , "change a column size using address"   );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFER, "colreset"    , ""    , ""     , COL_reset                  , "reset all columns to default width"   );
   /*---(add yparse specification)-------*/
   rc = yPARSE_handler (FILE_COLS    , "width"     , 4.2, "Lss---------", COL_writer_all  , COL_reader      , "------------" , "label,wid,cnt"                        , "gyges cols (x-axis)"      );
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> clear all column customizations ----[ leaf   [ge.A53.123.40]*/ /*-[02.0000.014.!]-*/ /*-[--.---.---.--]-*/
COL_clear            (int a_tab)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_col       =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_senter  (__FUNCTION__);
   DEBUG_LOCS   yLOG_svalue  ("a_tab"     , a_tab);
   /*---(defense)------------------------*/
   rc = TAB_valid (a_tab);
   DEBUG_LOCS   yLOG_snote   (rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(initialize columns)-------------*/
   DEBUG_LOCS   yLOG_svalue  ("MAX_COLS"  , MAX_COLS);
   for (x_col = 0; x_col < MAX_COLS; ++x_col) {
      /*---(characteristics)-------------*/
      s_tabs [a_tab].cols [x_col].w       = s_tabs [a_tab].defwide;
      s_tabs [a_tab].cols [x_col].x       = 0;
      s_tabs [a_tab].cols [x_col].c       = 0;
      /*---(name)------------------------*/
      COL_defname (a_tab, x_col, s_tabs [a_tab].cols [x_col].l);
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
COL__validity        (char a_mode, int a_tab, int a_col)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   static char x_mode      =  '-';
   static int  x_tab       = -666;
   static int  x_col       = -666;
   static int  x_shorts    =    0;
   /*---(shortcut)-----------------------*/
   if (a_mode == x_mode && a_tab == x_tab && a_col == x_col) {
      ++x_shorts;
      return 1;
   }
   /*---(tab part)-----------------------*/
   rc = TAB_valid (a_tab);
   --rce;  if (rc    < 0)                                    return rce;
   /*---(col part)-----------------------*/
   --rce;  if (a_col < 0)                                    return rce;
   --rce;
   if      (a_mode == 'L' && a_col >= s_tabs [a_tab].ncol)   return rce;
   else if (a_col >= MAX_COLS)                               return rce;
   /*---(save success)-------------------*/
   x_mode  = a_mode;
   x_tab   = a_tab;
   x_col   = a_col;
   /*---(complete)-----------------------*/
   return 0;
}

char  COL_valid    (int a_tab, int a_col) { return COL__validity ('V', a_tab, a_col); }
char  COL_legal    (int a_tab, int a_col) { return COL__validity ('L', a_tab, a_col); }



/*====================------------------------------------====================*/
/*===----                       characteristics                        ----===*/
/*====================------------------------------------====================*/
static void  o___CHARS___________o () { return; }

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



/*====================------------------------------------====================*/
/*===----                        screen positions                      ----===*/
/*====================------------------------------------====================*/
static void  o___POSITION________o () { return; }

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



/*====================------------------------------------====================*/
/*===----                          column naming                       ----===*/
/*====================------------------------------------====================*/
static void  o___NAMING__________o () { return; }

char         /*-> return the col xpos ----------------[ ------ [gc.210.313.21]*/ /*-[00.0000.204.!]-*/ /*-[--.---.---.--]-*/
COL_defname          (int a_tab, int a_col, char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_name == NULL)   return rce;
   strlcpy (a_name, "??", LEN_ABBR);
   rc = COL_valid (a_tab, a_col);
   --rce;  if (rc < 0) return rce;
   /*---(make name)----------------------*/
   if        (a_col < 26)  {
      a_name [0] = '-';
      a_name [1] = a_col + 'a';
   } else  {
      a_name [0] = (a_col / 26) - 1 + 'a';
      a_name [1] = (a_col % 26) + 'a';
   }
   a_name [2] = '\0';
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> return the col xpos ----------------[ ------ [gc.210.313.21]*/ /*-[00.0000.204.!]-*/ /*-[--.---.---.--]-*/
COL_name             (int a_tab, int a_col, char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_name == NULL)   return rce;
   strlcpy (a_name, "??", LEN_ABBR);
   rc = COL_valid (a_tab, a_col);
   --rce;  if (rc < 0) return rce;
   /*---(copy name)----------------------*/
   strlcpy (a_name, s_tabs [a_tab].cols [a_col].l, LEN_ABBR);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          column sizing                       ----===*/
/*====================------------------------------------====================*/
static void  o___SIZING__________o () { return; }

char         /*-> change the col width ---------------[ ------ [gc.320.312.31]*/ /*-[00.0000.404.5]-*/ /*-[--.---.---.--]-*/
COL_widen            (int a_tab, int a_col, int a_size)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_row       =    0;
   int         x_max       =    0;
   tCELL      *x_curr      = NULL;
   /*---(defense)------------------------*/
   rc = COL_valid (a_tab, a_col);
   --rce;  if (rc < 0) return rc;
   /*---(limits)-------------------------*/
   if (a_size  < MIN_WIDTH)    a_size = MIN_WIDTH;
   if (a_size  > MAX_WIDTH)    a_size = MAX_WIDTH;
   /*---(check existing)-----------------*/
   if (s_tabs [a_tab].cols [a_col].w == a_size)  return 1;
   /*---(set default)--------------------*/
   s_tabs [a_tab].cols [a_col].w = a_size;
   /*---(find last row)------------------*/
   x_max = ROW_max (a_tab);
   /*---(update column printables)-------*/
   for (x_row = 0; x_row < x_max; ++x_row) {
      /*---(check for cell)--------------*/
      x_curr = LOC_cell_at_loc (a_col, x_row, a_tab);
      if (x_curr == NULL) continue;
      /*---(update merged cells)---------*/
      if (x_curr->t == YCALC_DATA_MERGED)  yCALC_calc_from (x_curr->ycalc);
      /*---(update printable)------------*/
      api_ycalc_printer (x_curr);
      /*---(done)------------------------*/
   }
   /*---(reset headers)------------------*/
   yVIKEYS_map_refresh ();
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> change the col width ---------------[ ------ [gc.320.312.31]*/ /*-[00.0000.404.5]-*/ /*-[--.---.---.--]-*/
COL_resize           (char *a_name, int a_size, int a_count)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_off       =    0;
   /*---(defense)------------------------*/
   rc = str2gyges (a_name, &x_tab, &x_col, NULL, NULL, 0);
   --rce;  if (rc < 0) return rc;
   /*---(resize)-------------------------*/
   if (a_count == 0)  a_count = 1;
   for (x_off = 0; x_off < a_count; ++x_off) {
      rc = COL_widen (x_tab, x_col + x_off, a_size);
      DEBUG_INPT  yLOG_value   ("widen"     , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> change the col width ---------------[ ------ [gc.320.312.31]*/ /*-[00.0000.404.5]-*/ /*-[--.---.---.--]-*/
COL_reset            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         x_max       =    0;
   int         i           =    0;
   /*---(resize)-------------------------*/
   x_max  = COL_max (CTAB);
   for (i = 0; i <= x_max; ++i) {
      rc = COL_widen (CTAB, i, s_tabs [CTAB].defwide);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> change cell column width -----------[ ------ [gc.E91.292.69]*/ /*-[02.0000.303.Y]-*/ /*-[--.---.---.--]-*/
COL_visual         (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num)
{  /*---(design notes)-------------------*/
   /*  update all cells to new width, either a standard size, or a specific   */
   /*  value communicated as a negative number.                               */
   /*---(locals)-----------+-----------+-*/
   int         x_prev      = 0;
   int         x_width     = 0;
   int         x_row       = 0;
   tCELL      *x_cell      = NULL;
   /*---(stop early)-------------------------*/
   if (a_head->row != a_curr->row)  return 1;
   /*---(adjust)----------------------*/
   if (a_num <   0) {
      x_width                = -(a_num);
   } else {
      x_width = x_prev = COL_width (a_curr->tab, a_curr->col);
      switch (a_num) {
      case  'm' : x_width    = 0;                           break;
      case  'd' : x_width    = DEF_WIDTH;                   break;
      case  'n' : x_width    = 8;                           break;
      case  'N' : x_width    = 12;                          break;
      case  'w' : x_width    = 20;                          break;
      case  'W' : x_width    = 50;                          break;
      case  'h' : x_width   -= 1;                           break;
      case  'l' : x_width   += 1;                           break;
      case  'H' : x_width    = ((x_width / 5) * 5);         break;
      case  'L' : x_width    = (((x_width / 5) + 1) * 5);   break;
      }
   }
   /*---(history)----------------------*/
   if (a_mode == HIST_BEG && a_head != a_curr)   a_mode = HIST_ADD;
   HIST_width  (a_mode, a_curr->tab, a_curr->col, a_curr->row, x_prev, x_width);
   /*---(set width)--------------------*/
   COL_widen  (a_curr->tab, a_curr->col, x_width);
   /*---(reset headers)---------------*/
   yVIKEYS_map_refresh ();
   /*---(complete)---------------------------*/
   DEBUG_CELL  yLOG_exit   (__FUNCTION__);
   return 0;
}

char         /*-> set the default width --------------[ ------ [gc.210.213.11]*/ /*-[00.0000.G03.!]-*/ /*-[--.---.---.--]-*/
COL_defwidth         (int a_tab, int a_size)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_def       =    0;
   int         x_max       =    0;
   int         x_col       =    0;
   /*---(defense)------------------------*/
   rc = TAB_valid (a_tab);
   --rce;  if (rc < 0) return rc;
   /*---(prepare)------------------------*/
   x_def  = s_tabs [a_tab].defwide;
   x_max  = COL_max (a_tab);
   /*---(update column printables)-------*/
   for (x_col = 0; x_col < x_max; ++x_col) {
      if (s_tabs [a_tab].cols [x_col].w != x_def)  continue;
      COL_widen (a_tab, x_col, a_size);
   }
   /*---(set default)--------------------*/
   s_tabs [a_tab].defwide = a_size;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> return the col width ---------------[ ------ [gc.210.213.11]*/ /*-[00.0000.G03.!]-*/ /*-[--.---.---.--]-*/
COL_width            (int a_tab, int a_col)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(defense)------------------------*/
   rc = COL_valid (a_tab, a_col);
   --rce;  if (rc < 0) return rc;
   /*---(complete)-----------------------*/
   return s_tabs [a_tab].cols [a_col].w;
}



/*====================------------------------------------====================*/
/*===----                     freezing columns                         ----===*/
/*====================------------------------------------====================*/
static void  o___FREEZING________o () { return; }

char         /*-> change the frozen cols -------------[ ------ [gc.430.323.31]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
COL_freeze           (int a_tab, int a_bcol, int a_ecol)
{
   char        rc          =    0;
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



/*====================------------------------------------====================*/
/*===----                    yparse record handling                    ----===*/
/*====================------------------------------------====================*/
static void  o___YPARSE__________o () { return; }

char
COL_reader           (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   char        x_verb      [LEN_LABEL];
   char        x_label     [LEN_LABEL];
   int         x_size      =    0;
   int         x_count     =    0;
   char        x_type      =  '-';
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(get verb)-----------------------*/
   rc = yPARSE_popstr (x_verb);
   DEBUG_INPT   yLOG_value   ("pop verb"  , rc);
   DEBUG_INPT   yLOG_info    ("x_verb"    , x_verb);
   --rce;  if (strcmp ("width", x_verb) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   rc = yPARSE_popstr (x_label);
   DEBUG_INPT   yLOG_value   ("pop max"   , rc);
   DEBUG_INPT   yLOG_info    ("max"       , x_label);
   /*---(size)---------------------------*/
   rc = yPARSE_popint  (&x_size);
   DEBUG_INPT   yLOG_value   ("pop width" , rc);
   DEBUG_INPT   yLOG_value   ("width"     , x_size);
   rc = yPARSE_popint  (&x_count);
   DEBUG_INPT   yLOG_value   ("pop count" , rc);
   DEBUG_INPT   yLOG_value   ("count"     , x_count);
   /*---(resize)-------------------------*/
   rc = COL_resize (x_label, x_size, x_count);
   DEBUG_INPT   yLOG_value   ("resize"    , rc);
   if (rc < 0) {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 1;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
COL_writer              (int a_tab, int a_col)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        c           =    0;
   int         n           =    0;
   int         j           =    0;
   int         x_beg       =    0;
   int         x_end       =    0;
   int         x_max       =    0;
   int         k           =    0;
   int         x_def       =    0;
   int         x_size      =    0;
   int         x_prev      =    0;
   char        x_label     [LEN_LABEL];
   int         x_used      =    0;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(prepare tab)--------------------*/
   DEBUG_OUTP   yLOG_value   ("a_tab"     , a_tab);
   rc = TAB_valid (a_tab);
   --rce; if (rc < 0) { 
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare range)------------------*/
   if (a_col < 0) {
      x_beg = 0;
      x_end = COL_max (a_tab) - 1;
   } else {
      x_beg = x_end = a_col;
   }
   DEBUG_OUTP   yLOG_value   ("x_beg"     , x_beg);
   DEBUG_OUTP   yLOG_value   ("x_end"     , x_end);
   x_max = COL_max (a_tab) - 1;
   DEBUG_OUTP   yLOG_value   ("x_max"     , x_max);
   /*---(prepare default)----------------*/
   x_def  = TAB_colwide (a_tab);
   DEBUG_OUTP   yLOG_value   ("x_def"     , x_def);
   if (x_beg > 0)  x_prev = COL_width  (a_tab, x_beg - 1);
   /*---(run range)----------------------*/
   for (j = x_beg; j <= x_end; ++j) {
      /*---(filter)----------------------*/
      if (COL_legal (a_tab, j) < 0)  continue;
      x_size = COL_width  (a_tab, j);
      DEBUG_OUTP   yLOG_value   ("x_size"    , x_size);
      if (x_size == x_def )  continue;
      if (x_size == x_prev)  continue;
      /*---(check repeats)---------------*/
      n = 1;
      for (k = j + 1; k <= x_max; ++k) {
         if (x_size != COL_width  (a_tab, k))  break;
         ++n;
      }
      DEBUG_OUTP   yLOG_value   ("n"         , n);
      /*---(write)-----------------------*/
      rc = str4gyges (a_tab, j, 0, 0, x_label);
      yPARSE_fullwrite ("width", x_label, x_size, n);
      /*---(clear)-----------------------*/
      if (x_beg != x_end)  yPARSE_outclear  ();
      /*---(next)------------------------*/
      c += n;
      j += n - 1;
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return c;
}

char
COL_writer_all          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         c           =    0;
   /*---(walk)---------------------------*/
   yPARSE_verb_begin ("width");
   yPARSE_verb_break (c);
   for (x_tab = 0; x_tab < MAX_TABS; ++x_tab) {
      for (x_col = 0; x_col < MAX_COLS; ++x_col) {
         rc = COL_writer   (x_tab, x_col);
         if (rc <= 0)    continue;
         ++c;
         yPARSE_verb_break (c);
      }
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
COL__unit          (char *a_question, char *a_label)
{
   /*---(locals)-------------------------*/
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_abbr      =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   char        x_name      [LEN_LABEL]   = "";
   /*---(parse location)-----------------*/
   strcpy  (unit_answer, "COL              : label could not be parsed");
   if (a_label == NULL)  return unit_answer;
   rc = str2gyges  (a_label, &x_tab, &x_col, &x_row, NULL, 0);
   if (rc <  0)  return unit_answer;
   rc = COL_valid (x_tab, x_col);
   if (rc <  0)  return unit_answer;
   x_abbr = TAB_label (x_tab);
   /*---(overall)------------------------*/
   strcpy  (unit_answer, "COL              : question not understood");
   if      (strcmp(a_question, "col_info"      ) == 0) {
      if (COL_legal (x_tab, x_col) < 0) {
         snprintf(unit_answer, LEN_UNIT, "COL info         : -t, --,   -#,   -w,   -x,   -c");
      } else {
         snprintf(unit_answer, LEN_UNIT, "COL info         : %ct, %-2.2s, %3d#, %3dw, %3dx, %3dc", x_abbr, s_tabs [x_tab].cols [x_col].l, x_col, s_tabs [x_tab].cols [x_col].w, s_tabs [x_tab].cols [x_col].x, s_tabs [x_tab].cols [x_col].c);
      }
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}
