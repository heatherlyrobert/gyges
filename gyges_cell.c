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



#define   MAX_CELLTYPE     30
struct cCELL_INFO {
   char        type;                   /* cell type                           */
   char        terse       [10];       /* short description of cell type      */
   char        prefix;                 /* prefix in cell source string        */
   char        rpn;                    /* require processing by rpn           */
   char        calc;                   /* must it be calculated               */
   char        deps;                   /* must follow the dependencies        */
   char        result;                 /* what type is the result             */
   char        desc        [50];       /* description of cell type            */
   int         count;                  /* current count of type               */
} s_cell_info [MAX_CELLTYPE] = {
   /*-ty- -terse------- -pre -rpn calc -dep -res ---description---------------------------------------- -cnt- */
   {  's', "string"    , ' ', '-', '-', '-', '#', "string literal presented from source field"         ,    0 },
   {  'n', "number"    , ' ', '-', '-', '-', '=', "numeric literal presented in various formats"       ,    0 },
   {  'f', "formula"   , '=', 'y', 'y', 'y', '=', "numeric formula"                                    ,    0 },
   {  'm', "mod_str"   , '#', 'y', 'y', 'y', '#', "string formula"                                     ,    0 },
   {  'l', "flike"     , '~', 'y', 'y', 'y', '=', "numeric formula derived from another cell"          ,    0 },
   {  'L', "mlike"     , '~', 'y', 'y', 'y', '#', "string formula derived from another cell"           ,    0 },
   {  'p', "range"     , '&', 'y', 'y', 'y', '-', "range pointer to use in other formulas"             ,    0 },
   {  'a', "address"   , '&', 'y', 'y', 'y', '-', "address pointer to use in other formulas"           ,    0 },
   {  '>', "source"    , ' ', '-', '-', 'y', '-', "content used to fill merged cells"                  ,    0 },
   {  '+', "merged"    , '<', '-', '-', 'y', '-', "empty cell used to present merged information"      ,    0 },
   {  'w', "warning"   , ' ', '-', '-', '-', 'e', "cell contains a warning"                            ,    0 },
   {  'E', "error"     , ' ', 'y', 'y', 'y', 'e', "cell contains an error"                             ,    0 },
   {  '-', "blank"     , ' ', '-', '-', '-', '-', "blank cell"                                         ,    0 },
};


char    G_CELL_ALL    [20] = "";
char    G_CELL_RPN    [20] = "";
char    G_CELL_CALC   [20] = "";
char    G_CELL_DEPS   [20] = "";
char    G_CELL_NUM    [20] = "";
char    G_CELL_STR    [20] = "";
char    G_CELL_ERR    [20] = "";
char    G_CELL_FPRE   [20] = "";



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___PROGRAM_________o () { return; }

char
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
   ACEL   = 0;
   hcell  = NULL;
   tcell  = NULL;
   NCEL   = 0;
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
   strlcpy (G_CELL_ALL , "", 20);
   strlcpy (G_CELL_RPN , "", 20);
   strlcpy (G_CELL_CALC, "", 20);
   strlcpy (G_CELL_DEPS, "", 20);
   strlcpy (G_CELL_NUM , "", 20);
   strlcpy (G_CELL_STR , "", 20);
   strlcpy (G_CELL_ERR , "", 20);
   strlcpy (G_CELL_FPRE, "", 20);
   /*---(complete info table)------------*/
   DEBUG_CELL   yLOG_note    ("build cell validation types");
   --rce;
   for (i = 0; i < MAX_CELLTYPE; ++i) {
      DEBUG_CELL_M yLOG_value   ("ENTRY"     , i);
      DEBUG_CELL_M yLOG_char    ("type"      , s_cell_info [i].type);
      /*---(check for end)---------------*/
      if (s_cell_info [i].type == CTYPE_BLANK)  break;
      /*---(add to lists)----------------*/
      sprintf (t, "%c", s_cell_info [i].type);
      DEBUG_CELL_M yLOG_info    ("str type"  , t);
      DEBUG_CELL_M yLOG_char    ("rpn flag"  , s_cell_info [i].rpn);
      strcat (G_CELL_ALL , t);
      if (s_cell_info [i].calc    == 'y')  strcat (G_CELL_CALC, t);
      if (s_cell_info [i].deps    == 'y')  strcat (G_CELL_DEPS, t);
      if (s_cell_info [i].result  == '=')  strcat (G_CELL_NUM , t);
      if (s_cell_info [i].result  == '#')  strcat (G_CELL_STR , t);
      if (s_cell_info [i].result  == 'e')  strcat (G_CELL_ERR , t);
      if (s_cell_info [i].rpn     == 'y') {
         strcat  (G_CELL_RPN , t);
         sprintf (t, "%c", s_cell_info [i].prefix);
         if   (s_cell_info [i].prefix != ' ' && 
               strchr (G_CELL_FPRE, s_cell_info [i].prefix) == 0) {
            strcat  (G_CELL_FPRE , t);
         }
      }
      ++x_count;
   }
   /*---(report out)---------------------*/
   DEBUG_CELL   yLOG_value   ("x_count"   , x_count);
   DEBUG_CELL   yLOG_info    ("G_CELL_ALL" , G_CELL_ALL );
   DEBUG_CELL   yLOG_info    ("G_CELL_RPN" , G_CELL_RPN );
   DEBUG_CELL   yLOG_info    ("G_CELL_CALC", G_CELL_CALC);
   DEBUG_CELL   yLOG_info    ("G_CELL_DEPS", G_CELL_DEPS);
   DEBUG_CELL   yLOG_info    ("G_CELL_NUM" , G_CELL_NUM );
   DEBUG_CELL   yLOG_info    ("G_CELL_STR" , G_CELL_STR );
   DEBUG_CELL   yLOG_info    ("G_CELL_ERR" , G_CELL_ERR );
   DEBUG_CELL   yLOG_info    ("G_CELL_FPRE", G_CELL_FPRE);
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
CELL__purge        (void)
{
   /*---(beginning)----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   tCELL      *curr        = NULL;
   tCELL      *next        = NULL;
   char        rc          = 0;
   long        x_stamp     = 0;
   /*---(disconnect dependent cells)-----*/
   x_stamp = rand ();
   rc = SEQ_wipe_deps ();
   DEBUG_CELL   yLOG_value   ("seq rc"    , rc);
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
   /*---(ending)-------------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   /*---(complete)-----------------------*/
   return 0;
}

