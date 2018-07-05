/*============================----beg-of-source---============================*/
#include   "gyges.h"



/*====================-----------------+------------------====================*/
/*===----                  ROW & COL DATA STRUCTURES                   ----===*/
/*====================-----------------+------------------====================*/
/*
 *
 *   general discussion...
 *
 *   second, in order to define the tab/sheet data structure, we must first
 *   define the information to be stored about the rows and columns.
 *
 *   for columns, the simple one is column width.  for easing the program and
 *   speeding refreshes, it is valuable to store the column label as converted
 *   from the index, i.e., column 0 is A, 3 is C, and 26 is AA.  this only need
 *   be figured out once and the reused each time.  additionally, we'll store
 *   the screen position as well to speed display.
 *
 *   for rows, this initial version will not allow multi-line rows which are
 *   only really handy for entering gobs of text -- not our intent.  also, the
 *   screen labels are the numbers themselves, so the only valuable piece of
 *   information we need at this point is the screen position for each row.
 *   
 *   to conserve a little room, i assumed that there would never be a cell wider
 *   that 255 characters (uchar).
 *
 *
 *   actual data structures...
 *
 */
struct cCOLS {
   char      l[3];            /* column label                                 */
   uchar     w;               /* column width                                 */
   ushort    x;               /* starting horizonal position on screen        */
   int       c;               /* optional: count of entries in column         */
};
struct cROWS {
   ushort    h;               /* row height                                   */
   ushort    y;               /* starting vertical position on screen         */
   int       c;               /* optional: count of entries in row            */
};



/*====================-----------------+------------------====================*/
/*===----                      TAB DATA STRUCTURE                      ----===*/
/*====================-----------------+------------------====================*/
/*
 *
 *   general discussion...
 *
 *   third we define the overarching tab/sheet data structure that provides
 *   the iconic spreadsheet feel to the program.  a tab is simply a name
 *   associated with a retangle of potenial cells and a set of basic
 *   characteristics, such as width and height.
 *
 *   the formost simplification is that a retangular matrix of cell pointers
 *   is created from the get go rather than using a clever data structure that
 *   conserves more memory.  while it only allocates pointers, that's still a
 *   great deal of space that will never get used.  but, on the performance
 *   side, a two-dimensional array provides direct access without any need to
 *   search a data structure, no matter how efficient.  it also provides a very
 *   efficient means of conducting operations on visual ranges like columns,
 *   rows, and retangular selections.  i think its well worth it -- you can
 *   adapt it in the long run if you wish.
 *
 *   just like the cell data structure, i have divided it into sections and
 *   provided a little commentary for each.
 *
 *
 *   actual data structure...
 *
 */
