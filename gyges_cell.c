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
char        g_dashes      [200] = "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";


static char sv_formats     [50] = "";
static char sv_nums        [20] = "?if";
static char sv_commas      [20] = "cCaAsS$#p";
static char sv_sci         [20] = "eE";
static char sv_roman       [20] = "rR";
static char sv_bases       [20] = "bBoOxXzZ";
static char sv_times       [20] = "tTdD";
static char sv_fillers     [20] = " -=_+./\"";

static char sv_align       [20] = "?<|>[^]{}";


static char sv_numeric     [20] = "0123456789.-+";    /* only digits               */
static char sv_hex         [20] = "0123456789abcdef"; /* only hex digits           */
static char sv_octal       [20] = "01234567";         /* only octal digits           */
static char sv_binary      [20] = "01";               /* only binary digits        */



/*> #define   MAX_CELLTYPE     30                                                                                      <* 
 *> struct cCELL_INFO {                                                                                                <* 
 *>    char        type;                   /+ cell type                           +/                                   <* 
 *>    char        terse       [10];       /+ short description of cell type      +/                                   <* 
 *>    char        prefix;                 /+ prefix in cell source string        +/                                   <* 
 *>    char        rpn;                    /+ require processing by rpn           +/                                   <* 
 *>    char        calc;                   /+ must it be calculated               +/                                   <* 
 *>    char        deps;                   /+ must follow the dependencies        +/                                   <* 
 *>    char        result;                 /+ what type is the result             +/                                   <* 
 *>    char        desc        [50];       /+ description of cell type            +/                                   <* 
 *>    int         count;                  /+ current count of type               +/                                   <* 
 *> } s_cell_info [MAX_CELLTYPE] = {                                                                                   <* 
 *>    /+-ty- -terse------- -pre -rpn calc -dep -res ---description---------------------------------------- -cnt- +/   <* 
 *>    {  's', "string"    , ' ', '-', '-', '-', '#', "string literal presented from source field"         ,    0 },   <* 
 *>    {  'n', "number"    , ' ', '-', '-', '-', '=', "numeric literal presented in various formats"       ,    0 },   <* 
 *>    {  'f', "formula"   , '=', 'y', 'y', 'y', '=', "numeric formula"                                    ,    0 },   <* 
 *>    {  'm', "mod_str"   , '#', 'y', 'y', 'y', '#', "string formula"                                     ,    0 },   <* 
 *>    {  'l', "flike"     , '~', 'y', 'y', 'y', '=', "numeric formula derived from another cell"          ,    0 },   <* 
 *>    {  'L', "mlike"     , '~', 'y', 'y', 'y', '#', "string formula derived from another cell"           ,    0 },   <* 
 *>    {  'p', "range"     , '&', 'y', 'y', 'y', '-', "range pointer to use in other formulas"             ,    0 },   <* 
 *>    {  'a', "address"   , '&', 'y', 'y', 'y', '-', "address pointer to use in other formulas"           ,    0 },   <* 
 *>    {  '>', "source"    , ' ', '-', '-', 'y', '-', "content used to fill merged cells"                  ,    0 },   <* 
 *>    {  '+', "merged"    , '<', '-', '-', 'y', '-', "empty cell used to present merged information"      ,    0 },   <* 
 *>    {  'w', "warning"   , ' ', '-', '-', '-', 'e', "cell contains a warning"                            ,    0 },   <* 
 *>    {  'E', "error"     , ' ', 'y', 'y', 'y', 'e', "cell contains an error"                             ,    0 },   <* 
 *>    {  '-', "blank"     , ' ', '-', '-', '-', '-', "blank cell"                                         ,    0 },   <* 
 *> };                                                                                                                 <*/


/*> char    G_CELL_ALL    [20] = "";                                                  <* 
 *> char    G_CELL_RPN    [20] = "";                                                  <* 
 *> char    G_CELL_CALC   [20] = "";                                                  <* 
 *> char    G_CELL_DEPS   [20] = "";                                                  <* 
 *> char    G_CELL_NUM    [20] = "";                                                  <* 
 *> char    G_CELL_STR    [20] = "";                                                  <* 
 *> char    G_CELL_ERR    [20] = "";                                                  <* 
 *> char    G_CELL_FPRE   [20] = "";                                                  <*/



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___PROGRAM_________o () { return; }

