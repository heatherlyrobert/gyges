/*============================----beg-of-source---============================*/
#include   "gyges.h"



static long s_stamp = 0;


/*====================------------------------------------====================*/
/*===----                    source mode                               ----===*/
/*====================------------------------------------====================*/
static void   o___SOURCE__________o (void) { return; }

char
api_yvikeys_saver       (char *a_contents)
{
   CELL_change  (NULL, CHG_INPUT, CTAB, CCOL, CROW, a_contents);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      map mode formatting                     ----===*/
/*====================------------------------------------====================*/
static void   o___FORMAT__________o (void) { return; }

char         /*-> keys for formatting sub-mode -------[ ------ [gc.MT0.202.C7]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
api_yvikeys_format      (int a_major, int a_minor)
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

char         /*-> tbd --------------------------------[ ------ [ge.#M5.1C#.#7]*/ /*-[03.0000.013.L]-*/ /*-[--.---.---.--]-*/
api_yvikeys_searcher      (char *a_search)
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
   DEBUG_CELL   yLOG_point   ("hcell"     , hcell);
   x_next = hcell;
   /*> x_next  = VISU_first (&x_tab, &x_col, &x_row);                                 <*/
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
      x_next = x_next->next;
      /*> x_next  = VISU_next (&x_tab, &x_col, &x_row);                               <*/
   } while (x_next != NULL && x_next != DONE_DONE);
   /*---(complete)---------------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_yvikeys_unsearcher   (int a_x, int a_y, int a_z)
{
   /*---(locals)-----------+------+----+-*/
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_SRCH   yLOG_enter   (__FUNCTION__);
   DEBUG_SRCH   yLOG_value   ("a_x"       , a_x);
   DEBUG_SRCH   yLOG_value   ("a_y"       , a_y);
   DEBUG_SRCH   yLOG_value   ("a_z"       , a_z);
   x_curr = LOC_cell_at_loc (a_x, a_y, a_z);
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
   x_next  = LOC_cell_at_loc (x_col, x_row, x_tab);
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
      x_next  = LOC_cell_at_loc (x_col, x_row, x_tab);
   } while (rc >= 0);
   /*---(complete)---------------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     map mode registers                       ----===*/
/*====================------------------------------------====================*/
static void  o___REGISTERS_______o () { return; }

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
   x_curr = LOC_cell_at_loc (x, y, z);
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

char         /*-> tail recursion function for copy ---[ ------ [ge.D55.237.63]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
api__yvikeys_copier_one       (tCELL *a_curr, long a_stamp)
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
   yCALC_stamp_set (a_curr->ycalc, s_stamp);
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
api_yvikeys_copier_seq        (void *a_owner, void *a_deproot, int a_seq, int a_level)
{
   return  api__yvikeys_copier_one ((tCELL *) a_owner, s_stamp);
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
   s_stamp = a_stamp;
   /*---(dependents)---------------------*/
   DEBUG_REGS   yLOG_note    ("DEPENDENT CELLS");
   rc      = yCALC_seq_downdown (s_stamp, api_yvikeys_copier_seq);
   /*---(independents)-------------------*/
   DEBUG_REGS   yLOG_note    ("INDEPENDENT CELLS");
   rc      = yVIKEYS_first (&x_col, &x_row, &x_tab);
   while (rc >= 0) {
      x_curr  = LOC_cell_at_loc (x_col, x_row, x_tab);
      if (x_curr != NULL && yCALC_stamp_cmp (x_curr->ycalc, s_stamp))   api__yvikeys_copier_one (x_curr, s_stamp);
      rc      = yVIKEYS_next  (&x_col, &x_row, &x_tab);
   }
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
   yCALC_disp_pros (LOC_cell_at_loc (x_scol, x_srow, x_stab), x_list);
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