struct cTAB {
   /*---(header)-------------------------*/
   /* tabs are pre-allocated and can put into and taken out of use simply by  */
   /* starting to use them.                                                   */
   short       tab;                         /* number of tab                  */
   char        name  [LEN_ABBR];            /* tab name for user reference    */
   char        type;                        /* tab type                       */
   /*---(contents)-----------------------*/
   /* tabs pull three other data structures together in a package: column     */
   /* characteristics, row characteristics, and a grid on which to hang cells.*/
   tCOLS       cols  [MAX_COLS];            /* column characteristics         */
   tROWS       rows  [MAX_ROWS];            /* row characteristics            */
   tCELL      *sheet [MAX_COLS][MAX_ROWS];  /* cell pointers                  */
   int         c;                           /* count of entries in sheet      */
   short       defwide;                     /* default col width              */
   short       deftall;                     /* default row height             */
   /*---(current size limits)------------*/
   /* while a maximum size sheet is allocated, there are logical user set     */
   /* maximums in order to manage the complexity.                             */
   short       ncol;                        /* current limit on cols          */
   short       nrow;                        /* current limit on rows          */
   /*---(current position)---------------*/
   /* while working, a user changes position to review and manipulate and     */
   /* these variables store the current screen position.                      */
   short       ccol;                        /* current column                 */
   short       crow;                        /* current row                    */
   /*---(screen limits)------------------*/
   /* given user movement, the program calculates and stores the first (beg)  */
   /* and last (end) cols and rows which can be seen.                         */
   short       bcol;                        /* beginning column               */
   short       brow;                        /* beginning row                  */
   short       ecol;                        /* ending column                  */
   short       erow;                        /* ending row                     */
   /*---(frozen rows and cols)-----------*/
   /* in order to handle large volumes of data in a table, it is necessary to */
   /* be able to freeze cols and/or rows so they remain visible               */
   char        froz_col;                    /* are the cols frozen            */
   short       froz_bcol;                   /* left of frozen cols            */
   short       froz_ecol;                   /* right of frozen cols           */
   char        froz_row;                    /* are the rows frozen            */
   short       froz_brow;                   /* top of frozen rows             */
   short       froz_erow;                   /* bottom of frozen rows          */
   /*---(end)----------------------------*/
};
static tTAB     s_tabs [MAX_TABS];
static tTAB    *p_tab;                        /* current tab pointer                 */




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
   NTAB      = MAX_TABS;
   /*---(update tab)---------------------*/
   LOC_tab_switch (-1);
   CCOL      = 0;
   CROW      = 0;
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear all cell placements ----------[ leaf   [fe.833.133.20]*/ /*-[02.0000.014.!]-*/ /*-[--.---.---.--]-*/
LOC__clear_locs      (short a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   short       x_col       =   0;
   short       x_row       =   0;
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
   short       x_tab       = 0;
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
      LOC_col_clear     (x_tab);
      LOC_row_clear     (x_tab);
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
}



/*====================------------------------------------====================*/
/*===----                     hooking and unhooking                    ----===*/
/*====================------------------------------------====================*/
PRIV void  o___HOOKING_________o () { return; }

char         /*-> attach a cell to a location --------[ ------ [ge.E63.137.82]*/ /*-[01.0000.115.J]-*/ /*-[--.---.---.--]-*/
LOC_hook           (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* cell to be placed in a location              */
      short       a_tab,      /* tab for target location                      */
      short       a_col,      /* col for target location                      */
      short       a_row)      /* row for target location                      */
{  /*---(design notes)--------------------------------------------------------*/
   /* add the cell reference to an appropriate empty location.  nothing else  */
   /* on the tab or cell should be modified.                                  */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
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
   rc = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);
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
   /*---(point location at cell)---------*/
   s_tabs [a_tab].sheet[a_col][a_row] = a_cell;
   if (LOC_label (a_cell, a_cell->label) < 0) {
      strcpy (a_cell->label, "tbd");
   }
   /*---(update totals)------------------*/
   ++s_tabs[a_tab].cols[a_col].c;
   ++s_tabs[a_tab].rows[a_row].c;
   ++s_tabs[a_tab].c;
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
   short       x_tab       = UNHOOKED;
   short       x_col       = UNHOOKED;
   short       x_row       = UNHOOKED;
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
   rc = LOC_legal (x_tab, x_col, x_row, CELL_FIXED);
   DEBUG_LOCS   yLOG_value   ("rc"        , rc);
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
      short       a_stab,         /* tab of source cell                       */
      short       a_scol,         /* col of source cell                       */
      short       a_srow,         /* row of source cell                       */
      short       a_ttab,         /* tab of target cell                       */
      short       a_tcol,         /* col of target cell                       */
      short       a_trow)         /* row of target cell                       */
{  /*---(design notes)--------------------------------------------------------*/
   /* changes the location of the cell at the source location to the target   */
   /* location.  if there is a cell already at the target, it is deleted.  if */
   /* there is nothing at the source, the target is wiped anyway as this is   */
   /* consistent with selection cut, copy, and paste.                         */
   /*---(locals)-----------+-----------+-*/
   tCELL      *source      = NULL;
   tCELL      *target      = NULL;
   char        rce         = -10;           /* return code for errors         */
   /*---(defense: source)----------------*/
   --rce;
   if (LOC_legal (a_stab, a_scol, a_srow, CELL_FIXED) != 0)  return rce;
   source      = s_tabs[a_stab].sheet[a_scol][a_srow];
   /*---(defense: target)----------------*/
   --rce;
   if (LOC_legal (a_ttab, a_tcol, a_trow, CELL_FIXED) != 0)  return rce;
   target      = s_tabs[a_ttab].sheet[a_tcol][a_trow];
   /*---(overwrite as necessary)---------*/
   if (target  != NULL) {
      CELL_delete (CHG_NOHIST, a_ttab, a_tcol, a_trow);
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

char         /*-> verify that a location is legal ----[ leaf   [ge.QA9.49#.M0]*/ /*-[02.0000.C94.#]-*/ /*-[--.---.---.--]-*/
LOC_legal          (short a_tab, short a_col, short a_row, char a_adapt)
{  /*---(design notes)--------------------------------------------------------*/
   /* tests the tab, col, and row against minimum and maximum limits as well  */
   /* as against current limits as a service to other functions.              */
   /* additionally, if requested, the function can expand the current limits  */
   /* to include the requested row and column if it is within the max limits. */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   short       x_max       = 0;             /* maximum used col/row in tab    */
   int         i           = 0;
   static short  x_xtab    = -666;
   static short  x_xcol    = -666;
   static short  x_xrow    = -666;
   static short  x_adapt   = -666;
   static char   x_xrc     =  -66;
   static int    x_xtotal  =    0;
   static int    x_xshort  =    0;
   /*---(shortcut)-----------------------*/
   ++x_xtotal;
   if (x_xtab == a_tab && x_xcol == a_col && x_xrow == a_row &&  x_adapt == a_adapt) {
      ++x_xshort;
      DEBUG_LOCS_M   yLOG_senter  (__FUNCTION__);
      DEBUG_LOCS_M   yLOG_snote   ("same as last search");
      DEBUG_LOCS_M   yLOG_sint    (x_xrc);
      DEBUG_LOCS_M   yLOG_sint    (x_xtotal);
      DEBUG_LOCS_M   yLOG_sint    (x_xshort);
      DEBUG_LOCS_M   yLOG_sexit   (__FUNCTION__);
      return x_xrc;
   }
   /*---(begin)--------------------------*/
   DEBUG_LOCS_M   yLOG_enter   (__FUNCTION__);
   /*---(save values)--------------------*/
   x_xtab  = a_tab;
   x_xcol  = a_col;
   x_xrow  = a_row;
   x_adapt = a_adapt;
   /*---(check absolute boudaries)-------*/
   DEBUG_LOCS_M   yLOG_value   ("a_tab"     , a_tab);
   --rce;  if (a_tab <  0       ) {
      DEBUG_LOCS_M   yLOG_note    ("tab less than zero");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return x_xrc = rce;
   }
   --rce;  if (a_tab >= MAX_TABS) {
      DEBUG_LOCS_M   yLOG_note    ("tab greater than max");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return x_xrc = rce;
   }
   DEBUG_LOCS_M   yLOG_value   ("a_col"     , a_col);
   --rce;  if (a_col <  0       ) {
      DEBUG_LOCS_M   yLOG_note    ("col less than zero");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return x_xrc = rce;
   }
   --rce;  if (a_col >= MAX_COLS) {
      DEBUG_LOCS_M   yLOG_note    ("col greater than max");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return x_xrc = rce;
   }
   DEBUG_LOCS_M   yLOG_value   ("a_row"     , a_row);
   --rce;  if (a_row <  0       ) {
      DEBUG_LOCS_M   yLOG_note    ("row less than zero");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return x_xrc = rce;
   }
   --rce;  if (a_row >= MAX_ROWS) {
      DEBUG_LOCS_M   yLOG_note    ("row greater than max");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return x_xrc = rce;
   }
   /*---(if fixed)-----------------------*/
   if (a_adapt == CELL_FIXED) {
      DEBUG_LOCS_M   yLOG_note    ("checking a fixed, no expansion allowed (FIXED)");
      DEBUG_LOCS_M   yLOG_value   ("ncol"      , s_tabs[a_tab].ncol);
      --rce;  if (a_col >= s_tabs[a_tab].ncol) {
         DEBUG_LOCS_M   yLOG_note    ("col greater than current size");
         DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
         return x_xrc = rce;
      }
      DEBUG_LOCS_M   yLOG_value   ("nrow"      , s_tabs[a_tab].nrow);
      --rce;  if (a_row >= s_tabs[a_tab].nrow) {
         DEBUG_LOCS_M   yLOG_note    ("row greater than current size");
         DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
         return x_xrc = rce;
      }
   }
   /*---(if adapting)--------------------*/
   else if (a_adapt == CELL_GROW ) {
      DEBUG_LOCS_M   yLOG_note    ("checking a expansion allowed (GROW)");
      if (a_col >=  s_tabs[a_tab].ncol)     s_tabs[a_tab].ncol = a_col  + 1;
      if (a_row >=  s_tabs[a_tab].nrow)     s_tabs[a_tab].nrow = a_row  + 1;
      /*---(update)---------*/
      if (a_tab == CTAB) {
         NCOL = s_tabs[a_tab].ncol;
         NROW = s_tabs[a_tab].nrow;
         DEBUG_LOCS_M   yLOG_value   ("NCOL"      , NCOL);
         DEBUG_LOCS_M   yLOG_value   ("NROW"      , NROW);
      }
   }
   /*---(if forcing)---------------------*/
   else if (a_adapt == CELL_EXACT) {
      DEBUG_LOCS_M   yLOG_note    ("checking exact size, expansion allowed (EXACT)");
      /*---(cols)-----------*/
      DEBUG_LOCS_M   yLOG_value   ("ncol"      , s_tabs[a_tab].ncol);
      for (i = 0; i < s_tabs [a_tab].ncol; ++i) {
         if (s_tabs [a_tab].cols [i].c > 0)  x_max   = i;
      }
      DEBUG_LOCS_M   yLOG_value   ("max used"  , x_max);
      if      (a_col >=  x_max  )          s_tabs[a_tab].ncol = a_col   + 1;
      else                                 s_tabs[a_tab].ncol = x_max   + 1;
      /*---(rows)-----------*/
      DEBUG_LOCS_M   yLOG_value   ("nrow"      , s_tabs[a_tab].nrow);
      for (i = 0; i < s_tabs [a_tab].nrow; ++i) {
         if (s_tabs [a_tab].rows [i].c > 0)  x_max   = i;
      }
      DEBUG_LOCS_M   yLOG_value   ("max used"  , x_max);
      if      (a_row >=  x_max  )          s_tabs[a_tab].nrow = a_row   + 1;
      else                                 s_tabs[a_tab].nrow = x_max   + 1;
      /*---(update)---------*/
      if (a_tab == CTAB) {
         NCOL = s_tabs[a_tab].ncol;
         NROW = s_tabs[a_tab].nrow;
         DEBUG_LOCS_M   yLOG_value   ("NCOL"      , NCOL);
         DEBUG_LOCS_M   yLOG_value   ("NROW"      , NROW);
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);
   return x_xrc = 0;
}

tCELL*       /*-> return the cell at current loc -----[ ------ [gp.522.023.11]*/ /*-[01.0000.603.!]-*/ /*-[--.---.---.--]-*/
LOC_cell_at_curr     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   /*---(begin)--------------------------*/
   DEBUG_LOCS_M   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   rc = LOC_legal (CTAB, CCOL, CROW, CELL_FIXED);
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
LOC_cell_at_loc      (short a_tab, short a_col, short a_row)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   /*---(begin)--------------------------*/
   DEBUG_LOCS_M   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   rc = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);
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
   short       x_tab       =   0;           /* working tab value              */
   short       x_col       =   0;           /* working col value              */
   short       x_row       =   0;           /* working row value              */
   tCELL      *x_curr      = NULL;
   /*---(begin)--------------------------*/
   DEBUG_LOCS_M   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   rc = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);
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

char         /*-> go directly to a specific cell -----[ ------ [gc.722.112.13]*/ /*-[01.0000.304.#]-*/ /*-[--.---.---.--]-*/
LOC_jump           (
      /*----------+-----------+-----------------------------------------------*/
      short       a_tab,      /* tab number                                   */
      short       a_col,      /* column number                                */
      short       a_row)      /* row number                                   */
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   /*---(begin)--------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_value   ("a_tab"     , a_tab);
   DEBUG_LOCS   yLOG_value   ("a_col"     , a_col);
   DEBUG_LOCS   yLOG_value   ("a_row"     , a_row);
   /*---(defenses)-----------------------*/
   rc = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);
   DEBUG_LOCS   yLOG_value   ("rc"        , rc);
   if (rc < 0)  {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(update globals)-----------------*/
   LOC_tab_switch (a_tab);
   CCOL          = a_col;
   CROW          = a_row;
   /*---(selection)----------------------*/
   /*> VISU_update (a_tab, a_col, a_row);                                             <*/
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> return coordinates for address -----[ ------ [gc.722.112.13]*/ /*-[01.0000.304.#]-*/ /*-[--.---.---.--]-*/
LOC_locator        (char *a_label, int *a_x, int *a_y, int *a_z)
{
   char        rc;
   short       x_tab;
   short       x_col;
   short       x_row;
   /*---(begin)--------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_info    ("a_label"   , a_label);
   DEBUG_LOCS   yLOG_point   ("a_x"       , a_x);
   DEBUG_LOCS   yLOG_point   ("a_y"       , a_y);
   DEBUG_LOCS   yLOG_point   ("a_z"       , a_z);
   rc = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);
   if (rc < 0)  {
      DEBUG_LOCS   yLOG_note    ("nothing found");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   if (a_x != NULL)  *a_x = x_col;
   if (a_y != NULL)  *a_y = x_row;
   if (a_z != NULL)  *a_z = x_tab;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> return address for coordinates -----[ ------ [gc.722.112.13]*/ /*-[01.0000.304.#]-*/ /*-[--.---.---.--]-*/
LOC_addressor      (char *a_label, int a_x, int a_y, int a_z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(begin)--------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_value   ("a_x"       , a_x);
   DEBUG_LOCS   yLOG_value   ("a_y"       , a_y);
   DEBUG_LOCS   yLOG_value   ("a_z"       , a_z);
   rc = LOC_ref (a_z, a_x, a_y, 0, a_label);
   if (rc < 0)  {
      DEBUG_LOCS   yLOG_note    ("nothing found");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return -1;
   }
   DEBUG_LOCS   yLOG_info    ("a_label"   , a_label);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        cell references                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___REFERENCES______o () { return; }

char         /*-> convert position into label --------[ ------ [ge.C90.177.91]*/ /*-[01.0000.C33.U]-*/ /*-[--.---.---.--]-*/
LOC_ref           (
      /*----------+-----------+-----------------------------------------------*/
      short       a_tab,      /* tab of location                              */
      short       a_col,      /* col of location                              */
      short       a_row,      /* row of location                              */
      char        a_abs,      /* referencing of location                      */
      char       *a_final)    /* variable to store location reference         */
{  /*---(design notes)-------------------*//*---------------------------------*/
   /* function purely and simply creates labels for cell locations, whether   */
   /* or not there is a cell there.  if the reference is outside current tab  */
   /* or sheet boundaries, it returns a warning (pos).  if it is outside the  */
   /* absolute minimum and maximum boundaries it returns an error (neg).      */
   /*---(locals)-----------+-----+-----+-*/
   char        x_cname     [100]       = "";
   char        x_tref      [5]         = "";
   char        x_cref      [5]         = "";
   char        x_rref      [5]         = "";
   char        rce         = -10;           /* return code for errors         */
   char        rc          = 0;             /* generic return code            */
   char        x_tab       =  '0';
   /*---(prepare)------------------------*/
   strcpy (s_label, "n/a");
   /*---(defense: valid output)----------*/
   --rce;  if (a_final == NULL)                  return rce;
   strcpy (a_final, "n/a");
   /*---(defense: legal reference)-------*/
   rc = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);
   if (rc < 0)                         return rc - 10;
   --rce;  if (a_abs < 0)                        return rce;
   --rce;  if (a_abs > 7)                        return rce;
   /*---(figure out tab name)------------*/
   if (a_tab <= 9)     x_tab = a_tab + '0';
   else                x_tab = a_tab - 10 + 'A';
   /*---(figure out column name)---------*/
   if        (a_col < 26)  {
      x_cname[0] = a_col + 'a';
   } else  {
      x_cname[0] = (a_col / 26) - 1 + 'a';
      x_cname[1] = (a_col % 26) + 'a';
   }
   x_cname[2] = '\0';
   /*---(figure out reference markers)---*/
   switch (a_abs) {
   case 0 :                                                                   break;
   case 1 :                       strcpy (x_cref, "$");                       break;
   case 2 :                                             strcpy (x_rref, "$"); break;
   case 3 :                       strcpy (x_cref, "$"); strcpy (x_rref, "$"); break;
   case 4 : strcpy (x_tref, "$");                                             break;
   case 5 : strcpy (x_tref, "$"); strcpy (x_cref, "$");                       break;
   case 6 : strcpy (x_tref, "$");                       strcpy (x_rref, "$"); break;
   default: strcpy (x_tref, "@");                                             break;
   }
   /*---(create label)-------------------*/
   sprintf (s_label, "%s%c%s%s%s%d", x_tref, x_tab, x_cref, x_cname, x_rref, a_row + 1);
   strcpy  (a_final, s_label);
   /*---(complete)-----------------------*/
   return  0;
}

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
   char        label       [100]       = "";
   char        rce         = -10;           /* return code for errors         */
   char        rc          =   0;           /* return code                    */
   /*---(begin)--------------------------*/
   DEBUG_LOCS_M   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS_M   yLOG_point   ("*a_cell"   , a_cell);
   DEBUG_LOCS_M   yLOG_point   ("*a_final"  , a_final);
   /*---(defense: valid output)----------*/
   --rce;  if (a_final == NULL) {
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (a_final, "((null))", LEN_LABEL);
   /*---(defense: valid cell)------------*/
   --rce;  if (a_cell  == NULL) {
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(fiugure out reference)----------*/
   rc = LOC_ref (a_cell->tab, a_cell->col, a_cell->row, 0, a_final);
   DEBUG_LOCS_M   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0) {
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);
   return  rc;
}

char         /*-> convert a cell into a coordinates --[ leaf   [ge.520.127.40]*/ /*-[01.0000.409.I]-*/ /*-[--.---.---.--]-*/
LOC_coords        (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* cell to be converted                         */
      short      *a_tab,      /* return variable for the tab                  */
      short      *a_col,      /* return variable for the column               */
      short      *a_row)      /* return variable for the row                  */
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

char         /*-> convert label into tab, col, row ---[ leaf   [ge.#HB.1B#.Z0]*/ /*-[01.0000.K44.W]-*/ /*-[--.---.---.--]-*/
LOC_parse         (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_label,    /* cell label used for parsing                  */
      short      *a_tab,      /* return variable for the tab                  */
      short      *a_col,      /* return variable for the column               */
      short      *a_row,      /* return variable for the row                  */
      char       *a_abs)      /* return variable for the rel/abs markers      */
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   int         len         =   0;           /* length of label                */
   int         i           =   0;           /* iterator -- character position */
   int         s_tab       =   0;           /* starting position of tab chars */
   int         x_tab       =   0;           /* working tab value              */
   int         s_col       =   0;           /* starting position of col chars */
   int         x_col       =   0;           /* working col value              */
   int         s_row       =   0;           /* starting position of row chars */
   int         x_row       =   0;           /* working row value              */
   int         e_row       =   0;           /* ending position of row chars   */
   int         x_abs       =   0;           /* working value for absolutes    */
   static char x_label     [LEN_LABEL] = "-----";
   static int  x_tsave, x_csave, x_rsave, x_asave;
   /*---(begin)--------------------------*/
   DEBUG_LOCS_M   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS_M   yLOG_info    ("a_label"   , a_label);
   /*---(prepare values)-----------------*/
   if (a_tab != NULL)  *a_tab = -1;
   if (a_col != NULL)  *a_col = -1;
   if (a_row != NULL)  *a_row = -1;
   if (a_abs != NULL)  *a_abs =  0;
   /*---(defense: empty label)-----------*/
   --rce;  if (a_label == NULL) {
      DEBUG_LOCS_M   yLOG_note    ("aborting, no a_label means no point");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(shortcut)-----------------------*/
   if (strcmp (a_label, x_label) == 0) {
      DEBUG_LOCS_M   yLOG_note    ("shortcut, same as last request");
      /*---(return values)------------------*/
      if (a_tab != NULL)  *a_tab = x_tsave;
      if (a_col != NULL)  *a_col = x_csave;
      if (a_row != NULL)  *a_row = x_rsave;
      if (a_abs != NULL)  *a_abs = x_asave;
      /*---(complete)-----------------------*/
      DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(display)------------------------*/
   DEBUG_LOCS_M   yLOG_point   ("*a_tab"    , a_tab);
   DEBUG_LOCS_M   yLOG_point   ("*a_col"    , a_col);
   DEBUG_LOCS_M   yLOG_point   ("*a_row"    , a_row);
   DEBUG_LOCS_M   yLOG_point   ("*a_abs"    , a_abs);
   /*---(check sizes)--------------------*/
   len = strlen (a_label);
   DEBUG_LOCS_M   yLOG_value   ("len"       , len);
   --rce;  if (len <=  0) {
      DEBUG_LOCS_M   yLOG_note    ("aborting, a_label contains nothing, no point");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (len >  12) {  /* a1 to $24$ab$12345 */
      DEBUG_LOCS_M   yLOG_note    ("aborting, a_label too long for legal address");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense: check special)---------*/
   --rce;  if (strcmp (a_label, "root") == 0) {
      DEBUG_LOCS_M   yLOG_note    ("aborting, no need to parse root");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strcmp (a_label, ".") == 0) {
      DEBUG_LOCS_M   yLOG_note    ("aborting, no need to parse dot (.)");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strcmp (a_label, "-") == 0) {
      DEBUG_LOCS_M   yLOG_note    ("aborting, no need to parse dash (-)");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(look for absolute tab)----------*/
   DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[s_tab]);
   if (a_label[s_tab] == '@') {
      x_abs  = 7;
      ++s_tab;
      DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[s_tab]);
   }
   else if (a_label[s_tab] == '$') {
      x_abs  = 4;
      ++s_tab;
   }
   DEBUG_LOCS_M   yLOG_value   ("x_abs"     , x_abs);
   DEBUG_LOCS_M   yLOG_value   ("s_tab"     , s_tab);
   /*---(parse tab characters)-----------*/
   s_col = s_tab;
   DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[s_tab]);
   if (strchr ("0123456789"                , a_label[s_tab]) != NULL) {
      x_tab = a_label[s_tab] - '0';
      ++s_col;
   } else if (strchr ("ABCDEFGHIJKLMNOPQRSTUVWXYZ", a_label[s_tab]) != NULL) {
      x_tab = a_label[s_tab] - 'A' + 10;
      ++s_col;
   } else {
      x_tab = CTAB;
   }
   /*> printf ("x_tab = %d\n", x_tab);                                                <*/
   --rce;  if (x_tab + 1 <  MIN_TABS) {
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (x_tab + 1 >  MAX_TABS) {
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (s_col == s_tab && x_abs == 4)  x_abs = 1;
   DEBUG_LOCS_M   yLOG_value   ("x_tab"     , x_tab);
   DEBUG_LOCS_M   yLOG_value   ("s_col"     , s_col);
   /*---(look for absolute col)----------*/
   DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[s_col]);
   if (a_label [s_col] == '$') {
      x_abs += 1;
      ++s_col;
   }
   DEBUG_LOCS_M   yLOG_value   ("x_abs"     , x_abs);
   DEBUG_LOCS_M   yLOG_value   ("s_col"     , s_col);
   /*---(parse col characters)-----------*/
   s_row = s_col;
   for (i = s_col; i < s_col + 2; ++i) {
      DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[i]);
      if (strchr ("abcdefghijklmnopqrstuvwxyz", a_label[i]) == 0)   break;
      if (i >  s_col)  x_col *= 26;
      x_col += a_label[i] - 'a' + 1;
      ++s_row;
   }
   DEBUG_LOCS_M   yLOG_value   ("x_col"     , x_col);
   DEBUG_LOCS_M   yLOG_value   ("s_row"     , s_row);
   --rce;  if (s_row == s_col) {
      DEBUG_LOCS_M   yLOG_note    ("aborting, no column found");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --x_col;
   /*> printf ("x_col = %d\n", x_col);                                                <*/
   --rce;  if (x_col + 1 <  MIN_COLS) {
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (x_col + 1 >  MAX_COLS) {
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS_M   yLOG_value   ("x_col fix" , x_col);
   /*---(look for absolute row)----------*/
   DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[s_row]);
   if (a_label [s_row] == '$') {
      x_abs += 2;
      ++s_row;
   }
   DEBUG_LOCS_M   yLOG_value   ("x_abs"     , x_abs);
   DEBUG_LOCS_M   yLOG_value   ("s_row"     , s_row);
   if (x_abs > 7)  x_abs = 7;
   /*---(parse row characters)-----------*/
   e_row = s_row;
   for (i = s_row; i < len; ++i) {
      DEBUG_LOCS_M   yLOG_char    ("CH"        , a_label[i]);
      if (strchr ("0123456789", a_label[i]) == 0)   break;
      if (i >  s_row)  x_row *= 10;
      x_row += a_label[i] - '0';
      ++e_row;
   }
   DEBUG_LOCS_M   yLOG_value   ("x_row"     , x_row);
   DEBUG_LOCS_M   yLOG_value   ("e_row"     , e_row);
   --rce;  if (e_row != len  ) {
      DEBUG_LOCS_M   yLOG_note    ("aborting, no row found");
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --x_row;
   /*> printf ("x_row = %d\n", x_row);                                                <*/
   --rce;  if (x_row + 1 <  MIN_ROWS) {
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (x_row + 1 >  MAX_ROWS) {
      DEBUG_LOCS_M   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save for shortcut)--------------*/
   strlcpy (x_label, a_label, LEN_LABEL);
   x_tsave = x_tab;
   x_csave = x_col;
   x_rsave = x_row;
   x_asave = x_abs;
   /*---(return values)------------------*/
   if (a_tab != NULL)  *a_tab = x_tab;
   if (a_col != NULL)  *a_col = x_col;
   if (a_row != NULL)  *a_row = x_row;
   if (a_abs != NULL)  *a_abs = x_abs;
   /*---(complete)-----------------------*/
   DEBUG_LOCS_M   yLOG_exit    (__FUNCTION__);
   return  0;
}



/*====================------------------------------------====================*/
/*===----                     mapping for map mode                     ----===*/
/*====================------------------------------------====================*/
static void  o___MAPPER__________o () { return; }

char
MAP__clear            (tMAPPED *a_map)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =    0;
   /*---(lefts)--------------------------*/
   a_map->gmin = a_map->amin = a_map->lmin = a_map->prev = -1;
   /*---(map)----------------------------*/
   for (i= 0; i < LEN_MAP; ++i)  a_map->map [i] =  YVIKEYS_EMPTY;
   /*---(rights)-------------------------*/
   a_map->gmax = a_map->amax = a_map->lmax = a_map->next = -1;
   /*---(screen)-------------------------*/
   a_map->beg  = a_map->cur  = a_map->end  = a_map->len  = a_map->tend = 0;
   /*---(grids)--------------------------*/
   a_map->gbeg = a_map->gcur = a_map->gend = 0;
   /*---(complete)-----------------------*/
   return  0;
}

char
LOC__mapper                (char a_dir)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_max       =    0;
   tMAPPED    *x_map       = NULL;
   int         x_size      =    0;
   int         x_count     =    0;
   int         x_cell      =    0;
   tCELL      *x_curr      = NULL;
   tCELL      *x_prev      = NULL;
   int         x_unit      =    0;
   int         x_save      =    0;
   int         x_mark      =    0;
   /*---(prepare)------------------------*/
   switch (a_dir) {
   case 'C' : case 'c' :
      x_map  = &g_xmap;
      x_max  = NCOL - 1;
      break;
   case 'R' : case 'r' :
      x_map  = &g_ymap;
      x_max  = NROW - 1;
      break;
   case 'T' : case 't' :
      x_map  = &g_zmap;
      x_max  = NTAB - 1;
      break;
   }
   x_mark = x_map->gcur;
   /*---(clear)--------------------------*/
   for (i= 0; i < LEN_MAP; ++i)  x_map->map [i] =  YVIKEYS_EMPTY;
   x_map->gmin = x_map->amin = x_map->lmin = x_map->prev = -1;
   x_map->gmax = x_map->amax = x_map->lmax = x_map->next = -1;
   /*---(do columns)---------------------*/
   for (x_cell = 0; x_cell <= x_max; ++x_cell) {
      /*---(get base data)---------------*/
      switch (a_dir) {
      case 'C' : case 'c' :
         x_size  = s_tabs [CTAB].cols [x_cell].w;
         x_curr  = LOC_cell_at_loc (CTAB, x_cell, CROW);
         x_count = s_tabs [CTAB].cols [x_cell].c;
         break;
      case 'R' : case 'r' :
         x_size  = s_tabs [CTAB].rows [x_cell].h;
         x_curr  = LOC_cell_at_loc (CTAB, CCOL, x_cell);
         x_count = s_tabs [CTAB].rows [x_cell].c;
         break;
      case 'T' : case 't' :
         x_size  = 1;
         x_curr  = LOC_cell_at_loc (x_cell, CCOL, CROW);
         x_count = 1;
         break;
      }
      /*---(mins)------------------------*/
      x_map->gmin = 0;
      if (x_map->amin < 0 && x_count > 0)      x_map->amin = x_unit;
      if (x_map->lmin < 0 && x_curr != NULL)   x_map->lmin = x_unit;
      if (x_cell <  x_mark) {
         if (x_prev == NULL && x_curr != NULL) x_map->prev = x_unit;
         if (x_prev != NULL && x_curr == NULL) x_map->prev = x_save;
      }
      /*---(update map)------------------*/
      for (i = 0; i < x_size; ++i) {
         x_map->map [x_unit++] = x_cell;
      }
      /*---(maxes)-----------------------*/
      if (x_curr != NULL)                      x_map->lmax = x_unit - 1;
      if (x_count > 0)                         x_map->amax = x_unit - 1;
      x_map->gmax = x_unit - 1;
      if (x_cell > x_mark) {
         if (x_prev == NULL && x_curr != NULL) x_map->next = x_unit;
         if (x_prev != NULL && x_curr == NULL) x_map->next = x_save;
      }
      /*---(done)------------------------*/
      x_save = x_unit;
      x_prev = x_curr;
   }
   /*---(update lefts)-------------------*/
   if (x_map->amin < 0)  x_map->amin = x_map->gmin;
   if (x_map->lmin < 0)  x_map->lmin = x_map->gmin;
   if (x_map->prev < 0)  x_map->prev = x_map->gmin;
   /*---(update rights)------------------*/
   if (x_map->amax < 0)  x_map->amax = x_map->gmin;
   if (x_map->lmax < 0)  x_map->lmax = x_map->gmin;
   if (x_map->next < 0)  x_map->next = x_map->gmax;
   /*---(other)--------------------------*/
   if (a_dir != tolower (a_dir)) {
      x_map->beg   = 0;
      x_map->cur   = 0;
      x_map->end   = 0;
      x_map->len   = 0;
      x_map->tend  = 0;
      x_map->gbeg  = 0;
      x_map->gcur  = 0;
      x_map->gend  = 0;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
LOC__mapprint    (char a_dir)
{
   /*---(locals)-----------+-----------+-*/
   FILE       *f           = NULL;
   char        x_name      [LEN_LABEL] = "";
   tMAPPED    *x_map       = NULL;
   int         i           =    0;
   /*---(prepare)------------------------*/
   switch (a_dir) {
   case 'c' :
      x_map = &g_xmap;
      strlcpy (x_name, "gyges.cmap", LEN_LABEL);
      break;
   case 'r' :
      x_map = &g_ymap;
      strlcpy (x_name, "gyges.rmap", LEN_LABEL);
      break;
   case 't' :
      x_map = &g_zmap;
      strlcpy (x_name, "gyges.tmap", LEN_LABEL);
      break;
   }
   /*---(write it out)-------------------*/
   f = fopen (x_name, "w");
   if (f == NULL)  return -1;
   /*---(headers)------------------------*/
   fprintf (f, "gmin amin lmin prev    ");
   for (i = 0; i < LEN_MAP; ++i) {
      if (x_map->map [i] < 0)  break;
      fprintf (f, "%4d "  , i);
   }
   fprintf (f, "   next lmax amax gmax\n");
   /*---(content)------------------------*/
   fprintf (f, "%4d %4d %4d %4d    "  , x_map->gmin, x_map->amin, x_map->lmin, x_map->prev);
   for (i = 0; i < LEN_MAP; ++i) {
      if (x_map->map [i] < 0)  break;
      fprintf (f, "%4d "  , x_map->map [i]);
   }
   fprintf (f, "   %4d %4d %4d %4d\n", x_map->next, x_map->lmax, x_map->amax, x_map->gmax);
   fclose (f);
   /*---(complete)-----------------------*/
   return 0;
}

char
MAP_mapper           (char a_req)
{
   tCELL      *x_curr      = NULL;
   char        t           [LEN_RECD];
   yVIKEYS_view_size     (YVIKEYS_MAIN, NULL, &g_xmap.avail, NULL, &g_ymap.avail, NULL);
   if (a_req == YVIKEYS_INIT) {
      LOC__mapper   ('C');
      LOC__mapper   ('R');
      LOC__mapper   ('T');
   } else {
      LOC__mapper   ('c');
      LOC__mapper   ('r');
      LOC__mapper   ('t');
   }
   BCOL = g_xmap.gbeg;
   CCOL = g_xmap.gcur;
   ECOL = g_xmap.gend;
   BROW = g_ymap.gbeg;
   CROW = g_ymap.gcur;
   EROW = g_ymap.gend;
   CTAB = g_zmap.gcur;
   x_curr = LOC_cell_at_curr ();
   if      (x_curr == NULL || x_curr->s == NULL) {
      LOC_ref (CTAB, CCOL, CROW, 0, t);
      yVIKEYS_source (t, "");
   } else {
      yVIKEYS_source (x_curr->label, x_curr->s);
   }
   return 0;
}

/*> char                                                                              <* 
 *> LOC_row_map          (char a_type)                                                <* 
 *> {                                                                                 <* 
 *>    if (a_type == YVIKEYS_INIT)  LOC_mapper   ('R');                               <* 
 *>    else                         LOC_mapper   ('r');                               <* 
 *>    LOC__mapprint ('r');                                                            <* 
 *>    return 0;                                                                      <* 
 *> }                                                                                 <*/



/*====================------------------------------------====================*/
/*===----                           tab related                        ----===*/
/*====================------------------------------------====================*/
PRIV void  o___TABS____________o () { return; }

char         /*-> tbd --------------------------------[ leaf   [ge.320.113.20]*/ /*-[00.0000.184.I]-*/ /*-[--.---.---.--]-*/
LOC_tab_valid        (short a_tab)
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
LOC_tab_index        (char  a_abbr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char       *x_valid     = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   char       *p           = NULL;
   int         x_index     =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_abbr == 0   )  return rce;
   p = strchr (x_valid, a_abbr);
   --rce;  if (p      == NULL)  return rce;
   /*---(convert to index)---------------*/
   x_index = (int) (p - x_valid);
   /*---(complete)-----------------------*/
   return x_index;
}

char         /*-> tbd --------------------------------[ ------ [ge.220.122.11]*/ /*-[00.0000.10#.!]-*/ /*-[--.---.---.--]-*/
LOC_tab_type         (short a_tab)
{
   char        rce         =  -20;
   char        rc          =    0;
   rc = LOC_tab_valid (a_tab);
   if (rc < 0) return rc;
   return s_tabs [a_tab].type;
}

short        /*-> indicate if tab is used ------------[ ------ [gn.210.212.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
LOC_tab_used         (short a_tab)
{
   char rc = LOC_tab_valid (a_tab);
   if (rc < 0) return rc;
   return s_tabs [a_tab].c;
}

char         /*-> tbd --------------------------------[ ------ [ge.320.222.11]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
LOC_tab_retype       (short a_tab, char a_type)
{
   char        rce         =  -20;
   char        rc          =    0;
   rc = LOC_tab_valid (a_tab);
   if (rc < 0) return rc;
   s_tabs [a_tab].type = a_type;
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.320.223.21]*/ /*-[00.0000.10#.4]-*/ /*-[--.---.---.--]-*/
LOC_tab_defname      (short a_tab, char *a_name)
{
   char        rce         =  -20;
   char        rc          =    0;
   rc = LOC_tab_valid (a_tab);
   if (rc < 0) return rc;
   --rce;  if (a_name  == NULL)                       return rce;
   sprintf (a_name, "tab_%02d", a_tab);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.320.223.21]*/ /*-[00.0000.404.A]-*/ /*-[--.---.---.--]-*/
LOC_tab_name         (short a_tab, char *a_name)
{
   char        rce         =  -20;
   char rc = LOC_tab_valid (a_tab);
   if (rc < 0) return rc;
   --rce;  if (a_name  == NULL)                       return rce;
   strlcpy (a_name, s_tabs [a_tab].name, LEN_ABBR);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.330.125.41]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
LOC_tab_rename_curr  (char *a_name)
{
   char        rce         =  -20;
   char rc = LOC_tab_valid (CTAB);
   if (rc < 0) return rc;
   --rce;  if (a_name  == NULL)                       return rce;
   --rce;  if (a_name [0] == '\0')                    return rce;
   --rce;  if (strlen (a_name) >= LEN_ABBR)           return rce;
   strlcpy (s_tabs [CTAB].name, a_name, LEN_ABBR);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.330.225.41]*/ /*-[00.0000.104.C]-*/ /*-[--.---.---.--]-*/
LOC_tab_rename       (short a_tab, char *a_name)
{
   char        rce         =  -20;
   char rc = LOC_tab_valid (a_tab);
   if (rc < 0) return rc;
   --rce;  if (a_name  == NULL)                       return rce;
   --rce;  if (a_name [0] == '\0')                    return rce;
   --rce;  if (strlen (a_name) >= LEN_ABBR)           return rce;
   strlcpy (s_tabs [a_tab].name, a_name, LEN_ABBR);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.320.223.22]*/ /*-[00.0000.00#.D]-*/ /*-[--.---.---.--]-*/
LOC_tab_size         (short a_tab, char *a_max)
{
   char        rce         =  -20;
   char rc = LOC_tab_valid (a_tab);
   if (rc < 0) return rc;
   --rce;  if (a_max   == NULL)                       return rce;
   LOC_ref (a_tab, s_tabs[a_tab].ncol - 1, s_tabs[a_tab].nrow - 1, 0, a_max);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gc.E82.112.31]*/ /*-[02.0000.073.!]-*/ /*-[--.---.---.--]-*/
LOC_tab_switch         (short a_tab)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(begin)--------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_value   ("a_tab"     , a_tab);
   /*---(non-init run)-------------------*/
   if (a_tab >=  0) {
      /*---(defense)---------------------*/
      rc = LOC_tab_valid (a_tab);
      DEBUG_LOCS   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
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
   /*---(init run)-----------------------*/
   else {
      CTAB      = 0;
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
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gc.220.112.31]*/ /*-[00.0000.103.!]-*/ /*-[--.---.---.--]-*/
LOC_tab_switch_char    (char a_tab)
{
   short       x_tab       = 0;
   if      (a_tab >= '0' && a_tab <= '9')   x_tab = a_tab - '0';
   else if (a_tab >= 'A' && a_tab <= 'Z')   x_tab = a_tab - 'A' + 10;
   else    return -1;
   return LOC_tab_switch (x_tab);
}

char         /*-> tbd --------------------------------[ ------ [ge.532.143.22]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
LOC_tab_resize_curr  (char *a_max)
{
   char        rce         = -10;
   char        rc          =   0;
   int         x_col       =   0;
   int         x_row       =   0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   rc = LOC_parse  (a_max, NULL  , &x_col, &x_row, NULL);
   DEBUG_LOCS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = LOC_legal  (CTAB , x_col, x_row, CELL_EXACT);
   DEBUG_LOCS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.632.153.22]*/ /*-[01.0000.104.U]-*/ /*-[--.---.---.--]-*/
LOC_tab_resize       (char *a_max)
{
   char        rce         = -10;
   char        rc          =   0;
   int         x_tab       =   0;
   int         x_col       =   0;
   int         x_row       =   0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   rc = LOC_parse  (a_max, &x_tab, &x_col, &x_row, NULL);
   DEBUG_LOCS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = LOC_legal  (x_tab, x_col, x_row, CELL_EXACT);
   DEBUG_LOCS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  LOC_tab_colwide  (short a_tab) { return s_tabs [a_tab].defwide; }
char  LOC_tab_rowtall  (short a_tab) { return s_tabs [a_tab].deftall; }

char         /*-> tbd --------------------------------[ leaf   [ge.632.233.70]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
LOC_tab_line       (char a_tab, char *a_list)
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
LOC_tab_status     (char a_tab, char *a_list)
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

char        LOC_tab_first        (void)  { return LOC_tab_switch (0); }
char        LOC_tab_prev         (void)  { return LOC_tab_switch (CTAB - 1); }
char        LOC_tab_next         (void)  { return LOC_tab_switch (CTAB + 1); }
char        LOC_tab_last         (void)  { return LOC_tab_switch (MAX_TABS - 1); }



/*====================------------------------------------====================*/
/*===----                           col related                        ----===*/
/*====================------------------------------------====================*/
PRIV void  o___COLUMNS_________o () { return; }

char         /*-> clear all column customizations ----[ leaf   [ge.A53.123.40]*/ /*-[02.0000.014.!]-*/ /*-[--.---.---.--]-*/
LOC_col_clear        (short a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   short       x_col       =   0;
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
   /*---(initialize columns)-------------*/
   DEBUG_LOCS   yLOG_snote   ("clear columns to defaults");
   DEBUG_LOCS   yLOG_svalue  ("MAX_COLS"  , MAX_COLS);
   for (x_col = 0; x_col < MAX_COLS; ++x_col) {
      /*---(characteristics)-------------*/
      s_tabs [a_tab].cols [x_col].w       = s_tabs [a_tab].defwide;
      s_tabs [a_tab].cols [x_col].x       = 0;
      s_tabs [a_tab].cols [x_col].c       = 0;
      /*---(labels)----------------------*/
      if        (x_col < 26)  {
         s_tabs [a_tab].cols [x_col].l[0] = '-';
         s_tabs [a_tab].cols [x_col].l[1] = x_col + 'a';
      } else  {
         s_tabs [a_tab].cols [x_col].l[0] = (x_col / 26) - 1 + 'a';
         s_tabs [a_tab].cols [x_col].l[1] = (x_col % 26) + 'a';
      }
      s_tabs [a_tab].cols [x_col].l[2] = '\0';
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
LOC_col_valid        (short a_tab, short a_col)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (a_tab   < 0)                           return rce;
   --rce;  if (a_tab   >= MAX_TABS)                   return rce;
   --rce;  if (a_col   < 0)                           return rce;
   --rce;  if (a_col   >= MAX_COLS)                   return rce;
   /*---(complete)-----------------------*/
   return 0;
}

short        /*-> return default col size ------------[ leaf   [gn.210.002.00]*/ /*-[00.0000.10#.6]-*/ /*-[--.---.---.--]-*/
LOC_col_defmax       (void)
{
   return DEF_COLS;
}

short        /*-> return max col for tab -------------[ ------ [gn.210.113.11]*/ /*-[00.0000.704.D]-*/ /*-[--.---.---.--]-*/
LOC_col_max          (short a_tab)
{
   char rc = LOC_col_valid (a_tab, 0);
   if (rc < 0) return rc;
   return s_tabs [a_tab].ncol;
}

short        /*-> indicate if column is used ---------[ ------ [gn.210.212.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
LOC_col_used         (short a_tab, short a_col)
{
   char rc = LOC_col_valid (a_tab, a_col);
   if (rc < 0) return rc;
   return s_tabs [a_tab].cols [a_col].c;
}

short        /*-> return the col xpos ----------------[ ------ [gn.210.213.11]*/ /*-[00.0000.503.!]-*/ /*-[--.---.---.--]-*/
LOC_col_xpos         (short a_tab, short a_col)
{
   char rc = LOC_col_valid (a_tab, a_col);
   if (rc < 0) return rc;
   return s_tabs [a_tab].cols [a_col].x;
}

char         /*-> set a new col x-pos ----------------[ ------ [gc.210.312.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
LOC_col_xset         (short a_tab, short a_col, short a_pos)
{
   char rc = LOC_col_valid (a_tab, a_col);
   if (rc < 0) return rc;
   s_tabs [a_tab].cols [a_col].x = a_pos;
   return 0;
}

char         /*-> return the col xpos ----------------[ ------ [gc.210.313.21]*/ /*-[00.0000.204.!]-*/ /*-[--.---.---.--]-*/
LOC_col_label        (short a_tab, short a_col, char *a_label)
{
   char rc = LOC_col_valid (a_tab, a_col);
   if (rc < 0) return rc;
   if (a_label != NULL)  strlcpy (a_label, s_tabs [a_tab].cols [a_col].l, LEN_LABEL);
   return 0;
}

char         /*-> set the default width --------------[ ------ [gc.210.213.11]*/ /*-[00.0000.G03.!]-*/ /*-[--.---.---.--]-*/
LOC_col_defwidth     (short a_tab, short a_size)
{
   char        rc          =    0;
   short       x_col       =    0;
   rc = LOC_tab_valid (a_tab);
   if (rc < 0) return rc;
   if (a_size  < MIN_WIDTH)    a_size = MIN_WIDTH;
   if (a_size  > MAX_WIDTH)    a_size = MAX_WIDTH;
   for (x_col = 0; x_col < MAX_ROWS; ++x_col) {
      if (s_tabs [a_tab].cols [x_col].w != s_tabs [a_tab].defwide)  continue;
      if (s_tabs [a_tab].cols [x_col].c >  0)                       continue;
      s_tabs [a_tab].cols [x_col].w = a_size;
   }
   s_tabs [a_tab].defwide = a_size;
   return 0;
}

char         /*-> return the col width ---------------[ ------ [gc.210.213.11]*/ /*-[00.0000.G03.!]-*/ /*-[--.---.---.--]-*/
LOC_col_width        (short a_tab, short a_col)
{
   char rc = LOC_col_valid (a_tab, a_col);
   if (rc < 0) return rc;
   return s_tabs [a_tab].cols [a_col].w;
}

char         /*-> change the col width ---------------[ ------ [gc.320.312.31]*/ /*-[00.0000.404.5]-*/ /*-[--.---.---.--]-*/
LOC_col_widen        (short a_tab, short a_col, short a_size)
{
   char rc = LOC_col_valid (a_tab, a_col);
   if (rc < 0) return rc;
   if (a_size  < MIN_WIDTH)    a_size = MIN_WIDTH;
   if (a_size  > MAX_WIDTH)    a_size = MAX_WIDTH;
   s_tabs [a_tab].cols [a_col].w = a_size;
   return 0;
}

char         /*-> change the frozen cols -------------[ ------ [gc.430.323.31]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
LOC_col_freeze       (short a_tab, short a_bcol, short a_ecol)
{
   char rc = LOC_col_valid (a_tab, a_bcol);
   if (rc < 0) return rc;
   rc = LOC_col_valid (a_tab, a_ecol);
   if (rc < 0) return rc;
   short  x_col;
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
LOC_col_unfreeze     (short a_tab)
{
   char rc = LOC_col_valid (a_tab, 0);
   if (rc < 0) return rc;
   s_tabs [a_tab].froz_col    = '-';
   s_tabs [a_tab].froz_bcol   = 0;
   s_tabs [a_tab].froz_ecol   = 0;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                           row related                        ----===*/
/*====================------------------------------------====================*/
PRIV void  o___ROWS____________o () { return; }

char         /*-> clear all row customizations -------[ leaf   [ge.843.123.20]*/ /*-[01.0000.014.!]-*/ /*-[--.---.---.--]-*/
LOC_row_clear        (short a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   short       x_row       =   0;
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
LOC_row_valid        (short a_tab, short a_row)
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

short        /*-> return default row size ------------[ leaf   [gn.210.002.00]*/ /*-[00.0000.10#.6]-*/ /*-[--.---.---.--]-*/
LOC_row_defmax       (void)
{
   return DEF_ROWS;
}

short        /*-> return max row for tab -------------[ ------ [gn.210.113.11]*/ /*-[00.0000.604.D]-*/ /*-[--.---.---.--]-*/
LOC_row_max          (short a_tab)
{
   char rc = LOC_row_valid (a_tab, 0);
   if (rc < 0) return rc;
   return s_tabs [a_tab].nrow;
}

short        /*-> return the row used ----------------[ ------ [gn.210.213.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
LOC_row_used         (short a_tab, short a_row)
{
   char rc = LOC_row_valid (a_tab, 0);
   if (rc < 0) return rc;
   return s_tabs [a_tab].rows [a_row].c;
}

short        /*-> return the row ypos ----------------[ ------ [gn.210.213.11]*/ /*-[00.0000.603.!]-*/ /*-[--.---.---.--]-*/
LOC_row_ypos         (short a_tab, short a_row)
{
   char rc = LOC_row_valid (a_tab, 0);
   if (rc < 0) return rc;
   return s_tabs [a_tab].rows [a_row].y;
}

char         /*-> set a new row y-pos ----------------[ ------ [gc.210.312.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
LOC_row_yset         (short a_tab, short a_row, short a_pos)
{
   char rc = LOC_row_valid (a_tab, a_row);
   if (rc < 0) return rc;
   s_tabs [a_tab].rows [a_row].y = a_pos;
   return 0;
}

char         /*-> set the default width --------------[ ------ [gc.210.213.11]*/ /*-[00.0000.G03.!]-*/ /*-[--.---.---.--]-*/
LOC_row_defheight    (short a_tab, short a_size)
{
   char        rc          =    0;
   short       x_row       =    0;
   rc = LOC_tab_valid (a_tab);
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
LOC_row_height       (short a_tab, short a_row)
{
   char rc = LOC_row_valid (a_tab, a_row);
   if (rc < 0) return rc;
   return s_tabs [a_tab].rows [a_row].h;
}

char         /*-> change the row height --------------[ ------ [gc.320.312.31]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
LOC_row_heighten     (short a_tab, short a_row, short a_size)
{
   char rc = LOC_row_valid (a_tab, a_row);
   if (rc < 0) return rc;
   if (a_size  < MIN_HEIGHT)   a_size = MIN_HEIGHT;
   if (a_size  > MAX_HEIGHT)   a_size = MAX_HEIGHT;
   s_tabs [a_tab].rows [a_row].h = a_size;
   return 0;
}

char         /*-> change the frozen rows -------------[ ------ [gc.430.323.31]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
LOC_row_freeze       (short a_tab, short a_brow, short a_erow)
{
   char rc = LOC_row_valid (a_tab, a_brow);
   if (rc < 0) return rc;
   rc = LOC_row_valid (a_tab, a_erow);
   if (rc < 0) return rc;
   short  x_row;
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
LOC_row_unfreeze     (short a_tab)
{
   char rc = LOC_row_valid (a_tab, 0);
   if (rc < 0) return rc;
   s_tabs [a_tab].froz_row    = '-';
   s_tabs [a_tab].froz_brow   = 0;
   s_tabs [a_tab].froz_erow   = 0;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNIT_TEST_______o () { return; }

char*        /*-> unit test accessor -----------------[ light  [us.B60.2A3.F2]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
LOC__unit          (char *a_question, char *a_label)
{
   /*---(locals)-------------------------*/
   char        rc          =  0;
   int         i           =  0;
   int         x_tab       =  0;
   int         x_col       =  0;
   int         x_row       =  0;
   char        x_abs       =  0;
   char        x_beg       [LEN_LABEL]   = "";
   char        x_end       [LEN_LABEL]   = "";
   char        x_cur       [LEN_LABEL]   = "";
   char        x_max       [LEN_LABEL]   = "";
   /*---(parse location)-----------------*/
   strcpy  (unit_answer, "LOC              : label could not be parsed");
   if (rc >= 0)  rc = LOC_parse  (a_label, &x_tab, &x_col, &x_row, &x_abs);
   if (rc <  0)  return unit_answer;
   /*---(prepare data)-------------------*/
   strcpy  (unit_answer, "LOC              : locations could not be prepared");
   if (rc >= 0)  rc = LOC_ref    (x_tab, s_tabs [x_tab].bcol, s_tabs [x_tab].brow, 0, x_beg);
   if (rc >= 0)  rc = LOC_ref    (x_tab, s_tabs [x_tab].ecol, s_tabs [x_tab].erow, 0, x_end);
   if (rc >= 0)  rc = LOC_ref    (x_tab, s_tabs [x_tab].ccol, s_tabs [x_tab].crow, 0, x_cur);
   if (rc >= 0)  rc = LOC_ref    (x_tab, s_tabs [x_tab].ncol - 1, s_tabs [x_tab].nrow - 1, 0, x_max);
   if (rc <  0)  return unit_answer;
   /*---(overall)------------------------*/
   strcpy  (unit_answer, "LOC              : question not understood");
   if      (strcmp(a_question, "tab_info"      ) == 0) {
      snprintf(unit_answer, LEN_UNIT, "LOC tab info (%c) : %-12.12s %-7.7s %-7.7s %-7.7s %-7.7s %d", a_label [0], s_tabs [x_tab].name, x_beg, x_end, x_cur, x_max, s_tabs [x_tab].c);
   }
   else if (strcmp(a_question, "cell_size"     ) == 0) {
      snprintf(unit_answer, LEN_UNIT, "LOC cell size    : width=%3d, height=%3d", s_tabs [x_tab].cols [x_col].w, s_tabs [x_tab].rows [x_row].h);
   }
   else if (strcmp(a_question, "loc_who"       )  == 0) {
      snprintf (unit_answer, LEN_UNIT, "LOC occupant/who : ptr=%10p, tab=%4d, col=%4d, row=%4d", s_tabs [x_tab].sheet [x_col][x_row], x_tab, x_col, x_row);
   }
   else if (strcmp(a_question, "loc_col"       )  == 0) {
      snprintf (unit_answer, LEN_UNIT, "LOC col stats    : tab=%4d, col=%4d, width =%4d, used=%4d", x_tab, x_col, s_tabs[x_tab].cols[x_col].w, s_tabs[x_tab].cols[x_col].c);
   }
   else if (strcmp(a_question, "loc_row"       )  == 0) {
      snprintf (unit_answer, LEN_UNIT, "LOC row stats    : tab=%4d, row=%4d, height=%4d, used=%4d", x_tab, x_row, s_tabs[x_tab].rows[x_row].h, s_tabs[x_tab].rows[x_row].c);
   }
   else if (strcmp(a_question, "tab_def"       )  == 0) {
      snprintf (unit_answer, LEN_UNIT, "LOC tab defaults : col=%2d, row=%2d", s_tabs[x_tab].defwide, s_tabs[x_tab].deftall);
   }
   /*> else if (strcmp(a_question, "tab_beg"       ) == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_UNIT, "s_move tab beg   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].bcol, s_tabs [a_num].brow);   <* 
    *> }                                                                                                                                  <* 
    *> else if (strcmp(a_question, "tab_pos"       ) == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_UNIT, "s_move tab pos   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ccol, s_tabs [a_num].crow);   <* 
    *> }                                                                                                                                  <* 
    *> else if (strcmp(a_question, "tab_max" )       == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_UNIT, "s_move tab max   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ncol, s_tabs [a_num].nrow);   <* 
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
   char        t           [LEN_STR]   = "";
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
         snprintf (unit_answer, LEN_UNIT, "s_loc cell       : requested a null cell");
      } else if (x_curr == NULL) {
         snprintf (unit_answer, LEN_UNIT, "s_loc cell       : ptr=%9p, cell not found in list"   , a_cell);
      } else {
         snprintf (unit_answer, LEN_UNIT, "s_loc cell       : ptr=%9p, tab=%4d, col=%4d, row=%4d", x_curr, x_curr->tab, x_curr->col, x_curr->row);
      }
   }
   else if (strcmp(a_question, "loc_who"       )  == 0) {
      if (a_cell != NULL) {
         snprintf (unit_answer, LEN_UNIT, "s_loc occupant   : ptr=%9p, tab=%4d, col=%4d, row=%4d", x_curr, x_curr->tab, x_curr->col, x_curr->row);
      } else {
         snprintf (unit_answer, LEN_UNIT, "s_loc occupant   : ptr=%9p, no cell attached", a_cell);
      }
   }
   else if (strcmp (a_question, "cell_where")     == 0) {
      if (a_cell == NULL) {

      } else if (rc == 0) {
         snprintf (unit_answer, LEN_UNIT, "s_cell location  : tab=%4d, col=%4d, row=%4d", x_curr->tab, x_curr->col, x_curr->row);
      } else {
         snprintf (unit_answer, LEN_UNIT, "s_cell location  : not found in cell list");
      }
   }
   else if (strcmp(a_question, "cell_list")      == 0) {
      snprintf(unit_answer, LEN_UNIT, "s_cell main list : num=%4d, head=%9p, tail=%9p", NCEL, hcell, tcell);
   }
   else if (strcmp(a_question, "loc_label")      == 0) {
      snprintf(unit_answer, LEN_UNIT, "s_loc label      : %s", s_label);
   }
   /*> else if (strcmp(a_question, "tab_beg"       ) == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_UNIT, "s_move tab beg   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].bcol, s_tabs [a_num].brow);   <* 
    *> }                                                                                                                                  <* 
    *> else if (strcmp(a_question, "tab_pos"       ) == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_UNIT, "s_move tab pos   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ccol, s_tabs [a_num].crow);   <* 
    *> }                                                                                                                                  <* 
    *> else if (strcmp(a_question, "tab_max" )       == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_UNIT, "s_move tab max   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ncol, s_tabs [a_num].nrow);   <* 
    *> }                                                                                                                                  <*/
   /*---(complete)-----------------------*/
   return unit_answer;
}
