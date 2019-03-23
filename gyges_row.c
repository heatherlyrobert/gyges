/*============================----beg-of-source---============================*/
#include   "gyges.h"





/*====================------------------------------------====================*/
/*===----                         program level                        ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
ROW_init             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(add buffer commands)------------*/
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFER, "rowtall"     , ""    , "sii"  , ROW_resize                 , "change a row size using address"      );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFER, "rowreset"    , ""    , ""     , ROW_reset                  , "reset all rows to default width"      );
   /*---(add yparse specification)-------*/
   rc = yPARSE_handler (FILE_ROWS    , "height"    , 4.3, "Lss---------", ROW_reader      , ROW_writer_all  , "------------" , "label,tal,cnt"                        , "gyges rows (y-axis)"      );
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return rc;
}

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
      /*---(characteristics)-------------*/
      s_tabs [a_tab].rows [x_row].h = s_tabs [a_tab].deftall;
      /*> s_tabs [a_tab].rows [x_row].y = 0;                                          <*/
      s_tabs [a_tab].rows [x_row].c = 0;
      /*---(done)------------------------*/
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



char         /*-> tbd --------------------------------[ leaf   [ge.330.215.40]*/ /*-[00.0000.194.I]-*/ /*-[--.---.---.--]-*/
ROW__validity        (char a_mode, int a_tab, int a_row)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   static char x_mode      =  '-';
   static int  x_tab       = -666;
   static int  x_row       = -666;
   static int  x_shorts    =    0;
   /*---(shortcut)-----------------------*/
   if (a_mode == x_mode && a_tab == x_tab && a_row == x_row) {
      ++x_shorts;
      return 1;
   }
   /*---(tab part)-----------------------*/
   rc = VALID_tab (a_tab);
   --rce;  if (rc   == 0)                                    return rce;
   /*---(col part)-----------------------*/
   --rce;  if (a_row < 0)                                    return rce;
   --rce;
   if      (a_mode == 'L' && a_row >= s_tabs [a_tab].nrow)   return rce;
   else if (a_row >= MAX_ROWS)                               return rce;
   /*---(save success)-------------------*/
   x_mode  = a_mode;
   x_tab   = a_tab;
   x_row   = a_row;
   /*---(complete)-----------------------*/
   return 0;
}

char  ROW_legal    (int a_tab, int a_row) { return ROW__validity ('L', a_tab, a_row); }



/*====================------------------------------------====================*/
/*===----                       characteristics                        ----===*/
/*====================------------------------------------====================*/
static void  o___CHARS___________o () { return; }

int          /*-> return default col size ------------[ leaf   [gn.210.002.00]*/ /*-[00.0000.10#.6]-*/ /*-[--.---.---.--]-*/
ROW_defmax           (void)
{
   return DEF_ROWS;
}

int          /*-> return max col for tab -------------[ ------ [gn.210.113.11]*/ /*-[00.0000.704.D]-*/ /*-[--.---.---.--]-*/
ROW_max              (int a_tab)
{
   if (!LEGAL_TAB (a_tab))   return -1;
   return s_tabs [a_tab].nrow;
}

