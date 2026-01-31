/*============================----beg-of-source---============================*/
#include   "gyges.h"



/*===[[ GNU GENERAL PUBLIC LICENSE (GPL) ]]===================================*/
/*´´·········1·········2·········3·········4·········5·········6·········7·········8  */

#define  P_COPYRIGHT   \
   "copyright (c) 2010 robert.s.heatherly at balsashrike at gmail dot com"

#define  P_LICENSE     \
   "the only place you could have gotten this code is my github, my website,¦"   \
   "or illegal sharing. given that, you should be aware that this is GPL licensed."

#define  P_COPYLEFT    \
   "the GPL COPYLEFT REQUIREMENT means any modifications or derivative works¦"   \
   "must be released under the same GPL license, i.e, must be free and open."

#define  P_INCLUDE     \
   "the GPL DOCUMENTATION REQUIREMENT means that you must include the original¦" \
   "copyright notice and the full licence text with any resulting anything."

#define  P_AS_IS       \
   "the GPL NO WARRANTY CLAUSE means the software is provided without any¦"      \
   "warranty and the author cannot be held liable for damages."

#define  P_THEFT    \
   "if you knowingly violate the spirit of these ideas, i suspect you might "    \
   "find any number of freedom-minded hackers may take it quite personally ;)"

/*´´·········1·········2·········3·········4·········5·········6·········7·········8  */
/*===[[ GNU GENERAL PUBLIC LICENSE (GPL) ]]===================================*/


