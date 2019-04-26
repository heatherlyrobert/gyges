/*============================----beg-of-source---============================*/
#include   "gyges.h"



static long s_stamp = 0;



/*====================------------------------------------====================*/
/*===----                   support for map mode                       ----===*/
/*====================------------------------------------====================*/
static void   o___MAP_____________o (void) { return; }

char         /*-> break label into coordinates -------[ ------ [gc.722.112.13]*/ /*-[01.0000.304.#]-*/ /*-[--.---.---.--]-*/
api_yvikeys_locator     (char *a_label, int *a_buf, int *a_x, int *a_y, int *a_z)
{
   if (a_z != NULL)  *a_z = 0;
   return str2gyges (a_label, a_buf, a_x, a_y, NULL, NULL, 0, YSTR_LEGAL);
}

char         /*-> return address for coordinates -----[ ------ [gc.722.112.13]*/ /*-[01.0000.304.#]-*/ /*-[--.---.---.--]-*/
api_yvikeys_addressor   (char *a_label, int a_buf, int a_x, int a_y, int a_z)
{
   return str4gyges (a_buf, a_x, a_y, 0, 0, a_label, YSTR_LEGAL);
}



/*====================------------------------------------====================*/
/*===----                    source mode                               ----===*/
/*====================------------------------------------====================*/
static void   o___SOURCE__________o (void) { return; }

