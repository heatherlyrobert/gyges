/*============================----beg-of-source---============================*/


/*===[[ MODULE ]]=============================================================*

 *   application   : s
 *   module        : s_cell
 *   purpose       : provide all code for accessing individual cells or sheets
 *
 */
/*===[[ EXPLANATION ]]========================================================*

 *
 *
 *
 *
 */

#include   "gyges.h"

/*
 *   legend for coded fields...
 *
 *   t = cell content type.  this field drives how much of the program views
 *   the cell, including display and calculation.  it is a single character so
 *   i can easily debug and test with simple prints rather than decoding a enum.
 *   it can be one of the following values...
 *
 *      basics
 *         -    empty placeholder
 *         s    source string
 *         n    numeric value
 *
 *      calculated
 *         m    modified string
 *         f    numeric formula formula
 *         l    like formula
 *         p    range pointer
 *         a    address pointer
 *         d    display of formula to left
 *
 *      troubles
 *         w    warning
 *         e    error
 *         E    MAJOR error (display error message instead)
 *
 *
 *   a = alignment code.  this field drives how the cell value is displayed and
 *   is therefore of primary use to formatting and display code.  it is also a
 *   single character for debugging and testing.  it can have the following
 *   values...
 *      <    individual cell left aligned
 *      |    individual cell center aligned
 *      >    individual cell right aligned
 *      [    merged cells left aligned
 *      ^    merged cells center aligned
 *      ]    merged cells right aligned
 *      {    TBD
 *      }    TBD
 *
 *   f = formatting style.  this field drives how numberic fields are displayed
 *   and also how filler may be used with strings.  there is a great deal of
 *   variety which can be provided here which is useful when working without
 *   colors, patterns, fonts, or font styles -- old school.  it can have the
 *   following values...
 *
 *      general formats (4)...
 *      i    integer
 *      r    floating point
 *      g    floating with six decimal points
 *      e    exponential notation
 *
 *      comma formats (6)...
 *      ,    integer with thousand markers
 *      a    accounting style with thousands and negative parenthesis
 *      $    currency with dollars and thousand markers
 *      s    signed notation which always shows sign and thousand markers
 *      #    technical comma format that delimits decimals as well
 *      %    percentage with thousands markers
 *
 *      specialty base formats (6)...
 *      x    hexadecimal notation
 *      X    hexadecimal notation with spacing for clarity
 *      b    binary notation
 *      B    binary notation with spacing for clarity
 *      o    octal notation
 *      O    octal notation with spacing for clarity
 *
 *      time formats (3)...
 *      t    time (HH:MM) in 24 hour time
 *      d    date (YYYY-mmm-dd)
*      T    timestamp (YY.MM.DD.hh.mm.ss)
*
*      other formats (1)...
*      p    bullet point format
*
*      filler formats (6)...
*      sp   filled with spaces
*      -    filled with dashes
*      =    filled with double dashes
*      _    filled with underscores
*      .    filled with dots
*      +    filled with pluses
*
*
*   data structure variables...
*
*   in order to enable the doublly linked list of cells as well as a little
*   other safety, i have included the following globals.  i follow an odd
*   convension, but it serves me well so far.  i use prefixes to indicate
*   the supporting variables for a data structure.
*      h....      head pointer
*      t....      tail pointer
*      n....      count of elements
*   there are more, but that might just prove how crazy i am.  LOL.
*      
*/

char        g_empty       [200] = "                                                                                                                                                                                                       ";
char        g_dashes      [200] = "----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----·----";




/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___PROGRAM_________o () { return; }

