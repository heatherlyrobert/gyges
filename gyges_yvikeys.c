/*============================----beg-of-source---============================*/
#include   "gyges.h"



/*====================------------------------------------====================*/
/*===----                       yVIKEYS                                ----===*/
/*====================------------------------------------====================*/
static void   o___YVIKEYS_________o (void) { return; }

char
api_yvikeys_saver       (char *a_contents)
{
   CELL_change  (NULL, CHG_INPUT, CTAB, CCOL, CROW, a_contents);
   return 0;
}



char         /*-> keys for formatting sub-mode -------[ ------ [gc.MT0.202.C7]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
SMOD_format             (int a_major, int a_minor)
{
   /*---(column widths)------------------*/
   switch (a_minor) {
   case  'm' : CELL_visual   (CHANGE_WIDTH  , CHG_INPUT, 'm');  break;  /* smallest    */
   case  'h' : CELL_visual   (CHANGE_WIDTH  , CHG_INPUT, 'h');  break;  /* less by 1   */
   case  'H' : CELL_visual   (CHANGE_WIDTH  , CHG_INPUT, 'H');  break;  /* less by 5   */
   case  'l' : CELL_visual   (CHANGE_WIDTH  , CHG_INPUT, 'l');  break;  /* more by 1   */
   case  'L' : CELL_visual   (CHANGE_WIDTH  , CHG_INPUT, 'L');  break;  /* more by 5   */
   case  'n' : CELL_visual   (CHANGE_WIDTH  , CHG_INPUT, 'n');  break;  /* normal      */
   case  'N' : CELL_visual   (CHANGE_WIDTH  , CHG_INPUT, 'N');  break;  /* normal-plus */
   case  'w' : CELL_visual   (CHANGE_WIDTH  , CHG_INPUT, 'w');  break;  /* wide        */
   case  'W' : CELL_visual   (CHANGE_WIDTH  , CHG_INPUT, 'W');  break;  /* largest     */
   }
   /*---(decimal places)-----------------*/
   switch (a_minor) {
   case  '0' : CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '0');  break;  /* none        */
   case  '1' : CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '1');  break;  /* less by 1   */
   case  '2' : CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '2');  break;  /* exactly 2   */
   case  '3' : CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '3');  break;  /* exactly 3   */
   case  '4' : CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '4');  break;  /* exactly 4   */
   case  '5' : CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '5');  break;  /* exactly 5   */
   case  '6' : CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '6');  break;  /* exactly 6   */
   case  '7' : CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '7');  break;  /* exactly 7   */
   case  '8' : CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '8');  break;  /* exactly 8   */
   case  '9' : CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '9');  break;  /* exactly 9   */
   }
   /*---(horizontal alignments)----------*/
   switch (a_minor) {
   case  '<' : CELL_visual   (CHANGE_ALIGN  , CHG_INPUT, '<');  break;  /* left        */
   case  '|' : CELL_visual   (CHANGE_ALIGN  , CHG_INPUT, '|');  break;  /* center      */
   case  '>' : CELL_visual   (CHANGE_ALIGN  , CHG_INPUT, '>');  break;  /* right       */
   case  '[' : CELL_visual   (CHANGE_ALIGN  , CHG_INPUT, '[');  break;  /* left dec    */
   case  '^' : CELL_visual   (CHANGE_ALIGN  , CHG_INPUT, '^');  break;  /* center dec  */
   case  ']' : CELL_visual   (CHANGE_ALIGN  , CHG_INPUT, ']');  break;  /* right dec   */
   case  '}' : CELL_visual   (CHANGE_ALIGN  , CHG_INPUT, '}');  break;  /* right +2    */
   case  '{' : CELL_visual   (CHANGE_ALIGN  , CHG_INPUT, '{');  break;  /* left  +2    */
   }
   /*---(merging)------------------------*/
   switch (a_minor) {
   case  'M' : CELL_visual   (CHANGE_MERGE  , CHG_INPUT, 'm');  break;  /* merge       */
   case  'U' : CELL_visual   (CHANGE_UNMERGE, CHG_INPUT, 'u');  break;  /* merge       */
   }
   /*---(integer formats)----------------*/
   switch (a_minor) {
   case  'i' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'i');  break;  /* integer         */
   case  'f' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'f');  CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '3'); break;  /* real/float      */
   case  'g' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'f');  CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '6'); break;  /* real/float      */
   case  'e' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'e');  CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '3'); break;  /* exponencial     */
   case  'E' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'E');  CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '3'); break;  /* spaced exponent */
   }
   /*---(comma formats)------------------*/
   switch (a_minor) {
   case  'c' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'c');  break;  /* comma           */
   case  'C' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'C');  break;  /* comma           */
   case  'a' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'a');  break;  /* accounting      */
   case  'A' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'A');  break;  /* accounting      */
   case  '$' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, '$');  break;  /* dollar          */
   case  's' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 's');  break;  /* signed          */
   case  'S' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'S');  break;  /* signed          */
   case  '#' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, '#');  CELL_visual   (CHANGE_DECIMAL, CHG_INPUT, '6'); break;  /* technical       */
   case  'p' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'p');  break;  /* bullet point    */
   case  'P' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'P');  break;  /* bullet point    */
   }
   /*---(roman numerals)-----------------*/
   switch (a_minor) {
   case  'r' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'r');  break;  /* roman           */
   case  'R' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'R');  break;  /* roman upper     */
   }
   /*---(special formats)----------------*/
   switch (a_minor) {
   case  'o' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'o');  break;  /* octal           */
   case  'O' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'O');  break;  /* spaced octal    */
   case  'x' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'x');  break;  /* hex             */
   case  'X' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'X');  break;  /* spaced hex      */
   case  'b' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'b');  break;  /* binary          */
   case  'B' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'B');  break;  /* spaced binary   */
   case  'z' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'z');  break;  /* base-62         */
   case  'Z' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'Z');  break;  /* spaced base-62  */
   }
   /*---(time and date)------------------*/
   switch (a_minor) {
   case  't' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 't');  break;  /* time            */
   case  'T' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'T');  break;  /* timestamp       */
   case  'd' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'd');  break;  /* date            */
   case  'D' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, 'D');  break;  /* full date       */
   }
   /*---(string fills)-------------------*/
   switch (a_minor) {
   case  '-' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, '-');  break;  /* filled dash     */
   case  '=' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, '=');  break;  /* filled equal    */
   case  '_' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, '_');  break;  /* filled under    */
   case  '.' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, '.');  break;  /* filled period   */
   case  '+' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, '+');  break;  /* filled plus     */
   case  '/' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, '/');  break;  /* filled divs     */
   case  '"' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, '"');  break;  /* filled euro quot*/
   case  '!' : CELL_visual   (CHANGE_FORMAT , CHG_INPUT, ' ');  break;  /* filled empty    */
   }
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        searching                             ----===*/
/*====================------------------------------------====================*/
PRIV void  o___SEARCH__________o () { return; }

