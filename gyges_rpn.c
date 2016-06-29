/*============================----beg-of-source---============================*/

/*===[[ MODULE ]]=============================================================*

 *   application   : s
 *   module        : s_rpn
 *   purpose       : provide all code for converting infix formulas to postfix
 *
 */
/*===[[ EXPLANATION ]]========================================================*

 *   while normal formula notation (infix) is intuitive for the user, it is
 *   unnatural, slow, and cumbersome for a computer.  given the difficulty in
 *   getting humans to think and communicate in any other notation, it is best
 *   to simply take formulas in this way.
 *
 *   but, we are talking about then asking the computer to recalculate the
 *   formulas at wicked fast speeds which can not be sustained for large volumes
 *   of cells/data.
 *
 *   so, they key is to find the method which optimizes the computers usage and
 *   convert the user-entered infix notation only when the formula is initially
 *   entered or each time it is updated later.
 *
 *   given this is a well trod topic in computer science circles, the answer is
 *   generally accepted to be using postfix notation, also known as reverse
 *   polish notation, which can be executed with a simple stack data structure
 *   and little compexity.
 *
 *   therefore, the purpose of this module is to implement a clean, specific
 *   algorithm for conversion from infix to RPN notation.  this algorithm is
 *   also well studied and is called the "shunting yard algorithm" -- named
 *   after an area used by railroads to rearrange train cars efficiently.  this
 *   algorithm was designed by the one-and-only edsgar dijjistra.
 *
 */



/*---[[ global header ]]----------------------------------*/
#include   "gyges.h"





/*---[[ globally shared ]]--------------------------------*/
char     *g_alpha     = "$abcdefghijklmnopqrstuvwxyz";
char     *g_rcnum     = "-0123456789";
char     *g_rcops     = "|&=!<>*/%+-():;";




/*---[[ private variables ]]------------------------------*/
/*---(copy of source)-------*/
PRIV   char      rpn__working[MAX_STR];
/*---(rpn output)-----------*/
PRIV   int       rpn__offcol   = 0;
PRIV   int       rpn__offrow   = 0;

char        s_final     [MAX_STR];



/*====================------------------------------------====================*/
/*===----                     formula modification                     ----===*/
/*====================------------------------------------====================*/
PRIV void  o___MODIFY__________o () { return; }