char         /*-> tbd --------------------------------[ ------ [fz.842.041.24]*/ /*-[01.0000.013.T]-*/ /*-[--.---.---.--]-*/
CELL_purge         (void)
{
   /*---(locals)-----------+-----------+-*/
   tCELL      *curr        = NULL;
   tCELL      *next        = NULL;
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(walk through list)--------------*/
   next = hcell;
   DEBUG_CELL   yLOG_point   ("hcell"     , hcell);
   while (next != NULL) {
      curr = next;
      next = curr->next;
      rc = CELL__wipe    (curr);
      DEBUG_CELL   yLOG_value   ("wipe rc"   , rc);
      rc = LOC_unhook   (curr);
      DEBUG_CELL   yLOG_value   ("unhook rc" , rc);
      rc = CELL__free    (&curr, LINKED);
      DEBUG_CELL   yLOG_value   ("free rc"   , rc);
      DEBUG_CELL   yLOG_point   ("next"      , next);
   }
   /*---(clean ends)---------------------*/
   if (ncell == 0) {
      hcell = NULL;
      tcell = NULL;
   }
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ shoot  [ge.G94.071.80]*/ /*-[04.0000.102.!]-*/ /*-[--.---.---.--]-*/
CELL_init          (void)
{
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =    0;
   /*---(cells)--------------------------*/
   ACEL        = 0;
   my.root     = NULL;
   hcell       = NULL;
   tcell       = NULL;
   NCEL        = 0;
   /*---(handlers)-----------------------*/
   rc = yPARSE_handler (FILE_DEPCEL  , "cell_dep"  , 5.1, "LTO---------", CELL_reader     , CELL_writer_all , "------------" , "label,afdu?-----,contents-----------------" , "gyges dependent cells"  );
   rc = yPARSE_handler (FILE_FREECEL , "cell"      , 5.2, "LTO---------", CELL_reader     , NULL            , "------------" , "label,afdu?-----,contents-----------------" , "gyges free cells"       );
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ shoot  [gz.321.001.01]*/ /*-[00.0000.102.!]-*/ /*-[--.---.---.--]-*/
CELL_wrap          (void)
{
   /*---(header)-------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(cells)--------------------------*/
   CELL_purge  ();
   hcell  = NULL;
   tcell  = NULL;
   NCEL   = 0;
   /*---(root)---------------------------*/
   CELL__free    (&my.root, UNLINKED);
   my.root = NULL;
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        basic utilities                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___BASICS__________o () { return; }

char         /*-> create a single new empty cell -----[ leaf   [fe.KB4.224.80]*/ /*-[12.0000.123.A]-*/ /*-[--.---.---.--]-*/
CELL__new          (tCELL **a_cell, char a_linked)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         x_tries     =    0;
   /*---(begin)--------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CELL   yLOG_point   ("a_cell"    , a_cell);
   --rce;  if (a_cell == NULL) {
      DEBUG_CELL   yLOG_note    ("pointer can not be null (no point)");
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CELL   yLOG_point   ("*a_cell"   , *a_cell);
   --rce;  if (*a_cell != NULL) {
      DEBUG_CELL   yLOG_note    ("cell pointer already contains a value");
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(create cell)--------------------*/
   while (*a_cell == NULL) {
      ++x_tries;
      *a_cell = (tCELL *) malloc (sizeof (tCELL));
      if (x_tries > 10)   break;
   }
   DEBUG_CELL   yLOG_value   ("x_tries"   , x_tries);
   DEBUG_CELL   yLOG_point   ("*a_cell"   , *a_cell);
   --rce;  if (*a_cell == NULL) {
      DEBUG_CELL   yLOG_note    ("cell could not be created");
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ++ACEL;
   DEBUG_CELL   yLOG_value   ("ACEL"      , ACEL);
   /*---(into linked list)---------------*/
   DEBUG_CELL   yLOG_value   ("a_linked"  , a_linked);
   (*a_cell)->linked  = a_linked;
   (*a_cell)->next    = NULL;
   (*a_cell)->prev    = NULL;
   if (a_linked != UNLINKED) {
      (*a_cell)->linked  = LINKED;
      if (tcell == NULL) {
         hcell           = *a_cell;
         tcell           = *a_cell;
      } else {
         (*a_cell)->prev = tcell;
         (*a_cell)->next = NULL;
         tcell->next     = *a_cell;
         tcell           = *a_cell;
      }
      ++NCEL;
   }
   DEBUG_CELL   yLOG_value   ("NCEL"      , NCEL);
   /*---(location)-----------------------*/
   DEBUG_CELL   yLOG_note    ("assign default values/init");
   (*a_cell)->tab       = UNHOOKED;
   (*a_cell)->col       = UNHOOKED;
   (*a_cell)->row       = UNHOOKED;
   strcpy ((*a_cell)->label, "tbd");
   /*---(source)------------------------*/
   (*a_cell)->source    = NULL;
   (*a_cell)->len       = 0;
   /*---(results)-----------------------*/
   (*a_cell)->type      = YCALC_DATA_BLANK;
   (*a_cell)->v_num     =  0.0;
   (*a_cell)->v_str     = NULL;
   /*---(formatting)--------------------*/
   (*a_cell)->align     =  '?';
   (*a_cell)->format    =  '?';
   (*a_cell)->decs      =  '0';
   (*a_cell)->unit      =  '-';
   (*a_cell)->note      =  '-';
   (*a_cell)->print     = NULL;
   /*---(calculations)-------------------*/
   (*a_cell)->ycalc     = NULL;
   DEBUG_CELL   yLOG_note    ("successful");
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> wipe the contents of a cell --------[ ------ [fc.B52.112.94]*/ /*-[40.0000.144.M]-*/ /*-[--.---.---.--]-*/
CELL__wipe         (tCELL *a_curr)
{
   /*---(design notes)----------------*/
   /*
    *  this function should wipe the contents of a cell without
    *  modifying its location on the sheet or its connection in
    *  the cell dll (doubly-linked-list).
    *
    */
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   /*---(beginning)----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_CELL   yLOG_point   ("a_curr"    , a_curr);
   if (a_curr            == NULL) {
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return 0;   /* no harm, no foul, nothing to do    */
   }
   /*---(source)-------------------------*/
   DEBUG_CELL   yLOG_note    ("clear source string");
   if (a_curr->source    != NULL)  free (a_curr->source);
   a_curr->source              = NULL;
   a_curr->len            = 0;
   /*---(results)------------------------*/
   DEBUG_CELL   yLOG_note    ("clear results, string and value");
   a_curr->type           = YCALC_DATA_BLANK;
   if (a_curr->v_str     != NULL)  free (a_curr->v_str);
   a_curr->v_str          = NULL;
   a_curr->v_num          = 0.0;
   /*---(printable)----------------------*/
   DEBUG_CELL   yLOG_note    ("wipe printable");
   if (a_curr->print     != NULL)  free (a_curr->print);
   a_curr->print          = NULL;
   /*---(calculations)-------------------*/
   DEBUG_CELL   yLOG_note    ("clear rpn formulas and calculations");
   /*> if (a_curr->rpn       != NULL)  free (a_curr->rpn);                            <* 
    *> a_curr->rpn            = NULL;                                                 <* 
    *> a_curr->nrpn           = 0;                                                    <* 
    *> if (a_curr->calc      != NULL)  CALC_free (a_curr);                            <* 
    *> a_curr->calc           = NULL;                                                 <*/
   /*---(dependencies)-------------------*/
   DEBUG_CELL   yLOG_note    ("destroy all requires dependencies");
   /*> if (a_curr->requires  != NULL)  DEP_cleanse (a_curr);                          <* 
    *> a_curr->requires       = NULL;                                                 <* 
    *> a_curr->nrequire       = 0;                                                    <*/
   DEBUG_CELL   yLOG_note    ("unroot, but leave any other provides dependences");
   /*> if (a_curr->provides  != NULL)  rc = DEP_rooting (a_curr, G_DEP_UNROOT);       <*/
   /*---(errors)-------------------------*/
   DEBUG_CELL   yLOG_note    ("clear errors");
   /*> if (a_curr->errors    != NULL)  ERROR_cleanse (a_curr);                        <* 
    *> a_curr->errors         = NULL;                                                 <* 
    *> a_curr->nerror         = 0;                                                    <*/
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> test a cell pointer for safety -----[ leaf   [fe.C34.237.50]*/ /*-[02.0000.014.#]-*/ /*-[--.---.---.--]-*/
CELL__valid        (tCELL *a_cell, char a_linked)
{
   /*---(design notes)-------------------*/
   /*
    * allows a common safety check before pointer dereferencing.  also allows
    * for a quick path if the cell was just found in the last call to decrease
    * processing and stop the need to walk the list again.  since many functions
    * will use this for safety, the quick path will be exercised a great deal.
    *
    * will return a zero (0) if found, one (1) if refound/quick path, a negative
    * one (-1) if not found, and other negatives in the case of an error.
    *
    */
   /*---(static locals)----+-----------+-*/
   static      tCELL      *x_save      = NULL;
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   int         i           =   0;           /* iterator -- cell               */
   /*---(header)-------------------------*/
   DEBUG_CELL   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CELL   yLOG_spoint  (a_cell);
   --rce;  if (a_cell == NULL) {
      DEBUG_CELL   yLOG_snote   ("null cell");
      DEBUG_CELL   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(quick path: already found)------*/
   DEBUG_CELL   yLOG_spoint  (x_save);
   if  (a_cell == x_save) {
      DEBUG_CELL   yLOG_snote   ("quick path");
      DEBUG_CELL   yLOG_sexit   (__FUNCTION__);
      return 1;
   }
   /*---(check for unlinked)-------------*/
   DEBUG_CELL   yLOG_schar   (a_linked);
   if (a_linked == UNLINKED) {
      DEBUG_CELL   yLOG_snote   ("can not verify unlinked");
      DEBUG_CELL   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   /*---(find cell by pointer)-----------*/
   DEBUG_CELL   yLOG_snote   ("search");
   x_save  = hcell;
   while (x_save != NULL) {
      if (a_cell == x_save) {
         DEBUG_CELL   yLOG_snote   ("found");
         DEBUG_CELL   yLOG_sexit   (__FUNCTION__);
         return 0;
      }
      x_save = x_save->next;
   }
   DEBUG_CELL   yLOG_snote   ("NOT found");
   --rce;
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_sexitr  (__FUNCTION__, rce);
   return rce;
}

char         /*-> remove a cell completely -----------[ ------ [fe.943.224.81]*/ /*-[11.0000.133.7]-*/ /*-[--.---.---.--]-*/
CELL__free         (tCELL **a_cell, char a_linked)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(beginning)----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(defense: valid cell)------------*/
   DEBUG_CELL   yLOG_point   ("a_cell"    , a_cell);
   --rce;  if (a_cell == NULL) {
      DEBUG_CELL   yLOG_note    ("no null pointers (no reason)");
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CELL   yLOG_point   ("*a_cell"   , *a_cell);
   --rce;  if (*a_cell == NULL) {
      DEBUG_CELL   yLOG_note    ("no null pointers (no reason)");
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = CELL__valid (*a_cell, a_linked);
   --rce;  if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(out of linked list)-------------*/
   if ((*a_cell)->linked == LINKED) {
      DEBUG_CELL   yLOG_note    ("linked cell, unlinking now");
      if ((*a_cell)->next != NULL)   (*a_cell)->next->prev = (*a_cell)->prev;
      else                           tcell                 = (*a_cell)->prev;
      if ((*a_cell)->prev != NULL)   (*a_cell)->prev->next = (*a_cell)->next;
      else                           hcell                 = (*a_cell)->next;
      --NCEL;
   }
   /*---(free main)----------------------*/
   DEBUG_CELL   yLOG_note    ("freeing and nulling");
   free (*a_cell);
   *a_cell = NULL;
   --ACEL;
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

/*> char         /+-> act on a dependent cell ------------[ ------ [fz.722.621.41]+/ /+-[01.0000.00#.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> CELL__depwipe      (FILE *a_file, char a_type, int *a_seq, int a_level, tCELL *a_curr, long a_stamp)                           <* 
 *> {                                                                                                                              <* 
 *>    /+---(design notes)-------------------+/                                                                                    <* 
 *>    /+                                                                                                                          <* 
 *>     *  to prepare cells for easy deletion, just wipe the content which will                                                    <* 
 *>     *  remove all downward dependencies.  once this is done to all cells, there                                                <* 
 *>     *  are no connections left to stop a sequencial purge.                                                                     <* 
 *>     +/                                                                                                                         <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        x_pre       [50]        = "-           ";                                                                       <* 
 *>    char        rc          = 0;                                                                                                <* 
 *>    /+---(beginning)----------------------+/                                                                                    <* 
 *>    DEBUG_CELL   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    DEBUG_CELL   yLOG_char    ("a_type"    ,  a_type);                                                                          <* 
 *>    DEBUG_CELL   yLOG_value   ("a_seq"     , *a_seq);                                                                           <* 
 *>    DEBUG_CELL   yLOG_point   ("a_curr"    ,  a_curr);                                                                          <* 
 *>    /+---(defense)------------------------+/                                                                                    <* 
 *>    if (a_curr    == NULL)        return;     /+ no cell                       +/                                               <* 
 *>    DEBUG_CELL   yLOG_info    ("label"     ,  a_curr->label);                                                                   <* 
 *>    DEBUG_CELL   yLOG_value   ("stamp"     ,  a_curr->u);                                                                       <* 
 *>    if (a_curr->u == a_stamp)     return;     /+ already traveled across       +/                                               <* 
 *>    /+---(update)-------------------------+/                                                                                    <* 
 *>    if (a_type == 'x')  {                                                                                                       <* 
 *>       rc = CELL__wipe    (a_curr);                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    /+---(update)-------------------------+/                                                                                    <* 
 *>    ++(*a_seq);                                                                                                                 <* 
 *>    if (a_type != 'x') a_curr->u = a_stamp;                                                                                     <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_CELL   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/




/*====================------------------------------------====================*/
/*===----                      creation/destruction                    ----===*/
/*====================------------------------------------====================*/
PRIV void  o___EXISTANCE_______o () { return; }

char         /*-> tbd --------------------------------[ ------ [fe.732.424.33]*/ /*-[01.0000.014.K]-*/ /*-[--.---.---.--]-*/
CELL__create       (tCELL **a_cell, int a_tab, int a_col, int a_row)
{
   /*---(locals)----------------------*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_label     [LEN_LABEL];
   /*---(defenses)-----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   rc = str4gyges (a_tab, a_col, a_row, 0, 0, x_label, YSTR_ADAPT);
   DEBUG_CELL   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(create cell)-----------------*/
   rc = CELL__new (a_cell, LINKED);
   DEBUG_CELL   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear it out)----------------*/
   rc = LOC_hook  (*a_cell, a_tab, a_col, a_row);
   DEBUG_CELL   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)--------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.I75.468.FA]*/ /*-[02.0000.524.C]-*/ /*-[--.---.---.--]-*/
CELL__delete            (char a_mode, int a_tab, int a_col, int a_row)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   tCELL      *x_curr      = NULL;
   char        x_label     [LEN_LABEL]  = "";
   char        x_format    [LEN_LABEL]  = "";
   char        x_before    [LEN_RECD]   = "";
   char        x_after     [LEN_RECD]   = "";
   /*---(defenses)-----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   DEBUG_CELL   yLOG_value   ("a_tab"     , a_tab);
   DEBUG_CELL   yLOG_value   ("a_col"     , a_col);
   DEBUG_CELL   yLOG_value   ("a_row"     , a_row);
   rc = str4gyges (a_tab, a_col, a_row, 0, 0, x_label, YSTR_LEGAL);
   DEBUG_CELL   yLOG_value   ("legal"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_curr      = LOC_cell_at_loc (a_tab, a_col, a_row);
   DEBUG_CELL   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save before)-----------------*/
   if (x_curr->source != NULL) {
      strcpy  (x_before, x_curr->source);
      sprintf (x_format, "%c%c%c", x_curr->format, x_curr->align, x_curr->decs);
   }
   /*---(history)------------------------*/
   HIST_delete (a_mode, a_tab, a_col, a_row, x_before, x_format);
   /*---(clear it out)----------------*/
   /*> DEBUG_CELL   yLOG_complex ("details"   , "ptr=%p, tab=%4d, col=%4d, row=%4d, t=%c, u=%d", x_curr, x_curr->tab, x_curr->col, x_curr->row, x_curr->type, x_curr->u);   <*/
   rc = CELL__wipe   (x_curr);
   DEBUG_CELL   yLOG_value   ("wipe rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(see if its still there)------*/
   x_curr        = LOC_cell_at_loc (a_tab, a_col, a_row);
   if (x_curr == NULL) {
      DEBUG_CELL   yLOG_note    ("cell already removed, moving on");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(remove)----------------------*/
   --rce;
   rc = LOC_unhook  (x_curr);
   DEBUG_CELL   yLOG_value   ("unhook rc" , rc);
   if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = CELL__free   (&x_curr, LINKED);
   DEBUG_CELL   yLOG_value   ("free rc"   , rc);
   if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce - 1);
      return rce - 1;
   }
   /*---(complete)--------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> create a duplicate cell ------------[ ------ [ge.B44.225.61]*/ /*-[01.0000.205.2]-*/ /*-[--.---.---.--]-*/
CELL_dup           (tCELL **a_new, tCELL *a_old)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(beginning)----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_CELL   yLOG_point   ("a_new"     , a_new);
   --rce;  if (a_new  == NULL) {
      DEBUG_CELL   yLOG_note    ("must be null contents to start");
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce; /* nothing to move */
   }
   DEBUG_CELL   yLOG_point   ("*a_new"    , *a_new);
   --rce;  if (*a_new != NULL) {
      DEBUG_CELL   yLOG_note    ("must be a valid pointer");
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce; /* nothing to move */
   }
   DEBUG_CELL   yLOG_point   ("a_old"     , a_old);
   --rce;  if (a_old  == NULL)  {
      DEBUG_CELL   yLOG_note    ("can not be null, no point");
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce; /* nothing to move */
   }
   /*---(clear it out)-------------------*/
   rc = CELL__new (a_new , LINKED);
   DEBUG_CELL   yLOG_point   ("*a_new"    , *a_new);
   --rce;  if (*a_new == NULL)  {
      DEBUG_CELL   yLOG_note    ("new cell could not be created");
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(copy source)--------------------*/
   DEBUG_CELL   yLOG_note    ("copy old source and length");
   if (a_old->source == NULL)    (*a_new)->source = NULL;
   else                     (*a_new)->source = strndup (a_old->source, LEN_RECD);
   (*a_new)->len      = a_old->len;
   /*---(copy formatting)----------------*/
   DEBUG_CELL   yLOG_note    ("copy old type and formatting");
   (*a_new)->type        = a_old->type;
   (*a_new)->align       = a_old->align;
   (*a_new)->format      = a_old->format;
   (*a_new)->decs        = a_old->decs;
   (*a_new)->unit        = a_old->unit;
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return  0;
}

char         /*-> change te contents of a cell -------[ leaf   [ge.M96.647.HB]*/ /*-[02.0000.953.#]-*/ /*-[--.---.---.--]-*/
CELL_change        (tCELL** a_cell, char a_mode, int a_tab, int a_col, int a_row, char *a_source)
{
   /*---(design notes)-------------------*/
   /*
    *   this function changes the source string of a cell and then
    *   updates all other content as required.  if the cell already exists
    *   it wipes it clean, if not, it creates a new one.
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_label     [LEN_LABEL];
   tCELL      *x_curr      = NULL;
   char       x_bsource    [200] = "";
   char       x_bformat    [200] = "´´´´´";
   /*---(beginning)----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   DEBUG_CELL   yLOG_complex ("location"  , "tab %4d, col %4d, row %4d", a_tab, a_col, a_row);
   DEBUG_CELL   yLOG_point   ("contents"  , a_source);
   --rce;  if (a_source == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CELL   yLOG_info    ("contents"  , a_source);
   /*---(legal location)-----------------*/
   rc = str4gyges (a_tab, a_col, a_row, 0, 0, x_label, YSTR_LEGAL);
   DEBUG_CELL   yLOG_info    ("legal"     , (rc >= 0) ? "yes" : "no" );
   --rce;  if (rc <  0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(cell present)-------------------*/
   x_curr      = LOC_cell_at_loc (a_tab, a_col, a_row);
   DEBUG_CELL   yLOG_point   ("x_curr"    , x_curr);
   /*---(save before picture)------------*/
   --rce;  if (x_curr != NULL) {
      DEBUG_CELL   yLOG_note    ("save existing data");
      if (x_curr->source != NULL)  strcpy (x_bsource, x_curr->source);
      sprintf (x_bformat, "%c%c%c%c-", x_curr->align, x_curr->format, x_curr->decs, x_curr->unit);
   } else if (x_curr == NULL) {
      rc = CELL__create (&x_curr, a_tab, a_col, a_row);
      DEBUG_CELL   yLOG_point   ("new cell"  , x_curr);
      if (x_curr == NULL) {
         DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   DEBUG_CELL   yLOG_info    ("cell label", x_curr->label);
   /*---(history)------------------------*/
   if (a_source == NULL)            HIST_clear   (a_mode, a_tab, a_col, a_row, x_bsource, x_bformat);
   else if (a_source [0] == '\0')   HIST_clear   (a_mode, a_tab, a_col, a_row, x_bsource, x_bformat);
   else                             HIST_change  (a_mode, a_tab, a_col, a_row, x_bsource, x_bformat, a_source);
   /*---(update)-------------------------*/
   DEBUG_CELL   yLOG_note    ("change source and length values");
   x_curr->source = strndup (a_source, LEN_RECD);
   x_curr->len = strlen  (x_curr->source);
   /*---(interpret)----------------------*/
   DEBUG_CELL   yLOG_note    ("interpret new contents");
   rc = yCALC_handle (x_curr->label);
   DEBUG_CELL   yLOG_value   ("handle"    , rc);
   /*---(return)-------------------------*/
   if (a_cell != NULL)  *a_cell = x_curr;
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

tCELL*       /*-> change the contents of a cell ------[ ------ [gp.B44.627.94]*/ /*-[01.0000.504.!]-*/ /*-[--.---.---.--]-*/
CELL_overwrite     (char a_mode, int a_tab, int a_col, int a_row, char *a_source, char *a_format)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   tCELL      *x_new       = NULL;
   char       x_aformat    [LEN_LABEL] = "??0--";
   /*---(defense)------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   if (strlen (a_format) < 5) {
      DEBUG_CELL   yLOG_warn    ("format not valid length");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   /*---(start)--------------------------*/
   CELL_change (&x_new, a_mode, a_tab, a_col, a_row, a_source);
   if (x_new == NULL)  {
      DEBUG_CELL   yLOG_warn    ("x_new change returned a null pointer");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   /*---(history)------------------------*/
   if (a_format [0] != '´')   strcpy (x_aformat, a_format);
   if (a_mode != HIST_NONE)  HIST_overwrite (a_mode, a_tab, a_col, a_row, x_aformat);
   /*---(formatting)---------------------*/
   DEBUG_CELL   yLOG_note    ("update format fields");
   x_new->align  = x_aformat [0];
   DEBUG_CELL   yLOG_char    ("align"     , x_new->align);
   x_new->format = x_aformat [1];
   DEBUG_CELL   yLOG_char    ("format"    , x_new->format);
   x_new->decs   = x_aformat [2];
   DEBUG_CELL   yLOG_char    ("decs"      , x_new->decs);
   x_new->unit   = x_aformat [3];
   DEBUG_CELL   yLOG_char    ("unit"      , x_new->unit);
   /*---(update)-------------------------*/
   DEBUG_CELL   yLOG_note    ("call printable");
   rc = api_ycalc_printer (x_new);
   if (rc < 0) {
      DEBUG_CELL   yLOG_warn    ("printable returned a bad return code");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   DEBUG_CELL   yLOG_note    ("call calc");
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return x_new;
}




/*====================------------------------------------====================*/
/*===----                         formatting                           ----===*/
/*====================------------------------------------====================*/
PRIV void  o___FORMATTING______o () { return; }

char         /*-> erase cells in current selection ---[ ------ [ge.751.093.33]*/ /*-[01.0000.106.!]-*/ /*-[--.---.---.--]-*/
CELL_erase         (tCELL *a_head, tCELL *a_curr, char a_mode, char a_format)
{
   /*> if (a_mode == HIST_BEG && a_head != a_curr)   a_mode = HIST_ADD;               <*/
   CELL_change (NULL, a_mode, a_curr->tab, a_curr->col, a_curr->row, "");
   return 0;
}

char         /*-> change cell number formatting ------[ ------ [gc.940.252.A4]*/ /*-[04.0000.303.#]-*/ /*-[--.---.---.--]-*/
CELL_units         (tCELL *a_head, tCELL *a_curr, char a_mode, char a_unit)
{
   /*---(design notes)-------------------*/
   /*
    *  update all cells to new format regardless of type to keep complexity
    *  down and flexibility up.  if there is no cell, simply move on.
    */
   /*---(defenses)-----------------------*/
   if (a_head == NULL || a_curr == NULL)    return  0;
   /*---(prepare)------------------------*/
   /*> if (a_mode == HIST_BEG && a_head != a_curr)   a_mode = HIST_ADD;               <*/
   HIST_format (a_mode, a_curr->tab, a_curr->col, a_curr->row, a_curr->unit, a_unit);
   a_curr->unit = a_unit;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> change cell number formatting ------[ ------ [gc.940.252.A4]*/ /*-[04.0000.303.#]-*/ /*-[--.---.---.--]-*/
CELL_format        (tCELL *a_head, tCELL *a_curr, char a_mode, char a_format)
{
   /*---(design notes)-------------------*/
   /*
    *  update all cells to new format regardless of type to keep complexity
    *  down and flexibility up.  if there is no cell, simply move on.
    */
   char        x_type      =  '-';
   /*---(defenses)-----------------------*/
   if (a_head == NULL || a_curr == NULL)        return  0;
   if (a_curr->align == '+')                    return  0;
   if (a_format  == '"')  a_format = a_curr->format;
   /*---(reset printing errors)----------*/
   if (a_curr->type == YCALC_DATA_ERROR && a_curr->print != NULL && strncmp (a_curr->print, "#p/", 3) == 0) {
      x_type = a_curr->print [9];  /*    #p/ali  (=)  */
   } else {
      x_type = a_curr->type;
   }
   /*---(prepare)------------------------*/
   /*> if (a_mode == HIST_BEG && a_head != a_curr)   a_mode = HIST_ADD;               <*/
   if      (strchr (YCALC_GROUP_STR , x_type) != 0) {
      if (str9filler (a_format) < 0)  return 0;
      HIST_format (a_mode, a_curr->tab, a_curr->col, a_curr->row, a_curr->format, a_format);
      a_curr->format = a_format;
   } else if (strchr (YCALC_GROUP_NUM , x_type) != 0) {
      if (str9format (a_format) < 0)  return 0;
      HIST_format (a_mode, a_curr->tab, a_curr->col, a_curr->row, a_curr->format, a_format);
      a_curr->format = a_format;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> change cell horizontal alignment ---[ ------ [gc.940.262.94]*/ /*-[04.0000.403.Y]-*/ /*-[--.---.---.--]-*/
CELL_align         (tCELL *a_head, tCELL *a_curr, char a_mode, char a_align)
{
   /*---(defense)------------------------*/
   if (a_head == NULL || a_curr == NULL)    return  0;
   if (a_curr->align == '+')                    return  0;
   if (a_align   == '"')  a_align = a_curr->align;
   if (str9align (a_align) < 0)             return 0;
   /*---(process all cells in range)-----*/
   /*> if (a_mode == HIST_BEG && a_head != a_curr)   a_mode = HIST_ADD;               <*/
   HIST_align  (a_mode, a_curr->tab, a_curr->col, a_curr->row, a_curr->align, a_align);
   a_curr->align = a_align;
   /*---(complete)---------------------------*/
   return 0;
}

char         /*-> change cell decimal places ---------[ ------ [gc.950.272.94]*/ /*-[03.0000.303.S]-*/ /*-[--.---.---.--]-*/
CELL_decimals      (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num)
{
   /*---(locals)-----------+-----------+-*/
   char        x_type      =  '-';
   char        x_decs      = 0;
   /*---(defense)------------------------*/
   if (a_head == NULL || a_curr == NULL)    return  0;
   if (a_num  == '"')  x_decs = a_curr->decs;
   else                x_decs = a_num;
   /*---(fix ranges)-------------------------*/
   if      (x_decs <  '0')  x_decs = '0';
   else if (x_decs >  '9')  x_decs = '9';
   /*---(reset printing errors)----------*/
   if (a_curr->type == YCALC_DATA_ERROR && a_curr->print != NULL && strncmp (a_curr->print, "#p/", 3) == 0) {
      x_type = a_curr->print [9];  /*    #p/ali  (=)  */
   } else {
      x_type = a_curr->type;
   }
   /*---(update)-----------------------*/
   if (strchr (YCALC_GROUP_NUM , x_type) != 0) {
      HIST_decimals (a_mode, a_curr->tab, a_curr->col, a_curr->row, a_curr->decs, x_decs);
      a_curr->decs = x_decs;
   }
   /*---(complete)---------------------------*/
   return 0;
}

char
CELL_visual        (char a_what, char a_mode, char a_how)
{
   /*---(design notes)-------------------*/
   /*
    * called functions return...
    *   0     for success
    *   < 0   to indicate error/stop
    *   > 0   to indicate done early, no need to continue
    *
    */
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   tCELL      *x_first     = NULL;
   tCELL      *x_next      = NULL;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   int         x_total     =    0;
   int         x_handle    =    0;
   int         x_top       =    0;
   int         x_left      =    0;
   /*---(header)-------------------------*/
   DEBUG_CELL    yLOG_enter  (__FUNCTION__);
   DEBUG_CELL    yLOG_char   ("a_what"    , a_what);
   DEBUG_CELL    yLOG_char   ("a_mode"    , a_mode);
   DEBUG_CELL    yLOG_char   ("a_how"     , a_how);
   /*---(get first)----------------------*/
   rc       = yVIKEYS_first (&x_tab, &x_col, &x_row, NULL);
   DEBUG_CELL    yLOG_value  ("rc"        , rc);
   x_first  = LOC_cell_at_loc (x_tab, x_col, x_row);
   x_top    = x_row;
   x_left   = x_col;
   DEBUG_CELL    yLOG_point  ("x_first"   , x_first);
   x_next   = x_first;
   /*---(process range)------------------*/
   while (rc >= 0) {
      ++x_total;
      /*---(widths)----------------------*/
      if (x_top  == x_row && a_what == CHANGE_WIDTH) {
         rc = COL_visual (x_tab, x_col, x_row, a_mode, a_how);
         DEBUG_CELL   yLOG_value  ("width"     , rc);
         if (a_mode == HIST_BEG)  a_mode = HIST_ADD;
      }
      /*---(heights)---------------------*/
      else if (x_left == x_col && a_what == CHANGE_HEIGHT) {
         rc = ROW_visual (x_tab, x_col, x_row, a_mode, a_how);
         DEBUG_CELL   yLOG_value  ("height"    , rc);
         if (a_mode == HIST_BEG)  a_mode = HIST_ADD;
      }
      /*---(cell-specific)---------------*/
      else if (x_next != NULL) {
         DEBUG_CELL   yLOG_note   ("cell exists");
         ++x_handle;
         switch (a_what) {
         case CHANGE_DECIMAL : rc = CELL_decimals     (x_first, x_next, a_mode, a_how); break;
         case CHANGE_ALIGN   : rc = CELL_align        (x_first, x_next, a_mode, a_how); break;
         case CHANGE_FORMAT  : rc = CELL_format       (x_first, x_next, a_mode, a_how); break;
         case CHANGE_UNITS   : rc = CELL_units        (x_first, x_next, a_mode, a_how); break;
         case CHANGE_ERASE   : rc = CELL_erase        (x_first, x_next, a_mode, a_how); break;
                               /*> case CHANGE_MERGE   : rc = CELL_merge_visu   (x_first, x_next, a_mode, a_how); break;   <*/
                               /*> case CHANGE_UNMERGE : rc = CELL_unmerge_visu (x_first, x_next, a_mode, a_how); break;   <*/
         }
         DEBUG_CELL   yLOG_value  ("handle_rc" , rc);
         /*---(trouble)---------------------*/
         if (rc < 0) {
            DEBUG_CELL    yLOG_note   ("detailed function indicated trouble");
            DEBUG_CELL    yLOG_exitr  (__FUNCTION__, rc);
            return rc;
         }
         /*---(early exit)------------------*/
         if (rc > 0)  {
            DEBUG_CELL    yLOG_note   ("detailed function indicated done early");
            DEBUG_CELL    yLOG_exit   (__FUNCTION__);
            return 0;
         }
         /*---(done)------------------------*/
         if (a_mode == HIST_BEG)  a_mode = HIST_ADD;
         api_ycalc_printer (x_next);
      }
      /*---(get next)--------------------*/
      rc      = yVIKEYS_next  (&x_tab, &x_col, &x_row, NULL);
      DEBUG_CELL   yLOG_value  ("next_rc"   , rc);
      x_next  = LOC_cell_at_loc (x_tab, x_col, x_row);
      DEBUG_CELL   yLOG_point  ("x_next"    , x_next);
      /*---(done)------------------------*/
   }
   DEBUG_CELL    yLOG_value  ("x_total"   , x_total);
   DEBUG_CELL    yLOG_value  ("x_handle"  , x_handle);
   /*---(complete)-----------------------*/
   DEBUG_CELL    yLOG_exit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        yparse functions                      ----===*/
/*====================------------------------------------====================*/
static void  o___YPARSE__________o () { return; }

char
CELL_reader          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_verb      [LEN_LABEL];
   char        x_label     [LEN_LABEL];
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   char        x_format    [LEN_LABEL];
   char        x_source    [LEN_RECD ];
   tCELL      *x_new       = NULL;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(get verb)-----------------------*/
   rc = yPARSE_popstr (x_verb);
   DEBUG_INPT   yLOG_value   ("pop verb"  , rc);
   DEBUG_INPT   yLOG_info    ("x_verb"    , x_verb);
   --rce;  if (strncmp ("cell", x_verb, 4) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   rc = yPARSE_popstr (x_label);
   DEBUG_INPT   yLOG_value   ("pop label" , rc);
   DEBUG_INPT   yLOG_info    ("label"     , x_label);
   rc = str2gyges (x_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_ADAPT);
   DEBUG_INPT  yLOG_value   ("parse"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_complex ("location"  , "%2dt, %3dc, %4dr", x_tab, x_col, x_row);
   rc = str4gyges (x_tab, x_col, x_row, 0, 0, x_label, YSTR_ADAPT);
   DEBUG_INPT  yLOG_value   ("legal"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(format)-------------------------*/
   rc = yPARSE_popstr  (&x_format);
   DEBUG_INPT   yLOG_value   ("pop format", rc);
   if      (strcmp (x_format , "")         == 0) strcpy  (x_format, "??0--");
   else if (strcmp (x_format , "-")        == 0) strcpy  (x_format, "??0--");
   else if (strlen (x_format) <  5)              strcpy  (x_format, "??0--");
   DEBUG_INPT  yLOG_info    ("x_format"  , x_format);
   /*---(source)-------------------------*/
   rc = yPARSE_popstr  (&x_source);
   DEBUG_INPT   yLOG_value   ("pop source", rc);
   DEBUG_INPT   yLOG_info    ("source"    , x_source);
   strldchg (x_source, G_CHAR_STORAGE, G_KEY_SPACE, LEN_RECD);
   DEBUG_INPT   yLOG_info    ("source"    , x_source);
   /*---(update)-------------------------*/
   x_new = CELL_overwrite (HIST_NONE, x_tab, x_col, x_row, x_source, x_format);
   DEBUG_INPT  yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 1;
}

char         /*-> write a single cell to a file ------[ leaf   [ge.630.514.50]*/ /*-[02.0000.124.!]-*/ /*-[--.---.---.--]-*/
CELL_writer        (tCELL *a_curr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_format    [LEN_LABEL] = "";
   char        x_level     [LEN_LABEL] = "";
   char        t           [LEN_RECD]  = "";
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(defenses)-----------------------*/
   DEBUG_OUTP   yLOG_point   ("a_curr"    , a_curr);
   --rce;  if (a_curr  == NULL) {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_OUTP   yLOG_point   ("label"     , a_curr->label);
   --rce;  if (a_curr->label  == NULL) {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_OUTP   yLOG_info    ("label"     , a_curr->label);
   DEBUG_OUTP   yLOG_complex ("type"      , "%-3d (%c)", a_curr->type, a_curr->type);
   if (a_curr->type == '-') {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_OUTP   yLOG_point   ("source"    , a_curr->source);
   if (a_curr->source == NULL) {      
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_OUTP   yLOG_info    ("source"    , a_curr->source);
   /*---(format)-------------------------*/
   DEBUG_OUTP   yLOG_complex ("align"     , "%-3d (%c)", a_curr->align , a_curr->align);
   DEBUG_OUTP   yLOG_complex ("format"    , "%-3d (%c)", a_curr->format, a_curr->format);
   DEBUG_OUTP   yLOG_complex ("decs"      , "%-3d (%c)", a_curr->decs  , a_curr->decs);
   DEBUG_OUTP   yLOG_complex ("unit"      , "%-3d (%c)", a_curr->unit  , a_curr->unit);
   sprintf (x_format, "%c%c%c%c-", a_curr->align, a_curr->format, a_curr->decs, a_curr->unit);
   /*---(call writer)--------------------*/
   strlcpy  (t, a_curr->source, LEN_RECD);
   strldchg (t, G_KEY_SPACE, G_CHAR_STORAGE, LEN_RECD);
   yPARSE_fullwrite ("cell", a_curr->label, x_format, t);
   /*---(complete)-----------------------*/
   DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
   return 1;
}

static      s_count     =    0;

char
CELL_writer_seq         (void *a_owner, void *a_deproot, int a_seq, int a_level)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   /*---(write)--------------------------*/
   rc = CELL_writer ((tCELL *) a_owner);
   if (rc > 0)  ++s_count;
   yPARSE_verb_break (s_count);
   /*---(complete)-----------------------*/
   return 0;
}

char
CELL_writer_all         (void)
{
   /*---(FIX ME)-------------------------*/
   /*
    *  change independent cells to walking cell master list, rather than
    *  every spreadsheet location -- much, much faster and clearer.
    *
    */
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   int         b           =    0;             /* iterator -- tabs               */
   int         x           =    0;             /* iterator -- columns            */
   int         x_end       =    0;
   int         y           =    0;             /* iterator -- row                */
   int         y_end       =    0;
   tCELL      *x_curr      = NULL;
   int         x_stamp     =    0;
   int         c           =    0;
   /*---(dependent)----------------------*/
   s_count = 0;
   yPARSE_verb_begin ("cell_dep");
   x_stamp   = rand ();
   rc = yCALC_seq_downup (x_stamp, CELL_writer_seq);
   yPARSE_verb_end   (s_count);
   c  = s_count;
   /*---(independent)--------------------*/
   s_count = 0;
   yPARSE_verb_begin ("cell");
   for (b = 0; b < NTAB; ++b) {
      x_end = COL_max (b) - 1;
      y_end = ROW_max (b) - 1;
      for (x = 0; x <= x_end; ++x) {
         for (y = 0; y <= y_end; ++y) {
            x_curr = LOC_cell_at_loc (b, x, y);
            if (x_curr    == NULL)                         continue;
            if (x_curr->source == NULL)                         continue;
            if (x_curr->type == YCALC_DATA_BLANK)             continue;
            if (yCALC_getstamp (x_curr->ycalc) == x_stamp) continue;
            rc = CELL_writer (x_curr);
            if (rc > 0)  ++s_count;
            yPARSE_verb_break (s_count);
         }
      }
   }
   yPARSE_verb_end   (s_count);
   c += s_count;
   if (c > 100)  c = 100;
   /*---(complete)-----------------------*/
   DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
   return c;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNIT_TEST_______o () { return; }

char*        /*-> unit test accessor -----------------[ light  [us.960.251.A0]*/ /*-[02.0000.00#.#]-*/ /*-[--.---.---.--]-*/
CELL__unit         (char *a_question, tCELL *a_cell)
{
   /*---(locals)-----------+-----------+-*/
   char        x_found     = '-';
   int         i           =  0;
   tCELL      *x_curr      = NULL;
   int         x_fore      = 0;
   int         x_back      = 0;
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_cell           : question not understood");
   /*---(find cell by pointer)-----------*/
   x_curr  = hcell;
   for (i = 0; i < NCEL; ++i) {
      if (x_curr == NULL)     break;
      if (x_curr == a_cell) {
         x_found = 'y';
         break;
      }
      x_curr = x_curr->next;
   }
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "cell_where")    == 0) {
      if (x_found == 'y')  snprintf (unit_answer, LEN_FULL, "s_cell location  : ptr=%10p, tab=%4d, col=%4d, row=%4d", x_curr, x_curr->tab, x_curr->col, x_curr->row);
      else                 snprintf (unit_answer, LEN_FULL, "s_cell location  : ptr=%10p, tab=%4d, col=%4d, row=%4d", x_curr, -10        , -10        , -10        );
   }
   else if (strcmp(a_question, "cell_list")      == 0) {
      snprintf(unit_answer, LEN_FULL, "s_cell main list : num=%4d, head=%10p, tail=%10p", NCEL, hcell, tcell);
   }
   else if (strcmp(a_question, "cell_count")     == 0) {
      x_curr = hcell; while (x_curr != NULL) { ++x_fore; x_curr = x_curr->next; }
      x_curr = tcell; while (x_curr != NULL) { ++x_back; x_curr = x_curr->prev; }
      snprintf (unit_answer, LEN_FULL, "s_cell count     : all=%4d, num=%4d, fore=%4d, back=%4d", ACEL, NCEL, x_fore, x_back);
   }
   /*---(printing)-----------------------*/
   /*> else if (strcmp(a_question, "cell_print")     == 0) {                          <* 
    *>    snprintf (unit_answer, LEN_FULL, "s_cell print     : <<%s>>", s_print);     <* 
    *> }                                                                              <*/
   /*---(complete)-----------------------*/
   return unit_answer;
}

char*        /*-> unit testing accessor --------------[ light  [us.IA0.2A5.X3]*/ /*-[02.0000.00#.#]-*/ /*-[--.---.---.--]-*/
CELL__unitnew      (char *a_question, char *a_label)
{  /*---(design notes)--------------------------------------------------------*/
   /*
    *  i formerly used cell pointers as the second argument, but it was hugely
    *  problematic when testing cells that are created and destroyed regularly.
    *  switching to labels adds an enormous safety factor and takes complexity
    *  out of the unit testing script.  more safety, more resilience, all good.
    */
   /*---(locals)-----------+-----------+-*/
   int         len         = 0;
   char        temp        [LEN_RECD];
   char        x_label     [LEN_LABEL];
   int         x_fore      = 0;
   int         x_back      = 0;
   tDEP       *x_dep       = NULL;
   tCELL      *x_cell      = NULL;
   char        rc          = 0;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_celln          : question not understood");
   /*---(identify the cell pointer)------*/
   if (a_label == NULL || strcmp ("root", a_label) == 0) {
      /*> x_cell = s_root;                                                            <*/
      sprintf (unit_answer, "s_celln error    : can not call on dependency s_root");
      return unit_answer;
   } else {
      rc     = str2gyges (a_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_CHECK);
      if (rc < 0) {
         sprintf (unit_answer, "s_celln error    : label <%s> not legal", a_label);
         return unit_answer;
      }
      rc = str4gyges (x_tab, x_col, x_row, 0, 0, x_label, YSTR_LEGAL);
      if (rc < 0) {
         sprintf (unit_answer, "s_celln error    : label <%s> not in-range", a_label);
         return unit_answer;
      }
      x_cell = LOC_cell_at_loc  (x_tab, x_col, x_row);
      /*> if (x_cell == NULL) {                                                         <* 
       *>    sprintf (unit_answer, "s_celln          : label <%s> is NULL", a_label);   <* 
       *>    return unit_answer;                                                        <* 
       *> }                                                                             <*/
   }
   /*---(cell contents)------------------*/
   if (strcmp(a_question, "info"     )      == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln info     : --- --- --- --- --- --- ----- -----");
      else                             snprintf(unit_answer, LEN_FULL, "s_celln info     : t=%c a=%c f=%c d=%c u=%c 5=%c w=%3d h=%3d", x_cell->type, x_cell->align, x_cell->format, x_cell->decs, x_cell->unit, '-', COL_width (x_cell->tab, x_cell->col), ROW_height (x_cell->tab, x_cell->row));
   }
   else if   (strcmp(a_question, "source"     )    == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln source   : (----) ::");
      else if (x_cell->source     == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln source   : (null) ::");
      else                             snprintf(unit_answer, LEN_FULL, "s_celln source   : (%4d) :%-.40s:", x_cell->len, x_cell->source);
   }
   else if (strcmp(a_question, "value"     )     == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln value    :         ---.------");
      else                             snprintf(unit_answer, LEN_FULL, "s_celln value    : %18.6F", x_cell->v_num);
   }
   else if (strcmp(a_question, "modified")  == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln modded   : (----) ::");
      else if (x_cell->v_str == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln modded   : (null) ::");
      else                             snprintf(unit_answer, LEN_FULL, "s_celln modded   : (%4d) :%-.40s:", (int) strlen(x_cell->v_str), x_cell->v_str);
   }
   else if (strcmp(a_question, "printable") == 0) {
      /*> snprintf(unit_answer, LEN_FULL, "Cell Printable   : (%4d) :%-.40s:", (int) strlen(x_cell->print), x_cell->print);   <*/
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln print    : (----) ::");
      else if (x_cell->print     == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln print    : (null) ::");
      else {
         len = strlen (x_cell->print);
         if      (len        == 0   )  snprintf(unit_answer, LEN_FULL, "s_celln print    : (null) ::");
         else if (len        >= 35  )  snprintf(unit_answer, LEN_FULL, "s_celln print    : (%4d) :%-34.34s++", (int) strlen(x_cell->print), x_cell->print);
         else                          snprintf(unit_answer, LEN_FULL, "s_celln print    : (%4d) :%s:"       , (int) strlen(x_cell->print), x_cell->print);
      }
   }
   else if (strcmp(a_question, "cell_contents")  == 0) {
      if      (x_cell       == NULL)   snprintf(unit_answer, LEN_FULL, "s_celln cont (-) : (--:--) ::");
      else                             snprintf(unit_answer, LEN_FULL, "s_celln cont (%c) : (%2d:%2d) :%-.40s:", (g_contents[my.cpos] >= ' ' && g_contents[my.cpos] <= '~') ? g_contents[my.cpos] : ' ', my.cpos, (int) strlen(g_contents), g_contents);
   }
   /*---(cell contents)------------------*/
   else if (strcmp(a_question, "cell_rpn")       == 0) {
      /*> if      (x_cell       == NULL)   snprintf(unit_answer, LEN_FULL, "s_celln rpn      : (----) -");                                  <* 
       *> else if (x_cell->nrpn == 0)      snprintf(unit_answer, LEN_FULL, "s_celln rpn      : (%4d) ."     , x_cell->nrpn);                <* 
       *> else                             snprintf(unit_answer, LEN_FULL, "s_celln rpn      : (%4d) %s"    , x_cell->nrpn, x_cell->rpn);   <*/
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}

char         /*-> force direct cell content change ---[ light  [ue.320.213.20]*/ /*-[00.0000.00#.#]-*/ /*-[--.---.---.--]-*/
CELL__unitchange     (tCELL *a_cell, char *a_source)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(defence)------------------------*/
   --rce;  if (a_cell   == NULL)  return rce;
   --rce;  if (a_source == NULL)  return rce;
   /*---(change)-------------------------*/
   a_cell->source = strndup (a_source, LEN_RECD);
   a_cell->len = strlen  (a_cell->source);
   /*---(complete)-----------------------*/
   return 0;
}



/*============================----end-of-source---============================*/
