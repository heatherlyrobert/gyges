/*============================----beg-of-source---============================*/
#include   "gyges.h"

/*
 * metis § wn2#· § col width changes not updating effected merged cell printables         § M2O126 §  1 §
 * metis § dn4#· § cut and paste is disabled                                              § M2O1DD §  2 §
 * metis § yv+·· § be able to delete cell and remove from all formulas that require it    § M2PMDa §  · §
 * metis § yv+·· § add move option that copies, pastes, then clears original              § M2PNVU §  · §
 *
 * metis § dn2·· § interactive cut and paste does not update formulas (#REF)              § M2R63C §  · §
 *
 *
 */

/*
 *  complex copy/paste scenario...
 *  -- INNER has all four absolute/relative, provides/requires combos
 *  -- OUTER has all four absolute/relative, provides/requires combos
 *  -- first, INNER is copied
 *  -- then, pasted on top of SQUASH area
 *  -- INNER connections must apporpriately stay intack
 *  -- SQUASH areas not overwritten must still work (0g8 Ö 0d7 Ö 0a6)
 *  -- all OUTER connections must be appropriately updated
 *
 *
 *   legend   Ï source    › destination
 *
 *  ORIGINAL ====================================================
 *
 *     ---a--- ---b--- ---c--- ---d--- ---e--- ---f--- ---g---
 *
 *  1                  abs req ›€€‚                 OUTER
 *                  ƒ²²²²²²²²²²²²²²²²²²²²²INNER·‚
 *  2               Œ  rel proÏ€‚ ƒ€›rel req   Œ
 *               ƒ€€Œ€€€€€€€€€€Ï› ÏÏ            Œ
 *  3  rel req ›€…  Œ          crucial ›€€€€€€€€Œ€€€€€ rel pro
 *                 ƒŒ€€€€€€€€€€››  Ï ›€€€€€€€€€€Œ€‚
 *  4              Œ  abs proÏ€…  „€›abs req   Œ 
 *                 ‡²²²²²²²²²²²²²²²²²²²²²²²²²²²†  
 *  5              Œ···························Œ „€€Ï abs pro
 *                 Œ·················SQUASH····Œ
 *  6  rel req ›€‚ Œ···························Œ
 *                Œ···························Œ
 *  7             Œ···········safe············Œ
 *                „²²²²²²²²²²²ÏÏ²›²²²²²²²²²²²²…
 *  8             „€€€€€€€€€€€€…               ƒ€€Ï rel pro
 *               „€€€€€€€€€€€€€€€… „€€€€€€€€€€€€€€…
 *  9
 *
 * 10        
 *
 *
 *
 *  COPY/NORMAL (confirmed) =====================================
 *
 *     ---a--- ---b--- ---c--- ---d--- ---e--- ---f--- ---g---
 *
 *  1                  abs req ›€€‚                 OUTER
 *                  ƒ²²²²²²²²²²²²²²²²²²²²²INNER·‚
 *  2               Œ  rel proÏ€‚ ƒ€›rel req   Œ
 *               ƒ€€Œ€€€€€€€€€€Ï› ÏÏ            Œ
 *  3  rel req ›€…  Œ          crucial ›€€€€€€€€Œ€€€€Ï rel pro
 *                 ƒŒ€€€€€€€€€€››  Ï ›€€€€€€€€€€Œ€‚
 *  4              Œ  abs proÏ€ˆ‚ ‡€Öabs req   Œ 
 *                 ‡²²²²²²²²²²²²²„€€€€€€€€€€‚²† 
 *  5              Œ··rel proÏ€‚·ƒ€›rel req··Œ ‡€€Ï abs pro
 *                 Œ···········››·Ï·›€€€€€€€€€Œ€…
 *  6  rel req ›€‚ Œƒ€€€€€€€›·CRUCIAL ›€€€€€€€Œ€€€€Ï rel pro
 *                Œ·························Œ
 *  7             Œ·abs pro··safe ·abs req›…·Œ
 *                „²²²²²²²²²²ÏÏ²›²²²²²²²²²²²²…
 *  8             „€€€€€€€€€€€…               ƒ€€Ï rel pro
 *               „€€€€€€€€€€€€€€€… „€€€€€€€€€€€€€€…
 *  9                      
 *                         
 * 10                „€€€€€€€€Ï SAFE
 *
 *
 *
 *  COPY/REPLACE ================================================
 *
 *     ---a--- ---b--- ---c--- ---d--- ---e--- ---f--- ---g---
 *
 *  1                  abs req ›€€‚                 OUTER
 *                  ƒ²²²²²²²²²²²²²²²²²²²²²INNER·‚
 *  2               Œ  rel proÏ€‚ ƒ€›rel req   Œ
 *               ƒ€€Œ€€€€€€€€€€Ï› ÏÏ            Œ
 *  3  rel req ›€…  Œ          crucial ›€€€€€€€€Œ€€€€Ï rel pro
 *                 ƒŒ€€€€€€€€€€››  Ï ›€€€€€€€€€€Œ€‚
 *  4              Œ  abs proÏ€ˆ‚ ‡€Öabs req   Œ 
 *                 ‡²²²²²²²²²²²²²„€€€€€€€€€€‚²† 
 *  5              Œ··rel proÏ€‚·ƒ€›rel req··Œ ‡€€Ï abs pro
 *                 Œ···········››·Ï·›€€€€€€€€€Œ€…
 *  6    ERROR ›€‚ Œƒ€€€€€€€›·CRUCIAL ›€€€€€€€Œ€€€€Ï rel pro
 *                Œ·························Œ
 *  7             Œ·abs pro··XXXX ·abs req›…·Œ
 *                „²²²²²²²²²²ÏÏ²²²²²²²²²²²²²²…
 *  8             „€€€€€€€€€€€…                   Ï rel pro
 *               „€€€€€€€€€€€€€€€…                 
 *  9                      
 *                         
 * 10                „€€€€€€€€Ï SAFE
 *
 *
 *
 *  CUT/NORMAL (confirmed) ======================================
 *
 *     ---a--- ---b--- ---c--- ---d--- ---e--- ---f--- ---g---
 *
 *  1                    ERROR ›€€‚                 OUTER
 *                  ƒ²²²²²²²²²²²²²²²²²²²²²INNER·‚
 *  2               Œ    BLANKÏ     ÏBLANK     Œ
 *               ƒ€€Œ€€€€€€€€€€Ï  Ï             Œ
 *  3    ERROR ›€…  Œ          XXXXXXX          Œ    Ï rel pro
 *                  Œ              Ï            Œ  
 *  4               Œ    BLANKÏ€€‚ ‡€ÖBLANK     Œ  
 *                  ‡²²²²²²²²²²²²²„€€€€€€€€€€‚²†  
 *  5               Œ··rel proÏ€‚·ƒ€›rel req··Œ ƒ€€Ï abs pro
 *                  Œ···········››·Ï·›€€€€€€€€€Œ€…
 *  6  rel req ›€‚  Œƒ€€€€€€€›· ERROR  ›€€€€€€€Œ€€€€Ï rel pro
 *                 Œ·························Œ
 *  7              Œ·abs pro··safe ·  ERROR›…·Œ
 *                 „²²²²²²²²²²²Ï²›²²²²²²²²²²²²…
 *  8                                         ƒ€€Ï rel pro
 *               „€€€€€€€€€€€€€€€… „€€€€€€€€€€€€€€…
 *  9                      
 *                         
 * 10                „€€€€€€€€Ï SAFE
 *
 *
 *
 *  CUT/MOVE ====================================================
 *     (ybR-) clear, adjust all copied formulas, adjust relative in providers
 *
 *     ---a--- ---b--- ---c--- ---d--- ---e--- ---f--- ---g---
 *
 *  1                    ERROR ›€€‚                 OUTER
 *                  ƒ²²²²²²²²²²²²²²²²²²²²²INNER·‚
 *  2               Œ                          Œ
 *                  Œ             Ï             Œ
 *  3  rel req ›€€‚ Œ          XXXXXXX          Œ    Ï rel pro
 *                 Œ                           Œ  
 *  4              Œ                           Œ  
 *                 ‡²²²²²²²²²²²²²²²²²²²²²²²²²²²†  
 *  5              Œ··rel proÏ€‚··ƒ€›rel req···Œ
 *                „€Œ€€€€€€€€€Ï·›··Ï············Œ  
 *  6    ERROR ›€‚  Œƒ€€€€€€€›·CRUCIAL ›€€€€€€€€Œ€€€€Ï rel pro
 *                 Œ········ƒ€›··Ï€‚›€€€€€€€€€Œ€‚
 *  7              Œ·abs proÏ··XXX·›abs req···Œ „€€Ï abs pro
 *                 „²²²²²²²²²²²Ï²²²²²²²²²²²²²²…
 *  8                                             Ï rel pro
 *               „€€€€€€€€€€€€€€€…                 
 *  9                      
 *                         
 * 10                „€€€€€€€€Ï SAFE
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

struct cACCESSOR my;
char    g_contents      [LEN_RECD] = "";
char    message [LEN_RECD] = "";
char    special = 0;


static long s_stamp = 0;



char
api_vikeys_init         (void)
{
   /*> yVIKEYS_menu_add ("µePn", "normal"    , "Pn");                                 <* 
    *> yVIKEYS_menu_add ("µeP#", "clear"     , "P#");                                 <* 
    *> yVIKEYS_menu_add ("µePr", "replace"   , "Pr");                                 <* 
    *> yVIKEYS_menu_add ("µePd", "dup"       , "Pd");                                 <* 
    *> yVIKEYS_menu_add ("µePc", "combo"     , "Pc");                                 <* 
    *> yVIKEYS_menu_add ("µePm", "move"      , "Pm");                                 <* 
    *> yVIKEYS_menu_add ("µePf", "force"     , "Pf");                                 <* 
    *> yVIKEYS_menu_add ("µePv", "values"    , "Pv");                                 <*/
   return 0;
}

