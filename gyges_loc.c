/*============================----beg-of-source---============================*/
#include   "gyges.h"


static char    s_label     [LEN_RECD]   = "";



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___PROGRAM_________o () { return; }

char         /*-> initialize location function -------[ shoot  [gz.421.001.02]*/ /*-[00.0000.102.!]-*/ /*-[--.---.---.--]-*/
LOC_init             (void)
{
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(clean tabs)---------------------*/
   LOC__purge    ();
   /*---(set defaults)-------------------*/
   NTAB      = DEF_TABS;
   /*---(update tab)---------------------*/
   CTAB      = 0;
   TAB_retrieve ();
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear all cell placements ----------[ leaf   [fe.833.133.20]*/ /*-[02.0000.014.!]-*/ /*-[--.---.---.--]-*/
LOC__clear_locs      (int a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_col       =   0;
   int         x_row       =   0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_senter  (__FUNCTION__);
   DEBUG_PROG   yLOG_svalue  ("a_tab"     , a_tab);
   /*---(defense)------------------------*/
   --rce;  if (a_tab <  0) {
      DEBUG_PROG   yLOG_snote   ("tab too small");
      DEBUG_PROG   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_tab >= MAX_TABS) {
      DEBUG_PROG   yLOG_snote   ("tab too big");
      DEBUG_PROG   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(unhook all cells)---------------*/
   /* tbd */
   /*---(initialize cells)---------------*/
   DEBUG_PROG   yLOG_snote   ("clear cells off sheet");
   DEBUG_PROG   yLOG_svalue  ("MAX_COLS"  , MAX_COLS);
   DEBUG_PROG   yLOG_svalue  ("MAX_ROWS"  , MAX_ROWS);
   for (x_col = 0; x_col < MAX_COLS; ++x_col) {
      for (x_row = 0; x_row < MAX_ROWS; ++x_row) {
         s_tabs [a_tab].sheet [x_col][x_row] = NULL;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> clean all tab and sheet locations --[ ------ [fz.A52.021.03]*/ /*-[01.0000.023.!]-*/ /*-[--.---.---.--]-*/
LOC__purge           (void)
{  /*---(design notes)--------------------------------------------------------*/
   /* run CELL_wrap/purge before LOC_wrap/purge so all cells are unhooked     */
   /*---(locals)-----------+-----------+-*/
   int         x_tab       = 0;
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(initialize s_tabs)----------------*/
   for (x_tab = 0; x_tab < MAX_TABS; ++x_tab) {
      DEBUG_PROG   yLOG_value   ("x_tab"     , x_tab);
      /*---(main config)-----------------*/
      DEBUG_PROG   yLOG_note    ("reset naming");
      sprintf (t, "tab_%02d", x_tab);
      strlcpy (s_tabs [x_tab].name, t, LEN_RECD);
      s_tabs [x_tab].tab     = x_tab;
      s_tabs [x_tab].type    = G_TAB_NORMAL;
      s_tabs [x_tab].c       =    0;
      s_tabs [x_tab].defwide = DEF_WIDTH;
      s_tabs [x_tab].deftall = DEF_HEIGHT;
      /*---(size limits)-----------------*/
      DEBUG_PROG   yLOG_note    ("reset default size");
      DEBUG_PROG   yLOG_value   ("DEF_COLS"  , DEF_COLS);
      DEBUG_PROG   yLOG_value   ("DEF_ROWS"  , DEF_ROWS);
      s_tabs [x_tab].ncol    = DEF_COLS;
      s_tabs [x_tab].nrow    = DEF_ROWS;
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
      /*---(initialize)------------------*/
      COL_clear         (x_tab);
      ROW_clear         (x_tab);
      LOC__clear_locs   (x_tab);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> wrap up location function ----------[ shoot  [gc.211.000.01]*/ /*-[00.0000.102.!]-*/ /*-[--.---.---.--]-*/
LOC_wrap             (void)
{
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   LOC__purge  ();
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     hooking and unhooking                    ----===*/
/*====================------------------------------------====================*/
PRIV void  o___HOOKING_________o () { return; }

char         /*-> attach a cell to a location --------[ ------ [ge.E63.137.82]*/ /*-[01.0000.115.J]-*/ /*-[--.---.---.--]-*/
LOC_hook           (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* cell to be placed in a location              */
      int         a_tab,      /* tab for target location                      */
      int         a_col,      /* col for target location                      */
      int         a_row)      /* row for target location                      */
{  /*---(design notes)--------------------------------------------------------*/
   /* add the cell reference to an appropriate empty location.  nothing else  */
   /* on the tab or cell should be modified.                                  */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_label     [LEN_LABEL];
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_point   ("*a_cell"   , a_cell);
   DEBUG_LOCS   yLOG_value   ("a_tab"     , a_tab);
   DEBUG_LOCS   yLOG_value   ("a_col"     , a_col);
   DEBUG_LOCS   yLOG_value   ("a_row"     , a_row);
   /*---(defense: valid cell)------------*/
   --rce;  if (a_cell == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense: already hooked)--------*/
   --rce;  if (a_cell->tab != UNHOOKED) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_cell->col != UNHOOKED) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_cell->row != UNHOOKED) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense: valid destination)-----*/
   rc = str4gyges (a_tab, a_col, a_row, 0, 0, x_label, YSTR_LEGAL);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense: already occupied)------*/
   x_curr = s_tabs[a_tab].sheet[a_col][a_row] != NULL;
   DEBUG_LOCS   yLOG_point   ("*a_curr"   , x_curr);
   --rce;  if (x_curr != NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(point cell at location)---------*/
   a_cell->tab = a_tab;
   a_cell->col = a_col;
   a_cell->row = a_row;
   str4gyges (a_tab, a_col, a_row, 0, 0, a_cell->label, YSTR_LEGAL);
   /*---(point location at cell)---------*/
   s_tabs [a_tab].sheet[a_col][a_row] = a_cell;
   /*> if (LOC_label (a_cell, a_cell->label) < 0) {                                   <* 
    *>    strcpy (a_cell->label, "tbd");                                              <* 
    *> }                                                                              <*/
   /*---(update totals)------------------*/
   ++s_tabs[a_tab].cols[a_col].c;
   ++s_tabs[a_tab].rows[a_row].c;
   ++s_tabs[a_tab].c;
   /*---(default printable)-------------*/
   api_ycalc_printer (a_cell);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> detach a cell from location --------[ ------ [ge.F74.167.71]*/ /*-[01.0000.214.7]-*/ /*-[--.---.---.--]-*/
LOC_unhook         (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell)     /* cell to detach from current location         */
{  /*---(design notes)--------------------------------------------------------*/
   /* remove the cell reference from the location and mark it as              */
   /* locationless.  nothing else on the tab or cell will be modified.        */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_label     [LEN_LABEL];
   int         x_tab       = UNHOOKED;
   int         x_col       = UNHOOKED;
   int         x_row       = UNHOOKED;
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_point   ("*a_cell"   , a_cell);
   /*---(defense: valid cell)------------*/
   --rce;  if (a_cell == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense: already unhooked)------*/
   DEBUG_LOCS   yLOG_value   ("->tab"     , a_cell->tab);
   --rce;  if (a_cell->tab == UNHOOKED) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_value   ("->col"     , a_cell->col);
   --rce;  if (a_cell->col == UNHOOKED) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_value   ("->row"     , a_cell->row);
   --rce;  if (a_cell->row == UNHOOKED) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   x_tab       = a_cell->tab;
   x_col       = a_cell->col;
   x_row       = a_cell->row;
   /*---(defense: valid position)--------*/
   rc = str4gyges (x_tab, x_col, x_row, 0, 0, x_label, YSTR_LEGAL);
   DEBUG_LOCS   yLOG_value   ("legal"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense: wrong cell)------------*/
   x_curr = s_tabs[x_tab].sheet[x_col][x_row];
   DEBUG_LOCS   yLOG_point   ("*a_curr"   , x_curr);
   --rce;  if (x_curr != a_cell) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update totals)------------------*/
   DEBUG_LOCS   yLOG_note    ("update totals");
   --s_tabs[x_tab].cols[x_col].c;
   --s_tabs[x_tab].rows[x_row].c;
   --s_tabs[x_tab].c;
   /*---(detach)-------------------------*/
   DEBUG_LOCS   yLOG_note    ("mark cell unhooked");
   a_cell->tab = UNHOOKED;
   a_cell->col = UNHOOKED;
   a_cell->row = UNHOOKED;
   strcpy (a_cell->label, "tbd");
   DEBUG_LOCS   yLOG_note    ("mark location unused");
   s_tabs [x_tab].sheet[x_col][x_row] = NULL;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> move a cell between locations ------[ ------ [ge.930.134.55]*/ /*-[01.0000.00#.J]-*/ /*-[--.---.---.--]-*/
LOC_move           (
      /*----------+-----------+-----------------------------------------------*/
      int         a_stab,         /* tab of source cell                       */
      int         a_scol,         /* col of source cell                       */
      int         a_srow,         /* row of source cell                       */
      int         a_ttab,         /* tab of target cell                       */
      int         a_tcol,         /* col of target cell                       */
      int         a_trow)         /* row of target cell                       */
{  /*---(design notes)--------------------------------------------------------*/
   /* changes the location of the cell at the source location to the target   */
   /* location.  if there is a cell already at the target, it is deleted.  if */
   /* there is nothing at the source, the target is wiped anyway as this is   */
   /* consistent with selection cut, copy, and paste.                         */
   /*---(locals)-----------+-----------+-*/
   tCELL      *source      = NULL;
   tCELL      *target      = NULL;
   char        rce         = -10;           /* return code for errors         */
   char        rc          =   0;
   char        x_label     [LEN_LABEL];
   /*---(defense: source)----------------*/
   rc = str4gyges (a_stab, a_scol, a_srow, 0, 0, x_label, YSTR_LEGAL);
   --rce;  if (rc < 0)  return rce;
   source      = s_tabs[a_stab].sheet[a_scol][a_srow];
   /*---(defense: target)----------------*/
   rc = str4gyges (a_ttab, a_tcol, a_trow, 0, 0, x_label, YSTR_LEGAL);
   --rce;  if (rc < 0)  return rce;
   target      = s_tabs[a_ttab].sheet[a_tcol][a_trow];
   /*---(overwrite as necessary)---------*/
   if (target  != NULL) {
      CELL_change (NULL, HIST_NONE, a_ttab, a_tcol, a_trow, "");
   }
   /*---(set location)-------------------*/
   if (source != NULL) {
      LOC_unhook  (source);
      LOC_hook    (source, a_ttab, a_tcol, a_trow);
   }
   /*---(update printable)---------------*/
   if (s_tabs[a_stab].cols[a_scol].w != s_tabs[a_ttab].cols[a_tcol].w) {
      api_ycalc_printer (source);
      /*> CELL_printable (source);                                                    <*/
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          locations                           ----===*/
/*====================------------------------------------====================*/
PRIV void  o___LOCATION________o () { return; }

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
   /*---(locals)-----------+-----------+-*/
   char          rce       =  -10;          /* rcode for errors               */
   /*---(header)----------------------*/
   DEBUG_LOCS     yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS     yLOG_note    ("checking for legal, but expansion allowed (ADAPT)");
   /*---(tab)-------------------------*/
   if (a_tab > TAB_max ()) {
      TAB_setmax (a_tab + 1);
      DEBUG_LOCS     yLOG_value   ("tab max"   , TAB_max ());
   }
   /*---(col)-------------------------*/
   if (a_col >= COL_max (a_tab)) {
      COL_setmax (a_tab, a_col + 1);
      DEBUG_LOCS     yLOG_value   ("col max"   , COL_max (a_tab));
   }
   /*---(row)-------------------------*/
   if (a_row >= ROW_max (a_tab)) {
      ROW_setmax (a_tab, a_row + 1);
      DEBUG_LOCS     yLOG_value   ("row max"   , ROW_max (a_tab));
   }
   /*---(done)------------------------*/
   DEBUG_LOCS     yLOG_exit    (__FUNCTION__);
   return 0;
}

char
loc__checker_exact      (int a_tab, int a_col, int a_row)
{
   /*---(locals)-----------+-----------+-*/
   char          rce       =  -10;
   int           x_max     =    0;
   /*---(header)----------------------*/
   DEBUG_LOCS     yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS     yLOG_note    ("checking for legal with exact update (EXACT)");
   /*---(tab)-------------------------*/
   if (a_tab > TAB_max ()) {
      TAB_setmax (a_tab + 1);
      DEBUG_LOCS     yLOG_value   ("tab max"   , TAB_max ());
   }
   /*---(col)-------------------------*/
   x_max = COL_maxused (a_tab);
   if (a_col < x_max)  a_col = x_max;
   COL_setmax (a_tab, a_col + 1);
   DEBUG_LOCS     yLOG_value   ("col max"   , COL_max (a_tab));
   /*---(row)-------------------------*/
   x_max = ROW_maxused (a_tab);
   if (a_row < x_max)  a_row = x_max;
   ROW_setmax (a_tab, a_row + 1);
   DEBUG_LOCS     yLOG_value   ("row max"   , ROW_max (a_tab));
   /*---(done)------------------------*/
   DEBUG_LOCS     yLOG_exit    (__FUNCTION__);
   return 0;
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

/*> char         /+-> verify that a location is legal ----[ leaf   [ge.QA9.49#.M0]+/ /+-[02.0000.C94.#]-+/ /+-[--.---.---.--]-+/   <* 
 *> LOC_legal          (int a_tab, int a_col, int a_row, char a_adapt)                                                             <* 
 *> {  /+---(design notes)--------------------------------------------------------+/                                               <* 
 *>    /+ tests the tab, col, and row against minimum and maximum limits as well  +/                                               <* 
 *>    /+ as against current limits as a service to other functions.              +/                                               <* 
 *>    /+ additionally, if requested, the function can expand the current limits  +/                                               <* 
 *>    /+ to include the requested row and column if it is within the max limits. +/                                               <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char          rce       =  -10;          /+ rcode for errors               +/                                               <* 
 *>    short         x_max     =    0;          /+ maximum used col/row in tab    +/                                               <* 
 *>    int           i         =    0;                                                                                             <* 
 *>    static int    x_xtab    = -666;                                                                                             <* 
 *>    static int    x_xcol    = -666;                                                                                             <* 
 *>    static int    x_xrow    = -666;                                                                                             <* 
 *>    static char   x_adapt   =  '-';                                                                                             <* 
 *>    static char   x_xrc     =  -66;                                                                                             <* 
 *>    static int    x_xtotal  =    0;                                                                                             <* 
 *>    static int    x_xshort  =    0;                                                                                             <* 
 *>    /+---(shortcut)-----------------------+/                                                                                    <* 
 *>    ++x_xtotal;                                                                                                                 <* 
 *>    if (x_xtab == a_tab && x_xcol == a_col && x_xrow == a_row &&  x_adapt == a_adapt) {                                         <* 
 *>       ++x_xshort;                                                                                                              <* 
 *>       DEBUG_LOCS_M   yLOG_senter  (__FUNCTION__);                                                                              <* 
 *>       DEBUG_LOCS_M   yLOG_snote   ("same as last search");                                                                     <* 
 *>       DEBUG_LOCS_M   yLOG_sint    (x_xrc);                                                                                     <* 
 *>       DEBUG_LOCS_M   yLOG_sint    (x_xtotal);                                                                                  <* 
 *>       DEBUG_LOCS_M   yLOG_sint    (x_xshort);                                                                                  <* 
 *>       DEBUG_LOCS_M   yLOG_sexit   (__FUNCTION__);                                                                              <* 
 *>       return x_xrc;                                                                                                            <* 
 *>    }                                                                                                                           <* 
 *>    /+---(begin)--------------------------+/                                                                                    <* 
 *>    DEBUG_LOCS_M   yLOG_enter   (__FUNCTION__);                                                                                 <* 
 *>    /+---(save values)--------------------+/                                                                                    <* 
 *>    x_xtab  = a_tab;                                                                                                            <* 
 *>    x_xcol  = a_col;                                                                                                            <* 
 *>    x_xrow  = a_row;                                                                                                            <* 
 *>    x_adapt = x_adapt;                                                                                                          <* 
 *>    /+---(check absolute boudaries)-------+/                                                                                    <* 
 *>    DEBUG_LOCS_M   yLOG_value   ("a_tab"     , a_tab);                                                                          <* 
 *>    --rce;  if (a_tab <  0       ) {                                                                                            <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("tab less than zero");                                                                      <* 
 *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
 *>       return x_xrc = rce;                                                                                                      <* 
 *>    }                                                                                                                           <* 
 *>    --rce;  if (a_tab >= MAX_TABS) {                                                                                            <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("tab greater than max");                                                                    <* 
 *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
 *>       return x_xrc = rce;                                                                                                      <* 
 *>    }                                                                                                                           <* 
 *>    DEBUG_LOCS_M   yLOG_value   ("a_col"     , a_col);                                                                          <* 
 *>    --rce;  if (a_col <  0       ) {                                                                                            <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("col less than zero");                                                                      <* 
 *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
 *>       return x_xrc = rce;                                                                                                      <* 
 *>    }                                                                                                                           <* 
 *>    --rce;  if (a_col >= MAX_COLS) {                                                                                            <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("col greater than max");                                                                    <* 
 *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
 *>       return x_xrc = rce;                                                                                                      <* 
 *>    }                                                                                                                           <* 
 *>    DEBUG_LOCS_M   yLOG_value   ("a_row"     , a_row);                                                                          <* 
 *>    --rce;  if (a_row <  0       ) {                                                                                            <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("row less than zero");                                                                      <* 
 *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
 *>       return x_xrc = rce;                                                                                                      <* 
 *>    }                                                                                                                           <* 
 *>    --rce;  if (a_row >= MAX_ROWS) {                                                                                            <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("row greater than max");                                                                    <* 
 *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
 *>       return x_xrc = rce;                                                                                                      <* 
 *>    }                                                                                                                           <* 
 *>    /+---(if fixed)-----------------------+/                                                                                    <* 
 *>    if (a_adapt == CELL_FIXED) {                                                                                                <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("checking a fixed, no expansion allowed (FIXED)");                                          <* 
 *>       DEBUG_LOCS_M   yLOG_value   ("ncol"      , s_tabs[a_tab].ncol);                                                          <* 
 *>       --rce;  if (a_col >= s_tabs[a_tab].ncol) {                                                                               <* 
 *>          DEBUG_LOCS_M   yLOG_note    ("col greater than current size");                                                        <* 
 *>          DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                      <* 
 *>          return x_xrc = rce;                                                                                                   <* 
 *>       }                                                                                                                        <* 
 *>       DEBUG_LOCS_M   yLOG_value   ("nrow"      , s_tabs[a_tab].nrow);                                                          <* 
 *>       --rce;  if (a_row >= s_tabs[a_tab].nrow) {                                                                               <* 
 *>          DEBUG_LOCS_M   yLOG_note    ("row greater than current size");                                                        <* 
 *>          DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                      <* 
 *>          return x_xrc = rce;                                                                                                   <* 
 *>       }                                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    /+---(if adapting)--------------------+/                                                                                    <* 
 *>    else if (a_adapt == CELL_GROW ) {                                                                                           <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("checking a expansion allowed (GROW)");                                                     <* 
 *>       if (a_col >=  s_tabs[a_tab].ncol)     s_tabs[a_tab].ncol = a_col  + 1;                                                   <* 
 *>       if (a_row >=  s_tabs[a_tab].nrow)     s_tabs[a_tab].nrow = a_row  + 1;                                                   <* 
 *>       /+---(update)---------+/                                                                                                 <* 
 *>       if (a_tab == CTAB) {                                                                                                     <* 
 *>          NCOL = s_tabs[a_tab].ncol;                                                                                            <* 
 *>          NROW = s_tabs[a_tab].nrow;                                                                                            <* 
 *>          DEBUG_LOCS_M   yLOG_value   ("NCOL"      , NCOL);                                                                     <* 
 *>          DEBUG_LOCS_M   yLOG_value   ("NROW"      , NROW);                                                                     <* 
 *>       }                                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    /+---(if forcing)---------------------+/                                                                                    <* 
 *>    else if (a_adapt == CELL_EXACT) {                                                                                           <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("checking exact size, expansion allowed (EXACT)");                                          <* 
 *>       /+---(cols)-----------+/                                                                                                 <* 
 *>       DEBUG_LOCS_M   yLOG_value   ("ncol"      , s_tabs[a_tab].ncol);                                                          <* 
 *>       for (i = 0; i < s_tabs [a_tab].ncol; ++i) {                                                                              <* 
 *>          if (s_tabs [a_tab].cols [i].c > 0)  x_max   = i;                                                                      <* 
 *>       }                                                                                                                        <* 
 *>       DEBUG_LOCS_M   yLOG_value   ("max used"  , x_max);                                                                       <* 
 *>       if      (a_col >=  x_max  )          s_tabs[a_tab].ncol = a_col   + 1;                                                   <* 
 *>       else                                 s_tabs[a_tab].ncol = x_max   + 1;                                                   <* 
 *>       /+---(rows)-----------+/                                                                                                 <* 
 *>       DEBUG_LOCS_M   yLOG_value   ("nrow"      , s_tabs[a_tab].nrow);                                                          <* 
 *>       for (i = 0; i < s_tabs [a_tab].nrow; ++i) {                                                                              <* 
 *>          if (s_tabs [a_tab].rows [i].c > 0)  x_max   = i;                                                                      <* 
 *>       }                                                                                                                        <* 
 *>       DEBUG_LOCS_M   yLOG_value   ("max used"  , x_max);                                                                       <* 
 *>       if      (a_row >=  x_max  )          s_tabs[a_tab].nrow = a_row   + 1;                                                   <* 
 *>       else                                 s_tabs[a_tab].nrow = x_max   + 1;                                                   <* 
 *>       /+---(update)---------+/                                                                                                 <* 
 *>       if (a_tab == CTAB) {                                                                                                     <* 
 *>          NCOL = s_tabs[a_tab].ncol;                                                                                            <* 
 *>          NROW = s_tabs[a_tab].nrow;                                                                                            <* 
 *>          DEBUG_LOCS_M   yLOG_value   ("NCOL"      , NCOL);                                                                     <* 
 *>          DEBUG_LOCS_M   yLOG_value   ("NROW"      , NROW);                                                                     <* 
 *>       }                                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);                                                                                 <* 
 *>    return x_xrc = 0;                                                                                                           <* 
 *> }                                                                                                                              <*/

tCELL*       /*-> return the cell at current loc -----[ ------ [gp.522.023.11]*/ /*-[01.0000.603.!]-*/ /*-[--.---.---.--]-*/
LOC_cell_at_curr     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   char        x_label     [LEN_LABEL];
   /*---(begin)--------------------------*/
   DEBUG_LOCS_M   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   rc = str4gyges (CTAB, CCOL, CROW, 0, 0, x_label, YSTR_LEGAL);
   if (rc < 0)  {
      DEBUG_LOCS_M   yLOG_note    ("nothing found");
      DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   x_curr = s_tabs [CTAB].sheet[CCOL][CROW];
   DEBUG_LOCS_M   yLOG_point   ("found"     , x_curr);
   /*---(complete)-----------------------*/
   DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);
   return x_curr;
}

tCELL*       /*-> return the cell at a location ------[ ------ [gp.522.323.11]*/ /*-[01.0000.#14.#]-*/ /*-[--.---.---.--]-*/
LOC_cell_at_loc      (int a_tab, int a_col, int a_row)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   char        x_label     [LEN_LABEL];
   /*---(begin)--------------------------*/
   DEBUG_LOCS_M   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   rc = str4gyges (a_tab, a_col, a_row, 0, 0, x_label, YSTR_LEGAL);
   if (rc < 0)  {
      DEBUG_LOCS_M   yLOG_note    ("nothing found");
      DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   x_curr = s_tabs[a_tab].sheet[a_col][a_row];
   DEBUG_LOCS_M   yLOG_point   ("found"     , x_curr);
   /*---(complete)-----------------------*/
   DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);
   return x_curr;
}

tCELL*       /*-> return the cell at a location ------[ ------ [gp.632.154.22]*/ /*-[01.0000.106.T]-*/ /*-[--.---.---.--]-*/
LOC_cell_labeled   (char *a_label)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   int         x_tab       =   0;           /* working tab value              */
   int         x_col       =   0;           /* working col value              */
   int         x_row       =   0;           /* working row value              */
   tCELL      *x_curr      = NULL;
   /*---(begin)--------------------------*/
   DEBUG_LOCS_M   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   rc = str2gyges (a_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_LEGAL);
   if (rc < 0)  {
      DEBUG_LOCS_M   yLOG_note    ("nothing found");
      DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   x_curr = LOC_cell_at_loc  (x_tab, x_col, x_row);
   DEBUG_LOCS_M   yLOG_point   ("cell"      , x_curr);
   if (x_curr == NULL) {
      DEBUG_LOCS_M   yLOG_note    ("nothing found");
      DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);
   return x_curr;
}



/*====================------------------------------------====================*/
/*===----                        cell references                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___REFERENCES______o () { return; }

/*> char         /+-> convert position into label --------[ ------ [ge.C90.177.91]+/ /+-[01.0000.C33.U]-+/ /+-[--.---.---.--]-+/   <* 
 *> LOC_ref           (                                                                                                            <* 
 *>       /+----------+-----------+-----------------------------------------------+/                                               <* 
 *>       short       a_tab,      /+ tab of location                              +/                                               <* 
 *>       short       a_col,      /+ col of location                              +/                                               <* 
 *>       short       a_row,      /+ row of location                              +/                                               <* 
 *>       char        a_abs,      /+ referencing of location                      +/                                               <* 
 *>       char       *a_final)    /+ variable to store location reference         +/                                               <* 
 *> {  /+---(design notes)-------------------+//+---------------------------------+/                                               <* 
 *>    /+ function purely and simply creates labels for cell locations, whether   +/                                               <* 
 *>    /+ or not there is a cell there.  if the reference is outside current tab  +/                                               <* 
 *>    /+ or sheet boundaries, it returns a warning (pos).  if it is outside the  +/                                               <* 
 *>    /+ absolute minimum and maximum boundaries it returns an error (neg).      +/                                               <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                    <* 
 *>    char        x_cname     [100]       = "";                                                                                   <* 
 *>    char        x_tref      [5]         = "";                                                                                   <* 
 *>    char        x_cref      [5]         = "";                                                                                   <* 
 *>    char        x_rref      [5]         = "";                                                                                   <* 
 *>    char        rce         = -10;           /+ return code for errors         +/                                               <* 
 *>    char        rc          = 0;             /+ generic return code            +/                                               <* 
 *>    char        x_tab       =  '0';                                                                                             <* 
 *>    /+---(prepare)------------------------+/                                                                                    <* 
 *>    strcpy (s_label, "n/a");                                                                                                    <* 
 *>    /+---(defense: valid output)----------+/                                                                                    <* 
 *>    --rce;  if (a_final == NULL)                  return rce;                                                                   <* 
 *>    strcpy (a_final, "n/a");                                                                                                    <* 
 *>    /+---(defense: legal reference)-------+/                                                                                    <* 
 *>    rc = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);                                                                           <* 
 *>    if (rc < 0)                         return rc - 10;                                                                         <* 
 *>    --rce;  if (a_abs < 0)                        return rce;                                                                   <* 
 *>    --rce;  if (a_abs > 7)                        return rce;                                                                   <* 
 *>    /+---(figure out tab name)------------+/                                                                                    <* 
 *>    if      (a_tab <=  9)  x_tab = a_tab + '0';                                                                                 <* 
 *>    else if (a_tab <= 35)  x_tab = a_tab - 10 + 'A';                                                                            <* 
 *>    else if (a_tab == 36)  x_tab = '®';                                                                                         <* 
 *>    else if (a_tab == 37)  x_tab = '¯';                                                                                         <* 
 *>    /+---(figure out column name)---------+/                                                                                    <* 
 *>    if        (a_col < 26)  {                                                                                                   <* 
 *>       x_cname[0] = a_col + 'a';                                                                                                <* 
 *>    } else  {                                                                                                                   <* 
 *>       x_cname[0] = (a_col / 26) - 1 + 'a';                                                                                     <* 
 *>       x_cname[1] = (a_col % 26) + 'a';                                                                                         <* 
 *>    }                                                                                                                           <* 
 *>    x_cname[2] = '\0';                                                                                                          <* 
 *>    /+---(figure out reference markers)---+/                                                                                    <* 
 *>    switch (a_abs) {                                                                                                            <* 
 *>    case 0 :                                                                   break;                                           <* 
 *>    case 1 :                       strcpy (x_cref, "$");                       break;                                           <* 
 *>    case 2 :                                             strcpy (x_rref, "$"); break;                                           <* 
 *>    case 3 :                       strcpy (x_cref, "$"); strcpy (x_rref, "$"); break;                                           <* 
 *>    case 4 : strcpy (x_tref, "$");                                             break;                                           <* 
 *>    case 5 : strcpy (x_tref, "$"); strcpy (x_cref, "$");                       break;                                           <* 
 *>    case 6 : strcpy (x_tref, "$");                       strcpy (x_rref, "$"); break;                                           <* 
 *>    default: strcpy (x_tref, "@");                                             break;                                           <* 
 *>    }                                                                                                                           <* 
 *>    /+---(create label)-------------------+/                                                                                    <* 
 *>    sprintf (s_label, "%s%c%s%s%s%d", x_tref, x_tab, x_cref, x_cname, x_rref, a_row + 1);                                       <* 
 *>    strcpy  (a_final, s_label);                                                                                                 <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return  0;                                                                                                                  <* 
 *> }                                                                                                                              <*/

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
   strlcpy (a_final, ""           , LEN_LABEL);
   --rce;  if (a_cell         == NULL) return rce;
   --rce;  if (a_cell->label  == NULL) return rce;
   /*---(copy label)---------------------*/
   strlcpy (a_final, a_cell->label, LEN_LABEL);
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
   if (a_tab != NULL)  *a_tab  = a_cell->tab;
   if (a_col != NULL)  *a_col  = a_cell->col;
   if (a_row != NULL)  *a_row  = a_cell->row;
   /*---(complete)-----------------------*/
   return  0;
}

/*> char         /+-> convert label into tab, col, row ---[ leaf   [ge.#HB.1B#.Z0]+/ /+-[01.0000.K44.W]-+/ /+-[--.---.---.--]-+/   <* 
 *> LOC_parse         (                                                                                                            <* 
 *>       /+----------+-----------+-----------------------------------------------+/                                               <* 
 *>       char       *a_label,    /+ cell label used for parsing                  +/                                               <* 
 *>       short      *a_tab,      /+ return variable for the tab                  +/                                               <* 
 *>       short      *a_col,      /+ return variable for the column               +/                                               <* 
 *>       short      *a_row,      /+ return variable for the row                  +/                                               <* 
 *>       char       *a_abs)      /+ return variable for the rel/abs markers      +/                                               <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         = -10;           /+ return code for errors         +/                                               <* 
 *>    int         len         =   0;           /+ length of label                +/                                               <* 
 *>    int         i           =   0;           /+ iterator -- character position +/                                               <* 
 *>    int         s_tab       =   0;           /+ starting position of tab chars +/                                               <* 
 *>    int         x_tab       =   0;           /+ working tab value              +/                                               <* 
 *>    int         s_col       =   0;           /+ starting position of col chars +/                                               <* 
 *>    int         x_col       =   0;           /+ working col value              +/                                               <* 
 *>    int         s_row       =   0;           /+ starting position of row chars +/                                               <* 
 *>    int         x_row       =   0;           /+ working row value              +/                                               <* 
 *>    int         e_row       =   0;           /+ ending position of row chars   +/                                               <* 
 *>    int         x_abs       =   0;           /+ working value for absolutes    +/                                               <* 
 *>    static char x_label     [LEN_LABEL] = "-----";                                                                              <* 
 *>    static int  x_tsave, x_csave, x_rsave, x_asave;                                                                             <* 
 *>    /+---(begin)--------------------------+/                                                                                    <* 
 *>    DEBUG_LOCS_M   yLOG_enter   (__FUNCTION__);                                                                                 <* 
 *>    DEBUG_LOCS_M   yLOG_info    ("a_label"   , a_label);                                                                        <* 
 *>    /+---(prepare values)-----------------+/                                                                                    <* 
 *>    if (a_tab != NULL)  *a_tab = -1;                                                                                            <* 
 *>    if (a_col != NULL)  *a_col = -1;                                                                                            <* 
 *>    if (a_row != NULL)  *a_row = -1;                                                                                            <* 
 *>    if (a_abs != NULL)  *a_abs =  0;                                                                                            <* 
 *>    /+---(defense: empty label)-----------+/                                                                                    <* 
 *>    --rce;  if (a_label == NULL) {                                                                                              <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("aborting, no a_label means no point");                                                     <* 
 *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(shortcut)-----------------------+/                                                                                    <* 
 *>    if (strcmp (a_label, x_label) == 0) {                                                                                       <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("shortcut, same as last request");                                                          <* 
 *>       /+---(return values)------------------+/                                                                                 <* 
 *>       if (a_tab != NULL)  *a_tab = x_tsave;                                                                                    <* 
 *>       if (a_col != NULL)  *a_col = x_csave;                                                                                    <* 
 *>       if (a_row != NULL)  *a_row = x_rsave;                                                                                    <* 
 *>       if (a_abs != NULL)  *a_abs = x_asave;                                                                                    <* 
 *>       /+---(complete)-----------------------+/                                                                                 <* 
 *>       DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);                                                                              <* 
 *>       return  0;                                                                                                               <* 
 *>    }                                                                                                                           <* 
 *>    /+---(display)------------------------+/                                                                                    <* 
 *>    DEBUG_LOCS_M   yLOG_point   ("*a_tab"    , a_tab);                                                                          <* 
 *>    DEBUG_LOCS_M   yLOG_point   ("*a_col"    , a_col);                                                                          <* 
 *>    DEBUG_LOCS_M   yLOG_point   ("*a_row"    , a_row);                                                                          <* 
 *>    DEBUG_LOCS_M   yLOG_point   ("*a_abs"    , a_abs);                                                                          <* 
 *>    /+---(check sizes)--------------------+/                                                                                    <* 
 *>    len = strlen (a_label);                                                                                                     <* 
 *>    DEBUG_LOCS_M   yLOG_value   ("len"       , len);                                                                            <* 
 *>    --rce;  if (len <=  0) {                                                                                                    <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("aborting, a_label contains nothing, no point");                                            <* 
 *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    --rce;  if (len >  12) {  /+ a1 to $24$ab$12345 +/                                                                          <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("aborting, a_label too long for legal address");                                            <* 
 *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(defense: check special)---------+/                                                                                    <* 
 *>    --rce;  if (strcmp (a_label, "root") == 0) {                                                                                <* 
 *>       DEBUG_LOCS_M   yLOG_note    ("aborting, no need to parse root");                                                         <* 
 *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
*>       return rce;                                                                                                              <* 
*>    }                                                                                                                           <* 
*>    --rce;  if (strcmp (a_label, ".") == 0) {                                                                                   <* 
   *>       DEBUG_LOCS_M   yLOG_note    ("aborting, no need to parse dot (.)");                                                      <* 
      *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
      *>       return rce;                                                                                                              <* 
      *>    }                                                                                                                           <* 
      *>    --rce;  if (strcmp (a_label, "-") == 0) {                                                                                   <* 
         *>       DEBUG_LOCS_M   yLOG_note    ("aborting, no need to parse dash (-)");                                                     <* 
            *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
            *>       return rce;                                                                                                              <* 
            *>    }                                                                                                                           <* 
            *>    /+---(look for absolute tab)----------+/                                                                                    <* 
            *>    DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[s_tab]);                                                                 <* 
            *>    if (a_label[s_tab] == '@') {                                                                                                <* 
               *>       x_abs  = 7;                                                                                                              <* 
                  *>       ++s_tab;                                                                                                                 <* 
                  *>       DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[s_tab]);                                                              <* 
                  *>    }                                                                                                                           <* 
                  *>    else if (a_label[s_tab] == '$') {                                                                                           <* 
                     *>       x_abs  = 4;                                                                                                              <* 
                        *>       ++s_tab;                                                                                                                 <* 
                        *>    }                                                                                                                           <* 
                        *>    DEBUG_LOCS_M   yLOG_value   ("x_abs"     , x_abs);                                                                          <* 
                        *>    DEBUG_LOCS_M   yLOG_value   ("s_tab"     , s_tab);                                                                          <* 
                        *>    /+---(parse tab characters)-----------+/                                                                                    <* 
                        *>    s_col = s_tab;                                                                                                              <* 
                        *>    DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[s_tab]);                                                                 <* 
                        *>    if (strchr ("0123456789"                , a_label[s_tab]) != NULL) {                                                        <* 
                           *>       x_tab = a_label[s_tab] - '0';                                                                                            <* 
                              *>       ++s_col;                                                                                                                 <* 
                              *>    } else if (strchr ("ABCDEFGHIJKLMNOPQRSTUVWXYZ", a_label[s_tab]) != NULL) {                                                 <* 
                                 *>       x_tab = a_label[s_tab] - 'A' + 10;                                                                                       <* 
                                    *>       ++s_col;                                                                                                                 <* 
                                    *>    } else if (a_label[s_tab] == '®') {                                                                                         <* 
                                       *>       x_tab = 36;                                                                                                              <* 
                                          *>       ++s_col;                                                                                                                 <* 
                                          *>    } else if (a_label[s_tab] == '¯') {                                                                                         <* 
                                             *>       x_tab = 37;                                                                                                              <* 
                                                *>       ++s_col;                                                                                                                 <* 
                                                *>    } else {                                                                                                                    <* 
                                                   *>       x_tab = CTAB;                                                                                                            <* 
                                                      *>    }                                                                                                                           <* 
                                                      *>    /+> printf ("x_tab = %d\n", x_tab);                                                <+/                                      <* 
                                                      *>    --rce;  if (x_tab + 1 <  MIN_TABS) {                                                                                        <* 
                                                         *>       DEBUG_LOCS_M   yLOG_note    ("tab less than min");                                                                       <* 
                                                            *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
                                                            *>       return rce;                                                                                                              <* 
                                                            *>    }                                                                                                                           <* 
                                                            *>    --rce;  if (x_tab + 1 >  MAX_TABS) {                                                                                        <* 
                                                               *>       DEBUG_LOCS_M   yLOG_note    ("tab greater than max");                                                                    <* 
                                                                  *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
                                                                  *>       return rce;                                                                                                              <* 
                                                                  *>    }                                                                                                                           <* 
                                                                  *>    if (s_col == s_tab && x_abs == 4)  x_abs = 1;                                                                               <* 
                                                                  *>    DEBUG_LOCS_M   yLOG_value   ("x_tab"     , x_tab);                                                                          <* 
                                                                  *>    DEBUG_LOCS_M   yLOG_value   ("s_col"     , s_col);                                                                          <* 
                                                                  *>    /+---(look for absolute col)----------+/                                                                                    <* 
                                                                  *>    DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[s_col]);                                                                 <* 
                                                                  *>    if (a_label [s_col] == '$') {                                                                                               <* 
                                                                     *>       x_abs += 1;                                                                                                              <* 
                                                                        *>       ++s_col;                                                                                                                 <* 
                                                                        *>    }                                                                                                                           <* 
                                                                        *>    DEBUG_LOCS_M   yLOG_value   ("x_abs"     , x_abs);                                                                          <* 
                                                                        *>    DEBUG_LOCS_M   yLOG_value   ("s_col"     , s_col);                                                                          <* 
                                                                        *>    /+---(parse col characters)-----------+/                                                                                    <* 
                                                                        *>    s_row = s_col;                                                                                                              <* 
                                                                        *>    for (i = s_col; i < s_col + 2; ++i) {                                                                                       <* 
                                                                           *>       DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[i]);                                                                  <* 
                                                                              *>       if (strchr ("abcdefghijklmnopqrstuvwxyz", a_label[i]) == 0)   break;                                                     <* 
                                                                              *>       if (i >  s_col)  x_col *= 26;                                                                                            <* 
                                                                              *>       x_col += a_label[i] - 'a' + 1;                                                                                           <* 
                                                                              *>       ++s_row;                                                                                                                 <* 
                                                                              *>    }                                                                                                                           <* 
                                                                              *>    DEBUG_LOCS_M   yLOG_value   ("x_col"     , x_col);                                                                          <* 
                                                                              *>    DEBUG_LOCS_M   yLOG_value   ("s_row"     , s_row);                                                                          <* 
                                                                              *>    --rce;  if (s_row == s_col) {                                                                                               <* 
                                                                                 *>       DEBUG_LOCS_M   yLOG_note    ("aborting, no column found");                                                               <* 
                                                                                    *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
                                                                                    *>       return rce;                                                                                                              <* 
                                                                                    *>    }                                                                                                                           <* 
                                                                                    *>    --x_col;                                                                                                                    <* 
                                                                                    *>    /+> printf ("x_col = %d\n", x_col);                                                <+/                                      <* 
                                                                                    *>    --rce;  if (x_col + 1 <  MIN_COLS) {                                                                                        <* 
                                                                                       *>       DEBUG_LOCS_M   yLOG_note    ("col less than min");                                                                       <* 
                                                                                          *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
                                                                                          *>       return rce;                                                                                                              <* 
                                                                                          *>    }                                                                                                                           <* 
                                                                                          *>    --rce;  if (x_col + 1 >  MAX_COLS) {                                                                                        <* 
                                                                                             *>       DEBUG_LOCS_M   yLOG_note    ("col greater than max");                                                                    <* 
                                                                                                *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
                                                                                                *>       return rce;                                                                                                              <* 
                                                                                                *>    }                                                                                                                           <* 
                                                                                                *>    DEBUG_LOCS_M   yLOG_value   ("x_col fix" , x_col);                                                                          <* 
                                                                                                *>    /+---(look for absolute row)----------+/                                                                                    <* 
                                                                                                *>    DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[s_row]);                                                                 <* 
                                                                                                *>    if (a_label [s_row] == '$') {                                                                                               <* 
                                                                                                   *>       x_abs += 2;                                                                                                              <* 
                                                                                                      *>       ++s_row;                                                                                                                 <* 
                                                                                                      *>    }                                                                                                                           <* 
                                                                                                      *>    DEBUG_LOCS_M   yLOG_value   ("x_abs"     , x_abs);                                                                          <* 
                                                                                                      *>    DEBUG_LOCS_M   yLOG_value   ("s_row"     , s_row);                                                                          <* 
                                                                                                      *>    if (x_abs > 7)  x_abs = 7;                                                                                                  <* 
                                                                                                      *>    /+---(parse row characters)-----------+/                                                                                    <* 
                                                                                                      *>    e_row = s_row;                                                                                                              <* 
                                                                                                      *>    for (i = s_row; i < len; ++i) {                                                                                             <* 
                                                                                                         *>       DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[i]);                                                                  <* 
                                                                                                            *>       if (strchr ("0123456789", a_label[i]) == 0)   break;                                                                     <* 
                                                                                                            *>       if (i >  s_row)  x_row *= 10;                                                                                            <* 
                                                                                                            *>       x_row += a_label[i] - '0';                                                                                               <* 
                                                                                                            *>       ++e_row;                                                                                                                 <* 
                                                                                                            *>    }                                                                                                                           <* 
                                                                                                            *>    DEBUG_LOCS_M   yLOG_value   ("x_row"     , x_row);                                                                          <* 
                                                                                                            *>    DEBUG_LOCS_M   yLOG_value   ("e_row"     , e_row);                                                                          <* 
                                                                                                            *>    --rce;  if (e_row != len  ) {                                                                                               <* 
                                                                                                               *>       DEBUG_LOCS_M   yLOG_note    ("aborting, no row found");                                                                  <* 
                                                                                                                  *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
                                                                                                                  *>       return rce;                                                                                                              <* 
                                                                                                                  *>    }                                                                                                                           <* 
                                                                                                                  *>    --x_row;                                                                                                                    <* 
                                                                                                                  *>    /+> printf ("x_row = %d\n", x_row);                                                <+/                                      <* 
                                                                                                                  *>    --rce;  if (x_row + 1 <  MIN_ROWS) {                                                                                        <* 
                                                                                                                     *>       DEBUG_LOCS_M   yLOG_note    ("row less than min");                                                                       <* 
                                                                                                                        *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
                                                                                                                        *>       return rce;                                                                                                              <* 
                                                                                                                        *>    }                                                                                                                           <* 
                                                                                                                        *>    --rce;  if (x_row + 1 >  MAX_ROWS) {                                                                                        <* 
                                                                                                                           *>       DEBUG_LOCS_M   yLOG_note    ("row greater than max");                                                                    <* 
                                                                                                                              *>       DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
                                                                                                                              *>       return rce;                                                                                                              <* 
                                                                                                                              *>    }                                                                                                                           <* 
                                                                                                                              *>    /+---(save for shortcut)--------------+/                                                                                    <* 
                                                                                                                              *>    strlcpy (x_label, a_label, LEN_LABEL);                                                                                      <* 
                                                                                                                              *>    x_tsave = x_tab;                                                                                                            <* 
                                                                                                                              *>    x_csave = x_col;                                                                                                            <* 
                                                                                                                              *>    x_rsave = x_row;                                                                                                            <* 
                                                                                                                              *>    x_asave = x_abs;                                                                                                            <* 
                                                                                                                              *>    /+---(return values)------------------+/                                                                                    <* 
                                                                                                                              *>    if (a_tab != NULL)  *a_tab = x_tab;                                                                                         <* 
                                                                                                                              *>    if (a_col != NULL)  *a_col = x_col;                                                                                         <* 
                                                                                                                              *>    if (a_row != NULL)  *a_row = x_row;                                                                                         <* 
                                                                                                                              *>    if (a_abs != NULL)  *a_abs = x_abs;                                                                                         <* 
                                                                                                                              *>    /+---(complete)-----------------------+/                                                                                    <* 
                                                                                                                              *>    DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);                                                                                 <* 
                                                                                                                              *>    return  0;                                                                                                                  <* 
                                                                                                                              *> }                                                                                                                              <*/



                                                                                                                              /*====================------------------------------------====================*/
                                                                                                                              /*===----                         unit testing                         ----===*/
                                                                                                                              /*====================------------------------------------====================*/
                                                                                                                              PRIV void  o___UNIT_TEST_______o () { return; }

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
      rc = str2gyges  (a_label, &x_tab, &x_col, &x_row, NULL, &x_abs, 0, YSTR_LEGAL);
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
   if (rc >= 0)  rc = str4gyges  (x_tab, s_tabs [x_tab].bcol, s_tabs [x_tab].brow, 0, 0, x_beg, YSTR_CHECK);
   if (rc >= 0)  rc = str4gyges  (x_tab, s_tabs [x_tab].ecol, s_tabs [x_tab].erow, 0, 0, x_end, YSTR_CHECK);
   if (rc >= 0)  rc = str4gyges  (x_tab, s_tabs [x_tab].ccol, s_tabs [x_tab].crow, 0, 0, x_cur, YSTR_CHECK);
   if (rc >= 0)  rc = str4gyges  (x_tab, s_tabs [x_tab].ncol - 1, s_tabs [x_tab].nrow - 1, 0, 0, x_max, YSTR_CHECK);
   if (rc <  0)  return unit_answer;
   /*---(overall)------------------------*/
   strcpy  (unit_answer, "LOC              : question not understood");
   if      (strcmp(a_question, "tab_info"      ) == 0) {
      snprintf(unit_answer, LEN_FULL, "LOC tab info (%c) : %-12.12s %-7.7s %-7.7s %-7.7s %-7.7s %d", x_label, s_tabs [x_tab].name, x_beg, x_end, x_cur, x_max, s_tabs [x_tab].c);
   }
   else if (strcmp(a_question, "cell_size"     ) == 0) {
      snprintf(unit_answer, LEN_FULL, "LOC cell size    : width=%3d, height=%3d", s_tabs [x_tab].cols [x_col].w, s_tabs [x_tab].rows [x_row].h);
   }
   else if (strcmp(a_question, "loc_who"       )  == 0) {
      snprintf (unit_answer, LEN_FULL, "LOC occupant/who : ptr=%10p, tab=%4d, col=%4d, row=%4d", s_tabs [x_tab].sheet [x_col][x_row], x_tab, x_col, x_row);
   }
   else if (strcmp(a_question, "loc_col"       )  == 0) {
      snprintf (unit_answer, LEN_FULL, "LOC col stats    : tab=%4d, col=%4d, width =%4d, used=%4d", x_tab, x_col, s_tabs[x_tab].cols[x_col].w, s_tabs[x_tab].cols[x_col].c);
   }
   else if (strcmp(a_question, "loc_row"       )  == 0) {
      snprintf (unit_answer, LEN_FULL, "LOC row stats    : tab=%4d, row=%4d, height=%4d, used=%4d", x_tab, x_row, s_tabs[x_tab].rows[x_row].h, s_tabs[x_tab].rows[x_row].c);
   }
   else if (strcmp(a_question, "tab_def"       )  == 0) {
      snprintf (unit_answer, LEN_FULL, "LOC tab defaults : col=%2d, row=%2d", s_tabs[x_tab].defwide, s_tabs[x_tab].deftall);
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
      x_curr = x_curr->next;
   }
   if (rc < 0)   x_curr = NULL;
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "loc_cell"     )  == 0) {
      if        (a_cell == NULL) {
         snprintf (unit_answer, LEN_FULL, "s_loc cell       : requested a null cell");
      } else if (x_curr == NULL) {
         snprintf (unit_answer, LEN_FULL, "s_loc cell       : ptr=%9p, cell not found in list"   , a_cell);
      } else {
         snprintf (unit_answer, LEN_FULL, "s_loc cell       : ptr=%9p, tab=%4d, col=%4d, row=%4d", x_curr, x_curr->tab, x_curr->col, x_curr->row);
      }
   }
   else if (strcmp(a_question, "loc_who"       )  == 0) {
      if (a_cell != NULL) {
         snprintf (unit_answer, LEN_FULL, "s_loc occupant   : ptr=%9p, tab=%4d, col=%4d, row=%4d", x_curr, x_curr->tab, x_curr->col, x_curr->row);
      } else {
         snprintf (unit_answer, LEN_FULL, "s_loc occupant   : ptr=%9p, no cell attached", a_cell);
      }
   }
   else if (strcmp (a_question, "cell_where")     == 0) {
      if (a_cell == NULL) {

      } else if (rc == 0) {
         snprintf (unit_answer, LEN_FULL, "s_cell location  : tab=%4d, col=%4d, row=%4d", x_curr->tab, x_curr->col, x_curr->row);
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