char         /*-> tbd --------------------------------[ shoot  [ge.G94.071.80]*/ /*-[04.0000.102.!]-*/ /*-[--.---.---.--]-*/
CELL_init          (void)
{
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         i           = 0;
   int         x_count     = 0;
   int         x_found     = 0;
   int         x_reqs      = 0;
   int         x_pros      = 0;
   int         t           [5];
   /*---(cells)--------------------------*/
   ACEL        = 0;
   my.root     = NULL;
   hcell       = NULL;
   tcell       = NULL;
   NCEL        = 0;
   /*---(prepare validatations)----------*/
   DEBUG_CELL   yLOG_note    ("concatinate format types");
   strcat (sv_formats, sv_nums   );
   strcat (sv_formats, sv_commas );
   strcat (sv_formats, sv_sci    );
   strcat (sv_formats, sv_roman  );
   strcat (sv_formats, sv_bases  );
   strcat (sv_formats, sv_times  );
   strcat (sv_formats, sv_fillers);
   DEBUG_CELL   yLOG_info    ("sv_formats", sv_formats);
   DEBUG_CELL   yLOG_note    ("clear validation types");
   /*> strlcpy (G_CELL_ALL , "", 20);                                                 <* 
    *> strlcpy (G_CELL_RPN , "", 20);                                                 <* 
    *> strlcpy (G_CELL_CALC, "", 20);                                                 <* 
    *> strlcpy (G_CELL_DEPS, "", 20);                                                 <* 
    *> strlcpy (G_CELL_NUM , "", 20);                                                 <* 
    *> strlcpy (G_CELL_STR , "", 20);                                                 <* 
    *> strlcpy (G_CELL_ERR , "", 20);                                                 <* 
    *> strlcpy (G_CELL_FPRE, "", 20);                                                 <*/
   /*---(complete info table)------------*/
   /*> DEBUG_CELL   yLOG_note    ("build cell validation types");                     <*/
   /*> --rce;                                                                         <* 
    *> for (i = 0; i < MAX_CELLTYPE; ++i) {                                           <* 
    *>    DEBUG_CELL_M yLOG_value   ("ENTRY"     , i);                                <* 
    *>    DEBUG_CELL_M yLOG_char    ("type"      , s_cell_info [i].type);             <* 
    *>    /+---(check for end)---------------+/                                       <* 
    *>    if (s_cell_info [i].type == CTYPE_BLANK)  break;                            <* 
    *>    /+---(add to lists)----------------+/                                       <* 
    *>    sprintf (t, "%c", s_cell_info [i].type);                                    <* 
    *>    DEBUG_CELL_M yLOG_info    ("str type"  , t);                                <* 
    *>    DEBUG_CELL_M yLOG_char    ("rpn flag"  , s_cell_info [i].rpn);              <* 
    *>    strcat (G_CELL_ALL , t);                                                    <* 
    *>    if (s_cell_info [i].calc    == 'y')  strcat (G_CELL_CALC, t);               <* 
    *>    if (s_cell_info [i].deps    == 'y')  strcat (G_CELL_DEPS, t);               <* 
    *>    if (s_cell_info [i].result  == '=')  strcat (G_CELL_NUM , t);               <* 
    *>    if (s_cell_info [i].result  == '#')  strcat (G_CELL_STR , t);               <* 
    *>    if (s_cell_info [i].result  == 'e')  strcat (G_CELL_ERR , t);               <* 
    *>    if (s_cell_info [i].rpn     == 'y') {                                       <* 
    *>       strcat  (G_CELL_RPN , t);                                                <* 
    *>       sprintf (t, "%c", s_cell_info [i].prefix);                               <* 
    *>       if   (s_cell_info [i].prefix != ' ' &&                                   <* 
    *>             strchr (G_CELL_FPRE, s_cell_info [i].prefix) == 0) {               <* 
    *>          strcat  (G_CELL_FPRE , t);                                            <* 
    *>       }                                                                        <* 
    *>    }                                                                           <* 
    *>    ++x_count;                                                                  <* 
    *> }                                                                              <*/
   /*---(report out)---------------------*/
   /*> DEBUG_CELL   yLOG_value   ("x_count"   , x_count);                             <* 
    *> DEBUG_CELL   yLOG_info    ("G_CELL_ALL" , G_CELL_ALL );                        <* 
    *> DEBUG_CELL   yLOG_info    ("G_CELL_RPN" , G_CELL_RPN );                        <* 
    *> DEBUG_CELL   yLOG_info    ("G_CELL_CALC", G_CELL_CALC);                        <* 
    *> DEBUG_CELL   yLOG_info    ("G_CELL_DEPS", G_CELL_DEPS);                        <* 
    *> DEBUG_CELL   yLOG_info    ("G_CELL_NUM" , G_CELL_NUM );                        <* 
    *> DEBUG_CELL   yLOG_info    ("G_CELL_STR" , G_CELL_STR );                        <* 
    *> DEBUG_CELL   yLOG_info    ("G_CELL_ERR" , G_CELL_ERR );                        <* 
    *> DEBUG_CELL   yLOG_info    ("G_CELL_FPRE", G_CELL_FPRE);                        <*/
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [fz.842.041.24]*/ /*-[01.0000.013.T]-*/ /*-[--.---.---.--]-*/
CELL__purge        (void)
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

char         /*-> tbd --------------------------------[ shoot  [gz.321.001.01]*/ /*-[00.0000.102.!]-*/ /*-[--.---.---.--]-*/
CELL_wrap          (void)
{
   /*---(header)-------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(cells)--------------------------*/
   CELL__purge ();
   hcell  = NULL;
   tcell  = NULL;
   NCEL   = 0;
   /*---(root)---------------------------*/
   CELL__free    (&my.root, UNLINKED);
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
   (*a_cell)->s         = NULL;
   (*a_cell)->l         = 0;
   /*---(results)-----------------------*/
   (*a_cell)->t         = YCALC_DATA_BLANK;
   (*a_cell)->v_num     =  0.0;
   (*a_cell)->v_str     = NULL;
   /*---(formatting)--------------------*/
   (*a_cell)->a         =  '?';
   (*a_cell)->d         =  '0';
   (*a_cell)->f         =  '?';
   (*a_cell)->n         =  '-';
   (*a_cell)->p         = NULL;
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
   /*---(defenses)-----------------------*/
   if (a_curr            == NULL)  return 0;   /* no harm, no foul, nothing to do    */
   /*---(beginning)----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(source)-------------------------*/
   DEBUG_CELL   yLOG_note    ("clear source string");
   if (a_curr->s         != NULL)  free (a_curr->s);
   a_curr->s              = NULL;
   a_curr->l              = 0;
   /*---(results)------------------------*/
   DEBUG_CELL   yLOG_note    ("clear results, string and value");
   a_curr->t              = YCALC_DATA_BLANK;
   if (a_curr->v_str     != NULL)  free (a_curr->v_str);
   a_curr->v_str          = NULL;
   a_curr->v_num          = 0.0;
   /*---(printable)----------------------*/
   DEBUG_CELL   yLOG_note    ("wipe printable");
   if (a_curr->p         != NULL)  free (a_curr->p);
   a_curr->p              = NULL;
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
   /*---(defenses)-----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   rc = LOC_legal (a_col, a_row, a_tab, CELL_GROW);
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
   char        x_before    [LEN_RECD]   = "[<{(null)}>]";
   char        x_after     [LEN_RECD]   = "[<{(null)}>]";
   /*---(defenses)-----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   DEBUG_CELL   yLOG_value   ("a_tab"     , a_tab);
   DEBUG_CELL   yLOG_value   ("a_col"     , a_col);
   DEBUG_CELL   yLOG_value   ("a_row"     , a_row);
   rc          = LOC_legal (a_col, a_row, a_tab, CELL_FIXED);
   DEBUG_CELL   yLOG_value   ("LOC_legal" , rc);
   --rce;  if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_curr      = LOC_cell_at_loc (a_col, a_row, a_tab);
   DEBUG_CELL   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save before)-----------------*/
   if (x_curr->s != NULL)  strcpy (x_before, x_curr->s);
   /*---(history)------------------------*/
   if (a_mode == CHG_INPUT   )  HIST_change ("delete", a_tab, a_col, a_row, x_before, x_after);
   if (a_mode == CHG_INPUTAND)  HIST_change ("DELETE", a_tab, a_col, a_row, x_before, x_after);
   /*---(clear it out)----------------*/
   /*> DEBUG_CELL   yLOG_complex ("details"   , "ptr=%p, tab=%4d, col=%4d, row=%4d, t=%c, u=%d", x_curr, x_curr->tab, x_curr->col, x_curr->row, x_curr->t, x_curr->u);   <*/
   rc = CELL__wipe   (x_curr);
   DEBUG_CELL   yLOG_value   ("wipe rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(see if its still there)------*/
   x_curr        = LOC_cell_at_loc (a_col, a_row, a_tab);
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
   if (a_old->s == NULL)    (*a_new)->s = NULL;
   else                     (*a_new)->s = strndup (a_old->s, LEN_RECD);
   (*a_new)->l        = a_old->l;
   /*---(copy formatting)----------------*/
   DEBUG_CELL   yLOG_note    ("copy old type and formatting");
   (*a_new)->t        = a_old->t;
   (*a_new)->f        = a_old->f;
   (*a_new)->d        = a_old->d;
   (*a_new)->a        = a_old->a;
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return  0;
}

char      xlabel    [200] = "";
char      s_bsource [200] = "";
char      s_bformat [200] = "";


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
   rc = LOC_legal (a_col, a_row, a_tab, CELL_FIXED);
   DEBUG_CELL   yLOG_info    ("legal"     , (rc >= 0) ? "yes" : "no" );
   --rce;  if (rc <  0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(cell present)-------------------*/
   x_curr      = LOC_cell_at_loc (a_col, a_row, a_tab);
   DEBUG_CELL   yLOG_point   ("x_curr"    , x_curr);
   /*---(save before picture)------------*/
   strcpy (s_bsource, "[<{(null)}>]");
   strcpy (s_bformat, "???");
   --rce;  if (x_curr != NULL) {
      DEBUG_CELL   yLOG_note    ("save existing data");
      if (x_curr->s != NULL)  strcpy (s_bsource, x_curr->s);
      sprintf (s_bformat, "%c%c%c", x_curr->f, x_curr->a, x_curr->d);
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
   if (a_mode == CHG_INPUT   )  HIST_change ("change", a_tab, a_col, a_row, s_bsource, a_source);
   if (a_mode == CHG_INPUTAND)  HIST_change ("CHANGE", a_tab, a_col, a_row, s_bsource, a_source);
   /*---(update)-------------------------*/
   DEBUG_CELL   yLOG_note    ("change source and length values");
   x_curr->s = strndup (a_source, LEN_RECD);
   x_curr->l = strlen  (x_curr->s);
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
   /*---(defense)------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   if (strlen (a_format) != 3) {
      DEBUG_CELL   yLOG_warn    ("format"    , "not valid length");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   /*---(start)--------------------------*/
   CELL_change (&x_new, a_mode, a_tab, a_col, a_row, a_source);
   if (x_new == NULL)  {
      DEBUG_CELL   yLOG_warn    ("x_new"     , "change returned a null pointer");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   /*---(history)------------------------*/
   DEBUG_CELL   yLOG_note    ("write history");
   if (a_mode == CHG_OVER   )  HIST_overwrite ("overwrite", a_tab, a_col, a_row, s_bsource, s_bformat, a_source, a_format);
   if (a_mode == CHG_OVERAND)  HIST_overwrite ("OVERWRITE", a_tab, a_col, a_row, s_bsource, s_bformat, a_source, a_format);
   /*---(formatting)---------------------*/
   DEBUG_CELL   yLOG_note    ("update format fields");
   if (a_format [0] != '?')  x_new->f = a_format[0];
   DEBUG_CELL   yLOG_char    ("f"         , x_new->f);
   if (a_format [1] != '?')  x_new->a = a_format[1];
   DEBUG_CELL   yLOG_char    ("a"         , x_new->a);
   if (a_format [2] != '0')  x_new->d = a_format[2];
   DEBUG_CELL   yLOG_char    ("d"         , x_new->d);
   /*---(update)-------------------------*/
   DEBUG_CELL   yLOG_note    ("call printable");
   /*> rc = CELL_printable (x_new);                                                   <*/
   rc = api_ycalc_printer (x_new);
   if (rc < 0) {
      DEBUG_CELL   yLOG_warn    ("printable" , "returned a bad return code");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   DEBUG_CELL   yLOG_note    ("call calc");
   /*> rc = SEQ_calc_up    (x_new);                                                   <* 
    *> if (rc < 0) {                                                                  <* 
    *>    DEBUG_CELL   yLOG_warn    ("calc"      , "returned a bad return code");     <* 
    *>    DEBUG_CELL   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return NULL;                                                                <* 
    *> }                                                                              <*/
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
   if (a_head == a_curr) CELL_change (NULL, CHG_INPUT   , a_curr->tab, a_curr->col, a_curr->row, "");
   else                  CELL_change (NULL, CHG_INPUTAND, a_curr->tab, a_curr->col, a_curr->row, "");
   return 0;
}

char         /*-> validate number formatting ---------[ leaf   [gc.210.102.10]*/ /*-[00.0000.104.!]-*/ /*-[--.---.---.--]-*/
CELL_format_valid    (char a_format)
{
   if (strchr (sv_formats, a_format) == 0)   return '?';
   return a_format;
}

char         /*-> change cell number formatting ------[ ------ [gc.940.252.A4]*/ /*-[04.0000.303.#]-*/ /*-[--.---.---.--]-*/
CELL_format        (tCELL *a_head, tCELL *a_curr, char a_mode, char a_format)
{
   /*---(design notes)-------------------*/
   /*
    *  update all cells to new format regardless of type to keep complexity
    *  down and flexibility up.  if there is no cell, simply move on.
    */
   /*---(defenses)-----------------------*/
   if (a_head == NULL || a_curr == NULL)    return  0;
   if (a_curr->a == '+')                    return  0;
   if (strchr (sv_formats, a_format)  == 0) return -1;
   /*---(prepare)------------------------*/
   if (a_mode == CHG_INPUT) {
      if (a_head == a_curr) HIST_format ("format", a_curr->tab, a_curr->col, a_curr->row, a_curr->f, a_format);
      else                  HIST_format ("FORMAT", a_curr->tab, a_curr->col, a_curr->row, a_curr->f, a_format);
   }
   if      (strchr (YCALC_GROUP_STR , a_curr->t) != 0 && strchr (sv_fillers, a_format) != NULL)
      a_curr->f = a_format;
   else if (strchr (YCALC_GROUP_NUM , a_curr->t) != 0 && strchr (sv_fillers, a_format) == NULL)
      a_curr->f = a_format;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> validate horizontal alignment ------[ leaf   [gc.210.102.10]*/ /*-[00.0000.104.!]-*/ /*-[--.---.---.--]-*/
CELL_align_valid   (char a_align)
{
   if (strchr (sv_align, a_align) == 0)   return '?';
   return a_align;
}

char         /*-> change cell horizontal alignment ---[ ------ [gc.940.262.94]*/ /*-[04.0000.403.Y]-*/ /*-[--.---.---.--]-*/
CELL_align         (tCELL *a_head, tCELL *a_curr, char a_mode, char a_align)
{
   /*---(defense: bad alignment code)----*/
   if (strchr (sv_align, a_align) == 0) return -1;
   /*---(prepare)------------------------*/
   if (a_curr->a == '+')  return 0;
   if (a_align   == '"')  a_align = a_curr->a;
   /*---(process all cells in range)-----*/
   /*> if ((a_align == '}' || a_align == '{') &&                                <* 
    *>       (a_curr->t != 'n' && a_curr->t != 'f')) a_align = a_curr->a;       <*/
   if (a_mode == CHG_INPUT) {
      if (a_head == a_curr) HIST_format ("align", a_curr->tab, a_curr->col, a_curr->row, a_curr->a, a_align);
      else                  HIST_format ("ALIGN", a_curr->tab, a_curr->col, a_curr->row, a_curr->a, a_align);
   }
   a_curr->a = a_align;
   /*---(complete)---------------------------*/
   return 0;
}

char         /*-> validate decimal places ------------[ leaf   [gc.210.102.10]*/ /*-[00.0000.104.!]-*/ /*-[--.---.---.--]-*/
CELL_decimals_valid  (char a_decs)
{
   if (strchr ("0123456789\"", a_decs) == 0)   return '0';
   return a_decs;
}

char         /*-> change cell decimal places ---------[ ------ [gc.950.272.94]*/ /*-[03.0000.303.S]-*/ /*-[--.---.---.--]-*/
CELL_decimals      (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num)
{
   /*---(locals)-----------+-----------+-*/
   char        x_decs      = 0;
   /*---(handle input)-----------------------*/
   if (a_num  == '"')  x_decs = a_curr->d;
   else                x_decs = a_num;
   /*---(fix ranges)-------------------------*/
   if      (x_decs <  '0')  x_decs = '0';
   else if (x_decs >  '9')  x_decs = '9';
   /*---(update)-----------------------*/
   if (a_mode == CHG_INPUT) {
      if (a_head == a_curr)  HIST_format ("decimals", a_curr->tab, a_curr->col, a_curr->row, a_curr->d, x_decs);
      else                   HIST_format ("DECIMALS", a_curr->tab, a_curr->col, a_curr->row, a_curr->d, x_decs);
   }
   a_curr->d = x_decs;
   /*---(complete)---------------------------*/
   return 0;
}

char         /*-> change cell column width -----------[ ------ [gc.E91.292.69]*/ /*-[02.0000.303.Y]-*/ /*-[--.---.---.--]-*/
CELL_width         (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num)
{  /*---(design notes)-------------------*/
   /*  update all cells to new width, either a standard size, or a specific   */
   /*  value communicated as a negative number.                               */
   /*---(locals)-----------+-----------+-*/
   int         x_width     = 0;
   int         x_row       = 0;
   int         x_last      = 0;
   tCELL      *x_cell      = NULL;
   /*---(stop early)-------------------------*/
   if (a_head->row != a_curr->row)  return 1;
   /*---(process range)----------------------*/
   x_last = ROW_max (a_curr->tab);
   /*---(adjust)----------------------*/
   if (a_num <   0) {
      x_width                = -(a_num);
   } else {
      x_width = COL_width (a_curr->tab, a_curr->col);
      switch (a_num) {
      case  'm' : x_width    = 0;                           break;
      case  'n' : x_width    = 8;                           break;
      case  'N' : x_width    = 12;                          break;
      case  'w' : x_width    = 20;                          break;
      case  'W' : x_width    = 50;                          break;
      case  'h' : x_width   -= 1;                           break;
      case  'l' : x_width   += 1;                           break;
      case  'H' : x_width    = ((x_width / 5) * 5);         break;
      case  'L' : x_width    = (((x_width / 5) + 1) * 5);   break;
      }
   }
   /*---(set width)--------------------*/
   COL_widen  (a_curr->tab, a_curr->col, x_width);
   /*---(update column printables)----*/
   for (x_row = 0; x_row < x_last; ++x_row) {
      x_cell = LOC_cell_at_loc (a_curr->col, x_row, a_curr->tab);
      if (x_cell == NULL) continue;
      /*---(update merged cells)----------*/
      if (x_cell->t == YCALC_DATA_MERGED)  yCALC_calc_from (x_cell->ycalc);
      /*---(update printable)-------------*/
      /*> CELL_printable (x_cell);                                                    <*/
      api_ycalc_printer (x_cell);
   }
   /*---(reset headers)---------------*/
   yVIKEYS_map_refresh ();
   /*> KEYS_bcol    (BCOL);                                                           <*/
   /*> CURS_col_head ();                                                              <*/
   /*---(complete)---------------------------*/
   DEBUG_CELL  yLOG_exit   (__FUNCTION__);
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
   /*---(header)-------------------------*/
   DEBUG_CELL    yLOG_enter  (__FUNCTION__);
   DEBUG_CELL    yLOG_char   ("a_what"    , a_what);
   DEBUG_CELL    yLOG_char   ("a_mode"    , a_mode);
   DEBUG_CELL    yLOG_char   ("a_how"     , a_how);
   /*---(get first)----------------------*/
   rc       = yVIKEYS_first (&x_col, &x_row, &x_tab);
   DEBUG_CELL    yLOG_value  ("rc"        , rc);
   x_first  = LOC_cell_at_loc (x_col, x_row, x_tab);
   DEBUG_CELL    yLOG_point  ("x_first"   , x_first);
   x_next   = x_first;
   /*---(process range)------------------*/
   while (rc >= 0) {
      ++x_total;
      if (x_next != NULL) {
         ++x_handle;
         DEBUG_CELL   yLOG_note   ("handlers");
         switch (a_what) {
         case CHANGE_WIDTH   : rc = CELL_width        (x_first, x_next, a_mode, a_how); break;
         case CHANGE_DECIMAL : rc = CELL_decimals     (x_first, x_next, a_mode, a_how); break;
         case CHANGE_ALIGN   : rc = CELL_align        (x_first, x_next, a_mode, a_how); break;
         case CHANGE_FORMAT  : rc = CELL_format       (x_first, x_next, a_mode, a_how); break;
         case CHANGE_ERASE   : rc = CELL_erase        (x_first, x_next, a_mode, a_how); break;
                               /*> case CHANGE_MERGE   : rc = CELL_merge_visu   (x_first, x_next, a_mode, a_how); break;   <*/
                               /*> case CHANGE_UNMERGE : rc = CELL_unmerge_visu (x_first, x_next, a_mode, a_how); break;   <*/
         }
         DEBUG_CELL   yLOG_value  ("rc"        , rc);
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
         api_ycalc_printer (x_next);
      }
      /*---(get next)--------------------*/
      rc      = yVIKEYS_next  (&x_col, &x_row, &x_tab);
      DEBUG_CELL   yLOG_value  ("rc"        , rc);
      x_next  = LOC_cell_at_loc (x_col, x_row, x_tab);
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
/*===----                        final display                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___DISPLAY_________o () { return; }

char        s_print     [LEN_RECD] = "";



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
      if (x_found == 'y')  snprintf (unit_answer, LEN_UNIT, "s_cell location  : ptr=%10p, tab=%4d, col=%4d, row=%4d", x_curr, x_curr->tab, x_curr->col, x_curr->row);
      else                 snprintf (unit_answer, LEN_UNIT, "s_cell location  : ptr=%10p, tab=%4d, col=%4d, row=%4d", x_curr, -10        , -10        , -10        );
   }
   else if (strcmp(a_question, "cell_list")      == 0) {
      snprintf(unit_answer, LEN_UNIT, "s_cell main list : num=%4d, head=%10p, tail=%10p", NCEL, hcell, tcell);
   }
   else if (strcmp(a_question, "cell_count")     == 0) {
      x_curr = hcell; while (x_curr != NULL) { ++x_fore; x_curr = x_curr->next; }
      x_curr = tcell; while (x_curr != NULL) { ++x_back; x_curr = x_curr->prev; }
      snprintf(unit_answer, LEN_UNIT, "s_cell count     : all=%4d, num=%4d, fore=%4d, back=%4d", ACEL, NCEL, x_fore, x_back);
   }
   /*---(printing)-----------------------*/
   else if (strcmp(a_question, "cell_print")     == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_cell print     : <<%s>>", s_print);
   }
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
      rc     = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);
      if (rc < 0) {
         sprintf (unit_answer, "s_celln error    : label <%s> not legal", a_label);
         return unit_answer;
      }
      rc     = LOC_legal (x_col, x_row, x_tab, CELL_FIXED);
      if (rc < 0) {
         sprintf (unit_answer, "s_celln error    : label <%s> not in-range", a_label);
         return unit_answer;
      }
      x_cell = LOC_cell_at_loc  (x_col, x_row, x_tab);
      /*> if (x_cell == NULL) {                                                         <* 
       *>    sprintf (unit_answer, "s_celln          : label <%s> is NULL", a_label);   <* 
       *>    return unit_answer;                                                        <* 
       *> }                                                                             <*/
   }
   /*---(cell contents)------------------*/
   if (strcmp(a_question, "cell_info")      == 0) {
      /*> if      (x_cell        == NULL)  snprintf(unit_answer, LEN_UNIT, "s_celln info     : --- --- --- --- ----- ----- -----");                                                                                                     <* 
       *> else                             snprintf(unit_answer, LEN_UNIT, "s_celln info     : t=%c f=%c d=%c a=%c c=%3d r=%3d d=%3d", x_cell->t, x_cell->f, x_cell->d, x_cell->a, x_cell->nrpn, x_cell->nrequire, x_cell->nprovide);   <*/
   }
   else if   (strcmp(a_question, "cell_source")    == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_UNIT, "s_celln source   : (----) ::");
      else if (x_cell->s     == NULL)  snprintf(unit_answer, LEN_UNIT, "s_celln source   : (null) ::");
      else                             snprintf(unit_answer, LEN_UNIT, "s_celln source   : (%4d) :%-.40s:", x_cell->l, x_cell->s);
   }
   else if (strcmp(a_question, "cell_value")     == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_UNIT, "s_celln value    :         ---.------");
      else                             snprintf(unit_answer, LEN_UNIT, "s_celln value    : %18.6F", x_cell->v_num);
   }
   else if (strcmp(a_question, "cell_modified")  == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_UNIT, "s_celln modded   : (----) ::");
      else if (x_cell->v_str == NULL)  snprintf(unit_answer, LEN_UNIT, "s_celln modded   : (null) ::");
      else                             snprintf(unit_answer, LEN_UNIT, "s_celln modded   : (%4d) :%-.40s:", (int) strlen(x_cell->v_str), x_cell->v_str);
   }
   else if (strcmp(a_question, "cell_printable") == 0) {
      /*> snprintf(unit_answer, LEN_UNIT, "Cell Printable   : (%4d) :%-.40s:", (int) strlen(x_cell->p), x_cell->p);   <*/
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_UNIT, "s_celln print    : (----) ::");
      else if (x_cell->p     == NULL)  snprintf(unit_answer, LEN_UNIT, "s_celln print    : (null) ::");
      else {
         len = strlen (x_cell->p);
         if      (len        == 0   )  snprintf(unit_answer, LEN_UNIT, "s_celln print    : (null) ::");
         else if (len        >= 35  )  snprintf(unit_answer, LEN_UNIT, "s_celln print    : (%4d) :%-34.34s++", (int) strlen(x_cell->p), x_cell->p);
         else                          snprintf(unit_answer, LEN_UNIT, "s_celln print    : (%4d) :%s:"       , (int) strlen(x_cell->p), x_cell->p);
      }
   }
   else if (strcmp(a_question, "cell_contents")  == 0) {
      if      (x_cell       == NULL)   snprintf(unit_answer, LEN_UNIT, "s_celln cont (-) : (--:--) ::");
      else                             snprintf(unit_answer, LEN_UNIT, "s_celln cont (%c) : (%2d:%2d) :%-.40s:", (g_contents[my.cpos] >= ' ' && g_contents[my.cpos] <= '~') ? g_contents[my.cpos] : ' ', my.cpos, (int) strlen(g_contents), g_contents);
   }
   /*---(cell contents)------------------*/
   else if (strcmp(a_question, "cell_rpn")       == 0) {
      /*> if      (x_cell       == NULL)   snprintf(unit_answer, LEN_UNIT, "s_celln rpn      : (----) -");                                  <* 
       *> else if (x_cell->nrpn == 0)      snprintf(unit_answer, LEN_UNIT, "s_celln rpn      : (%4d) ."     , x_cell->nrpn);                <* 
       *> else                             snprintf(unit_answer, LEN_UNIT, "s_celln rpn      : (%4d) %s"    , x_cell->nrpn, x_cell->rpn);   <*/
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
   a_cell->s = strndup (a_source, LEN_RECD);
   a_cell->l = strlen  (a_cell->s);
   /*---(complete)-----------------------*/
   return 0;
}



/*============================----end-of-source---============================*/