char
CELL_wrap          (void)
{
   /*---(cells)--------------------------*/
   CELL__purge ();
   hcell  = NULL;
   tcell  = NULL;
   NCEL   = 0;
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        basic utilities                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___BASICS__________o () { return; }

char         /*--> create a single new empty cell --------[ pistil [ 2c---- ]-*/
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
   (*a_cell)->t         = '-';
   (*a_cell)->v_num     = 0.0;
   (*a_cell)->v_str     = NULL;
   /*---(formatting)--------------------*/
   (*a_cell)->a         = '?';
   (*a_cell)->d         = '0';
   (*a_cell)->f         = '?';
   (*a_cell)->p         = NULL;
   (*a_cell)->n         = '-';
   /*---(calculations)-------------------*/
   (*a_cell)->rpn       = NULL;
   (*a_cell)->nrpn      = 0;
   (*a_cell)->calc      = NULL;
   (*a_cell)->clevel    = -1;
   (*a_cell)->cprev     = NULL;
   (*a_cell)->cnext     = NULL;
   /*---(dependencies)-------------------*/
   (*a_cell)->requires  = NULL;
   (*a_cell)->nrequire  = 0;
   (*a_cell)->provides  = NULL;
   (*a_cell)->nprovide  = 0;
   (*a_cell)->u         = 0;
   /*---(errors)-------------------------*/
   (*a_cell)->nerror    = 0;
   (*a_cell)->errors    = NULL;
   DEBUG_CELL   yLOG_note    ("successful");
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /*LG--: wipe the contents of a cell -------------------------------*/
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
   a_curr->t              = CTYPE_BLANK;
   if (a_curr->v_str     != NULL)  free (a_curr->v_str);
   a_curr->v_str          = NULL;
   a_curr->v_num          = 0.0;
   /*---(printable)----------------------*/
   DEBUG_CELL   yLOG_note    ("wipe printable");
   if (a_curr->p         != NULL)  free (a_curr->p);
   a_curr->p              = NULL;
   /*---(calculations)-------------------*/
   DEBUG_CELL   yLOG_note    ("clear rpn formulas and calculations");
   if (a_curr->rpn       != NULL)  free (a_curr->rpn);
   a_curr->rpn            = NULL;
   a_curr->nrpn           = 0;
   if (a_curr->calc      != NULL)  CALC_free (a_curr);
   a_curr->calc           = NULL;
   /*---(dependencies)-------------------*/
   DEBUG_CELL   yLOG_note    ("destroy all requires dependencies");
   if (a_curr->requires  != NULL)  DEP_cleanse (a_curr);
   a_curr->requires       = NULL;
   a_curr->nrequire       = 0;
   DEBUG_CELL   yLOG_note    ("unroot, but leave any other provides dependences");
   if (a_curr->provides  != NULL)  rc = DEP_rooting (a_curr, G_DEP_UNROOT);
   /*---(errors)-------------------------*/
   DEBUG_CELL   yLOG_note    ("clear errors");
   if (a_curr->errors    != NULL)  ERROR_cleanse (a_curr);
   a_curr->errors         = NULL;
   a_curr->nerror         = 0;
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*> test a cell pointer for safety ----------[ ------ [ ------ ]-*/
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

char         /*> remove a cell completely ----------------[ ------ [ ------ ]-*/
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

char         /*--> act on a dependent cell ---------------[--------[--------]-*/
CELL__depwipe      (FILE *a_file, char a_type, int *a_seq, int a_level, tCELL *a_curr, long a_stamp)
{
   /*---(design notes)-------------------*/
   /*
    *  to prepare cells for easy deletion, just wipe the content which will
    *  remove all downward dependencies.  once this is done to all cells, there
    *  are no connections left to stop a sequencial purge.
    */
   /*---(locals)-----------+-----------+-*/
   char        x_pre       [50]        = "-           ";
   char        rc          = 0;
   /*---(beginning)----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   DEBUG_CELL   yLOG_char    ("a_type"    ,  a_type);
   DEBUG_CELL   yLOG_value   ("a_seq"     , *a_seq);
   DEBUG_CELL   yLOG_point   ("a_curr"    ,  a_curr);
   /*---(defense)------------------------*/
   if (a_curr    == NULL)        return;     /* no cell                       */
   DEBUG_CELL   yLOG_info    ("label"     ,  a_curr->label);
   DEBUG_CELL   yLOG_value   ("stamp"     ,  a_curr->u);
   if (a_curr->u == a_stamp)     return;     /* already traveled across       */
   /*---(update)-------------------------*/
   if (a_type == 'x')  {
      rc = CELL__wipe    (a_curr);
   }
   /*---(update)-------------------------*/
   ++(*a_seq);
   if (a_type != 'x') a_curr->u = a_stamp;
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       setup and teardown                     ----===*/
/*====================------------------------------------====================*/
PRIV void  o___SETUP___________o () { return; }

char         /*> delete a register cell ------------------[--------[--------]-*/
CELL_regdel        (tCELL *a_curr)
{
   CELL__free (&a_curr, LINKED);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      creation/destruction                    ----===*/
/*====================------------------------------------====================*/
PRIV void  o___EXISTANCE_______o () { return; }

char
CELL__create       (tCELL **a_cell, int a_tab, int a_col, int a_row)
{
   /*---(locals)----------------------*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(defenses)-----------------------*/
   rc = LOC_legal (a_tab, a_col, a_row, CELL_GROW);
   --rce;  if (rc <  0)  return rce;
   /*---(create cell)-----------------*/
   rc = CELL__new (a_cell, LINKED);
   --rce;  if (rc <  0)  return rce;
   /*---(clear it out)----------------*/
   rc = LOC_hook  (*a_cell, a_tab, a_col, a_row);
   --rce;  if (rc <  0)  return rce;
   /*---(complete)--------------------*/
   return 0;
}

char
CELL_delete        (char a_mode, int a_tab, int a_col, int a_row)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   tCELL      *curr        = NULL;
   tCELL      *x_other     = NULL;
   char        x_before    [LEN_RECD]   = "[<{(null)}>]";
   char        x_after     [LEN_RECD]   = "[<{(null)}>]";
   /*---(defenses)-----------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   DEBUG_CELL   yLOG_value   ("a_tab"     , a_tab);
   DEBUG_CELL   yLOG_value   ("a_col"     , a_col);
   DEBUG_CELL   yLOG_value   ("a_row"     , a_row);
   rc          = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);
   DEBUG_CELL   yLOG_value   ("LOC_legal" , rc);
   --rce;  if (rc < 0)             return rce;
   curr        = LOC_cell_at_loc (a_tab, a_col, a_row);
   DEBUG_CELL   yLOG_point   ("curr"      , curr);
   --rce;  if (curr == NULL)       return rce;
   /*---(save before)-----------------*/
   if (curr->s != NULL)  strcpy (x_before, curr->s);
   /*---(history)------------------------*/
   if (a_mode == CHG_INPUT   )  HIST_change ("delete", a_tab, a_col, a_row, x_before, x_after);
   if (a_mode == CHG_INPUTAND)  HIST_change ("DELETE", a_tab, a_col, a_row, x_before, x_after);
   /*---(check on merges)-------------*/
   if (curr->t == CTYPE_MERGE) {
      DEBUG_CELL   yLOG_note    ("remove merge dependencies");
      x_other = DEP_delmerge (curr);
      curr->a = '<';
      curr->f = '?';
   } else  {
      DEBUG_CELL   yLOG_note    ("remove merge root");
      DEP_delmergeroot (curr);
   }
   /*---(clear it out)----------------*/
   DEBUG_CELL   yLOG_complex ("details"   , "ptr=%p, tab=%4d, col=%4d, row=%4d, t=%c, u=%d", curr, curr->tab, curr->col, curr->row, curr->t, curr->u);
   rc = CELL__wipe   (curr);
   DEBUG_CELL   yLOG_value   ("wipe rc"   , rc);
   --rce;  if (rc < 0)             return rce;
   /*---(see if its still there)------*/
   curr        = LOC_cell_at_loc (a_tab, a_col, a_row);
   if (curr == NULL) {
      DEBUG_CELL   yLOG_note    ("cell already removed, moving on");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(remove)----------------------*/
   --rce;
   DEBUG_CELL   yLOG_point   ("provides"  , curr->provides);
   if (curr->provides == NULL) {
      rc = LOC_unhook  (curr);
      DEBUG_CELL   yLOG_value   ("unhook rc" , rc);
      if (rc < 0)          return rce;
      rc = CELL__free   (&curr, LINKED);
      DEBUG_CELL   yLOG_value   ("free rc"   , rc);
      if (rc < 0)          return rce - 1;
   } else {
      CELL_change  (&curr, CHG_INPUT, a_tab, a_col, a_row, "");
      DEBUG_CELL   yLOG_value   ("change rc" , rc);
      if (curr == NULL)    return rce - 2;
   }
   /*---(update former merges)-----------*/
   if (x_other != NULL)  CELL_printable (x_other);
   /*---(complete)--------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*> create a duplicate locationless cell ----[ ------ [ ------ ]-*/
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

char       /*----: change te contents of a cell ------------------------------*/
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
   tCELL      *x_curr      = NULL;
   tCELL      *x_other     = NULL;
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
   rc = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);
   DEBUG_CELL   yLOG_info    ("legal"     , (rc >= 0) ? "yes" : "no" );
   --rce;  if (rc <  0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(cell present)-------------------*/
   x_curr      = LOC_cell_at_loc (a_tab, a_col, a_row);
   DEBUG_CELL   yLOG_point   ("x_curr"    , x_curr);
   /*---(check merge)--------------------*/
   if (x_curr != NULL) {
      if (x_curr->t == CTYPE_MERGE) {
         x_other = DEP_delmerge (x_curr);
         x_curr->a = '<';
         x_curr->f = '?';
      } else if (a_source == NULL || strlen (a_source) == 0) {
         DEP_delmergeroot (x_curr);
      }
   }
   /*---(wipe or create)-----------------*/
   strcpy (s_bsource, "[<{(null)}>]");
   strcpy (s_bformat, "???");
   if (x_curr != NULL) {
      DEBUG_CELL   yLOG_note    ("save existing data");
      if (x_curr->s != NULL)  strcpy (s_bsource, x_curr->s);
      sprintf (s_bformat, "%c%c%c", x_curr->f, x_curr->a, x_curr->d);
      DEBUG_CELL   yLOG_note    ("wipe existing cell");
      rc   = CELL__wipe (x_curr);
      DEBUG_CELL   yLOG_value   ("rc"        , rc);
      x_curr        = LOC_cell_at_loc (a_tab, a_col, a_row);
      DEBUG_CELL   yLOG_point   ("x_curr now"  , x_curr);
   }
   --rce;  if (x_curr == NULL) {
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
   rc = CELL__interpret (x_curr);
   DEBUG_CELL   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      rc = CELL_printable (x_curr);      /* show as error */
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (a_mode != CHG_OVER && a_mode != CHG_OVERAND) {
      DEBUG_CELL   yLOG_note    ("create printable version");
      rc = CELL_printable (x_curr);
      DEBUG_CELL   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_CELL   yLOG_note    ("review dependency tree calculation");
      rc = SEQ_calc_up    (x_curr);
      DEBUG_CELL   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(process likes)------------------*/
   DEP_updatelikes (x_curr);
   /*---(update former merges)-----------*/
   if (x_other != NULL)  CELL_printable (x_other);
   /*---(return)-------------------------*/
   if (a_cell != NULL)  *a_cell = x_curr;
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

tCELL*     /*----: change te contents of a cell ------------------------------*/
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
   rc = CELL_printable (x_new);
   if (rc < 0) {
      DEBUG_CELL   yLOG_warn    ("printable" , "returned a bad return code");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   DEBUG_CELL   yLOG_note    ("call calc");
   rc = SEQ_calc_up    (x_new);
   if (rc < 0) {
      DEBUG_CELL   yLOG_warn    ("calc"      , "returned a bad return code");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return x_new;
}



/*====================------------------------------------====================*/
/*===----                             driver                           ----===*/
/*====================------------------------------------====================*/
PRIV void  o___NUMERICS________o () { return; }

char         /*-> interpret any/all numbers ---------------[ floret [ 1a---- ]*/
CELL__numerics     (tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =  -1;
   double      x_value     =   0;
   /*---(header)-------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   DEBUG_CELL   yLOG_point   ("a_cell"    , a_cell);
   a_cell->v_num = x_value;
   /*---(defense)------------------------*/
   DEBUG_CELL   yLOG_value   ("l"         , a_cell->l);
   DEBUG_CELL   yLOG_char    ("s [0]"     , a_cell->s [0]);
   DEBUG_CELL   yLOG_info    ("valid"     , sv_numeric);
   --rce;  if (strchr (sv_numeric, a_cell->s [0]) == 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CELL   yLOG_char    ("s [1]"     , a_cell->s [1]);
   /*---(oct, bin, hex)------------------*/
   if (a_cell->l > 1 && a_cell->s [0] == '0') {
      switch (a_cell->s [1]) {
      case 'b':  case 'B':
         DEBUG_CELL   yLOG_note    ("binary");
         rc = strl2bin  (a_cell->s, &x_value, LEN_RECD);
         break;
      case 'o':  default :
         DEBUG_CELL   yLOG_note    ("octal");
         rc = strl2oct  (a_cell->s, &x_value, LEN_RECD);
         break;
      case 'x':  case 'X':
         DEBUG_CELL   yLOG_note    ("hexadecimal");
         rc = strl2hex  (a_cell->s, &x_value, LEN_RECD);
         break;
      }
   }
   /*---(float, int)---------------------*/
   if (rc < 0) {
      DEBUG_CELL   yLOG_note    ("float/int");
      rc = strl2real (a_cell->s, &x_value, LEN_RECD);
   }
   /*---(judge outcome)------------------*/
   if (rc >= 0) {
      a_cell->t     = CTYPE_NUM;
      a_cell->v_num = x_value;
      if (a_cell->a == '?')  a_cell->a = '>';
   }
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                             driver                           ----===*/
/*====================------------------------------------====================*/
PRIV void  o___FORMULAS________o () { return; }

char         /*-> build any/all formulas ------------------[ petal  [ 1c---- ]*/
CELL__rpn          (tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_type      =  '-';
   /*---(header)-------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(set type)-----------------------*/
   x_type = a_cell->s [0];
   DEBUG_CELL   yLOG_char    ("s [0]"     , x_type);
   a_cell->t = CELL__ftype (x_type);
   DEBUG_CELL   yLOG_char    ("ftype"     , a_cell->t);
   /*---(tighten)------------------------*/
   strltrim (a_cell->s, ySTR_EVERY, LEN_RECD);
   DEBUG_CELL   yLOG_info    ("compressed", a_cell->s);
   a_cell->l = strlen  (a_cell->s);
   DEBUG_CELL   yLOG_value   ("rev len"   , a_cell->l);
   /*---(rpn)----------------------------*/
   rc = RPN_convert (a_cell);
   DEBUG_CELL   yLOG_value   ("rc"        , rc);
   /*---(troubles)-----------------------*/
   --rce;  if (rc <  0) {
      DEBUG_CELL   yLOG_note    ("failed");
      a_cell->t     = CTYPE_ERROR;
      a_cell->v_str = strndup ("#.rpn", LEN_RECD);
      CALC_free   (a_cell);
      DEP_cleanse (a_cell);
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CELL   yLOG_info    ("rpn"       , a_cell->rpn);
   /*---(complete)----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> build any/all formulas ------------------[ petal  [ 1c---- ]*/
CELL__build        (tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(calc)---------------------------*/
   rc = CALC_build  (a_cell);
   DEBUG_CELL   yLOG_value   ("rc"        , rc);
   /*---(troubles)-----------------------*/
   --rce;  if (rc <  0) {
      DEBUG_CELL   yLOG_note    ("failed");
      a_cell->t     = CTYPE_ERROR;
      a_cell->v_str = strndup ("#.build", LEN_RECD);
      CALC_free   (a_cell);
      DEP_cleanse (a_cell);
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> build any/all formulas ------------------[ petal  [ 1c---- ]*/
CELL__like         (tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   short       x_tab       =    0;
   short       x_col       =    0;
   short       x_row       =    0;
   tCELL      *x_like      =    0;
   /*---(header)-------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (a_cell->t != CTYPE_FLIKE && a_cell->t != CTYPE_MLIKE) {
      DEBUG_CELL   yLOG_note    ("unnecessary");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return -(rce);
   }
   /*---(parse master cell)--------------*/
   rc = LOC_parse (a_cell->s + 1, &x_tab, &x_col, &x_row, NULL);
   DEBUG_CELL   yLOG_value   ("parse rc"  , rc);
   --rce;  if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get master cell)----------------*/
   x_like = LOC_cell_at_loc (x_tab, x_col, x_row);
   DEBUG_CELL   yLOG_point   ("x_like"    , x_like);
   --rce;  if (x_like == NULL) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(create dependency)--------------*/
   rc = DEP_create (G_DEP_SOURCE, a_cell, x_like);
   DEBUG_CELL   yLOG_value   ("dep rc"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> build any/all formulas ------------------[ petal  [ 1c---- ]*/
CELL__point        (tCELL *a_cell)
{
   if (a_cell->t != CTYPE_RANGE && a_cell->t != CTYPE_ADDR )  return 0;
}

char         /*-> build any/all formulas ------------------[ petal  [ 1c---- ]*/
CELL__eval         (tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(eval)---------------------------*/
   rc = CALC_eval   (a_cell);
   DEBUG_CELL   yLOG_value   ("rc"        , rc);
   /*---(troubles)-----------------------*/
   --rce;  if (rc < 0) {
      a_cell->t     = CTYPE_ERROR;
      a_cell->v_str = strndup ("#.eval", LEN_RECD);
      CALC_free   (a_cell);
      DEP_cleanse (a_cell);
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> interpret any/all formulas --------------[ floret [ 1a---- ]*/
CELL__formulas     (tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =   -1;
   char        x_type      =  '-';
   /*---(header)-------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   DEBUG_CELL   yLOG_point   ("a_cell"    , a_cell);
   /*---(defense)------------------------*/
   x_type = a_cell->s [0];
   DEBUG_CELL   yLOG_char    ("s [0]"     , x_type);
   DEBUG_CELL   yLOG_info    ("valid"     , G_CELL_FPRE);
   --rce;  if (strchr (G_CELL_FPRE, x_type) == 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(build)--------------------------*/
   rc = CELL__rpn    (a_cell);
   DEBUG_CELL   yLOG_value   ("rpn rc"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(build)--------------------------*/
   rc = CELL__build  (a_cell);
   DEBUG_CELL   yLOG_value   ("build rc"  , rc);
   --rce;  if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(like)---------------------------*/
   rc = CELL__like    (a_cell);
   DEBUG_CELL   yLOG_value   ("like rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(pointer)------------------------*/
   rc = CELL__point   (a_cell);
   DEBUG_CELL   yLOG_value   ("point rc"  , rc);
   --rce;  if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(exec)---------------------------*/
   rc = CELL__eval    (a_cell);
   DEBUG_CELL   yLOG_value   ("eval rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_CELL   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(sort aligment)------------------*/
   if (a_cell->a == '?') {
      switch (a_cell->t) {
      case CTYPE_FORM  :
      case CTYPE_FLIKE :
         a_cell->a = '>';
         break;
      case CTYPE_MOD   :
      case CTYPE_MLIKE :
      case CTYPE_ADDR  :
      case CTYPE_RANGE :
         a_cell->a = '<';
         break;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                             driver                           ----===*/
/*====================------------------------------------====================*/
PRIV void  o___DRIVER__________o () { return; }

char
CELL__ftype        (char a_prefix)
{
   int         i           = 0;                  /* iterator -- character     */
   for (i = 0; i < MAX_CELLTYPE; ++i) {
      if (s_cell_info [i].type   == '-'     )  break;
      if (s_cell_info [i].prefix != a_prefix)  continue;
      return s_cell_info [i].type;
   }
   return -1;
}

char
CELL__merges       (tCELL *a_cell)
{
   int         x_col       =   0;
   tCELL      *x_next      = NULL;
   char        rc          =   0;
   /*---(check for merges)------------*/
   if (strchr (G_CELL_STR, a_cell->t) == NULL)  return 0;
   DEBUG_CELL   yLOG_note    ("check for merges in cells to right");
   x_col   = a_cell->col + 1;
   DEBUG_CELL   yLOG_value   ("x_col"     , x_col);
   DEBUG_CELL   yLOG_value   ("NCOL"      , NCOL);
   while (x_col <  NCOL) {
      x_next = LOC_cell_at_loc (a_cell->tab, x_col, a_cell->row);
      DEBUG_CELL   yLOG_point   ("next cell" , x_next);
      if (x_next    == NULL)  break;
      DEBUG_CELL   yLOG_char    ("type"      , x_next->a);
      if (x_next->a != '+' )  break;
      x_next->t = CTYPE_MERGE;
      rc = DEP_create (G_DEP_MERGED, a_cell, x_next);
      ++x_col;
   }
   return 0;
}

char         /*- identify cell type from contents --------[ ------ [ ------ ]-*/
CELL__interpret    (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell)     /* cell to interpret                            */
{  /*---(design notes)--------------------------------------------------------*/
   /* review the contents of a cell and infer the cell type.  the types then  */
   /* will drive the actions taken on the cell.  types include...             */
   /*    -      empty cell                                                    */
   /*    "      string literal                                                */
   /*    &      address pointer                                               */
   /*    #      string formula                                                */
   /*    value  numeric literal                                               */
   /*    =      numeric formula                                               */
   /*    ~      like another cell                                             */
   /*    else   string literal                                                */
   /*---(locals)-----------+-----------+-*//*---------------------------------*/
   int         i           = 0;                  /* iterator -- character     */
   char        temp        [LEN_RECD];            /* working storage           */
   char        x_pre       = ' ';                /* source string prefix      */
   int         len         = a_cell->l;          /* current string length     */
   char        rce         = -10;
   char        rc          =   0;
   double      x_value     =   0;
   int         x_tab       =   0;
   int         x_col       =   0;
   int         x_row       =   0;
   tCELL      *x_like      = NULL;
   /*---(heading)------------------------*/
   DEBUG_CELL   yLOG_enter   (__FUNCTION__);
   /*---(defense: null cell)-------------*/
   DEBUG_CELL   yLOG_point   ("a_cell"    , a_cell);
   --rce;
   if (a_cell    == NULL) {
      DEBUG_CELL   yLOG_note    ("cell is null");
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: null contents)---------*/
   DEBUG_CELL   yLOG_point   ("contents"  , a_cell->s);
   --rce;
   if (a_cell->s == NULL) {
      DEBUG_CELL   yLOG_note    ("cell contents are null");
      a_cell->t = CTYPE_BLANK;
      a_cell->s = strndup ("", LEN_RECD);
      a_cell->l = 0;
      a_cell->a = '?';
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for blank/empty)----------*/
   DEBUG_CELL   yLOG_point   ("length"    , a_cell->l);
   if      (strcmp (a_cell->s, "") == 0) {
      a_cell->t = CTYPE_BLANK;
      a_cell->l = 0;
      a_cell->a = '?';
      DEBUG_CELL   yLOG_complex ("type"      , "empty which is an %c", a_cell->t);
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(initialize)---------------------*/
   a_cell->v_num = 0.0;
   strncpy (temp, a_cell->s, LEN_RECD);
   x_pre = temp [0];
   --rce;
   /*---(numerics)-----------------------*/
   rc = CELL__numerics (a_cell);
   DEBUG_CELL   yLOG_value   ("numeric"   , rc);
   if (rc >= 0) {
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for merge)----------------*/
   if (a_cell->l == 1 && x_pre == '<') {
      CELL_merge (a_cell);
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   CELL_unmerge (a_cell);
   /*---(check for formula mark)---------*/
   rc = CELL__formulas (a_cell);
   DEBUG_CELL   yLOG_value   ("formulas"  , rc);
   if (rc >= 0) {
      DEBUG_CELL   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*> if (strchr (G_CELL_FPRE, x_pre) != 0) {                                             <* 
    *>    DEBUG_CELL   yLOG_complex ("type"      , "formula which is an %c", a_cell->t);   <* 
    *>    DEBUG_CELL   yLOG_info    ("source"    , a_cell->s);                             <* 
    *>    a_cell->t = CELL__ftype (x_pre);                                                 <* 
    *>    DEBUG_CELL   yLOG_char    ("ftype"     , a_cell->t);                             <* 
    *>    strltrim (a_cell->s, ySTR_EVERY, LEN_RECD);                                      <* 
    *>    DEBUG_CELL   yLOG_info    ("compressed", a_cell->s);                             <* 
    *>    a_cell->l = strlen  (a_cell->s);                                                 <* 
    *>    if (a_cell->a == '?')  {                                                         <* 
    *>       if (x_pre == '#')  a_cell->a = '<';                                           <* 
    *>       else               a_cell->a = '>';                                           <* 
    *>    }                                                                                <* 
    *>    DEBUG_CELL   yLOG_value   ("rev len"   , a_cell->l);                             <* 
    *>    DEBUG_CELL   yLOG_info    ("RPN"       , "call rpn converion");                  <* 
    *>    rc = RPN_convert (a_cell);                                                       <* 
    *>    --rce;  if (rc < 0) {                                                            <* 
    *>       a_cell->t = CTYPE_ERROR;                                                      <* 
    *>       a_cell->v_str = strndup ("#.rpn", LEN_RECD);                                  <* 
    *>       CALC_free   (a_cell);                                                         <* 
    *>       DEP_cleanse (a_cell);                                                         <* 
    *>       DEBUG_CELL   yLOG_exit    (__FUNCTION__);                                     <* 
    *>       return rce;                                                                   <* 
    *>    }                                                                                <* 
    *>    DEBUG_CELL   yLOG_info    ("rpn"       , a_cell->rpn);                           <* 
    *>    DEBUG_CELL   yLOG_info    ("CALC"      , "call calc build");                     <* 
    *>    rc = CALC_build  (a_cell);                                                       <* 
    *>    if (rc < 0) {                                                                    <* 
    *>       a_cell->t = CTYPE_ERROR;                                                      <* 
    *>       a_cell->v_str = strndup ("#.build", LEN_RECD);                                <* 
    *>       CALC_free   (a_cell);                                                         <* 
    *>       DEP_cleanse (a_cell);                                                         <* 
    *>       DEBUG_CELL   yLOG_exit    (__FUNCTION__);                                     <* 
    *>       return rce - 1;                                                               <* 
    *>    }                                                                                <* 
    *>    /+---(like dependency)-------------+/                                            <* 
    *>    if (temp[0] == '~') {                                                            <* 
    *>       DEBUG_CELL   yLOG_note    ("processing a like formula");                      <* 
    *>       rc = LOC_parse (a_cell->s + 1, &x_tab, &x_col, &x_row, NULL);                 <* 
    *>       DEBUG_CELL   yLOG_value   ("parse rc"  , rc);                                 <* 
    *>       --rce;                                                                        <* 
    *>       if (rc < 0) {                                                                 <* 
    *>          DEBUG_CELL   yLOG_exit    (__FUNCTION__);                                  <* 
    *>          return rce;                                                                <* 
    *>       }                                                                             <* 
    *>       x_like = LOC_cell_at_loc (x_tab, x_col, x_row);                               <* 
    *>       DEBUG_CELL   yLOG_point   ("x_like"    , x_like);                             <* 
    *>       --rce;                                                                        <* 
    *>       if (x_like == NULL) {                                                         <* 
    *>          DEBUG_CELL   yLOG_exit    (__FUNCTION__);                                  <* 
    *>          return rce;                                                                <* 
    *>       }                                                                             <* 
    *>       rc = DEP_create (G_DEP_SOURCE, a_cell, x_like);                               <* 
    *>       DEBUG_CELL   yLOG_value   ("dep rc"    , rc);                                 <* 
    *>       if (rc < 0) {                                                                 <* 
    *>          DEBUG_CELL   yLOG_exit    (__FUNCTION__);                                  <* 
    *>          return rce;                                                                <* 
    *>       }                                                                             <* 
    *>    }                                                                                <* 
    *>    /+---(pointers)--------------------+/                                            <* 
    *>    if (temp [0] == '&') {                                                           <* 
    *>       DEBUG_CELL   yLOG_info    ("POINTER"   , "mark as a pointer");                <* 
    *>       CALC_checkpointer (a_cell);                                                   <* 
    *>    }                                                                                <* 
    *>    /+---(other)-----------------------+/                                            <* 
    *>    else {                                                                           <* 
    *>       DEBUG_CELL   yLOG_info    ("CALC"      , "call calc eval");                   <* 
    *>       rc = CALC_eval   (a_cell);                                                    <* 
    *>       if (rc < 0) {                                                                 <* 
    *>          a_cell->t = CTYPE_ERROR;                                                   <* 
    *>          a_cell->v_str = strndup ("#.eval", LEN_RECD);                              <* 
    *>          CALC_free   (a_cell);                                                      <* 
   *>          DEP_cleanse (a_cell);                                                      <* 
      *>          DEBUG_CELL   yLOG_exit    (__FUNCTION__);                                  <* 
      *>          return rce - 2;                                                            <* 
      *>       }                                                                             <* 
      *>    }                                                                                <* 
      *>    CELL__merges (a_cell);                                                           <* 
      *>    DEBUG_CELL   yLOG_exit    (__FUNCTION__);                                        <* 
      *>    return 0;                                                                        <* 
      *> }                                                                                   <*/
      /*---(check for non-numbers)----------*/
      /*> a_cell->t = CTYPE_NUM;                                                         <* 
       *> for (i = 0; i < len; ++i) {                                                    <* 
       *>    if (strchr(sv_numeric, temp[i])  != 0) continue;                            <* 
       *>    a_cell->t = CTYPE_STR;                                                      <* 
       *> }                                                                              <*/
      /*---(therefore, number)-----------*/
      /*> if (a_cell->t == CTYPE_NUM) {                                                       <* 
       *>    a_cell->v_num = atof (a_cell->s);                                                <* 
       *>    if (a_cell->a == '?')  a_cell->a = '>';                                          <* 
       *>    DEBUG_CELL   yLOG_complex ("type"      , "numeric which is an %c", a_cell->t);   <* 
       *> }                                                                                   <*/
      /*---(then string)-----------------*/
      /*> else {                                                                         <*/

      a_cell->t = CTYPE_STR;
   if (a_cell->a == '?')  a_cell->a = '<';
   DEBUG_CELL   yLOG_complex ("type"      , "string which is an %c", a_cell->t);
   CELL__merges (a_cell);
   /*> }                                                                              <*/
   /*---(complete)-----------------------*/
   DEBUG_CELL   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        merge-specific                        ----===*/
/*====================------------------------------------====================*/
PRIV void  o___MERGES__________o () { return; }

char         /*--> check for merge-type cell -------------[-leaf---[--------]-*/
CELL__merge_valid    (tCELL *a_curr)
{  /*---(design notes)--------------------------------------------------------*/
   /* check if a cell is a merge/bleed type cell                              */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (a_curr       == NULL)  return rce;  /* must be '<'             */
   --rce;  if (a_curr->s    == NULL)  return rce;  /* must be '<'             */
   --rce;  if (a_curr->l    != 1   )  return rce;  /* must be '<'             */
   --rce;  if (a_curr->s[0] != '<' )  return rce;  /* must be '<'             */
   --rce;  if (a_curr->col  <= 0   )  return rce;  /* must have room          */
   /*---(complete)-----------------------*/
   return 0;
}

tCELL*       /*--> check to left for merge ---------------[--------[--------]-*/
CELL__merge_left     (tCELL *a_curr)
{  /*---(design notes)--------------------------------------------------------*/
   /* if possible, merge with the cells to the left                           */
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_left      = NULL;
   /*> tCELL      *x_save      = NULL;                                                <*/
   int         i           = 0;
   /*---(defenses)-----------------------*/
   if (CELL__merge_valid (a_curr) < 0)  return a_curr;
   /*---(find real head)-----------------*/
   /*> x_save = a_curr;                                                               <*/
   for (i = a_curr->col - 1; i >= 0; --i) {
      x_left  = LOC_cell_at_loc (a_curr->tab, i, a_curr->row);
      if (x_left    == NULL)                      return NULL;
      if (x_left->l == 1 && x_left->s[0] == '<')  continue;  /* should merge  */
      break;
   }
   /*---(complete)-----------------------*/
   return x_left;
}

char         /*--> add merges to right -------------------[--------[--------]-*/
CELL__merge_right    (tCELL *a_left)
{  /*---(design notes)--------------------------------------------------------*/
   /* add all applicable cells to the right into the merge                    */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_right     = NULL;
   tCELL      *x_merged    = NULL;
   int         i           =    0;
   int         c           =    0;
   /*---(header)-------------------------*/
   /*---(defenses)-----------------------*/
   --rce;  if (a_left == NULL)  return rce;
   /*---(merge)--------------------------*/
   for (i = 1; i <= MAX_MERGE; ++i) {
      /*---(get next)--------------------*/
      x_right = LOC_cell_at_loc (a_left->tab, a_left->col + i, a_left->row);
      /*---(filter)----------------------*/
      if (CELL__merge_valid (x_right) < 0)  return c;
      ++c;
      /*---(label)-----------------------*/
      x_right->t = CTYPE_MERGE;
      x_right->f = '+';
      x_right->a = '+';
      /*---(check existing)--------------*/
      x_merged = DEP_merge_source (x_right);
      if (x_merged != NULL) {
         if (x_merged == a_left)                 continue; /* already correct */
         rc = DEP_delete (G_DEP_MERGED, x_merged, x_right);
         --rce;  if (rc < 0)  return rce;
      }
      /*---(new merge)-------------------*/
      rc = DEP_create (G_DEP_MERGED, a_left, x_right);
      --rce;  if (rc < 0)  return rce;
   }
   /*---(complete)-----------------------*/
   return c;
}

char         /*--> remove merges to right ----------------[--------[--------]-*/
CELL__unmerge_right  (tCELL *a_left)
{  /*---(design notes)--------------------------------------------------------*/
   /* remove merge from all applicable cells to the right                     */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_right     = NULL;
   tCELL      *x_merged    = NULL;
   int         i           =    0;
   /*---(header)-----------------------------*/
   DEBUG_CELL  yLOG_enter  (__FUNCTION__);
   DEBUG_CELL  yLOG_point  ("a_left"    , a_left);
   /*---(defenses)-----------------------*/
   --rce;  if (a_left == NULL) {
      DEBUG_CELL  yLOG_exitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(merge)--------------------------*/
   for (i = 1; i < MAX_MERGE; ++i) {
      DEBUG_CELL  yLOG_value  ("i"         , i);
      /*---(get next)--------------------*/
      x_right = LOC_cell_at_loc (a_left->tab, a_left->col + i, a_left->row);
      DEBUG_CELL  yLOG_point  ("x_right"   , x_right);
      if (x_right == NULL) {
         DEBUG_CELL  yLOG_exit   (__FUNCTION__);
         return 0;
      }
      /*---(filter)----------------------*/
      rc = CELL__merge_valid (x_right);
      DEBUG_CELL  yLOG_value  ("valid_rc"  , rc);
      if (rc < 0) {
         DEBUG_CELL  yLOG_exit   (__FUNCTION__);
         return 0;
      }
      /*---(label)-----------------------*/
      DEBUG_CELL  yLOG_note   ("turn into string");
      x_right->t = CTYPE_STR;
      x_right->f = '?';
      x_right->a = '<';
      /*---(unmerge)---------------------*/
      x_merged = DEP_merge_source (x_right);
      DEBUG_CELL  yLOG_point  ("x_merged"  , x_merged);
      if (x_merged != NULL) {
         rc = DEP_delete (G_DEP_MERGED, x_merged, x_right);
         DEBUG_CELL  yLOG_value  ("dep_rc"    , rc);
      }
      /*---(reprint)---------------------*/
      rc = CELL_printable (x_right);  /* with no dependency now, not automatic */
      DEBUG_CELL  yLOG_value  ("print_rc"  , rc);
   }
   DEBUG_CELL  yLOG_note   ("done with loop");
   /*---(complete)-----------------------*/
   DEBUG_CELL  yLOG_exit   (__FUNCTION__);
   return 0;
}

char         /*--> coordinate a merge --------------------[--------[--------]-*/
CELL_merge           (tCELL *a_curr)
{  /*---(design notes)--------------------------------------------------------*/
   /* when adding a cell with a merge indicator, try to add to existing merge */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   tCELL      *x_left      = NULL;
   /*---(look left)----------------------*/
   x_left = CELL__merge_left (a_curr);
   /*---(check for broken merge)---------*/
   --rce;  if (x_left == NULL) {
      rc = CELL__merge_valid (a_curr);
      if (rc >=  0) {
         a_curr->t = CTYPE_STR;
         a_curr->f = '?';
         a_curr->a = '<';
         rc = CELL__unmerge_right (a_curr);
         if (rc < 0)  return rce;
         return rce + 1;
      }
      x_left = a_curr;
   }
   /*---(merge right)--------------------*/
   rc = CELL__merge_right (x_left);
   --rce;  if (rc < 0)  return rce;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> check and coordinate an unmerge -------[--------[--------]-*/
CELL_unmerge         (tCELL *a_curr)
{  /*---(design notes)--------------------------------------------------------*/
   /* checks whether a larger merge must be split by a cell change            */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   tCELL      *x_merged    = NULL;
   /*---(defenses)-----------------------*/
   --rce;  if (CELL__merge_valid (a_curr) >= 0)  return rce;
   /*---(check merge status)-------------*/
   x_merged = DEP_merge_source (a_curr);
   /*---(unmerge)------------------------*/
   if (x_merged != NULL) {
      DEP_delete (G_DEP_MERGED, x_merged, a_curr);
      a_curr->t = CTYPE_STR;
      a_curr->f = '?';
      a_curr->a = '<';
   }
   /*---(merge right)--------------------*/
   rc = CELL__merge_right (a_curr);
   --rce;  if (rc < 0)  return rce;
   /*---(complete)-----------------------*/
   return 0;
}

char               /* PURPOSE : merge horizontal cells -----------------------*/
CELL_merge_visu    (void)
{
   /*---(defenses)---------------------------*/
   char      a_type = 'm';
   char     *valid = "mu";
   if (strchr(valid, a_type) == 0) return -1;
   int       xcol, xrow;
   /*---(process range)----------------------*/
   tCELL   *first = VISU_first(NULL, &xcol, &xrow);
   tCELL   *next  = first;
   do {
      if (a_type == 'm') {
         if (next == NULL) {
            CELL_change (&next, CHG_NOHIST, CTAB, xcol, xrow, "<");
         }
         if (first->col != next->col) next->a = '+';
      } else if (next != NULL) next->a = a_type;
      CELL_printable (next);
      next  = VISU_next(NULL, &xcol, &xrow);
   } while (next != DONE_DONE);
   /*---(complete)---------------------------*/
   return 0;
}

char         /*--> unmerge horizontal cells --------------[ ------ [--------]-*/
CELL_unmerge_visu  (void)
{
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_start     = NULL;      
   tCELL      *x_next      = NULL;
   int         x_col       = 0;
   int         x_row       = 0;
   int         x_row_save  = 0;
   int         x_inc       = 0;
   /*---(start in upper left)------------*/
   x_start  = VISU_first (NULL, &x_col, &x_row);
   x_next   = x_start;
   /*---(process range)------------------*/
   do {
      /*---(nulls)-----------------------*/
      if (x_next == NULL) {
         x_start = x_next  = VISU_next (NULL, &x_col, &x_row);
         continue;
      }
      /*---(new line)--------------------*/
      if (x_next->l == 1)
         CELL_change (&x_next, CHG_NOHIST, CTAB, x_col, x_row, "<");
      if (x_start->col != x_next->col) x_next->a = '+';
      CELL_printable (x_next);
      x_next  = VISU_next (NULL, &x_col, &x_row);
   } while (x_next != DONE_DONE);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         formatting                           ----===*/
/*====================------------------------------------====================*/
PRIV void  o___FORMATTING______o () { return; }

char         /*--> erase cells in current selection ------[ ------ [ ------ ]-*/
CELL_erase         (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;             /* generic return code            */
   char        rce         = -10;           /* return code for errors         */
   long        x_stamp     = 0;             /* timestamp for cell marking     */
   int         x_seq       = 0;             /* sequential number for action   */
   tCELL      *x_next      = NULL;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   int         x_count     = 0;
   /*---(process dedependent cells)----------*/
   /*> x_stamp = rand ();                                                             <* 
    *> DEBUG_CELL   yLOG_llong   ("x_stamp"   , x_stamp);                             <* 
    *> x_seq   = 0;                                                                   <* 
    *> DEBUG_CELL   yLOG_note    ("dependent cells");                                 <* 
    *> DEBUG_CELL   yLOG_value   ("x_seq"     , x_seq);                               <* 
    *> rc = DEP__tail (NULL, '-', &x_seq, 0, s_root, x_stamp, CELL_depwipe);            <* 
    *> DEBUG_CELL   yLOG_value   ("x_seq"     , x_seq);                               <*/
   /*---(process independent cells)----------*/
   x_next = VISU_first(NULL, &x_col, &x_row);
   do {
      if (x_count == 0)  CELL_delete (CHG_INPUT   , CTAB, x_col, x_row);
      else               CELL_delete (CHG_INPUTAND, CTAB, x_col, x_row);
      /*> CELL_printable (x_next);                                                      <*/
      ++x_count;
      x_next  = VISU_next(NULL, &x_col, &x_row);
   } while (x_next != DONE_DONE);
   /*---(complete)---------------------------*/
   return 0;
}

char               /* PURPOSE : change horizontal alignment for cells --------*/
CELL_format_valid    (char a_format)
{
   if (strchr (sv_formats, a_format) == 0)   return '?';
   return a_format;
}

char               /* PURPOSE : change number formatting for cells -----------*/
CELL_format        (char a_mode, char a_format)
{
   /*---(design notes)-------------------*/
   /*
    *  update all cells to new format regardless of type to keep complexity
    *  down and flexibility up.  if there is no cell, simply move on.
    */
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_next      = NULL;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   int         x_count     = 0;
   /*---(defenses)-----------------------*/
   if (strchr (sv_formats, a_format)  == 0) return -1;
   /*---(prepare)------------------------*/
   x_next  = VISU_first (&x_tab, &x_col, &x_row);
   /*> if (a_format == '"')  a_format = x_next->f;                                    <*/
   /*---(process range)------------------*/
   do {
      if (x_next != NULL) {
         if (x_next->a != '+') {
            if (a_mode == CHG_INPUT) {
               if (x_count == 0)  HIST_format ("format", x_tab, x_col, x_row, x_next->f, a_format);
               else               HIST_format ("FORMAT", x_tab, x_col, x_row, x_next->f, a_format);
            }
            if      (strchr (G_CELL_STR , x_next->t) != 0 && strchr (sv_fillers, a_format) != NULL)
               x_next->f = a_format;
            else if (strchr (G_CELL_NUM , x_next->t) != 0 && strchr (sv_fillers, a_format) == NULL)
               x_next->f = a_format;
            ++x_count;
         }
         CELL_printable (x_next);
      }
      x_next  = VISU_next (&x_tab, &x_col, &x_row);
   } while (x_next != DONE_DONE);
   /*---(complete)-----------------------*/
   return 0;
}

char               /* PURPOSE : change horizontal alignment for cells --------*/
CELL_align_valid   (char a_align)
{
   if (strchr (sv_align, a_align) == 0)   return '?';
   return a_align;
}

char               /* PURPOSE : change horizontal alignment for cells --------*/
CELL_align         (char a_mode, char a_align)
{
   /*---(design notes)-------------------*/
   /*
    *  update all cells to new alignment regardless of type to keep complexity
    *  down and flexibility up.  if there is no cell, simply move on.
    */
   /*---(locals)-----------+-----------+-*/
   char        x_align     = ' ';
   tCELL      *x_next      = NULL;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   int         x_count     = 0;
   /*---(defense: bad alignment code)----*/
   if (strchr (sv_align, a_align) == 0) return -1;
   /*---(prepare)------------------------*/
   x_next  = VISU_first (&x_tab, &x_col, &x_row);
   if (a_align == '"')  a_align = x_next->a;
   /*---(process all cells in range)-----*/
   do {
      x_align = a_align;
      if (x_next != NULL) {
         /*> if ((x_align == '}' || x_align == '{') &&                                <* 
          *>       (x_next->t != 'n' && x_next->t != 'f')) x_align = x_next->a;       <*/
         if (x_next->a != '+') {
            if (a_mode == CHG_INPUT) {
               if (x_count == 0) HIST_format ("align", x_tab, x_col, x_row, x_next->a, x_align);
               else              HIST_format ("ALIGN", x_tab, x_col, x_row, x_next->a, x_align);
            }
            x_next->a = x_align;
            ++x_count;
         }
         CELL_printable (x_next);
      }
      x_next  = VISU_next (&x_tab, &x_col, &x_row);
   } while (x_next != DONE_DONE);
   /*---(complete)---------------------------*/
   return 0;
}

char               /* PURPOSE : change horizontal alignment for cells --------*/
CELL_decimals_valid  (char a_decs)
{
   if (strchr ("0123456789\"", a_decs) == 0)   return '0';
   return a_decs;
}

char               /* PURPOSE : change decimal places shown for cells --------*/
CELL_decimals      (char a_mode, char a_num)
{
   /*---(design notes)-------------------*/
   /*
    *  update all cells to new format regardless of type to keep complexity
    *  down and flexibility up.  if there is no cell, simply move on.
    */
   /*---(locals)-----------+-----------+-*/
   char        x_decs      = 0;
   tCELL      *x_next      = NULL;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   int         x_count     = 0;
   /*---(defenses)---------------------------*/
   char     *valid = "0123456789\"";
   if (strchr(valid, a_num)   == 0) return -1;
   /*---(process range)----------------------*/
   x_next  = VISU_first (&x_tab, &x_col, &x_row);
   x_decs  = a_num;
   if (x_decs < '0') x_decs = '0';
   if (x_decs > '9') x_decs = '9';
   if (a_num  == '"')  a_num  = x_next->d;
   do {
      if (x_next != NULL) {
         /*---(update)-----------------------*/
         if (a_mode == CHG_INPUT) {
            if (x_count == 0)  HIST_format ("decimals", x_tab, x_col, x_row, x_next->d, x_decs);
            else               HIST_format ("DECIMALS", x_tab, x_col, x_row, x_next->d, x_decs);
         }
         x_next->d = x_decs;
         ++x_count;
         CELL_printable (x_next);
      }
      x_next  = VISU_next (&x_tab, &x_col, &x_row);
   } while (x_next != DONE_DONE);
   /*---(complete)---------------------------*/
   return 0;
}

char       /*----: change the width of selected columns ----------------------*/
CELL_width         (char a_mode, char a_num)
{  /*---(design notes)-------------------*/
   /*  update all cells to new width, either a standard size, or a specific   */
   /*  value communicated as a negative number.                               */
   /*---(locals)-----------+-----------+-*/
   /*------(positions)-----*/
   int         x_tab       = 0;
   int         x_bcol      = 0;
   int         x_ecol      = 0;
   /*------(objects)-------*/
   tCELL      *x_cell      = NULL;      /* cell for updating                  */
   /*------(working)-------*/
   int         x_col       = 0;
   int         x_row       = 0;
   int         x_last      = 0;
   int         x_width     = 0;
   /*---(header)-----------------------------*/
   DEBUG_CELL  yLOG_enter  (__FUNCTION__);
   /*---(defenses)---------------------------*/
   char     *valid = "mnNwWhHlL";
   if (a_num >   0 && strchr(valid, a_num)   == 0) {
      DEBUG_CELL  yLOG_exit   (__FUNCTION__);
      return -1;
   }
   /*---(process range)----------------------*/
   VISU_range (&x_tab, &x_bcol, NULL, &x_ecol, NULL);
   x_last = LOC_row_max (x_tab);
   for (x_col = x_bcol; x_col <= x_ecol; ++x_col) {
      DEBUG_CELL  yLOG_complex ("position"  , "tab=%3d, col=%3d", x_tab, x_col);
      /*---(adjust)----------------------*/
      if (a_num <   0) {
         x_width                = -(a_num);
      } else {
         x_width = LOC_col_width (x_tab, x_col);
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
      LOC_col_widen  (x_tab, x_col, x_width);
      /*---(update column printables)----*/
      for (x_row = 0; x_row < x_last; ++x_row) {
         x_cell = LOC_cell_at_loc (x_tab, x_col, x_row);
         if (x_cell == NULL) continue;
         /*---(update merged cells)----------*/
         if (x_cell->t == CTYPE_MERGE)  SEQ_calc_up (x_cell);
         /*---(update printable)-------------*/
         CELL_printable (x_cell);
      }
   }
   /*---(reset headers)---------------*/
   KEYS_bcol    (BCOL);
   CURS_col_head ();
   /*---(complete)---------------------------*/
   DEBUG_CELL  yLOG_exit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        final display                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___DISPLAY_________o () { return; }

char        s_print     [LEN_RECD] = "";

char
CELL_macro_get       (char *a_macro)
{
   char        rce         =  -10;
   short       x_row       =    0;
   tCELL      *x_curr      = NULL;
   --rce;  if (a_macro   == NULL     )                      return rce;
   --rce;  if (my.macro_name < 'a' || my.macro_name > 'z')  return rce;
   x_row = my.macro_name - 'a';
   x_curr = LOC_cell_at_loc ( 35, 1, x_row);
   --rce;  if (x_curr    == NULL)                           return rce;
   if (x_curr->t == CTYPE_STR) {
      strlcpy (a_macro, x_curr->s    , LEN_RECD);
      return 0;
   }
   if (x_curr->t == CTYPE_MOD) {
      strlcpy (a_macro, x_curr->v_str, LEN_RECD);
      return 0;
   }
   --rce;
   return rce;
}

char
CELL_macro_set       (char *a_macro)
{
   char        rce         =  -10;
   short       x_row       =    0;
   char        t           [LEN_LABEL]   = "";
   int         x_len       =    0;
   tCELL      *x_curr      = NULL;
   --rce;  if (a_macro   == NULL     )                      return rce;
   --rce;  if (my.macro_name < 'a' || my.macro_name > 'z')  return rce;
   x_row = my.macro_name - 'a';
   x_len = strlen (a_macro);
   if (x_len == 0) {
      CELL_delete (CHG_NOHIST  , 35, 0, x_row);
      CELL_delete (CHG_NOHIST  , 35, 1, x_row);
   } else {
      sprintf (t, "%c", my.macro_name);
      CELL_change (NULL, CHG_NOHIST, 35, 0, x_row, t      );
      CELL_change (NULL, CHG_NOHIST, 35, 1, x_row, a_macro);
   }
   return 0;
}

char         /*--> determine full print width ------------[ ------ [ ------ ]-*/
CELL_print_width     (tCELL *a_curr, int *a_width, int *a_merge)
{
   /*---(locals)-------------------------*/
   tCELL      *x_curr      = NULL;
   int         i           = 0;
   /*---(initialize)---------------------*/
   *a_merge = 0;
   *a_width = LOC_col_width (a_curr->tab, a_curr->col);
   /*---(look for mergse)----------------*/
   for (i = a_curr->col + 1; i < LOC_col_max (a_curr->tab); ++i) {
      x_curr = LOC_cell_at_loc (a_curr->tab, i, a_curr->row);
      if (x_curr    == NULL)         break;
      if (x_curr->t != CTYPE_MERGE)  break;
      *a_width += LOC_col_width (a_curr->tab, i);
      ++(*a_merge);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> parse print into merged cells ---------[ ------ [ ------ ]-*/
CELL_print_parse     (tCELL *a_curr, char *p, int a_merge)
{
   /*---(locals)-------------------------*/
   tCELL      *x_curr      = NULL;
   int         w           = 0;             /* available printing width       */
   int         wa          = 0;             /* adjusted width                 */
   int         i           = 0;
   char       *pp          = NULL;
   for (i = 0; i <= a_merge; ++i) {
      w     = LOC_col_width (a_curr->tab, a_curr->col + i);
      DEBUG_CELL  yLOG_value ("#w", w);
      while (pp == NULL)  pp = (char*) malloc(w + 1);
      sprintf (pp, "%-*.*s", w, w, p + wa);
      DEBUG_CELL  yLOG_info  ("#1p", pp);
      x_curr = LOC_cell_at_loc (a_curr->tab, a_curr->col + i, a_curr->row);
      if (x_curr->p != NULL) {
         free (x_curr->p);
         x_curr->p = NULL;
      }
      x_curr->p = pp;
      pp    = NULL;
      wa   += w;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char               /* PURPOSE : create a curses printable image of the cell --*/
CELL_printable     (tCELL *a_curr) {
   /*---(locals)-------------------------*/
   char        rce         =  -10;
   char        rc          =    0;
   int         len         = 0;             /* string length                  */
   int         w           = 0;             /* available printing width       */
   int         wa          = 0;             /* adjusted width                 */
   char        x_temp      [LEN_RECD] = "";  /* temp working string            */
   char        x_work      [LEN_RECD] = "";  /* temp working string            */
   char       *p           = NULL;          /* final printable string         */
   int         x_merge     = 0;             /* merged cells to right          */
   /*---(defense)------------------------*/
   --rce;  if (a_curr    == NULL) return rce;     /* cell does not exist                */
   --rce;  if (a_curr->s == NULL) return rce;     /* nothing to do without source       */
   /*---(header)-------------------------*/
   DEBUG_CELL  yLOG_enter (__FUNCTION__);
   /*---(prepare)------------------------*/
   char x_type   = a_curr->t;
   char x_format = a_curr->f;
   char x_align  = a_curr->a;
   char x_decs   = a_curr->d - '0';
   DEBUG_CELL  yLOG_info  ("label"     , a_curr->label);
   DEBUG_CELL  yLOG_char  ("type"      , x_type);
   DEBUG_CELL  yLOG_char  ("format"    , x_format);
   DEBUG_CELL  yLOG_char  ("align"     , x_align);
   DEBUG_CELL  yLOG_value ("decs"      , x_decs);
   /*---(check for hidden)---------------*/
   if (x_type == CTYPE_MERGE) {
      DEBUG_CELL  yLOG_note  ("merged cell");
      DEBUG_CELL  yLOG_exit  (__FUNCTION__);
      return 0;
   }
   /*---(numbers)------------------------*/
   --rce;
   if (strchr (G_CELL_NUM, x_type) != 0) {
      DEBUG_CELL  yLOG_note  ("number");
      rc = strl4main   (a_curr->v_num, x_temp, x_decs, x_format, LEN_RECD);
   }
   /*---(calced tsrings------------------*/
   else if (strchr (G_CELL_STR, x_type) != 0) {
      DEBUG_CELL  yLOG_note  ("string");
      if      (x_type == CTYPE_STR)   strcat (x_temp, a_curr->s);
      else if (x_type == CTYPE_MOD)   strcat (x_temp, a_curr->v_str);
      else if (x_type == CTYPE_MLIKE) strcat (x_temp, a_curr->v_str);
      else                            strcat (x_temp, "");
   }
   /*---(empty)--------------------------*/
   else if (x_type == CTYPE_BLANK) {
      DEBUG_CELL  yLOG_note  ("empty");
      strcat (x_temp, "-");
   }
   /*---(troubles)-----------------------*/
   else if (strchr(G_CELL_ERR, x_type) != 0) {
      DEBUG_CELL  yLOG_note  ("error");
      /*> strcat (x_temp, a_curr->s);                                                 <*/
      strcat (x_temp, a_curr->v_str);
      x_align = '<';
   }
   /*---(detault)-----------------------*/
   else {
      DEBUG_CELL  yLOG_note  ("other");
      strcat (x_temp, a_curr->s);
   }
   /*---(formatting errors)--------------*/
   if (strncmp (x_temp, "#.", 2) == 0) {
      x_type  = 'w';
      x_align = '<';
   }
   /*---(indented formats)---------------*/
   DEBUG_CELL  yLOG_info  ("x", x_temp);
   /*---(get width)----------------------*/
   CELL_print_width (a_curr, &w, &x_merge);
   DEBUG_CELL  yLOG_value ("w", w);
   wa    = w - 1;
   if (strchr (G_CELL_NUM, x_type) != 0)
      strlpad (x_temp, x_work, ' '      , x_align, wa);
   else
      strlpad (x_temp, x_work, x_format, x_align, wa);
   /*---(prepare)------*/
   if (a_curr->p != NULL) {
      free (a_curr->p);
      a_curr->p = NULL;
   }
   while (p == NULL)  p = (char*) malloc(w + 1);
   sprintf (p, "%s ", x_work);
   DEBUG_CELL  yLOG_value ("x_merge", x_merge);
   DEBUG_CELL  yLOG_info  ("p"      , p);
   if (x_merge == 0) {
      a_curr->p = p;
   } else {
      CELL_print_parse (a_curr, p, x_merge);
      free (p);
      p = NULL;
   }
   /*---(ending)-------------------------*/
   DEBUG_CELL   yLOG_exit   (__FUNCTION__);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNIT_TEST_______o () { return; }

char*        /*> unit test accessor ----------------------[ ------ [ ------ ]-*/
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

char*        /*--> unit testing accessor -----------------[--------[--------]-*/
CELL__unitnew      (
      /*----------+--------------+--------------------------------------------*/
      char       *a_question,    /* text string identifying the question      */
      char       *a_label)       /* label of cell that has the answer         */
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
      rc     = LOC_legal (x_tab, x_col, x_row, CELL_FIXED);
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
   if (strcmp(a_question, "cell_info")      == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_UNIT, "s_celln info     : --- --- --- --- ----- ----- -----");
      else                             snprintf(unit_answer, LEN_UNIT, "s_celln info     : t=%c f=%c d=%c a=%c c=%3d r=%3d d=%3d", x_cell->t, x_cell->f, x_cell->d, x_cell->a, x_cell->nrpn, x_cell->nrequire, x_cell->nprovide);
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
      if      (x_cell       == NULL)   snprintf(unit_answer, LEN_UNIT, "s_celln rpn      : (----) -");
      else if (x_cell->nrpn == 0)      snprintf(unit_answer, LEN_UNIT, "s_celln rpn      : (%4d) ."     , x_cell->nrpn);
      else                             snprintf(unit_answer, LEN_UNIT, "s_celln rpn      : (%4d) %s"    , x_cell->nrpn, x_cell->rpn);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}

char        /*----: change te contents of a cell ------------------------------*/
CELL_unitchange      (tCELL *a_cell, char *a_source)
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