char         /*-> delete a register cell -------------[ ------ [gz.210.101.01]*/ /*-[00.0000.104.!]-*/ /*-[--.---.---.--]-*/
api_yvikeys_regkiller    (tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_APIS   yLOG_enter   (__FUNCTION__);
   DEBUG_APIS   yLOG_point   ("a_cell"    , a_cell);
   /*---(kill)---------------------------*/
   rc = CELL__free (&a_cell, LINKED);
   DEBUG_APIS   yLOG_value   ("free"      , rc);
   /*---(complete)-----------------------*/
   DEBUG_APIS   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                     keyboard macro handler                   ----===*/
/*====================------------------------------------====================*/
static void  o___MACRO___________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.540.136.51]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
api_yvikeys_macro_get       (char a_name, char *a_macro)
{
   char        rce         =  -10;
   short       x_row       =    0;
   tCELL      *x_curr      = NULL;
   --rce;  if (a_macro   == NULL     )                      return rce;
   --rce;  if (a_name < 'a' || a_name > 'z')  return rce;
   x_row = a_name - 'a';
   x_curr = LOC_cell_at_loc (1, x_row, 37);
   --rce;  if (x_curr    == NULL)                           return rce;
   if (x_curr->t == YCALC_DATA_STR) {
      strlcpy (a_macro, x_curr->s    , LEN_RECD);
      return 0;
   }
   if (x_curr->t == YCALC_DATA_SFORM) {
      strlcpy (a_macro, x_curr->v_str, LEN_RECD);
      return 0;
   }
   --rce;
   return rce;
}

