/*============================----beg-of-source---============================*/

#include   "gyges.h"


static char    s_label     [LEN_RECD]   = "";



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___PROGRAM_________o () { return; }

char         /*--> initialize location function ----------[ shoot  [ ------ ]-*/
LOC_init             (void)
{
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(clean tabs)---------------------*/
   LOC__purge    ();
   /*---(set defaults)-------------------*/
   NTAB    = 1;
   CTAB    = 0;
   p_tab   = &tabs [CTAB];
   tabs [0].active  = 'y';
   /*---(column settings)----------------*/
   /*> BCOL =    0;                                                                   <* 
    *> ECOL =    tabs[0].ncol - 1;                                                    <*/
   /*---(row settings)-------------------*/
   /*> BROW =    0;                                                                   <* 
    *> EROW =    tabs[0].nrow - 1;                                                    <*/
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> clear all cell placements -------------[ leaf   [ ------ ]-*/
LOC__clear_locs      (short a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   short       x_col       =   0;
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
   --rce;  if (a_tab >= NTAB) {
      DEBUG_LOCS   yLOG_snote   ("tab too big");
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(unhook all cells)---------------*/
   /* tbd */
   /*---(initialize cells)---------------*/
   DEBUG_LOCS   yLOG_snote   ("clear cells off sheet");
   DEBUG_LOCS   yLOG_svalue  ("MAX_COLS"  , MAX_COLS);
   DEBUG_LOCS   yLOG_svalue  ("MAX_ROWS"  , MAX_ROWS);
   for (x_col = 0; x_col < MAX_COLS; ++x_col) {
      for (x_row = 0; x_row < MAX_ROWS; ++x_row) {
         tabs [a_tab].sheet [x_col][x_row] = NULL;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> clean all tab and sheet locations -----[ stem   [ ------ ]-*/
LOC__purge           (void)
{  /*---(design notes)--------------------------------------------------------*/
   /* run CELL_wrap/purge before LOC_wrap/purge so all cells are unhooked     */
   /*---(locals)-----------+-----------+-*/
   short       x_tab       = 0;
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(initialize tabs)----------------*/
   for (x_tab = 0; x_tab < MAX_TABS; ++x_tab) {
      DEBUG_LOCS   yLOG_value   ("x_tab"     , x_tab);
      /*---(main config)-----------------*/
      DEBUG_LOCS   yLOG_note    ("reset naming");
      tabs [x_tab].active  = '/';
      sprintf (t, "tab_%02d", x_tab);
      strlcpy (tabs [x_tab].name, t, LEN_RECD);
      tabs [x_tab].c       =    0;
      /*---(size limits)-----------------*/
      DEBUG_LOCS   yLOG_note    ("reset default size");
      DEBUG_LOCS   yLOG_value   ("DEF_COLS"  , DEF_COLS);
      DEBUG_LOCS   yLOG_value   ("DEF_ROWS"  , DEF_ROWS);
      tabs [x_tab].ncol    = DEF_COLS;
      tabs [x_tab].nrow    = DEF_ROWS;
      /*---(current position)------------*/
      DEBUG_LOCS   yLOG_note    ("reset current position");
      tabs [x_tab].ccol    =    0;
      tabs [x_tab].crow    =    0;
      /*---(screen position)-------------*/
      DEBUG_LOCS   yLOG_note    ("reset beginning and ending cells");
      tabs [x_tab].bcol    =    0;
      tabs [x_tab].brow    =    0;
      tabs [x_tab].ecol    =    0;
      tabs [x_tab].erow    =    0;
      /*---(initialize)------------------*/
      LOC_col_clear     (x_tab);
      LOC_row_clear     (x_tab);
      LOC__clear_locs   (x_tab);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> wrap up location function -------------[ shoot  [ ------ ]-*/
LOC_wrap             (void)
{
   LOC__purge  ();
}



/*====================------------------------------------====================*/
/*===----                     hooking and unhooking                    ----===*/
/*====================------------------------------------====================*/
PRIV void  o___HOOKING_________o () { return; }

char         /*--> attach a cell to a location -----------[ ------ [ ------ ]-*/
LOC_hook           (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* cell to be placed in a location              */
      short       a_tab,      /* tab for target location                      */
      short       a_col,      /* col for target location                      */
      short       a_row)      /* row for target location                      */
{  /*---(design notes)--------------------------------------------------------*/
   /* add the cell reference to an appropriate empty location.  nothing else  */
   /* on the tab or cell should be modified.                                  */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defense: valid cell)------------*/
   --rce;  if (a_cell == NULL)                             return rce;
   /*---(defense: already hooked)--------*/
   --rce;  if (a_cell->tab != UNHOOKED)                    return rce;
   --rce;  if (a_cell->col != UNHOOKED)                    return rce;
   --rce;  if (a_cell->row != UNHOOKED)                    return rce;
   /*---(defense: valid destination)-----*/
   --rce;
   if (LOC_legal (a_tab, a_col, a_row, CELL_FIXED) < 0)    return rce;
   /*---(defense: already occupied)------*/
   --rce;
   if (tabs[a_tab].sheet[a_col][a_row] != NULL)            return rce;
   /*---(point cell at location)---------*/
   a_cell->tab = a_tab;
   a_cell->col = a_col;
   a_cell->row = a_row;
   /*---(point location at cell)---------*/
   tabs [a_tab].sheet[a_col][a_row] = a_cell;
   if (LOC_label (a_cell, a_cell->label) < 0) {
      strcpy (a_cell->label, "tbd");
   }
   /*---(update totals)------------------*/
   ++tabs[a_tab].cols[a_col].c;
   ++tabs[a_tab].rows[a_row].c;
   ++tabs[a_tab].c;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> dettach a cell from current location --[ ------ [ ------ ]-*/
LOC_unhook         (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell)     /* cell to detach from current location         */
{  /*---(design notes)--------------------------------------------------------*/
   /* remove the cell reference from the location and mark it as              */
   /* locationless.  nothing else on the tab or cell will be modified.        */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   short       x_tab       = UNHOOKED;
   short       x_col       = UNHOOKED;
   short       x_row       = UNHOOKED;
   /*---(defense: valid cell)------------*/
   --rce;  if (a_cell == NULL)                             return rce;
   /*---(defense: already unhooked)------*/
   --rce;  if (a_cell->tab == UNHOOKED)                    return rce;
   --rce;  if (a_cell->col == UNHOOKED)                    return rce;
   --rce;  if (a_cell->row == UNHOOKED)                    return rce;
   /*---(prepare)------------------------*/
   x_tab       = a_cell->tab;
   x_col       = a_cell->col;
   x_row       = a_cell->row;
   /*---(defense: valid position)--------*/
   --rce;
   if (LOC_legal (x_tab, x_col, x_row, CELL_FIXED) < 0)   return rce;
   /*---(defense: wrong cell)------------*/
   --rce;
   if (tabs [x_tab].sheet[x_col][x_row] != a_cell)         return rce;
   /*---(update totals)------------------*/
   --tabs[x_tab].cols[x_col].c;
   --tabs[x_tab].rows[x_row].c;
   --tabs[x_tab].c;
   /*---(detach)-------------------------*/
   a_cell->tab = UNHOOKED;
   a_cell->col = UNHOOKED;
   a_cell->row = UNHOOKED;
   strcpy (a_cell->label, "tbd");
   tabs [x_tab].sheet[x_col][x_row] = NULL;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> move a cell between locations ---------[ ------ [ ------ ]-*/
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
   source      = tabs[a_stab].sheet[a_scol][a_srow];
   /*---(defense: target)----------------*/
   --rce;
   if (LOC_legal (a_ttab, a_tcol, a_trow, CELL_FIXED) != 0)  return rce;
   target      = tabs[a_ttab].sheet[a_tcol][a_trow];
   /*---(overwrite as necessary)---------*/
   if (target  != NULL) {
      CELL__delete (CHG_NOHIST, a_ttab, a_tcol, a_trow);
   }
   /*---(set location)-------------------*/
   if (source != NULL) {
      LOC_unhook  (source);
      LOC_hook    (source, a_ttab, a_tcol, a_trow);
   }
   /*---(update printable)---------------*/
   if (tabs[a_stab].cols[a_scol].w != tabs[a_ttab].cols[a_tcol].w) {
      CELL_printable (source);
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          locations                           ----===*/
/*====================------------------------------------====================*/
PRIV void  o___LOCATION________o () { return; }

char         /*--> verify that a location is legal -------[ ------ [ ------ ]-*/
LOC_legal          (
      /*----------+-----------+-----------------------------------------------*/
      short       a_tab,      /* tab number                                   */
      short       a_col,      /* column number                                */
      short       a_row,      /* row number                                   */
      char        a_adapt)    /* y/n should enlarge boundaries to fit         */
{  /*---(design notes)--------------------------------------------------------*/
   /* tests the tab, col, and row against minimum and maximum limits as well  */
   /* as against current limits as a service to other functions.              */
   /* additionally, if requested, the function can expand the current limits  */
   /* to include the requested row and column if it is within the maximum     */
   /* limits.                                                                 */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   short       new_col     = 0;             /* holder for enlarged area       */
   short       new_row     = 0;             /* holder for enlarged area       */
   /*---(check absolute boudaries)-------*/
   --rce;  if (a_tab <  0       )      return rce;
   --rce;  if (a_col <  0       )      return rce;
   --rce;  if (a_row <  0       )      return rce;
   --rce;  if (a_tab >= MAX_TABS)      return rce;
   --rce;  if (a_col >= MAX_COLS)      return rce;
   --rce;  if (a_row >= MAX_ROWS)      return rce;
   /*---(check tab out of range)---------*/
   --rce;  if (a_tab >= NTAB )      return rce;
   /*---(save values)--------------------*/
   new_col = tabs[a_tab].ncol;
   new_row = tabs[a_tab].nrow;
   /*---(check current col limits)-------*/
   --rce;
   if (a_col >= tabs[a_tab].ncol) {
      if (a_adapt == CELL_ADAPT)   new_col = a_col + 1;
      else                             return rce;
   }
   /*---(check current row limits)-------*/
   --rce;
   if (a_row >= tabs[a_tab].nrow) {
      if (a_adapt == CELL_ADAPT)   new_row = a_row + 1;
      else                             return rce;
   }
   /*---(make changes if requested)------*/
   if (a_adapt == CELL_ADAPT) {
      if (new_col != tabs[a_tab].ncol)  tabs[a_tab].ncol = new_col;
      if (new_row != tabs[a_tab].nrow)  tabs[a_tab].nrow = new_row;
   }
   /*---(complete)-----------------------*/
   return 0;
}

tCELL*       /*--> return the cell at current loc --------[ leaf   [ ------ ]-*/
LOC_cell_at_curr     (void)
{
   return tabs [CTAB].sheet[CCOL][CROW];
}

tCELL*       /*--> return the cell at a location ---------[ leaf   [ ------ ]-*/
LOC_cell_at_loc      (short a_tab, short a_col, short a_row)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   /*---(defenses)-----------------------*/
   rc = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);
   if (rc < 0)    return NULL;
   /*---(complete)-----------------------*/
   return tabs[a_tab].sheet[a_col][a_row];
}

tCELL*       /*--> return the cell at a location ---------[ ------ [ ------ ]-*/
LOC_cell_labeled   (char *a_label)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   short       x_tab       =   0;           /* working tab value              */
   short       x_col       =   0;           /* working col value              */
   short       x_row       =   0;           /* working row value              */
   tCELL      *x_curr      = NULL;
   /*---(defenses)-----------------------*/
   rc = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);
   if (rc < 0)  return NULL;
   x_curr = LOC_cell_at_loc  (x_tab, x_col, x_row);
   if (x_curr == NULL)    return NULL;
   /*---(complete)-----------------------*/
   return x_curr;
}

char         /*--> go directly to a specific cell---------[ leaf   [ 123y4x ]-*/
LOC_jump           (
      /*----------+-----------+-----------------------------------------------*/
      short       a_tab,      /* tab number                                   */
      short       a_col,      /* column number                                */
      short       a_row)      /* row number                                   */
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   /*---(defenses)-----------------------*/
   rc = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);
   if (rc < 0)    return rc;
   /*---(update globals)-----------------*/
   CTAB          = a_tab;
   p_tab         = &tabs[CTAB];
   CCOL          = a_col;
   CROW          = a_row;
   /*---(selection)----------------------*/
   VISU_update (a_tab, a_col, a_row);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        cell references                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___REFERENCES______o () { return; }

char         /*--> convert position into label -----------[ ------ [ ------ ]-*/
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
   /*---(locals)-----------+-----------+-*/
   char        x_cname     [100]       = "";
   char        x_tref      [5]         = "";
   char        x_cref      [5]         = "";
   char        x_rref      [5]         = "";
   char        rce         = -10;           /* return code for errors         */
   char        rc          = 0;             /* generic return code            */
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
   sprintf (s_label, "%s%d%s%s%s%d", x_tref, a_tab, x_cref, x_cname, x_rref, a_row + 1);
   strcpy  (a_final, s_label);
   /*---(complete)-----------------------*/
   return  0;
}

char         /*--> convert a cell into a label -----------[ ------ [ ------ ]-*/
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
   /*---(defense: valid cell)------------*/
   --rce;  if (a_cell  == NULL)             return rce;
   /*---(defense: valid output)----------*/
   --rce;  if (a_final == NULL)             return rce;
   /*---(fiugure out reference)----------*/
   rc = LOC_ref (a_cell->tab, a_cell->col, a_cell->row, 0, a_final);
   --rce;  if (rc <  0)                     return rce;
   --rce;  if (rc >  0)                     return rc;
   /*---(complete)-----------------------*/
   return  0;
}

char         /*--> convert a cell into a coordinates -----[ ------ [ ------ ]-*/
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

char         /*--> convert label into tab, col, row ------[ ------ [ ------ ]-*/
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
   /*---(begin)--------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_info    ("a_label"   , a_label);
   DEBUG_LOCS   yLOG_point   ("a_tab"     , a_tab);
   DEBUG_LOCS   yLOG_point   ("a_col"     , a_col);
   DEBUG_LOCS   yLOG_point   ("a_row"     , a_row);
   DEBUG_LOCS   yLOG_point   ("a_abs"     , a_abs);
   /*---(defense: empty label)-----------*/
   --rce;  if (a_label == NULL) {
      DEBUG_LOCS   yLOG_note    ("aborting, no a_label means no point");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check sizes)--------------------*/
   len = strlen (a_label);
   DEBUG_LOCS   yLOG_value   ("len"       , len);
   --rce;  if (len <=  0) {
      DEBUG_LOCS   yLOG_note    ("aborting, a_label contains nothing, no point");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (len >  12) {  /* a1 to $24$ab$12345 */
      DEBUG_LOCS   yLOG_note    ("aborting, a_label too long for legal address");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: check special)---------*/
   --rce;  if (strcmp (a_label, "root") == 0) {
      DEBUG_LOCS   yLOG_note    ("aborting, no need to parse root");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (strcmp (a_label, ".") == 0) {
      DEBUG_LOCS   yLOG_note    ("aborting, no need to parse dot (.)");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (strcmp (a_label, "-") == 0) {
      DEBUG_LOCS   yLOG_note    ("aborting, no need to parse dash (-)");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(look for absolute tab)----------*/
   DEBUG_LOCS   yLOG_char    ("CH"        , a_label[s_tab]);
   if (a_label[s_tab] == '@') {
      x_abs  = 7;
      ++s_tab;
      DEBUG_LOCS   yLOG_char    ("CH"        , a_label[s_tab]);
   }
   if (a_label[s_tab] == '$') {
      x_abs += 4;
      ++s_tab;
   }
   DEBUG_LOCS   yLOG_value   ("x_abs"     , x_abs);
   DEBUG_LOCS   yLOG_value   ("s_tab"     , s_tab);
   /*---(parse tab characters)-----------*/
   s_col = s_tab;
   for (i = s_tab; i < s_tab + 2; ++i) {
      DEBUG_LOCS   yLOG_char    ("CH"        , a_label[i]);
      if (strchr ("0123456789", a_label[i]) == 0)   break;
      if (i >  s_tab)  x_tab *= 10;
      x_tab += a_label[i] - '0';
      ++s_col;
   }
   if (s_col == s_tab && x_abs == 4)  x_abs = 1;
   DEBUG_LOCS   yLOG_value   ("x_tab"     , x_tab);
   DEBUG_LOCS   yLOG_value   ("s_col"     , s_col);
   if (a_tab != NULL)  *a_tab = x_tab;
   /*---(look for absolute col)----------*/
   DEBUG_LOCS   yLOG_char    ("CH"        , a_label[s_col]);
   if (a_label [s_col] == '$') {
      x_abs += 1;
      ++s_col;
   }
   DEBUG_LOCS   yLOG_value   ("x_abs"     , x_abs);
   DEBUG_LOCS   yLOG_value   ("s_col"     , s_col);
   /*---(parse col characters)-----------*/
   s_row = s_col;
   for (i = s_col; i < s_col + 2; ++i) {
      DEBUG_LOCS   yLOG_char    ("CH"        , a_label[i]);
      if (strchr ("abcdefghijklmnopqrstuvwxyz", a_label[i]) == 0)   break;
      if (i >  s_col)  x_col *= 26;
      x_col += a_label[i] - 'a' + 1;
      ++s_row;
   }
   DEBUG_LOCS   yLOG_value   ("x_col"     , x_col);
   DEBUG_LOCS   yLOG_value   ("s_row"     , s_row);
   --rce;  if (s_row == s_col) {
      DEBUG_LOCS   yLOG_note    ("aborting, no column found");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --x_col;
   DEBUG_LOCS   yLOG_value   ("x_col fix" , x_col);
   if (a_col != NULL)  *a_col = x_col;
   /*---(look for absolute row)----------*/
   DEBUG_LOCS   yLOG_char    ("CH"        , a_label[s_row]);
   if (a_label [s_row] == '$') {
      x_abs += 2;
      ++s_row;
   }
   DEBUG_LOCS   yLOG_value   ("x_abs"     , x_abs);
   DEBUG_LOCS   yLOG_value   ("s_row"     , s_row);
   if (x_abs > 7)  x_abs = 7;
   if (a_abs != NULL)  *a_abs = x_abs;
   /*---(parse row characters)-----------*/
   e_row = s_row;
   for (i = s_row; i < len; ++i) {
      DEBUG_LOCS   yLOG_char    ("CH"        , a_label[i]);
      if (strchr ("0123456789", a_label[i]) == 0)   break;
      if (i >  s_row)  x_row *= 10;
      x_row += a_label[i] - '0';
      ++e_row;
   }
   DEBUG_LOCS   yLOG_value   ("x_row"     , x_row);
   DEBUG_LOCS   yLOG_value   ("e_row"     , e_row);
   --rce;  if (e_row != len  ) {
      DEBUG_LOCS   yLOG_note    ("aborting, no row found");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --x_row;
   if (a_row != NULL)  *a_row = x_row;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return  0;
}



/*====================------------------------------------====================*/
/*===----                           tab related                        ----===*/
/*====================------------------------------------====================*/
PRIV void  o___TABS____________o () { return; }

char 
LOC_tab_chg_max      (short a_size)
{
   char        rce         = -10;
   int         i           =   0;
   --rce;  if (a_size  <  MIN_TABS)              return rce;
   --rce;  if (a_size  >= MAX_TABS)              return rce;
   my.ntab = a_size;
   for (i = 0; i < MAX_TABS; ++i) {
      if      (i >= a_size)             tabs [i].active = '/';
      else if (tabs [i].active == '/')  tabs [i].active = '_';
   }
   return 0;
}

char*
LOC_tab_get_name     (short a_tab)
{
   if (a_tab < 0)      return "";
   if (a_tab >= NTAB)  return "";
   return tabs [a_tab].name;
}

char 
LOC_tab_chg_name     (short a_tab, char *a_name)
{

   char        rce         = -10;
   --rce;  if (a_tab < 0)      return rce;
   --rce;  if (a_tab >= NTAB)  return rce;
   strlcpy (tabs [a_tab].name, a_name, LEN_DESC);
   return 0;
}

char 
LOC_tab_activate     (short a_tab)
{
   char        rce         = -10;
   --rce;  if (a_tab < 0)      return rce;
   --rce;  if (a_tab >= NTAB)  return rce;
   tabs [a_tab].active = 'y';
   return 0;
}

char 
LOC_tab_deactivate   (short a_tab)
{
   char        rce         = -10;
   --rce;  if (a_tab < 0)      return rce;
   --rce;  if (a_tab >= NTAB)  return rce;
   tabs [a_tab].active = '_';
   return 0;
}



/*====================------------------------------------====================*/
/*===----                           col related                        ----===*/
/*====================------------------------------------====================*/
PRIV void  o___COLUMNS_________o () { return; }

char         /*--> clear all column customizations -------[ septal [ 1----- ]-*/
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
   --rce;  if (a_tab >= NTAB) {
      DEBUG_LOCS   yLOG_snote   ("tab too big");
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(initialize columns)-------------*/
   DEBUG_LOCS   yLOG_snote   ("clear columns to defaults");
   DEBUG_LOCS   yLOG_svalue  ("MAX_COLS"  , MAX_COLS);
   for (x_col = 0; x_col < MAX_COLS; ++x_col) {
      /*---(characteristics)-------------*/
      tabs [a_tab].cols [x_col].w       = DEF_WIDTH;
      tabs [a_tab].cols [x_col].x       = 0;
      tabs [a_tab].cols [x_col].c       = 0;
      /*---(labels)----------------------*/
      if        (x_col < 26)  {
         tabs [a_tab].cols [x_col].l[0] = '-';
         tabs [a_tab].cols [x_col].l[1] = x_col + 'a';
      } else  {
         tabs [a_tab].cols [x_col].l[0] = (x_col / 26) - 1 + 'a';
         tabs [a_tab].cols [x_col].l[1] = (x_col % 26) + 'a';
      }
      tabs [a_tab].cols [x_col].l[2] = '\0';
      /*---(done)------------------------*/
   }
   /*---(clear frozen cols)--------------*/
   DEBUG_LOCS   yLOG_snote   ("clear any frozen columns");
   tabs [a_tab].froz_col  = '-';
   tabs [a_tab].froz_bcol = 0;
   tabs [a_tab].froz_ecol = 0;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> return max col for tab ----------------[ petal  [ 1----- ]-*/
LOC_col_get_max      (short a_tab)
{
   char        rce         =  -10;
   --rce;  if (a_tab   < 0)                      return rce;
   --rce;  if (a_tab   >= NTAB)                  return rce;
   return tabs [a_tab].ncol;
}

char         /*--> change max col for tab ----------------[ stigma [ 2----- ]-*/
LOC_col_chg_max      (short a_tab, short a_size)
{
   char        rce         =  -10;
   --rce;  if (a_tab   < 0)                      return rce;
   --rce;  if (a_tab   >= NTAB)                  return rce;
   --rce;  if (a_size  <  MIN_COLS)              return rce;
   --rce;  if (a_size  >  MAX_COLS)              return rce;
   tabs [a_tab].ncol = a_size;
   if (tabs [a_tab].bcol >= tabs [a_tab].ncol)   tabs [a_tab].bcol = tabs [a_tab].ncol - 1;
   if (tabs [a_tab].ecol >= tabs [a_tab].ncol)   tabs [a_tab].ecol = tabs [a_tab].ncol - 1;
   if (tabs [a_tab].ccol >= tabs [a_tab].ncol)   tabs [a_tab].ccol = tabs [a_tab].ncol - 1;
   return 0;
}

char         /*--> return the col width ------------------[ petal  [ 2----- ]-*/
LOC_col_get_width    (short a_tab, short a_col)
{
   char        rce         =  -10;
   --rce;  if (a_tab   < 0)                      return rce;
   --rce;  if (a_tab   >= NTAB)                  return rce;
   --rce;  if (a_col   < 0)                      return rce;
   --rce;  if (a_col   >= tabs [a_tab].ncol)     return rce;
   return tabs [a_tab].cols [a_col].w;
}

char         /*--> change the col width ------------------[ stigma [ 3----- ]-*/
LOC_col_chg_width    (short a_tab, short a_col, short a_size)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (a_tab   < 0)                      return rce;
   --rce;  if (a_tab   >= NTAB)                  return rce;
   --rce;  if (a_col   < 0)                      return rce;
   --rce;  if (a_col   >= tabs [a_tab].ncol)     return rce;
   /*---(limits)-------------------------*/
   if (a_size  < MIN_WIDTH)    a_size = MIN_WIDTH;
   if (a_size  > MAX_WIDTH)    a_size = MAX_WIDTH;
   /*---(set)----------------------------*/
   tabs [a_tab].cols [a_col].w = a_size;
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                           row related                        ----===*/
/*====================------------------------------------====================*/
PRIV void  o___ROWS____________o () { return; }

char         /*--> clear all row customizations ----------[ septal [ 1----- ]-*/
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
   --rce;  if (a_tab >= NTAB) {
      DEBUG_LOCS   yLOG_snote   ("tab too big");
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(initialize rows)----------------*/
   DEBUG_LOCS   yLOG_snote   ("clear rows to defaults");
   DEBUG_LOCS   yLOG_svalue  ("MAX_ROWS"  , MAX_ROWS);
   for (x_row = 0; x_row < MAX_ROWS; ++x_row) {
      tabs [a_tab].rows [x_row].h = DEF_HEIGHT;
      tabs [a_tab].rows [x_row].y = 0;
      tabs [a_tab].rows [x_row].c = 0;
   }
   /*---(clear frozen rows)--------------*/
   DEBUG_LOCS   yLOG_snote   ("clear any frozen rows");
   tabs [a_tab].froz_row  = '-';
   tabs [a_tab].froz_brow = 0;
   tabs [a_tab].froz_erow = 0;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> return max row for tab ----------------[ petal  [ 1----- ]-*/
LOC_row_get_max      (short a_tab)
{
   char        rce         =  -10;
   --rce;  if (a_tab   < 0)                      return 0;
   --rce;  if (a_tab   >= NTAB)                  return 0;
   return tabs [a_tab].nrow;
}

char         /*--> change max row for tab ----------------[ stigma [ 2----- ]-*/
LOC_row_chg_max      (short a_tab, short a_size)
{
   char        rce         =  -10;
   --rce;  if (a_tab   < 0)                      return rce;
   --rce;  if (a_tab   >= NTAB)                  return rce;
   --rce;  if (a_size  <  MIN_ROWS)              return rce;
   --rce;  if (a_size  >  MAX_ROWS)              return rce;
   tabs [a_tab].nrow = a_size;
   if (tabs [a_tab].brow >= tabs [a_tab].nrow)   tabs [a_tab].brow = tabs [a_tab].nrow - 1;
   if (tabs [a_tab].erow >= tabs [a_tab].nrow)   tabs [a_tab].erow = tabs [a_tab].nrow - 1;
   if (tabs [a_tab].crow >= tabs [a_tab].nrow)   tabs [a_tab].crow = tabs [a_tab].nrow - 1;
   return 0;
}

char         /*--> return height for a row ---------------[ petal  [ 2----- ]-*/
LOC_row_get_height   (short a_tab, short a_row)
{
   char        rce         =  -10;
   --rce;  if (a_tab   < 0)                      return rce;
   --rce;  if (a_tab   >= NTAB)                  return rce;
   --rce;  if (a_row   < 0)                      return rce;
   --rce;  if (a_row   >= tabs [a_tab].nrow)     return rce;
   return tabs [a_tab].rows [a_row].h;
}

char         /*--> change height for a row ---------------[ stigma [ 3----- ]-*/
LOC_row_chg_height   (short a_tab, short a_row, short a_size)
{
   char        rce         =  -10;
   --rce;  if (a_tab   < 0)                      return rce;
   --rce;  if (a_tab   >= NTAB)                  return rce;
   --rce;  if (a_row   < 0)                      return rce;
   --rce;  if (a_row   >= tabs [a_tab].nrow)     return rce;
   --rce;  if (a_size  < MIN_HEIGHT)             return rce;
   --rce;  if (a_size  > MAX_HEIGHT)             return rce;
   tabs [a_tab].rows [a_row].h = a_size;
   return 0;
}

char         /*--> change the frozen rows ----------------[ stigma [ 3----- ]-*/
LOC_row_freeze       (short a_tab, short a_brow, short a_erow)
{
   char        rce         =  -10;
   --rce;  if (a_tab   <  0)                     return rce;
   --rce;  if (a_tab   >= NTAB)                  return rce;
   --rce;  if (a_brow  <  0)                     return rce;
   --rce;  if (a_brow  >= tabs [a_tab].nrow)     return rce;
   --rce;  if (a_erow  <  0)                     return rce;
   --rce;  if (a_erow  >= tabs [a_tab].nrow)     return rce;
   --rce;  if (a_brow  >  a_erow)                return rce;
   tabs [a_tab].froz_row    = 'y';
   tabs [a_tab].froz_brow   = a_brow;
   tabs [a_tab].froz_erow   = a_erow;
   return 0;
}

char         /*--> clear the frozen rows -----------------[ stigma [ 1----- ]-*/
LOC_row_unfreeze     (short a_tab)
{
   char        rce         =  -10;
   --rce;  if (a_tab   < 0)                      return rce;
   --rce;  if (a_tab   >= NTAB)                  return rce;
   tabs [a_tab].froz_row    = '-';
   tabs [a_tab].froz_brow   = 0;
   tabs [a_tab].froz_erow   = 0;
   return 0;
}

char         /*--> return the frozen rows ----------------[ petal  [ 3----- ]-*/
LOC_row_get_freeze   (short a_tab, short *a_brow, short *a_erow)
{
   char        rce         =  -10;
   --rce;  if (a_tab   < 0)                      return rce;
   --rce;  if (a_tab   >= NTAB)                  return rce;
   --rce;  if (tabs [a_tab].froz_row != 'y')     return rce;
   if (a_brow != NULL)  *a_brow = tabs [a_tab].froz_brow;
   if (a_erow != NULL)  *a_erow = tabs [a_tab].froz_erow;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNIT_TEST_______o () { return; }

char*        /*> unit test accessor ----------------------[ ------ [ ------ ]-*/
TAB__unit          (char *a_question, int a_num)
{
   /*---(locals)-------------------------*/
   int         i           =  0;
   char        x_beg       [LEN_LABEL]   = "";
   char        x_end       [LEN_LABEL]   = "";
   char        x_cur       [LEN_LABEL]   = "";
   char        x_max       [LEN_LABEL]   = "";
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "LOC              : question not understood");
   LOC_ref    (a_num, tabs [a_num].bcol, tabs [a_num].brow, 0, x_beg);
   LOC_ref    (a_num, tabs [a_num].ecol, tabs [a_num].erow, 0, x_end);
   LOC_ref    (a_num, tabs [a_num].ccol, tabs [a_num].crow, 0, x_cur);
   LOC_ref    (a_num, tabs [a_num].ncol - 1, tabs [a_num].nrow - 1, 0, x_max);
   /*---(overall)------------------------*/
   if      (strcmp(a_question, "tab_info"      ) == 0) {
      snprintf(unit_answer, LEN_UNIT, "LOC tab info (%1d) : %c %-10.10s %-7.7s %-7.7s %-7.7s %-7.7s %d", a_num, tabs [a_num].active, tabs [a_num].name, x_beg, x_end, x_cur, x_max, tabs [a_num].c);
   }
   /*> else if (strcmp(a_question, "tab_beg"       ) == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_UNIT, "s_move tab beg   : tab=%4d, col=%4d, row=%4d", a_num, tabs [a_num].bcol, tabs [a_num].brow);   <* 
    *> }                                                                                                                                  <* 
    *> else if (strcmp(a_question, "tab_pos"       ) == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_UNIT, "s_move tab pos   : tab=%4d, col=%4d, row=%4d", a_num, tabs [a_num].ccol, tabs [a_num].crow);   <* 
    *> }                                                                                                                                  <* 
    *> else if (strcmp(a_question, "tab_max" )       == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_UNIT, "s_move tab max   : tab=%4d, col=%4d, row=%4d", a_num, tabs [a_num].ncol, tabs [a_num].nrow);   <* 
    *> }                                                                                                                                  <*/
   /*---(complete)-----------------------*/
   return unit_answer;
}

char*        /*> unit test accessor ----------------------[ ------ [ ------ ]-*/
LOC__unit          (char *a_question, tCELL *a_cell)
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
   /*---(current position)---------------*/
   else if (strcmp(a_question, "tab_statuses"  ) == 0) {
      for (i = 0; i < MAX_TABS; ++i) {
         switch (tabs [i].active) {
         case 'y':  strlcat (t, " y", LEN_STR);   break;
         case '_':  strlcat (t, " _", LEN_STR);   break;
         case '/':  strlcat (t, " /", LEN_STR);   break;
         default :  strlcat (t, " ?", LEN_STR);   break;
         }
      }
      snprintf (unit_answer, LEN_UNIT, "LOC tab statuses :%s", t);
   }
   /*> else if (strcmp(a_question, "tab_beg"       ) == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_UNIT, "s_move tab beg   : tab=%4d, col=%4d, row=%4d", a_num, tabs [a_num].bcol, tabs [a_num].brow);   <* 
    *> }                                                                                                                                  <* 
    *> else if (strcmp(a_question, "tab_pos"       ) == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_UNIT, "s_move tab pos   : tab=%4d, col=%4d, row=%4d", a_num, tabs [a_num].ccol, tabs [a_num].crow);   <* 
    *> }                                                                                                                                  <* 
    *> else if (strcmp(a_question, "tab_max" )       == 0) {                                                                              <* 
    *>    snprintf(unit_answer, LEN_UNIT, "s_move tab max   : tab=%4d, col=%4d, row=%4d", a_num, tabs [a_num].ncol, tabs [a_num].nrow);   <* 
    *> }                                                                                                                                  <*/
   /*---(complete)-----------------------*/
   return unit_answer;
}
