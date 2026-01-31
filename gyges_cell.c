/*============================----beg-of-source---============================*/



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

tCELL      *hcell = NULL;    /* head pointer for cell data structure          */
tCELL      *tcell = NULL;    /* tail pointer for cell data structure          */
tCELL      *rcell = NULL;    /* root pointer for tree operations              */
int         ncell = 0;       /* count of linked cells in data structure       */
int         acell = 0;       /* count of all cells                            */

char       *g_tbd         = "tbd";
tCELL       denada;


static int s_count        =    0;


/*====================------------------------------------====================*/
/*===----                      memory allocation                       ----===*/
/*====================------------------------------------====================*/
static void  o___MEMORY__________o () { return; }

char         /*-> create a single new empty cell -----[ leaf   [fe.KB4.224.80]*/ /*-[12.0000.123.A]-*/ /*-[--.---.---.--]-*/
CELL__new_driver   (char a_root, tCELL **a_cell, char a_linked)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_tries     =    0;
   tCELL      *x_new       = NULL;
   /*---(begin)--------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CELL   yLOG_point   ("a_cell"    , a_cell);
   --rce;  if (a_cell == NULL) {
      DEBUG_CELL   yLOG_note    ("pointer can not be null (no point)");
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_root == 'y') {
      DEBUG_CELL   yLOG_note    ("ROOT creation process");
      DEBUG_CELL   yLOG_point   ("my.root"   , my.root);
      if (my.root != NULL) {
         DEBUG_CELL   yLOG_note    ("root exists, attempting to recreate");
         DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   } else {
      DEBUG_CELL   yLOG_note    ("normal cell creation process");
   }
   *a_cell = NULL;
   DEBUG_CELL   yLOG_value   ("ACEL (bef)", ACEL);
   DEBUG_CELL   yLOG_value   ("NCEL (bef)", NCEL);
   /*---(create cell)--------------------*/
   while (x_new == NULL) {
      ++x_tries;
      x_new = (tCELL *) malloc (sizeof (tCELL));
      if (x_tries > 10)   break;
   }
   DEBUG_CELL   yLOG_value   ("x_tries"   , x_tries);
   DEBUG_CELL   yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_CELL   yLOG_note    ("cell could not be created");
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ++ACEL;
   DEBUG_CELL   yLOG_value   ("ACEL (aft)", ACEL);
   /*---(into linked list)---------------*/
   DEBUG_CELL   yLOG_char    ("a_linked"  , a_linked);
   x_new->d_linked  = a_linked;
   x_new->d_next    = NULL;
   x_new->d_prev    = NULL;
   if (a_linked != UNLINKED) {
      DEBUG_CELL   yLOG_note    ("LINKED CELL");
      if (hcell == NULL) {
         DEBUG_CELL   yLOG_note    ("first new cell added");
         hcell         = x_new;
         tcell         = x_new;
      } else {
         DEBUG_CELL   yLOG_note    ("append new cell to end");
         x_new->d_prev = tcell;
         x_new->d_next = NULL;
         tcell->d_next = x_new;
         tcell         = x_new;
      }
      ++NCEL;
   } else {
      DEBUG_CELL   yLOG_note    ("UNLINKED CELL");
   }
   DEBUG_CELL   yLOG_value   ("NCEL (aft)", NCEL);
   /*---(location)-----------------------*/
   DEBUG_CELL   yLOG_note    ("assign default values/init");
   x_new->d_tab       = UNHOOKED;
   x_new->d_col       = UNHOOKED;
   x_new->d_row       = UNHOOKED;
   x_new->d_label     = g_tbd;
   x_new->d_key       = UNHOOKED;
   /*---(source)------------------------*/
   x_new->d_source    = NULL;
   x_new->d_len       = 0;
   /*---(results)-----------------------*/
   x_new->d_type      = YCALC_DATA_BLANK;
   x_new->d_num       =  0.0;
   x_new->d_str       = NULL;
   /*---(formatting)--------------------*/
   x_new->d_align     =  '?';
   x_new->d_format    =  '?';
   x_new->d_decs      =  '0';
   x_new->d_unit      =  '-';
   x_new->d_fill      =  '?';
   x_new->d_zero      =  '?';
   x_new->d_print     = NULL;
   x_new->d_note      =  '-';
   x_new->d_srch      =    0;
   /*---(btree)--------------------------*/
   x_new->b_left    = NULL;
   x_new->b_right   = NULL;
   /*---(calculations)-------------------*/
   x_new->d_ycalc     = NULL;
   DEBUG_CELL   yLOG_note    ("successful");
   /*---(row/col)------------------------*/
   x_new->d_Cowner  = NULL;
   x_new->d_Cprev    = NULL;
   x_new->d_Cnext    = NULL;
   x_new->d_Rowner  = NULL;
   x_new->d_Rprev    = NULL;
   x_new->d_Rnext    = NULL;
   /*---(sort)---------------------------*/
   /*> rc = api_ysort_update ();                                                      <*/
   /*> DEBUG_CELL   yLOG_value   ("ysort"     , rc);                                  <*/
   /*---(return)-------------------------*/
   *a_cell = x_new;
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  CELL__new          (tCELL **a_cell, char a_linked) { return CELL__new_driver ('-', a_cell, a_linked); }
char  CELL__new_root     (tCELL **a_cell)                { return CELL__new_driver ('y', a_cell, UNLINKED); }