char
api_yvikeys_handlers    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_FILE   yLOG_enter   (__FUNCTION__);
   /*---(handlers)-----------------------*/
   rc = yPARSE_handler_max (FILE_TABS    , "tab"       , 4.1, "LTcTTc------", -1, TAB_reader      , TAB_writer_all  , "------------" , "name,max,t,beg,cur,c"      , "gyges tabs (universe)"    );
   DEBUG_FILE   yLOG_value   ("tab"       , rc);
   rc = yPARSE_handler_max (FILE_COLS    , "width"     , 4.2, "Tss---------", -1, COL_reader      , COL_writer_all  , "------------" , "label,wid,cnt"             , "gyges cols (x-axis)"      );
   DEBUG_FILE   yLOG_value   ("width"     , rc);
   rc = yPARSE_handler_max (FILE_ROWS    , "height"    , 4.3, "Tss---------", -1, ROW_reader      , ROW_writer_all  , "------------" , "label,tal,cnt"             , "gyges rows (y-axis)"      );
   DEBUG_FILE   yLOG_value   ("height"    , rc);
   rc = yPARSE_handler_max (FILE_DEPCEL  , "cell_dep"  , 5.1, "TTO---------", -1, CELL_reader     , CELL_writer_all , "------------" , "label,afdu?,contents"      , "gyges dependent cells"    );
   DEBUG_FILE   yLOG_value   ("cell_dep"  , rc);
   rc = yPARSE_handler_max (FILE_FREECEL , "cell_free" , 5.2, "TTO---------", -1, CELL_reader     , NULL            , "------------" , "label,afdu?,contents"      , "gyges free cells"         );
   DEBUG_FILE   yLOG_value   ("cell_free" , rc);
   /*---(complete)-----------------------*/
   DEBUG_FILE   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                   support for map mode                       ----===*/
/*====================------------------------------------====================*/
static void   o___MAP_____________o (void) { return; }



/*====================------------------------------------====================*/
/*===----                    source mode                               ----===*/
/*====================------------------------------------====================*/
static void   o___SOURCE__________o (void) { return; }

