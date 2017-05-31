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

char         /*--> clear a tab entry ---------------------[ petal -[--------]-*/
TAB_init           (int a_tab)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   int         j           = 0;
   char        t           [100];
   char        x_new       = 'y';
   /*---(check for new tab)-----------*/
   if (tabs[a_tab].active != 'y')  x_new = '-';
   /*---(main config)-----------------*/
   tabs[a_tab].active  = '-';
   sprintf (t, "tab%02d", a_tab);
   strcpy  (tabs[a_tab].name, t);
   tabs[a_tab].c       =    0;
   /*---(size limits)-----------------*/
   tabs[a_tab].ncol    = DEF_COLS;
   tabs[a_tab].nrow    = DEF_ROWS;
   /*---(current position)------------*/
   tabs[a_tab].ccol    =    0;
   tabs[a_tab].crow    =    0;
   /*---(screen position)-------------*/
   tabs[a_tab].bcol    =    0;
   tabs[a_tab].brow    =    0;
   tabs[a_tab].ecol    =    0;
   tabs[a_tab].erow    =    0;
   /*---(initialize columns)----------*/
   for (i = 0; i < MAX_COLS; ++i) {
      tabs[a_tab].cols[i].w        = DEF_WIDTH;
      tabs[a_tab].cols[i].x        = 0;
      tabs[a_tab].cols[i].c        = 0;
      if        (i < 26)  {
         tabs[a_tab].cols[i].l[0] = '-';
         tabs[a_tab].cols[i].l[1] = i + 'a';
      } else  {
         tabs[a_tab].cols[i].l[0] = (i / 26) - 1 + 'a';
         tabs[a_tab].cols[i].l[1] = (i % 26) + 'a';
      }
      tabs[a_tab].cols[i].l[2] = '\0';
   }
   /*---(initialize rows)-------------*/
   for (i = 0; i < MAX_ROWS; ++i) {
      tabs[a_tab].rows[i].h = DEF_HEIGHT;
      tabs[a_tab].rows[i].y = 0;
      tabs[a_tab].rows[i].c = 0;
   }
   /*---(clean cells)-----------------*/
   for (i = 0; i < MAX_COLS; ++i) {
      for (j = 0; j < MAX_ROWS; ++j) {
         tabs[a_tab].sheet[i][j] = NULL;
      }
   }
   /*---(locked row/col)--------------*/
   tabs[a_tab].froz_col  = '-';
   tabs[a_tab].froz_bcol = 0;
   tabs[a_tab].froz_ecol = 0;
   tabs[a_tab].froz_row  = '-';
   tabs[a_tab].froz_brow = 0;
   tabs[a_tab].froz_erow = 0;
   /*---(deactivate tab)--------------*/
   tabs[a_tab].active  = '-';
   /*---(done)------------------------*/
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
         CELL__delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
      } else {
         CELL_change  (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].before);
      }
   } else if (strcmp ("overwrite", x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_overwrite");
      if (strcmp (hist[chist].before, "???::[<{(null)}>]") == 0) {
         CELL__delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
      } else {
         sprintf (x_temp, "%c%c%c", hist[chist].before[0], hist[chist].before[1], hist[chist].before[2]);
         CELL_overwrite (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].before + 5, x_temp);
      }
   } else if (strcmp ("delete", x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL__delete");
      CELL_change   (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].before);
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
   } else if (strcmp ("height"  , x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_height");
      CELL_height   (CHG_NOHIST, -(atoi (hist[chist].before)));
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
         CELL__delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
      } else {
         CELL_change (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].after);
      }
   } else if (strcmp ("overwrite", x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_overwrite");
      if (strcmp (hist[chist].after , "???::[<{(null)}>]") == 0) {
         CELL__delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
      } else {
         sprintf (x_temp, "%c%c%c", hist[chist].after [0], hist[chist].after [1], hist[chist].after [2]);
         CELL_overwrite (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].after  + 5, x_temp);
      }
   } else if (strcmp ("delete", x_lower) == 0) {
      CELL__delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
   } else if (strcmp ("decimals", x_lower) == 0) {
      CELL_decimals (CHG_NOHIST, hist[chist].after[0]);
   } else if (strcmp ("align"   , x_lower) == 0) {
      CELL_align    (CHG_NOHIST, hist[chist].after[0]);
   } else if (strcmp ("format"  , x_lower) == 0) {
      CELL_format   (CHG_NOHIST, hist[chist].after[0]);
   } else if (strcmp ("width"   , x_lower) == 0) {
      CELL_width    (CHG_NOHIST, -(atoi (hist[chist].after )));
   } else if (strcmp ("height"  , x_lower) == 0) {
      CELL_height   (CHG_NOHIST, -(atoi (hist[chist].after )));
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
 *
 *
 *
 */

char
INPT_rowcol        (
      char  *a_cell,
      short *a_col   , short *a_row   , int  a_min,
      int    a_defcol, int    a_defrow,
      int    a_maxcol, int    a_maxrow)
{
   /*---(design notes)-------------------*/
   /*
    *  this function is infrequently used, so i made it highly defensive.
    *  i don't want messed up files to blow up the system ;)
    *
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_senter  (__FUNCTION__);
   DEBUG_INPT   yLOG_spoint  (a_cell);
   /*---(defense)------------------------*/
   --rce;  if (a_cell == NULL) {
      DEBUG_INPT   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_INPT   yLOG_sinfo   ("a_cell"    , a_cell);
   /*---(fixing limits and defaults)-----*/
   if (a_defcol < 0)         a_defcol = DEF_COLS;
   if (a_defcol > MAX_COLS)  a_defcol = MAX_COLS;
   if (a_maxcol < 0)         a_maxcol = DEF_COLS;
   if (a_maxcol > MAX_COLS)  a_maxcol = MAX_COLS;
   if (a_defrow < 0)         a_defrow = DEF_ROWS;
   if (a_defrow > MAX_ROWS)  a_defrow = MAX_ROWS;
   if (a_maxrow < 0)         a_maxrow = DEF_ROWS;
   if (a_maxrow > MAX_ROWS)  a_maxrow = MAX_ROWS;
   /*---(parse)--------------------------*/
   rc = LOC_parse (a_cell, NULL, &x_col, &x_row, NULL);
   DEBUG_INPT   yLOG_sint    (rc);
   /*---(add min value)------------------*/
   x_col += a_min;
   x_row += a_min;
   /*---(column checking)----------------*/
   if (rc < 0 || x_col <= 0)  x_col = a_defcol;
   if (x_col >  a_maxcol)     x_col = a_maxcol;
   /*---(row checking)-------------------*/
   if (rc < 0 || x_row <= 0)  x_row = a_defrow;
   if (x_row >  a_maxrow)     x_row = a_maxrow;
   /*---(display)------------------------*/
   DEBUG_INPT   yLOG_sint    (x_col);
   DEBUG_INPT   yLOG_sint    (x_row);
   /*---(assign)-------------------------*/
   if (a_col != NULL)  *a_col = x_col;
   if (a_row != NULL)  *a_row = x_row;
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_sexit   (__FUNCTION__);
   return 0;
}

#define     FIELD_RREG     1
#define     FIELD_RTAB     2
#define     FIELD_RBEG     3
#define     FIELD_REND     4
#define     FIELD_RMIN     5
#define     FIELD_RMAX     6
#define     FIELD_RTYPE    7
#define     FIELD_RDATA    8

char         /*--> parse a register entry ----------------[ flower [--------]-*/
INPT_register      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         i           = 0;
   char       *p           = NULL;               /* strtok return pointer     */
   int         x_len       = 0;
   int         x_tab       = 0;
   char        x_regid     = ' ';
   char        x_beg       [10];
   char        x_end       [10];
   char        x_min       [10];
   char        x_max       [10];
   char        x_type      = ' ';
   char        x_regtest   [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(read fields)--------------------*/
   for (i = FIELD_RREG; i <= FIELD_RDATA; ++i) {
      DEBUG_INPT   yLOG_note    ("read next field");
      p = strtok_r (NULL  , s_q, &s_context);
      --rce;  if (p == NULL) {
         DEBUG_INPT   yLOG_note    ("strtok_r came up empty");
         break;
      }
      if (i != FIELD_RDATA)  strltrim (p, ySTR_BOTH, LEN_RECD);
      /*> if (p [0] == '-')  p[0] = '\0';                                             <*/
      x_len = strlen (p);
      switch (i) {
      case  FIELD_RREG :  /*--------------*/
         --rce;  if (x_len != 1)  {
            DEBUG_INPT  yLOG_note    ("reg id not correct");
            DEBUG_INPT  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         x_regid = p[0];
         DEBUG_INPT  yLOG_char    ("regid"     , x_regid);
         break;
      case  FIELD_RTAB  : /*-------------*/
         x_tab = atoi (p);
         DEBUG_INPT  yLOG_value   ("tab num"   , x_tab);
         --rce;  if (x_tab == 0 && strcmp (p, "0") != 0) {
            DEBUG_INPT  yLOG_note    ("tab number not correct");
            DEBUG_INPT  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         --rce;  if (x_tab >= 10) {
            DEBUG_INPT  yLOG_note    ("tab number too large");
            DEBUG_INPT  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         DEBUG_INPT   yLOG_note    ("initializing tab");
         break;
      case  FIELD_RBEG  : /*-------------*/
         strlcpy (x_beg, p, 10);
         break;
      case  FIELD_REND  : /*-------------*/
         strlcpy (x_end, p, 10);
         break;
      case  FIELD_RMIN  : /*-------------*/
         strlcpy (x_min, p, 10);
         break;
      case  FIELD_RMAX  : /*-------------*/
         strlcpy (x_max, p, 10);
         break;
      case  FIELD_RTYPE : /*-------------*/
         --rce;  if (x_len != 1)  {
            DEBUG_INPT  yLOG_note    ("reg type not correct");
            DEBUG_INPT  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         x_type = p[0];
         DEBUG_INPT  yLOG_char    ("type"      , x_type);
         if (x_type != 't')  REG_read (x_regid, x_tab, x_beg, x_end, x_min, x_max, x_type);
         break;
      case  FIELD_RDATA : /*-------------*/
         if (x_type == 't')  TREG_read (x_regid, x_beg, atoi (x_min), atoi (x_max), p + 1);
         break;
      }
      DEBUG_INPT   yLOG_note    ("done with loop");
   } 
   DEBUG_INPT   yLOG_note    ("done parsing fields");
   REG_entry (x_regid, x_regtest);
   DEBUG_INPT   yLOG_info    ("regtest"   , x_regtest);
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

#define     FIELD_TAB      1
#define     FIELD_MAX      2
#define     FIELD_TOP      3
#define     FIELD_CUR      4
#define     FIELD_FTYPE    5
#define     FIELD_FBEG     6
#define     FIELD_FEND     7
#define     FIELD_NAME     8

char         /*--> parse a tab entry ---------------------[ flower [--------]-*/
INPT_tab_new       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         i           = 0;
   char       *p           = NULL;               /* strtok return pointer     */
   char       *q           = "";               /* strtok delimeters         */
   int         x_len       = 0;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   char        x_type      = 0;
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(read fields)--------------------*/
   for (i = FIELD_TAB; i <= FIELD_NAME; ++i) {
      DEBUG_INPT   yLOG_note    ("read next field");
      p = strtok_r (NULL  , q, &s_context);
      --rce;  if (p == NULL) {
         DEBUG_INPT   yLOG_note    ("strtok_r came up empty");
         break;
      }
      strltrim (p, ySTR_BOTH, LEN_RECD);
      /*> if (p [0] == '-')  p[0] = '\0';                                             <*/
      x_len = strlen (p);
      switch (i) {
      case  FIELD_TAB   :  /*---(tab number)-------*/
         x_tab = atoi (p);
         DEBUG_INPT  yLOG_value   ("tab num"   , x_tab);
         if (x_tab == 0 && strcmp (p, "0") != 0) {
            DEBUG_INPT  yLOG_note    ("tab number not correct");
            DEBUG_INPT  yLOG_exit    (__FUNCTION__);
            return rce + i;
         }
         DEBUG_INPT   yLOG_note    ("initializing tab");
         TAB_init (x_tab);
         break;
      case  FIELD_MAX   : /*---(size of sheet)-----*/
         rc = INPT_rowcol (p, &(tabs[x_tab].ncol), &(tabs[x_tab].nrow),
               1, DEF_COLS, DEF_ROWS, MAX_COLS, MAX_ROWS);
         break;
      case  FIELD_TOP   : /*---(top of screen)-----*/
         rc = INPT_rowcol (p, &(tabs[x_tab].bcol), &(tabs[x_tab].brow),
               0, 0, 0, tabs[x_tab].ncol, tabs[x_tab].nrow);
         DEBUG_INPT   yLOG_value   ("bcol"      , tabs[x_tab].bcol);
         DEBUG_INPT   yLOG_value   ("brow"      , tabs[x_tab].brow);
         break;
      case  FIELD_CUR   : /*---(cur position)------*/
         rc = INPT_rowcol (p, &(tabs[x_tab].ccol), &(tabs[x_tab].crow),
               0, 0, 0, tabs[x_tab].ncol, tabs[x_tab].nrow);
         tabs[x_tab].ecol = tabs[x_tab].ccol;
         tabs[x_tab].erow = tabs[x_tab].crow;
         break;
      case  FIELD_FTYPE : /*---(freeze type)-------*/
         DEBUG_INPT   yLOG_char    ("freeze"    , p[0]);
         switch (p[0]) {
         case  '-' : tabs[x_tab].froz_col  = '-';
                     tabs[x_tab].froz_row  = '-';
                     break;
         case  'r' : tabs[x_tab].froz_col  = '-';
                     tabs[x_tab].froz_row  = 'y';
                     break;
         case  'c' : tabs[x_tab].froz_col  = 'y';
                     tabs[x_tab].froz_row  = '-';
                     break;
         case  'b' : tabs[x_tab].froz_col  = 'y';
                     tabs[x_tab].froz_row  = 'y';
                     break;
         default   : tabs[x_tab].froz_col  = '-';
                     tabs[x_tab].froz_row  = '-';
                     break;
         }
         break;
      case  FIELD_FBEG  : /*---(freeze beg)--------*/
         rc = INPT_rowcol (p, &(tabs[x_tab].froz_bcol), &(tabs[x_tab].froz_brow),
               0, 0, 0, tabs[x_tab].ncol, tabs[x_tab].nrow);
         break;
      case  FIELD_FEND  : /*---(freeze end)--------*/
         rc = INPT_rowcol (p, &(tabs[x_tab].froz_ecol), &(tabs[x_tab].froz_erow),
               0, 0, 0, tabs[x_tab].ncol, tabs[x_tab].nrow);
         break;
      case  FIELD_NAME  : /*---(name)--------------*/
         if (x_len > 0)  strlcpy (tabs[x_tab].name, p, LEN_STR);
         DEBUG_INPT   yLOG_info    ("name"      , p);
         tabs[x_tab].active = 'y';
         NTAB               = x_tab + 1;
         break;
      }
      DEBUG_INPT   yLOG_note    ("done with loop");
   } 
   DEBUG_INPT   yLOG_note    ("done parsing fields");
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> parse a tab entry ---------------------[ flower [--------]-*/
INPT_tabF          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         i           = 0;
   char       *p           = NULL;               /* strtok return pointer     */
   char       *q           = "";               /* strtok delimeters         */
   int         x_len       = 0;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   char        x_type      = 0;
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(read fields)--------------------*/
   for (i = 1; i < 20; ++i) {
      DEBUG_INPT   yLOG_note    ("read next field");
      p = strtok_r (NULL  , q, &s_context);
      --rce;  if (p == NULL) {
         DEBUG_INPT   yLOG_note    ("strtok_r came up empty");
         break;
      }
      strltrim (p, ySTR_BOTH, LEN_RECD);
      /*> if (p [0] == '-')  p[0] = '\0';                                             <*/
      x_len = strlen (p);
      switch (i) {
      case  1 :  /*---(tab number)-------*/
         x_tab = atoi (p);
         DEBUG_INPT  yLOG_value   ("tab num"   , x_tab);
         if (x_tab == 0 && strcmp (p, "0") != 0) {
            DEBUG_INPT  yLOG_note    ("tab number not correct");
            DEBUG_INPT  yLOG_exit    (__FUNCTION__);
            return rce + i;
         }
         DEBUG_INPT   yLOG_note    ("initializing tab");
         TAB_init (x_tab);
         break;
      case  2 : /*---(size of sheet)-----*/
         rc = INPT_rowcol (p, &(tabs[x_tab].ncol), &(tabs[x_tab].nrow),
               1, DEF_COLS, DEF_ROWS, MAX_COLS, MAX_ROWS);
         break;
      case  3 : /*---(top of screen)-----*/
         rc = INPT_rowcol (p, &(tabs[x_tab].bcol), &(tabs[x_tab].brow),
               0, 0, 0, tabs[x_tab].ncol, tabs[x_tab].nrow);
         DEBUG_INPT   yLOG_value   ("bcol"      , tabs[x_tab].bcol);
         DEBUG_INPT   yLOG_value   ("brow"      , tabs[x_tab].brow);
         break;
      case  4 : /*---(cur position)------*/
         rc = INPT_rowcol (p, &(tabs[x_tab].ccol), &(tabs[x_tab].crow),
               0, 0, 0, tabs[x_tab].ncol, tabs[x_tab].nrow);
         tabs[x_tab].ecol = tabs[x_tab].ccol;
         tabs[x_tab].erow = tabs[x_tab].crow;
         break;
      case  5 : /*---(freeze type)-------*/
         DEBUG_INPT   yLOG_char    ("freeze"    , p[0]);
         switch (p[0]) {
         case  '-' : tabs[x_tab].froz_col  = '-';
                     tabs[x_tab].froz_row  = '-';
                     break;
         case  'r' : tabs[x_tab].froz_col  = '-';
                     tabs[x_tab].froz_row  = 'y';
                     break;
         case  'c' : tabs[x_tab].froz_col  = 'y';
                     tabs[x_tab].froz_row  = '-';
                     break;
         case  'b' : tabs[x_tab].froz_col  = 'y';
                     tabs[x_tab].froz_row  = 'y';
                     break;
         default   : tabs[x_tab].froz_col  = '-';
                     tabs[x_tab].froz_row  = '-';
                     break;
         }
         break;
      case  6 : /*---(freeze beg)--------*/
         rc = INPT_rowcol (p, &(tabs[x_tab].froz_bcol), &(tabs[x_tab].froz_brow),
               0, 0, 0, tabs[x_tab].ncol, tabs[x_tab].nrow);
         break;
      case  7 : /*---(freeze end)--------*/
         rc = INPT_rowcol (p, &(tabs[x_tab].froz_ecol), &(tabs[x_tab].froz_erow),
               0, 0, 0, tabs[x_tab].ncol, tabs[x_tab].nrow);
         break;
      case  8 : /*---(name)--------------*/
         if (x_len > 0)  strlcpy (tabs[x_tab].name, p, LEN_STR);
         DEBUG_INPT   yLOG_info    ("name"      , p);
         tabs[x_tab].active = 'y';
         NTAB               = x_tab + 1;
         break;
      }
      DEBUG_INPT   yLOG_note    ("done with loop");
   } 
   DEBUG_INPT   yLOG_note    ("done parsing fields");
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> parse a tab entry ---------------------[ flower [--------]-*/
INPT_tab           (
      /*----------+-----------+-----------------------------------------------*/
      cchar      *a_recd)     /* input record (const)                         */
{  /*---(design notes)--------------------------------------------------------*/
   /*
   */
   /*---(locals)-----------+-----------+-*/
   char        x_temp      [LEN_RECD];            /* strtok working string     */
   /*> char       *p           = NULL;               /+ strtok return pointer     +/   <*/
   /*> char       *q           = "\x1F";             /+ strtok delimeters         +/   <*/
   /*> char       *r           = NULL;               /+ strtok context variable   +/   <*/
   int         x_tab       = 0;
   char        rc          = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   char        x_type      = 0;
   int         i           = 0;               /* temp */
   char        x_beg       [10] = "";         /* temp */
   char        x_end       [10] = "";         /* temp */
   char        x_ver       = '-';
   char        rce         = -10;                /* return code for errors    */
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(defense: a_recd null)-----------*/
   DEBUG_INPT  yLOG_point   ("a_recd"    , a_recd);
   --rce;  if (a_recd == NULL) {
      DEBUG_INPT  yLOG_note    ("record pointer can not be null");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_INPT  yLOG_info    ("a_recd"    , a_recd);
   strncpy (my.f_recd, a_recd, LEN_RECD);   /* globally saved version            */
   /*---(defense: a_recd length)---------*/
   s_len = strlen (a_recd);
   DEBUG_INPT  yLOG_value   ("length"    , s_len);
   --rce;  if (s_len <   80) {
      DEBUG_INPT  yLOG_note    ("length shorter than minimum (80)");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (s_len >  120)  {
      DEBUG_INPT  yLOG_note    ("length longer than maximum (120)");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: type/verb)-------------*/
   strncpy (s_recd , a_recd, LEN_RECD);   /* working version                   */
   s_p = strtok_r (s_recd, s_q, &s_context);
   --rce;  if (s_p == NULL) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   strltrim  (s_p, ySTR_BOTH, LEN_RECD);
   DEBUG_INPT  yLOG_info    ("verb"      , s_p);
   --rce;  if (strcmp (s_p, "tab") != 0) {
      DEBUG_INPT  yLOG_note    ("not a tab record");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(ver number)---------------------*/
   s_p = strtok_r (NULL, s_q, &s_context);
   --rce;  if (s_p == NULL) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   strltrim  (s_p, ySTR_BOTH, LEN_RECD);
   --rce;  if (strlen (s_p) != 3) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (s_p[0] != '-')  {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (s_p[2] != '-') {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   x_ver = s_p[1];
   DEBUG_INPT  yLOG_char    ("ver num"   , x_ver);
   --rce;  if (strchr (s_vers, x_ver) == 0) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   if (x_ver == 'F') {
      INPT_tabF ();
      /*> printf ("s_file freeze    : col=%c (%4d to %4d)\n",                          <* 
       *>       tabs[x_tab].froz_col, tabs[x_tab].froz_bcol, tabs[x_tab].froz_ecol);   <* 
       *> printf ("s_file freeze    : row=%c (%4d to %4d)\n",                          <* 
       *>       tabs[x_tab].froz_row, tabs[x_tab].froz_brow, tabs[x_tab].froz_erow);   <*/
      return 0;
   }
   /*---(tab number)---------------------*/
   s_p = strtok_r (NULL, s_q, &s_context);
   --rce;  if (s_p == NULL) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   strltrim  (s_p, ySTR_BOTH, LEN_RECD);
   --rce;  if (strlen (s_p) <= 0)  {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   x_tab = atoi (s_p);
   DEBUG_INPT  yLOG_value   ("tab num"   , x_tab);
   /*---(maximums)-----------------------*/
   if (x_ver >= 'E') {
      x_col = x_row = 0;
      s_p = strtok_r (NULL, s_q, &s_context);
      if (s_p != NULL) {
         strltrim  (s_p, ySTR_BOTH, LEN_RECD);
         DEBUG_INPT  yLOG_info    ("maximum"   , s_p);
         rc = LOC_parse (s_p, NULL, &x_col, &x_row, NULL);
      } else {
         DEBUG_INPT  yLOG_info    ("maximum"   , "not found/readable");
      }
      ++x_col;
      ++x_row;
      if (rc < 0 || x_col <= 0)  x_col = DEF_COLS;
      if (x_col >  MAX_COLS)     x_col = MAX_COLS;
      if (rc < 0 || x_row <= 0)  x_row = DEF_ROWS;
      if (x_row >  MAX_ROWS)     x_row = MAX_ROWS;
      tabs[x_tab].ncol = x_col;
      tabs[x_tab].nrow = x_row;
      DEBUG_INPT  yLOG_value   ("ncol"      , tabs[x_tab].ncol);
      DEBUG_INPT  yLOG_value   ("nrow"      , tabs[x_tab].nrow);
   } else {
      s_p = strtok_r (NULL, s_q, &s_context);
      strltrim (s_p, ySTR_BOTH, LEN_RECD);
      tabs[x_tab].ncol = atoi (s_p);
      DEBUG_INPT  yLOG_value   ("ncol"      , tabs[x_tab].ncol);
      s_p = strtok_r (NULL, s_q, &s_context);
      --rce;  if (s_p == NULL) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      strltrim (s_p, ySTR_BOTH, LEN_RECD);
      tabs[x_tab].nrow = atoi (s_p);
      DEBUG_INPT  yLOG_value   ("nrow"      , tabs[x_tab].nrow);
   }
   /*---(beginning)----------------------*/
   if (x_ver >= 'E') {
      x_col = x_row = 0;
      s_p = strtok_r (NULL, s_q, &s_context);
      if (s_p != NULL) {
         strltrim  (s_p, ySTR_BOTH, LEN_RECD);
         DEBUG_INPT  yLOG_info    ("beginning" , s_p);
         LOC_parse (s_p, NULL, &x_col, &x_row, NULL);
      } else {
         DEBUG_INPT  yLOG_info    ("beginning" , "not found/readable");
      }
      if (x_col <= 0)   x_col = 0;
      if (x_row <= 0)   x_row = 0;
      tabs[x_tab].bcol = x_col;
      tabs[x_tab].brow = x_row;
      DEBUG_INPT  yLOG_value   ("bcol"      , tabs[x_tab].bcol);
      DEBUG_INPT  yLOG_value   ("brow"      , tabs[x_tab].brow);
   } else {
      s_p = strtok_r (NULL, s_q, &s_context);
      --rce;  if (s_p == NULL) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      strltrim (s_p, ySTR_BOTH, LEN_RECD);
      tabs[x_tab].bcol = atoi (s_p);
      DEBUG_INPT  yLOG_value   ("bcol"      , tabs[x_tab].bcol);
      s_p = strtok_r (NULL, s_q, &s_context);
      --rce;  if (s_p == NULL) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      strltrim (s_p, ySTR_BOTH, LEN_RECD);
      tabs[x_tab].brow = atoi (s_p);
      DEBUG_INPT  yLOG_value   ("brow"      , tabs[x_tab].brow);
   }
   /*---(current)------------------------*/
   if (x_ver >= 'E') {
      x_col = x_row = 0;
      s_p = strtok_r (NULL, s_q, &s_context);
      if (s_p != NULL) {
         strltrim  (s_p, ySTR_BOTH, LEN_RECD);
         DEBUG_INPT  yLOG_info    ("current"   , s_p);
         LOC_parse (s_p, NULL, &x_col, &x_row, NULL);
      } else {
         DEBUG_INPT  yLOG_info    ("current"   , "not found/readable");
      }
      if (x_col <= 0)   x_col = 0;
      if (x_row <= 0)   x_row = 0;
      tabs[x_tab].ccol = x_col;
      tabs[x_tab].crow = x_row;
      DEBUG_INPT  yLOG_value   ("ccol"      , tabs[x_tab].ccol);
      DEBUG_INPT  yLOG_value   ("crow"      , tabs[x_tab].crow);
   } else {
      s_p = strtok_r (NULL, s_q, &s_context);
      --rce;  if (s_p == NULL) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      strltrim (s_p, ySTR_BOTH, LEN_RECD);
      tabs[x_tab].ccol = atoi (s_p);
      DEBUG_INPT  yLOG_value   ("ccol"      , tabs[x_tab].ccol);
      s_p = strtok_r (NULL, s_q, &s_context);
      --rce;  if (s_p == NULL) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      strltrim (s_p, ySTR_BOTH, LEN_RECD);
      tabs[x_tab].crow = atoi (s_p);
      DEBUG_INPT  yLOG_value   ("crow"      , tabs[x_tab].crow);
   }
   /*---(frozen)-------------------------*/
   if (x_ver >= 'E') {
      /*---(type)--------------*/
      x_type = '-';
      tabs[x_tab].froz_col  = tabs[x_tab].froz_row  = '-';
      tabs[x_tab].froz_bcol = tabs[x_tab].froz_ecol = 0;
      tabs[x_tab].froz_brow = tabs[x_tab].froz_erow = 0;
      s_p = strtok_r (NULL, s_q, &s_context);
      if (s_p != NULL) {
         strltrim  (s_p, ySTR_BOTH, LEN_RECD);
         x_type = s_p[0];
         DEBUG_INPT  yLOG_char    ("froz_type" , x_type);
      } else {
         DEBUG_INPT  yLOG_info    ("froz_type" , "not found/readable");
      }
      switch (x_type) {
      case  'r' : tabs[x_tab].froz_col  = '-';
                  tabs[x_tab].froz_row  = 'y';
                  break;
      case  'c' : tabs[x_tab].froz_col  = 'y';
                  tabs[x_tab].froz_row  = '-';
                  break;
      case  'b' : tabs[x_tab].froz_col  = 'y';
                  tabs[x_tab].froz_row  = 'y';
                  break;
      }
      if (strchr ("rcb", x_type) != 0) {
         /*---(beg)---------------*/
         x_col = x_row = 0;
         s_p = strtok_r (NULL, s_q, &s_context);
         if (s_p != NULL) {
            strltrim  (s_p, ySTR_BOTH, LEN_RECD);
            DEBUG_INPT  yLOG_info    ("froz_beg"  , s_p);
            LOC_parse (s_p, NULL, &x_col, &x_row, NULL);
         } else {
            DEBUG_INPT  yLOG_info    ("froz_beg"  , "not found/readable");
         }
         if (x_col <= 0)   x_col = 0;
         if (x_row <= 0)   x_row = 0;
         tabs[x_tab].froz_bcol = x_col;
         tabs[x_tab].froz_brow = x_row;
         DEBUG_INPT  yLOG_value   ("froz_bcol" , tabs[x_tab].froz_bcol);
         DEBUG_INPT  yLOG_value   ("froz_brow" , tabs[x_tab].froz_brow);
         /*---(end)---------------*/
         x_col = x_row = 0;
         s_p = strtok_r (NULL, s_q, &s_context);
         if (s_p != NULL) {
            strltrim  (s_p, ySTR_BOTH, LEN_RECD);
            DEBUG_INPT  yLOG_info    ("froz_end"  , s_p);
            LOC_parse (s_p, NULL, &x_col, &x_row, NULL);
         } else {
            DEBUG_INPT  yLOG_info    ("froz_end"  , "not found/readable");
         }
         if (x_col <= 0)   x_col = 0;
         if (x_row <= 0)   x_row = 0;
         tabs[x_tab].froz_ecol = x_col;
         tabs[x_tab].froz_erow = x_row;
         DEBUG_INPT  yLOG_value   ("froz_ecol" , tabs[x_tab].froz_ecol);
         DEBUG_INPT  yLOG_value   ("froz_erow" , tabs[x_tab].froz_erow);
      } else {
         s_p = strtok_r (NULL, "\x1F", &s_context);
      }
   } else if (x_ver == '8') {
      s_p = strtok_r (NULL, "\x1F", &s_context);
      --rce;  if (s_p == NULL) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      strltrim  (s_p, ySTR_BOTH, LEN_RECD);
      --rce;  if (strlen (s_p) < 5) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      tabs[x_tab].froz_col  = '-';
      tabs[x_tab].froz_row  = '-';
      DEBUG_INPT  yLOG_char    ("lock type" , s_p[0]);
      switch (s_p[0]) {
      case  'r' : tabs[x_tab].froz_row  = 'y';
                  break;
      case  'c' : tabs[x_tab].froz_col  = 'y';
                  break;
      case  'b' : tabs[x_tab].froz_col  = 'y';
                  tabs[x_tab].froz_row  = 'y';
                  break;
      }
      DEBUG_INPT  yLOG_char    ("froz_col"  , tabs[x_tab].froz_col);
      DEBUG_INPT  yLOG_char    ("froz_row"  , tabs[x_tab].froz_row);
      if (tabs[x_tab].froz_col == 'y' || tabs[x_tab].froz_row == 'y') {
         strcpy (x_beg, "");
         strcpy (x_end, "");
         for (i = 2; i < 8; ++i) {
            if (s_p[i] == ' ')  break;
            x_beg [i -  2] = s_p [i];
            x_beg [i -  1] = '\0';
         }
         DEBUG_INPT  yLOG_info    ("beg"       , x_beg);
         if (s_p != NULL) {
            rc = LOC_parse (x_beg, NULL, &tabs[x_tab].froz_bcol, &tabs[x_tab].froz_brow, NULL);
         }
         DEBUG_INPT  yLOG_value   ("froz_bcol" , tabs[x_tab].froz_bcol);
         DEBUG_INPT  yLOG_value   ("froz_brow" , tabs[x_tab].froz_brow);
         for (i = 10; i < 16; ++i) {
            if (s_p[i] == ' ')  break;
            x_end [i - 10] = s_p [i];
            x_end [i -  9] = '\0';
         }
         DEBUG_INPT  yLOG_info    ("end"       , x_end);
         if (s_p != NULL) {
            rc = LOC_parse (x_end, NULL, &tabs[x_tab].froz_ecol, &tabs[x_tab].froz_erow, NULL);
         }
         DEBUG_INPT  yLOG_value   ("froz_ecol" , tabs[x_tab].froz_ecol);
         DEBUG_INPT  yLOG_value   ("froz_erow" , tabs[x_tab].froz_erow);
      }
      switch (s_p[0]) {
      case  'b' : if (tabs[x_tab].bcol <= tabs[x_tab].froz_ecol)  tabs[x_tab].bcol = tabs[x_tab].ccol = tabs[x_tab].froz_ecol + 1;
                     tabs[x_tab].brow = tabs[x_tab].crow = tabs[x_tab].froz_erow + 1;
                  break;
      case  'c' : tabs[x_tab].bcol = tabs[x_tab].ccol = tabs[x_tab].froz_ecol + 1;
                  break;
      case  'r' : tabs[x_tab].bcol = tabs[x_tab].ccol = tabs[x_tab].froz_ecol;
                  tabs[x_tab].brow = tabs[x_tab].crow = tabs[x_tab].froz_erow + 1;
                  break;
      }
   }
   /*---(name)---------------------------*/
   s_p = strtok_r (NULL, s_q, &s_context);
   if (s_p != NULL) {
      strltrim  (s_p, ySTR_BOTH, LEN_RECD);
      DEBUG_INPT  yLOG_info    ("name"      , s_p);
      if (strlen (s_p) > 0)  strncpy (tabs[x_tab].name, s_p, LEN_RECD);
   }
   /*---(complet)------------------------*/
   tabs[x_tab].active = 'y';
   NTAB               = x_tab + 1;
   /*---(complet)------------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> write file tab information ------------[ leaf   [ ------ ]-*/
FILE_Otabs         (FILE *a_file, int *a_seq, int a_btab, int a_etab)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;             /* iterator -- tab                */
   char        rc          = 0;             /* generic return code            */
   char        rce         = -10;           /* return code for errors         */
   char        x_type      = '-';
   char        x_addr      [25]        = "";
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   --rce;  if (*a_seq <  0)                      return rce;
   --rce;  if (a_btab <  0)                      return rce;
   --rce;  if (a_btab >= my.ntab)                return rce;
   --rce;  if (a_etab <  a_btab)                 return rce;
   --rce;  if (a_etab <  0)                      return rce;
   --rce;  if (a_etab >= my.ntab)                return rce;
   /*---(header)-------------------------*/
   if (*a_seq == 0) {
      fprintf (a_file, "\n\n\n");
      fprintf (a_file, "#===[[ TABS AND SIZES ]]================================================================================================\n");
      fprintf (a_file, "#--------- %c ver %c tab# %c --max-- %c --beg-- %c --cur-- %c f %c --top-- %c --bot-- %c ---name------------------------\n", 31, 31, 31, 31, 31, 31, 31, 31, 31);
   }
   /*---(tabs)---------------------------*/
   for (i = a_btab; i <= a_etab; ++i) {
      /*---(lead)--------------*/
      fprintf (a_file, "tab        %c -%c- %c %4d ", 31, 'F', 31, i);
      /*---(max)---------------*/
      rc = LOC_ref (i, tabs[i].ncol - 1, tabs[i].nrow - 1, 0, x_addr);
      if (rc < 0)  LOC_ref (i, DEF_COLS - 1, DEF_ROWS - 1, 0, x_addr);
      fprintf (a_file, "%c %-7.7s "        , 31, x_addr);
      /*---(beg)---------------*/
      rc = LOC_ref (i, tabs[i].bcol, tabs[i].brow, 0, x_addr);
      if (rc < 0)  strcpy (x_addr, "------");
      fprintf (a_file, "%c %-7.7s "        , 31, x_addr);
      /*---(cur)---------------*/
      rc = LOC_ref (i, tabs[i].ccol, tabs[i].crow, 0, x_addr);
      if (rc < 0)  strcpy (x_addr, "------");
      fprintf (a_file, "%c %-7.7s "        , 31, x_addr);
      /*---(frozen type)-------*/
      x_type = '-';
      if      (tabs[i].froz_col == 'y' && tabs[i].froz_row == 'y')  x_type = 'b';
      else if (tabs[i].froz_col == 'y' && tabs[i].froz_row != 'y')  x_type = 'c';
      else if (tabs[i].froz_col != 'y' && tabs[i].froz_row == 'y')  x_type = 'r';
      fprintf (a_file, "%c %c "            , 31, x_type);
      /*---(frozen near)-------*/
      rc = LOC_ref (i, tabs[i].froz_bcol, tabs[i].froz_brow, 0, x_addr);
      if (rc < 0)  strcpy (x_addr, "------");
      fprintf (a_file, "%c %-7.7s "        , 31, x_addr);
      /*---(frozen far)--------*/
      rc = LOC_ref (i, tabs[i].froz_ecol, tabs[i].froz_erow, 0, x_addr);
      if (rc < 0)  strcpy (x_addr, "------");
      fprintf (a_file, "%c %-7.7s "        , 31, x_addr);
      /*---(name)--------------*/
      fprintf (a_file, "%c %s", 31, tabs[i].name);
   }
   /*---(complete)-----------------------*/
   fflush (a_file);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    reading and writing cells                 ----===*/
/*====================------------------------------------====================*/
PRIV void  o___SIZES___________o () { return; }


/* initial version
 *
 *   A   #---------  ver  mark  ---loc-- 
 *       mark        -A-   a    0b16     
 *
 *
 */

char
INPT_mark          (void)
{
   char        rce         = -11;
   char        rc          = 0;
   char       *p           = NULL;
   char        x_mark      = ' ';
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(mark)---------------------------*/
   p = strtok_r (NULL, s_q, &s_context);
   --rce;  if (p == NULL) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   strltrim  (p, ySTR_BOTH, LEN_RECD);
   --rce;  if (strlen (p) != 1) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   x_mark = p[0];
   DEBUG_INPT   yLOG_char    ("mark"      , x_mark);
   /*---(location)-----------------------*/
   p = strtok_r (NULL, s_q, &s_context);
   --rce;  if (p == NULL) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   strltrim  (p, ySTR_BOTH, LEN_RECD);
   DEBUG_INPT   yLOG_info    ("label"     , p);
   /*---(process mark)-------------------*/
   rc = MARK_read (x_mark, p);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /* parse a cell entry -----------------------[--------[--------]-*/
INPT_cellreal      (int a_tab, int a_col, int a_row, char *a_format, char *a_source)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   tCELL      *x_new       = NULL;
   tCELL      *x_merge     = NULL;
   int         i           = 0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(create)-------------------------*/
   x_new = CELL_overwrite (CHG_NOHIST, a_tab, a_col, a_row, a_source, a_format);
   DEBUG_INPT  yLOG_point   ("new"       , x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_INPT  yLOG_warn    ("creation"  , "new cell failed");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(activate tab)-------------*/
   DEBUG_INPT   yLOG_note    ("activate tab");
   tabs [a_tab].active = 'y';
   /*---(check for a merged cell)--*/
   DEBUG_INPT   yLOG_note    ("check for rightward merged cells");
   for (i = x_new->col + 1; i < tabs [a_tab].ncol; i++) {
      x_merge = LOC_cell (a_tab, i, a_row);
      if (x_merge == NULL)    break;
      if (x_merge->a != '+')  break;
      DEP_create (G_DEP_MERGED, x_new, x_merge);
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
}


#define     FIELD_LVL      1
#define     FIELD_SEQ      2
#define     FIELD_LOC      3
#define     FIELD_FOR      4
#define     FIELD_SRC      5

char         /* parse a cell entry -----------------------[--------[--------]-*/
INPT_cellreg       (char a_reg, char *a_label, char *a_format, char *a_source)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          =   0;                /* generic return code       */
   tCELL      *x_new       = NULL;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(create)-------------------------*/
   x_new = CELL__new (LINKED);
   DEBUG_INPT  yLOG_point   ("new"       , x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_INPT  yLOG_warn    ("creation"  , "new cell failed");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(hook to register)---------------*/
   rc = REG__hook  (x_new, a_reg, '-');
   DEBUG_INPT  yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0) {
      DEBUG_INPT  yLOG_warn    ("reg_hook"  , "could not hook to register");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(update fields)------------------*/
   x_new->f = a_format [0];
   x_new->a = a_format [1];
   x_new->d = a_format [2];
   strlcpy (x_new->label, a_label , 10);
   x_new->s = strndup (a_source, LEN_RECD);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
}

char         /* parse a cell entry -----------------------[--------[--------]-*/
INPT_cell_new      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   char       *p           = NULL;
   int         i           = 0;
   int         x_len       = 0;
   char        x_label     [10]        = "";
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   char        x_format    = '-';
   char        x_decs      = '0';
   char        x_align     = '-';
   char        x_bformat   [10]        = "";
   char        x_regid     = '-';
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(read fields)--------------------*/
   for (i = FIELD_LVL; i <= FIELD_SRC ; ++i) {
      /*---(parse field)-----------------*/
      DEBUG_INPT   yLOG_note    ("read next field");
      p = strtok_r (NULL  , s_q, &s_context);
      --rce;  if (p == NULL) {
         DEBUG_INPT   yLOG_note    ("strtok_r came up empty");
         DEBUG_INPT   yLOG_exit    (__FUNCTION__);
         break;
      }
      if (i != FIELD_SRC) strltrim (p, ySTR_BOTH, LEN_RECD);
      x_len = strlen (p);
      DEBUG_INPT  yLOG_info    ("field"     , p);
      /*---(handle)----------------------*/
      switch (i) {
      case  FIELD_LVL :  /*--------------*/
         if (x_len == 1)  {
            x_regid = p[0];
            DEBUG_INPT  yLOG_char    ("regid"     , x_regid);
         }
         break;
      case  FIELD_SEQ :  /*--------------*/
         break;
      case  FIELD_LOC :  /*--------------*/
         strlcpy (x_label, p, LEN_RECD);
         rc = LOC_parse (p, &x_tab, &x_col, &x_row, NULL);
         DEBUG_INPT  yLOG_value   ("rc"        , rc);
         --rce;  if (rc < 0) {
            DEBUG_INPT  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         DEBUG_INPT  yLOG_complex ("address"   , "t=%4d, c=%4d, r=%4d", x_tab, x_col, x_row);
         break;
      case  FIELD_FOR :  /*--------------*/
         --rce;  if (x_len != 9) {
            DEBUG_INPT  yLOG_warn    ("format len", "len wrong");
            DEBUG_INPT  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         x_format = p [2];
         x_decs   = p [4];
         x_align  = p [6];
         sprintf (x_bformat, "%c%c%c", x_format, x_align, x_decs);
         DEBUG_INPT  yLOG_info    ("format"    , x_bformat);
         break;
      case  FIELD_SRC :  /*--------------*/
         DEBUG_INPT  yLOG_info    ("source"    , p + 1);
         if      (strcmp ("cell_dep" , my.f_type) == 0) 
            INPT_cellreal (x_tab, x_col, x_row, x_bformat, p + 1);
         else if (strcmp ("cell_free", my.f_type) == 0) 
            INPT_cellreal (x_tab, x_col, x_row, x_bformat, p + 1);
         else if (strcmp ("cell_reg" , my.f_type) == 0) 
            INPT_cellreg  (x_regid, x_label, x_bformat, p + 1);
         break;
      }
      DEBUG_INPT   yLOG_note    ("done with loop");
   } 
   DEBUG_INPT   yLOG_note    ("done parsing fields");
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /* parse a cell entry -----------------------[--------[--------]-*/
INPT_cellD         (cchar *a_recd)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         i           = 0;
   int         x_len       = 0;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   char        x_format    = '-';
   char        x_decs      = '0';
   char        x_align     = '-';
   char        x_bformat   [10];
   tCELL      *x_new       = NULL;
   tCELL      *x_merge     = NULL;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(read fields)--------------------*/
   for (i = 1; i < 20; ++i) {
      DEBUG_INPT   yLOG_note    ("read next field");
      s_p = strtok_r (NULL  , s_q, &s_context);
      --rce;  if (s_p == NULL) {
         DEBUG_INPT   yLOG_note    ("strtok_r came up empty");
         DEBUG_INPT   yLOG_exit    (__FUNCTION__);
         break;
      }
      if (i != 5) strltrim (s_p, ySTR_BOTH, LEN_RECD);
      x_len = strlen (s_p);
      switch (i) {
      case  1 :  /*---(dep graph)--------*/
         break;
      case  2 :  /*---(seq number)-------*/
         break;
      case  3 :  /*---(location)----------*/
         rc = LOC_parse (s_p, &x_tab, &x_col, &x_row, NULL);
         DEBUG_INPT  yLOG_value   ("rc"        , rc);
         --rce;  if (rc < 0) {
            DEBUG_INPT  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         DEBUG_INPT  yLOG_complex ("address"   , "t=%4d, c=%4d, r=%4d", x_tab, x_col, x_row);
         break;
      case  4 :  /*---(formatting)--------*/
         --rce;  if (x_len != 9) {
            DEBUG_INPT  yLOG_warn    ("format len", "len wrong");
            DEBUG_INPT  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         x_format = s_p [2];
         x_decs   = s_p [4];
         x_align  = s_p [6];
         sprintf (x_bformat, "%c%c%c", x_format, x_align, x_decs);
         DEBUG_INPT  yLOG_info    ("format"    , x_bformat);
         break;
      case  5 :  /*---(source)------------*/
         DEBUG_INPT  yLOG_info    ("source"    , s_p + 1);
         x_new = CELL_overwrite (CHG_NOHIST, x_tab, x_col, x_row, s_p + 1, x_bformat);
         DEBUG_INPT  yLOG_point   ("new"       , x_new);
         --rce;  if (x_new == NULL) {
            DEBUG_INPT  yLOG_warn    ("creation"  , "new cell failed");
            DEBUG_INPT  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         /*---(activate tab)-------------*/
         DEBUG_INPT   yLOG_note    ("activate tab");
         tabs [x_tab].active = 'y';
         /*---(check for a merged cell)--*/
         DEBUG_INPT   yLOG_note    ("check for rightward merged cells");
         for (i = x_new->col + 1; i < tabs [x_tab].ncol; i++) {
            x_merge = LOC_cell (x_tab, i, x_row);
            if (x_merge == NULL)    break;
            if (x_merge->a != '+')  break;
            DEP_create (G_DEP_MERGED, x_new, x_merge);
         }
         break;
      }
      DEBUG_INPT   yLOG_note    ("done with loop");
   } 
   DEBUG_INPT   yLOG_note    ("done parsing fields");
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /* parse a cell entry -----------------------[--------[--------]-*/
INPT_cell          (
      /*----------+-----------+-----------------------------------------------*/
      cchar      *a_recd)     /* input record (const)                         */
{  /*---(design notes)--------------------------------------------------------*/
   /*
   */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;                  /* generic return code       */
   int         x_len       = 0;                  /* generic string length     */
   char        x_temp      [LEN_RECD];            /* strtok working string     */
   char       *p           = NULL;               /* strtok return pointer     */
   char       *q           = "\x1F";             /* strtok delimeters         */
   char       *r           = NULL;               /* strtok context variable   */
   char        x_ver       = ' ';                /* record version number     */
   char        x_verb      [20];                 /* type/verb of record       */
   char        x_format    = '-';
   int         x_decs      = 0;
   char        x_align     = '-';
   char        x_bformat   [10];
   tCELL      *new         = NULL;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(defense: a_recd null)-----------*/
   DEBUG_INPT  yLOG_point   ("a_recd"    , a_recd);
   --rce;  if (a_recd == NULL) {
      DEBUG_INPT  yLOG_warn    ("ERROR"     , "record null");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_INPT  yLOG_info    ("a_recd"    , a_recd);
   /*---(defense: a_recd length)---------*/
   x_len = strlen (a_recd);
   DEBUG_INPT  yLOG_value   ("length"    , x_len);
   --rce;  if (x_len <   55) {
      DEBUG_INPT  yLOG_warn    ("ERROR"     , "length shorter than minimum (60)");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (x_len >  900)  {
      DEBUG_INPT  yLOG_warn    ("ERROR"     , "length longer than maximum (900)");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: type/verb)-------------*/
   strncpy (x_temp, a_recd, LEN_RECD);
   s_p = strtok_r (x_temp, s_q, &s_context);
   --rce;  if (s_p == NULL) {
      DEBUG_INPT  yLOG_warn    ("ERROR"     , "record type null");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   strltrim (s_p, ySTR_BOTH, LEN_RECD);
   strcpy (x_verb, s_p);
   DEBUG_INPT  yLOG_info    ("verb"      , x_verb);
   --rce;  if ((strcmp (x_verb, "cell_dep") != 0) && (strcmp (x_verb, "cell_free") != 0))  {
      DEBUG_INPT  yLOG_warn    ("ERROR"     , "not a cell_dep or cell_free record");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(ver number)---------------------*/
   s_p = strtok_r (NULL, s_q, &s_context);
   --rce;  if (s_p == NULL) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   strltrim  (s_p, ySTR_BOTH, LEN_RECD);
   --rce;  if (strlen (s_p) != 3) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (s_p[0] != '-')  {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (s_p[2] != '-') {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   x_ver = s_p[1];
   DEBUG_INPT  yLOG_char    ("ver num"   , x_ver);
   --rce;  if (strchr (s_vers, x_ver) == 0) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(handle versions)----------------*/
   switch (x_ver) {
   case '*' : /* latest */
   case 'D' : INPT_cellD (x_temp);
              DEBUG_INPT  yLOG_exit    (__FUNCTION__);
              return 0;
              break;
   }
   return -5;




   /*---(sequence)-----------------------*/
   p = strtok_r (NULL, q, &r);
   --rce;  if (p == NULL) {
      DEBUG_INPT  yLOG_warn    ("ERROR"     , "version null");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   x_len = strlen (p);
   strltrim (p, ySTR_BOTH, LEN_RECD);
   if (x_len == 3 && strlen (p) == 1) {
      x_ver = p[0];
      p = strtok_r (NULL, q, &r);
      --rce;  if (p == NULL) {
         DEBUG_INPT  yLOG_warn    ("ERROR"     , "level null");
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      strltrim (p, ySTR_BOTH, LEN_RECD);
   }
   if (x_ver != ' ')  DEBUG_INPT  yLOG_char    ("ver num"   , x_ver);
   else               DEBUG_INPT  yLOG_info    ("ver num"   , "unassigned");
   --rce;  if (strchr (" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", x_ver) == 0) {
      DEBUG_INPT  yLOG_warn    ("ERROR"     , "version number not valid [ A-Za-z]");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(clear level)--------------------*/
   x_len = strlen (p);
   if (x_len == 0 || x_ver > 'F' || strcmp (x_verb, "cell_dep") == 0) {
      DEBUG_INPT  yLOG_info    ("level"     , p);
      p = strtok_r (NULL, q, &r);
      --rce;  if (p == NULL) {
         DEBUG_INPT  yLOG_warn    ("ERROR"     , "sequence null");
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      strltrim (p, ySTR_BOTH, LEN_RECD);
   }
   /*---(clear sequence)-----------------*/
   DEBUG_INPT  yLOG_info    ("sequence"  , p);
   /*---(get location)-------------*/
   p = strtok_r (NULL, q, &r);
   --rce;  if (p == NULL) {
      DEBUG_INPT  yLOG_warn    ("ERROR"     , "null pointer for location");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   strltrim (p, ySTR_BOTH, LEN_RECD);
   DEBUG_INPT  yLOG_info    ("loc"       , p);
   rc = LOC_parse (p, &x_tab, &x_col, &x_row, NULL);
   DEBUG_INPT  yLOG_value   ("rc"        , rc);
   DEBUG_INPT  yLOG_value   ("x_tab"     , x_tab);
   DEBUG_INPT  yLOG_value   ("x_col"     , x_col);
   DEBUG_INPT  yLOG_value   ("x_row"     , x_row);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_warn    ("ERROR"     , "can not parse location");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(formatting)---------------*/
   p = strtok_r (NULL, q, &r);
   --rce;  if (p == NULL) {
      DEBUG_INPT  yLOG_warn    ("ERROR"     , "null pointer for formatting");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   strltrim (p, ySTR_BOTH, LEN_RECD);
   DEBUG_INPT  yLOG_info    ("formatting", p);
   DEBUG_INPT  yLOG_value   ("length"    , strlen (p));
   --rce;
   if        (strlen (p) == 7) {
      x_format = p[2];
      x_decs   = p[4];
      x_align  = p[6];
   } else if (strlen (p) == 8) {   /* if wrote decimals as number */
      x_format = p[2];
      x_decs   = ((p[4] - '0') * 10) + (p[5] - '0');
      x_align  = p[7];
   } else {
      DEBUG_INPT  yLOG_warn    ("ERROR"    , "wrong length for fomatting");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_INPT  yLOG_char    ("x_format"  , x_format);
   DEBUG_INPT  yLOG_char    ("x_decs"    , x_decs);
   DEBUG_INPT  yLOG_char    ("x_align"   , x_align);
   sprintf (x_bformat, "%c%c%c", x_format, x_align, x_decs);
   /*---(contents)-----------------*/
   p = strtok_r (NULL, q, &r);
   --rce;  if (p == NULL) {
      DEBUG_INPT  yLOG_warn    ("ERROR"     , "no contents found");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   x_len = strlen (p);
   /*---(create)-------------------*/
   --rce;
   if (x_len > 1) {
      DEBUG_INPT  yLOG_info    ("source"    , p + 1);
      new = CELL_overwrite (CHG_NOHIST, x_tab, x_col, x_row, p + 1, x_bformat);
      DEBUG_INPT  yLOG_point   ("new"       , new);
      if (new == NULL) {
         DEBUG_INPT  yLOG_warn    ("creation"  , "new cell failed");
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
   } else {
      DEBUG_INPT  yLOG_info    ("source"    , "(nada)");
      new = CELL_overwrite (CHG_NOHIST, x_tab, x_col, x_row, ""   , x_bformat);
      DEBUG_INPT  yLOG_point   ("new"       , new);
      if (new == NULL) {
         DEBUG_INPT  yLOG_warn    ("creation"  , "new cell failed");
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
   }
   /*---(sort it out)--------------*/
   tabs[x_tab].active = 'y';
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_note    ("successful addition of cell");
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          reading files                       ----===*/
/*====================------------------------------------====================*/
static void   o___READ____________o (void) { return; }

char         /*--> open file for reading and prep --------[ leaf   [ ------ ]-*/
INPT_open          (char *a_name)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   DEBUG_INPT  yLOG_point   ("filename"  , a_name);
   /*---(defense)------------------------*/
   --rce;  if (a_name == NULL) {
      DEBUG_INPT  yLOG_note    ("file name can not be null");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   }
   /*---(open file)----------------------*/
   DEBUG_INPT  yLOG_info    ("filename"  , a_name);
   my.f_file = fopen (a_name, "r");
   DEBUG_INPT  yLOG_point   ("f_file"    , my.f_file);
   --rce;  if (my.f_file == NULL) {
      DEBUG_INPT  yLOG_note    ("file could not be openned");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_INPT  yLOG_note    ("file successfully opened");
   /*---(initialize)---------------------*/
   DEBUG_INPT  yLOG_note    ("initializing new environment");
   strncpy (f_maker, "unknown", LEN_RECD);
   my.f_lines = 0;
   NCOL = DEF_COLS;
   BCOL = ECOL = 0;
   NROW = DEF_ROWS;
   BROW = EROW = 0;
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
   DEBUG_INPT  yLOG_note    ("close file");
   fclose  (my.f_file);
   /*---(screen)-------------------------*/
   DEBUG_INPT   yLOG_note    ("set screen positions correctly");
   KEYS_basics (' ', 'r');
   KEYS_bcol (BCOL);
   CURS_colhead();
   KEYS_brow (BROW);
   CURS_rowhead();
   /*---(calculate)----------------------*/
   DEBUG_INPT  yLOG_note    ("recalc");
   SEQ_calc_full ();
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> process a column width record ---------[ leaf   [ ------ ]-*/
INPT_width         (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char       *p;
   int         rc          = 0;
   int         x_tab       = 0;
   int         x_col       = 0;
   /*---(parse address)------------*/
   rc = LOC_parse (my.f_vers, &x_tab, &x_col, NULL, NULL);
   --rce;  if (rc < 0)         return rce;
   /*---(parse width)--------------*/
   p = strtok_r (NULL, s_q, &s_context);
   --rce;  if (p == NULL)      return rce;
   strltrim (p, ySTR_BOTH, LEN_RECD);
   /*---(update column)------------*/
   tabs[x_tab].cols [x_col].w = atoi (p);
   tabs[x_tab].active         = 'y';
   /*---(complete)-----------------*/
   return 0;
}

char         /*--> process a row height record -----------[ leaf   [ ------ ]-*/
INPT_height        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char       *p;
   int         rc          = 0;
   int         x_tab       = 0;
   int         x_row       = 0;
   /*---(parse address)------------*/
   rc = LOC_parse (my.f_vers, &x_tab, NULL, &x_row, NULL);
   --rce;  if (rc < 0)         return rce;
   /*---(parse height)-------------*/
   p = strtok_r (NULL, s_q, &s_context);
   --rce;  if (p == NULL)      return rce;
   strltrim (p, ySTR_BOTH, LEN_RECD);
   /*---(update column)------------*/
   tabs[x_tab].rows [x_row].h = atoi (p);
   tabs[x_tab].active         = 'y';
   /*---(complete)-----------------*/
   return 0;
}

char         /* file reading driver ----------------------[--------[--------]-*/
INPT_main          (char *a_name)
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
   /*---(read lines)---------------------*/
   DEBUG_INPT  yLOG_note    ("read lines");
   while (1) {
      /*---(read and clean)--------------*/
      ++my.f_lines;
      DEBUG_INPT  yLOG_value   ("line"      , my.f_lines);
      fgets (my.f_recd, LEN_RECD, my.f_file);
      if (feof (my.f_file))  {
         DEBUG_INPT  yLOG_note    ("end of file reached");
         break;
      }
      x_len = strlen (my.f_recd);
      if (x_len <= 0)  {
         DEBUG_INPT  yLOG_note    ("record empty");
         continue;
      }
      my.f_recd [--x_len] = '\0';
      DEBUG_INPT  yLOG_value   ("length"    , x_len);
      DEBUG_INPT  yLOG_info    ("fixed"     , my.f_recd);
      if (my.f_recd [0] == '#') {
         DEBUG_INPT  yLOG_note    ("comment line, skipping");
         continue;
      }
      /*---(get recd type)---------------*/
      p = strtok_r (my.f_recd, s_q, &s_context);
      if (p == NULL) {
         DEBUG_INPT  yLOG_note    ("can not parse type field");
         continue;
      }
      strltrim  (p, ySTR_BOTH, LEN_RECD);
      strncpy   (my.f_type, p,  10);
      DEBUG_INPT  yLOG_info    ("type"      , my.f_type);
      /*---(get version)-----------------*/
      p = strtok_r (NULL     , s_q, &s_context);
      if (p == NULL) {
         DEBUG_INPT  yLOG_note    ("can not parse version field");
         continue;
      }
      strltrim  (p, ySTR_BOTH, LEN_RECD);
      strncpy   (my.f_vers, p,  10);
      DEBUG_INPT  yLOG_info    ("version"   , my.f_vers);
      /*---(handle types)----------------*/
      switch (my.f_type [0]) {
      case 'f' : /* format      */ break;
      case 'v' : /* versioned   */ break;
      case 'w' : INPT_width   ();
                 break;
      case 'h' : INPT_height  ();
                 break;
      case 't' : if (strcmp ("-F-", my.f_vers) == 0)
                    INPT_tab_new ();
                 break;
      case 'm' : if (strcmp ("-A-", my.f_vers) == 0)
                    rc = INPT_mark     ();
                 break;
      case 'c' : if (strcmp ("-D-", my.f_vers) == 0)
                    rc = INPT_cell_new ();
                 if (rc < 0)  ++x_cellbad;
                 break;
      case 'r' : if (strcmp ("-A-", my.f_vers) == 0)
                    INPT_register ();
                 break;
      case 's' : if (strcmp ("-A-", my.f_vers) == 0)
                    INPT_register ();
                 break;
      }
      /*---(versioned)-------------------*/
      if (strcmp (my.f_type, "versioned") == 0) {
         continue;
         /*> ;;                                                                       <* 
          *> DEBUG_INPT  yLOG_note    ("found version entry");                        <* 
          *> p = strtok (NULL, "\x1F");                                               <* 
          *> if (p == NULL)      continue;                                            <* 
          *> strltrim (p, ySTR_BOTH, LEN_RECD);                                        <* 
          *> rc = FILE_version (p, ver_num);                                          <* 
          *> if (rc >= 0)   ver_ctrl = 'y';                                           <* 
          *> DEBUG_INPT  yLOG_info    ("ver_num"   , ver_num);                        <* 
          *> p = strtok (NULL, "\x1F");                                               <* 
          *> if (p == NULL)      continue;                                            <* 
          *> strltrim (p, ySTR_BOTH, LEN_RECD);                                        <* 
          *> strcpy (ver_txt, p);                                                     <* 
          *> DEBUG_INPT  yLOG_info    ("ver_txt"   , ver_txt);                        <* 
          *> ;;                                                                       <*/
      }
   }
   /*---(close file)---------------------*/
   INPT_close ();
   /*---(complete)-------------------------*/
   DEBUG_INPT yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          writing files                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___WRITE___________o () { return; }

char         /*--> write file header ---------------------[ leaf   [ ------ ]-*/
FILE_header        (FILE *a_file)
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
   fprintf (a_file, "#===[[ GENERAL INFO AND SETTINGS ]]==========================================================================\n");
   fprintf (a_file, "#--------- %c -ver- %c ---description ------------------------------------------------\n", 31, 31);
   /*---(format identifiers)---------------*/
   fprintf (a_file, "gyges      %c %5s %c %-60.60s %c\n", 31, VER_NUM, 31, VER_TXT                      , 31);
   fprintf (a_file, "format     %c %5d %c %-60.60s %c\n", 31, 9      , 31, "improved tab records"       , 31);
   /*---(timestamp)------------------------*/
   x_time = time(NULL);
   strftime (x_temp, 100, "%y.%m.%d.%H.%M.%S", localtime(&x_time));
   fprintf (a_file, "timestamp  %c %5s %c %-60.60s %c\n", 31, "-----", 31, x_temp                       , 31);
   /*---(version)------------------------*/
   if (ver_ctrl == 'y') {
      fprintf (a_file, "versioned  %c %5s %c %-60.60s %c\n",
            31, ver_num, 31, ver_txt, 31);
   }
   /*---(complete)-----------------------*/
   fflush (a_file);
   return 0;
}

char         /*--> write file column widths --------------[ leaf   [ ------ ]-*/
FILE_Ocols         (FILE *a_file, int *a_seq, int a_tab, int a_bcol, int a_ecol)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;             /* iterator -- column             */
   char        rc          = 0;             /* generic return code            */
   char        rce         = -10;           /* return code for errors         */
   char        x_label     [20];            /* holder for cell address        */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   --rce;  if (*a_seq <  0)                      return rce;
   --rce;  if (a_tab  <  0)                      return rce;
   --rce;  if (a_tab  >= my.ntab)                return rce;
   --rce;  if (a_bcol <  0)                      return rce;
   --rce;  if (a_bcol >= tabs[a_tab].ncol)       return rce;
   --rce;  if (a_ecol <  a_bcol)                 return rce;
   --rce;  if (a_ecol <  0)                      return rce;
   --rce;  if (a_ecol >= tabs[a_tab].ncol)       return rce;
   /*---(header)-------------------------*/
   if (*a_seq == 0) {
      fprintf (a_file, "\n\n\n");
      fprintf (a_file, "#===[[ COLUMN WIDTHS, changes only ]]========================================================================\n");
      fprintf (a_file, "#--------- %c ---loc-- %c size %c\n", 31, 31, 31);
      fprintf (a_file, "width_def  %c -------- %c %4d %c\n", 31, 31, DEF_WIDTH, 31);
   }
   /*---(columns)------------------------*/
   for (i = a_bcol; i <= a_ecol; ++i) {
      /*---(filter unchanged)------------*/
      if (tabs[a_tab].cols[i].w == DEF_WIDTH)  continue;
      /*---(break every five)------------*/
      if (*a_seq % 5 == 0)  {
         fprintf (a_file, "#--------- %c ---loc-- %c size %c\n", 31, 31, 31);
      }
      /*---(make column address)---------*/
      rc = LOC_ref (a_tab, i, 0, 0, x_label);
      if (rc < 0) continue;
      /*---(write exception)-------------*/
      fprintf (a_file, "width      %c %-8.8s %c %4d %c\n",
            31, x_label, 31, tabs[a_tab].cols[i].w, 31);
      /*---(prepare for next)------------*/
      ++(*a_seq);
   }
   /*---(complete)-----------------------*/
   fflush (a_file);
   return 0;
}

char         /*--> write file row heights ----------------[ leaf   [ ------ ]-*/
FILE_rows          (FILE *a_file, int *a_seq, int a_tab, int a_brow, int a_erow)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;             /* iterator -- row                */
   char        rc          = 0;             /* generic return code            */
   char        rce         = -10;           /* return code for errors         */
   char        x_label     [20];            /* holder for cell address        */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   --rce;  if (*a_seq <  0)                      return rce;
   --rce;  if (a_tab  <  0)                      return rce;
   --rce;  if (a_tab  >= my.ntab)                return rce;
   --rce;  if (a_brow <  0)                      return rce;
   --rce;  if (a_brow >= tabs[a_tab].nrow)       return rce;
   --rce;  if (a_erow <  a_brow)                 return rce;
   --rce;  if (a_erow <  0)                      return rce;
   --rce;  if (a_erow >= tabs[a_tab].nrow)       return rce;
   /*---(header)-------------------------*/
   if (*a_seq == 0) {
      fprintf (a_file, "\n\n\n");
      fprintf (a_file, "#===[[ ROW HEIGHTS, changes only ]]==========================================================================\n");
      fprintf (a_file, "#--------- %c ---loc-- %c size %c\n", 31, 31, 31);
      fprintf (a_file, "height_def %c -------- %c %4d %c\n", 31, 31, DEF_HEIGHT, 31);
   }
   /*---(rows)---------------------------*/
   for (i = a_brow; i <= a_erow; ++i) {
      /*---(filter unchanged)------------*/
      if (tabs[a_tab].rows[i].h == DEF_HEIGHT)  continue;
      /*---(break every five)------------*/
      if (*a_seq % 5 == 0)  {
         fprintf (a_file, "#--------- %c ---loc-- %c size %c\n", 31, 31, 31);
      }
      /*---(make row address)------------*/
      rc = LOC_ref (a_tab, 0, i, 0, x_label);
      if (rc < 0) continue;
      /*---(write exception)-------------*/
      fprintf (a_file, "height     %c %-8.8s %c %4d %c\n",
            31, x_label, 31, tabs[a_tab].rows[i].h, 31);
      /*---(prepare for next)------------*/
      ++(*a_seq);
   }
   /*---(complete)-----------------------*/
   fflush (a_file);
   return 0;
}

char         /*--> write a single cell to a file ---------[ leaf   [ ------ ]-*/
OUTP_cell          (FILE *a_file, char *a_type, int a_seq, char *a_level, tCELL *a_curr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (a_file  == NULL)   return rce;
   --rce;  if (a_type  == NULL)   return rce;
   --rce;  if (a_level == NULL)   return rce;
   --rce;  if (a_curr  == NULL)   return rce;
   /*---(header)-------------------------*/
   if ((a_seq % 5) == 0) {
      fprintf (a_file, "#---------  ver  ---lvl/reg--  -seq-  ---loc-- ");
      fprintf (a_file, " t-f-d-a-m  ---source--------------------------------------\n");
   }
   /*---(write entry)--------------------*/
   fprintf (a_file, "%-10.10s  -D-  %-12.12s  %5d  %-8.8s  %c %c %c %c %c  %s\n",
         a_type, a_level, a_seq,
         a_curr->label,
         a_curr->t, a_curr->f, a_curr->d, a_curr->a, a_curr->n,
         a_curr->s);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> write file dependent cells ------------[ leaf   [ ------ ]-*/
FILE_dep           (FILE *a_file, int a_seq, int a_level, tCELL *a_curr)
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
FILE_cells         (FILE *a_file, int *a_seq, long a_stamp, int a_tab, int a_bcol, int a_ecol, int a_brow, int a_erow)
{
   /*---(locals)-----------+-----------+-*/
   int         x           = 0;             /* iterator -- columns            */
   int         y           = 0;             /* iterator -- row                */
   char        rc          = 0;             /* generic return code            */
   char        rce         = -10;           /* return code for errors         */
   char        x_label     [20];            /* holder for cell address        */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   --rce;  if (*a_seq <  0)                      return rce;
   --rce;  if (a_tab  <  0)                      return rce;
   --rce;  if (a_tab  >= my.ntab)                return rce;
   --rce;  if (a_bcol <  0)                      return rce;
   --rce;  if (a_bcol >= tabs[a_tab].ncol)       return rce;
   --rce;  if (a_ecol <  a_bcol)                 return rce;
   --rce;  if (a_ecol <  0)                      return rce;
   --rce;  if (a_ecol >= tabs[a_tab].ncol)       return rce;
   --rce;  if (a_brow <  0)                      return rce;
   --rce;  if (a_brow >= tabs[a_tab].nrow)       return rce;
   --rce;  if (a_erow <  a_brow)                 return rce;
   --rce;  if (a_erow <  0)                      return rce;
   --rce;  if (a_erow >= tabs[a_tab].nrow)       return rce;
   /*---(cells)--------------------------*/
   for (x = a_bcol; x <= a_ecol; ++x) {
      for (y = a_brow; y <= a_erow; ++y) {
         if (tabs[a_tab].sheet[x][y]    == NULL)        continue;
         if (tabs[a_tab].sheet[x][y]->s == NULL)        continue;
         if (tabs[a_tab].sheet[x][y]->t == '-'    )     continue;
         if (tabs[a_tab].sheet[x][y]->t == 'l'    )     continue;
         if (tabs[a_tab].sheet[x][y]->u == a_stamp)     continue;
         OUTP_cell (a_file, "cell_free", *a_seq, "", tabs[a_tab].sheet[x][y]);
         ++(*a_seq);
      }
   }
   /*---(complete)-----------------------*/
   fflush (a_file);
   return 0;
}

char
FILE_write         (char *a_name)
{
   /*---(locals)-----------+-----------+-*/
   FILE       *f           = NULL;
   char       *p;
   int         x_seq;
   int         i           = 0;       /* iterator -- columns                            */
   long        x_stamp     = 0;
   char        rc          = 0;
   char        x_temp      [100];
   char       *x_bufs      = "abcdefghijklmnopqrstuvwxyz";
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_INPT yLOG_enter   (__FUNCTION__);
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
   FILE_header (f);
   /*---(tab data)-----------------------*/
   x_seq = 0;
   rc = FILE_Otabs    (f, &x_seq, 0, my.ntab - 1);
   /*---(column data)--------------------*/
   x_seq = 0;
   for (i = 0; i < NTAB; ++i) {
      rc = FILE_Ocols (f, &x_seq, i, 0, tabs[i].ncol - 1);
   }
   if (x_seq == 0)  fprintf (f, "# no column widths vary from default\n");
   /*---(row data)-----------------------*/
   x_seq = 0;
   for (i = 0; i < NTAB; ++i) {
      rc = FILE_rows    (f, &x_seq, i, 0, tabs[i].nrow - 1);
   }
   if (x_seq == 0)  fprintf (f, "# no row heights vary from default\n");
   /*---(dependent cells)------------------*/
   fprintf (f, "\n\n\n#===[[ DEPENDENCY TREE CELLS, in reverse order ]]====================================================================#\n");
   x_stamp   = rand ();
   rc = SEQ_file_deps (x_stamp, f);
   fprintf (f, "# dependent cells complete\n");
   /*---(non-dependency cells)-------------*/
   fprintf (f, "\n\n\n#===[[ INDENPENDENT CELLS, tab then col then row order]]=============================================================#\n");
   x_seq     = 0;
   for (i = 0; i < NTAB; ++i) {
      rc = FILE_cells   (f, &x_seq, x_stamp, i, 0, tabs[i].ncol - 1, 0, tabs[i].nrow - 1);
   }
   fprintf (f, "# independent cells complete\n");
   /*---(buffer contents)------------------*/
   fprintf (f, "\n\n\n#===[[ REGISTER CELLS, in proper order ]]============================================================================#\n");
   x_seq     = 0;
   x_len     = strlen (x_bufs);
   for (i = 0; i < x_len; ++i) {
      rc = REG_write    (f, &x_seq, x_bufs [i]);
   }
   if (x_seq == 0)  fprintf (f, "# no cells in any lettered registers\n");
   else             fprintf (f, "# register cells complete, count = %d\n", x_seq);
   /*---(buffer contents)------------------*/
   fprintf (f, "\n\n\n#===[[ TEXT REGISTERS ]]=============================================================================================#\n");
   x_seq     = 0;
   x_len     = strlen (x_bufs);
   for (i = 0; i < x_len; ++i) {
      rc = TREG_write   (f, &x_seq, x_bufs [i]);
   }
   if (x_seq == 0)  fprintf (f, "# no cells in any lettered registers\n");
   else             fprintf (f, "# register cells complete, count = %d\n", x_seq);
   /*---(marks)----------------------------*/
   fprintf (f, "\n\n\n#===[[ LOCATION and OBJECT MARKS ]]==================================================================================#\n");
   x_seq     = 0;
   rc = MARK_write (f, &x_seq);
   if      (rc    <  0)  fprintf (f, "# ERROR writing marks\n");
   else if (x_seq == 0)  fprintf (f, "# no lettered marks\n");
   else                  fprintf (f, "# lettered marks complete, count = %d\n", x_seq);
   /*---(footer data)----------------------*/
   fprintf (f, "\n\n\n# done, finito, complete\n");
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
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_file           : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "ver_num"   )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file ver_num   : %s", ver_num);
   } else if (strcmp (a_question, "freeze"    )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file freeze    : col=%c (%4d to %4d)   row=%c (%4d to %4d)",
            tabs[a_ref].froz_col, tabs[a_ref].froz_bcol, tabs[a_ref].froz_ecol,
            tabs[a_ref].froz_row, tabs[a_ref].froz_brow, tabs[a_ref].froz_erow);
   } else if (strcmp (a_question, "tab_name"  )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file tab name  : tab=%4d, act=%c, :%s:", a_ref, tabs[a_ref].active, tabs[a_ref].name);
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
