/*============================----beg-of-source---============================*/

#include   "gyges.h"


char        s_label     [MAX_STR]   = "";


/*====================------------------------------------====================*/
/*===----                     hooking and unhooking                    ----===*/
/*====================------------------------------------====================*/
PRIV void  o___HOOKING_________o () { return; }

char         /*--> attach a cell to a location -----------[ ------ [ ------ ]-*/
LOC_hook           (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* cell to be placed in a location              */
      int         a_tab,      /* tab for target location                      */
      int         a_col,      /* col for target location                      */
      int         a_row)      /* row for target location                      */
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
      int         a_tab,      /* tab number                                   */
      int         a_col,      /* column number                                */
      int         a_row,      /* row number                                   */
      char        a_adapt)    /* y/n should enlarge boundaries to fit         */
{  /*---(design notes)--------------------------------------------------------*/
   /* tests the tab, col, and row against minimum and maximum limits as well  */
   /* as against current limits as a service to other functions.              */
   /* additionally, if requested, the function can expand the current limits  */
   /* to include the requested row and column if it is within the maximum     */
   /* limits.                                                                 */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   int         new_col     = 0;             /* holder for enlarged area       */
   int         new_row     = 0;             /* holder for enlarged area       */
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

tCELL*       /*--> return the cell at a location ---------[ ------ [ ------ ]-*/
LOC_cell           (
      /*----------+-----------+-----------------------------------------------*/
      int         a_tab,      /* tab number                                   */
      int         a_col,      /* column number                                */
      int         a_row)      /* row number                                   */
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
LOC_at             (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_label)    /* tab number                                   */
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   int         x_tab       =   0;           /* working tab value              */
   int         x_col       =   0;           /* working col value              */
   int         x_row       =   0;           /* working row value              */
   tCELL      *x_curr      = NULL;
   /*---(defenses)-----------------------*/
   rc = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);
   if (rc < 0)  return NULL;
   x_curr = LOC_cell  (x_tab, x_col, x_row);
   if (x_curr == NULL)    return NULL;
   /*---(complete)-----------------------*/
   return x_curr;
}

char         /*--> go directly to a specific cell---------[ leaf   [ 123y4x ]-*/
LOC_jump           (
      /*----------+-----------+-----------------------------------------------*/
      int         a_tab,      /* tab number                                   */
      int         a_col,      /* column number                                */
      int         a_row)      /* row number                                   */
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   /*---(defenses)-----------------------*/
   rc = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);
   if (rc < 0)    return rc;
   /*---(update globals)-----------------*/
   CTAB          = a_tab;
   tab           = &tabs[CTAB];
   CCOL          = a_col;
   CROW          = a_row;
   /*---(selection)----------------------*/
   SEL_update (a_tab, a_col, a_row);
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
      int         a_tab,      /* tab of location                              */
      int         a_col,      /* col of location                              */
      int         a_row,      /* row of location                              */
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
LOC_coordinates   (
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
   rc = LOC_parse (a_cell->label, a_tab, a_col, a_row, NULL);
   --rce;  if (rc <  0)                     return rce;
   /*---(complete)-----------------------*/
   return  0;
}

