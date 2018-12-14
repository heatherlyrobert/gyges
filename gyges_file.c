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




char
FILE_init               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(handlers)-----------------------*/
   /*> rc = yPARSE_handler (FILE_TABS    , "tab"       , 4.1, "NLLsssc-----", TABS_writer_all , TABS_reader     , "------------" , "name,min,max,x_def,y_def,z_def,type"  , "gyges tabs (v-axis)"      );   <*/
   /*> rc = yPARSE_handler (FILE_ROWS    , "height"    , 4.3, "Lss---------", ROWS_writer_all , ROWS_reader     , "------------" , "label,size,count"                     , "gyges rows (y-axis)"      );   <*/
   rc = yPARSE_handler (FILE_DEPCEL  , "cell_dep"  , 5.1, "TiLTO-------", OUTP_cell_dep   , INPT_cell       , "------------" , "lvl/reg,seq,label,t-f-d-a-m,contents" , "gyges dependent cells"    );
   rc = yPARSE_handler (FILE_FREECEL , "cell_free" , 5.2, "TiLTO-------", OUTP_cell_free  , INPT_cell       , "------------" , "lvl/reg,seq,label,t-f-d-a-m,contents" , "gyges independent cells"  );

   /*> if (rc == 0)  rc = yVIKEYS_file_add     (FILE_DEPCEL , OUTP_cell_dep  , INPT_cell);     <* 
    *> if (rc == 0)  rc = yVIKEYS_file_add     (FILE_FREECEL, OUTP_cell_free , INPT_cell);     <* 
    *> if (rc == 0)  rc = yVIKEYS_file_add     (FILE_TABS   , TABS_writer_all, TABS_reader);   <* 
    *> if (rc == 0)  rc = yVIKEYS_file_add     (FILE_COLS   , COLS_writer_all, COLS_reader);   <* 
    *> if (rc == 0)  rc = yVIKEYS_file_add     (FILE_ROWS   , ROWS_writer_all, ROWS_reader);   <*/

   /*> { 'e', FILE_DEPCEL , "dependent cells"    , "cell_dep"  , 'D', "TiaTO-----", "lvl/reg"   , "seq"       , "label"     , "t-f-d-a-m" , "contents"  , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },   <* 
    *> { 'e', FILE_FREECEL, "independent cells"  , "cell_free" , 'D', "TiaTO-----", "lvl/reg"   , "seq"       , "label"     , "t-f-d-a-m" , "contents"  , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },   <* 
    *> { 'e', FILE_TABS   , "tab (v-axis)"       , "tab"       , 'I', "Naaiiic---", "name"      , "min"       , "max"       , "x_size"    , "y_size"    , "z_size"    , "type"      , ""          , ""          , NULL  , NULL  ,   0,   0 },   <* 
    *> { 'e', FILE_COLS   , "columns (x-axis)"   , "width"     , 'D', "aii-------", "label"     , "size"      , "count"     , ""          , ""          , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },   <* 
    *> { 'e', FILE_ROWS   , "rows (y-axis)"      , "height"    , 'D', "aii-------", "label"     , "size"      , "count"     , ""          , ""          , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },   <*/
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    reading and writing tabs                  ----===*/
/*====================------------------------------------====================*/
static void   o___TABS____________o (void) { return; }

