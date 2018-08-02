/*============================----beg-of-source---============================*/

/*===[[ DECISION :: FORMAT ]]=================================================*/

/*   PROBLEM (pr)
 *   gyges is a intended to be a powerful, technical, and stable spreadsheet on
 *   which i will constantly depend.  i simply won't have time and energy to
 *   constantly update the file format or desire to debug something complicated.
 *
 *   OBJECTIVES (o)
 *      -- human readable and editable
 *      -- not require an external library or tool to access, read, or convert
 *      -- stable over very long periods
 *      -- only store what gyges requires and nothing more
 *
 *    ALTERNATIVES (act)
 *      a) excel     proprietary, binary, evolving, and complex
 *      b) oocalc    open, xml, compressed, evolving, and complex
 *      c) gnumeric  open, evolving, and complex
 *      d) sylk      open, xml, and complex
 *      e) custom    requires definition and might not work long term
 *
 *   DECISION (ion)
 *   gyges is an order of magnitude simplier that these spreadsheets.  they
 *   each do so much and have tons of complexity, not the least of which is
 *   all the formatting options that i will never have or allow.  the answer
 *   is obvious -- custom and simple.
 *
 */

/*===[[ DESIGN PRINCIPLES ]]==================================================*/

/*   given that i will be away from the code for extended periods,
 *   maintainability requires simplicity and clarity rather than compactness
 *   and brilliance.
 *
 *   the design principles for the file format are...
 *      -- ascii text only that can be read, altered, and created in an editor
 *      -- 7 bit safe (unsigned char) for transportability (values 0 - 127)
 *      -- columnar for readability, even though it wastes space
 *      -- ascii field separator delimited to make column boundaries clear
 *      -- record type fields to aid parsing and interpretation
 *      -- internal comments, headings, and counts for human readability
 *      -- as close to gyges data representation as possible for clarity
 *      -- some additional contextual data if it helps readability
 *
 */



#include   "gyges.h"



/*---(globals)----------+-----------+-*/
char        ver_ctrl    = '-';
char        ver_num     [10]        = "----";
char        ver_txt     [100]       = "-----";

char        s_vers      [LEN_RECD]   = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
char        s_recd      [LEN_RECD];
int         s_len       = 0;
char       *s_p         = NULL;               /* strtok return pointer     */
char       *s_q         = "";               /* strtok delimeters         */
char       *s_context   = NULL;               /* strtok context variable   */
char        s_fields    [20][LEN_RECD];
int         s_nfield    =   0;


char        s_cellbad   = 0;


#define     FILE_BLANK  "untitled"
#define     FILE_SUFFIX "gyges"
static char     f_suffix    [LEN_RECD];      /* file suffix for spreadsheet    */
static FILE    *s_file;                      /* file pointer                   */



/*====================------------------------------------====================*/
/*===----                    reading and writing tabs                  ----===*/
/*====================------------------------------------====================*/
static void   o___TABS____________o (void) { return; }