char         /*--> convert label into tab, col, row ------[ ------ [ ------ ]-*/
LOC_parse         (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_label,    /* cell label used for parsing                  */
      int        *a_tab,      /* return variable for the tab                  */
      int        *a_col,      /* return variable for the column               */
      int        *a_row,      /* return variable for the row                  */
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
   DEBUG_LOC   yLOG_enter   (__FUNCTION__);
   DEBUG_LOC   yLOG_info    ("a_label"   , a_label);
   DEBUG_LOC   yLOG_point   ("a_tab"     , a_tab);
   DEBUG_LOC   yLOG_point   ("a_col"     , a_col);
   DEBUG_LOC   yLOG_point   ("a_row"     , a_row);
   DEBUG_LOC   yLOG_point   ("a_abs"     , a_abs);
   /*---(defense: empty label)-----------*/
   --rce;  if (a_label == NULL) {
      DEBUG_LOC   yLOG_note    ("aborting, no a_label means no point");
      DEBUG_LOC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check sizes)--------------------*/
   len = strlen (a_label);
   DEBUG_LOC   yLOG_value   ("len"       , len);
   --rce;  if (len <=  0) {
      DEBUG_LOC   yLOG_note    ("aborting, a_label contains nothing, no point");
      DEBUG_LOC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (len >  12) {  /* a1 to $24$ab$12345 */
      DEBUG_LOC   yLOG_note    ("aborting, a_label too long for legal address");
      DEBUG_LOC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: check special)---------*/
   --rce;  if (strcmp (a_label, "root") == 0) {
      DEBUG_LOC   yLOG_note    ("aborting, no need to parse root");
      DEBUG_LOC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (strcmp (a_label, ".") == 0) {
      DEBUG_LOC   yLOG_note    ("aborting, no need to parse dot (.)");
      DEBUG_LOC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (strcmp (a_label, "-") == 0) {
      DEBUG_LOC   yLOG_note    ("aborting, no need to parse dash (-)");
      DEBUG_LOC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(look for absolute tab)----------*/
   DEBUG_LOC   yLOG_char    ("CH"        , a_label[s_tab]);
   if (a_label[s_tab] == '@') {
      x_abs  = 7;
      ++s_tab;
      DEBUG_LOC   yLOG_char    ("CH"        , a_label[s_tab]);
   }
   if (a_label[s_tab] == '$') {
      x_abs += 4;
      ++s_tab;
   }
   DEBUG_LOC   yLOG_value   ("x_abs"     , x_abs);
   DEBUG_LOC   yLOG_value   ("s_tab"     , s_tab);
   /*---(parse tab characters)-----------*/
   s_col = s_tab;
   for (i = s_tab; i < s_tab + 2; ++i) {
      DEBUG_LOC   yLOG_char    ("CH"        , a_label[i]);
      if (strchr ("0123456789", a_label[i]) == 0)   break;
      if (i >  s_tab)  x_tab *= 10;
      x_tab += a_label[i] - '0';
      ++s_col;
   }
   if (s_col == s_tab && x_abs == 4)  x_abs = 1;
   DEBUG_LOC   yLOG_value   ("x_tab"     , x_tab);
   DEBUG_LOC   yLOG_value   ("s_col"     , s_col);
   if (a_tab != NULL)  *a_tab = x_tab;
   /*---(look for absolute col)----------*/
   DEBUG_LOC   yLOG_char    ("CH"        , a_label[s_col]);
   if (a_label [s_col] == '$') {
      x_abs += 1;
      ++s_col;
   }
   DEBUG_LOC   yLOG_value   ("x_abs"     , x_abs);
   DEBUG_LOC   yLOG_value   ("s_col"     , s_col);
   /*---(parse col characters)-----------*/
   s_row = s_col;
   for (i = s_col; i < s_col + 2; ++i) {
      DEBUG_LOC   yLOG_char    ("CH"        , a_label[i]);
      if (strchr ("abcdefghijklmnopqrstuvwxyz", a_label[i]) == 0)   break;
      if (i >  s_col)  x_col *= 26;
      x_col += a_label[i] - 'a' + 1;
      ++s_row;
   }
   DEBUG_LOC   yLOG_value   ("x_col"     , x_col);
   DEBUG_LOC   yLOG_value   ("s_row"     , s_row);
   --rce;  if (s_row == s_col) {
      DEBUG_LOC   yLOG_note    ("aborting, no column found");
      DEBUG_LOC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --x_col;
   DEBUG_LOC   yLOG_value   ("x_col fix" , x_col);
   if (a_col != NULL)  *a_col = x_col;
   /*---(look for absolute row)----------*/
   DEBUG_LOC   yLOG_char    ("CH"        , a_label[s_row]);
   if (a_label [s_row] == '$') {
      x_abs += 2;
      ++s_row;
   }
   DEBUG_LOC   yLOG_value   ("x_abs"     , x_abs);
   DEBUG_LOC   yLOG_value   ("s_row"     , s_row);
   if (x_abs > 7)  x_abs = 7;
   if (a_abs != NULL)  *a_abs = x_abs;
   /*---(parse row characters)-----------*/
   e_row = s_row;
   for (i = s_row; i < len; ++i) {
      DEBUG_LOC   yLOG_char    ("CH"        , a_label[i]);
      if (strchr ("0123456789", a_label[i]) == 0)   break;
      if (i >  s_row)  x_row *= 10;
      x_row += a_label[i] - '0';
      ++e_row;
   }
   DEBUG_LOC   yLOG_value   ("x_row"     , x_row);
   DEBUG_LOC   yLOG_value   ("e_row"     , e_row);
   --rce;  if (e_row != len  ) {
      DEBUG_LOC   yLOG_note    ("aborting, no row found");
      DEBUG_LOC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --x_row;
   if (a_row != NULL)  *a_row = x_row;
   /*---(complete)-----------------------*/
   DEBUG_LOC   yLOG_exit    (__FUNCTION__);
   return  0;
}

int        /*----: turn a cell name into a tab index -------------------------*/
LOC_tab            (char *a_name)
{
   int       i         = 0;
   int       rc        = 0;
   for (i = 0; i < NTAB; ++i) {
      if ((rc = strcmp(tabs[i].name, a_name)) == 0) {
         return i;
      }
      /*> printf ("LOC_tab           : trying %d which is <<%s>>==<<%s>> rc = %d\n", i, a_name, tabs[i].name, rc);   <*/
   }
   return 0;
}




/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNIT_TEST_______o () { return; }

char*        /*> unit test accessor ----------------------[ ------ [ ------ ]-*/
LOC_unit           (char *a_question, tCELL *a_cell)
{
   /*---(locals)-----------+-----------+-*/
   int         rc          = -1;
   int         i           =  0;
   tCELL      *x_curr      = NULL;
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_loc            : question not understood");
   /*---(find cell by pointer)-----------*/
   x_curr  = hcell;
   for (i = 0; i < NCEL; ++i) {
      if (x_curr == NULL)     break;
      if (x_curr == a_cell) { rc = 0; break; }
      x_curr = x_curr->next;
   }
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "loc_cell"     )  == 0) {
      if (rc == 0) {
         snprintf (unit_answer, LEN_TEXT, "s_loc cell       : ptr=%9p, tab=%4d, col=%4d, row=%4d", x_curr, x_curr->tab, x_curr->col, x_curr->row);
      } else {
         snprintf (unit_answer, LEN_TEXT, "s_loc cell       : cell not found");
      }
   }
   else if (strcmp(a_question, "loc_who"       )  == 0) {
      if (a_cell != NULL) {
         snprintf (unit_answer, LEN_TEXT, "s_loc occupant   : ptr=%9p, tab=%4d, col=%4d, row=%4d", x_curr, x_curr->tab, x_curr->col, x_curr->row);
      } else {
         snprintf (unit_answer, LEN_TEXT, "s_loc occupant   : ptr=%9p, no cell attached", a_cell);
      }
   }
   else if (strcmp (a_question, "cell_where")     == 0) {
      if (rc == 0) {
         snprintf (unit_answer, LEN_TEXT, "s_cell location  : tab=%4d, col=%4d, row=%4d", x_curr->tab, x_curr->col, x_curr->row);
      } else {
         snprintf (unit_answer, LEN_TEXT, "s_cell location  : not found in cell list");
      }
   }
   else if (strcmp(a_question, "cell_list")      == 0) {
      snprintf(unit_answer, LEN_TEXT, "s_cell main list : num=%4d, head=%9p, tail=%9p", NCEL, hcell, tcell);
   }
   else if (strcmp(a_question, "loc_label")      == 0) {
      snprintf(unit_answer, LEN_TEXT, "s_loc label      : %s", s_label);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}