char
api_yvikeys_saver       (char *a_contents)
{
   CELL_change  (NULL, YMAP_BEG, CTAB, CCOL, CROW, a_contents);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     data export/import                       ----===*/
/*====================------------------------------------====================*/
static void   o___EXIM____________o (void) { return; }

char         /*-> keys for formatting sub-mode -------[ ------ [gc.MT0.202.C7]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
api_yvikeys_exim        (char a_dir, char a_style)
{
   char        rc          =    0;
   /*---(header)--------------------s----*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_dir"     , a_dir);
   DEBUG_REGS   yLOG_char    ("a_style"   , a_style);
   switch (a_dir) {
   case '+' :
      DEBUG_REGS   yLOG_note    ("call importer (+)");
      rc = EXIM_import (a_style);
      break;
   case '-' :
      DEBUG_REGS   yLOG_note    ("call exporter (-)");
      rc = EXIM_export (a_style);
      break;
   }
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                        searching                             ----===*/
/*====================------------------------------------====================*/
PRIV void  o___SEARCH__________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.#M5.1C#.#7]*/ /*-[03.0000.013.L]-*/ /*-[--.---.---.--]-*/
api_yvikeys_searcher      (uchar a_not, uchar *a_search)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   tCELL      *x_next      = NULL;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   /*---(header)--------------------s----*/
   DEBUG_SRCH   yLOG_enter   (__FUNCTION__);
   DEBUG_SRCH   yLOG_point   ("a_search"  , a_search);
   /*---(defenses)---------------------------*/
   --rce;  if (a_search == NULL) {
      DEBUG_SRCH   yLOG_note    ("can not use null search");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SRCH   yLOG_info    ("a_search"  , a_search);
   x_len = strlen (a_search);
   DEBUG_SRCH   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <= 0) {
      DEBUG_SRCH   yLOG_note    ("can be an empty search");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_search [0] != '/') {
      DEBUG_SRCH   yLOG_note    ("must start with a forward slash");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (x_len == 1) {
      DEBUG_SRCH   yLOG_note    ("nothing to do, redraw only");
      DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   rc = yREGEX_comp (a_search + 1);
   DEBUG_SRCH   yLOG_value   ("comp rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_SRCH   yLOG_note    ("could not compile search");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process range)----------------------*/
   DEBUG_CELL   yLOG_point   ("hcell"     , hcell);
   x_next = hcell;
   do {
      DEBUG_SRCH   yLOG_complex ("x_next"    , "ptr %p, tab %2d, col %3d, row %4d", x_next, x_tab, x_col, x_row);
      if (x_next != NULL && x_next->source != NULL) {
         DEBUG_SRCH   yLOG_char    ("->type"    , x_next->type);
         switch (x_next->type) {
         case YCALC_DATA_STR   :
            DEBUG_SRCH   yLOG_info    ("->source"       , x_next->source);
            rc = yREGEX_filter (x_next->source);
            break;
         case YCALC_DATA_SFORM :
         case YCALC_DATA_SLIKE :
            DEBUG_SRCH   yLOG_info    ("->v_str"   , x_next->v_str);
            rc = yREGEX_filter (x_next->v_str);
            break;
         default          :
            DEBUG_SRCH   yLOG_note    ("can not process cell type");
            rc = -1;
            break;
         }
         DEBUG_SRCH   yLOG_value   ("exec rc"   , rc);
         if (a_not != 'y' && rc > 0) {
            yMARK_found (x_next->label, x_next->tab, x_next->col, x_next->row, 0);
            x_next->note = 's';
         } else if (a_not == 'y' && rc <= 0) {
            yMARK_found (x_next->label, x_next->tab, x_next->col, x_next->row, 0);
            x_next->note = 's';
         }
      }
      x_next = x_next->m_next;
   } while (x_next != NULL && x_next != DONE_DONE);
   /*---(complete)---------------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_yvikeys_unsearcher   (uchar *a_label, ushort u, ushort x, ushort y, ushort z)
{
   /*---(locals)-----------+------+----+-*/
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_SRCH   yLOG_enter   (__FUNCTION__);
   DEBUG_SRCH   yLOG_value   ("u"         , u);
   DEBUG_SRCH   yLOG_value   ("x"         , x);
   DEBUG_SRCH   yLOG_value   ("y"         , y);
   DEBUG_SRCH   yLOG_value   ("z"         , z);
   x_curr = LOC_cell_at_loc (u, x, y);
   DEBUG_SRCH   yLOG_point   ("x_curr"    , x_curr);
   if (x_curr != NULL) x_curr->note = '-';
   DEBUG_SRCH   yLOG_char    ("x_curr->n" , x_curr->note);
   /*---(complete)---------------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     map mode registers                       ----===*/
/*====================------------------------------------====================*/
static void  o___REGISTERS_______o () { return; }

char
api_yvikeys_clearer     (char a_1st, ushort u, ushort x, ushort y, ushort z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   static int  x_count     =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(check first)--------------------*/
   DEBUG_REGS   yLOG_char    ("a_1st"     , a_1st);
   if (a_1st == 'y')  x_count = 0;
   /*---(identify cell)------------------*/
   DEBUG_REGS   yLOG_complex ("coords"    , "%3db, %3dx, %3dy", u, x, y);
   x_curr = LOC_cell_at_loc (u, x, y);
   DEBUG_REGS   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr == NULL) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(delete)-------------------------*/
   DEBUG_REGS   yLOG_value   ("x_count"   , x_count);
   if (a_1st == 'y')  rc = CELL_change (NULL, YMAP_BEG, u, x, y, "");
   else               rc = CELL_change (NULL, YMAP_ADD, u, x, y, "");
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update count)-------------------*/
   ++x_count;
   DEBUG_REGS   yLOG_value   ("x_count"   , x_count);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tail recursion function for copy ---[ ------ [ge.D55.237.63]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
api__yvikeys_copier_one       (tCELL *a_curr, long a_stamp)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   tCELL      *x_copy      = NULL;
   ushort      u, xb, xe, yb, ye;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_REGS   yLOG_point   ("a_curr"    , a_curr);
   --rce;  if (a_curr    == NULL) {
      DEBUG_REGS   yLOG_note    ("original cell null");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* no cell                       */
   }
   DEBUG_REGS   yLOG_complex ("DEBUG 2"   , "%-10.10s, %2dt, %3dc, %4dr", a_curr->label, a_curr->tab, a_curr->col, a_curr->row);
   DEBUG_REGS   yLOG_point   ("s"         , a_curr->source);
   --rce;  if (a_curr->source == NULL) {
      DEBUG_REGS   yLOG_note    ("no source");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* nothing to write              */
   }
   DEBUG_REGS   yLOG_info    ("s"         , a_curr->source);
   DEBUG_REGS   yLOG_char    ("t"         , a_curr->type);
   --rce;  if (a_curr->type == '-')  {
      DEBUG_REGS   yLOG_note    ("could not copy an empty");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* don't write, recreate on read */
   }
   /*---(check for bounds)---------------*/
   yMAP_visu_range (&u, &xb, &xe, &yb, &ye, NULL, NULL, NULL);
   DEBUG_REGS   yLOG_complex ("visual"    , "%2dt, %3d to %3dc, %4d to %4dr", u, xb, xe, yb, ye);
   rc = yMAP_inside (a_curr->tab, a_curr->col, a_curr->row, 0);
   DEBUG_REGS   yLOG_value   ("visu_rc"   , rc);
   --rce;  if (rc <= 0)  {
      DEBUG_REGS   yLOG_note    ("cell not in visual area");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(make a copy)--------------------*/
   rc = CELL_dup (&x_copy, a_curr);
   DEBUG_REGS   yLOG_value   ("dup_rc"    , rc);
   --rce;  if (rc < 0 ) {
      DEBUG_REGS   yLOG_note    ("could not duplicate");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(move in critical data)----------*/
   x_copy->label = strdup (a_curr->label);
   yCALC_stamp_set (a_curr->ycalc, s_stamp);
   DEBUG_REGS   yLOG_info    ("label"     , x_copy->label);
   /*---(place in buffer)----------------*/
   rc = yMAP_mreg_add  (x_copy, x_copy->label);
   DEBUG_REGS   yLOG_value   ("hook_rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_note    ("could not hook to register");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_yvikeys_copier_seq        (void *a_owner, void *a_deproot, int a_seq, int a_level)
{
   return  api__yvikeys_copier_one ((tCELL *) a_owner, s_stamp);
}

char
api_yvikeys_copier      (char a_type, long a_stamp)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   ushort      x_tab       =    0;
   ushort      x_col       =    0;
   ushort      x_row       =    0;
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   DEBUG_REGS   yLOG_value   ("a_stamp"   , a_stamp);
   s_stamp = a_stamp;
   /*---(dependents)---------------------*/
   DEBUG_REGS   yLOG_note    ("DEPENDENT CELLS");
   rc      = yCALC_seq_downdown (s_stamp, api_yvikeys_copier_seq);
   /*---(independents)-------------------*/
   DEBUG_REGS   yLOG_note    ("INDEPENDENT CELLS");
   rc      = yMAP_visu_first (&x_tab, &x_col, &x_row, NULL);
   while (rc >= 0) {
      x_curr  = LOC_cell_at_loc (x_tab, x_col, x_row);
      if (x_curr != NULL && !yCALC_stamp_cmp (x_curr->ycalc, s_stamp))   api__yvikeys_copier_one (x_curr, s_stamp);
      rc      = yMAP_visu_next  (&x_tab, &x_col, &x_row, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_yvikeys_router      (tCELL *a_cell, char *a_list)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_label     [LEN_LABEL]  = "";
   int         x_npro      =    0;
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_REGS   yLOG_point   ("a_cell"    , a_cell);
   --rce;  if (a_cell == NULL)  {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_point   ("a_list"    , a_list);
   --rce;  if (a_list == NULL)  {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find real cell)-----------------*/
   strlcpy (x_label, a_cell->label, LEN_LABEL);
   DEBUG_REGS   yLOG_info    ("label"     , x_label);
   x_curr = LOC_cell_labeled (x_label);
   DEBUG_REGS   yLOG_point   ("x_curr"    , x_curr);
   if (x_curr == NULL) {
      DEBUG_REGS   yLOG_note    ("no cell at original location");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_REGS   yLOG_point   ("ycalc"     , x_curr->ycalc);
   /*---(return providers)---------------*/
   yCALC_show_pros (x_curr->ycalc, &x_npro, a_list);
   DEBUG_REGS   yLOG_value   ("npro"      , x_npro);
   DEBUG_REGS   yLOG_info    ("a_list"    , a_list);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_yvikeys__rerouter   (char a_pros, ushort a_uoff, ushort a_xoff, ushort a_yoff, int a_zoff, tCELL *a_cell, char *a_list)
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
   ushort      u, xb, xe, yb, ye;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_pros"    , a_pros);
   if (a_pros == G_RPN_IGNORE || a_pros == G_RPN_PNONE) {
      DEBUG_REGS   yLOG_note    ("provider updates not requested");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_list == NULL) {
      DEBUG_REGS   yLOG_note    ("no providers identified");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   yMAP_visu_range (&u, &xb, &xe, &yb, &ye, NULL, NULL, NULL);
   DEBUG_REGS   yLOG_complex ("visual"    , "%2dt, %3d to %3dc, %4d to %4dr", u, xb, xe, yb, ye);
   p  = strtok_r (a_list, q, &s);
   DEBUG_REGS   yLOG_point   ("p"         , p);
   while (p != NULL) {
      DEBUG_REGS   yLOG_info    ("p"         , p);
      x_provider = LOC_cell_labeled (p);
      DEBUG_REGS   yLOG_point   ("x_provider", x_provider);
      if (x_provider != NULL) {
         DEBUG_REGS   yLOG_complex ("details"   , "%s, %3db, %3dx, %3dy", x_provider->label, x_provider->tab, x_provider->col, x_provider->row);
         rc = yMAP_inside (x_provider->tab, x_provider->col, x_provider->row, 0);
         DEBUG_REGS   yLOG_value   ("rc"        , rc);
         if (rc == 0) {
            DEBUG_REGS   yLOG_info    ("source"    , x_provider->source);
            DEBUG_REGS   yLOG_info    ("change"    , a_cell->label);
            rc = yRPN_addr_provide (x_provider->source, a_pros, a_cell->label, a_uoff, a_xoff, a_yoff, a_zoff, LEN_RECD, x_source);
            DEBUG_REGS   yLOG_value   ("rc"        , rc);
            DEBUG_REGS   yLOG_info    ("x_source"  , x_source);
            sprintf (x_bformat, "%c%c%c%c-", x_provider->align, x_provider->format, x_provider->decs, x_provider->unit);
            DEBUG_REGS   yLOG_info    ("x_bformat" , x_bformat);
            CELL_overwrite (YMAP_ADD, x_provider->tab, x_provider->col, x_provider->row, x_source, x_bformat);
         }
      }
      /*> CELL_change  (x_copy, YMAP_NONE, x_dtab, x_dcol, x_drow, strdup (x_copy->source));   <*/
      p  = strtok_r (NULL  , q, &s);
      DEBUG_REGS   yLOG_point   ("p"         , p);
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_yvikeys_paster      (char a_reqs, char a_pros, char a_intg, char a_1st, short a_uoff, short a_xoff, short a_yoff, short a_zoff, tCELL *a_cell, char *a_list)
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
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_reqs"    , a_reqs);
   DEBUG_REGS   yLOG_char    ("a_pros"    , a_pros);
   DEBUG_REGS   yLOG_char    ("a_intg"    , a_intg);
   DEBUG_REGS   yLOG_char    ("a_1st"     , a_1st);
   DEBUG_REGS   yLOG_complex ("offset"    , "b=%4d, x=%4d, y=%4d, z=%4d", a_uoff, a_xoff, a_yoff, a_zoff);
   /*---(defense)------------------------*/
   DEBUG_REGS   yLOG_point   ("a_cell"    , a_cell);
   --rce;  if (a_cell == NULL)  {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_info    ("G_RPN_REQS", G_RPN_REQS);
   --rce;  if (strchr (G_RPN_REQS, a_reqs) == NULL)  {
      DEBUG_REGS   yLOG_note    ("a_reqs type not recognized");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_info    ("G_RPN_PROS", G_RPN_PROS);
   --rce;  if (strchr (G_RPN_PROS, a_pros) == NULL)  {
      DEBUG_REGS   yLOG_note    ("a_pros type not recognized");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get original location)----------*/
   DEBUG_REGS   yLOG_info    ("a_label"   , a_cell->label);
   rc = str2gyges (a_cell->label, &x_stab, &x_scol, &x_srow, NULL, NULL, 0, YSTR_USABLE);
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0)  {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_complex ("original"  , "tab=%4d, col=%4d, row=%4d", x_stab, x_scol, x_srow);
   /*---(set new location)---------------*/
   x_dtab  = x_stab + a_uoff;
   x_dcol  = x_scol + a_xoff;
   x_drow  = x_srow + a_yoff;
   DEBUG_REGS   yLOG_complex ("going to"  , "tab=%4d, col=%4d, row=%4d", x_dtab, x_dcol, x_drow);
   /*---(reroute providers)--------------*/
   api_yvikeys__rerouter (a_pros, a_uoff, a_xoff, a_yoff, a_zoff, a_cell, a_list);
   /*---(check cell type)----------------*/
   DEBUG_REGS   yLOG_info    ("source"    , a_cell->source);
   DEBUG_REGS   yLOG_char    ("type"      , a_cell->type);
   strcpy (x_source, "");
   if (strchr (YCALC_GROUP_RPN, a_cell->type) != 0) {
      DEBUG_REGS   yLOG_note    ("formula, calling yRPN_adjust");
      rc = yRPN_addr_require (a_cell->source, a_reqs, a_uoff, a_xoff, a_yoff, a_zoff, LEN_RECD, x_source);
      DEBUG_REGS   yLOG_value   ("rc"        , rc);
      /*> if (rc < 0) {                                                               <* 
       *>    DEBUG_REGS   yLOG_note    ("formula could not be parsed");               <* 
       *>    strcpy (x_source, "not_translatable");                                   <* 
       *> }                                                                           <*/
   } else {
      DEBUG_REGS   yLOG_note    ("just copy straight across");
      strcpy (x_source, a_cell->source);
   }
   DEBUG_REGS   yLOG_info    ("x_source"  , x_source);
   sprintf (x_bformat, "%c%c%c%c-", a_cell->align, a_cell->format, a_cell->decs, a_cell->unit);
   DEBUG_REGS   yLOG_info    ("x_bformat" , x_bformat);
   if (a_1st == 'y')  x_copy = CELL_overwrite (YMAP_BEG, x_dtab, x_dcol, x_drow, x_source, x_bformat);
   else               x_copy = CELL_overwrite (YMAP_ADD, x_dtab, x_dcol, x_drow, x_source, x_bformat);
   DEBUG_REGS   yLOG_complex ("DEBUG 3"   , "%-10.10s, %2dt, %3dc, %4dr", x_copy->label, x_copy->tab, x_copy->col, x_copy->row);
   /*---(providers)----------------------*/
   /*> DEBUG_REGS   yLOG_note    ("CHECK PROVIDERS");                                 <*/
   /*> DEBUG_REGS   yLOG_char    ("a_pros"    , a_pros);                              <*/
   /*> if (a_pros == G_RPN_IGNORE || a_pros == G_RPN_PNONE) {                                                                                          <* 
    *>    DEBUG_REGS   yLOG_note    ("provider updates not requested");                                                                                <* 
    *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                                    <* 
    *>    return 0;                                                                                                                                    <* 
    *> }                                                                                                                                               <* 
    *> DEBUG_REGS   yLOG_complex ("original"  , "tab=%4d, col=%4d, row=%4d", x_stab, x_scol, x_srow);                                                  <* 
    *> x_original = LOC_cell_at_loc (x_stab, x_scol, x_srow);                                                                                          <* 
    *> DEBUG_REGS   yLOG_point   ("x_original", x_original);                                                                                           <* 
    *> if (x_original == NULL) {                                                                                                                       <* 
    *>    DEBUG_REGS   yLOG_note    ("no cell at original location");                                                                                  <* 
    *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                                    <* 
    *>    return 0;                                                                                                                                    <* 
    *> }                                                                                                                                               <* 
    *> strlcpy (x_label, x_original->label, LEN_LABEL);                                                                                                <* 
    *> DEBUG_REGS   yLOG_info    ("label"     , x_label);                                                                                              <* 
    *> yCALC_show_reqs (x_original->ycalc, &x_nreq, NULL);                                                                                             <* 
    *> DEBUG_REGS   yLOG_value   ("nreq"      , x_nreq);                                                                                               <* 
    *> yCALC_show_pros (x_original->ycalc, &x_npro, x_list);                                                                                           <* 
    *> DEBUG_REGS   yLOG_value   ("npro"      , x_npro);                                                                                               <* 
    *> DEBUG_REGS   yLOG_info    ("x_list"    , x_list);                                                                                               <* 
    *> DEBUG_REGS   yLOG_complex ("x_original", "%s, nreq=%d, npro=%d", x_label, x_nreq, x_npro);                                                      <* 
    *> if (strchr ("-.", x_list [0]) != NULL) {                                                                                                        <* 
    *>    DEBUG_REGS   yLOG_note    ("no providers identified");                                                                                       <* 
    *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                                    <* 
    *>    return 0;                                                                                                                                    <* 
    *> }                                                                                                                                               <* 
    *> if (a_list == NULL) {                                                                                                                           <* 
    *>    DEBUG_REGS   yLOG_note    ("no providers identified");                                                                                       <* 
    *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                                    <* 
    *>    return 0;                                                                                                                                    <* 
    *> }                                                                                                                                               <* 
    *> p  = strtok_r (a_list, q, &s);                                                                                                                  <* 
    *> DEBUG_REGS   yLOG_point   ("p"         , p);                                                                                                    <* 
    *> while (p != NULL) {                                                                                                                             <* 
    *>    DEBUG_REGS   yLOG_info    ("p"         , p);                                                                                                 <* 
    *>    x_provider = LOC_cell_labeled (p);                                                                                                           <* 
    *>    DEBUG_REGS   yLOG_point   ("x_provider", x_provider);                                                                                        <* 
    *>    if (x_provider != NULL) {                                                                                                                    <* 
    *>       DEBUG_REGS   yLOG_complex ("details"   , "%s, %3db, %3dx, %3dy", x_provider->label, x_provider->tab, x_provider->col, x_provider->row);   <* 
    *>       rc = yMAP_inside (x_provider->tab, x_provider->col, x_provider->row, 0);                                                                  <* 
    *>       DEBUG_REGS   yLOG_value   ("rc"        , rc);                                                                                             <* 
    *>       if (rc == 0) {                                                                                                                            <* 
    *>          DEBUG_REGS   yLOG_info    ("source"    , x_provider->source);                                                                          <* 
    *>          DEBUG_REGS   yLOG_info    ("change"    , a_cell->label);                                                                               <* 
    *>          rc = yRPN_addr_provide (x_provider->source, a_pros, a_cell->label, a_uoff, a_xoff, a_yoff, a_zoff, LEN_RECD, x_source);                <* 
    *>          DEBUG_REGS   yLOG_value   ("rc"        , rc);                                                                                          <* 
    *>          DEBUG_REGS   yLOG_info    ("x_source"  , x_source);                                                                                    <* 
    *>          sprintf (x_bformat, "%c%c%c%c-", x_provider->align, x_provider->format, x_provider->decs, x_provider->unit);                           <* 
    *>          DEBUG_REGS   yLOG_info    ("x_bformat" , x_bformat);                                                                                   <* 
    *>          CELL_overwrite (YMAP_ADD, x_provider->tab, x_provider->col, x_provider->row, x_source, x_bformat);                                     <* 
    *>       }                                                                                                                                         <* 
    *>    }                                                                                                                                            <* 
    *>    /+> CELL_change  (x_copy, YMAP_NONE, x_dtab, x_dcol, x_drow, strdup (x_copy->source));   <+/                                                 <* 
    *>    p  = strtok_r (NULL  , q, &s);                                                                                                               <* 
    *>    DEBUG_REGS   yLOG_point   ("p"         , p);                                                                                                 <* 
    *> }                                                                                                                                               <*/
   /*> DEBUG_REGS   yLOG_complex ("DEBUG 5"   , "%-10.10s, %2dt, %3dc, %4dr", x_copy->label, x_copy->tab, x_copy->col, x_copy->row);   <*/
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_yvikeys_finisher    (short a_uoff, short a_xoff, short a_yoff, short a_zoff, tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_label     [LEN_LABEL]  = "";
   short       x_stab, x_scol, x_srow;
   short       x_dtab, x_dcol, x_drow;
   tCELL      *x_curr      = NULL;
   char        x_list      [LEN_RECD]   = "";
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_REGS   yLOG_point   ("a_cell"    , a_cell);
   --rce;  if (a_cell == NULL)  {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get original location)----------*/
   DEBUG_REGS   yLOG_info    ("a_label"   , a_cell->label);
   rc = str2gyges (a_cell->label, &x_stab, &x_scol, &x_srow, NULL, NULL, 0, YSTR_USABLE);
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0)  {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_complex ("original"  , "tab=%4d, col=%4d, row=%4d", x_stab, x_scol, x_srow);
   /*---(set new location)---------------*/
   x_dtab  = x_stab + a_uoff;
   x_dcol  = x_scol + a_xoff;
   x_drow  = x_srow + a_yoff;
   DEBUG_REGS   yLOG_complex ("going to"  , "tab=%4d, col=%4d, row=%4d", x_dtab, x_dcol, x_drow);
   x_curr  = LOC_cell_at_loc (x_dtab, x_dcol, x_drow);
   DEBUG_REGS   yLOG_point   ("x_curr"    , x_curr);
   if (x_curr == NULL) {
      DEBUG_REGS   yLOG_note    ("can not find cell");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_REGS   yLOG_complex ("DEBUG 6"   , "%-10.10p, %-10.10s, %2dt, %3dc, %4dr", x_curr, x_curr->label, x_curr->tab, x_curr->col, x_curr->row);
   /*---(find likes)---------------------*/
   /*> yCALC_disp_like (x_curr->ycalc, x_list);                                       <*/
   /*> DEBUG_REGS   yLOG_info    ("x_list"    , x_list);                              <*/
   DEBUG_REGS   yLOG_complex ("update"    , "tab=%4d, col=%4d, row=%4d", x_dtab, x_dcol, x_drow);
   yCALC_handle (x_curr->label);
   /*> if (x_curr->source != NULL && x_curr->source [0] == '~') {                     <* 
    *>    DEBUG_REGS   yLOG_note    ("recalculate cell and dependencies");            <* 
    *>    yCALC_handle (x_curr->label);                                               <* 
    *> }                                                                              <*/
   DEBUG_REGS   yLOG_complex ("finally"   , "tab=%4d, col=%4d, row=%4d", x_dtab, x_dcol, x_drow);
   x_curr  = LOC_cell_at_loc (x_dtab, x_dcol, x_drow);
   DEBUG_REGS   yLOG_complex ("DEBUG 8"   , "%-10.10p, %-10.10s, %2dt, %3dc, %4dr", x_curr, x_curr->label, x_curr->tab, x_curr->col, x_curr->row);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> delete a register cell -------------[ ------ [gz.210.101.01]*/ /*-[00.0000.104.!]-*/ /*-[--.---.---.--]-*/
api_yvikeys_regkiller    (tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_APIS   yLOG_enter   (__FUNCTION__);
   DEBUG_APIS   yLOG_point   ("a_cell"    , a_cell);
   /*---(kill)---------------------------*/
   rc = CELL__free (&a_cell);
   DEBUG_APIS   yLOG_value   ("free"      , rc);
   /*---(complete)-----------------------*/
   DEBUG_APIS   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                     keyboard macro handler                   ----===*/
/*====================------------------------------------====================*/
static void  o___MACRO___________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.540.136.51]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
api_yvikeys_macro_get       (char a_name, char *a_macro)
{
   char        rce         =  -10;
   short       x_row       =    0;
   tCELL      *x_curr      = NULL;
   --rce;  if (a_macro   == NULL     )                      return rce;
   --rce;  if (a_name < 'a' || a_name > 'z')  return rce;
   x_row = a_name - 'a';
   x_curr = LOC_cell_at_loc (37, 1, x_row);
   --rce;  if (x_curr    == NULL)                           return rce;
   if (x_curr->type == YCALC_DATA_STR) {
      strlcpy (a_macro, x_curr->source    , LEN_RECD);
      return 0;
   }
   if (x_curr->type == YCALC_DATA_SFORM) {
      strlcpy (a_macro, x_curr->v_str, LEN_RECD);
      return 0;
   }
   --rce;
   return rce;
}

char         /*-> tbd --------------------------------[ ------ [ge.540.153.42]*/ /*-[01.0000.105.!]-*/ /*-[--.---.---.--]-*/
api_yvikeys_macro_set       (char a_name, char *a_keys)
{
   char        rce         =  -10;
   short       x_row       =    0;
   char        t           [LEN_LABEL]   = "";
   int         x_len       =    0;
   tCELL      *x_curr      = NULL;
   --rce;  if (a_keys   == NULL     )                      return rce;
   --rce;  if (a_name < 'a' || a_name > 'z')  return rce;
   x_row = a_name - 'a';
   x_len = strlen (a_keys);
   if (x_len == 0) {
      CELL_change (NULL, YMAP_NONE , 37, 0, x_row, "");
      CELL_change (NULL, YMAP_NONE , 37, 1, x_row, "");
   } else {
      sprintf (t, "%c", a_name);
      CELL_change (NULL, YMAP_NONE , 37, 0, x_row, t     );
      CELL_change (NULL, YMAP_NONE , 37, 1, x_row, a_keys);
   }
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     mapping for map mode                     ----===*/
/*====================------------------------------------====================*/
static void  o___MAPPING_________o () { return; }

/*> char                                                                               <* 
 *> MAP__clear            (tMAPPED *a_map)                                             <* 
 *> {                                                                                  <* 
 *>    /+---(locals)-----------+-----------+-+/                                        <* 
 *>    int         i           =    0;                                                 <* 
 *>    /+---(lefts)--------------------------+/                                        <* 
 *>    a_map->umin = a_map->gmin = a_map->gamin = a_map->glmin = a_map->gprev = -1;    <* 
 *>    /+---(map)----------------------------+/                                        <* 
 *>    for (i= 0; i < LEN_HUGE; ++i)  a_map->map [i] =  YVIKEYS_EMPTY;                 <* 
 *>    /+---(rights)-------------------------+/                                        <* 
 *>    a_map->umax = a_map->gmax = a_map->gamax = a_map->glmax = a_map->gnext = -1;    <* 
 *>    /+---(screen)-------------------------+/                                        <* 
 *>    a_map->ubeg  = a_map->ucur  = a_map->uend  = a_map->ulen  = a_map->utend = 0;   <* 
 *>    /+---(grids)--------------------------+/                                        <* 
 *>    a_map->gbeg = a_map->gcur = a_map->gend = 0;                                    <* 
 *>    /+---(complete)-----------------------+/                                        <* 
 *>    return  0;                                                                      <* 
 *> }                                                                                  <*/

char
MAP_rows             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   ushort      b, c, e;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(rows)---------------------------*/
   rc = yMAP_axis_grid (YMAP_YAXIS, &b, &c, &e, NULL);
   DEBUG_MAP    yLOG_complex ("ROWS"      , "%4d § %4dn, %4db, %4dc, %4de § %4db, %4dc, %4de", rc, NROW, BROW, CROW, EROW, b, c, e);
   if (CROW == c) {
      DEBUG_MAP    yLOG_note    ("row did not change, do not remap");
   } else {
      DEBUG_MAP    yLOG_note    ("row change, remap");
      COL_mapper (0);
   }
   BROW   = b;
   CROW   = c;
   EROW   = e;
   DEBUG_MAP    yLOG_complex ("ROWS+"     , "%4d § %4dn, %4db, %4dc, %4de § %4db, %4dc, %4de", rc, NROW, BROW, CROW, EROW, b, c, e);
   /*---(ocmplete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
MAP_columns          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   ushort      b, c, e;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(columns)------------------------*/
   rc = yMAP_axis_grid (YMAP_XAXIS, &b, &c, &e, NULL);
   DEBUG_MAP    yLOG_complex ("COLS"      , "%4d § %4dn, %4db, %4dc, %4de § %4db, %4dc, %4de", rc, NCOL, BCOL, CCOL, ECOL, b, c, e);
   if (CCOL == c) {
      DEBUG_MAP    yLOG_note    ("column did not change, do not remap");
   } else {
      DEBUG_MAP    yLOG_note    ("column change, remap");
      ROW_mapper (0);
   }
   BCOL   = b;
   CCOL   = c;
   ECOL   = e;
   DEBUG_MAP    yLOG_complex ("COLS+"     , "%4d § %4dn, %4db, %4dc, %4de § %4db, %4dc, %4de", rc, NCOL, BCOL, CCOL, ECOL, b, c, e);
   /*---(ocmplete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
MAP_universe         (char a_req)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   ushort      b, c, e;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(tabs)---------------------------*/
   rc = yMAP_axis_grid (YMAP_UNIV , &b, &c, &e, NULL);
   DEBUG_MAP    yLOG_complex ("TABS"      , "%4d § %4dn, %4db, %4dc, %4de § %4db, %4dc, %4de", NTAB, 0   , CTAB, 0   , b, c, e);
   if (CTAB != c || a_req == YMAP_UNIVERSE) {
      DEBUG_MAP    yLOG_note    ("tab change, force remap");
      TAB_mapper ();
   } else if (a_req == YMAP_UNIVDISP) {
      DEBUG_MAP    yLOG_note    ("UNIVDISP, tab did not change, but forcing remap");
      TAB_mapper ();
   } else {
      DEBUG_MAP    yLOG_note    ("tab did not change, do not remap");
   }
   DEBUG_MAP    yLOG_complex ("TABS+"     , "%4d § %4dn, %4db, %4dc, %4de § %4db, %4dc, %4de", NTAB, 0   , CTAB, 0   , b, c, e);
   /*---(ocmplete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

/*> char                                                                                                                         <* 
 *> MAP_display_only     (void)                                                                                                  <* 
 *> {                                                                                                                            <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                  <* 
 *>    char        rc          =    0;                                                                                           <* 
 *>    ushort      b, c, e;                                                                                                      <* 
 *>    /+---(header)-------------------------+/                                                                                  <* 
 *>    DEBUG_MAP    yLOG_enter   (__FUNCTION__);                                                                                 <* 
 *>    /+---(columns)------------------------+/                                                                                  <* 
 *>    yMAP_axis_grid (YMAP_XAXIS, &b, &c, &e, NULL);                                                                            <* 
 *>    BCOL   = b;                                                                                                               <* 
 *>    CCOL   = c;                                                                                                               <* 
 *>    ECOL   = e;                                                                                                               <* 
 *>    DEBUG_MAP    yLOG_complex ("columns"   , "%4dn, %4db, %4dc, %4de § %4db, %4dc, %4de", NCOL, BCOL, CCOL, ECOL, b, c, e);   <* 
 *>    /+---(rows)---------------------------+/                                                                                  <* 
 *>    yMAP_axis_grid (YMAP_YAXIS, &b, &c, &e, NULL);                                                                            <* 
 *>    BROW   = b;                                                                                                               <* 
 *>    CROW   = c;                                                                                                               <* 
 *>    EROW   = e;                                                                                                               <* 
 *>    DEBUG_MAP    yLOG_complex ("rows"      , "%4dn, %4db, %4dc, %4de § %4db, %4dc, %4de", NROW, BROW, CROW, EROW, b, c, e);   <* 
 *>    /+---(ocmplete)-----------------------+/                                                                                  <* 
 *>    DEBUG_MAP    yLOG_exit    (__FUNCTION__);                                                                                 <* 
 *>    return 0;                                                                                                                 <* 
 *> }                                                                                                                            <*/


char
api_yvikeys_axis_get    (char a_axis, ushort *n, ushort *b, ushort *c, ushort *e)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_senter  (__FUNCTION__);
   DEBUG_MAP    yLOG_schar   (a_axis);
   /*---(defaults)-----------------------*/
   DEBUG_MAP    yLOG_snote   ("default");
   if (n != NULL)  *n = 0;
   if (b != NULL)  *b = 0;
   if (c != NULL)  *c = 0;
   if (e != NULL)  *e = 0;
   /*---(axis)---------------------------*/
   --rce;  switch (a_axis) {
   case YMAP_UNIV  :
      DEBUG_MAP    yLOG_snote   ("u-axis is tabs");
      if (n != NULL)  *n = NTAB;
      if (b != NULL)  *b = 0;
      if (c != NULL)  *c = CTAB;
      if (e != NULL)  *e = NTAB - 1;
      break;
   case YMAP_XAXIS :
      DEBUG_MAP    yLOG_snote   ("x-axis is cols");
      if (n != NULL)  *n = NCOL;
      if (b != NULL)  *b = BCOL;
      if (c != NULL)  *c = CTAB;
      if (e != NULL)  *e = ECOL;
      break;
   case YMAP_YAXIS :
      DEBUG_MAP    yLOG_snote   ("y-axis is rows");
      if (n != NULL)  *n = NROW;
      if (b != NULL)  *b = BROW;
      if (c != NULL)  *c = CROW;
      if (e != NULL)  *e = EROW;
      break;
   case YMAP_ZAXIS :
      DEBUG_MAP    yLOG_snote   ("z-axis, un-used in gyges");
      break;
   default  :
      DEBUG_MAP    yLOG_snote   ("axis not understood");
      DEBUG_MAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
api_yvikeys_axis_set    (char a_axis, ushort b, ushort c, ushort e)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_senter  (__FUNCTION__);
   DEBUG_MAP    yLOG_schar   (a_axis);
   /*---(axis)---------------------------*/
   --rce;  switch (a_axis) {
   case YMAP_UNIV  :
      DEBUG_MAP    yLOG_snote   ("u-axis is tabs");
      CTAB = c;
      break;
   case YMAP_XAXIS :
      DEBUG_MAP    yLOG_snote   ("x-axis is cols");
      BCOL = b;
      CCOL = c;
      ECOL = e;
      break;
   case YMAP_YAXIS :
      DEBUG_MAP    yLOG_snote   ("y-axis is rows");
      BROW = b;
      CROW = c;
      EROW = e;
      break;
   case YMAP_ZAXIS :
      DEBUG_MAP    yLOG_snote   ("z-axis, un-used in gyges");
      break;
   default  :
      DEBUG_MAP    yLOG_snote   ("axis not understood");
      DEBUG_MAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
api_yvikeys_axis_entry  (char a_axis, ushort a_pos, short *a_ref, uchar *a_size, uchar *a_used)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        c           =    0;
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_senter  (__FUNCTION__);
   DEBUG_MAP    yLOG_schar   (a_axis);
   /*---(defaults)-----------------------*/
   DEBUG_MAP    yLOG_snote   ("default");
   if (a_ref  != NULL)  *a_ref  = 0;
   if (a_size != NULL)  *a_size = 0;
   if (a_used != NULL)  *a_used = 0;
   /*---(axis)---------------------------*/
   --rce;  switch (a_axis) {
   case YMAP_UNIV  :
      DEBUG_MAP    yLOG_snote   ("u-axis is tabs");
      /*---(illegal)---------------------*/
      if (VALID_tab (a_pos) != 1) {
         DEBUG_MAP    yLOG_snote   ("not legal");
         DEBUG_MAP    yLOG_sexitr  (__FUNCTION__, rce);
         return rce;
      }
      /*---(basics)----------------------*/
      if (a_ref  != NULL)  *a_ref  = YSTR_UNIV [a_pos];
      if (a_size != NULL)  *a_size = 1;
      /*---(usage)-----------------------*/
      if (a_used == NULL)  break;
      c = TAB_used (a_pos);
      if      (c <  0)   *a_used = YMAP_NADA;
      else if (c == 0)   *a_used = YMAP_PLACE;
      else               *a_used = YMAP_USED;
      /*---(done)------------------------*/
      break;
   case YMAP_XAXIS :
      DEBUG_MAP    yLOG_snote   ("x-axis is cols");
      /*---(illegal)---------------------*/
      if (VALID_col (a_pos) != 1) {
         DEBUG_MAP    yLOG_snote   ("not legal");
         DEBUG_MAP    yLOG_sexitr  (__FUNCTION__, rce);
         return rce;
      }
      /*---(basics)----------------------*/
      if (a_ref  != NULL)  *a_ref  = a_pos;
      if (a_size != NULL)  *a_size = COL_size (CTAB, a_pos);
      if (a_used == NULL)  break;
      /*---(usage)-----------------------*/
      x_curr = LOC_cell_at_loc (CTAB, a_pos, CROW);
      if (x_curr == NULL)  *a_used = YMAP_NADA;
      else                 *a_used = YMAP_USED;
      /*---(done)------------------------*/
      break;
   case YMAP_YAXIS :
      DEBUG_MAP    yLOG_snote   ("y-axis is rows");
      /*---(illegal)---------------------*/
      if (VALID_row (a_pos) != 1) {
         DEBUG_MAP    yLOG_snote   ("not legal");
         DEBUG_MAP    yLOG_sexitr  (__FUNCTION__, rce);
         return rce;
      }
      /*---(basics)----------------------*/
      if (a_ref  != NULL)  *a_ref  = a_pos;
      if (a_size != NULL)  *a_size = ROW_size (CTAB, a_pos);
      if (a_used == NULL)  break;
      /*---(usage)-----------------------*/
      x_curr = LOC_cell_at_loc (CTAB, CCOL, a_pos);
      if (x_curr == NULL)  *a_used = YMAP_NADA;
      else                 *a_used = YMAP_USED;
      /*---(done)------------------------*/
      break;
   case YMAP_ZAXIS :
      DEBUG_MAP    yLOG_snote   ("z-axis, un-used in gyges");
      break;
   default  :
      DEBUG_MAP    yLOG_snote   ("axis not understood");
      DEBUG_MAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_sexit   (__FUNCTION__);
   return 0;
}

/*> char                                                                                 <* 
 *> NODE_map_update            (char a_type, char a_level)                               <* 
 *> {                                                                                    <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                          <* 
 *>    char        rce         =  -10;                                                   <* 
 *>    char        rc          =    0;                                                   <* 
 *>    uchar       x_axis      =  '-';                                                   <* 
 *>    ushort      x_size      =    0;                                                   <* 
 *>    short       x_avail     =    0;                                                   <* 
 *>    int         i           =    0;                                                   <* 
 *>    tCELL      *x_curr      = NULL;                                                   <* 
 *>    tNODE      *x_node      = NULL;                                                   <* 
 *>    ushort      x_wide      =    0;                                                   <* 
 *>    /+---(header)-------------------------+/                                          <* 
 *>    DEBUG_MAP    yLOG_enter   (__FUNCTION__);                                         <* 
 *>    DEBUG_MAP    yLOG_char    ("a_type"    , a_type);                                 <* 
 *>    DEBUG_MAP    yLOG_complex ("sizes"     , "%4dt, %4dc, %4dr", CTAB, NCOL, NROW);   <* 
 *>    /+---(defense)------------------------+/                                          <* 
 *>    --rce;  switch (a_type) {                                                         <* 
 *>    case 'c' :                                                                        <* 
 *>       x_axis = 'x';                                                                  <* 
 *>       x_size = NCOL;                                                                 <* 
 *>       break;                                                                         <* 
 *>    case 'r' :                                                                        <* 
 *>       x_axis = 'y';                                                                  <* 
 *>       x_size = NROW;                                                                 <* 
 *>       break;                                                                         <* 
 *>    default  :                                                                        <* 
 *>       DEBUG_MAP    yLOG_exitr   (__FUNCTION__, rce);                                 <* 
 *>       return rce;                                                                    <* 
 *>       break;                                                                         <* 
 *>    }                                                                                 <* 
 *>    DEBUG_MAP    yLOG_char    ("x_axis"    , x_axis);                                 <* 
 *>    DEBUG_MAP    yLOG_value   ("x_size"    , x_size);                                 <* 
 *>    /+---(preparation)--------------------+/                                          <* 
 *>    rc = yMAP_clear  (x_axis);                                                        <* 
 *>    rc = yMAP_start  (x_axis, x_size);                                                <* 
 *>    IF_COL    yVIEW_size (YVIEW_MAIN, NULL, NULL, &x_avail, NULL, NULL);              <* 
 *>    ELSE_ROW  yVIEW_size (YVIEW_MAIN, NULL, NULL, NULL, NULL, &x_avail);              <* 
 *>    yMAP_axis_avail (x_axis, x_avail);                                                <* 
 *>    /+---(update grid)--------------------+/                                          <* 
 *>    for (i = 0; i < x_size; ++i) {                                                    <* 
 *>       /+---(defaults)--------------------+/                                          <* 
 *>       IF_COL    x_wide = DEF_WIDTH;                                                  <* 
 *>       ELSE_ROW  x_wide = DEF_HEIGHT;                                                 <* 
 *>       /+---(get cell)--------------------+/                                          <* 
 *>       IF_COL    x_curr = LOC_cell_at_loc (CTAB, i, CROW);                            <* 
 *>       ELSE_ROW  x_curr = LOC_cell_at_loc (CTAB, CCOL, i);                            <* 
 *>       /+---(handle used)-----------------+/                                          <* 
 *>       if (x_curr != NULL) {                                                          <* 
 *>          rc = NODE_by_index (&x_node, CTAB, a_type, i);                              <* 
 *>          if (x_node != NULL)   x_wide = x_node->size;                                <* 
 *>          rc = yMAP_append (i, x_wide, 'Ï');                                          <* 
 *>       }                                                                              <* 
 *>       /+---(handle empty)----------------+/                                          <* 
 *>       else {                                                                         <* 
 *>          rc = yMAP_append (i, x_wide, '·');                                          <* 
 *>       }                                                                              <* 
 *>       /+---(done)------------------------+/                                          <* 
 *>    }                                                                                 <* 
 *>    /+---(update)-------------------------+/                                          <* 
 *>    rc = yMAP_finish ();                                                              <* 
 *>    /+---(complete)-----------------------+/                                          <* 
 *>    DEBUG_MAP    yLOG_exit    (__FUNCTION__);                                         <* 
 *>    return rc;                                                                        <* 
 *> }                                                                                    <*/


char
MAP_mapper           (char a_req)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   char        t           [LEN_RECD];
   ushort      b, c, e;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   DEBUG_MAP    yLOG_char    ("a_req"     , a_req);
   /*---(just display)---------*/
   if (a_req == YMAP_DISPONLY) {
      MAP_universe (a_req);
      MAP_columns ();
      MAP_rows    ();
      DEBUG_MAP    yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(universe change)------*/
   if (a_req == YMAP_UNIVERSE) {
      MAP_universe (a_req);
      ++CCOL;
      MAP_columns  ();
      ++CROW;
      MAP_rows     ();
      DEBUG_MAP    yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_req == YMAP_UNIVDISP) {
      MAP_universe (a_req);
   }
   /*---(initialization)-------*/
   /*> if (a_req == YMAP_INIT) {                                                      <* 
    *>    TAB_mapper       (a_req);                                                   <* 
    *>    COL_mapper       (a_req);                                                   <* 
    *>    ROW_mapper       (a_req);                                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(everything)-----------*/
   if (a_req == YMAP_INIT) {
      DEBUG_MAP    yLOG_note    ("full initialization");
      TAB_mapper       ();
      COL_mapper       (0);
      ROW_mapper       (0);
   }
   /*---(tab)------------------*/
   if (a_req == YMAP_INIT) {
      DEBUG_MAP    yLOG_note    ("full initialization, map tab");
      TAB_mapper       ();
   } else {
      rc = yMAP_axis_grid (YMAP_UNIV , &b, &c, &e, NULL);
      DEBUG_MAP    yLOG_complex ("TABS"      , "%4d § %4dn, %4db, %4dc, %4de § %4db, %4dc, %4de", rc, NTAB, 0, CTAB, 0, b, c, e);
      if (CTAB == c)  {
         DEBUG_MAP    yLOG_note    ("tab did not change, do not remap");
      } else {
         DEBUG_MAP    yLOG_note    ("tab change, remap");
         a_req = YMAP_INIT;
         TAB_mapper       ();
      }
   }
   /*---(col)------------------*/
   if (a_req == YMAP_INIT) {
      DEBUG_MAP    yLOG_note    ("full initialization, map col");
      ROW_mapper       (0);
   } else {
      rc = yMAP_axis_grid (YMAP_XAXIS, &b, &c, &e, NULL);
      DEBUG_MAP    yLOG_complex ("COLS"      , "%4d § %4dn, %4db, %4dc, %4de § %4db, %4dc, %4de", rc, NCOL, BCOL, CCOL, ECOL, b, c, e);
      if (CCOL == c) {
         DEBUG_MAP    yLOG_note    ("col did not change, do not remap");
      } else {
         DEBUG_MAP    yLOG_note    ("col change, remap");
         ROW_mapper (0);
      }
      BCOL   = b;
      CCOL   = c;
      ECOL   = e;
      DEBUG_MAP    yLOG_complex ("COLS+"     , "%4d § %4dn, %4db, %4dc, %4de § %4db, %4dc, %4de", rc, NCOL, BCOL, CCOL, ECOL, b, c, e);
   }
   /*---(row)------------------*/
   if (a_req == YMAP_INIT) {
      DEBUG_MAP    yLOG_note    ("full initialization, map row");
      COL_mapper       (0);
   } else {
      rc = yMAP_axis_grid (YMAP_YAXIS, &b, &c, &e, NULL);
      DEBUG_MAP    yLOG_complex ("ROWS"      , "%4d § %4dn, %4db, %4dc, %4de § %4db, %4dc, %4de", rc, NROW, BROW, CROW, EROW, b, c, e);
      if (CROW == c) {
         DEBUG_MAP    yLOG_note    ("row did not change, do not remap");
      } else {
         DEBUG_MAP    yLOG_note    ("row change, remap");
         COL_mapper (0);
      }
      BROW   = b;
      CROW   = c;
      EROW   = e;
      DEBUG_MAP    yLOG_complex ("ROWS+"     , "%4d § %4dn, %4db, %4dc, %4de § %4db, %4dc, %4de", rc, NROW, BROW, CROW, EROW, b, c, e);
   }
   /*---(get current)----------*/
   x_curr = LOC_cell_at_curr ();
   if  (yMODE_curr () == UMOD_WANDER) {
      ;;
   } else if (x_curr == NULL || x_curr->source == NULL) {
      DEBUG_MAP    yLOG_complex ("CURRENT"   , "%4dt, %4dc, %4dr", CTAB, CCOL, CROW);
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
   } else {
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
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    unit testing accessor                     ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;}

char*        /*-> unit test accessor -----------------[ leaf   [gs.930.201.A0]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
api_yvikeys__unit      (char *a_question, int a_num)
{
   /*---(prepare)------------------------*/
   strcpy  (unit_answer, "s_move unit      : question not understood");
   /*---(defenses)-----------------------*/
   if      (a_num < 0) {
      snprintf (unit_answer, LEN_FULL, "s_move error     : tab ref (%d) is negative", a_num);
   }
   else if (a_num >= MAX_TABS) {
      snprintf (unit_answer, LEN_FULL, "s_move error     : tab ref (%d vs %d) is too high", a_num, MAX_TABS);
   }
   /*---(screen location)----------------*/
   else if (strcmp(a_question, "row_stats"     ) == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move row stat  : num=%4d, scr=%4d, beg=%4d, cur=%4d, end=%4d", NROW, my.y_avail, BROW, CROW, EROW);
   }
   else if (strcmp(a_question, "col_stats"     ) == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move col stat  : num=%4d, scr=%4d, beg=%4d, cur=%4d, end=%4d", NCOL, my.x_avail, BCOL, CCOL, ECOL);
   }
   /*---(current position)---------------*/
   else if (strcmp(a_question, "tab_cur"       ) == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move tab cur   : tab=%4d, col=%4d, row=%4d", CTAB, CCOL, CROW);
   }
   /*> else if (strcmp(a_question, "tab_beg"       ) == 0) {                                                                                  <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab beg   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].bcol, s_tabs [a_num].brow);   <* 
    *> }                                                                                                                                      <*/
   /*> else if (strcmp(a_question, "tab_pos"       ) == 0) {                                                                                  <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab pos   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ccol, s_tabs [a_num].crow);   <* 
    *> }                                                                                                                                      <*/
   /*> else if (strcmp(a_question, "tab_max" )       == 0) {                                                                                  <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab max   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ncol, s_tabs [a_num].nrow);   <* 
    *> }                                                                                                                                      <*/
   else if (strcmp(a_question, "edit_con")       == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move edit con  : :%-*.*s:", my.apos, my.apos, g_contents + my.bpos);
   }
   else if (strcmp(a_question, "edit_pos")       == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move edit pos  : n=%3d, a=%3d, b=%3d, c=%3d, e=%3d", my.npos, my.apos, my.bpos, my.cpos, my.epos);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



