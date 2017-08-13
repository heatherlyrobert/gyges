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
char        ver_txt     [100]       = "----------";

char        s_vers      [LEN_RECD]   = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
char        s_recd      [LEN_RECD];
int         s_len       = 0;
char       *s_p         = NULL;               /* strtok return pointer     */
char       *s_q         = "";               /* strtok delimeters         */
char       *s_context   = NULL;               /* strtok context variable   */
char        s_fields    [20][LEN_RECD];
int         s_nfield    =   0;


char        s_cellbad   = 0;



/*====================------------------------------------====================*/
/*===----                          versioning                          ----===*/
/*====================------------------------------------====================*/
PRIV void  o___VERSIONING______o () { return; }

char
FILE_bump          (char a_type, char *a_ver)
{
   /*---(locals)-----------+-----------+-*/
   char        x_temp      [LEN_RECD];
   char        rc          = 0;
   char        rce         = -10;
   /*---(defense: a_type)----------------*/
   --rce;  if (strchr ("SMmf", a_type) == NULL)  return rce;
   /*---(defense: a_ver)-----------------*/
   --rce;  if (a_ver == NULL)                    return rce;
   --rce;  if (FILE_version (a_ver, x_temp) < 0) return rce;
   /*---(factor)-------------------------*/
   if (strchr ("f", a_type) != NULL) {
      if (a_ver [4] <  'z') {
         ++a_ver[4];
         return 0;
      }
   }
   a_ver [4] = 'a';
   /*---(minor)--------------------------*/
   if (strchr ("mf", a_type) != NULL) {
      if (a_ver [3] <  '9') {
         ++a_ver[3];
         return 0;
      }
      if (a_ver [3] == '9') {
         a_ver  [3] =  'A';
         return 0;
      }
      if (a_ver [3] <  'F') {
         ++a_ver[3];
         return 0;
      }
   }
   a_ver [3] = '0';
   /*---(major)--------------------------*/
   if (strchr ("Mmf", a_type) != NULL) {
      if (a_ver [1] <  '9') {
         ++a_ver[1];
         return 0;
      }
      if (a_ver [1] == '9') {
         a_ver  [1] =  'A';
         return 0;
      }
      if (a_ver [1] <  'F') {
         ++a_ver[1];
         return 0;
      }
   }
   a_ver [1] = '0';
   /*---(super)--------------------------*/
   if (strchr ("SMmf", a_type) != NULL) {
      if (a_ver [0] <  '9') {
         ++a_ver[0];
         return 0;
      }
      if (a_ver [0] == '9') {
         a_ver  [0] =  'A';
         return 0;
      }
      if (a_ver [0] <  'F') {
         ++a_ver[0];
         return 0;
      }
   }
   /*---(complete)-----------------------*/
   strcpy (a_ver, "XX.Xx");
   return  -1;
}

char
FILE_version       (char *a_ver, char *a_final)
{
   /*---(locals)-----------+-----------+-*/
   int         x_len       = 0;
   char        rce         = -10;
   char        x_work      [10];
   /*---(defense : a_final)--------------*/
   --rce;  if (a_final == NULL)             return rce;
   /*---(set default)--------------------*/
   strcpy (a_final, "00.0a");
   /*---(defense : empty)----------------*/
   --rce;  if (a_ver == NULL)               return rce;
   x_len = strlen (a_ver);
   --rce;  if (strcmp (a_ver, "") == 0)     return rce;
   --rce;  if (x_len <= 0)                  return rce;
   /*---(defense: bad length)------------*/
   --rce;  if (x_len < 4)                   return rce;
   --rce;  if (x_len > 5)                   return rce;
   /*---(prepare)------------------------*/
   if (x_len == 4)   sprintf (x_work, "0%s", a_ver);
   else              strcpy  (x_work, a_ver);
   /*---(test chars)---------------------*/
   --rce;  if (strchr ("abcdefghijklmnopqrstuvwxyz", x_work [4]) == 0)  return rce;
   --rce;  if (strchr ("0123456789ABCDEF",           x_work [3]) == 0)  return rce;
   --rce;  if (x_work [2] != '.')                       return rce;
   --rce;  if (strchr ("0123456789ABCDEF",           x_work [1]) == 0)  return rce;
   --rce;  if (strchr ("0123456789ABCDEF",           x_work [0]) == 0)  return rce;
   /*---(finalize)-----------------------*/
   strcpy (a_final, x_work);
   /*---(complete)-----------------------*/
   return 0;
}




/*====================------------------------------------====================*/
/*===----                       history recording                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___HISTORY_________o () { return; }

char         /*--> clear out the history -----------------[--------[--------]-*/
HIST_clear         (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   /*---(clear list)------------------*/
   for (i = 0; i < MAX_HIST; ++i) {
      strcpy (hist [i].act   , "");
      hist [i].btab  = -1;
      hist [i].bcol  = -1;
      hist [i].brow  = -1;
      strcpy (hist [i].before, "");
      strcpy (hist [i].after , "");
   }
   /*---(clear vars)------------------*/
   nhist =  0;
   chist = -1;
   /*---(complete)--------------------*/
   return 0;
}

char         /*--> record a cell change ------------------[--------[--------]-*/
HIST_change        (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_type,     /* type of formatting                           */
      int         a_tab,      /* tab of change                                */
      int         a_col,      /* col of change                                */
      int         a_row,      /* row of change                                */
      char       *a_before,   /* contents before change (for undo)            */
      char       *a_after)    /* contents after change  (for redo)            */
{
   /*---(update pointers)-------------*/
   ++chist;
   nhist = chist + 1;
   /*---(add record)------------------*/
   strncpy (hist [chist].act   , a_type, 14);
   hist [chist].btab  = a_tab;
   hist [chist].bcol  = a_col;
   hist [chist].brow  = a_row;
   strcpy (hist [chist].before, a_before);
   strcpy (hist [chist].after , a_after );
   /*---(complete)--------------------*/
   return 0;
}