int          /*-> indicate if column is used ---------[ ------ [gn.210.212.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
ROW_used             (int a_tab, int a_row)
{
   if (!LEGAL_ROW (a_tab, a_row))   return -1;
   return s_tabs [a_tab].rows [a_row].c;
}

int          /*-> find largest used row in tab -------[ ------ [gn.210.212.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
ROW_maxused          (int a_tab)
{
   int         i           =    0;
   int         x_max       =   -1;
   if (!LEGAL_TAB (a_tab))        return -1;
   for (i = 0; i < s_tabs [a_tab].nrow; ++i) {
      if (s_tabs [a_tab].rows [i].c > 0)  x_max   = i;
   }
   return x_max;
}

int          /*-> update the row count in tab --------[ ------ [gn.210.213.11]*/ /*-[00.0000.503.!]-*/ /*-[--.---.---.--]-*/
ROW_setmax           (int a_tab, int a_count)
{
   int         x_max       =    0;
   x_max = ROW_maxused (a_tab);
   if (x_max >= a_count)  a_count = x_max + 1;
   if (a_count < 1)  a_count = 1;
   if (!VALID_row (a_count - 1))  return -2;
   s_tabs [a_tab].nrow = a_count;
   if (a_tab == CTAB)  CROW = a_count;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                            row sizing                        ----===*/
/*====================------------------------------------====================*/
static void  o___SIZING__________o () { return; }

char         /*-> change the col width ---------------[ ------ [gc.320.312.31]*/ /*-[00.0000.404.5]-*/ /*-[--.---.---.--]-*/
ROW_heighten         (int a_tab, int a_row, int a_size)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_col       =    0;
   int         x_max       =    0;
   tCELL      *x_curr      = NULL;
   /*---(defense)------------------------*/
   if (!LEGAL_ROW (a_tab, a_row))  return -1;
   /*---(limits)-------------------------*/
   if (a_size  < MIN_HEIGHT)    a_size = MIN_HEIGHT;
   if (a_size  > MAX_HEIGHT)    a_size = MAX_HEIGHT;
   /*---(check existing)-----------------*/
   if (s_tabs [a_tab].rows [a_row].h == a_size)  return 1;
   /*---(set default)--------------------*/
   s_tabs [a_tab].rows [a_row].h = a_size;
   /*---(find last row)------------------*/
   x_max = ROW_max (a_tab);
   /*---(update column printables)-------*/
   for (x_col = 0; x_col < x_max; ++x_col) {
      /*---(check for cell)--------------*/
      x_curr = LOC_cell_at_loc (a_tab, x_col, a_row);
      if (x_curr == NULL) continue;
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
ROW_resize           (char *a_name, int a_size, int a_count)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_row       =    0;
   int         x_off       =    0;
   /*---(defense)------------------------*/
   rc = str2gyges (a_name, &x_tab, NULL, &x_row, NULL, NULL, 0, YSTR_LEGAL);
   --rce;  if (rc < 0) return rc;
   /*---(resize)-------------------------*/
   if (a_count == 0)  a_count = 1;
   for (x_off = 0; x_off < a_count; ++x_off) {
      rc = ROW_heighten (x_tab, x_row + x_off, a_size);
      DEBUG_INPT  yLOG_value   ("heighten"  , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> change the col width ---------------[ ------ [gc.320.312.31]*/ /*-[00.0000.404.5]-*/ /*-[--.---.---.--]-*/
ROW_reset            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         x_max       =    0;
   int         i           =    0;
   /*---(resize)-------------------------*/
   x_max  = ROW_max (CTAB);
   for (i = 0; i <= x_max; ++i) {
      rc = ROW_heighten (CTAB, i, s_tabs [CTAB].deftall);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> change cell column width -----------[ ------ [gc.E91.292.69]*/ /*-[02.0000.303.Y]-*/ /*-[--.---.---.--]-*/
ROW_visual         (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num)
{  /*---(design notes)-------------------*/
   /*  update all cells to new width, either a standard size, or a specific   */
   /*  value communicated as a negative number.                               */
   /*---(locals)-----------+-----------+-*/
   int         x_prev      = 0;
   int         x_height    = 0;
   int         x_row       = 0;
   tCELL      *x_cell      = NULL;
   /*---(stop early)-------------------------*/
   if (a_head->row != a_curr->row)  return 1;
   /*---(adjust)----------------------*/
   if (a_num <   0) {
      x_height                = -(a_num);
   } else {
      x_height = x_prev = ROW_height (a_curr->tab, a_curr->row);
      switch (a_num) {
      case  'm' : x_height    = 0;                           break;
      case  'd' : x_height    = DEF_HEIGHT;                  break;
      case  'n' : x_height    = 1;                           break;
      case  'j' : x_height   -= 1;                           break;
      case  'k' : x_height   += 1;                           break;
      }
   }
   /*---(history)----------------------*/
   if (a_mode == HIST_BEG && a_head != a_curr)   a_mode = HIST_ADD;
   HIST_height (a_mode, a_curr->tab, a_curr->col, a_curr->row, x_prev, x_height);
   /*---(set width)--------------------*/
   ROW_heighten (a_curr->tab, a_curr->row, x_height);
   /*---(reset headers)---------------*/
   yVIKEYS_map_refresh ();
   /*---(complete)---------------------------*/
   DEBUG_CELL  yLOG_exit   (__FUNCTION__);
   return 0;
}

char         /*-> set the default width --------------[ ------ [gc.210.213.11]*/ /*-[00.0000.G03.!]-*/ /*-[--.---.---.--]-*/
ROW_defheight        (int a_tab, int a_size)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_def       =    0;
   int         x_max       =    0;
   int         x_row       =    0;
   /*---(defense)------------------------*/
   rc = VALID_tab (a_tab);
   --rce;  if (rc == 0) return rc;
   /*---(prepare)------------------------*/
   x_def  = s_tabs [a_tab].deftall;
   x_max  = ROW_max (a_tab);
   /*---(update column printables)-------*/
   for (x_row = 0; x_row < x_max; ++x_row) {
      if (s_tabs [a_tab].rows [x_row].h != x_def)  continue;
      ROW_heighten (a_tab, x_row, a_size);
   }
   /*---(set default)--------------------*/
   s_tabs [a_tab].deftall = a_size;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> return the col width ---------------[ ------ [gc.210.213.11]*/ /*-[00.0000.G03.!]-*/ /*-[--.---.---.--]-*/
ROW_height           (int a_tab, int a_row)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(defense)------------------------*/
   if (!LEGAL_ROW (a_tab, a_row))  return -1;
   --rce;  if (rc < 0) return rc;
   /*---(complete)-----------------------*/
   return s_tabs [a_tab].rows [a_row].h;
}



/*====================------------------------------------====================*/
/*===----                     freezing columns                         ----===*/
/*====================------------------------------------====================*/
static void  o___FREEZING________o () { return; }

char         /*-> change the frozen rows -------------[ ------ [gc.430.323.31]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
ROW_freeze           (int a_tab, int a_brow, int a_erow)
{
   int    x_row;
   if (!LEGAL_ROW (a_tab, a_brow))  return -1;
   if (!LEGAL_ROW (a_tab, a_erow))  return -2;
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
   if (!LEGAL_TAB (a_tab))  return -1;
   s_tabs [a_tab].froz_row    = '-';
   s_tabs [a_tab].froz_brow   = 0;
   s_tabs [a_tab].froz_erow   = 0;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    yparse record handling                    ----===*/
/*====================------------------------------------====================*/
static void  o___YPARSE__________o () { return; }

char
ROW_reader           (void)
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
   --rce;  if (strcmp ("height", x_verb) != 0) {
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
   rc = ROW_resize (x_label, x_size, x_count);
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
ROW_writer              (int a_tab, int a_row)
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
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(prepare tab)--------------------*/
   DEBUG_OUTP   yLOG_value   ("a_tab"     , a_tab);
   rc = VALID_tab (a_tab);
   --rce; if (rc == 0) { 
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare range)------------------*/
   if (a_row < 0) {
      x_beg = 0;
      x_end = ROW_max (a_tab) - 1;
   } else {
      x_beg = x_end = a_row;
   }
   DEBUG_OUTP   yLOG_value   ("x_beg"     , x_beg);
   DEBUG_OUTP   yLOG_value   ("x_end"     , x_end);
   x_max = ROW_max (a_tab) - 1;
   DEBUG_OUTP   yLOG_value   ("x_max"     , x_max);
   /*---(prepare default)----------------*/
   x_def  = TAB_rowtall (a_tab);
   DEBUG_OUTP   yLOG_value   ("x_def"     , x_def);
   if (x_beg > 0)  x_prev = ROW_height (a_tab, x_beg - 1);
   /*---(run range)----------------------*/
   for (j = x_beg; j <= x_end; ++j) {
      /*---(filter)----------------------*/
      if (ROW_legal (a_tab, j) < 0)  continue;
      x_size = ROW_height (a_tab, j);
      DEBUG_OUTP   yLOG_value   ("x_size"    , x_size);
      if (x_size == x_def )  continue;
      if (x_size == x_prev)  continue;
      /*---(check repeats)---------------*/
      n = 1;
      for (k = j + 1; k <= x_max; ++k) {
         if (x_size != ROW_height (a_tab, k))  break;
         ++n;
      }
      DEBUG_OUTP   yLOG_value   ("n"         , n);
      /*---(write)-----------------------*/
      rc = str4gyges (a_tab, 0, j, 0, 0, x_label, YSTR_LEGAL);
      yPARSE_fullwrite ("height", x_label, x_size, n);
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
ROW_writer_all          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_row       =    0;
   int         c           =    0;
   /*---(walk)---------------------------*/
   yPARSE_verb_begin ("height");
   for (x_tab = 0; x_tab < MAX_TABS; ++x_tab) {
      for (x_row = 0; x_row < MAX_ROWS; ++x_row) {
         rc = ROW_writer   (x_tab, x_row);
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
ROW__unit          (char *a_question, char *a_label)
{
   /*---(locals)-------------------------*/
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   int         x_abbr      =    0;
   char        x_name      [LEN_LABEL]   = "";
   /*---(parse location)-----------------*/
   strcpy  (unit_answer, "ROW              : label could not be parsed");
   if (a_label == NULL)  return unit_answer;
   if (strcmp (a_label, "curr") == 0) {
      x_tab = CTAB;
      x_col = CCOL;
      x_row = CROW;
   } else {
      rc = str2gyges  (a_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_LEGAL);
      if (rc <  0)  return unit_answer;
      if (!VALID_tab (x_tab))  return unit_answer;
      if (!VALID_row (x_row))  return unit_answer;
   }
   x_abbr = LABEL_tab (x_tab);
   /*---(overall)------------------------*/
   strcpy  (unit_answer, "ROW              : question not understood");
   if      (strcmp(a_question, "count"         ) == 0) {
      snprintf(unit_answer, LEN_FULL, "ROW count        : %ct, %4d", x_abbr, ROW_max (x_tab));
   }
   else if (strcmp(a_question, "info"          ) == 0) {
      if (!LEGAL_ROW (x_tab, x_row)) {
         snprintf(unit_answer, LEN_FULL, "ROW info         : %ct, %4s, %4d#,   -h,   -c", x_abbr, LABEL_row (x_row), x_row);
      } else {
         snprintf(unit_answer, LEN_FULL, "ROW info         : %ct, %4s, %4d#, %3dh, %3dc", x_abbr, LABEL_row (x_row), x_row, s_tabs [x_tab].rows [x_row].h, s_tabs [x_tab].rows [x_row].c);
      }
   }
   else if (strcmp(a_question, "curr"          ) == 0) {
      snprintf(unit_answer, LEN_FULL, "ROW curr         : %ct, %4s, %4d#, %3dh, %3dc", x_abbr, LABEL_row (x_row), x_row, s_tabs [x_tab].rows [x_row].h, s_tabs [x_tab].rows [x_row].c);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}