static long s_stamp = 0;


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
   DEBUG_YMAP   yLOG_enter   (__FUNCTION__);
   /*---(defaults)-----------------------*/
   if (u != NULL)  *u = 0;
   if (x != NULL)  *x = 0;
   if (y != NULL)  *y = 0;
   if (z != NULL)  *z = 0;
   /*---(defense)------------------------*/
   DEBUG_YMAP   yLOG_point   ("a_label"   , a_label);
   --rce;  if (a_label == NULL) {
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(legality)-----------------------*/
   rc = ystr2gyges (a_label, &x_buf, &x_col, &x_row, NULL, NULL, 0, YSTR_USABLE);
   DEBUG_YMAP   yLOG_value   ("ystr2gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_YMAP   yLOG_note    ("address not legal");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (a_strict != 'y') {
      if (u != NULL)  *u = x_buf;
      if (x != NULL)  *x = x_col;
      if (y != NULL)  *y = x_row;
      DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(existance)----------------------*/
   --rce;  if (!TAB_live (x_buf)) {
      DEBUG_YMAP   yLOG_note    ("universe/tab not available");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(availability)-------------------*/
   rc = TAB_size_detail (x_buf, &x_size, &y_size);
   --rce;  if (rc < 0) {
      DEBUG_YMAP   yLOG_note    ("can not get universe/tab sizes");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMAP   yLOG_value   ("x_size"    , x_size);
   --rce;  if (x_size <  x_col) {
      DEBUG_YMAP   yLOG_note    ("column not available");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMAP   yLOG_value   ("y_size"    , y_size);
   --rce;  if (y_size <  x_row) {
      DEBUG_YMAP   yLOG_note    ("row not available");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save-back)----------------------*/
   if (u != NULL)  *u = x_buf;
   if (x != NULL)  *x = x_col;
   if (y != NULL)  *y = x_row;
   /*---(complete)-----------------------*/
   DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
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
   DEBUG_YMAP   yLOG_enter   (__FUNCTION__);
   /*---(defaults)-----------------------*/
   if (a_label != NULL)  ystrlcpy (a_label, "", LEN_LABEL);
   /*---(legality)-----------------------*/
   rc = ystr4gyges (u, x, y, z, 0, x_label, YSTR_USABLE);
   DEBUG_YMAP   yLOG_value   ("ystr4gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_YMAP   yLOG_note    ("address not legal");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (a_strict != 'y') {
      if (a_label != NULL)  ystrlcpy (a_label, x_label, LEN_LABEL);
      DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(existance)----------------------*/
   --rce;  if (!TAB_live (u)) {
      DEBUG_YMAP   yLOG_note    ("universe/tab not available");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(availability)-------------------*/
   rc = TAB_size_detail (u, &x_size, &y_size);
   --rce;  if (rc < 0) {
      DEBUG_YMAP   yLOG_note    ("can not get universe/tab sizes");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMAP   yLOG_value   ("x_size"    , x_size);
   --rce;  if (x_size < x) {
      DEBUG_YMAP   yLOG_note    ("column not available");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMAP   yLOG_value   ("y_size"    , y_size);
   --rce;  if (y_size < y) {
      DEBUG_YMAP   yLOG_note    ("row not available");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save-back)----------------------*/
   if (a_label != NULL)  ystrlcpy (a_label, x_label, LEN_LABEL);
   /*---(complete)-----------------------*/
   DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
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
   DEBUG_YMAP    yLOG_senter  (__FUNCTION__);
   DEBUG_YMAP    yLOG_schar   (a_axis);
   --rce;  switch (a_axis) {
   case YMAP_UNIV  :
      DEBUG_YMAP    yLOG_snote   ("univ");
      DEBUG_YMAP    yLOG_sint    (NTAB);
      if (n != NULL)  *n = NTAB;
      if (a != NULL)  *a = 0;
      if (b != NULL)  *b = 0;
      if (c != NULL)  *c = CTAB;
      if (e != NULL)  *e = 0;
      if (m != NULL)  *m = 0;
      if (x != NULL)  *x = 0;
      break;
   case YMAP_XAXIS :
      DEBUG_YMAP    yLOG_snote   ("cols");
      DEBUG_YMAP    yLOG_sint    (NCOL);
      if (n != NULL)  *n = NCOL;
      if (a != NULL)  yVIEW_size (YVIEW_MAIN, NULL, NULL, a, NULL, NULL);
      if (b != NULL)  *b = BCOL;
      if (c != NULL)  *c = CCOL;
      if (e != NULL)  *e = ECOL;
      if (m != NULL)  *m = COL_min_used (CTAB);
      if (x != NULL)  *x = COL_max_used (CTAB);
      break;
   case YMAP_YAXIS :
      DEBUG_YMAP    yLOG_snote   ("rows");
      DEBUG_YMAP    yLOG_sint    (NROW);
      if (n != NULL)  *n = NROW;
      if (a != NULL)  yVIEW_size (YVIEW_MAIN, NULL, NULL, NULL, NULL, a);
      if (b != NULL)  *b = BROW;
      if (c != NULL)  *c = CROW;
      if (e != NULL)  *e = EROW;
      if (m != NULL)  *m = ROW_min_used (CTAB);
      if (x != NULL)  *x = ROW_max_used (CTAB);
      break;
   case YMAP_ZAXIS :
      DEBUG_YMAP    yLOG_snote   ("depth NOT supported in gyges");
      DEBUG_YMAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   case YMAP_WHEN  :
      DEBUG_YMAP    yLOG_snote   ("when NOT supported in gyges");
      DEBUG_YMAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   default :
      DEBUG_YMAP    yLOG_snote   ("axis not understood");
      DEBUG_YMAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      break;
   }
   DEBUG_YMAP    yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
api_ymap_entry          (char a_axis, ushort a_pos, short *r_ref, uchar *r_wide, uchar *r_used)
{
   char        rce         =  -10;
   char        rc          =    0;
   DEBUG_YMAP    yLOG_enter   (__FUNCTION__);
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
      DEBUG_YMAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
      break;
   }
   DEBUG_YMAP    yLOG_value   ("rc"        , rc);
   DEBUG_YMAP    yLOG_exit    (__FUNCTION__);
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
   DEBUG_YMAP    yLOG_senter  (__FUNCTION__);
   DEBUG_YMAP    yLOG_schar   (a_axis);
   DEBUG_YMAP    yLOG_sint    (b);
   DEBUG_YMAP    yLOG_sint    (c);
   DEBUG_YMAP    yLOG_sint    (e);
   --rce;  switch (a_axis) {
   case YMAP_UNIV  :
      DEBUG_YMAP    yLOG_snote   ("univ NOT ALLOWED");
      DEBUG_YMAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      break;
   case YMAP_XAXIS :
      DEBUG_YMAP    yLOG_snote   ("cols");
      PTAB->bcol = BCOL = b;
      PTAB->ccol = CCOL = c;
      PTAB->ecol = ECOL = e;
      break;
   case YMAP_YAXIS :
      DEBUG_YMAP    yLOG_snote   ("rows");
      PTAB->brow = BROW = b;
      PTAB->crow = CROW = c;
      PTAB->erow = EROW = e;
      break;
   case YMAP_ZAXIS :
      DEBUG_YMAP    yLOG_snote   ("depth NOT supported in gyges");
      DEBUG_YMAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   case YMAP_WHEN  :
      DEBUG_YMAP    yLOG_snote   ("when NOT supported in gyges");
      DEBUG_YMAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   default :
      DEBUG_YMAP    yLOG_snote   ("axis not understood");
      DEBUG_YMAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      break;
   }
   DEBUG_YMAP    yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
api_ymap_done           (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tCELL      *x_curr      = NULL;
   char        t           [LEN_LABEL] = "";
   char        x_mute      =    0;
   /*---(header)-------------------------*/
   DEBUG_YMAP    yLOG_enter   (__FUNCTION__);
   /*---(update current)-------*/
   x_mute = yLOGS_mute_check ();
   yLOGS_mute ();
   ystr4gyges (CTAB, CCOL, CROW, 0, 0, t, YSTR_CHECK);
   DEBUG_YMAP   yLOG_value   ("NCEL (bef)", NCEL);
   x_curr      = LOC_cell_at_loc (37, 5, 1);
   if (x_curr != NULL)  CELL_change (NULL, YMAP_NONE , 37, 5, 1, t);
   DEBUG_YMAP   yLOG_value   ("NCEL (bef)", NCEL);
   if (x_mute)  yLOGS_unmute ();
   /*---(handle by mode)-----------------*/
   switch (yMODE_curr ()) {
   case MODE_MAP    :
      DEBUG_YMAP    yLOG_note    ("primarily about map mode");
      break;
   case UMOD_WANDER :
      DEBUG_YMAP    yLOG_note    ("not ready for wander yet ;)");
      DEBUG_YMAP    yLOG_exit    (__FUNCTION__);
      return 0;
      break;
   default :
      DEBUG_YMAP    yLOG_exit    (__FUNCTION__);
      return 0;
      break;
   }
   /*---(get current cell)---------------*/
   DEBUG_YMAP    yLOG_complex ("current"   , "%4dt, %4dc, %4dr", CTAB, CCOL, CROW);
   x_curr = LOC_cell_at_curr ();
   DEBUG_YMAP    yLOG_point   ("x_curr"    , x_curr);
   /*---(no current content)-------------*/
   if (x_curr == NULL || x_curr->d_source == NULL) {
      ystr4gyges (CTAB, CCOL, CROW, 0, 0, t, YSTR_CHECK);
      ySRC_update (t, "·····", "");
      ystrlcpy (my.reqs_list, "n/a", LEN_RECD);
      ystrlcpy (my.deps_list, "n/a", LEN_RECD);
      ystrlcpy (my.like_list, "n/a", LEN_RECD);
      ystrlcpy (my.copy_list, "n/a", LEN_RECD);
      ystrlcpy (my.rpn_list , "n/a", LEN_RECD);
      ystrlcpy (my.pros_plus, "n/a", LEN_RECD);
      ystrlcpy (my.reqs_plus, "n/a", LEN_RECD);
      ystrlcpy (g_curr.label, "-"          , LEN_LABEL);
      g_curr.len    = 0;
      g_curr.type   = YCALC_DATA_BLANK;
      g_curr.tab    = -1;
      g_curr.col    = -1;
      g_curr.row    = -1;
      g_curr.align  = '-';
      g_curr.format = '-';
      g_curr.decs   = '-';
      g_curr.unit   = '-';
      DEBUG_YMAP    yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(active cell)--------------------*/
   sprintf (t, "%c%c%c%c%c", x_curr->d_type, x_curr->d_align, x_curr->d_format, x_curr->d_decs, x_curr->d_unit);
   ySRC_update (x_curr->d_label, t, x_curr->d_source);
   yCALC_disp_reqs (x_curr->d_ycalc, my.reqs_list);
   yCALC_disp_pros (x_curr->d_ycalc, my.deps_list);
   yCALC_disp_like (x_curr->d_ycalc, my.like_list);
   yCALC_disp_copy (x_curr->d_ycalc, my.copy_list);
   yCALC_disp_prosplus (x_curr->d_ycalc, my.pros_plus);
   yCALC_disp_reqsplus (x_curr->d_ycalc, my.reqs_plus);
   ystrlcpy (g_curr.label, x_curr->d_label, LEN_LABEL);
   g_curr.len    = x_curr->d_len;
   g_curr.type   = x_curr->d_type;
   g_curr.tab    = x_curr->d_tab;
   g_curr.col    = x_curr->d_col;
   g_curr.row    = x_curr->d_row;
   g_curr.align  = x_curr->d_align;
   g_curr.format = x_curr->d_format;
   g_curr.decs   = x_curr->d_decs;
   g_curr.unit   = x_curr->d_unit;
   /*---(complete)-----------------------*/
   DEBUG_YMAP    yLOG_exit    (__FUNCTION__);
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
   DEBUG_YMAP  yLOG_enter   (__FUNCTION__);
   /*---(get location)-------------------*/
   rc = ystr2gyges (a_label, &u, &x, &y, NULL, NULL, 0, YSTR_USABLE);
   DEBUG_YMAP   yLOG_complex ("original"  , "label=%s, tab=%4d, col=%4d, row=%4d", a_label, u, x, y);
   x_curr = LOC_cell_at_loc (u, x, y);
   DEBUG_YMAP   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr == NULL) {
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (x_curr->d_type == YCALC_DATA_ERROR) {
      DEBUG_YMAP   yLOG_note    ("error cell, just a re-touch");
      CELL_change (NULL, YMAP_NONE, x_curr->d_tab, x_curr->d_col, x_curr->d_row, strdup (x_curr->d_source));
   } else {
      DEBUG_YMAP   yLOG_note    ("no trouble, nothing to do");
   }
   DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
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
   DEBUG_YMAP  yLOG_enter   (__FUNCTION__);
   DEBUG_YMAP  yLOG_info    ("a_format"  , a_format);
   DEBUG_YMAP  yLOG_info    ("a_content" , a_content);
   /*---(identify location)--------------*/
   DEBUG_YMAP  yLOG_info    ("label"     , a_label);
   if (a_label != NULL && strcmp (a_label, "n/a") != 0) {
      rc = ystr2gyges (a_label, &u, &x, &y, NULL, NULL, 0, YSTR_ADAPT);
      DEBUG_YMAP  yLOG_value   ("ystr2gyges"   , rc);
      --rce;  if (rc < 0) {
         DEBUG_YMAP  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_YMAP  yLOG_complex ("loc"       , "%4du, %4dx, %4dy", u, x, y);
      x_curr  = LOC_cell_at_loc (u, x, y);
   }
   /*---(handle request)-----------------*/
   DEBUG_YMAP  yLOG_char    ("act"       , a_act);
   switch (a_act) {
   case YMAP_OVERWRITE :
   case YMAP_CLEAR     :
   case YMAP_DELETE    :
      ystrldchg (a_content, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
      CELL_overwrite (YMAP_NONE, u, x, y, a_content, a_format);
      break;
   case YMAP_SOURCE    :
      ystrldchg (a_content, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
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
         DEBUG_YMAP  yLOG_note    ("running a sync with undo");
         ystrlcpy  (x_source, a_content, LEN_RECD);
         DEBUG_YMAP  yLOG_info    ("x_source"   , x_source);
         p = strtok_r (x_source, ",", &r);
         while (p != NULL) {
            rc = api_ymap__touch (p);
            p = strtok_r (NULL  , ",", &r);
         }
      } else {
         DEBUG_YMAP  yLOG_note    ("ignore sync with redo");
      }
      break;
   case YMAP_RECALC   : case 'ø' :
      if (a_dir == '>') {
         DEBUG_YMAP  yLOG_note    ("running recalc error cell with redo");
         ystrlcpy  (x_source, a_content, LEN_RECD);
         DEBUG_YMAP  yLOG_info    ("x_source"   , x_source);
         p = strtok_r (x_source, ",", &r);
         while (p != NULL) {
            rc = api_ymap__touch (p);
            p = strtok_r (NULL  , ",", &r);
         }
      } else {
         DEBUG_YMAP  yLOG_note    ("ignore recalc error cell with undo");
      }
      break;
   case YMAP_POSITION  :
      rc = yMAP_jump (u, x, y, 0);
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_YMAP  yLOG_exit    (__FUNCTION__);
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
   DEBUG_YMAP  yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   if (r != NULL)  *r = 0;
   /*---(locate cell)--------------------*/
   x_curr  = LOC_cell_at_loc (u, x, y);
   /*---(dispatch)-----------------------*/
   switch (a_type) {
   case YMAP_WIDTH    : case YMAP_WEXACT   :
      DEBUG_YMAP  yLOG_note    ("width handler");
      if (y == 0) {
         DEBUG_YMAP  yLOG_note    ("topmost, do important stuff");
         x_prev = COL_size (u, x);
         if (a_type == YMAP_WIDTH)   NODE__resize (u, IS_COL, x, 0, a_abbr, 0);
         else                        NODE__resize (u, IS_COL, x, a_abbr, 0, 0);
         x_new  = COL_size (u, x);
         if (r != NULL)  *r = x_new;
      } else if (x_curr != NULL) {
         DEBUG_YMAP  yLOG_note    ("other, just update printable");
         api_ycalc_printer (x_curr);
         if (r != NULL)  *r = x_new;
      }
      break;
   case YMAP_HEIGHT   : case YMAP_HEXACT   :
      DEBUG_YMAP  yLOG_note    ("height handler");
      if (x == 0) {
         DEBUG_YMAP  yLOG_note    ("leftmost, do important stuff");
         x_prev = ROW_size (u, y);
         if (a_type == YMAP_HEIGHT)  NODE__resize (u, IS_ROW, y, 0, a_abbr, 0);
         else                        NODE__resize (u, IS_ROW, y, a_abbr, 0, 0);
         x_new  = ROW_size (u, y);
         if (r != NULL)  *r = x_new;
      } else {
         DEBUG_YMAP  yLOG_note    ("other, just update printable");
         api_ycalc_printer (x_curr);
         if (r != NULL)  *r = x_new;
      }
      break;
   case YMAP_ALIGN    :
      DEBUG_YMAP  yLOG_note    ("alignment handler");
      x_prev = CELL_align    (x_curr, a_abbr);
      break;
   case YMAP_FORMAT   :
      DEBUG_YMAP  yLOG_note    ("format handler");
      x_prev = CELL_format   (x_curr, a_abbr);
      break;
   case YMAP_DECIMALS :
      DEBUG_YMAP  yLOG_note    ("decimals handler");
      x_prev = CELL_decimals (x_curr, a_abbr);
      break;
   case YMAP_UNITS    :
      DEBUG_YMAP  yLOG_note    ("units handler");
      x_prev = CELL_units    (x_curr, a_abbr);
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_YMAP  yLOG_exit    (__FUNCTION__);
   return x_prev;
}

char
api_ymap_range          (char *a_beg, char *a_end)
{

   char        rce         =  -10;
   char        rc          =    0;
   ushort      bu, bx, by, bz;
   ushort      eu, ex, ey, ez;
   rc = api_ymap_locator        ('y', a_beg, &bu, &bx, &by, &bz);
   --rce;  if (rc < 0)  return rce;
   rc = api_ymap_locator        ('y', a_end, &eu, &ex, &ey, &ez);
   --rce;  if (rc < 0)  return rce;
   --rce;  if (bu != eu)  return rce;
   rc = yMAP_visu_exact (bu, bx, ex, by, ey, 0, 0, 'e');
   --rce;  if (rc < 0)  return rce;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     map mode registers                       ----===*/
/*====================------------------------------------====================*/
static void  o___REGISTERS_______o () { return; }

static char s_1st  = '-';

char
api__ymap_clear_done    (void)
{
}

char         /*-> tail recursion function for copy ---[ ------ [ge.D55.237.63]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
api__ymap_clearer_one   (tCELL *a_curr, long a_stamp)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   ushort      u, xb, xe, yb, ye;
   /*---(header)-------------------------*/
   DEBUG_YMAP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YMAP   yLOG_point   ("a_curr"    , a_curr);
   --rce;  if (a_curr    == NULL) {
      DEBUG_YMAP   yLOG_note    ("original cell null");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* no cell                       */
   }
   DEBUG_YMAP   yLOG_complex ("DEBUG_YMAP 2"   , "%-10.10s, %2dt, %3dc, %4dr", a_curr->d_label, a_curr->d_tab, a_curr->d_col, a_curr->d_row);
   DEBUG_YMAP   yLOG_point   ("s"         , a_curr->d_source);
   --rce;  if (a_curr->d_source == NULL) {
      DEBUG_YMAP   yLOG_note    ("no source");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* nothing to write              */
   }
   DEBUG_YMAP   yLOG_info    ("s"         , a_curr->d_source);
   DEBUG_YMAP   yLOG_char    ("t"         , a_curr->d_type);
   --rce;  if (a_curr->d_type == '-')  {
      DEBUG_YMAP   yLOG_note    ("could not copy an empty");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* don't write, recreate on read */
   }
   /*---(check for bounds)---------------*/
   yMAP_visu_range (&u, &xb, &xe, &yb, &ye, NULL, NULL, NULL);
   DEBUG_YMAP   yLOG_complex ("visual"    , "%2dt, %3d to %3dc, %4d to %4dr", u, xb, xe, yb, ye);
   rc = yMAP_inside (a_curr->d_tab, a_curr->d_col, a_curr->d_row, 0);
   DEBUG_YMAP   yLOG_value   ("visu_rc"   , rc);
   --rce;  if (rc <= 0)  {
      DEBUG_YMAP   yLOG_note    ("REJECTED, cell not in visual area");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(add to placeholder reg)---------*/
   rc = yMAP_mreg_add  (&denada, a_curr->d_label, "", "");
   /*---(delete)-------------------------*/
   /*> if (s_1st == 'y')  rc = CELL_change (NULL, YMAP_BEG, a_curr->tab, a_curr->col, a_curr->row, "");   <* 
    *> else               rc = CELL_change (NULL, YMAP_ADD, a_curr->tab, a_curr->col, a_curr->row, "");   <* 
    *> DEBUG_YMAP   yLOG_value   ("rc"        , rc);                                                      <* 
    *> --rce;  if (rc < 0) {                                                                              <* 
    *>    DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);                                                  <* 
    *>    return rce;                                                                                     <* 
    *> }                                                                                                  <*/
   /*---(complete)-----------------------*/
   DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_ymap_clearer_seq    (void *a_owner, void *a_deproot, int a_seq, int a_level)
{
   return  api__ymap_clearer_one ((tCELL *) a_owner, s_stamp);
}

char
api_ymap_clearer_NEW    (char a_1st, char a_type, long a_stamp)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   ushort      u           =    0;
   ushort      x           =    0;
   ushort      y           =    0;
   tCELL      *x_curr      = NULL;
   int         x_cmp       =    0;
   char        x_abbr      =  '·';
   int         c           =    0;
   char        x_list      [LEN_HUGE]  = "";
   char       *p           = NULL;
   char       *r           = NULL;
   int         n           =    0;
   /*---(header)-------------------------*/
   DEBUG_YMAP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   DEBUG_YMAP   yLOG_value   ("a_stamp"   , a_stamp);
   s_stamp = a_stamp;
   s_1st   = a_1st;
   /*---(dependents)---------------------*/
   DEBUG_YMAP   yLOG_note    ("DEPENDENT CELLS (GATHERING)");
   rc      = yCALC_seq_downup   (s_stamp, api_ymap_clearer_seq);
   /*> yCALC_seq_list (x_list);                                                       <* 
    *> DEBUG_YMAP   yLOG_info    ("SEQ LIST"  , x_list);                              <* 
    *> yCALC_seq_dump (YSTR_CLIP);                                                    <*/
   /*---(clear them)---------------------*/
   DEBUG_YMAP   yLOG_note    ("DEPENDENT CELLS (DELETING)");
   rc = yMAP_mreg_list (&x_abbr, &c, &x_list);
   DEBUG_YMAP   yLOG_char    ("x_abbr"    , x_abbr);
   DEBUG_YMAP   yLOG_value   ("c"         , c);
   DEBUG_YMAP   yLOG_info    ("x_list"    , x_list);
   p = strtok_r (x_list, ",", &r);
   while (p != NULL) {
      x_curr  = LOC_cell_labeled (p);
      ++n;
      DEBUG_YMAP   yLOG_complex ("x_curr"    , "%4d, %-10.10s, %p", n, p, x_curr);
      if (x_curr != NULL) {
         DEBUG_YMAP   yLOG_complex ("coords"    , "%2du, %3dx, %4dy", x_curr->d_tab, x_curr->d_col, x_curr->d_row);
         if (s_1st == 'y')  rc = CELL_change (NULL, YMAP_BEG, x_curr->d_tab, x_curr->d_col, x_curr->d_row, "");
         else               rc = CELL_change (NULL, YMAP_ADD, x_curr->d_tab, x_curr->d_col, x_curr->d_row, "");
         DEBUG_YMAP   yLOG_value   ("rc"        , rc);
         --rce;  if (rc < 0) {
            DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         if (rc >=  0)  s_1st = '-';
      }
      p = strtok_r (NULL  , ",", &r);
   }
   /*---(independents)-------------------*/
   DEBUG_YMAP   yLOG_note    ("INDEPENDENT CELLS");
   rc      = yMAP_visu_first (&u, &x, &y, NULL);
   DEBUG_YMAP   yLOG_value   ("first"     , rc);
   while (rc >= 0) {
      x_curr  = LOC_cell_at_loc (u, x, y);
      if (x_curr != NULL) {
         x_cmp = yCALC_stamp_cmp (x_curr->d_ycalc, s_stamp);
         DEBUG_YMAP   yLOG_complex ("x_curr"    , "%-10.10s, %2du, %3dx, %4dy, %d, %p", x_curr->d_label, u, x, y, x_cmp, x_curr);
         if (!x_cmp) {
            DEBUG_YMAP   yLOG_note    ("call clear");
            if (s_1st == 'y')  rc = CELL_change (NULL, YMAP_BEG, x_curr->d_tab, x_curr->d_col, x_curr->d_row, "");
            else               rc = CELL_change (NULL, YMAP_ADD, x_curr->d_tab, x_curr->d_col, x_curr->d_row, "");
            s_1st = '-';
         } else {
            DEBUG_YMAP   yLOG_note    ("stamp matches, rejected");
         }
      } else {
         DEBUG_YMAP   yLOG_complex ("x_curr"    , "%-10.10s, %2du, %3dx, %4dy, %d", "······", u, x, y, 0, x_curr);
      }
      rc      = yMAP_visu_next  (&u, &x, &y, NULL);
      DEBUG_YMAP   yLOG_value   ("next"      , rc);
   }
   /*---(complete)-----------------------*/
   DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_ymap_clearer        (char a_1st, ushort u, ushort x, ushort y, ushort z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   static int  x_count     =    0;
   /*---(header)-------------------------*/
   DEBUG_YMAP   yLOG_enter   (__FUNCTION__);
   /*---(check first)--------------------*/
   DEBUG_YMAP   yLOG_char    ("a_1st"     , a_1st);
   if (a_1st == 'y')  x_count = 0;
   /*---(identify cell)------------------*/
   DEBUG_YMAP   yLOG_complex ("coords"    , "%3db, %3dx, %3dy", u, x, y);
   x_curr = LOC_cell_at_loc (u, x, y);
   DEBUG_YMAP   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr == NULL) {
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(delete)-------------------------*/
   DEBUG_YMAP   yLOG_value   ("x_count"   , x_count);
   if (a_1st == 'y')  rc = CELL_change (NULL, YMAP_BEG, u, x, y, "");
   else               rc = CELL_change (NULL, YMAP_ADD, u, x, y, "");
   DEBUG_YMAP   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update count)-------------------*/
   ++x_count;
   DEBUG_YMAP   yLOG_value   ("x_count"   , x_count);
   /*---(complete)-----------------------*/
   DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tail recursion function for copy ---[ ------ [ge.D55.237.63]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
api__ymap_copier_one    (tCELL *a_curr, long a_stamp)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   tCELL      *x_copy      = NULL;
   ushort      u, xb, xe, yb, ye;
   int         c           = 0;
   char        x_reqs      [LEN_RECD];
   char        x_pros      [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_YMAP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YMAP   yLOG_point   ("a_curr"    , a_curr);
   --rce;  if (a_curr    == NULL) {
      DEBUG_YMAP   yLOG_note    ("original cell null");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* no cell                       */
   }
   DEBUG_YMAP   yLOG_complex ("DEBUG_YMAP 2"   , "%-10.10s, %2dt, %3dc, %4dr", a_curr->d_label, a_curr->d_tab, a_curr->d_col, a_curr->d_row);
   DEBUG_YMAP   yLOG_point   ("s"         , a_curr->d_source);
   --rce;  if (a_curr->d_source == NULL) {
      DEBUG_YMAP   yLOG_note    ("no source");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* nothing to write              */
   }
   DEBUG_YMAP   yLOG_info    ("s"         , a_curr->d_source);
   DEBUG_YMAP   yLOG_char    ("t"         , a_curr->d_type);
   --rce;  if (a_curr->d_type == '-')  {
      DEBUG_YMAP   yLOG_note    ("could not copy an empty");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* don't write, recreate on read */
   }
   /*---(check for bounds)---------------*/
   yMAP_visu_range (&u, &xb, &xe, &yb, &ye, NULL, NULL, NULL);
   DEBUG_YMAP   yLOG_complex ("visual"    , "%2dt, %3d to %3dc, %4d to %4dr", u, xb, xe, yb, ye);
   rc = yMAP_inside (a_curr->d_tab, a_curr->d_col, a_curr->d_row, 0);
   DEBUG_YMAP   yLOG_value   ("visu_rc"   , rc);
   --rce;  if (rc <= 0)  {
      DEBUG_YMAP   yLOG_note    ("REJECTED, cell not in visual area");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(make a copy)--------------------*/
   rc = CELL_dup (&x_copy, a_curr);
   DEBUG_YMAP   yLOG_value   ("dup_rc"    , rc);
   --rce;  if (rc < 0 ) {
      DEBUG_YMAP   yLOG_note    ("could not duplicate");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(move in critical data)----------*/
   x_copy->d_label = strdup (a_curr->d_label);
   yCALC_stamp_set (a_curr->d_ycalc, s_stamp);
   DEBUG_YMAP   yLOG_info    ("label"     , x_copy->d_label);
   /*---(impacts)------------------------*/
   yCALC_show_pros (a_curr->d_ycalc, &c, x_pros);
   yCALC_show_reqs (a_curr->d_ycalc, &c, x_reqs);
   /*---(place in buffer)----------------*/
   rc = yMAP_mreg_add  (x_copy, x_copy->d_label, x_reqs, x_pros);
   DEBUG_YMAP   yLOG_value   ("hook_rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_YMAP   yLOG_note    ("could not hook to register");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_ymap_copier_seq     (void *a_owner, void *a_deproot, int a_seq, int a_level)
{
   return  api__ymap_copier_one ((tCELL *) a_owner, s_stamp);
}

char
api_ymap_copier         (char a_type, long a_stamp)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   ushort      x_tab       =    0;
   ushort      x_col       =    0;
   ushort      x_row       =    0;
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YMAP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   DEBUG_YMAP   yLOG_value   ("a_stamp"   , a_stamp);
   s_stamp = a_stamp;
   /*---(dependents)---------------------*/
   DEBUG_YMAP   yLOG_note    ("DEPENDENT CELLS");
   rc      = yCALC_seq_downdown (s_stamp, api_ymap_copier_seq);
   /*---(independents)-------------------*/
   DEBUG_YMAP   yLOG_note    ("INDEPENDENT CELLS");
   rc      = yMAP_visu_first (&x_tab, &x_col, &x_row, NULL);
   while (rc >= 0) {
      x_curr  = LOC_cell_at_loc (x_tab, x_col, x_row);
      if (x_curr != NULL && !yCALC_stamp_cmp (x_curr->d_ycalc, s_stamp))   api__ymap_copier_one (x_curr, s_stamp);
      rc      = yMAP_visu_next  (&x_tab, &x_col, &x_row, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_ymap_paster         (char a_reqs, char a_1st, short uo, short xo, short yo, short zo, tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   short       x_stab, x_scol, x_srow;
   short       x_dtab, x_dcol, x_drow;
   char        x_source    [LEN_RECD]   = "";
   char        x_bformat   [LEN_LABEL]  = "";
   tCELL      *x_copy      = NULL;
   char        x_label     [LEN_LABEL]  = "";
   char        x_list      [LEN_RECD]   = "";
   char       *p           = NULL;               /* pointer for providers     */
   char       *q           = ",";                /* delimiter for providers   */
   char       *s           = NULL;               /* context for providers     */
   tCELL      *x_provider  = NULL;               /* provider cell to adjust   */
   tCELL      *x_original  = NULL;
   int         x_nreq, x_npro;
   /*---(header)-------------------------*/
   DEBUG_YMAP   yLOG_enter   (__FUNCTION__);
   DEBUG_YMAP   yLOG_char    ("a_reqs"    , a_reqs);
   DEBUG_YMAP   yLOG_char    ("a_1st"     , a_1st);
   DEBUG_YMAP   yLOG_complex ("offset"    , "b=%4d, x=%4d, y=%4d, z=%4d", uo, xo, yo, zo);
   /*---(defense)------------------------*/
   DEBUG_YMAP   yLOG_point   ("a_cell"    , a_cell);
   --rce;  if (a_cell == NULL)  {
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for retouch)--------------*/
   --rce;  if (a_reqs != 0 && strchr ("?!", a_reqs) != NULL) {
      DEBUG_YMAP   yLOG_note    ("call for a retouch run");
      if (a_reqs == '!')  rc = ystr2gyges (a_cell->d_label, &x_stab, &x_scol, &x_srow, NULL, NULL, 0, YSTR_USABLE);
      else                rc = ystr2gyges ((char *) a_cell, &x_stab, &x_scol, &x_srow, NULL, NULL, 0, YSTR_USABLE);
      DEBUG_YMAP   yLOG_complex ("original"  , "tab=%4d, col=%4d, row=%4d", x_stab, x_scol, x_srow);
      x_dtab  = x_stab + uo;
      x_dcol  = x_scol + xo;
      x_drow  = x_srow + yo;
      DEBUG_YMAP   yLOG_complex ("going to"  , "tab=%4d, col=%4d, row=%4d", x_dtab, x_dcol, x_drow);
      x_copy = LOC_cell_at_loc (x_dtab, x_dcol, x_drow);
      DEBUG_YMAP   yLOG_point   ("x_copy"    , x_copy);
      if (x_copy == NULL) {
         DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      if (x_copy->d_type == YCALC_DATA_ERROR) {
         DEBUG_YMAP   yLOG_note    ("error cell, just a re-touch");
         CELL_change (NULL, YMAP_NONE, x_copy->d_tab, x_copy->d_col, x_copy->d_row, strdup (x_copy->d_source));
         yMAP_mundo_recalc (YMAP_ADD, x_copy->d_label);
      } else {
         DEBUG_YMAP   yLOG_note    ("no trouble, nothing to do");
      }
      DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(defense)------------------------*/
   DEBUG_YMAP   yLOG_info    ("G_RPN_REQS", G_RPN_REQS);
   --rce;  if (strchr (G_RPN_REQS, a_reqs) == NULL)  {
      DEBUG_YMAP   yLOG_note    ("a_reqs type not recognized");
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get original location)----------*/
   DEBUG_YMAP   yLOG_info    ("a_label"   , a_cell->d_label);
   rc = ystr2gyges (a_cell->d_label, &x_stab, &x_scol, &x_srow, NULL, NULL, 0, YSTR_USABLE);
   DEBUG_YMAP   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0)  {
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMAP   yLOG_complex ("original"  , "tab=%4d, col=%4d, row=%4d", x_stab, x_scol, x_srow);
   /*---(set new location)---------------*/
   x_dtab  = x_stab + uo;
   x_dcol  = x_scol + xo;
   x_drow  = x_srow + yo;
   DEBUG_YMAP   yLOG_complex ("going to"  , "tab=%4d, col=%4d, row=%4d", x_dtab, x_dcol, x_drow);
   /*---(check cell type)----------------*/
   DEBUG_YMAP   yLOG_info    ("source"    , a_cell->d_source);
   DEBUG_YMAP   yLOG_char    ("type"      , a_cell->d_type);
   strcpy (x_source, "");
   if (strchr (YCALC_GROUP_RPN, a_cell->d_type) != 0) {
      DEBUG_YMAP   yLOG_note    ("formula, calling yRPN_adjust");
      rc = yRPN_addr_require (a_cell->d_source, a_reqs, uo, xo, yo, zo, LEN_RECD, x_source);
      DEBUG_YMAP   yLOG_value   ("rc"        , rc);
   } else {
      DEBUG_YMAP   yLOG_note    ("just copy straight across");
      strcpy (x_source, a_cell->d_source);
   }
   DEBUG_YMAP   yLOG_info    ("x_source"  , x_source);
   sprintf (x_bformat, "%c%c%c%c-", a_cell->d_align, a_cell->d_format, a_cell->d_decs, a_cell->d_unit);
   DEBUG_YMAP   yLOG_info    ("x_bformat" , x_bformat);
   if (a_1st == 'y')  x_copy = CELL_overwrite (YMAP_BEG, x_dtab, x_dcol, x_drow, x_source, x_bformat);
   else               x_copy = CELL_overwrite (YMAP_ADD, x_dtab, x_dcol, x_drow, x_source, x_bformat);
   DEBUG_YMAP   yLOG_complex ("DEBUG_YMAP 3"   , "%-10.10s, %2dt, %3dc, %4dr", x_copy->d_label, x_copy->d_tab, x_copy->d_col, x_copy->d_row);
   /*---(complete)-----------------------*/
   DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_ymap_finisher       (char a_pros, char *a_target, char *a_labels, short uo, short xo, short yo, short zo)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   short       x_stab, x_scol, x_srow;
   short       x_dtab, x_dcol, x_drow;
   tCELL      *x_curr      = NULL;
   char        x_work      [LEN_RECD]  = "";
   char        x_tokens    [LEN_RECD]  = "";  /* source made into tokens   */
   char       *p           = NULL;               /* strtok field pointer      */
   char       *r           = NULL;               /* strtok context            */
   short       u, x, y;
   short       xb, xe, yb, ye;
   char        x_label     [LEN_LABEL] = "";
   char        t           [LEN_LABEL] = "";
   char        x_fixed     [LEN_LABEL] = "";
   char        x_final     [LEN_RECD]  = "";  /* new version of formula    */
   int         x_bad       =    0;
   char        x_pretty    [LEN_RECD]  = "";  /* new version of formula    */
   char        x_force     =  '-';
   /*---(header)-------------------------*/
   DEBUG_YMAP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YMAP   yLOG_char    ("a_pros"    , a_pros);
   DEBUG_YMAP   yLOG_info    ("G_RPN_PROS", G_RPN_PROS);
   --rce;  if (strchr (G_RPN_PROS, a_pros) == NULL)  {
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (a_pros == G_RPN_IGNORE || a_pros == G_RPN_PNONE) {
      DEBUG_YMAP   yLOG_note    ("provider updates not requested");
      DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (strchr ("AS", a_pros) != NULL) {
      DEBUG_YMAP   yLOG_note    ("all and split force all providers to update");
      x_force = 'y';
   } else {
      DEBUG_YMAP   yLOG_note    ("rel and part only update relative providers");
   }
   DEBUG_YMAP   yLOG_point   ("a_target"  , a_target);
   --rce;  if (a_target == NULL) {
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMAP   yLOG_info    ("a_target"  , a_target);
   DEBUG_YMAP   yLOG_point   ("a_labels"  , a_labels);
   --rce;  if (a_labels == NULL) {
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMAP   yLOG_info    ("a_labels"  , a_labels);
   --rce;  if (a_labels [0] != ',') {
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check cell)---------------------*/
   x_curr = LOC_cell_labeled (a_target);
   DEBUG_YMAP   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr == NULL) {
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   yMAP_visu_range (&u, &xb, &xe, &yb, &ye, NULL, NULL, NULL);
   DEBUG_YMAP   yLOG_complex ("visual"    , "%2dt, %3d to %3dc, %4d to %4dr", u, xb, xe, yb, ye);
   rc = yMAP_inside (x_curr->d_tab, x_curr->d_col, x_curr->d_row, 0);
   DEBUG_YMAP   yLOG_value   ("visu_rc"   , rc);
   if (rc >  0)  {
      DEBUG_YMAP   yLOG_note    ("cell inside range, nothing to do");
      DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(get formula)--------------------*/
   DEBUG_YMAP   yLOG_point   ("source"    , x_curr->d_source);
   --rce;  if (x_curr->d_source == NULL) {
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ystrlcpy (x_work, x_curr->d_source, LEN_RECD);
   DEBUG_YMAP   yLOG_point   ("x_work"    , x_work);
   /*---(prepare tokens)-----------------*/
   rc = yRPN_parsed (x_work, &x_tokens, NULL, LEN_RECD);
   DEBUG_YMAP    yLOG_value   ("tokenize"  , rc);
   --rce;  if (rc < 0)  {
      DEBUG_YMAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMAP    yLOG_info    ("x_tokens"  , x_tokens);
   /*---(loop through tokens)------------*/
   p = strtok_r (x_tokens, " ", &r);
   DEBUG_YMAP    yLOG_point   ("p"         , p);
   while (p != NULL) {
      /*---(compare to list)-------------*/
      rc = ystr2gyges (p, &u, &x, &y, NULL, NULL, 0, YSTR_USABLE);
      DEBUG_YMAP    yLOG_value   ("ystr2gyges" , rc);
      /*---(handle non-address)----------*/
      if (rc <  0)  {
         DEBUG_YMAP    yLOG_note    ("not an address, just append");
         ystrlcat (x_final, p      , LEN_RECD);
      }
      /*---(check against pros)----------*/
      else {
         rc = ystr4gyges (u, x, y, 0, 0, x_label, YSTR_USABLE);
         DEBUG_YMAP    yLOG_value   ("ystr4gyges" , rc);
         sprintf (t, ",%s,", x_label);
         DEBUG_YMAP    yLOG_info    ("t"         , t);
         /*---(not applicable)-----------*/
         if (strstr (a_labels, t) == NULL)  {
            DEBUG_YMAP    yLOG_note    ("not on provider list, just append");
            ystrlcat (x_final, p      , LEN_RECD);
         }
         /*---(update provider)----------*/
         else {
            rc = ystr8gyges (p, uo, xo, yo, zo, x_force, x_fixed, YSTR_LEGAL);
            DEBUG_YMAP    yLOG_value   ("ystr8gyges" , rc);
            ystrlcat (x_final, x_fixed, LEN_RECD);
         }
      }
      ystrlcat (x_final, " ", LEN_RECD);
      /*---(display)---------------------*/
      DEBUG_YMAP    yLOG_info    ("x_final"   , x_final);
      if (strcmp (x_fixed, "#REF") == 0)  ++x_bad;
      /*---(next)------------------------*/
      p = strtok_r (NULL    , " ", &r);
      DEBUG_YMAP    yLOG_point   ("p"         , p);
      /*---(done)------------------------*/
   }
   /*---(strip final space)--------------*/
   DEBUG_YMAP    yLOG_value   ("x_bad"     , x_bad);
   x_final [strlen (x_final) - 1] = '\0';
   /*---(clean and save)-----------------*/
   DEBUG_YMAP    yLOG_info    ("final"     , x_final);
   rc = yRPN_pretty (x_final, &x_pretty, NULL, LEN_RECD);
   DEBUG_YMAP    yLOG_value   ("pretty"    , rc);
   rc = CELL_change  (NULL, YMAP_ADD, x_curr->d_tab, x_curr->d_col, x_curr->d_row, x_pretty);
   DEBUG_YMAP    yLOG_value   ("change"    , rc);
   --rce; if (x_bad > 0) {
      DEBUG_YMAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> delete a register cell -------------[ ------ [gz.210.101.01]*/ /*-[00.0000.104.!]-*/ /*-[--.---.---.--]-*/
api_ymap_regkiller      (tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YMAP   yLOG_enter   (__FUNCTION__);
   DEBUG_YMAP   yLOG_point   ("a_cell"    , a_cell);
   /*---(kill)---------------------------*/
   rc = CELL__valid   (a_cell, LINKED);
   DEBUG_YMAP   yLOG_value   ("valid"     , rc);
   if (rc < 0) {
      DEBUG_YMAP   yLOG_note    ("cell no longer exists");
      DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
      return 1;
   }
   rc = CELL__free (&a_cell);
   DEBUG_YMAP   yLOG_value   ("free"      , rc);
   /*---(complete)-----------------------*/
   DEBUG_YMAP   yLOG_exit    (__FUNCTION__);
   return rc;
}