char
api_yvikeys_saver       (char *a_contents)
{
   CELL_change  (NULL, HIST_BEG, CTAB, CCOL, CROW, a_contents);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     data export/import                       ----===*/
/*====================------------------------------------====================*/
static void   o___EXIM____________o (void) { return; }

char         /*-> keys for formatting sub-mode -------[ ------ [gc.MT0.202.C7]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
api_yvikeys_exim        (char a_dir, char a_style)
{
   char        rc          =    0;
   /*---(header)--------------------s----*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_dir"     , a_dir);
   DEBUG_REGS   yLOG_char    ("a_style"   , a_style);
   switch (a_dir) {
   case '+' :
      DEBUG_REGS   yLOG_note    ("call importer (+)");
      rc = EXIM_import (a_style);
      break;
   case '-' :
      DEBUG_REGS   yLOG_note    ("call exporter (-)");
      rc = EXIM_export (a_style);
      break;
   }
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                      map mode formatting                     ----===*/
/*====================------------------------------------====================*/
static void   o___FORMAT__________o (void) { return; }

char         /*-> keys for formatting sub-mode -------[ ------ [gc.MT0.202.C7]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
api_yvikeys_format      (int a_major, int a_minor)
{
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   /*---(column widths)------------------*/
   switch (a_minor) {
   case  'm' : CELL_visual   (CHANGE_WIDTH  , HIST_BEG, 'm');  break;  /* smallest    */
   case  'h' : CELL_visual   (CHANGE_WIDTH  , HIST_BEG, 'h');  break;  /* less by 1   */
   case  'H' : CELL_visual   (CHANGE_WIDTH  , HIST_BEG, 'H');  break;  /* less by 5   */
   case  'l' : CELL_visual   (CHANGE_WIDTH  , HIST_BEG, 'l');  break;  /* more by 1   */
   case  'L' : CELL_visual   (CHANGE_WIDTH  , HIST_BEG, 'L');  break;  /* more by 5   */
   case  'n' : CELL_visual   (CHANGE_WIDTH  , HIST_BEG, 'n');  break;  /* normal      */
   case  'N' : CELL_visual   (CHANGE_WIDTH  , HIST_BEG, 'N');  break;  /* normal-plus */
   case  'w' : CELL_visual   (CHANGE_WIDTH  , HIST_BEG, 'w');  break;  /* wide        */
   case  'W' : CELL_visual   (CHANGE_WIDTH  , HIST_BEG, 'W');  break;  /* largest     */
   }
   /*---(row heights)--------------------*/
   switch (a_minor) {
   case  'J' : CELL_visual   (CHANGE_HEIGHT , HIST_BEG, 'J');  break;  /* smallest    */
   case  'j' : CELL_visual   (CHANGE_HEIGHT , HIST_BEG, 'j');  break;  /* less by 1   */
   case  'k' : CELL_visual   (CHANGE_HEIGHT , HIST_BEG, 'k');  break;  /* more by 1   */
   case  'K' : CELL_visual   (CHANGE_HEIGHT , HIST_BEG, 'K');  break;  /* largest     */
   }
   /*---(decimal places)-----------------*/
   switch (a_minor) {
   case  '0' : CELL_visual   (CHANGE_DECIMAL, HIST_BEG, '0');  break;  /* none        */
   case  '1' : CELL_visual   (CHANGE_DECIMAL, HIST_BEG, '1');  break;  /* less by 1   */
   case  '2' : CELL_visual   (CHANGE_DECIMAL, HIST_BEG, '2');  break;  /* exactly 2   */
   case  '3' : CELL_visual   (CHANGE_DECIMAL, HIST_BEG, '3');  break;  /* exactly 3   */
   case  '4' : CELL_visual   (CHANGE_DECIMAL, HIST_BEG, '4');  break;  /* exactly 4   */
   case  '5' : CELL_visual   (CHANGE_DECIMAL, HIST_BEG, '5');  break;  /* exactly 5   */
   case  '6' : CELL_visual   (CHANGE_DECIMAL, HIST_BEG, '6');  break;  /* exactly 6   */
   case  '7' : CELL_visual   (CHANGE_DECIMAL, HIST_BEG, '7');  break;  /* exactly 7   */
   case  '8' : CELL_visual   (CHANGE_DECIMAL, HIST_BEG, '8');  break;  /* exactly 8   */
   case  '9' : CELL_visual   (CHANGE_DECIMAL, HIST_BEG, '9');  break;  /* exactly 9   */
   }
   /*---(horizontal alignments)----------*/
   switch (a_minor) {
   case  '<' : CELL_visual   (CHANGE_ALIGN  , HIST_BEG, '<');  break;  /* left        */
   case  '|' : CELL_visual   (CHANGE_ALIGN  , HIST_BEG, '|');  break;  /* center      */
   case  '>' : CELL_visual   (CHANGE_ALIGN  , HIST_BEG, '>');  break;  /* right       */
   case  '[' : CELL_visual   (CHANGE_ALIGN  , HIST_BEG, '[');  break;  /* left dec    */
   case  '^' : CELL_visual   (CHANGE_ALIGN  , HIST_BEG, '^');  break;  /* center dec  */
   case  ']' : CELL_visual   (CHANGE_ALIGN  , HIST_BEG, ']');  break;  /* right dec   */
   case  '}' : CELL_visual   (CHANGE_ALIGN  , HIST_BEG, '}');  break;  /* right +2    */
   case  '{' : CELL_visual   (CHANGE_ALIGN  , HIST_BEG, '{');  break;  /* left  +2    */
   case  ':' : CELL_visual   (CHANGE_ALIGN  , HIST_BEG, ':');  break;  /* < with :    */
   case  '\'': CELL_visual   (CHANGE_ALIGN  , HIST_BEG, '\''); break;  /* > with :    */
   }
   /*---(merging)------------------------*/
   switch (a_minor) {
   case  'M' : CELL_visual   (CHANGE_MERGE  , HIST_BEG, 'm');  break;  /* merge       */
   case  'U' : CELL_visual   (CHANGE_UNMERGE, HIST_BEG, 'u');  break;  /* merge       */
   }
   /*---(integer formats)----------------*/
   switch (a_minor) {
   case  'i' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'i');  break;  /* integer    */
   case  'I' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'I');  break;  /* indented   */
   case  'f' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'f');  break;  /* real       */
   case  'e' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'e');  break;  /* exponencial*/
   case  'E' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'E');  break;  /* spaced exp */
   }
   /*---(comma formats)------------------*/
   switch (a_minor) {
   case  'c' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'c');  break;  /* comma           */
   case  'C' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'C');  break;  /* comma           */
   case  'a' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'a');  break;  /* accounting      */
   case  'A' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'A');  break;  /* accounting      */
   case  '$' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, '$');  break;  /* dollar          */
   case  's' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 's');  break;  /* signed          */
   case  'S' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'S');  break;  /* signed          */
   case  ';' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, ';');  break;  /* technical       */
   case  'p' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'p');  break;  /* bullet point    */
   case  'P' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'P');  break;  /* bullet point    */
   }
   /*---(roman numerals)-----------------*/
   switch (a_minor) {
   case  'r' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'r');  break;  /* roman           */
   case  'R' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'R');  break;  /* roman upper     */
   }
   /*---(special formats)----------------*/
   switch (a_minor) {
   case  'o' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'o');  break;  /* octal           */
   case  'O' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'O');  break;  /* spaced octal    */
   case  'x' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'x');  break;  /* hex             */
   case  'X' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'X');  break;  /* spaced hex      */
   case  'b' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'b');  break;  /* binary          */
   case  'B' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'B');  break;  /* spaced binary   */
   case  'z' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'z');  break;  /* base-62         */
   case  'Z' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'Z');  break;  /* spaced base-62  */
   }
   /*---(time and date)------------------*/
   switch (a_minor) {
   case  't' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 't');  break;  /* time            */
   case  'T' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'T');  break;  /* timestamp       */
   case  'd' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'd');  break;  /* date            */
   case  'D' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'D');  break;  /* full date       */
   }
   /*---(string fills)-------------------*/
   switch (a_minor) {
   case  '-' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, '-');  break;  /* filled dash     */
   case  '=' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, '=');  break;  /* filled equal    */
   case  '_' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, '_');  break;  /* filled under    */
   case  '.' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, '.');  break;  /* filled period   */
   case  '+' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, '+');  break;  /* filled plus     */
   case  '!' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, '!');  break;  /* filled empty    */
   case  '/' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, '/');  break;  /* filled divs     */
   case  '@' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, '@');  break;  /* filled divs     */
   case  'q' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'q');  break;  /* filled quote    */
   case  'Q' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, 'Q');  break;  /* filled quote    */
   case  '~' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, '~');  break;  /* filled ruler    */
   case  '#' : CELL_visual   (CHANGE_FORMAT , HIST_BEG, '#');  break;  /* filled numbers  */
   }
   /*---(defaults)-----------------------*/
   switch (a_minor) {
   case  '?' :
      CELL_visual   (CHANGE_FORMAT , HIST_BEG, '?');
      CELL_visual   (CHANGE_ALIGN  , HIST_BEG, '?');
      CELL_visual   (CHANGE_DECIMAL, HIST_BEG, '0');
      CELL_visual   (CHANGE_UNITS  , HIST_BEG, '-');
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> keys for formatting sub-mode -------[ ------ [gc.MT0.202.C7]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
api_yvikeys_units       (int a_major, int a_minor)
{
   CELL_visual   (CHANGE_UNITS  , HIST_BEG, a_minor);
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
      if (x_next != NULL && x_next->source != NULL) {
         DEBUG_SRCH   yLOG_char    ("->type"    , x_next->type);
         switch (x_next->type) {
         case YCALC_DATA_STR   :
            DEBUG_SRCH   yLOG_info    ("->source"       , x_next->source);
            rc = yREGEX_exec (x_next->source);
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
            yVIKEYS_srch_found (x_next->label, x_next->tab, x_next->col, x_next->row, 0);
            x_next->note = 's';
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
api_yvikeys_unsearcher   (int b, int x, int y, int z)
{
   /*---(locals)-----------+------+----+-*/
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_SRCH   yLOG_enter   (__FUNCTION__);
   DEBUG_SRCH   yLOG_value   ("b"         , b);
   DEBUG_SRCH   yLOG_value   ("x"         , x);
   DEBUG_SRCH   yLOG_value   ("y"         , y);
   DEBUG_SRCH   yLOG_value   ("z"         , z);
   x_curr = LOC_cell_at_loc (b, x, y);
   DEBUG_SRCH   yLOG_point   ("x_curr"    , x_curr);
   if (x_curr != NULL) x_curr->note = '-';
   DEBUG_SRCH   yLOG_char    ("x_curr->n" , x_curr->note);
   /*---(complete)---------------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     map mode registers                       ----===*/
/*====================------------------------------------====================*/
static void  o___REGISTERS_______o () { return; }

char
api_yvikeys_clearer     (char a_1st, int b, int x, int y, int z)
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
   DEBUG_REGS   yLOG_complex ("coords"    , "%3db, %3dx, %3dy", b, x, y);
   x_curr = LOC_cell_at_loc (b, x, y);
   DEBUG_REGS   yLOG_complex ("x_curr"    , x_curr);
   --rce;  if (x_curr == NULL) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(delete)-------------------------*/
   DEBUG_REGS   yLOG_value   ("x_count"   , x_count);
   if (a_1st == 'y')  rc = CELL_change (NULL, HIST_BEG, b, x, y, "");
   else               rc = CELL_change (NULL, HIST_ADD, b, x, y, "");
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
   DEBUG_REGS   yLOG_point   ("s"         , a_curr->source);
   --rce;  if (a_curr->source == NULL) {
      DEBUG_REGS   yLOG_note    ("no source");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* nothing to write              */
   }
   DEBUG_REGS   yLOG_info    ("s"         , a_curr->source);
   DEBUG_REGS   yLOG_char    ("t"         , a_curr->type);
   --rce;  if (a_curr->type == '-')  {
      DEBUG_REGS   yLOG_note    ("could not copy an empty");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* don't write, recreate on read */
   }
   /*---(check for bounds)---------------*/
   rc = yVIKEYS_mreg_inside (a_curr->tab, a_curr->col, a_curr->row, 0);
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
   rc = yVIKEYS_mreg_add  (x_copy, x_copy->label,'d');
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
   rc      = yVIKEYS_first (&x_tab, &x_col, &x_row, NULL);
   while (rc >= 0) {
      x_curr  = LOC_cell_at_loc (x_tab, x_col, x_row);
      if (x_curr != NULL && !yCALC_stamp_cmp (x_curr->ycalc, s_stamp))   api__yvikeys_copier_one (x_curr, s_stamp);
      rc      = yVIKEYS_next  (&x_tab, &x_col, &x_row, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_yvikeys_paster      (char a_reqs, char a_pros, char a_intg, char a_1st, int a_boff, int a_xoff, int a_yoff, int a_zoff, tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   short       x_stab, x_scol, x_srow;
   short       x_dtab, x_dcol, x_drow;
   char        x_source    [LEN_RECD]   = "";
   char        x_bformat   [LEN_LABEL]  = "";
   tCELL      *x_copy      = NULL;
   char        x_label     [LEN_LABEL]  = "";
   char        x_list      [LEN_RECD]   = "";
   char       *p           = NULL;               /* pointer for providers     */
   char       *q           = ",";                /* delimiter for providers   */
   char       *s           = NULL;               /* context for providers     */
   tCELL      *x_provider  = NULL;               /* provider cell to adjust   */
   tCELL      *x_original  = NULL;
   int         x_nreq, x_npro;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_reqs"    , a_reqs);
   DEBUG_REGS   yLOG_char    ("a_pros"    , a_pros);
   DEBUG_REGS   yLOG_char    ("a_intg"    , a_intg);
   DEBUG_REGS   yLOG_char    ("a_1st"     , a_1st);
   DEBUG_REGS   yLOG_complex ("offset"    , "b=%4d, x=%4d, y=%4d, z=%4d", a_boff, a_xoff, a_yoff, a_zoff);
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
   rc = str2gyges (a_cell->label, &x_stab, &x_scol, &x_srow, NULL, NULL, 0, YSTR_LEGAL);
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0)  {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_complex ("original"  , "tab=%4d, col=%4d, row=%4d", x_stab, x_scol, x_srow);
   /*---(set new location)---------------*/
   x_dtab  = x_stab + a_boff;
   x_dcol  = x_scol + a_xoff;
   x_drow  = x_srow + a_yoff;
   DEBUG_REGS   yLOG_complex ("going to"  , "tab=%4d, col=%4d, row=%4d", x_dtab, x_dcol, x_drow);
   /*---(check cell type)----------------*/
   DEBUG_REGS   yLOG_info    ("source"    , a_cell->source);
   DEBUG_REGS   yLOG_char    ("type"      , a_cell->type);
   strcpy (x_source, "");
   if (strchr (YCALC_GROUP_RPN, a_cell->type) != 0) {
      DEBUG_REGS   yLOG_note    ("formula, calling yRPN_adjust");
      rc = yRPN_addr_require (a_cell->source, a_reqs, a_boff, a_xoff, a_yoff, a_zoff, LEN_RECD, x_source);
      DEBUG_REGS   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         DEBUG_REGS   yLOG_note    ("formula could not be parsed");
         strcpy (x_source, "not_translatable");
      }
   } else {
      DEBUG_REGS   yLOG_note    ("just copy straight across");
      strcpy (x_source, a_cell->source);
   }
   DEBUG_REGS   yLOG_info    ("x_source"  , x_source);
   sprintf (x_bformat, "%c%c%c%c-", a_cell->align, a_cell->format, a_cell->decs, a_cell->unit);
   DEBUG_REGS   yLOG_info    ("x_bformat" , x_bformat);
   if (a_1st == 'y')  x_copy = CELL_overwrite (HIST_BEG, x_dtab, x_dcol, x_drow, x_source, x_bformat);
   else               x_copy = CELL_overwrite (HIST_ADD, x_dtab, x_dcol, x_drow, x_source, x_bformat);
   /*---(providers)----------------------*/
   DEBUG_REGS   yLOG_note    ("CHECK PROVIDERS");
   DEBUG_REGS   yLOG_char    ("a_pros"    , a_pros);
   if (a_pros == G_RPN_IGNORE || a_pros == G_RPN_PNONE) {
      DEBUG_REGS   yLOG_note    ("provider updates not requested");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_REGS   yLOG_complex ("original"  , "tab=%4d, col=%4d, row=%4d", x_stab, x_scol, x_srow);
   x_original = LOC_cell_at_loc (x_stab, x_scol, x_srow);
   DEBUG_REGS   yLOG_point   ("x_original", x_original);
   if (x_original == NULL) {
      DEBUG_REGS   yLOG_note    ("no cell at original location");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   strlcpy (x_label, x_original->label, LEN_LABEL);
   DEBUG_REGS   yLOG_info    ("label"     , x_label);
   yCALC_show_reqs (x_original->ycalc, &x_nreq, NULL);
   DEBUG_REGS   yLOG_value   ("nreq"      , x_nreq);
   yCALC_show_pros (x_original->ycalc, &x_npro, x_list);
   DEBUG_REGS   yLOG_value   ("npro"      , x_npro);
   DEBUG_REGS   yLOG_info    ("x_list"    , x_list);
   DEBUG_REGS   yLOG_complex ("x_original", "%s, nreq=%d, npro=%d", x_label, x_nreq, x_npro);
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
         DEBUG_REGS   yLOG_complex ("details"   , "%s, %3db, %3dx, %3dy", x_provider->label, x_provider->tab, x_provider->col, x_provider->row);
         rc = yVIKEYS_mreg_inside (x_provider->tab, x_provider->col, x_provider->row, 0);
         DEBUG_REGS   yLOG_value   ("rc"        , rc);
         if (rc == 0) {
            DEBUG_REGS   yLOG_info    ("source"    , x_provider->source);
            DEBUG_REGS   yLOG_info    ("change"    , a_cell->label);
            rc = yRPN_addr_provide (x_provider->source, a_pros, a_cell->label, a_boff, a_xoff, a_yoff, a_zoff, LEN_RECD, x_source);
            DEBUG_REGS   yLOG_value   ("rc"        , rc);
            DEBUG_REGS   yLOG_info    ("x_source"  , x_source);
            sprintf (x_bformat, "%c%c%c%c-", x_provider->align, x_provider->format, x_provider->decs, x_provider->unit);
            DEBUG_REGS   yLOG_info    ("x_bformat" , x_bformat);
            CELL_overwrite (HIST_ADD, x_provider->tab, x_provider->col, x_provider->row, x_source, x_bformat);
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
   x_curr = LOC_cell_at_loc (37, 1, x_row);
   --rce;  if (x_curr    == NULL)                           return rce;
   if (x_curr->type == YCALC_DATA_STR) {
      strlcpy (a_macro, x_curr->source    , LEN_RECD);
      return 0;
   }
   if (x_curr->type == YCALC_DATA_SFORM) {
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
      CELL_change (NULL, HIST_NONE , 37, 0, x_row, "");
      CELL_change (NULL, HIST_NONE , 37, 1, x_row, "");
   } else {
      sprintf (t, "%c", a_name);
      CELL_change (NULL, HIST_NONE , 37, 0, x_row, t     );
      CELL_change (NULL, HIST_NONE , 37, 1, x_row, a_keys);
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
   for (i= 0; i < LEN_HUGE; ++i)  a_map->map [i] =  YVIKEYS_EMPTY;
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
   DEBUG_MAP    yLOG_value   ("b->gcur"   , g_bmap.gcur);
   DEBUG_MAP    yLOG_value   ("x->gcur"   , g_xmap.gcur);
   DEBUG_MAP    yLOG_value   ("y->gcur"   , g_ymap.gcur);
   DEBUG_MAP    yLOG_value   ("z->gcur"   , g_zmap.gcur);
   switch (a_dir) {
   case 'T' : case 't' :
      DEBUG_MAP    yLOG_note    ("TABS (z)");
      x_map   = &g_bmap;
      x_max   = MAX_TABS;
      x_total = 1;
      break;
   case 'C' : case 'c' :
      DEBUG_MAP    yLOG_note    ("COLS (x)");
      x_map   = &g_xmap;
      x_max   = NCOL - 1;
      x_total = s_tabs [g_bmap.gcur].rows [g_ymap.gcur].c;
      break;
   case 'R' : case 'r' :
      DEBUG_MAP    yLOG_note    ("ROWS (y)");
      x_map   = &g_ymap;
      x_max   = NROW - 1;
      x_total = s_tabs [g_bmap.gcur].cols [g_xmap.gcur].c;
      break;
   }
   x_mark = x_map->gcur;
   /*---(clear)--------------------------*/
   for (i= 0; i < LEN_HUGE; ++i)  x_map->map [i] =  YVIKEYS_EMPTY;
   x_map->gmin = x_map->gamin = x_map->glmin = x_map->gprev = -1;
   x_map->gmax = x_map->gamax = x_map->glmax = x_map->gnext = -1;
   /*---(do columns)---------------------*/
   for (x_cell = 0; x_cell <= x_max; ++x_cell) {
      /*---(get base data)---------------*/
      switch (a_dir) {
      case 'T' : case 't' :
         x_size  = LEGAL_TAB (x_cell);
         x_curr  = LOC_cell_at_loc (x_cell, g_xmap.gcur, g_ymap.gcur);
         x_count = 1;
         break;
      case 'C' : case 'c' :
         x_size  = s_tabs [g_bmap.gcur].cols [x_cell].w;
         x_curr  = LOC_cell_at_loc (g_bmap.gcur, x_cell, g_ymap.gcur);
         x_count = s_tabs [g_bmap.gcur].cols [x_cell].c;
         break;
      case 'R' : case 'r' :
         x_size  = s_tabs [g_bmap.gcur].rows [x_cell].h;
         x_curr  = LOC_cell_at_loc (g_bmap.gcur, g_xmap.gcur, x_cell);
         x_count = s_tabs [g_bmap.gcur].rows [x_cell].c;
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
   case 't' :
      x_map = &g_bmap;
      strlcpy (x_name, "gyges.tmap", LEN_LABEL);
      break;
   case 'c' :
      x_map = &g_xmap;
      strlcpy (x_name, "gyges.cmap", LEN_LABEL);
      break;
   case 'r' :
      x_map = &g_ymap;
      strlcpy (x_name, "gyges.rmap", LEN_LABEL);
      break;
   }
   /*---(write it out)-------------------*/
   f = fopen (x_name, "w");
   if (f == NULL)  return -1;
   /*---(headers)------------------------*/
   fprintf (f, "gmin amin lmin prev    ");
   for (i = 0; i < LEN_HUGE; ++i) {
      if (x_map->map [i] < 0)  break;
      fprintf (f, "%4d "  , i);
   }
   fprintf (f, "   next lmax amax gmax\n");
   /*---(content)------------------------*/
   fprintf (f, "%4d %4d %4d %4d    "  , x_map->gmin, x_map->gamin, x_map->glmin, x_map->gprev);
   for (i = 0; i < LEN_HUGE; ++i) {
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
   /*---(locals)-----------+-----+-----+-*/
   tCELL      *x_curr      = NULL;
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   yVIKEYS_view_size     (YVIKEYS_MAIN, NULL, &g_xmap.avail, NULL, &g_ymap.avail, NULL);
   if (a_req == YVIKEYS_INIT) {
      LOC__mapper   ('T');
      LOC__mapper   ('C');
      LOC__mapper   ('R');
   } else {
      LOC__mapper   ('t');
      LOC__mapper   ('c');
      LOC__mapper   ('r');
   }
   CTAB   = g_bmap.gcur;
   p_tab  = &s_tabs[CTAB];
   BCOL   = g_xmap.gbeg;
   CCOL   = g_xmap.gcur;
   ECOL   = g_xmap.gend;
   BROW   = g_ymap.gbeg;
   CROW   = g_ymap.gcur;
   EROW   = g_ymap.gend;
   x_curr = LOC_cell_at_curr ();
   if      (x_curr == NULL || x_curr->source == NULL) {
      str4gyges (CTAB, CCOL, CROW, 0, 0, t, YSTR_CHECK);
      yVIKEYS_source (t, "");
   } else {
      yVIKEYS_source (x_curr->label, x_curr->source);
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
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