char
TABS_reader_OLD      (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
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
TABS_writer_OLD       (char  a_tab)
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
      rc = str4gyges (i, 0     , 0     , 0, x_min);
      rc = str4gyges (i, x_cols, x_rows, 0, x_max);
      /*> yVIKEYS_file_write (FILE_TABS, x_name, x_min, x_max, &x_wide, &x_tall, &x_zero, &x_type, NULL, NULL);   <*/
      yPARSE_fullwrite ("tab", x_name, x_min, x_max, x_wide, x_tall, x_zero, x_type);
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
   rc = str2gyges (a, &x_tab, &x_col, NULL, NULL, 0);
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
            rc = str4gyges (i, j, 0, 0, x_label);
            yVIKEYS_file_write (FILE_COLS, x_label, &x_size, &n, NULL, NULL, NULL, NULL, NULL, NULL);
         }
         else {
            rc = str4gyges (i, 0, j, 0, x_label);
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
   rc = str2gyges (a, &x_tab, NULL, &x_row, NULL, 0);
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

/*> char TABS_writer_all (void) { return TABS_writer    (-1);          }              <*/
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
   rc = str2gyges (c, &x_tab, &x_col, &x_row, NULL, 0);
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
/*===----                         import/export                        ----===*/
/*====================------------------------------------====================*/
static void  o___IMPORT__________o () { return; }

static FILE   *s_clip     = NULL;
static int     s_lines    =    0;

static int     s_max      =    0;

static char    s_sizer    =  '-';
static char    s_style    =  '-';
static char    s_mapper   =  '-';
static short   s_map      [MAX_COLS];

static short   s_icol     =    0;
static short   s_mcol     =    0;
static short   s_ccol     =    0;
static short   s_crow     =    0;

/*> char         /+-> prepare for data import ------------[ leaf   [fe.F83.123.30]+/ /+-[01.0000.014.X]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_prep       (char a_style)                                                                                            <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                    <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    int         i           =    0;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(save style)---------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("save style");                                                                                   <* 
 *>    s_style  = a_style;                                                                                                         <* 
 *>    /+---(clear sizer)--------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("clear sizer");                                                                                  <* 
 *>    s_sizer  = '-';                                                                                                             <* 
 *>    /+---(clear sizer)--------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("clear map");                                                                                    <* 
 *>    s_mapper = '-';                                                                                                             <* 
 *>    for (i = 0; i < MAX_COLS; ++i)  s_map [i] = 0;                                                                              <* 
 *>    /+---(prepare)------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("prepare positions");                                                                            <* 
 *>    s_icol  =    0;                                                                                                             <* 
 *>    s_mcol  =    0;                                                                                                             <* 
 *>    s_ccol  =    0;                                                                                                             <* 
 *>    s_crow  =    0;                                                                                                             <* 
 *>    s_lines =    0;                                                                                                             <* 
 *>    /+---(delimiter)----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("set delimiter");                                                                                <* 
 *>    switch (s_style) {                                                                                                          <* 
 *>    case 's'  : case 'S'  :                                                                                                     <* 
 *>       strlcpy (s_q, " "  , 5);                                                                                                 <* 
 *>       break;                                                                                                                   <* 
 *>    case 'c'  : case 'C'  :                                                                                                     <* 
 *>       strlcpy (s_q, ","  , 5);                                                                                                 <* 
 *>       break;                                                                                                                   <* 
 *>    case 't'  : case 'T'  :                                                                                                     <* 
 *>       strlcpy (s_q, "\t" , 5);                                                                                                 <* 
 *>       break;                                                                                                                   <* 
 *>    case 'r'  : case 'R'  :                                                                                                     <* 
 *>       strlcpy (s_q, "" , 5);                                                                                                  <* 
 *>       break;                                                                                                                   <* 
 *>    case 'v'  : case 'V'  :                                                                                                     <* 
 *>       strlcpy (s_q, " "  , 5);                                                                                                 <* 
 *>       break;                                                                                                                   <* 
 *>    default   :                                                                                                                 <* 
 *>       return rce;                                                                                                              <* 
 *>       break;                                                                                                                   <* 
 *>    }                                                                                                                           <* 
 *>    /+---(sizer)--------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("set sizer value");                                                                              <* 
 *>    switch (s_style) {                                                                                                          <* 
 *>    case 'T' : case 'C' : case 'R' : case 'V' : case 'S' :                                                                      <* 
 *>       s_sizer = 'a';                                                                                                           <* 
 *>       break;                                                                                                                   <* 
 *>    case 'v' :                                                                                                                  <* 
 *>       s_sizer = 'f';                                                                                                           <* 
 *>       break;                                                                                                                   <* 
 *>    default  :                                                                                                                  <* 
 *>       s_sizer = '-';                                                                                                           <* 
 *>       break;                                                                                                                   <* 
 *>    }                                                                                                                           <* 
 *>    /+---(open output file)---------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("open clip file");                                                                               <* 
 *>    s_clip = fopen("/root/z_gehye/vi_clip.txt", "r");                                                                           <* 
 *>    DEBUG_REGS   yLOG_point   ("s_clip"    , s_clip);                                                                           <* 
 *>    --rce;  if (s_clip == NULL) {                                                                                               <* 
 *>       DEBUG_REGS   yLOG_note    ("can not open clip file");                                                                    <* 
 *>       DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
*>    return 0;                                                                                                                   <* 
*> }                                                                                                                              <*/

/*> char         /+-> process a mapping request ----------[ ------ [fe.943.044.31]+/ /+-[02.0000.014.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_map        (void)                                                                                                    <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----+-----+-----+-----+-+/                                                                                    <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    int         i           =    0;                                                                                             <* 
 *>    tCELL      *x_curr      = NULL;                                                                                             <* 
 *>    short       x_ccol      =    0;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    DEBUG_REGS   yLOG_char    ("s_style"   , s_style);                                                                          <* 
 *>    if (s_style == 'v') {                                                                                                       <* 
 *>       DEBUG_REGS   yLOG_note    ("mapping not allowed in this import style");                                                  <* 
 *>       DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                <* 
 *>       return 0;                                                                                                                <* 
 *>    }                                                                                                                           <* 
 *>    /+---(import mapping)-----------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("start mapping");                                                                                <* 
 *>    for (i = 0; i < MAX_COLS; ++i) {                                                                                            <* 
 *>       x_ccol = CCOL + i;                                                                                                       <* 
 *>       DEBUG_REGS   yLOG_value   ("x_ccol"    , x_ccol);                                                                        <* 
 *>       x_curr = LOC_cell_at_loc (x_ccol, CROW, CTAB);                                                                           <* 
 *>       DEBUG_REGS   yLOG_point   ("x_curr"    , x_curr);                                                                        <* 
 *>       if (x_curr == NULL)  {                                                                                                   <* 
 *>          DEBUG_REGS   yLOG_note    ("end of mapping (null cell)");                                                             <* 
 *>          DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>       DEBUG_REGS   yLOG_value   ("v_num"     , x_curr->v_num);                                                                 <* 
 *>       if (x_curr->v_num <= 0) {                                                                                                <* 
 *>          DEBUG_REGS   yLOG_note    ("end of mapping (zero or negative cell)");                                                 <* 
 *>          DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>       s_mapper = 'y';                                                                                                          <* 
 *>       s_crow   = CROW + 1;                                                                                                     <* 
 *>       s_map [i] = x_curr->v_num;                                                                                               <* 
 *>       DEBUG_REGS   yLOG_pair    (i          , s_map [i] );                                                                     <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> read an import record --------------[ leaf   [fe.A34.026.50]+/ /+-[01.0001.014.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_read       (void)                                                                                                    <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----+-----+-----+-----+-+/                                                                                    <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    int         x_len       =    0;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(read and clean)-----------------+/                                                                                    <* 
 *>    ++s_lines;                                                                                                                  <* 
 *>    DEBUG_REGS   yLOG_value   ("line"      , s_lines);                                                                          <* 
 *>    fgets (s_recd, LEN_RECD, s_clip);                                                                                           <* 
 *>    if (feof (s_clip))  {                                                                                                       <* 
 *>       DEBUG_REGS   yLOG_note    ("end of file reached");                                                                       <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    x_len = strlen (s_recd);                                                                                                    <* 
 *>    if (x_len <= 0)  {                                                                                                          <* 
 *>       DEBUG_REGS   yLOG_note    ("record is empty");                                                                           <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);                                                                          <* 
 *>       return -rce;                                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    s_recd [--x_len] = '\0';                                                                                                    <* 
 *>    DEBUG_REGS   yLOG_value   ("length"    , x_len);                                                                            <* 
 *>    DEBUG_REGS   yLOG_info    ("fixed"     , s_recd);                                                                           <* 
 *>    if (s_recd [0] == '\0') {                                                                                                   <* 
 *>       DEBUG_REGS   yLOG_note    ("starts with a null");                                                                        <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);                                                                          <* 
 *>       return -rce;                                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    if (s_recd [0] == ' ' ) {                                                                                                   <* 
 *>       DEBUG_REGS   yLOG_note    ("starts with a space");                                                                       <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);                                                                          <* 
 *>       return -rce;                                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    if (s_recd [0] == '#' && strchr ("<>=", s_recd [1]) == NULL) {                                                              <* 
 *>       DEBUG_REGS   yLOG_note    ("starts with a comment marker");                                                              <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);                                                                          <* 
 *>       return -rce;                                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    s_max = x_len;                                                                                                              <* 
 *>    DEBUG_REGS   yLOG_value   ("s_max"     , s_max);                                                                            <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> adjust a column sizing -------------[ ------ [fe.420.223.22]+/ /+-[00.0000.015.B]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_width      (short a_col, short a_wide)                                                                               <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----+-----+-----+-----+-+/                                                                                    <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    short       w           =   0;                                                                                              <* 
 *>    /+---(defense)------------------------+/                                                                                    <* 
 *>    --rce;  if (s_sizer != 'a')      return rce;                                                                                <* 
 *>    /+---(resize)-------------------------+/                                                                                    <* 
 *>    w = COL_width (CTAB, CCOL + a_col);                                                                                     <* 
 *>    --rce;  if (w >= a_wide + 1)     return rce;                                                                                <* 
 *>    LOC_col_widen (CTAB, CCOL + a_col, a_wide + 1);                                                                             <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> process a column sizing request ----[ ------ [fe.A53.053.51]+/ /+-[02.0000.014.E]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_sizer      (void)                                                                                                    <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                    <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    char       *p           = NULL;                                                                                             <* 
 *>    char       *s           = NULL;                                                                                             <* 
 *>    int         x_len       =    0;                                                                                             <* 
 *>    short       x_col       =    0;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(defense)------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_value   ("s_sizer"   , s_sizer);                                                                          <* 
 *>    --rce;  if (s_sizer != 'a') {                                                                                               <* 
 *>       DEBUG_REGS   yLOG_note    ("wrong type, only 'a' allowed");                                                              <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    DEBUG_REGS   yLOG_info    ("s_recd"    , s_recd);                                                                           <* 
 *>    --rce;  if ((s_style != 'C' && strncmp ("#<34", s_recd, 4) != 0) ||                                                         <* 
 *>          (s_style == 'C' && strncmp ("\"#<34", s_recd, 5) != 0)) {                                                             <* 
 *>       DEBUG_REGS   yLOG_note    ("not a sizing line");                                                                         <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(process)------------------------+/                                                                                    <* 
 *>    p  = strtok_r (s_recd, s_q, &s);                                                                                            <* 
 *>    while (p != NULL) {                                                                                                         <* 
 *>       DEBUG_REGS   yLOG_value   ("x_col"     , x_col);                                                                         <* 
 *>       x_len = strlen (p);                                                                                                      <* 
 *>       if (s_style == 'C' && x_len >= 3)  p [0] = p [x_len - 1] = ' ';                                                          <* 
 *>       strltrim (p, ySTR_BOTH, LEN_RECD);                                                                                       <* 
 *>       DEBUG_REGS   yLOG_info    ("p (field)" , p);                                                                             <* 
 *>       x_len = strlen (p);                                                                                                      <* 
 *>       DEBUG_REGS   yLOG_value   ("x_len"     , x_len);                                                                         <* 
 *>       if (x_len == 0)  break;  /+ stop at unsized column +/                                                                    <* 
 *>       LOC_col_widen (CTAB, CCOL + x_col, x_len + 1);                                                                           <* 
 *>       ++x_col;                                                                                                                 <* 
 *>       p  = strtok_r (NULL, s_q, &s);                                                                                           <* 
 *>    }                                                                                                                           <* 
 *>    /+---(update)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("mark sizer processed");                                                                         <* 
 *>    s_sizer  = 's';                                                                                                             <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> place import data into a cell ------[ ------ [fz.310.301.21]+/ /+-[01.0000.025.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_place      (short a_col, short a_row, char *a_value)                                                                 <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                    <* 
 *>    /+---(non-mapped)---------------------+/                                                                                    <* 
 *>    if (s_mapper == '-') {                                                                                                      <* 
 *>       CELL_change (NULL, CHG_INPUT, CTAB, CCOL + a_col, CROW + a_row, a_value);                                                <* 
 *>    }                                                                                                                           <* 
 *>    /+---(mapped)-------------------------+/                                                                                    <* 
 *>    else {                                                                                                                      <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> complete data import ---------------[ ------ [fz.311.001.02]+/ /+-[00.0000.014.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_done       (void)                                                                                                    <* 
 *> {                                                                                                                              <* 
 *>    /+---(close file)---------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("closing file");                                                                                 <* 
 *>    fclose  (s_clip);                                                                                                           <* 
 *>    /+---(screen)-------------------------+/                                                                                    <* 
 *>    /+> CURS_screen_reset ();                                                          <+/                                      <* 
 *>    /+---(calculate)----------------------+/                                                                                    <* 
 *>    DEBUG_INPT  yLOG_note    ("recalc");                                                                                        <* 
 *>    SEQ_calc_full ();                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ ------ [fz.732.141.12]+/ /+-[01.0000.014.3]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_values     (short a_row)                                                                                             <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----+-----+-----+-----+-+/                                                                                    <* 
 *>    short       x_col       =    0;                                                                                             <* 
 *>    int         w           =    0;                                                                                             <* 
 *>    int         cw          =    0;                                                                                             <* 
 *>    char        t           [LEN_RECD]  = "";                                                                                   <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(process cells)---------------+/                                                                                       <* 
 *>    while (cw < s_max) {                                                                                                        <* 
 *>       /+---(read import data)---------+/                                                                                       <* 
 *>       w     = COL_width (CTAB, CCOL + x_col);                                                                              <* 
 *>       DEBUG_REGS  yLOG_value   ("w"         , w);                                                                              <* 
 *>       strlcpy (t, s_recd + cw, w);                                                                                             <* 
 *>       DEBUG_REGS  yLOG_info    ("t (orig)"  , t);                                                                              <* 
 *>       /+---(process)------------------+/                                                                                       <* 
 *>       strltrim (t, ySTR_BOTH, LEN_RECD);                                                                                       <* 
 *>       DEBUG_REGS  yLOG_info    ("t (trim)"  , t);                                                                              <* 
 *>       strlencode   (t, ySTR_NORM, LEN_RECD);                                                                                   <* 
 *>       DEBUG_REGS  yLOG_info    ("t (new)"   , t);                                                                              <* 
 *>       DEBUG_REGS  yLOG_value   ("x_col"     , x_col);                                                                          <* 
 *>       /+---(change)-------------------+/                                                                                       <* 
 *>       REG__inpt_place  (x_col, a_row, t);                                                                                      <* 
 *>       /+---(next)---------------------+/                                                                                       <* 
 *>       ++x_col;                                                                                                                 <* 
 *>       cw += w;                                                                                                                 <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ ------ [fz.842.141.32]+/ /+-[01.0000.014.9]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_delims     (short a_row)                                                                                             <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----+-----+-----+-----+-+/                                                                                    <* 
 *>    short       x_col       =    0;                                                                                             <* 
 *>    char       *p           = NULL;                                                                                             <* 
 *>    char       *s           = NULL;                                                                                             <* 
 *>    int         x_len       =    0;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(parse first)-----------------+/                                                                                       <* 
 *>    p  = strtok_r (s_recd, s_q, &s);                                                                                            <* 
 *>    /+---(process cells)---------------+/                                                                                       <* 
 *>    while (p != NULL) {                                                                                                         <* 
 *>       /+---(read import data)---------+/                                                                                       <* 
 *>       DEBUG_REGS  yLOG_info    ("p (orig)"  , p);                                                                              <* 
 *>       x_len = strlen (p);                                                                                                      <* 
 *>       DEBUG_REGS  yLOG_value   ("x_len"     , x_len);                                                                          <* 
 *>       /+---(process)------------------+/                                                                                       <* 
 *>       if (s_style == 'c' && x_len >= 3)  p [0] = p [x_len - 1] = ' ';                                                          <* 
 *>       strltrim (p, ySTR_BOTH, LEN_RECD);                                                                                       <* 
 *>       DEBUG_REGS  yLOG_info    ("p (trim)"  , p);                                                                              <* 
 *>       x_len = strlen (p);                                                                                                      <* 
 *>       DEBUG_REGS  yLOG_value   ("x_len"     , x_len);                                                                          <* 
 *>       if (s_sizer == 'a')  REG__inpt_width (x_col, x_len);                                                                     <* 
 *>       strlencode   (p, ySTR_NORM, LEN_RECD);                                                                                   <* 
 *>       DEBUG_REGS  yLOG_info    ("p (new)"   , p);                                                                              <* 
 *>       DEBUG_REGS  yLOG_value   ("x_col"     , x_col);                                                                          <* 
 *>       REG__inpt_place  (x_col, a_row, p);                                                                                      <* 
 *>       /+---(parse next)---------------+/                                                                                       <* 
 *>       p  = strtok_r (NULL, s_q, &s);                                                                                           <* 
 *>       ++x_col;                                                                                                                 <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ ------ [ge.A42.132.77]+/ /+-[02.0000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG_inpt_driver      (char a_style)                                                                                            <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----+-----+-----+-----+-+/                                                                                    <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    char        rc          =    0;                                                                                             <* 
 *>    short       x_row       =    0;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(prepare)------------------------+/                                                                                    <* 
 *>    rc = REG__inpt_prep   (a_style);                                                                                            <* 
 *>    DEBUG_REGS   yLOG_value   ("prep rc"   , rc);                                                                               <* 
 *>    --rce;  if (rc < 0) {                                                                                                       <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    rc = REG__inpt_map ();                                                                                                      <* 
 *>    /+---(process lines)------------------+/                                                                                    <* 
 *>    while (1) {                                                                                                                 <* 
 *>       /+---(read)------------------------+/                                                                                    <* 
 *>       rc = REG__inpt_read ();                                                                                                  <* 
 *>       if (rc >  0)  continue;                                                                                                  <* 
 *>       if (rc <  0)  break;                                                                                                     <* 
 *>       /+---(process cells)---------------+/                                                                                    <* 
 *>       DEBUG_REGS  yLOG_value   ("x_row"     , x_row);                                                                          <* 
 *>       /+---(check sizer)-----------------+/                                                                                    <* 
 *>       if (strncmp (s_recd, "#<34"  , 4) == 0) {                                                                                <* 
 *>          REG__inpt_sizer ();                                                                                                   <* 
 *>          continue;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>       /+---(handle values)---------------+/                                                                                    <* 
 *>       if (strchr ("Vv", s_style) != NULL) {                                                                                    <* 
 *>          REG__inpt_values  (x_row);                                                                                            <* 
 *>       } else {                                                                                                                 <* 
 *>          REG__inpt_delims  (x_row);                                                                                            <* 
 *>       }                                                                                                                        <* 
 *>       /+---(prepare for next)------------+/                                                                                    <* 
 *>       ++x_row;                                                                                                                 <* 
 *>    }                                                                                                                           <* 
 *>    /+---(wrap up)------------------------+/                                                                                    <* 
 *>    rc = REG__inpt_done ();                                                                                                     <* 
 *>    DEBUG_REGS   yLOG_value   ("done rc"   , rc);                                                                               <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

char         /*-> tbd --------------------------------[ ------ [ge.RE5.1E2.D3]*/ /*-[05.0000.013.!]-*/ /*-[--.---.---.--]-*/
EXIM_export             (char a_style)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   FILE       *f           = NULL;
   tCELL      *curr        = NULL;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   int         x_rowsave   = 0;
   int         w           = 0;
   char        x_source    [LEN_RECD]  = "";
   char        x_modded    [LEN_RECD]  = "";
   char        x_print     [LEN_RECD]  = "";
   char        x_trim      [LEN_RECD]  = "";
   int         c           = 0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(open output file)---------------*/
   f = fopen("/root/z_gehye/vi_clip.txt", "w");
   DEBUG_REGS   yLOG_point   ("f (file)"  , f);
   --rce;  if (f == NULL) {
      DEBUG_REGS   yLOG_note    ("can not open clip file");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(process independent cells)------*/
   rc    = yVIKEYS_first (&x_col, &x_row, &x_tab);
   curr  = LOC_cell_at_loc (x_col, x_row, x_tab);
   x_rowsave = x_row;
   while (rc >= 0) {
      DEBUG_REGS   yLOG_point   ("curr"      , curr);
      ++c;
      /*---(look for line break)---------*/
      if (strchr ("vVmMcCtTrR", a_style) != 0 && x_row != x_rowsave) {
         DEBUG_REGS   yLOG_note    ("line break");
         fprintf (f, "\n");
      }
      /*---(fill in blank cells)---------*/
      if (curr == NULL) {
         DEBUG_REGS   yLOG_note    ("NULL cell");
         w = COL_width (x_tab, x_col);
         switch (a_style) {
         case 'v' : case 'r' : case 'R' :
            fprintf (f, "%*.*s", w, w, g_empty);
            break;
         case 'c' : case 'C' :
            fprintf (f, "\"\",");
            break;
         case 't' : case 'T' :
            fprintf (f, "\t");
            break;
         }
      }
      /*---(write filled cells)----------*/
      else {
         DEBUG_REGS   yLOG_info    ("cell"      , curr->label);
         /*---(source)-------------------*/
         if (curr->s != NULL) {
            DEBUG_REGS   yLOG_note    ("convert source string");
            strlcpy      (x_source, curr->s, LEN_RECD);
            strldecode   (x_source, LEN_RECD);
         } else {
            DEBUG_REGS   yLOG_note    ("source is NULL");
         }
         DEBUG_REGS   yLOG_info    ("source"    , x_source);
         /*---(modded string)------------*/
         if (curr->v_str != NULL) {
            DEBUG_REGS   yLOG_note    ("convert modified string");
            strlcpy      (x_modded, curr->v_str, LEN_RECD);
            strldecode   (x_modded, LEN_RECD);
         } else {
            DEBUG_REGS   yLOG_note    ("modified is NULL");
         }
         /*---(printable)----------------*/
         if (curr->p != NULL) {
            DEBUG_REGS   yLOG_note    ("convert printable");
            strlcpy  (x_print , curr->p, LEN_RECD);
            strldecode   (x_print, LEN_RECD);
            strlcpy  (x_trim  , x_print, LEN_RECD);
         } else {
            DEBUG_REGS   yLOG_note    ("printable is NULL");
         }
         /*---(trimmed printable)--------*/
         strltrim (x_trim, ySTR_BOTH, LEN_RECD);
         DEBUG_REGS   yLOG_info    ("printable" , x_trim);
         DEBUG_REGS   yLOG_info    ("modified"  , x_modded);
         DEBUG_REGS   yLOG_note    ("handle");
         switch (a_style) {
         case 'v' : fprintf (f, "%s"                  , x_print);
                    break;
         case 'r' : fprintf (f, "%s  "              , x_print);
                    break;
         case 'V' : fprintf (f, "%s "                 , x_trim);
                    break;
         case 'm' :
         case 'M' : fprintf (f, "%s"                  , x_modded);
                    break;
         case 'c' : fprintf (f, "\"%s\","             , x_trim);
                    break;
         case 'C' : fprintf (f, "\"%s\","             , x_source);
                    break;
         case 't' : fprintf (f, "%s\t"                , x_trim);
                    break;
         case 'T' : fprintf (f, "%s\t"                , x_source);
                    break;
         case 's' : fprintf (f, "%s\n"                , x_source);
                    break;
         case 'S' : fprintf (f, "(%s) %s\n"           , curr->label, x_source);
                    break;
         case 'f' : fprintf (f, "%-10.10s  %s\n"    , curr->label, x_source);
                    break;
         case 'F' : fprintf (f, "cell_reg    -D-                ");
                    fprintf (f, "%5d  %-8.8s  "   , c, curr->label);
                    fprintf (f, "%c %c %c %c %c  "  , curr->t, curr->f, curr->d, curr->a, curr->n);
                    fprintf (f, "%s\n"                , x_source);
                    break;
         }
      }
      x_rowsave = x_row;
      rc      = yVIKEYS_next  (&x_col, &x_row, &x_tab);
      curr  = LOC_cell_at_loc (x_col, x_row, x_tab);
   };
   /*---(close file)---------------------*/
   DEBUG_REGS   yLOG_note    ("closing file");
   fprintf (f, "\n");
   fclose  (f);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}




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
      /*> } else if (strcmp (a_question, "history"   )    == 0) {                                                                                   <* 
       *>    if      (s_nhist == 0    )  snprintf (unit_answer, LEN_UNIT, "s_file history   : n=%4d, c=%4d, n/a", s_nhist, s_chist);                      <* 
       *>    if      (s_chist <  0    )  snprintf (unit_answer, LEN_UNIT, "s_file history   : n=%4d, c=%4d, n/a", s_nhist, s_chist);                      <* 
       *>    else                      snprintf (unit_answer, LEN_UNIT, "s_file history   : n=%4d, c=%4d, %s" , s_nhist, s_chist, s_hist[s_chist].act);   <*/
   /*> } else if (strcmp (a_question, "entry"     )    == 0) {                                                                                                                                                   <* 
    *>    if      (a_ref <  0    )  snprintf (unit_answer, LEN_UNIT, "s_file entry     : %4d too small", a_ref);                                                                                                 <* 
    *>    else if (a_ref >= s_nhist)  snprintf (unit_answer, LEN_UNIT, "s_file entry     : %4d too large", a_ref);                                                                                                 <* 
    *>    else                      snprintf (unit_answer, LEN_UNIT, "s_file entry     : %4d, t=%4d, c=%4d, r=%4d, %s", a_ref, s_hist[a_ref].btab, s_hist[a_ref].bcol, s_hist[a_ref].brow, s_hist[a_ref].act);   <*/
   /*> } else if (strcmp (a_question, "before"    )    == 0) {                                                                      <* 
    *>    if      (a_ref <  0    )  snprintf (unit_answer, LEN_UNIT, "s_file before    : %4d too small", a_ref);                    <* 
    *>    else if (a_ref >= s_nhist)  snprintf (unit_answer, LEN_UNIT, "s_file before    : %4d too large", a_ref);                    <* 
    *>    else                      snprintf (unit_answer, LEN_UNIT, "s_file before    : %4d :%s:", a_ref, s_hist[a_ref].before);   <*/
   /*> } else if (strcmp (a_question, "after"     )    == 0) {                                                                     <* 
    *>    if      (a_ref <  0    )  snprintf (unit_answer, LEN_UNIT, "s_file after     : %4d too small", a_ref);                   <* 
    *>    else if (a_ref >= s_nhist)  snprintf (unit_answer, LEN_UNIT, "s_file after     : %4d too large", a_ref);                   <* 
    *>    else                      snprintf (unit_answer, LEN_UNIT, "s_file after     : %4d :%s:", a_ref, s_hist[a_ref].after);   <*/
}
/*---(complete)-----------------------*/
return unit_answer;
}


/*============================----end-of-source---============================*/