char         /*-> tbd --------------------------------[ ------ [ge.540.153.42]*/ /*-[01.0000.105.!]-*/ /*-[--.---.---.--]-*/
api_yvikeys_macro_set       (char a_name, char *a_keys)
{
   char        rce         =  -10;
   short       x_row       =    0;
   char        t           [LEN_LABEL]   = "";
   int         x_len       =    0;
   tCELL      *x_curr      = NULL;
   --rce;  if (a_keys   == NULL     )                      return rce;
   --rce;  if (a_name < 'a' || a_name > 'z')  return rce;
   x_row = a_name - 'a';
   x_len = strlen (a_keys);
   if (x_len == 0) {
      CELL_change (NULL, CHG_NOHIST, 37, 0, x_row, "");
      CELL_change (NULL, CHG_NOHIST, 37, 1, x_row, "");
   } else {
      sprintf (t, "%c", a_name);
      CELL_change (NULL, CHG_NOHIST, 37, 0, x_row, t     );
      CELL_change (NULL, CHG_NOHIST, 37, 1, x_row, a_keys);
   }
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     mapping for map mode                     ----===*/
/*====================------------------------------------====================*/
static void  o___MAPPING_________o () { return; }

char
MAP__clear            (tMAPPED *a_map)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =    0;
   /*---(lefts)--------------------------*/
   a_map->umin = a_map->gmin = a_map->gamin = a_map->glmin = a_map->gprev = -1;
   /*---(map)----------------------------*/
   for (i= 0; i < LEN_MAP; ++i)  a_map->map [i] =  YVIKEYS_EMPTY;
   /*---(rights)-------------------------*/
   a_map->umax = a_map->gmax = a_map->gamax = a_map->glmax = a_map->gnext = -1;
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
   int         x_total     =    0;
   int         x_cell      =    0;
   tCELL      *x_curr      = NULL;
   tCELL      *x_prev      = NULL;
   int         x_unit      =    0;
   int         x_save      =    0;
   int         x_mark      =    0;
   /*---(prepare)------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   DEBUG_MAP    yLOG_value   ("x->gcur"   , g_xmap.gcur);
   DEBUG_MAP    yLOG_value   ("y->gcur"   , g_ymap.gcur);
   DEBUG_MAP    yLOG_value   ("z->gcur"   , g_zmap.gcur);
   switch (a_dir) {
   case 'C' : case 'c' :
      DEBUG_MAP    yLOG_note    ("COLS (x)");
      x_map   = &g_xmap;
      x_max   = NCOL - 1;
      x_total = s_tabs [g_zmap.gcur].rows [g_ymap.gcur].c;
      break;
   case 'R' : case 'r' :
      DEBUG_MAP    yLOG_note    ("ROWS (y)");
      x_map   = &g_ymap;
      x_max   = NROW - 1;
      x_total = s_tabs [g_zmap.gcur].cols [g_xmap.gcur].c;
      break;
   case 'T' : case 't' :
      DEBUG_MAP    yLOG_note    ("TABS (z)");
      x_map  = &g_zmap;
      x_max  = NTAB - 1;
      x_total = 1;
      break;
   }
   x_mark = x_map->gcur;
   /*---(clear)--------------------------*/
   for (i= 0; i < LEN_MAP; ++i)  x_map->map [i] =  YVIKEYS_EMPTY;
   x_map->gmin = x_map->gamin = x_map->glmin = x_map->gprev = -1;
   x_map->gmax = x_map->gamax = x_map->glmax = x_map->gnext = -1;
   /*---(do columns)---------------------*/
   for (x_cell = 0; x_cell <= x_max; ++x_cell) {
      /*---(get base data)---------------*/
      switch (a_dir) {
      case 'C' : case 'c' :
         x_size  = s_tabs [g_zmap.gcur].cols [x_cell].w;
         x_curr  = LOC_cell_at_loc (x_cell, g_ymap.gcur, g_zmap.gcur);
         x_count = s_tabs [g_zmap.gcur].cols [x_cell].c;
         break;
      case 'R' : case 'r' :
         x_size  = s_tabs [g_zmap.gcur].rows [x_cell].h;
         x_curr  = LOC_cell_at_loc (g_xmap.gcur, x_cell, g_zmap.gcur);
         x_count = s_tabs [g_zmap.gcur].rows [x_cell].c;
         break;
      case 'T' : case 't' :
         x_size  = 1;
         x_curr  = LOC_cell_at_loc (g_xmap.gcur, g_ymap.gcur, x_cell);
         x_count = 1;
         break;
      }
      DEBUG_MAP    yLOG_complex ("LOOP"      , "%3dn, %3du, %3ds, %-10p, %3dc", x_cell, x_unit, x_size, x_curr, x_count);
      /*---(big mins)--------------------*/
      x_map->umin = 0;
      x_map->gmin = 0;
      if (x_map->gamin < 0 && x_count > 0)      x_map->gamin = x_cell;
      if (x_map->glmin < 0 && x_curr != NULL)   x_map->glmin = x_cell;
      /*---(little mins)-----------------*/
      if (x_cell == x_mark) {
         if (x_prev != NULL && x_curr == NULL) x_map->gprev = x_save;
      }
      if (x_cell <  x_mark) {
         if (x_prev == NULL && x_curr != NULL) x_map->gprev = x_cell;
         if (x_prev != NULL && x_curr == NULL) x_map->gprev = x_save;
      }
      DEBUG_MAP    yLOG_complex ("mins"    , "%3dg, %3da, %3dl, %3dp", x_map->gmin, x_map->gamin, x_map->glmin, x_map->gprev);
      /*---(update map)------------------*/
      for (i = 0; i < x_size; ++i) {
         x_map->map  [x_unit++] = x_cell;
         if (x_curr == NULL)  x_map->used [x_cell] = '-';
         else                 x_map->used [x_cell] = 'y';
      }
      /*---(big maxs)--------------------*/
      if (x_curr != NULL)                      x_map->glmax = x_cell;
      if (x_count > 0)                         x_map->gamax = x_cell;
      x_map->gmax = x_cell;
      x_map->umax = x_unit - 1;
      /*---(little maxes)----------------*/
      if (x_cell > x_mark && x_map->gnext < 0) {
         if (x_prev == NULL && x_curr != NULL) x_map->gnext = x_cell;
      }
      if (x_cell > x_mark + 1 && x_map->gnext < 0) {
         if (x_prev == NULL && x_curr != NULL) x_map->gnext = x_cell;
         if (x_prev != NULL && x_curr == NULL) x_map->gnext = x_save;
      }
      DEBUG_MAP    yLOG_complex ("maxs"    , "%3dn, %3dl, %3da, %3dg", x_map->gnext, x_map->glmax, x_map->gamax, x_map->gmax);
      /*---(done)------------------------*/
      x_save = x_cell;
      x_prev = x_curr;
   }
   /*---(update mins and maxes)----------*/
   if (x_total > 0) {
      if (x_map->gamin < 0)  x_map->gamin = x_map->gmin;
      if (x_map->glmin < 0)  x_map->glmin = x_map->gmin;
      if (x_map->gprev < 0)  x_map->gprev = x_map->gmin;
      if (x_map->gamax < 0)  x_map->gamax = x_map->gmin;
      if (x_map->glmax < 0)  x_map->glmax = x_map->gmin;
      if (x_map->gnext < 0)  x_map->gnext = x_map->gmax;
   }
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
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
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
   fprintf (f, "%4d %4d %4d %4d    "  , x_map->gmin, x_map->gamin, x_map->glmin, x_map->gprev);
   for (i = 0; i < LEN_MAP; ++i) {
      if (x_map->map [i] < 0)  break;
      fprintf (f, "%4d "  , x_map->map [i]);
   }
   fprintf (f, "   %4d %4d %4d %4d\n", x_map->gnext, x_map->glmax, x_map->gamax, x_map->gmax);
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
