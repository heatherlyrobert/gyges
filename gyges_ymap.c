/*============================----beg-of-source---============================*/
#include   "gyges.h"



/*====================------------------------------------====================*/
/*===----                     labels and positions                     ----===*/
/*====================------------------------------------====================*/
static void   o___LABEL___________o (void) { return; }

char         /*-> break label into coordinates -------[ ------ [gc.722.112.13]*/ /*-[01.0000.304.#]-*/ /*-[--.---.---.--]-*/
api_ymap_locator        (char a_strict, char *a_label, ushort *u, ushort *x, ushort *y, ushort *z)
{
   char        rce         =    0;
   char        rc          =    0;
   int         x_buf, x_col, x_row;
   ushort      x_size, y_size;
   /*---(header)-------------------------*/
   DEBUG_FILE   yLOG_enter   (__FUNCTION__);
   /*---(defaults)-----------------------*/
   if (u != NULL)  *u = 0;
   if (x != NULL)  *x = 0;
   if (y != NULL)  *y = 0;
   if (z != NULL)  *z = 0;
   /*---(defense)------------------------*/
   DEBUG_FILE   yLOG_point   ("a_label"   , a_label);
   --rce;  if (a_label == NULL) {
      DEBUG_FILE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(legality)-----------------------*/
   rc = str2gyges (a_label, &x_buf, &x_col, &x_row, NULL, NULL, 0, YSTR_USABLE);
   DEBUG_FILE   yLOG_value   ("str2gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_FILE   yLOG_note    ("address not legal");
      DEBUG_FILE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (a_strict != 'y') {
      if (u != NULL)  *u = x_buf;
      if (x != NULL)  *x = x_col;
      if (y != NULL)  *y = x_row;
      DEBUG_FILE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(existance)----------------------*/
   --rce;  if (!TAB_live (x_buf)) {
      DEBUG_FILE   yLOG_note    ("universe/tab not available");
      DEBUG_FILE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(availability)-------------------*/
   rc = TAB_size_detail (x_buf, &x_size, &y_size);
   --rce;  if (rc < 0) {
      DEBUG_FILE   yLOG_note    ("can not get universe/tab sizes");
      DEBUG_FILE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_FILE   yLOG_value   ("x_size"    , x_size);
   --rce;  if (x_size <  x_col) {
      DEBUG_FILE   yLOG_note    ("column not available");
      DEBUG_FILE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_FILE   yLOG_value   ("y_size"    , y_size);
   --rce;  if (y_size <  x_row) {
      DEBUG_FILE   yLOG_note    ("row not available");
      DEBUG_FILE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save-back)----------------------*/
   if (u != NULL)  *u = x_buf;
   if (x != NULL)  *x = x_col;
   if (y != NULL)  *y = x_row;
   /*---(complete)-----------------------*/
   DEBUG_FILE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> return address for coordinates -----[ ------ [gc.722.112.13]*/ /*-[01.0000.304.#]-*/ /*-[--.---.---.--]-*/
api_ymap_addressor      (char a_strict, char *a_label, ushort u, ushort x, ushort y, ushort z)
{
   char        rce         =    0;
   char        rc          =    0;
   char        x_label     [LEN_LABEL] = "";
   ushort      x_size, y_size;
   /*---(header)-------------------------*/
   DEBUG_FILE   yLOG_enter   (__FUNCTION__);
   /*---(defaults)-----------------------*/
   if (a_label != NULL)  strlcpy (a_label, "", LEN_LABEL);
   /*---(legality)-----------------------*/
   rc = str4gyges (u, x, y, z, 0, x_label, YSTR_USABLE);
   DEBUG_FILE   yLOG_value   ("str4gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_FILE   yLOG_note    ("address not legal");
      DEBUG_FILE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (a_strict != 'y') {
      if (a_label != NULL)  strlcpy (a_label, x_label, LEN_LABEL);
      DEBUG_FILE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(existance)----------------------*/
   --rce;  if (!TAB_live (u)) {
      DEBUG_FILE   yLOG_note    ("universe/tab not available");
      DEBUG_FILE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(availability)-------------------*/
   rc = TAB_size_detail (u, &x_size, &y_size);
   --rce;  if (rc < 0) {
      DEBUG_FILE   yLOG_note    ("can not get universe/tab sizes");
      DEBUG_FILE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_FILE   yLOG_value   ("x_size"    , x_size);
   --rce;  if (x_size < x) {
      DEBUG_FILE   yLOG_note    ("column not available");
      DEBUG_FILE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_FILE   yLOG_value   ("y_size"    , y_size);
   --rce;  if (y_size < y) {
      DEBUG_FILE   yLOG_note    ("row not available");
      DEBUG_FILE   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save-back)----------------------*/
   if (a_label != NULL)  strlcpy (a_label, x_label, LEN_LABEL);
   /*---(complete)-----------------------*/
   DEBUG_FILE   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     map loading support                      ----===*/
/*====================------------------------------------====================*/
static void   o___LOAD____________o (void) { return; }

char
api_ymap_sizer          (char a_axis, ushort *n, ushort *a, ushort *b, ushort *c, ushort *e, ushort *m, ushort *x)
{
   char        rce         =  -10;
   char        rc          =    0;
   DEBUG_MAP    yLOG_senter  (__FUNCTION__);
   DEBUG_MAP    yLOG_schar   (a_axis);
   --rce;  switch (a_axis) {
   case YMAP_UNIV  :
      DEBUG_MAP    yLOG_snote   ("univ");
      DEBUG_MAP    yLOG_sint    (NTAB);
      if (n != NULL)  *n = NTAB;
      if (a != NULL)  *a = 0;
      if (b != NULL)  *b = 0;
      if (c != NULL)  *c = CTAB;
      if (e != NULL)  *e = 0;
      if (m != NULL)  *m = 0;
      if (x != NULL)  *x = 0;
      break;
   case YMAP_XAXIS :
      DEBUG_MAP    yLOG_snote   ("cols");
      DEBUG_MAP    yLOG_sint    (NCOL);
      if (n != NULL)  *n = NCOL;
      if (a != NULL)  yVIEW_size (YVIEW_MAIN, NULL, NULL, a, NULL, NULL);
      if (b != NULL)  *b = BCOL;
      if (c != NULL)  *c = CCOL;
      if (e != NULL)  *e = ECOL;
      if (m != NULL)  *m = COL_min_used (CTAB);
      if (x != NULL)  *x = COL_max_used (CTAB);
      break;
   case YMAP_YAXIS :
      DEBUG_MAP    yLOG_snote   ("rows");
      DEBUG_MAP    yLOG_sint    (NROW);
      if (n != NULL)  *n = NROW;
      if (a != NULL)  yVIEW_size (YVIEW_MAIN, NULL, NULL, NULL, NULL, a);
      if (b != NULL)  *b = BROW;
      if (c != NULL)  *c = CROW;
      if (e != NULL)  *e = EROW;
      if (m != NULL)  *m = ROW_min_used (CTAB);
      if (x != NULL)  *x = ROW_max_used (CTAB);
      break;
   case YMAP_ZAXIS :
      DEBUG_MAP    yLOG_snote   ("depth NOT supported in gyges");
      DEBUG_MAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   case YMAP_WHEN  :
      DEBUG_MAP    yLOG_snote   ("when NOT supported in gyges");
      DEBUG_MAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   default :
      DEBUG_MAP    yLOG_snote   ("axis not understood");
      DEBUG_MAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      break;
   }
   DEBUG_MAP    yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
api_ymap_entry          (char a_axis, ushort a_pos, short *r_ref, uchar *r_wide, uchar *r_used)
{
   char        rce         =  -10;
   char        rc          =    0;
   --rce;  switch (a_axis) {
   case YMAP_UNIV  :
      rc = TAB_entry  (a_pos, r_ref, r_wide, r_used);
      break;
   case YMAP_XAXIS :
      rc = COL_entry  (a_pos, r_ref, r_wide, r_used);
      break;
   case YMAP_YAXIS :
      rc = ROW_entry  (a_pos, r_ref, r_wide, r_used);
      break;
   case YMAP_ZAXIS :
   case YMAP_WHEN  :
      if (r_ref  != NULL)  *r_ref  = 0;
      if (r_wide != NULL)  *r_wide = 0;
      if (r_used != NULL)  *r_used = 0;
      break;
   default :
      return rce;
      break;
   }
   return rc;
}



/*====================------------------------------------====================*/
/*===----                       content update                         ----===*/
/*====================------------------------------------====================*/
static void   o___UPDATE__________o (void) { return; }

char
api_ymap_placer         (char a_axis, ushort b, ushort c, ushort e)
{
   char        rce         =  -10;
   char        rc          =    0;
   DEBUG_MAP    yLOG_senter  (__FUNCTION__);
   DEBUG_MAP    yLOG_schar   (a_axis);
   DEBUG_MAP    yLOG_sint    (b);
   DEBUG_MAP    yLOG_sint    (c);
   DEBUG_MAP    yLOG_sint    (e);
   --rce;  switch (a_axis) {
   case YMAP_UNIV  :
      DEBUG_MAP    yLOG_snote   ("univ NOT ALLOWED");
      DEBUG_MAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      break;
   case YMAP_XAXIS :
      DEBUG_MAP    yLOG_snote   ("cols");
      PTAB->bcol = BCOL = b;
      PTAB->ccol = CCOL = c;
      PTAB->ecol = ECOL = e;
      break;
   case YMAP_YAXIS :
      DEBUG_MAP    yLOG_snote   ("rows");
      PTAB->brow = BROW = b;
      PTAB->crow = CROW = c;
      PTAB->erow = EROW = e;
      break;
   case YMAP_ZAXIS :
      DEBUG_MAP    yLOG_snote   ("depth NOT supported in gyges");
      DEBUG_MAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   case YMAP_WHEN  :
      DEBUG_MAP    yLOG_snote   ("when NOT supported in gyges");
      DEBUG_MAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   default :
      DEBUG_MAP    yLOG_snote   ("axis not understood");
      DEBUG_MAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      break;
   }
   DEBUG_MAP    yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
api_ymap_done           (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tCELL      *x_curr      = NULL;
   char        t           [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(handle by mode)-----------------*/
   switch (yMODE_curr ()) {
   case MODE_MAP    :
      DEBUG_MAP    yLOG_note    ("primarily about map mode");
      break;
   case UMOD_WANDER :
      DEBUG_MAP    yLOG_note    ("not ready for wander yet ;)");
      DEBUG_MAP    yLOG_exit    (__FUNCTION__);
      return 0;
      break;
   default :
      DEBUG_MAP    yLOG_exit    (__FUNCTION__);
      return 0;
      break;
   }
   /*---(get current cell)---------------*/
   DEBUG_MAP    yLOG_complex ("current"   , "%4dt, %4dc, %4dr", CTAB, CCOL, CROW);
   x_curr = LOC_cell_at_curr ();
   DEBUG_MAP    yLOG_point   ("x_curr"    , x_curr);
   /*---(no current content)-------------*/
   if (x_curr == NULL || x_curr->source == NULL) {
      str4gyges (CTAB, CCOL, CROW, 0, 0, t, YSTR_CHECK);
      ySRC_update (t, "·····", "");
      strlcpy (my.reqs_list, "n/a", LEN_RECD);
      strlcpy (my.deps_list, "n/a", LEN_RECD);
      strlcpy (my.like_list, "n/a", LEN_RECD);
      strlcpy (my.rpn_list , "n/a", LEN_RECD);
      strlcpy (g_curr.label, "-"          , LEN_LABEL);
      g_curr.len    = 0;
      g_curr.type   = YCALC_DATA_BLANK;
      g_curr.tab    = -1;
      g_curr.col    = -1;
      g_curr.row    = -1;
      g_curr.align  = '-';
      g_curr.format = '-';
      g_curr.decs   = '-';
      g_curr.unit   = '-';
      DEBUG_MAP    yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(active cell)--------------------*/
   sprintf (t, "%c%c%c%c%c", x_curr->type, x_curr->align, x_curr->format, x_curr->decs, x_curr->unit);
   ySRC_update (x_curr->label, t, x_curr->source);
   yCALC_disp_reqs (x_curr->ycalc, my.reqs_list);
   yCALC_disp_pros (x_curr->ycalc, my.deps_list);
   yCALC_disp_like (x_curr->ycalc, my.like_list);
   strlcpy (g_curr.label, x_curr->label, LEN_LABEL);
   g_curr.len    = x_curr->len;
   g_curr.type   = x_curr->type;
   g_curr.tab    = x_curr->tab;
   g_curr.col    = x_curr->col;
   g_curr.row    = x_curr->row;
   g_curr.align  = x_curr->align;
   g_curr.format = x_curr->format;
   g_curr.decs   = x_curr->decs;
   g_curr.unit   = x_curr->unit;
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         other callbacks                      ----===*/
/*====================------------------------------------====================*/
static void   o___OTHER___________o (void) { return; }

char
api_ymap__touch         (char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   ushort      u, x, y;
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   /*---(get location)-------------------*/
   rc = str2gyges (a_label, &u, &x, &y, NULL, NULL, 0, YSTR_USABLE);
   DEBUG_REGS   yLOG_complex ("original"  , "label=%s, tab=%4d, col=%4d, row=%4d", a_label, u, x, y);
   x_curr = LOC_cell_at_loc (u, x, y);
   DEBUG_REGS   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr == NULL) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (x_curr->type == YCALC_DATA_ERROR) {
      DEBUG_REGS   yLOG_note    ("error cell, just a re-touch");
      CELL_change (NULL, YMAP_NONE, x_curr->tab, x_curr->col, x_curr->row, strdup (x_curr->source));
   } else {
      DEBUG_REGS   yLOG_note    ("no trouble, nothing to do");
   }
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_ymap_mundo          (char a_dir, char a_act, char *a_label, char *a_format, char *a_content)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_source    [LEN_RECD];
   char        x_format    [LEN_LABEL];
   ushort      u, x, y;
   tCELL      *x_curr      = NULL;
   int         n           =    0;
   char       *p           = NULL;
   char       *r           = NULL;
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   DEBUG_HIST  yLOG_info    ("a_format"  , a_format);
   DEBUG_HIST  yLOG_info    ("a_content" , a_content);
   /*---(identify location)--------------*/
   DEBUG_HIST  yLOG_info    ("label"     , a_label);
   if (a_label != NULL && strcmp (a_label, "n/a") != 0) {
      rc = str2gyges (a_label, &u, &x, &y, NULL, NULL, 0, YSTR_ADAPT);
      DEBUG_HIST  yLOG_value   ("str2gyges"   , rc);
      --rce;  if (rc < 0) {
         DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_HIST  yLOG_complex ("loc"       , "%4du, %4dx, %4dy", u, x, y);
      x_curr  = LOC_cell_at_loc (u, x, y);
   }
   /*---(handle request)-----------------*/
   DEBUG_HIST  yLOG_char    ("act"       , a_act);
   switch (a_act) {
   case YMAP_OVERWRITE :
   case YMAP_CLEAR     :
   case YMAP_DELETE    :
      strldchg (a_content, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
      CELL_overwrite (YMAP_NONE, u, x, y, a_content, a_format);
      break;
   case YMAP_SOURCE    :
      strldchg (a_content, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
      CELL_change  (NULL, YMAP_NONE, u, x, y, a_content);
      break;
   case YMAP_WIDTH    :
      NODE__resize (u, IS_COL, x, atoi (a_content), 0, 0);
      break;
   case YMAP_HEIGHT   :
      NODE__resize (u, IS_ROW, y, atoi (a_content), 0, 0);
      break;
   case YMAP_ALIGN    :
      CELL_align    (x_curr, a_content [0]);
      break;
   case YMAP_FORMAT   :
      CELL_format   (x_curr, a_content [0]);
      break;
   case YMAP_DECIMALS :
      CELL_decimals (x_curr, a_content [0]);
      break;
   case YMAP_UNITS    :
      CELL_units    (x_curr, a_content [0]);
      break;
   case YMAP_SYNC     : case 'ù' :
      if (a_dir == '<') {
         DEBUG_HIST  yLOG_note    ("running a sync with undo");
         strlcpy  (x_source, a_content, LEN_RECD);
         DEBUG_HIST  yLOG_info    ("x_source"   , x_source);
         p = strtok_r (x_source, ",", &r);
         while (p != NULL) {
            rc = api_ymap__touch (p);
            p = strtok_r (NULL  , ",", &r);
         }
      } else {
         DEBUG_HIST  yLOG_note    ("ignore sync with redo");
      }
      break;
   case YMAP_RECALC   : case 'ø' :
      if (a_dir == '>') {
         DEBUG_HIST  yLOG_note    ("running recalc error cell with redo");
         strlcpy  (x_source, a_content, LEN_RECD);
         DEBUG_HIST  yLOG_info    ("x_source"   , x_source);
         p = strtok_r (x_source, ",", &r);
         while (p != NULL) {
            rc = api_ymap__touch (p);
            p = strtok_r (NULL  , ",", &r);
         }
      } else {
         DEBUG_HIST  yLOG_note    ("ignore recalc error cell with undo");
      }
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_ymap_formatter      (uchar a_type, uchar a_abbr, ushort u, ushort x, ushort y, ushort z, uchar *r)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   char        x_prev      =    0;
   char        x_new       =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   if (r != NULL)  *r = 0;
   /*---(locate cell)--------------------*/
   x_curr  = LOC_cell_at_loc (u, x, y);
   /*---(dispatch)-----------------------*/
   switch (a_type) {
   case YMAP_WIDTH    : case YMAP_WEXACT   :
      DEBUG_HIST  yLOG_note    ("width handler");
      if (y == 0) {
         DEBUG_HIST  yLOG_note    ("topmost, do important stuff");
         x_prev = COL_size (u, x);
         if (a_type == YMAP_WIDTH)   NODE__resize (u, IS_COL, x, 0, a_abbr, 0);
         else                        NODE__resize (u, IS_COL, x, a_abbr, 0, 0);
         x_new  = COL_size (u, x);
         if (r != NULL)  *r = x_new;
      } else if (x_curr != NULL) {
         DEBUG_HIST  yLOG_note    ("other, just update printable");
         api_ycalc_printer (x_curr);
         if (r != NULL)  *r = x_new;
      }
      break;
   case YMAP_HEIGHT   : case YMAP_HEXACT   :
      DEBUG_HIST  yLOG_note    ("height handler");
      if (x == 0) {
         DEBUG_HIST  yLOG_note    ("leftmost, do important stuff");
         x_prev = ROW_size (u, y);
         if (a_type == YMAP_HEIGHT)  NODE__resize (u, IS_ROW, y, 0, a_abbr, 0);
         else                        NODE__resize (u, IS_ROW, y, a_abbr, 0, 0);
         x_new  = ROW_size (u, y);
         if (r != NULL)  *r = x_new;
      } else {
         DEBUG_HIST  yLOG_note    ("other, just update printable");
         api_ycalc_printer (x_curr);
         if (r != NULL)  *r = x_new;
      }
      break;
   case YMAP_ALIGN    :
      DEBUG_HIST  yLOG_note    ("alignment handler");
      x_prev = CELL_align    (x_curr, a_abbr);
      break;
   case YMAP_FORMAT   :
      DEBUG_HIST  yLOG_note    ("format handler");
      x_prev = CELL_format   (x_curr, a_abbr);
      break;
   case YMAP_DECIMALS :
      DEBUG_HIST  yLOG_note    ("decimals handler");
      x_prev = CELL_decimals (x_curr, a_abbr);
      break;
   case YMAP_UNITS    :
      DEBUG_HIST  yLOG_note    ("units handler");
      x_prev = CELL_units    (x_curr, a_abbr);
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return x_prev;
}