char         /*--> record a cell change ------------------[--------[--------]-*/
HIST_overwrite     (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_type,     /* type of formatting                           */
      int         a_tab,      /* tab of change                                */
      int         a_col,      /* col of change                                */
      int         a_row,      /* row of change                                */
      char       *a_before,   /* contents before change (for undo)            */
      char       *a_beforeF,  /* contents before change (for undo)            */
      char       *a_after,    /* contents after change  (for redo)            */
      char       *a_afterF)   /* contents after change  (for redo)            */
{
   /*---(update pointers)-------------*/
   ++chist;
   nhist = chist + 1;
   /*---(add record)------------------*/
   strncpy (hist [chist].act   , a_type, 14);
   hist [chist].btab  = a_tab;
   hist [chist].bcol  = a_col;
   hist [chist].brow  = a_row;
   sprintf (hist [chist].before, "%s::%s", a_beforeF, a_before);
   sprintf (hist [chist].after , "%s::%s", a_afterF , a_after );
   /*---(complete)--------------------*/
   return 0;
}

char         /*--> record a cell change ------------------[--------[--------]-*/
HIST_format        (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_type,     /* type of formatting                           */
      int         a_tab,      /* tab of change                                */
      int         a_col,      /* col of change                                */
      int         a_row,      /* row of change                                */
      char        a_before,   /* format before                                */
      char        a_after)    /* format after                                 */
{
   /*---(update pointers)-------------*/
   ++chist;
   nhist = chist + 1;
   /*---(add record)------------------*/
   strncpy (hist [chist].act   , a_type, 14);
   hist [chist].btab  = a_tab;
   hist [chist].bcol  = a_col;
   hist [chist].brow  = a_row;
   sprintf (hist [chist].before, "%c", a_before);
   sprintf (hist [chist].after , "%c", a_after );
   /*---(complete)--------------------*/
   return 0;
}

char         /*--> record a cell change ------------------[--------[--------]-*/
HIST_size          (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_type,     /* type of sizing                               */
      int         a_tab,      /* tab of change                                */
      int         a_col,      /* col of change                                */
      int         a_row,      /* row of change                                */
      int         a_before,   /* size before                                  */
      int         a_after)    /* size after                                   */
{
   /*---(update pointers)-------------*/
   ++chist;
   nhist = chist + 1;
   /*---(add record)------------------*/
   strncpy (hist [chist].act   , a_type, 14);
   hist [chist].btab  = a_tab;
   hist [chist].bcol  = a_col;
   hist [chist].brow  = a_row;
   sprintf (hist [chist].before, "%d", a_before);
   sprintf (hist [chist].after , "%d", a_after );
   /*---(complete)--------------------*/
   return 0;
}