char         /*-> remove a cell completely -----------[ ------ [fe.943.224.81]*/ /*-[11.0000.133.7]-*/ /*-[--.---.---.--]-*/
CELL__free_driver  (char a_root, tCELL **a_cell)
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
   DEBUG_CELL   yLOG_char    ("a_root"    , a_root);
   --rce;  if (a_root != 'y') {
      DEBUG_CELL   yLOG_note    ("a_root != 'y', normal cell");
      if (*a_cell == my.root) {
         DEBUG_CELL   yLOG_note    ("normal process attempted to free root");
         DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   } else {
      if (*a_cell != my.root) {
         DEBUG_CELL   yLOG_note    ("root process can only free root");
         DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*> rc = CELL__valid (*a_cell, a_linked);                                          <* 
    *> --rce;  if (rc < 0) {                                                          <* 
    *>    DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <*/
   DEBUG_CELL   yLOG_info    ("d_label"   , (*a_cell)->d_label);
   CELL__wipe (*a_cell);
   DEBUG_CELL   yLOG_value   ("ACEL (bef)", ACEL);
   DEBUG_CELL   yLOG_value   ("NCEL (bef)", NCEL);
   /*---(out of linked list)-------------*/
   if ((*a_cell)->d_linked == LINKED) {
      DEBUG_CELL   yLOG_note    ("linked cell, unlinking now");
      if ((*a_cell)->d_next != NULL)   (*a_cell)->d_next->d_prev = (*a_cell)->d_prev;
      else                             tcell                     = (*a_cell)->d_prev;
      if ((*a_cell)->d_prev != NULL)   (*a_cell)->d_prev->d_next = (*a_cell)->d_next;
      else                             hcell                     = (*a_cell)->d_next;
      --NCEL;
   }
   DEBUG_CELL   yLOG_value   ("ncell"     , ncell);
   /*---(free main)----------------------*/
   DEBUG_CELL   yLOG_note    ("freeing and nulling");
   (*a_cell)->d_prev = (*a_cell)->d_next = NULL;
   free (*a_cell);
   *a_cell = NULL;
   --ACEL;
   DEBUG_CELL   yLOG_value   ("acell"     , acell);
   /*---(sort)---------------------------*/
   /*> DEBUG_CELL    yLOG_complex ("ends (bef)", "%3a, %3n, head %p, tail %p", acell, ncell, hcell, tcell);   <*/
   /*> rc = api_ysort_update ();                                                      <*/
   /*> DEBUG_CELL   yLOG_value   ("ysort"     , rc);                                  <*/
   /*> DEBUG_CELL    yLOG_complex ("ends (aft)", "%3a, %3n, head %p, tail %p", acell, ncell, hcell, tcell);   <*/
   /*---(debug)--------------------------*/
   CELL_debug_list ();
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  CELL__free         (tCELL **a_cell) { return CELL__free_driver ('-', a_cell); }
char  CELL__free_root    (tCELL **a_cell) { return CELL__free_driver ('y', a_cell); }



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

/*> char                                                                              <* 
 *> CELL_start         (void)                                                         <* 
 *> {                                                                                 <* 
 *>    /+---(locals)-----------+-----------+-+/                                       <* 
 *>    tCELL      *curr        = NULL;                                                <* 
 *>    tCELL      *next        = NULL;                                                <* 
 *>    char        rc          = 0;                                                   <* 
 *>    /+---(header)-------------------------+/                                       <* 
 *>    DEBUG_CELL   yLOG_enter   (__FUNCTION__);                                      <* 
 *>    /+---(walk through list)--------------+/                                       <* 
 *>    next = hcell;                                                                  <* 
 *>    DEBUG_CELL   yLOG_point   ("hcell"     , hcell);                               <* 
 *>    while (next != NULL) {                                                         <* 
 *>       curr = next;                                                                <* 
 *>       next = curr->d_next;                                                        <* 
 *>       rc = CELL__wipe    (curr);                                                  <* 
 *>       DEBUG_CELL   yLOG_value   ("wipe rc"   , rc);                               <* 
 *>       rc = LOC_unhook   (curr);                                                   <* 
 *>       DEBUG_CELL   yLOG_value   ("unhook rc" , rc);                               <* 
 *>       rc = CELL__free    (&curr);                                                 <* 
 *>       DEBUG_CELL   yLOG_value   ("free rc"   , rc);                               <* 
 *>       DEBUG_CELL   yLOG_point   ("next"      , next);                             <* 
 *>    }                                                                              <* 
 *>    /+---(clean ends)---------------------+/                                       <* 
 *>    if (ncell == 0) {                                                              <* 
 *>       hcell = NULL;                                                               <* 
 *>       tcell = NULL;                                                               <* 
 *>    }                                                                              <* 
 *>    /+---(complete)-----------------------+/                                       <* 
 *>    DEBUG_CELL   yLOG_exit    (__FUNCTION__);                                      <* 
 *>    return 0;                                                                      <* 
 *> }                                                                                 <*/

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
   DEBUG_CELL   yLOG_value   ("ncell"     , ncell);
   next = hcell;
   DEBUG_CELL   yLOG_point   ("hcell"     , hcell);
   while (next != NULL) {
      DEBUG_CELL   yLOG_value   ("ncell"     , ncell);
      curr = next;
      next = curr->d_next;
      rc = CELL__wipe    (curr);
      DEBUG_CELL   yLOG_value   ("wipe rc"   , rc);
      rc = LOC_unhook   (curr);
      DEBUG_CELL   yLOG_value   ("unhook rc" , rc);
      rc = CELL__free    (&curr);
      DEBUG_CELL   yLOG_value   ("free rc"   , rc);
      DEBUG_CELL   yLOG_point   ("next"      , next);
   }
   /*---(report misses)------------------*/
   next = hcell;
   DEBUG_CELL   yLOG_point   ("hcell"     , hcell);
   while (next != NULL) {
      DEBUG_CELL   yLOG_complex ("next"      , "%p, %2du, %3dx, %4dy, %s", next, next->d_tab, next->d_col, next->d_row, next->d_label);
      next = next->d_next;
   }
   /*---(clean ends)---------------------*/
   DEBUG_CELL   yLOG_value   ("ncell"     , ncell);
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
   /*> denada.label = strdup ("DENADA");                                              <*/
   /*> yVIKEYS_dump_add ("cells"      , CELL_dump);                                   <*/
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
   DEBUG_CELL   yLOG_note    ("free the ROOT");
   CELL__free_root (&my.root);
   my.root = NULL;
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        basic utilities                       ----===*/
/*====================------------------------------------====================*/
static void  o___BASICS__________o () { return; }

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
   /*---(label)--------------------------*/
   DEBUG_CELL   yLOG_info    ("label"     , a_curr->d_label );
   if (a_curr->d_label  != g_tbd)   free (a_curr->d_label );
   a_curr->d_label  = g_tbd;
   /*---(source)-------------------------*/
   DEBUG_CELL   yLOG_info    ("source"    , a_curr->d_source);
   if (a_curr->d_source != NULL)   free (a_curr->d_source);
   a_curr->d_source = NULL;
   a_curr->d_len    = 0;
   /*---(results)------------------------*/
   DEBUG_CELL   yLOG_info    ("d_str"     , a_curr->d_str );
   if (a_curr->d_str  != NULL)   free (a_curr->d_str );
   a_curr->d_str  = NULL;
   a_curr->d_type   = YCALC_DATA_BLANK;
   a_curr->d_num  = 0.0;
   /*---(printable)----------------------*/
   DEBUG_CELL   yLOG_info    ("print"     , a_curr->d_print );
   if (a_curr->d_print  != NULL)   free (a_curr->d_print );
   a_curr->d_print  = NULL;
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
   /*> if  (a_cell == x_save) {                                                       <* 
    *>    DEBUG_CELL   yLOG_snote   ("quick path");                                   <* 
    *>    DEBUG_CELL   yLOG_sexit   (__FUNCTION__);                                   <* 
    *>    return 1;                                                                   <* 
    *> }                                                                              <*/
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
      x_save = x_save->d_next;
   }
   DEBUG_CELL   yLOG_snote   ("NOT found");
   --rce;
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_sexitr  (__FUNCTION__, rce);
   return rce;
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
 *>    DEBUG_CELL   yLOG_info    ("label"     ,  a_curr->d_label);                                                                   <* 
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
static void  o___EXISTANCE_______o () { return; }

char         /*-> tbd --------------------------------[ ------ [fe.732.424.33]*/ /*-[01.0000.014.K]-*/ /*-[--.---.---.--]-*/
CELL__create       (tCELL **a_cell, int a_tab, int a_col, int a_row)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_label     [LEN_LABEL];
   tCELL      *x_curr      = NULL;
   /*---(defenses)-----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_CELL   yLOG_point   ("a_cell"    , a_cell);
   --rce;  if (a_cell == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   *a_cell = NULL;
   /*---(defenses)-----------------------*/
   rc = ystr4gyges (a_tab, a_col, a_row, 0, 0, x_label, YSTR_ADAPT);
   DEBUG_CELL   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for existing)----------*/
   api_ysort_by_coord (&x_curr, a_tab, a_col, a_row);
   DEBUG_CELL   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr != NULL) {
      *a_cell = x_curr;
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
   DEBUG_CELL   yLOG_point   ("*a_cell"   , *a_cell);
   /*---(clear it out)----------------*/
   rc = LOC_hook  (*a_cell, a_tab, a_col, a_row);
   DEBUG_CELL   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CELL   yLOG_complex ("DEBUG 1"   , "%-10.10s, %2dt, %3dc, %4dr", (*a_cell)->d_label, (*a_cell)->d_tab, (*a_cell)->d_col, (*a_cell)->d_row);
   /*---(debug)--------------------------*/
   CELL_debug_list ();
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
   DEBUG_CELL   yLOG_complex ("args"      , "%c, %2dt, %3dc, %4dr", a_mode, a_tab, a_col, a_row);
   rc = ystr4gyges (a_tab, a_col, a_row, 0, 0, x_label, YSTR_USABLE);
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
   if (x_curr->d_source != NULL) {
      strcpy  (x_before, x_curr->d_source);
      sprintf (x_format, "%c%c%c%c-", x_curr->d_format, x_curr->d_align, x_curr->d_decs, x_curr->d_unit);
   }
   /*---(history)------------------------*/
   yMAP_mundo_delete (a_mode, x_curr->d_label, x_before, x_format);
   /*---(clear it out)----------------*/
   /*> DEBUG_CELL   yLOG_complex ("details"   , "ptr=%p, tab=%4d, col=%4d, row=%4d, t=%c, u=%d", x_curr, x_curr->d_tab, x_curr->d_col, x_curr->d_row, x_curr->d_type, x_curr->u);   <*/
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
   rc = CELL__free   (&x_curr);
   DEBUG_CELL   yLOG_value   ("free rc"   , rc);
   if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce - 1);
      return rce - 1;
   }
   /*---(debug)--------------------------*/
   CELL_debug_list ();
   /*---(complete)--------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char s_bsource  [LEN_RECD]  = "";
static char s_bformat  [LEN_LABEL] = "";

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
   char        x_locked    =    0;
   /*---(header)-------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   DEBUG_CELL   yLOG_complex ("args"      , "cell %p, mode %c, tab %4d, col %4d, row %4d", a_cell, a_mode, a_tab, a_col, a_row);
   /*---(defense)------------------------*/
   DEBUG_CELL   yLOG_value   ("a_tab"     , a_tab);
   x_locked = TAB_is_locked (a_tab);
   DEBUG_CELL   yLOG_value   ("x_locked"  , x_locked);
   --rce;  if (x_locked) {
      DEBUG_CELL   yLOG_note    ("can not complete, destination tab is locked");
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CELL   yLOG_point   ("a_source"  , a_source);
   --rce;  if (a_source == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CELL   yLOG_info    ("a_source"  , a_source);
   /*---(prepare)------------------------*/
   if (a_cell != NULL)  *a_cell = NULL;
   ystrlcpy (s_bsource, "", LEN_RECD);
   ystrlcpy (s_bformat, "??0--", LEN_LABEL);
   /*---(legal location)-----------------*/
   rc = ystr4gyges (a_tab, a_col, a_row, 0, 0, x_label, YSTR_USABLE);
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
      if (x_curr->d_source != NULL)  ystrlcpy (s_bsource, x_curr->d_source, LEN_RECD);
   } else if (x_curr == NULL) {
      rc = CELL__create (&x_curr, a_tab, a_col, a_row);
      DEBUG_CELL   yLOG_point   ("new cell"  , x_curr);
      if (x_curr == NULL) {
         DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   DEBUG_CELL   yLOG_info    ("cell label", x_curr->d_label);
   DEBUG_CELL   yLOG_complex ("DEBUG 9"   , "%-10.10s, %2dt, %3dc, %4dr", x_curr->d_label, x_curr->d_tab, x_curr->d_col, x_curr->d_row);
   sprintf (s_bformat, "%c%c%c%c-", x_curr->d_align, x_curr->d_format, x_curr->d_decs, x_curr->d_unit);
   DEBUG_CELL   yLOG_info    ("s_bformat" , s_bformat);
   /*---(history)------------------------*/
   if (a_mode != YMAP_NONE) {
      if      (a_source == NULL) {
         if (s_bsource [0] == '\0' && strcmp (s_bformat, DEF_FORMAT) == 0) {
            DEBUG_CELL   yLOG_note    ("nothing to do, try to clear a cleared cell");
         } else {
            yMAP_mundo_clear   (a_mode, x_curr->d_label, s_bformat, s_bsource, s_bformat);
         }
      } else if (a_source [0] == '\0') {
         if (s_bsource [0] == '\0' && strcmp (s_bformat, DEF_FORMAT) == 0) {
            DEBUG_CELL   yLOG_note    ("nothing to do, try to clear a cleared cell");
         } else {
            yMAP_mundo_clear   (a_mode, x_curr->d_label, s_bformat, s_bsource, s_bformat);
         }
      } else {
         yMAP_mundo_source  (a_mode, x_curr->d_label, s_bsource, a_source);
      }
   }
   /*---(update)-------------------------*/
   DEBUG_CELL   yLOG_note    ("change source and length values");
   if (x_curr->d_source != NULL)  free (x_curr->d_source);
   x_curr->d_source = strndup (a_source, LEN_RECD);
   x_curr->d_len    = strlen  (x_curr->d_source);
   /*---(interpret)----------------------*/
   DEBUG_CELL   yLOG_note    ("interpret new contents");
   rc = yCALC_handle (x_curr->d_label);
   DEBUG_CELL   yLOG_value   ("handle"    , rc);
   /*---(return)-------------------------*/
   rc = api_ysort_by_coord (&x_curr, a_tab, a_col, a_row);
   if (rc < 0 || x_curr == NULL) {
      DEBUG_CELL   yLOG_note    ("cell cleared, no longer exists");
   } else {
      DEBUG_CELL   yLOG_complex ("DEBUG 10"  , "%-10.10p, %-10.10s, %2dt, %3dc, %4dr", x_curr, x_curr->d_label, x_curr->d_tab, x_curr->d_col, x_curr->d_row);
   }
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
   char       x_aformat    [LEN_LABEL] = DEF_FORMAT;
   /*---(defense)------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   if (strlen (a_format) < 5) {
      DEBUG_CELL   yLOG_warn    ("format not valid length");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   /*---(start)--------------------------*/
   CELL_change (&x_new, YMAP_NONE, a_tab, a_col, a_row, a_source);
   if (x_new == NULL)  {
      DEBUG_CELL   yLOG_warn    ("x_new change returned a null pointer");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   /*---(history)------------------------*/
   if (a_format [0] != '´')   strcpy (x_aformat, a_format);
   if (a_mode != YMAP_NONE) {
      yMAP_mundo_overwrite (a_mode, x_new->d_label, s_bformat, s_bsource, x_aformat, a_source);
   }
   /*---(formatting)---------------------*/
   DEBUG_CELL   yLOG_note    ("update format fields");
   x_new->d_align  = x_aformat [0];
   DEBUG_CELL   yLOG_char    ("align"     , x_new->d_align);
   x_new->d_format = x_aformat [1];
   DEBUG_CELL   yLOG_char    ("format"    , x_new->d_format);
   x_new->d_decs   = x_aformat [2];
   DEBUG_CELL   yLOG_char    ("decs"      , x_new->d_decs);
   x_new->d_unit   = x_aformat [3];
   DEBUG_CELL   yLOG_char    ("unit"      , x_new->d_unit);
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
   if (a_old->d_source == NULL)  (*a_new)->d_source = NULL;
   else                        (*a_new)->d_source = strndup (a_old->d_source, LEN_RECD);
   (*a_new)->d_len      = a_old->d_len;
   /*---(copy formatting)----------------*/
   DEBUG_CELL   yLOG_note    ("copy old type and formatting");
   (*a_new)->d_type        = a_old->d_type;
   (*a_new)->d_align       = a_old->d_align;
   (*a_new)->d_format      = a_old->d_format;
   (*a_new)->d_decs        = a_old->d_decs;
   (*a_new)->d_unit        = a_old->d_unit;
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return  0;
}

char         /*-> erase cells in current selection ---[ ------ [ge.751.093.33]*/ /*-[01.0000.106.!]-*/ /*-[--.---.---.--]-*/
CELL_erase         (tCELL *a_head, tCELL *a_curr, char a_mode, char a_format)
{
   CELL_change (NULL, a_mode, a_curr->d_tab, a_curr->d_col, a_curr->d_row, "");
   return 0;
}




/*====================------------------------------------====================*/
/*===----                         formatting                           ----===*/
/*====================------------------------------------====================*/
static void  o___FORMATTING______o () { return; }

char         /*-> change cell horizontal alignment ---[ ------ [gc.940.262.94]*/ /*-[04.0000.403.Y]-*/ /*-[--.---.---.--]-*/
CELL_align         (tCELL *a_curr, char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_type      =  '-';
   char        x_prev      =  '0';
   /*---(defense)------------------------*/
   if (a_curr == NULL)            return  0;
   if (a_curr->d_align == '+')      return  0;
   /*---(existing)-----------------------*/
   x_prev = a_curr->d_align;
   /*---(limits)-------------------------*/
   if (a_abbr  == '"')  a_abbr = x_prev;
   if (ystr9align (a_abbr) < 0)   return 0;
   /*---(update)-------------------------*/
   a_curr->d_align = a_abbr;
   /*---(update)-------------------------*/
   api_ycalc_printer (a_curr);
   /*---(complete)---------------------------*/
   return x_prev;
}

char         /*-> change cell number formatting ------[ ------ [gc.940.252.A4]*/ /*-[04.0000.303.#]-*/ /*-[--.---.---.--]-*/
CELL_format        (tCELL *a_curr, char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_type      =  '-';
   char        x_prev      =  '0';
   /*---(defense)------------------------*/
   if (a_curr == NULL)            return  0;
   if (a_curr->d_align == '+')      return  0;
   /*---(existing)-----------------------*/
   x_prev = a_curr->d_format;
   /*---(limits)-------------------------*/
   if (a_abbr  == '"')  a_abbr = x_prev;
   /*---(reset printing errors)----------*/
   if (a_curr->d_type == YCALC_DATA_ERROR && a_curr->d_print != NULL && strncmp (a_curr->d_print, "#p/", 3) == 0) {
      x_type = a_curr->d_print [9];  /*    #p/ali  (=)  */
   } else {
      x_type = a_curr->d_type;
   }
   /*---(prepare)------------------------*/
   if        (strchr (YCALC_GROUP_STR , x_type) != 0) {
      if (ystr9filler (a_abbr) < 0)  return 0;
      a_curr->d_format = a_abbr;
   } else if (strchr (YCALC_GROUP_NUM , x_type) != 0) {
      if (ystr9format (a_abbr) < 0)  return 0;
      a_curr->d_format = a_abbr;
   } else {
      return 0;
   }
   /*---(update)-------------------------*/
   api_ycalc_printer (a_curr);
   /*---(complete)-----------------------*/
   return x_prev;
}

char         /*-> change cell decimal places ---------[ ------ [gc.950.272.94]*/ /*-[03.0000.303.S]-*/ /*-[--.---.---.--]-*/
CELL_decimals      (tCELL *a_curr, char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_type      =  '-';
   char        x_prev      =  '0';
   char        x_decs      =  '0';
   /*---(defense)------------------------*/
   if (a_curr == NULL)  return  0;
   /*---(existing)-----------------------*/
   x_prev = a_curr->d_decs;
   /*---(limits)-------------------------*/
   if (a_abbr  == '"')  x_decs = x_prev;
   else                 x_decs = a_abbr;
   /*---(limits)-------------------------*/
   if      (x_decs <  '0')  x_decs = '0';
   else if (x_decs >  '9')  x_decs = '9';
   /*---(reset printing errors)----------*/
   if (a_curr->d_type == YCALC_DATA_ERROR && a_curr->d_print != NULL && strncmp (a_curr->d_print, "#p/", 3) == 0) {
      x_type = a_curr->d_print [9];  /*    #p/ali  (=)  */
   } else {
      x_type = a_curr->d_type;
   }
   if (strchr (YCALC_GROUP_NUM , x_type) == 0)   return 0;
   /*---(update)-----------------------*/
   a_curr->d_decs = x_decs;
   /*---(update)-------------------------*/
   api_ycalc_printer (a_curr);
   /*---(complete)---------------------------*/
   return x_prev;
}

char         /*-> change cell number formatting ------[ ------ [gc.940.252.A4]*/ /*-[04.0000.303.#]-*/ /*-[--.---.---.--]-*/
CELL_units         (tCELL *a_curr, char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_type      =  '-';
   char        x_prev      =  '0';
   /*---(defense)------------------------*/
   if (a_curr == NULL)            return  0;
   /*---(existing)-----------------------*/
   x_prev = a_curr->d_unit;
   /*---(update)-------------------------*/
   x_type = a_curr->d_type;
   if (strchr (YCALC_GROUP_NUM , x_type) != 0) {
      a_curr->d_unit = a_abbr;
   } else {
      return 0;
   }
   /*---(update)-------------------------*/
   api_ycalc_printer (a_curr);
   /*---(complete)-----------------------*/
   return x_prev;
}

/*> char                                                                                                                             <* 
 *> CELL_visual        (char a_what, char a_mode, char a_how)                                                                        <* 
 *> {                                                                                                                                <* 
 *>    /+---(design notes)-------------------+/                                                                                      <* 
 *>    /+                                                                                                                            <* 
 *>     * called functions return...                                                                                                 <* 
 *>     *   0     for success                                                                                                        <* 
 *>     *   < 0   to indicate error/stop                                                                                             <* 
 *>     *   > 0   to indicate done early, no need to continue                                                                        <* 
 *>     *                                                                                                                            <* 
 *>     +/                                                                                                                           <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                      <* 
 *>    char        rc          =    0;                                                                                               <* 
 *>    tCELL      *x_first     = NULL;                                                                                               <* 
 *>    tCELL      *x_next      = NULL;                                                                                               <* 
 *>    ushort      x_tab       =    0;                                                                                               <* 
 *>    ushort      x_col       =    0;                                                                                               <* 
 *>    ushort      x_row       =    0;                                                                                               <* 
 *>    int         x_total     =    0;                                                                                               <* 
 *>    int         x_handle    =    0;                                                                                               <* 
 *>    int         x_top       =    0;                                                                                               <* 
 *>    int         x_left      =    0;                                                                                               <* 
 *>    /+---(header)-------------------------+/                                                                                      <* 
 *>    DEBUG_CELL    yLOG_enter  (__FUNCTION__);                                                                                     <* 
 *>    DEBUG_CELL    yLOG_char   ("a_what"    , a_what);                                                                             <* 
 *>    DEBUG_CELL    yLOG_char   ("a_mode"    , a_mode);                                                                             <* 
 *>    DEBUG_CELL    yLOG_char   ("a_how"     , a_how);                                                                              <* 
 *>    /+---(check for sizing)---------------+/                                                                                      <* 
 *>    if (a_what == CHANGE_WIDTH) {                                                                                                 <* 
 *>       rc = COL_multikey (a_how);                                                                                                 <* 
 *>       DEBUG_CELL    yLOG_exit   (__FUNCTION__);                                                                                  <* 
 *>       return 0;                                                                                                                  <* 
 *>    }                                                                                                                             <* 
 *>    if (a_what == CHANGE_HEIGHT) {                                                                                                <* 
 *>       rc = ROW_multikey (a_how);                                                                                                 <* 
 *>       DEBUG_CELL    yLOG_exit   (__FUNCTION__);                                                                                  <* 
 *>       return 0;                                                                                                                  <* 
 *>    }                                                                                                                             <* 
 *>    /+---(get first)----------------------+/                                                                                      <* 
 *>    rc       = yMAP_visu_first (&x_tab, &x_col, &x_row, NULL);                                                                    <* 
 *>    DEBUG_CELL    yLOG_value  ("rc"        , rc);                                                                                 <* 
 *>    x_first  = LOC_cell_at_loc (x_tab, x_col, x_row);                                                                             <* 
 *>    x_top    = x_row;                                                                                                             <* 
 *>    x_left   = x_col;                                                                                                             <* 
 *>    DEBUG_CELL    yLOG_point  ("x_first"   , x_first);                                                                            <* 
 *>    x_next   = x_first;                                                                                                           <* 
 *>    /+---(process range)------------------+/                                                                                      <* 
 *>    while (rc >= 0) {                                                                                                             <* 
 *>       ++x_total;                                                                                                                 <* 
 *>       /+---(cell-specific)---------------+/                                                                                      <* 
 *>       if (x_next != NULL) {                                                                                                      <* 
 *>          DEBUG_CELL   yLOG_note   ("cell exists");                                                                               <* 
 *>          ++x_handle;                                                                                                             <* 
 *>          switch (a_what) {                                                                                                       <* 
 *>          /+> case CHANGE_DECIMAL : rc = CELL_decimals     (x_first, x_next, a_mode, a_how); break;   <+/                         <* 
 *>          /+> case CHANGE_ALIGN   : rc = CELL_align        (x_first, x_next, a_mode, a_how); break;   <+/                         <* 
 *>          /+> case CHANGE_FORMAT  : rc = CELL_format       (x_first, x_next, a_mode, a_how); break;   <+/                         <* 
 *>          /+> case CHANGE_UNITS   : rc = CELL_units        (x_first, x_next, a_mode, a_how); break;   <+/                         <* 
 *>          case CHANGE_ERASE   : rc = CELL_erase        (x_first, x_next, a_mode, a_how); break;                                   <* 
 *>                                /+> case CHANGE_MERGE   : rc = CELL_merge_visu   (x_first, x_next, a_mode, a_how); break;   <+/   <* 
 *>                                /+> case CHANGE_UNMERGE : rc = CELL_unmerge_visu (x_first, x_next, a_mode, a_how); break;   <+/   <* 
 *>          }                                                                                                                       <* 
 *>          DEBUG_CELL   yLOG_value  ("handle_rc" , rc);                                                                            <* 
 *>          /+---(trouble)---------------------+/                                                                                   <* 
 *>          if (rc < 0) {                                                                                                           <* 
 *>             DEBUG_CELL    yLOG_note   ("detailed function indicated trouble");                                                   <* 
 *>             DEBUG_CELL    yLOG_exitr  (__FUNCTION__, rc);                                                                        <* 
 *>             return rc;                                                                                                           <* 
 *>          }                                                                                                                       <* 
 *>          /+---(early exit)------------------+/                                                                                   <* 
 *>          if (rc > 0)  {                                                                                                          <* 
*>             DEBUG_CELL    yLOG_note   ("detailed function indicated done early");                                                <* 
*>             DEBUG_CELL    yLOG_exit   (__FUNCTION__);                                                                            <* 
*>             return 0;                                                                                                            <* 
*>          }                                                                                                                       <* 
*>          /+---(done)------------------------+/                                                                                   <* 
*>          if (a_mode == YMAP_BEG)  a_mode = YMAP_ADD;                                                                             <* 
*>          api_ycalc_printer (x_next);                                                                                             <* 
*>       }                                                                                                                          <* 
*>       /+---(get next)--------------------+/                                                                                      <* 
*>       rc      = yMAP_visu_next  (&x_tab, &x_col, &x_row, NULL);                                                                  <* 
*>       DEBUG_CELL   yLOG_value  ("next_rc"   , rc);                                                                               <* 
*>       x_next  = LOC_cell_at_loc (x_tab, x_col, x_row);                                                                           <* 
*>       DEBUG_CELL   yLOG_point  ("x_next"    , x_next);                                                                           <* 
*>       /+---(done)------------------------+/                                                                                      <* 
*>    }                                                                                                                             <* 
*>    DEBUG_CELL    yLOG_value  ("x_total"   , x_total);                                                                            <* 
*>    DEBUG_CELL    yLOG_value  ("x_handle"  , x_handle);                                                                           <* 
*>    /+---(complete)-----------------------+/                                                                                      <* 
*>    DEBUG_CELL    yLOG_exit   (__FUNCTION__);                                                                                     <* 
*>    return 0;                                                                                                                     <* 
*> }                                                                                                                                <*/



/*====================------------------------------------====================*/
/*===----                        yparse functions                      ----===*/
/*====================------------------------------------====================*/
static void  o___YPARSE__________o () { return; }

char
CELL_reader          (int c, uchar *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   uchar       x_label     [LEN_LABEL];
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   uchar       x_format    [LEN_LABEL];
   uchar       x_source    [LEN_RECD ];
   tCELL      *x_new       = NULL;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(get verb)-----------------------*/
   DEBUG_INPT   yLOG_info    ("a_verb"    , a_verb);
   --rce;  if (strncmp ("cell", a_verb, 4) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(read all fields)----------------*/
   rc = yPARSE_vscanf (a_verb, x_label, x_format, x_source);
   DEBUG_INPT   yLOG_value   ("scanf"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   DEBUG_INPT   yLOG_info    ("label"     , x_label);
   rc = ystr2gyges (x_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_ADAPT);
   DEBUG_INPT  yLOG_value   ("parse"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_complex ("location"  , "%2dt, %3dc, %4dr", x_tab, x_col, x_row);
   rc = ystr4gyges (x_tab, x_col, x_row, 0, 0, x_label, YSTR_ADAPT);
   DEBUG_INPT  yLOG_value   ("legal"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(format)-------------------------*/
   if      (strcmp (x_format , "")         == 0) strcpy  (x_format, DEF_FORMAT);
   else if (strcmp (x_format , "-")        == 0) strcpy  (x_format, DEF_FORMAT);
   else if (strlen (x_format) <  5)              strcpy  (x_format, DEF_FORMAT);
   DEBUG_INPT  yLOG_info    ("x_format"  , x_format);
   /*---(source)-------------------------*/
   DEBUG_INPT   yLOG_info    ("source"    , x_source);
   ystrldchg (x_source, G_CHAR_STORAGE, G_KEY_SPACE, LEN_RECD);
   DEBUG_INPT   yLOG_info    ("source"    , x_source);
   /*---(update)-------------------------*/
   x_new = CELL_overwrite (YMAP_NONE, x_tab, x_col, x_row, x_source, x_format);
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
CELL_writer        (uchar *a_verb, tCELL *a_curr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   char        x_format    [LEN_LABEL] = "";
   char        x_level     [LEN_LABEL] = "";
   char        t           [LEN_RECD]  = "";
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(defenses)-----------------------*/
   DEBUG_OUTP   yLOG_point   ("a_verb"    , a_verb);
   --rce;  if (a_verb  == NULL) {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_OUTP   yLOG_info    ("a_verb"    , a_verb);
   DEBUG_OUTP   yLOG_point   ("a_curr"    , a_curr);
   --rce;  if (a_curr  == NULL) {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_OUTP   yLOG_point   ("label"     , a_curr->d_label);
   --rce;  if (a_curr->d_label  == NULL) {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_OUTP   yLOG_info    ("label"     , a_curr->d_label);
   DEBUG_OUTP   yLOG_complex ("type"      , "%-3d (%c)", a_curr->d_type, a_curr->d_type);
   if (a_curr->d_type == '-') {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_OUTP   yLOG_point   ("source"    , a_curr->d_source);
   if (a_curr->d_source == NULL) {      
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_OUTP   yLOG_info    ("source"    , a_curr->d_source);
   DEBUG_OUTP   yLOG_value   ("tab"       , a_curr->d_tab);
   --rce;  if (a_curr->d_tab >= 36)  {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(format)-------------------------*/
   DEBUG_OUTP   yLOG_complex ("align"     , "%-3d (%c)", a_curr->d_align , a_curr->d_align);
   DEBUG_OUTP   yLOG_complex ("format"    , "%-3d (%c)", a_curr->d_format, a_curr->d_format);
   DEBUG_OUTP   yLOG_complex ("decs"      , "%-3d (%c)", a_curr->d_decs  , a_curr->d_decs);
   DEBUG_OUTP   yLOG_complex ("unit"      , "%-3d (%c)", a_curr->d_unit  , a_curr->d_unit);
   sprintf (x_format, "%c%c%c%c-", a_curr->d_align, a_curr->d_format, a_curr->d_decs, a_curr->d_unit);
   /*---(call writer)--------------------*/
   ystrlcpy  (t, a_curr->d_source, LEN_RECD);
   ystrldchg (t, G_KEY_SPACE, G_CHAR_STORAGE, LEN_RECD);
   DEBUG_OUTP   yLOG_info    ("to write"  , t);
   rc = yPARSE_vprintf (s_count, a_verb, a_curr->d_label, x_format, t);
   DEBUG_OUTP   yLOG_value   ("vprintf"   , rc);
   /*---(complete)-----------------------*/
   DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
   return 1;
}

char
CELL_writer_seq         (void *a_owner, void *a_deproot, int a_seq, int a_level)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   /*---(write)--------------------------*/
   rc = CELL_writer ("cell_dep", (tCELL *) a_owner);
   if (rc > 0)  ++s_count;
   /*> yPARSE_verb_break (s_count);                                                   <*/
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
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(dependent)----------------------*/
   s_count = 0;
   /*> yPARSE_verb_begin ("cell_dep");                                                <*/
   x_stamp   = rand ();
   rc = yCALC_seq_downup (x_stamp, CELL_writer_seq);
   /*> yPARSE_verb_end   (s_count);                                                   <*/
   c  = s_count;
   /*---(independent)--------------------*/
   s_count = 0;
   /*> yPARSE_verb_begin ("cell");                                                    <*/
   x_curr = hcell;
   while (x_curr != NULL) {
      if (x_curr->d_source != NULL && x_curr->d_type != YCALC_DATA_BLANK && yCALC_getstamp (x_curr->d_ycalc) != x_stamp && x_curr->d_tab >= 0) {
         rc = CELL_writer ("cell_free", x_curr);
         if (rc > 0)  ++s_count;
         /*> yPARSE_verb_break (s_count);                                             <*/
      }
      x_curr = x_curr->d_next;
   }
   /*> yPARSE_verb_end   (s_count);                                                   <*/
   c += s_count;
   if (c > 100)  c = 100;
   /*---(complete)-----------------------*/
   DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
   return c;
}

char
CELL_dump               (FILE *f)
{
   /*---(locals)-----------+-----+-----+-*/
   int         c           =    0;
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   fprintf (f, "#! parsing åÏ---··Ï-------·Ï-·Ï--·Ï---··Ï·Ï·Ï·Ï·Ï··Ï-- Ï-----------------------------------------------------------··æ\n");
   fprintf (f, "#! titles  åseq···label····u· x·· y··· ·t a f d u  len contents······················································æ\n");
   fprintf (f, "\n");
   fprintf (f, "count = %d all, %d linked\n", ACEL, NCEL);
   x_curr = hcell;
   while (x_curr != NULL) {
      if (c %  5 == 0) fprintf (f, "\n");
      if (c % 25 == 0) fprintf (f, "seq-  label--- u- x-- y---  t a f d u  len contents----------------------------------------------------··´\n\n");
      ++c;
      fprintf (f, "%4d  %-8.8s %2d %3d %4d  %c %c %c %c %c  %3d %-60.60s  ´\n", c,
            x_curr->d_label, x_curr->d_tab  , x_curr->d_col  , x_curr->d_row,
            x_curr->d_type , x_curr->d_align, x_curr->d_format, x_curr->d_decs, x_curr->d_unit,
            x_curr->d_len  , x_curr->d_source);
      x_curr = x_curr->d_next;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
CELL_debug_list         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         c           =    0;
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_CELL yLOG_enter   (__FUNCTION__);
   x_curr = hcell;
   while (x_curr != NULL) {
      /*> if (c %  5 == 0) printf ("\n");                                             <*/
      ++c;
      DEBUG_CELL yLOG_complex ("cell"      , "%4d  %-8.8s %2d %3d %4d  %p  %p  %p", c, x_curr->d_label, x_curr->d_tab  , x_curr->d_col  , x_curr->d_row, x_curr->d_prev, x_curr, x_curr->d_next);
      x_curr = x_curr->d_next;
   }
   /*---(complete)-----------------------*/
   DEBUG_CELL yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

/*
 * large (L)
 * 123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-12
 * 2k12      2t  10c   11r  20,100,111  23[this is test input]                       ´
 *                                                                  |
 * medium (M)                                                       |
 * 123456789-123456789-123456789-123456789-123456789-               |
 * 2k12      23[this is test input]                       ´
 *                                         |                        |
 * small (S)                               |                        |
 * 123456789-123456789-123456789-          |                        |
 * 2k12    [this is test input]                       ´
 *
 */

char
CELL_source_line        (tCELL *a_cell, char *a_line)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        t           [LEN_HUND]  = "";
   char        s           [LEN_LABEL] = "";
   /*---(beginning)----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   DEBUG_CELL   yLOG_complex ("args"      , "%-10.10p, %-10.10p", a_cell, a_line);
   /*---(defense)---------------------*/
   --rce;  if (a_line  == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(prepare defaults)------------*/
   snprintf (a_line, LEN_FULL, "- -         -t   -c    -r           -  --åæ");
   --rce;  if (a_cell  == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(prepare full)----------------*/
   if (a_cell->d_source != NULL)  sprintf  (t, "%3då%.38sæ", a_cell->d_len, a_cell->d_source);
   else                         ystrlcpy  (t, " --åæ", LEN_HUND);
   ystrl4comma (a_cell->d_key, s, 0, 'c', '-', LEN_LABEL);
   if (a_cell->d_tab >= 0)  snprintf (a_line, LEN_FULL, "%c %-8.8s %2dt %3dc %4dr %11.11s %s", a_cell->d_linked, a_cell->d_label, a_cell->d_tab, a_cell->d_col, a_cell->d_row, s, t);
   else                     snprintf (a_line, LEN_FULL, "%c %-8.8s  -t   -c    -r           - %s", a_cell->d_linked, a_cell->d_label, t);
   /*---(complete)--------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
CELL_result_line        (tCELL *a_cell, char *a_line)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        t           [LEN_HUND]  = "";
   char        s           [LEN_LABEL] = "";
   /*---(beginning)----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   DEBUG_CELL   yLOG_complex ("args"      , "%-10.10p, %-10.10p", a_cell, a_line);
   /*---(defense)---------------------*/
   --rce;  if (a_line  == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(prepare defaults)------------*/
   snprintf (a_line, LEN_FULL, "´ -        t=·           -.-           --åæ");
   --rce;  if (a_cell  == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(prepare full)----------------*/
   if (a_cell->d_str    != NULL)  sprintf  (t, "%3då%.38sæ", strlen (a_cell->d_str), a_cell->d_str);
   else                           ystrlcpy  (t, " --åæ", LEN_HUND);
   snprintf (a_line, LEN_FULL, "´ %-8.8s t=%c    %17.8lf   %s", a_cell->d_label, a_cell->d_type, a_cell->d_num, t);
   /*---(complete)--------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
CELL_print_line         (tCELL *a_cell, char *a_line)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        t           [LEN_HUND]  = "";
   char        s           [LEN_LABEL] = "";
   /*---(beginning)----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   DEBUG_CELL   yLOG_complex ("args"      , "%-10.10p, %-10.10p", a_cell, a_line);
   /*---(defense)---------------------*/
   --rce;  if (a_line  == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(prepare defaults)------------*/
   snprintf (a_line, LEN_FULL, "´ -        a=· f=· d=· u=· 5=· -h  -w  --åæ");
   --rce;  if (a_cell  == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(prepare full)----------------*/
   if (a_cell->d_Cowner != NULL)  sprintf  (s, "%1dh %2dw", a_cell->d_Rowner->n_size, a_cell->d_Cowner->n_size);
   else                           ystrlcpy  (s, "-h  -w", LEN_LABEL);
   if (a_cell->d_print    != NULL)  sprintf  (t, "%3då%.38sæ", strlen (a_cell->d_print), a_cell->d_print);
   else                           ystrlcpy  (t, " --åæ", LEN_HUND);
   snprintf (a_line, LEN_FULL, "´ %-8.8s a=%c f=%c d=%c u=%c 5=%c %s %s", a_cell->d_label, a_cell->d_align, a_cell->d_format, a_cell->d_decs, a_cell->d_unit, '·', s, t);
   /*---(complete)--------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
CELL_linkage_line       (tCELL *a_cell, char *a_line)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        t           [LEN_HUND]  = "";
   char        s           [LEN_LABEL] = "";
   /*---(beginning)----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   DEBUG_CELL   yLOG_complex ("args"      , "%-10.10p, %-10.10p", a_cell, a_line);
   /*---(defense)---------------------*/
   --rce;  if (a_line  == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(prepare defaults)------------*/
   snprintf (a_line, LEN_FULL, "- -        mp:-      mn:-        -c cp:-      cn:-         -r rp:-      rn:-");
   --rce;  if (a_cell  == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(master)----------------------*/
   if (a_cell->d_prev   == NULL)    ystrlcpy  (s, " mp:-     ", LEN_LABEL);
   else                             sprintf  (s, " mp:%-6.6s", a_cell->d_prev->d_label);
   ystrlcpy (t, s, LEN_HUND);
   if (a_cell->d_next   == NULL)    ystrlcpy  (s, " mn:-     ", LEN_LABEL);
   else                             sprintf  (s, " mn:%-6.6s", a_cell->d_next->d_label);
   ystrlcat (t, s, LEN_HUND);
   /*---(column)----------------------*/
   if (a_cell->d_Cowner == NULL)    ystrlcpy  (s, "   -c", LEN_LABEL);
   else                             sprintf  (s, " %3dc", a_cell->d_Cowner->n_ref);
   ystrlcat (t, s, LEN_HUND);
   if (a_cell->d_Cprev   == NULL)    ystrlcpy  (s, " cp:-     ", LEN_LABEL);
   else                             sprintf  (s, " cp:%-6.6s", a_cell->d_Cprev->d_label);
   ystrlcat (t, s, LEN_HUND);
   if (a_cell->d_Cnext   == NULL)    ystrlcpy  (s, " cn:-     ", LEN_LABEL);
   else                             sprintf  (s, " cn:%-6.6s", a_cell->d_Cnext->d_label);
   ystrlcat (t, s, LEN_HUND);
   /*---(row)-------------------------*/
   if (a_cell->d_Rowner == NULL)    ystrlcpy  (s, "    -r", LEN_LABEL);
   else                             sprintf  (s, " %4dr", a_cell->d_Rowner->n_ref);
   ystrlcat (t, s, LEN_HUND);
   if (a_cell->d_Rprev   == NULL)    ystrlcpy  (s, " rp:-     ", LEN_LABEL);
   else                             sprintf  (s, " rp:%-6.6s", a_cell->d_Rprev->d_label);
   ystrlcat (t, s, LEN_HUND);
   if (a_cell->d_Rnext   == NULL)    ystrlcpy  (s, " rn:-"     , LEN_LABEL);
   else                             sprintf  (s, " rn:%s"    , a_cell->d_Rnext->d_label);
   ystrlcat (t, s, LEN_HUND);
   /*---(concat)----------------------*/
   snprintf (a_line, LEN_FULL, "%c %-8.8s%s", a_cell->d_linked, a_cell->d_label, t);
   /*---(complete)--------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char*        /*-> unit test accessor -----------------[ light  [us.960.251.A0]*/ /*-[02.0000.00#.#]-*/ /*-[--.---.---.--]-*/
CELL__unit_better       (char *a_question, tCELL *a_cell, char *a_label, int a_seq)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   tCELL      *x_cell      = NULL;
   int         n           =    0;
   char        t           [LEN_HUND]  = "";
   short       x_fore      =    0;
   short       x_back      =    0;
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "CELLb            : question not understood");
   /*---(prepare)------------------------*/
   if (a_question [0] == 'p') {
      x_cell = a_cell;
   }
   else if (a_question [0] == 's') {
      x_cell  = hcell;
      while (x_cell != NULL) {
         if (n == a_seq)  break;
         ++n;
         x_cell = x_cell->d_next;
      }
      if (x_cell == NULL)  n = -1;
   } else if (a_question [0] == 'l') {
      api_ysort_by_label (&x_cell, a_label);
   }
   /*---(selection)----------------------*/
   if      (strcmp(a_question, "n_count"       ) == 0) {
      x_cell = hcell; while (x_cell != NULL) { ++x_fore; x_cell = x_cell->d_next; }
      x_cell = tcell; while (x_cell != NULL) { ++x_back; x_cell = x_cell->d_prev; }
      snprintf (unit_answer, LEN_FULL, "CELLb count      : %4da, %4dn, %4df, %4db", ACEL, NCEL, x_fore, x_back);
   }
   else if (strcmp (a_question, "m_sequence")    == 0) {
      /*> if (x_cell == NULL)  snprintf (unit_answer, LEN_FULL, "CELLb sequence   : %10-10p, tab=%4d, col=%4d, row=%4d", x_curr, x_curr->d_tab, x_curr->d_col, x_curr->d_row);    <* 
       *> else                 snprintf (unit_answer, LEN_FULL, "CELLb sequence   : ptr=%10p, tab=%4d, col=%4d, row=%4d", x_curr, -10        , -10        , -10        );   <*/
   }
   else if (strcmp (a_question + 2, "source"       ) == 0) {
      CELL_source_line (x_cell, t);
      snprintf (unit_answer, LEN_FULL, "CELLb source     : %s", t);
   }
   else if (strcmp (a_question + 2, "entry"        ) == 0) {
      CELL_source_line (x_cell, t);
      snprintf (unit_answer, LEN_FULL, "CELLb entry (%2d) : %s", a_seq, t);
   }
   else if (strcmp (a_question + 2, "result"       ) == 0) {
      CELL_result_line (x_cell, t);
      snprintf (unit_answer, LEN_FULL, "CELLb result     : %s", t);
   }
   else if (strcmp (a_question + 2, "printable"    ) == 0) {
      CELL_print_line  (x_cell, t);
      snprintf (unit_answer, LEN_FULL, "CELLb printable  : %s", t);
   }
   else if (strcmp (a_question + 2, "linkage"      ) == 0) {
      CELL_linkage_line (x_cell, t);
      snprintf (unit_answer, LEN_FULL, "CELLb linkage    : %s", t);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}

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
      x_curr = x_curr->d_next;
   }
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "cell_where")    == 0) {
      if (x_found == 'y')  snprintf (unit_answer, LEN_FULL, "s_cell location  : ptr=%10p, tab=%4d, col=%4d, row=%4d", x_curr, x_curr->d_tab, x_curr->d_col, x_curr->d_row);
      else                 snprintf (unit_answer, LEN_FULL, "s_cell location  : ptr=%10p, tab=%4d, col=%4d, row=%4d", x_curr, -10        , -10        , -10        );
   }
   else if (strcmp (a_question, "where")         == 0) {
      if (x_found == 'y')  snprintf (unit_answer, LEN_FULL, "s_cell where     : %-8.8s  %3dt %3dc %3dr  %d", x_curr->d_label, x_curr->d_tab, x_curr->d_col, x_curr->d_row, x_curr->d_key);
      else                 snprintf (unit_answer, LEN_FULL, "s_cell where     : -           -t   -c   -r  -");
   }
   else if (strcmp(a_question, "cell_list")      == 0) {
      snprintf(unit_answer, LEN_FULL, "s_cell main list : num=%4d, head=%10p, tail=%10p", NCEL, hcell, tcell);
   }
   else if (strcmp(a_question, "cell_count")     == 0) {
      x_curr = hcell; while (x_curr != NULL) { ++x_fore; x_curr = x_curr->d_next; }
      x_curr = tcell; while (x_curr != NULL) { ++x_back; x_curr = x_curr->d_prev; }
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
   char        t           [LEN_HUND]  = "···åæ";
   char        s           [LEN_HUND]  = "···åæ";
   char        x_label     [LEN_LABEL];
   int         x_fore      = 0;
   int         x_back      = 0;
   tDEP       *x_dep       = NULL;
   tCELL      *x_cell      = NULL;
   char        rc          = 0;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   int         l           = 0;
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_celln          : question not understood");
   /*---(identify the cell pointer)------*/
   if (a_label == NULL || strcmp ("root", a_label) == 0) {
      /*> x_cell = s_root;                                                            <*/
      sprintf (unit_answer, "s_celln error    : can not call on dependency s_root");
      return unit_answer;
   } else {
      rc     = ystr2gyges (a_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_CHECK);
      if (rc < 0) {
         sprintf (unit_answer, "s_celln error    : label <%s> not legal", a_label);
         return unit_answer;
      }
      rc = ystr4gyges (x_tab, x_col, x_row, 0, 0, x_label, YSTR_USABLE);
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
      else                             snprintf(unit_answer, LEN_FULL, "s_celln info     : t=%c a=%c f=%c d=%c u=%c 5=%c w=%3d h=%3d", x_cell->d_type, x_cell->d_align, x_cell->d_format, x_cell->d_decs, x_cell->d_unit, '-', COL_size (x_cell->d_tab, x_cell->d_col), ROW_size (x_cell->d_tab, x_cell->d_row));
   }
   else if   (strcmp(a_question, "source"     )    == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln source   : (----) ::");
      else if (x_cell->d_source     == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln source   : (null) ::");
      else                             snprintf(unit_answer, LEN_FULL, "s_celln source   : (%4d) :%-.40s:", x_cell->d_len, x_cell->d_source);
   }
   else if (strcmp(a_question, "value"     )     == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln value    :         ---.------");
      else                             snprintf(unit_answer, LEN_FULL, "s_celln value    : %18.6F", x_cell->d_num);
   }
   else if (strcmp(a_question, "modified")  == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln modded   : (----) ::");
      else if (x_cell->d_str == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln modded   : (null) ::");
      else                             snprintf(unit_answer, LEN_FULL, "s_celln modded   : (%4d) :%-.40s:", (int) strlen(x_cell->d_str), x_cell->d_str);
   }
   else if (strcmp(a_question, "printable") == 0) {
      /*> snprintf(unit_answer, LEN_FULL, "Cell Printable   : (%4d) :%-.40s:", (int) strlen(x_cell->d_print), x_cell->d_print);   <*/
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln print    : (----) ::");
      else if (x_cell->d_print     == NULL)  snprintf(unit_answer, LEN_FULL, "s_celln print    : (null) ::");
      else {
         len = strlen (x_cell->d_print);
         if      (len        == 0   )  snprintf(unit_answer, LEN_FULL, "s_celln print    : (null) ::");
         else if (len        >= 35  )  snprintf(unit_answer, LEN_FULL, "s_celln print    : (%4d) :%-34.34s++", (int) strlen(x_cell->d_print), x_cell->d_print);
         else                          snprintf(unit_answer, LEN_FULL, "s_celln print    : (%4d) :%s:"       , (int) strlen(x_cell->d_print), x_cell->d_print);
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
   else if (strcmp(a_question, "quick"    )      == 0) {
      if (x_cell == NULL) {
         snprintf(unit_answer, LEN_FULL, "s_celln %-8.8s : ······· %3d ···åæ                                                   ···åæ", a_label, COL_size (x_tab, x_col));
      } else {
         if (x_cell->d_source != NULL)  sprintf (t, "%3då%.50sæ", strlen (x_cell->d_source), x_cell->d_source);
         if (x_cell->d_print  != NULL)  sprintf (s, "%3då%.30sæ", strlen (x_cell->d_print) , x_cell->d_print);
         snprintf(unit_answer, LEN_FULL, "s_celln %-8.8s : %c%c%c%c%c%c%c%c %3d %-55.55s %s", a_label, x_cell->d_type, x_cell->d_align, x_cell->d_format, x_cell->d_decs, x_cell->d_unit, x_cell->d_fill, x_cell->d_zero, x_cell->d_note, COL_size (x_cell->d_tab, x_cell->d_col), t, s);
      }
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
   if (a_cell->d_source != NULL)   free (a_cell->d_source);
   a_cell->d_source = strndup (a_source, LEN_RECD);
   a_cell->d_len    = strlen  (a_cell->d_source);
   /*---(complete)-----------------------*/
   return 0;
}



/*============================----end-of-source---============================*/
