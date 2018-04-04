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
static char   s_final  [LEN_RECD];
static short  s_ttab   = 0;    /* target tab                                  */
static short  s_tcol   = 0;    /* target col                                  */
static short  s_trow   = 0;    /* target row                                  */
static short  s_atab   = 0;    /* adjust tab                                  */
static short  s_acol   = 0;    /* adjust col                                  */
static short  s_arow   = 0;    /* adjust row                                  */



/*====================------------------------------------====================*/
/*===----                     formula modification                     ----===*/
/*====================------------------------------------====================*/
PRIV void  o___MODIFY__________o () { return; }

char         /*-> shared argument validiation --------[ ------ [fe.G67.55#.92]*/ /*-[01.0000.018.!]-*/ /*-[--.---.---.--]-*/
RPN__check_args      (tCELL *a_cell, char a_scope, char *a_target, char *a_final, int a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;                /* return code for errors    */
   char        rc          =    0;                  /* generic return code       */
   short       x_tab       =    0;
   short       x_col       =    0;
   short       x_row       =    0;
   /*---(begin)--------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   DEBUG_RPN    yLOG_point   ("a_cell"    , a_cell);
   DEBUG_RPN    yLOG_char    ("a_scope"   , a_scope);
   DEBUG_RPN    yLOG_point   ("a_target"  , a_target);
   DEBUG_RPN    yLOG_point   ("a_final"   , a_final);
   /*---(prepare)------------------------*/
   strcpy (s_final, "n/a");
   --rce;  if (a_final   == NULL)  {
      DEBUG_RPN    yLOG_note    ("aborted, a_final is NULL, no point");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strcpy (a_final, "n/a");
   --rce;  if (a_target   == NULL)  {
      DEBUG_RPN    yLOG_note    ("aborted, a_target is NULL, no point");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_cell    == NULL)  {
      DEBUG_RPN    yLOG_note    ("aborted, a_cell is NULL");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_RPN    yLOG_point   ("a_cell->s" , a_cell->s);
   --rce;  if (a_cell->s == NULL)  {
      DEBUG_RPN    yLOG_note    ("aborted, a_cell source is NULL");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_RPN    yLOG_info    ("source"    , a_cell->s);
   DEBUG_RPN    yLOG_value   ("a_cell->l" , a_cell->l);
   --rce;  if (a_cell->l <  2   )  {
      DEBUG_RPN    yLOG_note    ("aborted, a_cell source is too short");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_RPN    yLOG_char    ("a_cell->t" , a_cell->t);
   --rce;  if (strchr (G_CELL_RPN , a_cell->t) == 0)   {
      DEBUG_RPN    yLOG_note    ("aborted, a_cell type is not formula");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_RPN    yLOG_char    ("a_scope"   , a_scope);
   --rce;  if (strchr (G_RPN_ALL, a_scope) == 0)   {
      DEBUG_RPN    yLOG_note    ("aborted, a_scope not legal");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = LOC_parse (a_cell->label, &x_tab, &x_col, &x_row, NULL);
   DEBUG_RPN    yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0)  {
      DEBUG_RPN    yLOG_note    ("can not parse original cell label");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*> rc = REG_inside (a_index, x_tab, x_col, x_row);                                <* 
    *> --rce;  if (strchr ("ib", a_scope) != NULL && rc <= 0) {                       <* 
    *>    DEBUG_RPN    yLOG_note    ("cell not inside valid register bounds");        <* 
    *>    DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <*/
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> change a specific reference --------[ ------ [fe.HA4.498.A3]*/ /*-[02.0000.018.!]-*/ /*-[--.---.---.--]-*/
RPN__adjust_one      (char *a_old, char a_scope, int a_index, char *a_new)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;               /* generic return code       */
   int         x_tab       =    0;               /* tab of new address        */
   int         x_col       =    0;               /* col of new address        */
   int         x_row       =    0;               /* row of new address        */
   char        x_abs       =    0;               /* abs of new address        */
   char        x_addr      [20]        = "";     /* new address               */
   char        x_nabs      =    0;               /* technical abs of address  */
   int         x_bad       =    0;
   /*---(begin)--------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   DEBUG_RPN    yLOG_info    ("a_old"     , a_old);
   DEBUG_RPN    yLOG_char    ("a_scope"   , a_scope);
   DEBUG_RPN    yLOG_value   ("a_index"   , a_index);
   /*---(prepare)---------------------*/
   strcpy (a_new, a_old);
   /*---(check for do-nothing)--------*/
   --rce;  if (strchr ("nibra", a_scope) == NULL) {
      DEBUG_RPN    yLOG_note    ("scope indicates nothing to do");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(parse for address)-----------*/
   rc = LOC_parse (a_old, &x_tab, &x_col, &x_row, &x_abs);
   DEBUG_RPN    yLOG_value   ("parse rc"  , rc);
   --rce;  if (rc <  0) {
      DEBUG_RPN    yLOG_note    ("not an address, just append");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_nabs = x_abs;
   /*---(check targeted refs)---------*/
   --rce;  if (strchr (G_RPN_PROS, a_scope) != NULL) {
      if (s_ttab != x_tab || s_tcol != x_col || s_trow != x_row) {
         DEBUG_RPN    yLOG_note    ("tab, col, or row does not match, just append");
         DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(adjust to scopes)------------*/
   rc = REG_inside (a_index, x_tab, x_col, x_row);
   --rce;  if (strchr (G_RPN_INSIDE, a_scope) != NULL) {
      if (a_scope == G_RPN_INNER && rc <= 0) {
         DEBUG_RPN    yLOG_note    ("cell label not inner register area");
         DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      if (rc > 0)  x_nabs = 0;
   }
   if (a_scope == G_RPN_PALL)    x_nabs = 0;
   /*---(handle addresses)------------*/
   DEBUG_RPN    yLOG_value   ("x_nabs"    , x_nabs);
   switch (x_nabs) {
   case  0  : x_tab += s_atab; x_col += s_acol; x_row += s_arow;  break;
   case  1  : x_tab += s_atab;                  x_row += s_arow;  break;
   case  2  : x_tab += s_atab; x_col += s_acol;                   break;
   case  3  : x_tab += s_atab;                                    break;
   case  4  :                  x_col += s_acol; x_row += s_arow;  break;
   case  5  :                                   x_row += s_arow;  break;
   case  6  :                  x_col += s_acol;                   break;
   default  : break;
   }
   /*---(form new address)---------*/
   rc = LOC_ref   (x_tab, x_col, x_row, x_abs, x_addr);
   DEBUG_RPN    yLOG_value   ("loc rc"    , rc);
   if (rc != 0) {
      strcpy (a_new, "#REF");
      DEBUG_RPN    yLOG_note    ("#REF, address cant adjust that far");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_RPN    yLOG_info    ("new label" , x_addr);
   strcpy (a_new, x_addr);
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> change a specific reference --------[ ------ [fe.J75.197.84]*/ /*-[02.0000.037.!]-*/ /*-[--.---.---.--]-*/
RPN__adjust_main     (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* source cell                                  */
      char        a_scope,    /* what kind of changes                         */
      int         a_atab,     /* tab offset from original                     */
      int         a_acol,     /* col offset from original                     */
      int         a_arow,     /* row offset from original                     */
      char       *a_final,    /* updated source formula (uncompressed)        */
      int         a_index,    /* register index                               */
      char       *a_target)   /* cell ref to be changed                       */
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;               /* generic return code       */
   char       *x_tokens    = NULL;               /* source made into tokens   */
   char        x_final     [LEN_RECD]   = "";    /* new version of formula    */
   char       *p           = NULL;               /* strtok field pointer      */
   char       *q           =  " ";               /* strtok delimiters         */
   char       *r           = NULL;               /* strtok context            */
   char        x_new       [LEN_RECD]  = "";     /* new element               */
   int         x_bad       =    0;
   /*---(begin)--------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   DEBUG_RPN    yLOG_point   ("a_cell"    , a_cell);
   DEBUG_RPN    yLOG_char    ("a_scope"   , a_scope);
   DEBUG_RPN    yLOG_value   ("a_atab"    , a_atab);
   DEBUG_RPN    yLOG_value   ("a_acol"    , a_acol);
   DEBUG_RPN    yLOG_value   ("a_arow"    , a_arow);
   DEBUG_RPN    yLOG_point   ("a_final"   , a_final);
   DEBUG_RPN    yLOG_value   ("a_index"   , a_index);
   DEBUG_RPN    yLOG_point   ("a_target"  , a_target);
   /*---(defense)------------------------*/
   rc = RPN__check_args (a_cell, a_scope, a_target, a_final, a_index);
   DEBUG_RPN    yLOG_value   ("rc"        , rc);
   if (rc < 0)  {
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   rce -= 10;
   /*---(target cell)--------------------*/
   s_ttab = s_tcol = s_trow = -1;
   if (a_target [0] != '\0') {
      DEBUG_RPN    yLOG_note    ("analyze the target");
      rc = LOC_parse (a_target, &s_ttab, &s_tcol, &s_trow, NULL);
      DEBUG_RPN    yLOG_value   ("rc"        , rc);
      --rce;  if (rc < 0)  {
         DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(adjustment)---------------------*/
   DEBUG_RPN    yLOG_note    ("save the adjustment");
   s_atab = a_atab;
   s_acol = a_acol;
   s_arow = a_arow;
   /*---(prepare tokens)-----------------*/
   x_tokens = yRPN_stokens (a_cell->s);
   DEBUG_RPN    yLOG_info    ("x_tokens"  , x_tokens);
   /*---(parse first token)--------------*/
   p = strtok_r (x_tokens, q, &r);
   DEBUG_RPN    yLOG_point   ("p"         , p);
   if (p == NULL) {
      DEBUG_RPN    yLOG_note    ("aborted, very first parse returned NULL");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(loop through tokens)------------*/
   while (p != NULL) {
      rc = RPN__adjust_one (p, a_scope, a_index, x_new);
      strcat (x_final, x_new);
      strcat (x_final, " ");
      DEBUG_RPN    yLOG_info    ("x_final"   , x_final);
      if (strcmp (x_new, "#REF") == 0)  ++x_bad;
      p = strtok_r (NULL    , q, &r);
   }
   /*---(strip final space)--------------*/
   x_final [strlen (x_final) - 1] = '\0';
   /*---(wrap-up)------------------------*/
   DEBUG_RPN    yLOG_info    ("final"     , x_final);
   strcpy (s_final, x_final);
   if (x_final != NULL)  strcpy (a_final, x_final);
   /*---(check for ref troubles)---------*/
   DEBUG_RPN    yLOG_value   ("x_bad"     , x_bad);
   --rce;  if (x_bad > 0) {
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> adjust addr based on cell move -----[ ------ [gc.310.101.01]*/ /*-[01.0000.016.#]-*/ /*-[--.---.---.--]-*/
RPN_adjust         (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* source cell                                  */
      int         a_atab,     /* tab adjust from original                     */
      int         a_acol,     /* col adjust from original                     */
      int         a_arow,     /* row adjust from original                     */
      char       *a_final)    /* updated source formula (uncompressed)        */
{  /*---(design notes)--------------------------------------------------------*/
   return RPN__adjust_main (a_cell, G_RPN_REL , a_atab, a_acol, a_arow, a_final, -1, "");
}

char         /*-> change a specific reference --------[ ------ [gc.410.102.11]*/ /*-[01.0000.106.#]-*/ /*-[--.---.---.--]-*/
RPN_adjust_ref     (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* source cell                                  */
      char        a_scope,    /* what kind of changes                         */
      int         a_atab,     /* tab offset from original                     */
      int         a_acol,     /* col offset from original                     */
      int         a_arow,     /* row offset from original                     */
      char       *a_final,    /* updated source formula (uncompressed)        */
      char       *a_target)   /* cell ref to be changed                       */
{
   strcpy (s_final, "n/a");
   if (strchr (G_RPN_INSIDE, a_scope) == NULL)  return -1;
   return RPN__adjust_main (a_cell, a_scope, a_atab, a_acol, a_arow, a_final, -1, a_target);
}

char         /*-> change a register cell -------------[ ------ [gc.410.102.11]*/ /*-[01.0000.106.#]-*/ /*-[--.---.---.--]-*/
RPN_adjust_reg     (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* source cell                                  */
      char        a_scope,    /* what kind of changes                         */
      int         a_atab,     /* tab offset from original                     */
      int         a_acol,     /* col offset from original                     */
      int         a_arow,     /* row offset from original                     */
      char       *a_final,    /* updated source formula (uncompressed)        */
      int         a_index)    /* register index                               */
{
   strcpy (s_final, "n/a");
   if (strchr (G_RPN_REQS  , a_scope) == NULL)  return -1;
   return RPN__adjust_main (a_cell, a_scope, a_atab, a_acol, a_arow, a_final, a_index, "");
}



/*====================------------------------------------====================*/
/*===----                          main driver                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___DRIVER__________o () { return; }

char         /*-> convert infix notation to rpn ------[ ------ [ge.QC8.1F#.E4]*/ /*-[12.0000.107.!]-*/ /*-[--.---.---.--]-*/
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
   char        x_work      [LEN_RECD]     = "";
   char       *x_rpn       = NULL;
   char        x_rpn2      [LEN_RECD]     = "";
   int         x_ntoken;
   char        rce         = -10;
   char        ch          = ' ';
   char       *x_tokens    = NULL;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   char        rc          = 0;
   char        x_temp      [LEN_RECD]     = "";
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
   --rce;  if (strchr (G_CELL_RPN , a_curr->t) == 0) {
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
   --rce;  if (strchr (G_CELL_FPRE, ch) == 0) {
      DEBUG_RPN    yLOG_note    ("prefix not in =&~!#");
      DEBUG_RPN    yLOG_exit    (__FUNCTION__);
      return rce;  /* not a formula        */
   }
   /*---(clear existing rpn)-------------*/
   if (a_curr->rpn  != NULL)  free (a_curr->rpn);
   a_curr->rpn  = NULL;
   a_curr->nrpn = 0;
   /*---(convert)------------------------*/
   strncpy (x_work, a_curr->s, LEN_RECD);
   if (ch == '~') {
      DEBUG_RPN    yLOG_note    ("processing a like formula");
      rc = LOC_parse (x_work + 1, &x_tab, &x_col, &x_row, NULL);
      DEBUG_RPN    yLOG_value   ("parse rc"  , rc);
      --rce;
      if (rc < 0) {
         DEBUG_RPN    yLOG_exit    (__FUNCTION__);
         return rce;
      }
      x_like = LOC_cell_at_loc (x_tab, x_col, x_row);
      DEBUG_RPN    yLOG_point   ("x_like"    , x_like);
      --rce;
      if (x_like == NULL) {
         DEBUG_RPN    yLOG_exit    (__FUNCTION__);
         return rce;
      }
      strncpy (x_work, x_like->s, LEN_RECD);
      ch = x_work[0];
      rc = RPN_adjust (x_like, a_curr->tab - x_tab, a_curr->col - x_col, a_curr->row - x_row, x_temp);
      DEBUG_RPN    yLOG_value   ("adjust rc" , rc);
      --rce;
      if (rc < 0) {
         DEBUG_RPN    yLOG_exit    (__FUNCTION__);
         return rce;
      }
      strncpy (x_work, x_temp, LEN_RECD);
      /*> strltrim (x_work, ySTR_EVERY, LEN_RECD);                                     <*/
      if (ch == '#')  a_curr->t = CTYPE_MLIKE;
   }
   x_work [0] = '=';
   DEBUG_RPN    yLOG_info    ("x_work"    , x_work);
   DEBUG_RPN    yLOG_value   ("prefix"    , ch);
   DEBUG_RPN    yLOG_value   ("CTAB"      , CTAB);
   x_rpn = yRPN_spreadsheet (x_work, &x_ntoken, CTAB);
   --rce;  if (x_rpn == NULL)            return rce;
   DEBUG_RPN    yLOG_point   ("x_rpn"     , x_rpn);
   DEBUG_RPN    yLOG_info    ("x_rpn"     , x_rpn);
   /*---(clean-up)-----------------------*/
   if (ch == '#')   strcpy  (x_rpn2, "#,");
   else             strcpy  (x_rpn2, "=,");
   strcat (x_rpn2, x_rpn);
   len = strlen (x_rpn2);
   x_rpn2 [len - 2] = '\0';
   DEBUG_RPN    yLOG_info    ("x_rpn2"    , x_rpn2);
   DEBUG_RPN    yLOG_value   ("x_ntoken"  , x_ntoken);
   /*---(put into cell)------------------*/
   a_curr->rpn   = strndup (x_rpn2, LEN_RECD);
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

char*        /*-> tbd --------------------------------[ leaf   [gs.310.201.10]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
RPN_unit           (char *a_question, void *a_curr)
{
   /*---(locals)-----------+-----------+-*/
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_rpn            : question not understood");
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "rpn_updated" )   == 0) {
      snprintf(unit_answer, LEN_STR, "s_rpn updated    : %-.45s", s_final);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



/*============================----end-of-source---============================*/