char         /*--> take a change away --------------------[--------[--------]-*/
HIST_undo          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        x_lower     [15];                 /* action in lower case      */
   char        x_upper     [15];                 /* action in upper case      */
   int         i           = 0;                  /* generic iterator          */
   char        x_temp      [15];
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   DEBUG_HIST  yLOG_value   ("nhist"     , nhist);
   DEBUG_HIST  yLOG_value   ("chist"     , chist);
   /*---(defense)------------------------*/
   if (chist <  0    ) {
      DEBUG_HIST  yLOG_note    ("chist less than zero");
      DEBUG_HIST  yLOG_exit    (__FUNCTION__);
      return -1;
   }
   if (chist >= nhist) {
      DEBUG_HIST  yLOG_note    ("chist greater than or equal to nhist");
      DEBUG_HIST  yLOG_exit    (__FUNCTION__);
      return -2;
   }
   /*---(prepare)------------------------*/
   DEBUG_HIST  yLOG_info    ("action"    , hist[chist].act);
   strcpy (x_lower, hist[chist].act);
   for (i = 0; i < 15; ++i)   x_lower[i] = tolower (x_lower[i]);
   DEBUG_HIST  yLOG_info    ("lower"     , x_lower);
   strcpy (x_upper, hist[chist].act);
   for (i = 0; i < 15; ++i)   x_upper[i] = toupper (x_upper[i]);
   DEBUG_HIST  yLOG_info    ("upper"     , x_upper);
   /*---(get to right location)----------*/
   DEBUG_HIST  yLOG_note    ("clear any existing selection");
   VISU_clear ();
   DEBUG_HIST  yLOG_complex ("jump to"   , "t=%4d, c=%4d, r=%4d", hist[chist].btab, hist[chist].bcol, hist[chist].brow);
   LOC_jump  (hist[chist].btab, hist[chist].bcol, hist[chist].brow);
   /*---(handle request)-----------------*/
   DEBUG_HIST  yLOG_info    ("before"    , hist[chist].before);
   if        (strcmp ("change", x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_change");
      if (strcmp (hist[chist].before, "[<{(null)}>]") == 0) {
         CELL_delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
      } else {
         CELL_change  (NULL, CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].before);
      }
   } else if (strcmp ("overwrite", x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_overwrite");
      if (strcmp (hist[chist].before, "???::[<{(null)}>]") == 0) {
         CELL_delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
      } else {
         sprintf (x_temp, "%c%c%c", hist[chist].before[0], hist[chist].before[1], hist[chist].before[2]);
         CELL_overwrite (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].before + 5, x_temp);
      }
   } else if (strcmp ("delete", x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_delete");
      CELL_change   (NULL, CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].before);
   } else if (strcmp ("decimals", x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_decimals");
      CELL_decimals (CHG_NOHIST, hist[chist].before[0]);
   } else if (strcmp ("align"   , x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_align");
      CELL_align    (CHG_NOHIST, hist[chist].before[0]);
   } else if (strcmp ("format"  , x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_format");
      CELL_format   (CHG_NOHIST, hist[chist].before[0]);
   } else if (strcmp ("width"   , x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_width");
      CELL_width    (CHG_NOHIST, -(atoi (hist[chist].before)));
   }
   /*---(uptate)-------------------------*/
   --chist;
   DEBUG_HIST  yLOG_value   ("chist"     , chist);
   /*---(tail recursion)-----------------*/
   if (chist >= 0) {
      DEBUG_HIST  yLOG_info    ("action"    , hist[chist + 1].act);
      if (strcmp (hist[chist + 1].act, x_upper) == 0) {
         DEBUG_HIST  yLOG_note    ("check previous action");
         if      (strcmp (hist[chist].act, x_lower) == 0) {
            DEBUG_HIST  yLOG_note    ("final recursive call");
            HIST_undo ();
         }
         else if (strcmp (hist[chist].act, x_upper) == 0) {
            DEBUG_HIST  yLOG_note    ("another in series");
            HIST_undo ();
         }
         else {
            DEBUG_HIST  yLOG_note    ("does not match, don't recurse");
         }
      }
   }
   /*---(complet)------------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> put a change back ---------------------[--------[--------]-*/
HIST_redo          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        x_lower     [15];                 /* action in lower case      */
   char        x_upper     [15];                 /* action in upper case      */
   int         i           = 0;                  /* generic iterator          */
   char        x_temp      [15];
   /*---(defense)------------------------*/
   if (chist >= nhist - 1) return -1;
   /*---(uptate)-------------------------*/
   ++chist;
   /*---(prepare)------------------------*/
   strcpy (x_lower, hist[chist].act);
   for (i = 0; i < 15; ++i)   x_lower[i] = tolower (x_lower[i]);
   strcpy (x_upper, hist[chist].act);
   for (i = 0; i < 15; ++i)   x_upper[i] = toupper (x_upper[i]);
   /*---(get to right location)----------*/
   LOC_jump  (hist[chist].btab, hist[chist].bcol, hist[chist].brow);
   VISU_clear ();
   /*---(handle request)-----------------*/
   if        (strcmp ("change"  , x_lower) == 0) {
      if (strcmp (hist[chist].after , "[<{(null)}>]") == 0) {
         CELL_delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
      } else {
         CELL_change (NULL, CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].after);
      }
   } else if (strcmp ("overwrite", x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_overwrite");
      if (strcmp (hist[chist].after , "???::[<{(null)}>]") == 0) {
         CELL_delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
      } else {
         sprintf (x_temp, "%c%c%c", hist[chist].after [0], hist[chist].after [1], hist[chist].after [2]);
         CELL_overwrite (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].after  + 5, x_temp);
      }
   } else if (strcmp ("delete", x_lower) == 0) {
      CELL_delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
   } else if (strcmp ("decimals", x_lower) == 0) {
      CELL_decimals (CHG_NOHIST, hist[chist].after[0]);
   } else if (strcmp ("align"   , x_lower) == 0) {
      CELL_align    (CHG_NOHIST, hist[chist].after[0]);
   } else if (strcmp ("format"  , x_lower) == 0) {
      CELL_format   (CHG_NOHIST, hist[chist].after[0]);
   } else if (strcmp ("width"   , x_lower) == 0) {
      CELL_width    (CHG_NOHIST, -(atoi (hist[chist].after )));
   }
   /*---(tail recursion)-----------------*/
   if (chist <  nhist - 1) {
      if (strcmp (hist[chist + 1].act, x_upper) == 0)  HIST_redo ();
   }
   /*---(complet)------------------------*/
   return 0;
}

char         /*--> list history --------------------------[--------[--------]-*/
HIST_list          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_name      [100]       = "";
   char        rce         = -10;
   FILE       *x_file      = NULL;
   /*---(open)---------------------------*/
   snprintf (x_name, 95, "%s.hist", my.f_title);
   x_file = fopen(x_name, "w");
   --rce;
   if (x_file == NULL)      return rce;
   /*---(print)--------------------------*/
   fprintf (x_file, "details of history\n\n");
   fprintf (x_file, "nhist = %d\n" , nhist);
   fprintf (x_file, "chist = %d\n" , chist);
   for (i = 0; i < nhist; ++i) {
      if (i % 5 == 0)  fprintf (x_file, "\n-ref- %c action--------- %c btab %c bcol %c brow %c before/after ----------------------------------------\n", 31, 31, 31, 31, 31);
      fprintf (x_file, "%-5d %c %-15.15s %c %4d %c %4d %c %4d %c %s %c %s %c\n", i,
            31, hist [i].act   , 31, hist [i].btab  ,
            31, hist [i].bcol  , 31, hist [i].brow  ,
            31, hist [i].before, 31, hist [i].after , 31);
   }
   fprintf (x_file, "\ndone\n");
   /*---(close)--------------------------*/
   fclose (x_file);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    reading and writing tabs                  ----===*/
/*====================------------------------------------====================*/
static void   o___TABS____________o (void) { return; }

/* tab records are allowed to evolve separately from other input types as the
 * need arises.  as such, they have their own versioning and format.
 *
 * C) tab         tab#  ncol  nrow  ccol  crow  name-------------------
 *    tab            0    26   100     2     6  tab00
 *
 * D) tab         tab#  ncol  nrow  bcol  brow  ccol  crow  name-------------------
 *    tab            0    26   100     2     3     5    13  tab00
 *
 * E) tab         tab#  ncol  nrow  bcol  brow  ccol  crow  t -lbeg- -lend-  name-------------------
 *    tab            0    26   100     0     1     5    23  - ------ ------  tab00
 *
 * F) tab         ver  tab#  --max--  --beg--  --cur--  t  lockbeg  lockend  name-------------------
 *    tab         -F-     0  0az999   0a1      0g5      b  0a5      0b5      one
 *
 * G) tab         ver  --max--  name-------------------
 *    tab         -G-  0az999   one
 *
 *
 */

char         /*--> process a tab size record -------------[ leaf   [ ------ ]-*/
INPT_tab           (char *a_label, char *a_name)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         rc          = 0;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   DEBUG_INPT  yLOG_point   ("a_label"   , a_label);
   DEBUG_INPT  yLOG_point   ("a_name"    , a_name);
   /*---(parse address)------------*/
   rc = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);
   DEBUG_INPT  yLOG_value   ("parse"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT  yLOG_info    ("a_label"   , a_label);
   /*---(update size)--------------*/
   rc = LOC_tab_resize     (a_label);
   DEBUG_INPT  yLOG_value   ("resize"    , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT  yLOG_value   ("col_max"   , LOC_col_max (x_tab));
   DEBUG_INPT  yLOG_value   ("row_max"   , LOC_row_max (x_tab));
   /*---(change name)--------------*/
   rc = LOC_tab_rename     (x_tab, a_name);
   DEBUG_INPT  yLOG_value   ("rename"    , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT  yLOG_info    ("a_name"    , a_name);
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> write file tab information ------------[ leaf   [ ------ ]-*/
OUTP_tab             (short a_tab)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;          /* generic return code            */
   char        rce         =  -10;          /* return code for errors         */
   char        x_write     =  '-';
   short       x_cols      =    0;
   short       x_rows      =    0;
   char        x_addr      [25]        = "";
   char        x_name      [25]        = "";
   char        x_default   [25]        = "";
   /*---(prepare)------------------------*/
   sprintf (my.f_recd, "");
   /*---(defense)------------------------*/
   rc = LOC_tab_valid (a_tab);
   --rce;  if (rc      <  0)                     return rce;
   /*---(gather size)--------------------*/
   x_cols = LOC_col_max (a_tab);
   /*> printf ("x_cols = %4d, default = %4d\n", x_cols, LOC_col_defmax());            <*/
   if (x_cols != LOC_col_defmax())       x_write = 'y';
   x_rows = LOC_row_max (a_tab);
   /*> printf ("x_rows = %4d, default = %4d\n", x_rows, LOC_row_defmax());            <*/
   if (x_rows != LOC_row_defmax())       x_write = 'y';
   LOC_ref (a_tab, x_cols - 1, x_rows - 1, 0, x_addr);
   /*---(gather name)--------------------*/
   LOC_tab_name    (a_tab, x_name);
   LOC_tab_defname (a_tab, x_default);
   /*> printf ("name   = %-10.10s, default = %-10.10s\n", x_name, x_default);         <*/
   if (strcmp (x_default, x_name) != 0)  x_write = 'y';
   /*---(build record)-------------------*/
   if (x_write == 'y') {
      sprintf (my.f_recd, "tab         -G-  %-8s  %-12.12s ", x_addr, x_name);
      return 1;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> write file tab information ------------[ leaf   [ ------ ]-*/
OUTP_tab_head        (FILE *a_file)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   /*---(header)-------------------------*/
   fprintf (a_file, "#===[[ TAB LAYOUT ]]=================================================================================================#\n");
   fprintf (a_file, "#---------  ver  --max---  ---name----- \n");
   fflush  (a_file);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> write file tab information ------------[ leaf   [ ------ ]-*/
OUTP_tab_foot        (FILE *a_file, int a_count)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   /*---(header)-------------------------*/
   if (a_count == 0)  fprintf (a_file, "# no special or unique tab information\n");
   else               fprintf (a_file, "#---------  ver  --max---  ---name----- \n");
   fprintf (a_file, "\n\n\n");
   fflush  (a_file);
   /*---(complete)-----------------------*/
   return 0;
}

int          /*--> write file tab information ------------[ leaf   [ ------ ]-*/
OUTP_tabs            (FILE *a_file)
{
   int i = 0;
   int c = 0;
   char rc = 0;
   rc = OUTP_tab_head  (a_file);
   for (i = 0; i <= MAX_TABS; ++i) {
      rc = OUTP_tab    (i);
      if (rc <= 0)   continue;
      if (a_file != NULL)  fprintf (a_file, "%s\n", my.f_recd);
      ++c;
   }
   rc = OUTP_tab_foot  (a_file, c);
   return c;
}



/*====================------------------------------------====================*/
/*===----                       column width                           ----===*/
/*====================------------------------------------====================*/
static void   o___COLUMNS_________o (void) { return; }

char         /*--> process a column width record ---------[ leaf   [ ------ ]-*/
INPT_col           (char *a_label, int a_size)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         rc          = 0;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   DEBUG_INPT  yLOG_point   ("a_label"   , a_label);
   DEBUG_INPT  yLOG_value   ("a_size"    , a_size);
   /*---(parse address)------------*/
   rc = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);
   DEBUG_INPT  yLOG_value   ("parse"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT  yLOG_info    ("a_label"   , a_label);
   DEBUG_INPT  yLOG_value   ("tab"       , x_tab);
   DEBUG_INPT  yLOG_value   ("col"       , x_col);
   /*---(update size)--------------*/
   rc = LOC_col_widen      (x_tab, x_col, a_size);
   DEBUG_INPT  yLOG_value   ("widen"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> write file col information ------------[ leaf   [ ------ ]-*/
OUTP_col             (short a_tab, short a_col)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;          /* generic return code            */
   char        rce         =  -10;          /* return code for errors         */
   char        x_write     =  '-';
   short       x_cols      =    0;
   short       x_size      =    0;
   char        x_addr      [25]        = "";
   /*---(prepare)------------------------*/
   sprintf (my.f_recd, "");
   /*---(defense)------------------------*/
   rc = LOC_col_valid (a_tab, a_col);
   --rce;  if (rc      <  0)                     return rce;
   /*---(gather size)--------------------*/
   x_cols = LOC_col_max   (a_tab);
   --rce;  if (a_col > x_cols)                   return 0;
   x_size = LOC_col_width (a_tab, a_col);
   --rce;  if (x_size == DEF_WIDTH)              return 0;
   /*---(create a label)-----------------*/
   LOC_ref (a_tab, a_col, 0, 0, x_addr);
   /*---(build record)-------------------*/
   sprintf (my.f_recd, "width       -A-  %-8s  %4d ", x_addr, x_size);
   /*---(complete)-----------------------*/
   return 1;
}

char         /*--> write file col header -----------------[ leaf   [ ------ ]-*/
OUTP_col_head        (FILE *a_file)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   /*---(header)-------------------------*/
   fprintf (a_file, "#===[[ COLUMN WIDTHS ]]==============================================================================================#\n");
   fprintf (a_file, "#---------  ver  ---loc--  size \n");
   fflush  (a_file);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> write file col footer -----------------[ leaf   [ ------ ]-*/
OUTP_col_foot        (FILE *a_file, int a_count)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   /*---(header)-------------------------*/
   if (a_count == 0)  fprintf (a_file, "# no special or unique col information\n");
   fprintf (a_file, "#---------  ver  ---loc--  size \n");
   fprintf (a_file, "\n\n\n");
   fflush  (a_file);
   /*---(complete)-----------------------*/
   return 0;
}

int          /*--> write file col section ----------------[ leaf   [ ------ ]-*/
OUTP_cols            (FILE *a_file)
{
   int i = 0;
   int j = 0;
   int c = 0;
   char rc = 0;
   rc = OUTP_col_head  (a_file);
   for (i = 0; i <= MAX_TABS; ++i) {
      for (j = 0; j <= MAX_COLS; ++j) {
         rc = OUTP_col    (i, j);
         if (rc <= 0)   continue;
         if (a_file != NULL)  fprintf (a_file, "%s\n", my.f_recd);
         ++c;
      }
   }
   rc = OUTP_col_foot  (a_file, c);
   return c;
}




/*====================------------------------------------====================*/
/*===----                    reading and writing cells                 ----===*/
/*====================------------------------------------====================*/
PRIV void  o___CELLS___________o () { return; }


/* cell records are allowed to evolve separately from other input types as the
 * need arises.  as such, they have their own versioning and format.
 *
 * D) cell        tab#  ver  ---lvl/reg---  -seq-  --loc--  t-f-d-a-m  ---source--------------
 *    cell           0  -D-  ---3              21  0e4      f ? 0 < -  =now()
 *
 */

char
INPT_cell          (char *a_label, char *a_format, char *a_source)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   char        x_string    [LEN_LABEL] = "?0?";
   tCELL      *x_new       = NULL;
   char        x_format    =  '?';
   char        x_decs      =  '0';
   char        x_align     =  '?';
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(defence)------------------*/
   DEBUG_INPT  yLOG_point   ("a_source"  , a_source);
   --rce;  if (a_source == NULL)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(parse address)------------*/
   DEBUG_INPT  yLOG_point   ("a_label"   , a_label);
   --rce;  if (a_label  == NULL)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);
   DEBUG_INPT  yLOG_value   ("parse"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT  yLOG_info    ("a_label"   , a_label);
   DEBUG_INPT  yLOG_value   ("x_tab"     , x_tab);
   DEBUG_INPT  yLOG_value   ("x_col"     , x_col);
   DEBUG_INPT  yLOG_value   ("x_row"     , x_row);
   /*---(expand everything as needed)----*/
   rc = LOC_legal (x_tab, x_col, x_row, CELL_GROW);
   DEBUG_INPT  yLOG_value   ("legal"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(fix format)---------------------*/
   if (a_format == NULL) {
      strcpy  (x_string, "??0");
   } else if (strlen (a_format) == 9) {
      x_format  = CELL_format_valid   (a_format [2]);
      DEBUG_INPT  yLOG_char    ("x_format"  , x_format);
      x_decs    = CELL_decimals_valid (a_format [4]);
      DEBUG_INPT  yLOG_char    ("x_decs"    , x_decs);
      x_align   = CELL_align_valid    (a_format [6]);
      DEBUG_INPT  yLOG_char    ("x_align"   , x_align);
      sprintf (x_string, "%c%c%c", x_format, x_align, x_decs);
   } else {
      strcpy  (x_string, "??0");
   }
   DEBUG_INPT  yLOG_info    ("x_string"  , x_string);
   /*---(update)-------------------------*/
   DEBUG_INPT  yLOG_info    ("a_source"  , a_source);
   x_new = CELL_overwrite (CHG_NOHIST, x_tab, x_col, x_row, a_source, x_string);
   DEBUG_INPT  yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> write a single cell to a file ---------[ leaf   [ ------ ]-*/
OUTP_cell          (FILE *a_file, char *a_type, int a_seq, char *a_level, tCELL *a_curr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (a_type  == NULL)   return rce;
   --rce;  if (a_level == NULL)   return rce;
   --rce;  if (a_curr  == NULL)   return rce;
   /*---(header)-------------------------*/
   if ((a_seq % 5) == 0 && a_file != NULL) {
      fprintf (a_file, "#---------  ver  ---lvl/reg--  -seq-  ---loc-- ");
      fprintf (a_file, " t-f-d-a-m  ---source--------------------------------------\n");
   }
   /*---(build record)-------------------*/
   sprintf (my.f_recd, "%-10.10s  -D-  %-12.12s  %5d  %-8.8s  %c %c %c %c %c  %s",
         a_type, a_level, a_seq,
         a_curr->label,
         a_curr->t, a_curr->f, a_curr->d, a_curr->a, a_curr->n,
         a_curr->s);
   /*---(write entry)--------------------*/
   if (a_file != NULL)  fprintf (a_file, "%s\n", my.f_recd);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> write file dependent cells ------------[ leaf   [ ------ ]-*/
OUTP_cell_dep      (FILE *a_file, int a_seq, int a_level, tCELL *a_curr)
{
   /*---(locals)-----------+-----------+-*/
   char        x_pre       [50]        = "-           ";
   /*---(defense)------------------------*/
   if (a_curr    == NULL)        return;     /* no cell                       */
   if (a_curr->s == NULL)        return;     /* nothing to write              */
   if (a_curr->t == '-')         return;     /* don't write, recreate on read */
   /*---(prepare)------------------------*/
   if (a_level <  10)   sprintf (x_pre, "%-*.*s%d%-15.15s", a_level, a_level, "------------", a_level, " ");
   else                 sprintf (x_pre, "            +");
   /*---(print)--------------------------*/
   OUTP_cell (a_file, "cell_dep", a_seq, x_pre, a_curr);
   /*---(complete)-----------------------*/
   fflush (a_file);
   return 0;
}

char         /*--> write file independent cells ----------[ left   [ ------ ]-*/
OUTP_cell_free     (FILE *a_file, int *a_seq, long a_stamp, int a_tab, int a_bcol, int a_ecol, int a_brow, int a_erow)
{
   /*---(locals)-----------+-----------+-*/
   int         x           = 0;             /* iterator -- columns            */
   int         y           = 0;             /* iterator -- row                */
   char        rc          = 0;             /* generic return code            */
   char        rce         = -10;           /* return code for errors         */
   char        x_label     [20];            /* holder for cell address        */
   tCELL      *x_curr      = NULL;
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                        return rce;
   --rce;  if (*a_seq <  0)                           return rce;
   --rce;  if (a_tab  <  0)                           return rce;
   --rce;  if (a_tab  >= my.ntab)                     return rce;
   --rce;  if (a_bcol <  0)                           return rce;
   --rce;  if (a_bcol >= LOC_col_max (a_tab))         return rce;
   --rce;  if (a_ecol <  a_bcol)                      return rce;
   --rce;  if (a_ecol <  0)                           return rce;
   --rce;  if (a_ecol >= LOC_col_max (a_tab))         return rce;
   --rce;  if (a_brow <  0)                           return rce;
   --rce;  if (a_brow >= LOC_row_max (a_tab))         return rce;
   --rce;  if (a_erow <  a_brow)                      return rce;
   --rce;  if (a_erow <  0)                           return rce;
   --rce;  if (a_erow >= LOC_row_max (a_tab))         return rce;
   /*---(cells)--------------------------*/
   for (x = a_bcol; x <= a_ecol; ++x) {
      for (y = a_brow; y <= a_erow; ++y) {
         x_curr = LOC_cell_at_loc (a_tab, x, y);
         if (x_curr    == NULL)                       continue;
         if (x_curr->s == NULL)                       continue;
         if (x_curr->t == CTYPE_BLANK)                continue;
         if (x_curr->u == a_stamp)                    continue;
         OUTP_cell (a_file, "cell_free", *a_seq, "", x_curr);
         ++(*a_seq);
      }
   }
   /*---(complete)-----------------------*/
   fflush (a_file);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          reading files                       ----===*/
/*====================------------------------------------====================*/
static void   o___READ____________o (void) { return; }

char         /*--> open file for reading and prep --------[ leaf   [ ------ ]-*/
INPT_open          (cchar *a_name)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char       *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   DEBUG_INPT  yLOG_point   ("filename"  , a_name);
   /*---(defense)------------------------*/
   --rce;  if (a_name == NULL) {
      DEBUG_INPT  yLOG_note    ("file name can not be null");
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(parse name)---------------------*/
   strcpy (my.f_name, a_name);
   p = strchr (my.f_name, '.');
   if (p != NULL)  p[0] = '\0';
   --rce;  if (strcmp (my.f_name, FILE_BLANK) == 0) {
      DEBUG_INPT  yLOG_note    ("file name is default");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(open file)----------------------*/
   DEBUG_INPT  yLOG_info    ("filename"  , a_name);
   my.f_file = fopen (a_name, "r");
   DEBUG_INPT  yLOG_point   ("f_file"    , my.f_file);
   --rce;  if (my.f_file == NULL) {
      DEBUG_INPT  yLOG_note    ("file could not be openned");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_INPT  yLOG_note    ("file successfully opened");
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> open file for reading and prep --------[ leaf   [ ------ ]-*/
INPT_prep          (void)
{
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(initialize)---------------------*/
   strncpy (f_maker, "unknown", LEN_RECD);
   my.f_lines = 0;
   DEBUG_INPT  yLOG_note    ("setting default size and locations");
   NCOL = DEF_COLS;
   BCOL = ECOL = 0;
   NROW = DEF_ROWS;
   BROW = EROW = 0;
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> open file for reading and prep --------[ leaf   [ ------ ]-*/
INPT_done          (void)
{
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(screen)-------------------------*/
   DEBUG_INPT   yLOG_note    ("set screen positions correctly");
   KEYS_basics (' ', 'r');
   KEYS_bcol (BCOL);
   CURS_col_head();
   KEYS_brow (BROW);
   CURS_row_head();
   /*---(calculate)----------------------*/
   DEBUG_INPT  yLOG_note    ("recalc");
   SEQ_calc_full ();
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> close file for reading and wrap -------[ flower [ ------ ]-*/
INPT_close         (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(close file)---------------------*/
   if (my.f_file == NULL) {
      DEBUG_INPT  yLOG_note    ("no file to close");
   } else {
      DEBUG_INPT  yLOG_note    ("close file");
      fclose  (my.f_file);
   }
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /* file reading driver ----------------------[--------[--------]-*/
INPT_read          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   int         x_len       =    0;               /* string length             */
   /*---(read and clean)--------------*/
   ++my.f_lines;
   DEBUG_INPT  yLOG_value   ("line"      , my.f_lines);
   fgets (my.f_recd, LEN_RECD, my.f_file);
   --rce;  if (feof (my.f_file))  {
      DEBUG_INPT  yLOG_note    ("end of file reached");
      return rce;
   }
   x_len = strlen (my.f_recd);
   --rce;  if (x_len <= 0)  {
      DEBUG_INPT  yLOG_note    ("record empty");
      return -rce;
   }
   my.f_recd [--x_len] = '\0';
   DEBUG_INPT  yLOG_value   ("length"    , x_len);
   DEBUG_INPT  yLOG_info    ("fixed"     , my.f_recd);
   --rce;  if (my.f_recd [0] == '#') {
      DEBUG_INPT  yLOG_note    ("comment line, skipping");
      return -rce;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /* file reading driver ----------------------[--------[--------]-*/
INPT_parse         (cchar *a_recd)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        x_recd      [LEN_RECD];
   int         x_len       =    0;
   char       *p           = NULL;               /* strtok pointer            */
   char       *q           = "";               /* strtok delimeters         */
   char       *r           = NULL;               /* strtok context variable   */
   /*---(cleanse)---------------------*/
   for (s_nfield = 0; s_nfield < 20; ++s_nfield) {
      strlcpy (s_fields [s_nfield], "", LEN_RECD);
   }
   s_nfield = 0;
   strlcpy (x_recd, a_recd, LEN_RECD);
   /*---(parse fields)----------------*/
   p = strtok_r (x_recd, q, &r);
   while (p != NULL) {
      strltrim  (p, ySTR_BOTH, LEN_RECD);
      DEBUG_INPT  yLOG_bullet  (s_nfield    , p);
      strncpy   (s_fields [s_nfield++], p, LEN_RECD);
      p = strtok_r (NULL     , q, &r);
   }
   --rce;  if (s_nfield < 1) {
      DEBUG_INPT  yLOG_note    ("no fields found");
      return -rce;
   }
   strncpy   (my.f_type, s_fields [0], LEN_RECD);
   DEBUG_INPT  yLOG_info    ("type"      , my.f_type);
   if (s_fields [1][0] == '-' && s_fields [1][2] == '-')
      my.f_vers  = s_fields [1][1];
   else
      my.f_vers  = '?';
   DEBUG_INPT  yLOG_char    ("vers"      , my.f_vers);
   /*---(complete)-----------------------*/
   return 0;
}

char         /* file reading driver ----------------------[--------[--------]-*/
INPT_main          (cchar *a_name)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         rc          = 0;
   char        x_temp      [LEN_RECD];            /* strtok version of input   */
   int         x_len       = 0;                  /* string length             */
   char       *p;
   int         x_celltry   = 0;
   int         x_cellbad   = 0;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(open file)----------------------*/
   rc = INPT_open   (a_name);
   --rce;  if (rc < 0) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   rc = INPT_prep   ();
   /*---(read lines)---------------------*/
   DEBUG_INPT  yLOG_note    ("read lines");
   while (my.f_file != NULL) {
      /*---(read and clean)--------------*/
      rc = INPT_read ();
      if (rc < 0)  break;
      if (rc > 0)  continue;
      rc = INPT_parse (my.f_recd);
      if (rc < 0)  break;
      if (rc > 0)  continue;
      /*---(handle types)----------------*/
      switch (my.f_type [0]) {
      case 'w' :
         if  (my.f_vers == 'A')  rc = INPT_col   (s_fields [2], atoi (s_fields [3]));
         if  (my.f_vers == '?')  rc = INPT_col   (s_fields [1], atoi (s_fields [2]));
         break;
      case 't' :
         if  (my.f_vers == 'F')  rc = INPT_tab   (s_fields [3], s_fields [9]);
         if  (my.f_vers == 'G')  rc = INPT_tab   (s_fields [2], s_fields [3]);
         break;
      case 'c' :
         if  (my.f_vers == 'D')  rc = INPT_cell  (s_fields [4], s_fields [5], s_fields [6]);
         if (rc < 0)  ++x_cellbad;
         break;
      }
   }
   /*---(close file)---------------------*/
   INPT_close ();
   INPT_done  ();
   /*---(complete)-------------------------*/
   DEBUG_INPT yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          writing files                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___WRITE___________o () { return; }

char         /*--> write file header ---------------------[ leaf   [ ------ ]-*/
OUTP_header        (FILE *a_file)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;             /* iterator -- tab                */
   char        rc          = 0;             /* generic return code            */
   char        rce         = -10;           /* return code for errors         */
   char        x_temp      [100];
   time_t      x_time;
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   /*---(introduction)---------------------*/
   fprintf (a_file, "#!/usr/local/bin/gyges\n");
   fprintf (a_file, "#   generated by the heatherly spreadsheet tool gyges-hekatonkheires (hundred-handed)\n");
   /*---(write header)---------------------*/
   fprintf (a_file, "\n\n\n");
   fprintf (a_file, "#===[[ GENERAL ]]====================================================================================================#\n");
   fprintf (a_file, "#--------- %c -ver- %c ---description ------------------------------------------------\n", 31, 31);
   /*---(format identifiers)---------------*/
   fprintf (a_file, "gyges      %c %5s %c %-60.60s %c\n", 31, VER_NUM, 31, VER_TXT                      , 31);
   /*---(timestamp)------------------------*/
   x_time = time(NULL);
   strftime (x_temp, 100, "%Y.%m.%d.%H.%M.%S", localtime(&x_time));
   fprintf (a_file, "timestamp  %c %5s %c %-60.60s %c\n", 31, "-----", 31, x_temp                       , 31);
   /*---(version)------------------------*/
   if (ver_ctrl == 'y') {
      fprintf (a_file, "versioned  %c %5s %c %-60.60s %c\n",
            31, ver_num, 31, ver_txt, 31);
   }
   /*---(finish)-------------------------*/
   fprintf (a_file, "#--------- %c -ver- %c ---description ------------------------------------------------\n", 31, 31);
   fprintf (a_file, "\n\n\n");
   fflush  (a_file);
   /*---(complete)-----------------------*/
   return 0;
}

char
FILE_write         (char *a_name)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   FILE       *f           = NULL;
   char       *p;
   int         x_seq;
   int         i           = 0;       /* iterator -- columns                            */
   long        x_stamp     = 0;
   char        x_temp      [100];
   char       *x_bufs      = "abcdefghijklmnopqrstuvwxyz";
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_OUTP yLOG_enter   (__FUNCTION__);
   /*---(defense: name)------------------*/
   if (a_name == NULL) return -1;
   /*---(prepare versioning)-------------*/
   strcpy (my.f_name, a_name);
   p = strchr (my.f_name, '.');
   if (p != NULL)  p[0] = '\0';
   strcpy (my.f_suffix, "gyges");
   /*---(open file)----------------------*/
   f = fopen(a_name, "w");
   if (f == NULL)      return -2;
   /*---(header)-------------------------*/
   OUTP_header (f);
   /*---(tab data)-----------------------*/
   OUTP_tabs   (f);
   /*---(column data)--------------------*/
   OUTP_cols   (f);
   /*---(dependent cells)------------------*/
   fprintf (f, "#===[[ DEPENDENCY TREE CELLS, in reverse order ]]====================================================================#\n");
   x_stamp   = rand ();
   rc = SEQ_file_deps (x_stamp, f);
   fprintf (f, "# dependent cells complete\n\n\n\n");
   /*---(non-dependency cells)-------------*/
   fprintf (f, "#===[[ INDENPENDENT CELLS, tab then col then row order]]=============================================================#\n");
   x_seq     = 0;
   for (i = 0; i < NTAB; ++i) {
      rc = OUTP_cell_free (f, &x_seq, x_stamp, i, 0, LOC_col_max (i) - 1, 0, LOC_row_max (i) - 1);
   }
   fprintf (f, "# independent cells complete\n\n\n\n");
   /*---(footer data)----------------------*/
   fprintf (f, "# done, finito, complete\n");
   /*---(close file)-----------------------*/
   fclose  (f);
   /*---(make version)---------------------*/
   if (ver_ctrl == 'y') {
      sprintf (x_temp, "cp -f %s %s.v%c%c%s.gyges", a_name, my.f_name, ver_num[0], ver_num[1], ver_num + 3);
      system (x_temp);
   }
   /*---(complete)-------------------------*/
   DEBUG_INPT yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNIT_TEST_______o () { return; }

char*        /*> unit test accessor ----------------------[ ------ [ ------ ]-*/
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
   } else if (strcmp (a_question, "recd"      )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file recd      : %s", my.f_recd);
   } else if (strcmp (a_question, "freeze"    )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file freeze    : col=%c (%4d to %4d)   row=%c (%4d to %4d)",
            FR_COL, FR_BCOL, FR_ECOL, FR_ROW, FR_BROW, FR_EROW);
   } else if (strcmp (a_question, "tab_name"  )    == 0) {
      LOC_tab_name (a_ref, x_name);
      snprintf (unit_answer, LEN_UNIT, "s_file tab name  : tab=%4d, act=%c, :%s:", a_ref, 'y', x_name);
   } else if (strcmp (a_question, "tab_count" )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file tab count : ntab=%4d", NTAB);
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