char
SRCH_clearer         (int a_x, int a_y, int a_z)
{
   /*---(locals)-----------+------+----+-*/
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_SRCH   yLOG_enter   (__FUNCTION__);
   DEBUG_SRCH   yLOG_value   ("a_x"       , a_x);
   DEBUG_SRCH   yLOG_value   ("a_y"       , a_y);
   DEBUG_SRCH   yLOG_value   ("a_z"       , a_z);
   x_curr = LOC_cell_at_loc (a_z, a_x, a_y);
   DEBUG_SRCH   yLOG_point   ("x_curr"    , x_curr);
   if (x_curr != NULL) x_curr->n = '-';
   DEBUG_SRCH   yLOG_char    ("x_curr->n" , x_curr->n);
   /*---(complete)---------------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return 0;
}


char         /*-> tbd --------------------------------[ ------ [ge.#M5.1C#.#7]*/ /*-[03.0000.013.L]-*/ /*-[--.---.---.--]-*/
SRCH_searcher_OLD  (char *a_search)
{
   /*---(locals)-----------+------+----+-*/
   char        rce         =   -10;
   char        rc          =     0;
   tCELL      *x_next      = NULL;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   /*---(header)--------------------s----*/
   DEBUG_SRCH   yLOG_enter   (__FUNCTION__);
   DEBUG_SRCH   yLOG_point   ("a_search"  , a_search);
   /*---(defenses)---------------------------*/
   --rce;  if (a_search == NULL) {
      DEBUG_SRCH   yLOG_note    ("can not use null search");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
   }
   DEBUG_SRCH   yLOG_info    ("a_search"  , a_search);
   rc = yREGEX_comp (a_search + 1);
   DEBUG_SRCH   yLOG_value   ("comp rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_SRCH   yLOG_note    ("could not compile search");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process range)----------------------*/
   yVIKEYS_first (&x_tab, &x_col, &x_row);
   x_next  = LOC_cell_at_loc (x_tab, x_col, x_row);
   do {
      DEBUG_SRCH   yLOG_complex ("x_next"    , "ptr %p, tab %2d, col %3d, row %4d", x_next, x_tab, x_col, x_row);
      if (x_next != NULL && x_next->s != NULL) {
         DEBUG_SRCH   yLOG_char    ("->type"    , x_next->t);
         switch (x_next->t) {
         case YCALC_DATA_STR   :
            DEBUG_SRCH   yLOG_info    ("->s"       , x_next->s);
            rc = yREGEX_exec (x_next->s);
            break;
         case YCALC_DATA_SFORM :
         case YCALC_DATA_SLIKE :
            DEBUG_SRCH   yLOG_info    ("->v_str"   , x_next->v_str);
            rc = yREGEX_exec (x_next->v_str);
            break;
         default          :
            DEBUG_SRCH   yLOG_note    ("can not process cell type");
            rc = -1;
            break;
         }
         DEBUG_SRCH   yLOG_value   ("exec rc"   , rc);
         if (rc > 0) {
            yVIKEYS_srch_found (x_next->label, x_next->col, x_next->row, x_next->tab);
            x_next->n = 's';
         }
      }
      rc      = yVIKEYS_next  (&x_col, &x_row, &x_tab);
      x_next  = LOC_cell_at_loc (x_tab, x_col, x_row);
   } while (rc >= 0);
   /*---(complete)---------------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return 0;
}




/*====================------------------------------------====================*/
/*===----                       setup and teardown                     ----===*/
/*====================------------------------------------====================*/
PRIV void  o___SETUP___________o () { return; }

char         /*-> delete a register cell -------------[ ------ [gz.210.101.01]*/ /*-[00.0000.104.!]-*/ /*-[--.---.---.--]-*/
api_yvikeys_killer      (tCELL *a_curr)
{
   CELL__free (&a_curr, LINKED);
   return 0;
}



char         /*-> tail recursion function for copy ---[ ------ [ge.D55.237.63]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
REG_copy_one       (tCELL *a_curr, long a_stamp)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   tCELL      *x_copy      = NULL;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_REGS   yLOG_point   ("a_curr"    , a_curr);
   --rce;  if (a_curr    == NULL) {
      DEBUG_REGS   yLOG_note    ("original cell null");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* no cell                       */
   }
   DEBUG_REGS   yLOG_point   ("s"         , a_curr->s);
   --rce;  if (a_curr->s == NULL) {
      DEBUG_REGS   yLOG_note    ("no source");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* nothing to write              */
   }
   DEBUG_REGS   yLOG_info    ("s"         , a_curr->s);
   DEBUG_REGS   yLOG_char    ("t"         , a_curr->t);
   --rce;  if (a_curr->t == '-')  {
      DEBUG_REGS   yLOG_note    ("could not copy an empty");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* don't write, recreate on read */
   }
   /*---(check for bounds)---------------*/
   rc = yVIKEYS_regs_inside (a_curr->col, a_curr->row, a_curr->tab);
   DEBUG_REGS   yLOG_value   ("visu_rc"   , rc);
   --rce;  if (rc <= 0)  {
      DEBUG_REGS   yLOG_note    ("cell not in visual area");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check timestamp)----------------*/
   /*> DEBUG_REGS   yLOG_value   ("a_curr->u" , a_curr->u);                           <*/
   /*> --rce;  if (a_curr->u == a_stamp) {                                             <* 
    *>    DEBUG_REGS   yLOG_note    ("skipping, timestamp matches, already copied");   <* 
    *>    DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);                               <* 
    *>    return rce;                                                                  <* 
    *> }                                                                               <*/
   /*---(make a copy)--------------------*/
   rc = CELL_dup (&x_copy, a_curr);
   DEBUG_REGS   yLOG_value   ("dup_rc"    , rc);
   --rce;  if (rc < 0 ) {
      DEBUG_REGS   yLOG_note    ("could not duplicate");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(move in critical data)----------*/
   strlcpy (x_copy->label, a_curr->label, LEN_LABEL);
   /*> a_curr->u   = a_stamp;                                                         <*/
   /*> DEBUG_REGS   yLOG_complex ("STAMPED"   , "ptr=%p, tab=%4d, col=%4d, row=%4d, t=%c, u=%d, with %d", a_curr, a_curr->tab, a_curr->col, a_curr->row, a_curr->t, a_curr->u, a_stamp);   <*/
   /*---(place in buffer)----------------*/
   rc = yVIKEYS_regs_add  (x_copy, x_copy->label,'d');
   DEBUG_REGS   yLOG_value   ("hook_rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_note    ("could not hook to register");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
REG_copy_one_seq        (void *a_owner, void *a_deproot, int a_seq, int a_level)
{
   return  REG_copy_one (0, (tCELL *) a_owner);
}

char
api_yvikeys_copier      (char a_type, long a_stamp)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   DEBUG_REGS   yLOG_value   ("a_stamp"   , a_stamp);
   /*---(dependents)---------------------*/
   DEBUG_REGS   yLOG_note    ("DEPENDENT CELLS");
   /*> rc      = SEQ_reg_deps (a_stamp);                                              <*/
   rc      = yCALC_seq_downdown (a_stamp, REG_copy_one_seq);
   /*---(independents)-------------------*/
   DEBUG_REGS   yLOG_note    ("INDEPENDENT CELLS");
   rc      = yVIKEYS_first (&x_col, &x_row, &x_tab);
   while (rc >= 0) {
      x_curr  = LOC_cell_at_loc (x_tab, x_col, x_row);
      if (x_curr != NULL && yCALC_stamp (x_curr->ycalc, a_stamp))   REG_copy_one (x_curr, a_stamp);
      rc      = yVIKEYS_next  (&x_col, &x_row, &x_tab);
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_yvikeys_clearer     (char a_1st, int x, int y, int z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   static int  x_count     =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(check first)--------------------*/
   DEBUG_REGS   yLOG_char    ("a_1st"     , a_1st);
   if (a_1st == 'y')  x_count = 0;
   /*---(identify cell)------------------*/
   DEBUG_REGS   yLOG_complex ("coords"    , "%3dx, %3dy, %3dz", x, y, z);
   x_curr = LOC_cell_at_loc (z, x, y);
   DEBUG_REGS   yLOG_complex ("x_curr"    , x_curr);
   if (x_curr == NULL) {
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return x_count;
   }
   /*---(delete)-------------------------*/
   DEBUG_REGS   yLOG_value   ("x_count"   , x_count);
   if (a_1st == 'y')  rc = CELL_change (NULL, CHG_INPUT   , z, x, y, "");
   else               rc = CELL_change (NULL, CHG_INPUTAND, z, x, y, "");
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update count)-------------------*/
   ++x_count;
   DEBUG_REGS   yLOG_value   ("x_count"   , x_count);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_yvikeys_paster      (char a_reqs, char a_pros, char a_intg, char a_1st, int a_xoff, int a_yoff, int a_zoff, tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   short       x_stab, x_scol, x_srow;
   short       x_dtab, x_dcol, x_drow;
   char        x_source    [LEN_RECD]   = "";
   char        x_bformat   [LEN_RECD]   = "";
   tCELL      *x_copy      = NULL;
   char        x_list      [LEN_RECD]   = "";
   char       *p           = NULL;               /* pointer for providers     */
   char       *q           = ",";                /* delimiter for providers   */
   char       *s           = NULL;               /* context for providers     */
   tCELL      *x_provider  = NULL;               /* provider cell to adjust   */
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_reqs"    , a_reqs);
   DEBUG_REGS   yLOG_char    ("a_pros"    , a_pros);
   DEBUG_REGS   yLOG_char    ("a_intg"    , a_intg);
   DEBUG_REGS   yLOG_char    ("a_1st"     , a_1st);
   DEBUG_REGS   yLOG_complex ("offset"    , "x=%4d, y=%4d, z=%4d", a_xoff, a_yoff, a_zoff);
   /*---(defense)------------------------*/
   DEBUG_REGS   yLOG_point   ("a_cell"    , a_cell);
   --rce;  if (a_cell == NULL)  {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_info    ("G_RPN_REQS", G_RPN_REQS);
   --rce;  if (strchr (G_RPN_REQS, a_reqs) == NULL)  {
      DEBUG_REGS   yLOG_note    ("a_reqs type not recognized");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_info    ("G_RPN_PROS", G_RPN_PROS);
   --rce;  if (strchr (G_RPN_PROS, a_pros) == NULL)  {
      DEBUG_REGS   yLOG_note    ("a_pros type not recognized");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get original location)----------*/
   DEBUG_REGS   yLOG_info    ("a_label"   , a_cell->label);
   rc = LOC_parse (a_cell->label, &x_stab, &x_scol, &x_srow, NULL);
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0)  {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_complex ("original"  , "tab=%4d, col=%4d, row=%4d", x_stab, x_scol, x_srow);
   /*---(set new location)---------------*/
   x_dtab  = x_stab + a_zoff;
   x_dcol  = x_scol + a_xoff;
   x_drow  = x_srow + a_yoff;
   DEBUG_REGS   yLOG_complex ("going to"  , "tab=%4d, col=%4d, row=%4d", x_dtab, x_dcol, x_drow);
   /*---(check cell type)----------------*/
   DEBUG_REGS   yLOG_info    ("source"    , a_cell->s);
   DEBUG_REGS   yLOG_char    ("type"      , a_cell->t);
   strcpy (x_source, "");
   if (strchr (YCALC_GROUP_RPN, a_cell->t) != 0) {
      DEBUG_REGS   yLOG_note    ("formula, calling yRPN_adjust");
      /*> rc = RPN_adjust_reg (a_cell, a_reqs, a_zoff, a_xoff, a_yoff, x_source, s_index);   <*/
      rc = yRPN_adjust_reqs (a_cell->s, a_reqs, a_xoff, a_yoff, a_zoff, LEN_RECD, x_source);
      DEBUG_REGS   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         DEBUG_REGS   yLOG_note    ("formula could not be parsed");
         strcpy (x_source, "not_translatable");
      }
   } else {
      DEBUG_REGS   yLOG_note    ("just copy straight across");
      strcpy (x_source, a_cell->s);
   }
   DEBUG_REGS   yLOG_info    ("x_source"  , x_source);
   sprintf (x_bformat, "%c%c%c", a_cell->f, a_cell->a, a_cell->d);
   DEBUG_REGS   yLOG_info    ("x_bformat" , x_bformat);
   if (a_1st == 'y')  x_copy = CELL_overwrite (CHG_OVER   , x_dtab, x_dcol, x_drow, x_source, x_bformat);
   else               x_copy = CELL_overwrite (CHG_OVERAND, x_dtab, x_dcol, x_drow, x_source, x_bformat);
   /*---(providers)----------------------*/
   if (a_pros == G_RPN_IGNORE || a_pros == G_RPN_PNONE) {
      DEBUG_REGS   yLOG_note    ("provider updates not requested");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_REGS   yLOG_note    ("CHECK PROVIDERS");
   yCALC_disp_pros (LOC_cell_at_loc (x_stab, x_scol, x_srow), x_list);
   DEBUG_REGS   yLOG_info    ("x_list"    , x_list);
   if (strchr ("-.", x_list [0]) != NULL) {
      DEBUG_REGS   yLOG_note    ("no providers identified");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   p  = strtok_r (x_list, q, &s);
   DEBUG_REGS   yLOG_point   ("p"         , p);
   while (p != NULL) {
      DEBUG_REGS   yLOG_info    ("p"         , p);
      x_provider = LOC_cell_labeled (p);
      DEBUG_REGS   yLOG_point   ("x_provider", x_provider);
      if (x_provider != NULL) {
         DEBUG_REGS   yLOG_complex ("details"   , "%s, %3dx, %3dy, %3dz", x_provider->label, x_provider->col, x_provider->row, x_provider->tab);
         rc = yVIKEYS_regs_inside (x_provider->col, x_provider->row, x_provider->tab);
         DEBUG_REGS   yLOG_value   ("rc"        , rc);
         if (rc == 0) {
            DEBUG_REGS   yLOG_info    ("source"    , x_provider->s);
            DEBUG_REGS   yLOG_info    ("change"    , a_cell->label);
            /*> rc = RPN_adjust_ref (x_provider, a_pros, a_zoff, a_xoff, a_yoff, x_source, a_cell->label);   <*/
            rc = yRPN_adjust_pros (x_provider->s, a_pros, a_xoff, a_yoff, a_zoff, a_cell->label, LEN_RECD, x_source);
            DEBUG_REGS   yLOG_value   ("rc"        , rc);
            DEBUG_REGS   yLOG_info    ("x_source"  , x_source);
            sprintf (x_bformat, "%c%c%c", x_provider->f, x_provider->a, x_provider->d);
            DEBUG_REGS   yLOG_info    ("x_bformat" , x_bformat);
            CELL_overwrite (CHG_OVERAND, x_provider->tab, x_provider->col, x_provider->row, x_source, x_bformat);
         }
      }
      p  = strtok_r (NULL  , q, &s);
      DEBUG_REGS   yLOG_point   ("p"         , p);
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}


