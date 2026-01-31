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


static char    s_label     [LEN_RECD]   = "";


char
LOC_variable            (char *a_name)
{
   char        rce         =  -10;
   char        rc          =    0;
   char        x_target    [LEN_LABEL] = "";
   char        x_label     [LEN_LABEL] = "";
   int         u, x, y;
   --rce;  if (a_name == NULL)  return rce;
   rc = yCALC_variable (a_name, x_target, x_label);
   --rce;  if (rc < 0)  return rce;
   rc = ystr2gyges  (x_target, &u, &x, &y, NULL, NULL, 0, YSTR_USABLE);
   --rce;  if (rc < 0)  return rce;
   rc = yMAP_jump (u, x, y, 0);
   rce;  if (rc < 0)  return rce;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char         /*-> initialize location function -------[ shoot  [gz.421.001.02]*/ /*-[00.0000.102.!]-*/ /*-[--.---.---.--]-*/
LOC_init             (void)
{
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(clean tabs)---------------------*/
   /*> LOC__purge    ();                                                              <*/
   /*---(update tab)---------------------*/
   CTAB      = -1;
   TAB_retrieve ();
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clean all tab and sheet locations --[ ------ [fz.A52.021.03]*/ /*-[01.0000.023.!]-*/ /*-[--.---.---.--]-*/
LOC_purge            (void)
{
   /*---(locals)-----------+-----------+-*/
   tTAB       *x_tab       = NULL;
   char        x_seq       =   -1;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(initialize s_tabs)----------------*/
   rc = TAB_by_cursor (&x_tab, &x_seq, YDLST_HEAD);
   while (x_tab != NULL && rc == 0) {
      DEBUG_PROG   yLOG_value   ("tab"       , x_tab->tab);
      rc = TAB_cleanse   (x_tab);
      DEBUG_PROG   yLOG_value   ("cleanse"   , rc);
      rc = TAB_by_cursor (&x_tab, &x_seq, YDLST_NEXT);
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> wrap up location function ----------[ shoot  [gc.211.000.01]*/ /*-[00.0000.102.!]-*/ /*-[--.---.---.--]-*/
LOC_wrap             (void)
{
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   LOC_purge  ();
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     hooking and unhooking                    ----===*/
/*====================------------------------------------====================*/
static void  o___HOOKING_________o () { return; }

char         /*-> attach a cell to a location --------[ ------ [ge.E63.137.82]*/ /*-[01.0000.115.J]-*/ /*-[--.---.---.--]-*/
LOC_hook           (tCELL *a_cell, char a_tab, short a_col, short a_row)
{  /*---(design notes)--------------------------------------------------------*/
   /* add the cell reference to an appropriate empty location.  nothing else  */
   /* on the tab or cell should be modified.                                  */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_label     [LEN_LABEL];
   tCELL      *x_curr      = NULL;
   tTAB       *x_tab       = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "cell %-10.10p, %2dt, %3dc, %4dr", a_cell, a_tab, a_col, a_row);
   /*---(defense)------------------------*/
   --rce;  if (a_cell == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check unhooked)-----------------*/
   --rce;  if (a_cell->d_tab != UNHOOKED || a_cell->d_col != UNHOOKED || a_cell->d_row != UNHOOKED) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check its legal)----------------*/
   rc = ystr4gyges (a_tab, a_col, a_row, 0, 0, x_label, YSTR_USABLE);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for fixed tab)------------*/
   rc = TAB_pointer (&x_tab, a_tab);
   --rce;  if (rc < 0 || x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (x_tab->type == 'f' && (x_tab->ncol < a_col || x_tab->nrow < a_row)) {
      DEBUG_LOCS   yLOG_note    ("fixed tab and can not fit in bounds");
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check unoccupied)---------------*/
   rc = api_ysort_by_coord (&x_curr, a_tab, a_col, a_row);
   DEBUG_LOCS   yLOG_point   ("a_curr"    , x_curr);
   --rce;  if (x_curr != NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(assign)-------------------------*/
   rc = TAB_hook_cell (&x_tab, a_tab, a_cell);
   DEBUG_LOCS   yLOG_value   ("TAB hook"  , rc);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (rc < 0 || x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = COL_hook_cell (x_tab, a_col, a_row, a_cell);
   DEBUG_LOCS   yLOG_value   ("COL hook"  , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = ROW_hook_cell (x_tab, a_col, a_row, a_cell);
   DEBUG_LOCS   yLOG_value   ("ROW hook"  , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label and key)------------------*/
   if (a_cell->d_label != g_tbd)  free (a_cell->d_label);
   a_cell->d_label = strdup (x_label);
   a_cell->d_key   = api_ysort_label2key (x_label);
   DEBUG_LOCS   yLOG_complex ("DEBUG 4"   , "%-10.10s, %2dt, %3dc, %4dr", a_cell->d_label, a_cell->d_tab, a_cell->d_col, a_cell->d_row);
   rc = api_ysort_update ();
   DEBUG_LOCS   yLOG_value   ("ysort"     , rc);
   /*---(adjust tab)--------------------*/
   TAB_resize (a_tab, "");
   /*> if (x_tab->d_type == G_TAB_AUTO) {                                               <* 
    *>    rc = COL_max_adjust (a_tab);                                                <* 
    *>    DEBUG_LOCS   yLOG_value   ("COL max"   , rc);                               <* 
    *>    rc = ROW_max_adjust (a_tab);                                                <* 
    *>    DEBUG_LOCS   yLOG_value   ("ROW max"   , rc);                               <* 
    *> }                                                                              <*/
   /*---(default printable)-------------*/
   api_ycalc_printer (a_cell);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> detach a cell from location --------[ ------ [ge.F74.167.71]*/ /*-[01.0000.214.7]-*/ /*-[--.---.---.--]-*/
LOC_unhook         (tCELL *a_cell)
{  /*---(design notes)--------------------------------------------------------*/
   /* remove the cell reference from the location and mark it as              */
   /* locationless.  nothing else on the tab or cell will be modified.        */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_tab       =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_point   ("a_cell"    , a_cell);
   /*---(defense: valid cell)------------*/
   --rce;  if (a_cell == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense: already unhooked)------*/
   DEBUG_LOCS   yLOG_complex ("coord"     , "%2dt, %3dc, %4dr", a_cell->d_tab, a_cell->d_col, a_cell->d_row);
   --rce;  if (a_cell->d_tab == UNHOOKED || a_cell->d_col == UNHOOKED || a_cell->d_row == UNHOOKED) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   x_tab = a_cell->d_tab;
   rc = TAB_unhook_cell (a_cell);
   DEBUG_LOCS   yLOG_value   ("TAB unhook", rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = COL_unhook_cell (a_cell);
   DEBUG_LOCS   yLOG_value   ("COL unhook", rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = ROW_unhook_cell (a_cell);
   DEBUG_LOCS   yLOG_value   ("ROW unhook", rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label and key)------------------*/
   if (a_cell->d_label != g_tbd)  free (a_cell->d_label);
   a_cell->d_label = g_tbd;
   a_cell->d_key   = UNHOOKED;
   api_ysort_update ();
   /*---(adjust tab)--------------------*/
   TAB_resize (x_tab, "");
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> move a cell between locations ------[ ------ [ge.930.134.55]*/ /*-[01.0000.00#.J]-*/ /*-[--.---.---.--]-*/
LOC_move           (
      /*----------+-----------+-----------------------------------------------*/
      char        a_stab,         /* tab of source cell                       */
      short       a_scol,         /* col of source cell                       */
      short       a_srow,         /* row of source cell                       */
      char        a_dtab,         /* tab of target cell                       */
      short       a_dcol,         /* col of target cell                       */
      short       a_drow)         /* row of target cell                       */
{  /*---(design notes)--------------------------------------------------------*/
   /* changes the location of the cell at the source location to the target   */
   /* location.  if there is a cell already at the target, it is deleted.  if */
   /* there is nothing at the source, the target is wiped anyway as this is   */
   /* consistent with selection cut, copy, and paste.                         */
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_src       = NULL;
   tCELL      *x_dst       = NULL;
   char        rce         = -10;           /* return code for errors         */
   char        rc          =   0;
   char        x_label     [LEN_LABEL];
   /*---(defense: source)----------------*/
   rc = ystr4gyges (a_stab, a_scol, a_srow, 0, 0, x_label, YSTR_USABLE);
   DEBUG_LOCS   yLOG_value   ("ystr4gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = api_ysort_by_coord (&x_src, a_stab, a_scol, a_srow);
   DEBUG_LOCS   yLOG_point   ("x_src"     , x_src);
   /*---(defense: target)----------------*/
   rc = ystr4gyges (a_dtab, a_dcol, a_drow, 0, 0, x_label, YSTR_USABLE);
   DEBUG_LOCS   yLOG_value   ("ystr4gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_dst = api_ysort_by_coord (&x_dst, a_dtab, a_dcol, a_drow);
   /*---(overwrite as necessary)---------*/
   if (x_dst != NULL) {
      CELL_change (NULL, YMAP_NONE, a_dtab, a_dcol, a_drow, "");
   }
   /*---(set location)-------------------*/
   if (x_src != NULL) {
      LOC_unhook  (x_src);
      LOC_hook    (x_src, a_dtab, a_dcol, a_drow);
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          locations                           ----===*/
/*====================------------------------------------====================*/
static void  o___LOCATION________o () { return; }

char
loc__checker_legal      (int a_tab, int a_col, int a_row)
{
   /*---(locals)-----------+-----------+-*/
   char          rce       =  -10;          /* rcode for errors               */
   /*---(header)----------------------*/
   DEBUG_LOCS     yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS     yLOG_note    ("checking for legal, no expansion allowed (LEGAL)");
   /*---(tab)-------------------------*/
   --rce;  if (!LEGAL_TAB (a_tab)) {
      DEBUG_LOCS     yLOG_note    ("tab not legal");
      DEBUG_LOCS     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(col)-------------------------*/
   --rce;  if (!LEGAL_COL (a_tab, a_col)) {
      DEBUG_LOCS     yLOG_note    ("col not legal");
      DEBUG_LOCS     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(row)-------------------------*/
   --rce;  if (!LEGAL_ROW (a_tab, a_row)) {
      DEBUG_LOCS     yLOG_note    ("row not legal");
      DEBUG_LOCS     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(done)------------------------*/
   DEBUG_LOCS     yLOG_exit    (__FUNCTION__);
   return 0;
}

char
loc__checker_adapt      (int a_tab, int a_col, int a_row)
{
   /*> /+---(locals)-----------+-----------+-+/                                             <* 
    *> char          rce       =  -10;          /+ rcode for errors               +/        <* 
    *> /+---(header)----------------------+/                                                <* 
    *> DEBUG_LOCS     yLOG_enter   (__FUNCTION__);                                          <* 
    *> DEBUG_LOCS     yLOG_note    ("checking for legal, but expansion allowed (ADAPT)");   <* 
    *> /+---(tab)-------------------------+/                                                <* 
    *> if (a_tab > TAB_max ()) {                                                            <* 
    *>    TAB_setmax (a_tab + 1);                                                           <* 
    *>    DEBUG_LOCS     yLOG_value   ("tab max"   , TAB_max ());                           <* 
    *> }                                                                                    <* 
    *> /+---(col)-------------------------+/                                                <* 
    *> if (a_col >= COL_max (a_tab)) {                                                      <* 
    *>    COL_max_set (a_tab, a_col + 1);                                                    <* 
    *>    DEBUG_LOCS     yLOG_value   ("col max"   , COL_max (a_tab));                      <* 
    *> }                                                                                    <* 
    *> /+---(row)-------------------------+/                                                <* 
    *> if (a_row >= ROW_max (a_tab)) {                                                      <* 
    *>    ROW_max_set (a_tab, a_row + 1);                                                    <* 
    *>    DEBUG_LOCS     yLOG_value   ("row max"   , ROW_max (a_tab));                      <* 
    *> }                                                                                    <* 
    *> /+---(done)------------------------+/                                                <* 
    *> DEBUG_LOCS     yLOG_exit    (__FUNCTION__);                                          <* 
    *> return 0;                                                                            <*/
}

char
loc__checker_exact      (int a_tab, int a_col, int a_row)
{
   /*> /+---(locals)-----------+-----------+-+/                                        <* 
    *> char          rce       =  -10;                                                 <* 
    *> int           x_max     =    0;                                                 <* 
    *> /+---(header)----------------------+/                                           <* 
    *> DEBUG_LOCS     yLOG_enter   (__FUNCTION__);                                     <* 
    *> DEBUG_LOCS     yLOG_note    ("checking for legal with exact update (EXACT)");   <* 
    *> /+---(tab)-------------------------+/                                           <* 
    *> if (a_tab > TAB_max ()) {                                                       <* 
    *>    TAB_setmax (a_tab + 1);                                                      <* 
    *>    DEBUG_LOCS     yLOG_value   ("tab max"   , TAB_max ());                      <* 
    *> }                                                                               <* 
    *> /+---(col)-------------------------+/                                           <* 
    *> x_max = COL_max_used (a_tab);                                                    <* 
    *> if (a_col < x_max)  a_col = x_max;                                              <* 
    *> COL_max_set (a_tab, a_col + 1);                                                  <* 
    *> DEBUG_LOCS     yLOG_value   ("col max"   , COL_max (a_tab));                    <* 
    *> /+---(row)-------------------------+/                                           <* 
    *> x_max = ROW_max_used (a_tab);                                                    <* 
    *> if (a_row < x_max)  a_row = x_max;                                              <* 
    *> ROW_max_set (a_tab, a_row + 1);                                                  <* 
    *> DEBUG_LOCS     yLOG_value   ("row max"   , ROW_max (a_tab));                    <* 
    *> /+---(done)------------------------+/                                           <* 
    *> DEBUG_LOCS     yLOG_exit    (__FUNCTION__);                                     <* 
    *> return 0;                                                                       <*/
}

char
LOC_checker        (int a_tab, int a_col, int a_row, int a_nada, char a_check)
{
   /*
    *  all values are checked for validity before calling checker.
    *
    */
   /*---(locals)-----------+-----------+-*/
   char          rc        =    0;
   static int    x_xtab    = -666;
   static int    x_xcol    = -666;
   static int    x_xrow    = -666;
   static char   x_check   =  '-';
   static char   x_xrc     =  -66;
   static int    x_total   =    0;
   static int    x_xshort  =    0;
   /*---(short-cut)----------------------*/
   ++x_total;
   if (x_xtab == a_tab && x_xcol == a_col && x_xrow == a_row &&  x_check == a_check) {
      ++x_xshort;
      DEBUG_LOCS     yLOG_senter  (__FUNCTION__);
      DEBUG_LOCS     yLOG_snote   ("same as last search");
      DEBUG_LOCS     yLOG_sint    (x_xrc);
      DEBUG_LOCS     yLOG_sint    (x_total);
      DEBUG_LOCS     yLOG_sint    (x_xshort);
      DEBUG_LOCS     yLOG_sexit   (__FUNCTION__);
      return x_xrc;
   }
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("location"  , "%2db, %3dx, %4dy, %4dz, %c", a_tab, a_col, a_row, a_nada, a_check);
   /*---(save values)--------------------*/
   x_xtab  = a_tab;
   x_xcol  = a_col;
   x_xrow  = a_row;
   x_check = a_check;
   /*---(simple legal check)-------------*/
   switch (a_check) {
   case YSTR_LEGAL :
      rc = loc__checker_legal (a_tab, a_col, a_row);
      break;
   case YSTR_ADAPT :
      rc = loc__checker_adapt (a_tab, a_col, a_row);
      break;
   case YSTR_EXACT :
      rc = loc__checker_exact (a_tab, a_col, a_row);
      break;
   }
   DEBUG_LOCS   yLOG_value   ("rc"        , rc);
   x_xrc = rc;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return rc;
}

tCELL*       /*-> return the cell at a location ------[ ------ [gp.522.323.11]*/ /*-[01.0000.#14.#]-*/ /*-[--.---.---.--]-*/
LOC_cell_at_loc      (int a_tab, int a_col, int a_row)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   /*---(begin)--------------------------*/
   DEBUG_LOCS_M   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   rc = api_ysort_by_coord (&x_curr, a_tab, a_col, a_row);
   if (rc < 0)  {
      DEBUG_LOCS_M   yLOG_note    ("nothing found");
      DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   DEBUG_LOCS_M   yLOG_point   ("found"     , x_curr);
   /*---(complete)-----------------------*/
   DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);
   return x_curr;
}

tCELL*       /*-> return the cell at current loc -----[ ------ [gp.522.023.11]*/ /*-[01.0000.603.!]-*/ /*-[--.---.---.--]-*/
LOC_cell_at_curr     (void)
{
   return LOC_cell_at_loc (CTAB, CCOL, CROW);
}

tCELL*       /*-> return the cell at a location ------[ ------ [gp.632.154.22]*/ /*-[01.0000.106.T]-*/ /*-[--.---.---.--]-*/
LOC_cell_labeled   (char *a_label)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   int         x_tab       =   0;           /* working tab value              */
   int         x_col       =   0;           /* working col value              */
   int         x_row       =   0;           /* working row value              */
   /*---(defenses)-----------------------*/
   rc = ystr2gyges (a_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_USABLE);
   if (rc < 0)    return NULL;
   return LOC_cell_at_loc (x_tab, x_col, x_row);
}



/*====================------------------------------------====================*/
/*===----                        cell references                       ----===*/
/*====================------------------------------------====================*/
static void  o___REFERENCES______o () { return; }

char         /*-> convert a cell into a label --------[ ------ [ge.932.136.31]*/ /*-[01.0000.116.I]-*/ /*-[--.---.---.--]-*/
LOC_label         (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* cell to be converted                         */
      char       *a_final)    /* variable to store location reference         */
{  /*---(design notes)-------------------*//*---------------------------------*/
   /* function purely and simply creates labels for actual cells.  since its  */
   /* all about a cell, if the cell is non-existant or not hooked to a sheet, */
   /* it returns an error (neg).  if it is hooked in an illegal location, it  */
   /* returns an warning (pos).                                               */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(defense: valid output)----------*/
   --rce;  if (a_final        == NULL) return rce;
   ystrlcpy (a_final, ""           , LEN_LABEL);
   --rce;  if (a_cell         == NULL) return rce;
   --rce;  if (a_cell->d_label  == NULL) return rce;
   /*---(copy label)---------------------*/
   ystrlcpy (a_final, a_cell->d_label, LEN_LABEL);
   /*---(complete)-----------------------*/
   return  0;
}

char         /*-> convert a cell into a coordinates --[ leaf   [ge.520.127.40]*/ /*-[01.0000.409.I]-*/ /*-[--.---.---.--]-*/
LOC_coords        (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* cell to be converted                         */
      int        *a_tab,      /* return variable for the tab                  */
      int        *a_col,      /* return variable for the column               */
      int        *a_row)      /* return variable for the row                  */
{  /*---(design notes)-------------------*//*---------------------------------*/
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   char        rc          =   0;           /* return code                    */
   /*---(defense: valid cell)------------*/
   --rce;  if (a_cell  == NULL)             return rce;
   /*---(fiugure out reference)----------*/
   if (a_tab != NULL)  *a_tab  = a_cell->d_tab;
   if (a_col != NULL)  *a_col  = a_cell->d_col;
   if (a_row != NULL)  *a_row  = a_cell->d_row;
   /*---(complete)-----------------------*/
   return  0;
}



/*====================------------------------------------====================*/
/*===----                        unit testing                          ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*        /*-> unit test accessor -----------------[ light  [us.B60.2A3.F2]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
LOC__unit          (char *a_question, char *a_label)
{
   /*---(locals)-------------------------*/
   char        rc          =    0;
   int         i           =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   char        x_abs       =    0;
   char        x_label     =  '-';
   char        x_beg       [LEN_LABEL]   = "";
   char        x_end       [LEN_LABEL]   = "";
   char        x_cur       [LEN_LABEL]   = "";
   char        x_max       [LEN_LABEL]   = "";
   /*---(parse location)-----------------*/
   strcpy  (unit_answer, "LOC              : label could not be parsed");
   if (a_label != NULL && strcmp (a_label, "") != 0) {
      x_label = a_label [0];
      rc = ystr2gyges  (a_label, &x_tab, &x_col, &x_row, NULL, &x_abs, 0, YSTR_USABLE);
   } else {
      x_tab   = CTAB;
      x_label = LABEL_tab (x_tab);
      x_col   = CCOL;
      x_row   = CROW;
      x_abs   = 0;
   }
   if (rc <  0)  return unit_answer;
   /*> DEBUG_LOCS   yLOG_complex  ("address"   , "%2dt, %3dc, %3dr", CTAB, CCOL, CROW);   <*/
   /*> DEBUG_LOCS   yLOG_complex  ("column"    , "%3db, %3de, %3dc, %3dn", s_tabs [x_tab].bcol, s_tabs [x_tab].ecol, s_tabs [x_tab].ccol, s_tabs [x_tab].ncol - 1);   <*/
   /*> DEBUG_LOCS   yLOG_complex  ("row"       , "%3db, %3de, %3dc, %3dn", s_tabs [x_tab].brow, s_tabs [x_tab].erow, s_tabs [x_tab].crow, s_tabs [x_tab].nrow - 1);   <*/
   /*---(prepare data)-------------------*/
   strcpy  (unit_answer, "LOC              : locations could not be prepared");
   /*> if (rc >= 0)  rc = ystr4gyges  (x_tab, s_tabs [x_tab].bcol, s_tabs [x_tab].brow, 0, 0, x_beg, YSTR_CHECK);           <* 
    *> if (rc >= 0)  rc = ystr4gyges  (x_tab, s_tabs [x_tab].ecol, s_tabs [x_tab].erow, 0, 0, x_end, YSTR_CHECK);           <* 
    *> if (rc >= 0)  rc = ystr4gyges  (x_tab, s_tabs [x_tab].ccol, s_tabs [x_tab].crow, 0, 0, x_cur, YSTR_CHECK);           <* 
    *> if (rc >= 0)  rc = ystr4gyges  (x_tab, s_tabs [x_tab].ncol - 1, s_tabs [x_tab].nrow - 1, 0, 0, x_max, YSTR_CHECK);   <* 
    *> if (rc <  0)  return unit_answer;                                                                                   <*/
   /*---(overall)------------------------*/
   strcpy  (unit_answer, "LOC              : question not understood");
   /*> if      (strcmp(a_question, "tab_info"      ) == 0) {                                                                                                                                <* 
    *>    snprintf(unit_answer, LEN_FULL, "LOC tab info (%c) : %-12.12s %-7.7s %-7.7s %-7.7s %-7.7s %d", x_label, s_tabs [x_tab].name, x_beg, x_end, x_cur, x_max, s_tabs [x_tab].count);   <* 
    *> }                                                                                                                                                                                    <*/
   /*> else if (strcmp(a_question, "cell_size"     ) == 0) {                                                                                           <* 
    *>    snprintf(unit_answer, LEN_FULL, "LOC cell size    : width=%3d, height=%3d", s_tabs [x_tab].cols [x_col].w, s_tabs [x_tab].rows [x_row].h);   <* 
    *> }                                                                                                                                               <*/
   /*> else if (strcmp(a_question, "loc_who"       )  == 0) {                                                                                                     <* 
    *>    snprintf (unit_answer, LEN_FULL, "LOC occupant/who : ptr=%10p, tab=%4d, col=%4d, row=%4d", s_tabs [x_tab].sheet [x_col][x_row], x_tab, x_col, x_row);   <* 
    *> }                                                                                                                                                          <*/
   /*> else if (strcmp(a_question, "loc_col"       )  == 0) {                                                                                                                      <* 
    *>    snprintf (unit_answer, LEN_FULL, "LOC col stats    : tab=%4d, col=%4d, width =%4d, used=%4d", x_tab, x_col, s_tabs[x_tab].cols[x_col].w, s_tabs[x_tab].cols[x_col].c);   <* 
    *> }                                                                                                                                                                           <*/
   /*> else if (strcmp(a_question, "loc_row"       )  == 0) {                                                                                                                      <* 
    *>    snprintf (unit_answer, LEN_FULL, "LOC row stats    : tab=%4d, row=%4d, height=%4d, used=%4d", x_tab, x_row, s_tabs[x_tab].rows[x_row].h, s_tabs[x_tab].rows[x_row].c);   <* 
    *> }                                                                                                                                                                           <*/
   /*> else if (strcmp(a_question, "tab_def"       )  == 0) {                                               <* 
    *>    snprintf (unit_answer, LEN_FULL, "LOC tab defaults : col=%2d, row=%2d", DEF_WIDTH, DEF_HEIGHT);   <* 
    *> }                                                                                                    <*/
   /*> else if (strcmp(a_question, "tab_beg"       ) == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab beg   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].bcol, s_tabs [a_num].brow);   <* 
    *> }                                                                                                                                  <* 
    *> else if (strcmp(a_question, "tab_pos"       ) == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab pos   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ccol, s_tabs [a_num].crow);   <* 
    *> }                                                                                                                                  <* 
    *> else if (strcmp(a_question, "tab_max" )       == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab max   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ncol, s_tabs [a_num].nrow);   <* 
    *> }                                                                                                                                  <*/
   /*---(complete)-----------------------*/
   return unit_answer;
}

char*        /*-> unit test accessor -----------------[ light  [us.D60.241.J0]*/ /*-[02.0000.00#.T]-*/ /*-[--.---.---.--]-*/
LOC__unit_OLD      (char *a_question, tCELL *a_cell)
{
   /*---(locals)-----------+-----------+-*/
   int         rc          = -1;
   int         i           =  0;
   tCELL      *x_curr      = NULL;
   char        t           [LEN_FULL]   = "";
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_loc            : question not understood");
   /*---(find cell by pointer)-----------*/
   x_curr  = hcell;
   for (i = 0; i < NCEL; ++i) {
      if (x_curr == NULL)     break;
      if (x_curr == a_cell) { rc = 0; break; }
      x_curr = x_curr->d_next;
   }
   if (rc < 0)   x_curr = NULL;
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "loc_cell"     )  == 0) {
      if        (a_cell == NULL) {
         snprintf (unit_answer, LEN_FULL, "s_loc cell       : requested a null cell");
      } else if (x_curr == NULL) {
         snprintf (unit_answer, LEN_FULL, "s_loc cell       : ptr=%9p, cell not found in list"   , a_cell);
      } else {
         snprintf (unit_answer, LEN_FULL, "s_loc cell       : ptr=%9p, tab=%4d, col=%4d, row=%4d", x_curr, x_curr->d_tab, x_curr->d_col, x_curr->d_row);
      }
   }
   else if (strcmp(a_question, "loc_who"       )  == 0) {
      if (a_cell != NULL) {
         snprintf (unit_answer, LEN_FULL, "s_loc occupant   : ptr=%9p, tab=%4d, col=%4d, row=%4d", x_curr, x_curr->d_tab, x_curr->d_col, x_curr->d_row);
      } else {
         snprintf (unit_answer, LEN_FULL, "s_loc occupant   : ptr=%9p, no cell attached", a_cell);
      }
   }
   else if (strcmp (a_question, "cell_where")     == 0) {
      if (a_cell == NULL) {

      } else if (rc == 0) {
         snprintf (unit_answer, LEN_FULL, "s_cell location  : tab=%4d, col=%4d, row=%4d", x_curr->d_tab, x_curr->d_col, x_curr->d_row);
      } else {
         snprintf (unit_answer, LEN_FULL, "s_cell location  : not found in cell list");
      }
   }
   else if (strcmp(a_question, "cell_list")      == 0) {
      snprintf(unit_answer, LEN_FULL, "s_cell main list : num=%4d, head=%9p, tail=%9p", NCEL, hcell, tcell);
   }
   else if (strcmp(a_question, "loc_label")      == 0) {
      snprintf(unit_answer, LEN_FULL, "s_loc label      : %s", s_label);
   }
   /*> else if (strcmp(a_question, "tab_beg"       ) == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab beg   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].bcol, s_tabs [a_num].brow);   <* 
    *> }                                                                                                                                  <* 
    *> else if (strcmp(a_question, "tab_pos"       ) == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab pos   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ccol, s_tabs [a_num].crow);   <* 
    *> }                                                                                                                                  <* 
    *> else if (strcmp(a_question, "tab_max" )       == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab max   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ncol, s_tabs [a_num].nrow);   <* 
    *> }                                                                                                                                  <*/
   /*---(complete)-----------------------*/
   return unit_answer;
}