char         /*--> adjust addresses based on cell moves --[ ------ [ ------ ]-*/
RPN_adjust         (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* source cell                                  */
      int         a_toff,     /* tab offset from original                     */
      int         a_coff,     /* col offset from original                     */
      int         a_roff,     /* row offset from original                     */
      char       *a_final)    /* updated source formula (uncompressed)        */
{  /*---(design notes)--------------------------------------------------------*/
   /*
    *   there are a number of reasons to modify a cell formula, including,
    *   cell moves and basic clean-up of user formatted source text.  this
    *   function will provide a single testable and maintainable method for
    *   performing these functions.
    */
   /*---(locals)-----------+-----------+-*/
   char       *x_tokens    = NULL;               /* source made into tokens   */
   char        x_final     [MAX_STR]   = "";     /* new version of formula    */
   char       *p           = NULL;               /* strtok field pointer      */
   char       *q           = " ";                /* strtok delimiters         */
   char       *r           = NULL;               /* strtok context            */
   char        rc          = 0;                  /* generic return code       */
   char        rce         = -10;                /* return code for errors    */
   int         x_tab       = 0;                  /* tab of address            */
   int         x_col       = 0;                  /* col of address            */
   int         x_row       = 0;                  /* row of address            */
   char        x_abs       = 0;                  /* absolute address markers  */
   char        x_addr      [20]        = "";     /* new address               */
   /*---(begin)--------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   DEBUG_RPN    yLOG_point   ("a_cell"    , a_cell);
   DEBUG_RPN    yLOG_value   ("a_toff"    , a_toff);
   DEBUG_RPN    yLOG_value   ("a_coff"    , a_coff);
   DEBUG_RPN    yLOG_value   ("a_roff"    , a_roff);
   DEBUG_RPN    yLOG_value   ("a_final"   , a_final);
   /*---(defense)------------------------*/
   strcpy (s_final, "n/a");
   --rce;  if (a_final   == NULL)  {
      DEBUG_RPN    yLOG_note    ("aborted, a_final is NULL, no point");
      DEBUG_RPN    yLOG_exit    (__FUNCTION__);
      return rce;
   }
   strcpy (a_final, "n/a");
   --rce;  if (a_cell    == NULL)  {
      DEBUG_RPN    yLOG_note    ("aborted, a_cell is NULL");
      DEBUG_RPN    yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_RPN    yLOG_point   ("a_cell->s" , a_cell->s);
   --rce;  if (a_cell->s == NULL)  {
      DEBUG_RPN    yLOG_note    ("aborted, a_cell source is NULL");
      DEBUG_RPN    yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_RPN    yLOG_info    ("source"    , a_cell->s);
   DEBUG_RPN    yLOG_point   ("a_cell->l" , a_cell->l);
   --rce;  if (a_cell->l <  2   )  {
      DEBUG_RPN    yLOG_note    ("aborted, a_cell source is too short");
      DEBUG_RPN    yLOG_exit    (__FUNCTION__);
      return rce;
   }
   strcpy (a_final, a_cell->s);
   strcpy (s_final, a_cell->s);
   DEBUG_RPN    yLOG_char    ("a_cell->t" , a_cell->t);
   --rce;  if (a_cell->t != 'f')   {
      DEBUG_RPN    yLOG_note    ("aborted, a_cell type is not formula");
      DEBUG_RPN    yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   x_tokens = yRPN_stokens (a_cell->s);
   DEBUG_RPN    yLOG_info    ("x_tokens"  , x_tokens);
   /*---(parse first token)--------------*/
   p = strtok_r (x_tokens, q, &r);
   DEBUG_RPN    yLOG_point   ("p"         , p);
   if (p == NULL) {
      DEBUG_RPN    yLOG_note    ("aborted, very first parse returned NULL");
      DEBUG_RPN    yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(loop through tokens)------------*/
   while (p != NULL) {
      /*---(display token)---------------*/
      DEBUG_RPN    yLOG_info    ("p"         , p);
      /*---(parse for address)-----------*/
      rc = LOC_parse (p, &x_tab, &x_col, &x_row, &x_abs);
      DEBUG_RPN    yLOG_value   ("parse rc"  , rc);
      /*---(handle addresses)------------*/
      if (rc >= 0) {
         /*---(deal with absolutes)------*/
         DEBUG_RPN    yLOG_value   ("x_abs"     , x_abs);
         switch (x_abs) {
         case 0 : x_tab += a_toff; x_col += a_coff; x_row += a_roff;  break;
         case 1 : x_tab += a_toff;                  x_row += a_roff;  break;
         case 2 : x_tab += a_toff; x_col += a_coff;                   break;
         case 3 : x_tab += a_toff;                                    break;
         case 4 :                  x_col += a_coff; x_row += a_roff;  break;
         case 5 :                                   x_row += a_roff;  break;
         case 6 :                  x_col += a_coff;                   break;
         default:                                                     break;
         }
         /*---(form new address)---------*/
         rc = LOC_ref   (x_tab, x_col, x_row, x_abs, x_addr);
         DEBUG_RPN    yLOG_value   ("loc rc"    , rc);
         if (rc == 0) {
            DEBUG_RPN    yLOG_info    ("new label" , x_addr);
            strcat (x_final, x_addr);
         } else if (rc >  0) {
            DEBUG_RPN    yLOG_note    ("trouble, address cant adjust that far");
            strcat (x_final, "#REF");
         } else {
            DEBUG_RPN    yLOG_note    ("trouble, address cant adjust that far");
            strcat (x_final, "#REF");
         }
      }
      /*---(handle non-addresses)--------*/
      else {
         DEBUG_RPN    yLOG_note    ("not an address, just append");
         strcat (x_final, p);
      }
      strcat (x_final, " ");
      /*---(get next token)--------------*/
      p = strtok_r (NULL    , q, &r);
      DEBUG_RPN    yLOG_point   ("p"         , p);
   }
   /*---(strip final space)--------------*/
   x_final [strlen (x_final) - 1] = '\0';
   /*---(wrap-up)------------------------*/
   DEBUG_RPN    yLOG_info    ("final"     , x_final);
   strcpy (s_final, x_final);
   if (x_final != NULL)  strcpy (a_final, x_final);
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   /*---(complete)-----------------------*/
   return 0;
}

char               /* PURPOSE : convert normal infix notation to postfix/rpn -*/
RPN_makelike       (tCELL *a_curr, char *a_label)
{
   int      xtab, xcol, xrow;
   tCELL   *dest   = NULL;
   LOC_parse (a_label, &xtab, &xcol, &xrow, NULL);
   if (tab->sheet[xcol][xrow] == NULL || tab->sheet[xcol][xrow]->s == NULL) {
      rpn__offcol = 0;
      rpn__offrow = 0;
      strncpy(rpn__working, "", MAX_STR);
   } else {
      rpn__offcol = a_curr->col - xcol;
      rpn__offrow = a_curr->row - xrow;
      strncpy(rpn__working, tab->sheet[xcol][xrow]->s, MAX_STR);
   }
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          main driver                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___DRIVER__________o () { return; }

char         /*--> convert infix notation to rpn ---------[ ------ [ ------ ]-*/
RPN_convert        (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_curr)     /* cell whose formula is to be converted        */
{  /*---(design notes)--------------------------------------------------------*
    *   convert from normal infix to the more efficient RPN/postfix notation
    *   and at the same time, identify any critical calculation errors
    *      - unbalanced parens
    *      - unknown operator (+-, =+, etc)
    *      - unknown variable/reference name
    *      - unknown function
    *   all presedence should match ansi-c to keep it clear and clean for me
    */
   /*---(locals)-----------+-------------+-*/
   int         i           = 1;
   int         len         = 0;
   char        x_work      [MAX_STR]     = "";
   char       *x_rpn       = NULL;
   char        x_rpn2      [MAX_STR]     = "";
   int         x_ntoken;
   char        rce         = -10;
   char        ch          = ' ';
   char       *x_tokens    = NULL;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   char        rc          = 0;
   char        x_temp      [MAX_STR]     = "";
   tCELL      *x_like      = NULL;
   /*---(begin)----------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   DEBUG_RPN    yLOG_point   ("a_curr"    , a_curr);
   /*---(defenses)-----------------------*/
   --rce;  if (a_curr       == NULL) {
      DEBUG_RPN    yLOG_note    ("passed cell does not exist");
      DEBUG_RPN    yLOG_exit    (__FUNCTION__);
      return rce;  /* no cell to convert   */
   }
   DEBUG_RPN    yLOG_info    ("a_curr"    , a_curr->label);
   DEBUG_RPN    yLOG_point   ("a_curr->s" , a_curr->s);
   --rce;  if (a_curr->s    == NULL) {
      DEBUG_RPN    yLOG_note    ("source string of passed cell is null");
      DEBUG_RPN    yLOG_exit    (__FUNCTION__);
      return rce;  /* no source to convert */
   }
   DEBUG_RPN    yLOG_info    ("a_curr->s" , a_curr->s);
   DEBUG_RPN    yLOG_char    ("a_curr->t" , a_curr->t);
   --rce;  if (strchr ("fapml", a_curr->t) == 0) {
      DEBUG_RPN    yLOG_note    ("passed cell not a formula");
      DEBUG_RPN    yLOG_exit    (__FUNCTION__);
      return rce;  /* not a formula        */
   }
   ch = a_curr->s[0];
   DEBUG_RPN    yLOG_value   ("prefix"    , ch);
   --rce;  if (ch == '\0') {
      DEBUG_RPN    yLOG_note    ("source string in blank");
      DEBUG_RPN    yLOG_exit    (__FUNCTION__);
      return rce;  /* empty                */
   }
   DEBUG_RPN    yLOG_char    ("prefix"    , ch);
   --rce;  if (strchr ("=&~!#", ch) == 0) {
      DEBUG_RPN    yLOG_note    ("prefix not in =&~!#");
      DEBUG_RPN    yLOG_exit    (__FUNCTION__);
      return rce;  /* not a formula        */
   }
   /*---(clear existing rpn)-------------*/
   if (a_curr->rpn  != NULL)  free (a_curr->rpn);
   a_curr->rpn  = NULL;
   a_curr->nrpn = 0;
   /*---(convert)------------------------*/
   strncpy (x_work, a_curr->s, MAX_STR);
   x_work [0] = '=';
   if (ch == '~') {
      DEBUG_RPN    yLOG_note    ("processing a like formula");
      rc = LOC_parse (x_work + 1, &x_tab, &x_col, &x_row, NULL);
      DEBUG_RPN    yLOG_value   ("parse rc"  , rc);
      --rce;
      if (rc < 0) {
         DEBUG_RPN    yLOG_exit    (__FUNCTION__);
         return rce;
      }
      x_like = LOC_cell (x_tab, x_col, x_row);
      DEBUG_RPN    yLOG_point   ("x_like"    , x_like);
      --rce;
      if (x_like == NULL) {
         DEBUG_RPN    yLOG_exit    (__FUNCTION__);
         return rce;
      }
      strncpy (x_work, x_like->s, MAX_STR);
      ch = x_work[0];
      rc = RPN_adjust (x_like, a_curr->tab - x_tab, a_curr->col - x_col, a_curr->row - x_row, x_temp);
      DEBUG_RPN    yLOG_value   ("adjust rc" , rc);
      --rce;
      if (rc < 0) {
         DEBUG_RPN    yLOG_exit    (__FUNCTION__);
         return rce;
      }
      strncpy (x_work, x_temp, MAX_STR);
   }
   x_rpn = yRPN_spreadsheet (x_work, &x_ntoken);
   --rce;  if (x_rpn == NULL)            return rce;
   DEBUG_RPN    yLOG_point   ("x_rpn"     , x_rpn);
   DEBUG_RPN    yLOG_info    ("x_rpn"     , x_rpn);
   /*---(clean-up)-----------------------*/
   if (ch == '#')   strcpy  (x_rpn2, "#,");
   else             strcpy  (x_rpn2, "=,");
   strcat (x_rpn2, x_rpn);
   len = strlen (x_rpn2);
   x_rpn2 [len - 2] = '\0';
   /*---(put into cell)------------------*/
   a_curr->rpn   = strndup (x_rpn2, MAX_STR);
   a_curr->nrpn  = x_ntoken;
   /*> x_tokens = yRPN_tokens (a_curr->s);                                            <* 
    *> DEBUG_RPN    yLOG_point   ("x_tokens"  , x_tokens);                            <* 
    *> DEBUG_RPN    yLOG_info    ("x_tokens"  , x_tokens);                            <*/
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNIT_TEST_______o () { return; }

char*
RPN_unit           (char *a_question, void *a_curr)
{
   /*---(locals)-----------+-----------+-*/
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_rpn            : question not understood");
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "rpn_updated" )   == 0) {
      snprintf(unit_answer, LEN_TEXT, "s_rpn updated    : %-.45s", s_final);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



/*============================----end-of-source---============================*/