char
TABS_reader          (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   int         x_size      =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_INPT   yLOG_char    ("version"   , n);
   --rce;  if (n != 'I') {
      DEBUG_INPT   yLOG_note    ("illegal version");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(min)----------------------------*/
   rc = str2gyges (c, &x_tab, &x_col, &x_row, NULL, 0);
   DEBUG_INPT   yLOG_value   ("str2gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(max)----------------------------*/
   rc = str2gyges (d, &x_tab, &x_col, &x_row, NULL, 0);
   DEBUG_INPT   yLOG_value   ("str2gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check tab)----------------------*/
   DEBUG_INPT   yLOG_value   ("x_tab"     , x_tab);
   rc = TAB_valid (x_tab);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(resize)-------------------------*/
   if (x_col == 0)  x_col = COL_max (x_tab);
   DEBUG_INPT   yLOG_value   ("x_col"     , x_col);
   if (x_row == 0)  x_row = ROW_max (x_tab);
   DEBUG_INPT   yLOG_value   ("x_row"     , x_row);
   rc = LOC_legal  (x_col, x_row, x_tab, CELL_EXACT);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT  yLOG_value   ("col_max"   , COL_max (x_tab));
   DEBUG_INPT  yLOG_value   ("row_max"   , ROW_max (x_tab));
   /*---(check name)---------------------*/
   DEBUG_INPT  yLOG_point   ("name"      , b);
   if (b != NULL && strllen (b, LEN_LABEL) > 0) {
      rc = TAB_rename     (x_tab, b);
      DEBUG_INPT  yLOG_value   ("rename"    , rc);
      --rce;  if (rc < 0)  {
         DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_INPT  yLOG_info    ("a_name"    , b);
   }
   /*---(default sizes)------------------*/
   x_size  = atoi (d);
   if (x_size > 0)  COL_defwidth  (x_tab, x_size);
   x_size  = atoi (e);
   if (x_size > 0)  ROW_defheight (x_tab, x_size);
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
TABS_writer           (char  a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   char        c           =    0;
   int         x_btab      =  '-';
   int         x_etab      =  '-';
   char        x_type      =  '-';
   int         x_cols      =    0;
   int         x_rows      =    0;
   char        x_name      [LEN_LABEL];
   int         x_zero      =    0;
   int         x_wide      =    0;
   int         x_tall      =    0;
   char        x_min       [LEN_LABEL];
   char        x_max       [LEN_LABEL];
   /*---(prepare tab)--------------------*/
   yVIKEYS_unit_reset ();
   if (a_tab == -1) {
      x_btab = 0;
      x_etab = MAX_TABS - 1;
   } else {
      rc = TAB_valid (a_tab);
      if (rc < 0)  return rce;
      x_btab = x_etab = a_tab;
   }
   /*---(find marked entries)------------*/
   for (i = x_btab; i <= x_etab; ++i) {
      if (TAB_used (i) <= 0)  continue;
      x_type = TAB_type (i);
      x_cols = COL_max  (i) - 1;
      x_rows = ROW_max  (i) - 1;
      x_wide = TAB_colwide (i);
      x_tall = TAB_rowtall (i);
      TAB_name    (i, x_name);
      rc = str4gyges (0     , 0     , i, 0, x_min);
      rc = str4gyges (x_cols, x_rows, i, 0, x_max);
      yVIKEYS_file_write (FILE_TABS, x_name, x_min, x_max, &x_wide, &x_tall, &x_zero, &x_type, NULL, NULL);
      ++c;
   }
   /*---(complete)-----------------------*/
   return c;
}

char
COLS_reader          (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_size      =    0;
   int         x_count     =    0;
   int         x_cols      =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_INPT   yLOG_char    ("version"   , n);
   --rce;  if (n != 'D') {
      DEBUG_INPT   yLOG_note    ("illegal version");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(tab and col)--------------------*/
   rc = str2gyges (a, &x_col, NULL, &x_tab, NULL, 0);
   DEBUG_INPT   yLOG_value   ("str2gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update size)--------------*/
   x_size   = atoi (b);
   x_count  = atoi (c);
   if (x_count == 0)  x_count = 1;
   for (x_cols = 0; x_cols < x_count; ++x_cols) {
      rc = COL_widen (x_tab, x_col + x_cols, x_size);
      DEBUG_INPT  yLOG_value   ("widen"     , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
DETAIL_writer           (char a_type, char a_tab, int a_cur)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        c           =    0;
   int         n           =    0;
   int         i           =    0;
   int         x_btab      =  '-';
   int         x_etab      =  '-';
   int         j           =    0;
   int         x_beg       =  '-';
   int         x_end       =  '-';
   int         x_max       =  '-';
   int         k           =    0;
   int         x_def       =    0;
   int         x_size      =    0;
   int         x_prev      =    0;
   char        x_label     [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(clear output)-------------------*/
   yVIKEYS_unit_reset ();
   /*---(prepare tab)--------------------*/
   DEBUG_OUTP   yLOG_value   ("a_tab"     , a_tab);
   --rce;  if (a_tab == -1) {
      x_btab = 0;
      x_etab = MAX_TABS - 1;
   } else {
      rc = TAB_valid (a_tab);
      if (rc < 0) { 
         DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_btab = x_etab = a_tab;
   }
   DEBUG_OUTP   yLOG_value   ("x_btab"    , x_btab);
   DEBUG_OUTP   yLOG_value   ("x_etab"    , x_etab);
   /*---(find marked entries)------------*/
   for (i = x_btab; i <= x_etab; ++i) {
      if (TAB_used (i) <= 0)  continue;
      DEBUG_OUTP   yLOG_value   ("tab (i)"   , i);
      /*---(prepare range)------------------*/
      DEBUG_OUTP   yLOG_value   ("a_cur"     , a_cur);
      --rce;  if (a_cur == -1) {
         x_beg = 0;
         if (a_type == 'c')  x_end = COL_max  (i) - 1;
         else                x_end = ROW_max  (i) - 1;
      } else {
         if (a_type == 'c')  rc = COL_valid (a_tab, a_cur);
         else                rc = ROW_valid (a_tab, a_cur);
         if (rc < 0) {
            DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         x_beg = x_end = a_cur;
      }
      DEBUG_OUTP   yLOG_value   ("x_beg"     , x_beg);
      DEBUG_OUTP   yLOG_value   ("x_end"     , x_end);
      /*---(prepare default)----------------*/
      if (a_type == 'c')  x_def  = TAB_colwide (i);
      else                x_def  = TAB_rowtall (i);
      DEBUG_OUTP   yLOG_value   ("x_def"     , x_def);
      /*---(run range)----------------------*/
      for (j = x_beg; j <= x_end; ++j) {
         if (a_type == 'c')  DEBUG_OUTP   yLOG_value   ("col (j)"   , j);
         if (a_type == 'r')  DEBUG_OUTP   yLOG_value   ("row (j)"   , j);
         if (COL_valid (i, j) < 0)  continue;
         if      (a_type == 'c' && j > 0)  x_prev = COL_width  (i, j - 1);
         else if (a_type == 'r' && j > 0)  x_prev = ROW_height (i, j - 1);
         else                              x_prev = -1;
         DEBUG_OUTP   yLOG_value   ("x_prev"    , x_prev);
         if (a_type == 'c')  x_size = COL_width  (i, j);
         else                x_size = ROW_height (i, j);
         DEBUG_OUTP   yLOG_value   ("x_size"    , x_size);
         if (x_size == x_prev)  continue;
         if (x_size == x_def )  continue;
         n = 1;
         if (a_type == 'c')  x_max = COL_max (i) - 1;
         else                x_max = ROW_max (i) - 1;
         for (k = j + 1; k <= x_max; ++k) {
            if (a_type == 'c' && x_size != COL_width  (i, k))  break;
            if (a_type == 'r' && x_size != ROW_height (i, k))  break;
            ++n;
         }
         DEBUG_OUTP   yLOG_value   ("n"         , n);
         if (a_type == 'c') {
            rc = str4gyges (j, 0, i, 0, x_label);
            yVIKEYS_file_write (FILE_COLS, x_label, &x_size, &n, NULL, NULL, NULL, NULL, NULL, NULL);
         }
         else {
            rc = str4gyges (0, j, i, 0, x_label);
            yVIKEYS_file_write (FILE_ROWS, x_label, &x_size, &n, NULL, NULL, NULL, NULL, NULL, NULL);
         }
         c += n;
         j += n - 1;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return c;
}

char
ROWS_reader          (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_row       =    0;
   int         x_size      =    0;
   int         x_count     =    0;
   int         x_rows      =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_INPT   yLOG_char    ("version"   , n);
   --rce;  if (n != 'D') {
      DEBUG_INPT   yLOG_note    ("illegal version");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(tab and col)--------------------*/
   rc = str2gyges (a, NULL, &x_row, &x_tab, NULL, 0);
   DEBUG_INPT   yLOG_value   ("str2gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update size)--------------*/
   x_size   = atoi (b);
   x_count  = atoi (c);
   if (x_count == 0)  x_count = 1;
   for (x_rows = 0; x_rows < x_count; ++x_rows) {
      rc = ROW_heighten (x_tab, x_row + x_rows, x_size);
      DEBUG_INPT  yLOG_value   ("heigten"   , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char TABS_writer_all (void) { return TABS_writer    (-1);          }
char COLS_writer_all (void) { return DETAIL_writer  ('c', -1, -1); }
char ROWS_writer_all (void) { return DETAIL_writer  ('r', -1, -1); }




/*====================------------------------------------====================*/
/*===----                    reading and writing cells                 ----===*/
/*====================------------------------------------====================*/
PRIV void  o___CELLS___________o () { return; }

static int s_stamp     = 0;

char         /*-> write a single cell to a file ------[ leaf   [ge.630.514.50]*/ /*-[02.0000.124.!]-*/ /*-[--.---.---.--]-*/
OUTP_cell          (char a_type, int a_seq, int a_level, tCELL *a_curr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_format    [LEN_LABEL] = "";
   char        x_level     [LEN_LABEL] = "";
   char        t           [LEN_RECD]  = "";
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_OUTP   yLOG_point   ("a_curr"    , a_curr);
   --rce;  if (a_curr  == NULL) {
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_OUTP   yLOG_point   ("label"     , a_curr->label);
   --rce;  if (a_curr->label  == NULL) {
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_OUTP   yLOG_info    ("label"     , a_curr->label);
   DEBUG_OUTP   yLOG_complex ("type"      , "%-3d (%c)", a_curr->t, a_curr->t);
   if (a_curr->t == '-') {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_OUTP   yLOG_point   ("source"    , a_curr->s);
   if (a_curr->s == NULL) {      
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_OUTP   yLOG_info    ("source"    , a_curr->s);
   /*---(level)--------------------------*/
   DEBUG_OUTP   yLOG_value   ("a_level"   , a_level);
   if      (a_level <=  0)   sprintf (x_level, ".         ");
   else if (a_level <  10)   sprintf (x_level, "%-*.*s%d%-10.10s", a_level, a_level, "------------------------", a_level, " ");
   else                      sprintf (x_level, "     +++%-2d", a_level);
   /*---(format)-------------------------*/
   DEBUG_OUTP   yLOG_complex ("format"    , "%-3d (%c)", a_curr->f, a_curr->f);
   DEBUG_OUTP   yLOG_complex ("decs"      , "%-3d (%c)", a_curr->d, a_curr->d);
   DEBUG_OUTP   yLOG_complex ("align"     , "%-3d (%c)", a_curr->a, a_curr->a);
   DEBUG_OUTP   yLOG_complex ("note"      , "%-3d (%c)", a_curr->n, a_curr->n);
   sprintf (x_format, "%c %c %c %c %c", a_curr->t, a_curr->f, a_curr->d, a_curr->a, a_curr->n);
   /*---(call writer)--------------------*/
   strlcpy  (t, a_curr->s, LEN_RECD);
   strldchg (t, G_KEY_SPACE, G_CHAR_SPACE, LEN_RECD);
   yVIKEYS_file_write (a_type, x_level, &a_seq, a_curr->label, x_format, t, NULL, NULL, NULL, NULL);
   /*---(complete)-----------------------*/
   DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
OUTP_seq_cell           (void *a_owner, void *a_deproot, int a_seq, int a_level)
{
   OUTP_cell (FILE_DEPCEL, a_seq, a_level, (tCELL *) a_owner);
}

char         /*-> write file independent cells -------[ ------ [ge.9A0.86#.I4]*/ /*-[02.0000.01#.!]-*/ /*-[--.---.---.--]-*/
OUTP_cell_free          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x           =    0;             /* iterator -- columns            */
   int         x_end       =    0;
   int         y           =    0;             /* iterator -- row                */
   int         y_end       =    0;
   int         z           =    0;             /* iterator -- tabs               */
   tCELL      *x_curr      = NULL;
   int         x_seq       =    0;
   /*---(cells)--------------------------*/
   for (z = 0; z < NTAB; ++z) {
      x_end = COL_max (z) - 1;
      y_end = ROW_max (z) - 1;
      for (x = 0; x <= x_end; ++x) {
         for (y = 0; y <= y_end; ++y) {
            x_curr = LOC_cell_at_loc (x, y, z);
            if (x_curr    == NULL)                         continue;
            if (x_curr->s == NULL)                         continue;
            if (x_curr->t == YCALC_DATA_BLANK)             continue;
            if (yCALC_getstamp (x_curr->ycalc) == s_stamp) continue;
            OUTP_cell (FILE_FREECEL, x_seq++, -1, x_curr);
         }
      }
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
OUTP_cell_dep           (void)
{
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   s_stamp   = rand ();
   rc = yCALC_seq_downup (s_stamp, OUTP_seq_cell);
   DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
INPT_cell            (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   char        x_string    [LEN_LABEL] = "?0?";
   tCELL      *x_new       = NULL;
   char        x_format    =  '?';
   char        x_decs      =  '0';
   char        x_align     =  '?';
   /*> int         x_index     =    0;                                                <*/
   /*> char        x_label     [LEN_LABEL] = "";                                      <*/
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_INPT   yLOG_char    ("version"   , n);
   --rce;  if (strchr ("D", n) == NULL) {
      DEBUG_INPT   yLOG_note    ("illegal version");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(parse address)------------*/
   DEBUG_INPT  yLOG_point   ("label"     , c);
   --rce;  if (*c  == NULL)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = LOC_parse (c, &x_tab, &x_col, &x_row, NULL);
   DEBUG_INPT  yLOG_value   ("parse"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT  yLOG_info    ("label"     , c);
   /*---(get position)-------------------*/
   DEBUG_INPT  yLOG_value   ("x_col"     , x_col);
   DEBUG_INPT  yLOG_value   ("x_row"     , x_row);
   DEBUG_INPT  yLOG_value   ("x_tab"     , x_tab);
   /*---(expand everything as needed)----*/
   rc = LOC_legal (x_col, x_row, x_tab, CELL_GROW);
   DEBUG_INPT  yLOG_value   ("legal"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(fix format)---------------------*/
   if (d == NULL) {
      strcpy  (x_string, "??0");
   } else if (strlen (d) == 9) {
      x_format  = CELL_format_valid   (d [2]);
      DEBUG_INPT  yLOG_char    ("x_format"  , x_format);
      x_decs    = CELL_decimals_valid (d [4]);
      DEBUG_INPT  yLOG_char    ("x_decs"    , x_decs);
      x_align   = CELL_align_valid    (d [6]);
      DEBUG_INPT  yLOG_char    ("x_align"   , x_align);
      sprintf (x_string, "%c%c%c", x_format, x_align, x_decs);
   } else {
      strcpy  (x_string, "??0");
   }
   DEBUG_INPT  yLOG_info    ("x_string"  , x_string);
   /*---(update)-------------------------*/
   DEBUG_INPT  yLOG_info    ("source"    , e);
   strldchg (e, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
   x_new = CELL_overwrite (CHG_NOHIST, x_tab, x_col, x_row, e, x_string);
   DEBUG_INPT  yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*> char         /+-> tbd --------------------------------[ ------ [gc.320.121.32]+/ /+-[00.0000.00#.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> FILE_writeas         (char *a_name)                                                                                            <* 
 *> {                                                                                                                              <* 
 *>    char        rc          =    0;                                                                                             <* 
 *>    char        x_name      [LEN_RECD]  = "";                                                                                   <* 
 *>    strlcpy (x_name, my.f_name, LEN_RECD);                                                                                      <* 
 *>    if (rc >= 0)  rc = FILE_rename (a_name);                                                                                    <* 
 *>    if (rc >= 0)  rc = FILE_write  ();                                                                                          <* 
 *>    if (rc >= 0)  rc = FILE_rename (x_name);                                                                                    <* 
 *>    return rc;                                                                                                                  <* 
 *> }                                                                                                                              <*/



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNIT_TEST_______o () { return; }

char*        /*-> unit test accessor -----------------[ ------ [gs.950.221.M1]*/ /*-[03.0000.00#.#]-*/ /*-[--.---.---.--]-*/
FILE_unit          (char *a_question, int a_ref)
{
   /*---(locals)-----------+-----------+-*/
   int         rc          = -1;
   char        x_name      [LEN_LABEL];
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_file           : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "ver_num"   )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file ver_num   : %s", ver_num);
   } else if (strcmp (a_question, "version"   )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file version   : %c %-4.4s %s", ver_ctrl, ver_num, ver_txt);
   } else if (strcmp (a_question, "recd"      )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file recd      : %s", my.f_recd);
   } else if (strcmp (a_question, "freeze"    )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file freeze    : col=%c (%4d to %4d)   row=%c (%4d to %4d)",
            FR_COL, FR_BCOL, FR_ECOL, FR_ROW, FR_BROW, FR_EROW);
   } else if (strcmp (a_question, "tab_name"  )    == 0) {
      TAB_name (a_ref, x_name);
      snprintf (unit_answer, LEN_UNIT, "s_file tab name  : tab=%4d, act=%c, :%s:", a_ref, 'y', x_name);
   } else if (strcmp (a_question, "tab_count" )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file tab count : ntab=%4d", MAX_TABS);
   } else if (strcmp (a_question, "history"   )    == 0) {
      if      (nhist == 0    )  snprintf (unit_answer, LEN_UNIT, "s_file history   : n=%4d, c=%4d, n/a", nhist, chist);
      if      (chist <  0    )  snprintf (unit_answer, LEN_UNIT, "s_file history   : n=%4d, c=%4d, n/a", nhist, chist);
      else                      snprintf (unit_answer, LEN_UNIT, "s_file history   : n=%4d, c=%4d, %s" , nhist, chist, hist[chist].act);
   } else if (strcmp (a_question, "entry"     )    == 0) {
      if      (a_ref <  0    )  snprintf (unit_answer, LEN_UNIT, "s_file entry     : %4d too small", a_ref);
      else if (a_ref >= nhist)  snprintf (unit_answer, LEN_UNIT, "s_file entry     : %4d too large", a_ref);
      else                      snprintf (unit_answer, LEN_UNIT, "s_file entry     : %4d, t=%4d, c=%4d, r=%4d, %s", a_ref, hist[a_ref].btab, hist[a_ref].bcol, hist[a_ref].brow, hist[a_ref].act);
   } else if (strcmp (a_question, "before"    )    == 0) {
      if      (a_ref <  0    )  snprintf (unit_answer, LEN_UNIT, "s_file before    : %4d too small", a_ref);
      else if (a_ref >= nhist)  snprintf (unit_answer, LEN_UNIT, "s_file before    : %4d too large", a_ref);
      else                      snprintf (unit_answer, LEN_UNIT, "s_file before    : %4d :%s:", a_ref, hist[a_ref].before);
   } else if (strcmp (a_question, "after"     )    == 0) {
      if      (a_ref <  0    )  snprintf (unit_answer, LEN_UNIT, "s_file after     : %4d too small", a_ref);
      else if (a_ref >= nhist)  snprintf (unit_answer, LEN_UNIT, "s_file after     : %4d too large", a_ref);
      else                      snprintf (unit_answer, LEN_UNIT, "s_file after     : %4d :%s:", a_ref, hist[a_ref].after);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}


/*============================----end-of-source---============================*/
