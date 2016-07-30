/*===============================[[ beg code ]]===============================*/

/*====================-----------------+------------------====================*/
/*===----                           OVERVIEW                           ----===*/
/*====================-----------------+------------------====================*/
/*
 *   s_sel allows the identification of retangular sections of cells for a
 *   particular treatment, including, copying and formatting.  as such,
 *   selection is a crucial part of a spreadsheets function.  in a console
 *   environment, selection is the keyboard equivalent of mouse highlighting.
 *
 *   since selection is vital to all applications that allow any form of
 *   pointing or identification, a consistent interface will allow users to
 *   quickly become efficient and effective in each tool.  also, consistency
 *   will allow deeper tool usage as training will easily translate across
 *   applications.
 *
 *   gyges selection will be based on the usage and capabilities of vi/vim, but
 *   as mentioned above, generalized to be useful in a broad range of
 *   applications.  the resulting consistent command set is called the vi-keys
 *   mini-language.  because of this desired consistency, though, i will have to
 *   manage a series of broad applicability versus power/depth trade-offs which
 *   will result in better transportability, but perhaps a slightly sub-optimal
 *   feature set for a spreadsheet.
 *
 *   to be useful in a broad range of applications, selection must allow some
 *   very flexible options that would not be required in a text editor, such as,
 *   vi/vim.  these variations include non-retangular, multiple, and saved
 *   selections.
 *
 *
 *
 *
 *
 */



#include   "gyges.h"



/*====================-----------------+------------------====================*/
/*===----                   SELECTION DATA STRUCTURE                   ----===*/
/*====================-----------------+------------------====================*/
/*
 *   the selection data structure is really just a convienence as there will
 *   only be one selection allowed at a time.  the selection contains locations
 *   on the source cells and flags about the mode of selection.
 *
 */
typedef     struct cVISU    tVISU;
struct cVISU {
   /*---(#1, FLAGS)----------------------*/
   /*   these couple flags track whether the selection is currently underway  */
   /*   and which of several modes of selection is in operation.  please see  */
   /*   the legend below for the options used in these two flags.             */
   char        live;         /* is the selection active: 0=no, 1=yes          */
   char        mode;         /* what type of selection mode (see below)       */
   /*---(#2, ORIGIN)---------------------*/
   /*   these variables store the very first point in the selection, i.e.,    */
   /*   where it started, which is very important to one of the selection     */
   /*   modes.                                                                */
   int         otab;         /* tab of source cells                           */
   int         ocol;         /* original selection starting column            */
   int         orow;         /* original selection starting row               */
   /*---(#3, BOUNDS)---------------------*/
   /*   these four variables store the upper-left and lower-right bounds of   */
   /*   the selected range.  they are continually updated during selection,   */
   /*   but differently in the various selection modes.                       */
   int         bcol;         /* leftmost or beginning column                  */
   int         brow;         /* topmost or beginning row                      */
   int         ecol;         /* rightmost or ending column                    */
   int         erow;         /* bottommost or ending row                      */
   /*---(#4, CURRENT)--------------------*/
   /*   this is the current cell in the selection, and while used in normal   */
   /*   selection setting, is also very important when retrieving the         */
   /*   individual cells during operations.                                   */
   int         ccol;
   int         crow;
   /*---(#5, PROCESSING)-----------------*/
   /*   these two pointers are used during range retrieval operations and     */
   /*   represent the upper-left cell in the range and the cell currently     */
   /*   being processed.                                                      */
   int         scol;
   int         srow;
   tCELL      *home;
   tCELL      *curr;
   /*---(end)----------------------------*/
};
static tVISU  s_visu;
static tVISU  s_save;

/*
 *   macros for indicating whether the selection is active or not
 */
#define     VISU_NOT       0
#define     VISU_YES       1



#define     MAX_MARK       100
typedef  struct cMARK  tMARK;
struct cMARK {
   char        label       [10];
   short       tab;
   short       col;
   short       row;
   tCELL      *ref;
};
tMARK       s_mark_info [MAX_MARK];
static char S_MARK_LIST [MAX_MARK] = "'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ()";


/*====================------------------------------------====================*/
/*===----                          initialization                      ----===*/
/*====================------------------------------------====================*/
static void  o___INIT____________o () { return; }

char             /* clear all selections -----------------[ ------ [ ------ ]-*/
VISU_init          (void)
{
   /*---(selection)----------------------*/
   VISU_clear   ();
   VISU_save    ();
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> clear the selection -------------------[ leaf   [ ------ ]-*/
VISU_clear          (void)
{
   /*---(back to original cell)----------*/
   if (s_visu.live == VISU_YES) {
      CTAB = s_visu.otab;
      CCOL = s_visu.ocol;
      CROW = s_visu.orow;
   }
   /*---(backup)-------------------------*/
   VISU_save   ();
   /*---(status)-------------------------*/
   s_visu.live  = VISU_NOT;
   s_visu.mode  = VISU_NONE;
   /*---(clear saved locations)----------*/
   s_visu.otab  = s_visu.ocol  = s_visu.orow  = 0;
   s_visu.bcol  = s_visu.brow  = 0;
   s_visu.ecol  = s_visu.erow  = 0;
   s_visu.ccol  = s_visu.crow  = 0;
   s_visu.scol  = s_visu.srow  = 0;
   /*---(locations)----------------------*/
   s_visu.home  = s_visu.curr  = NULL;
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                            history                           ----===*/
/*====================------------------------------------====================*/
static void  o___HISTORY_________o () { return; }

char         /*--> save the selection --------------------[ leaf   [ ------ ]-*/
VISU_save          (void)
{
   /*---(status)-------------------------*/
   s_save.live  = VISU_NOT;
   s_save.mode  = s_visu.mode;
   /*---(clear saved locations)----------*/
   s_save.otab  = s_visu.otab;
   s_save.ocol  = s_visu.ocol;
   s_save.orow  = s_visu.orow;
   s_save.bcol  = s_visu.bcol;
   s_save.brow  = s_visu.brow;
   s_save.ecol  = s_visu.ecol;
   s_save.erow  = s_visu.erow;
   s_save.ccol  = s_visu.ccol;
   s_save.crow  = s_visu.crow;
   s_save.scol  = s_visu.scol;
   s_save.srow  = s_visu.srow;
   /*---(locations)----------------------*/
   s_save.home  = NULL;
   s_save.curr  = NULL;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> restore the selection -----------------[ leaf   [ ------ ]-*/
VISU_restore       (void)
{
   /*---(status)-------------------------*/
   s_visu.live  = VISU_YES;
   s_visu.mode  = s_save.mode;
   /*---(clear saved locations)----------*/
   s_visu.otab  = s_save.otab;
   s_visu.ocol  = s_save.ocol;
   s_visu.orow  = s_save.orow;
   s_visu.bcol  = s_save.bcol;
   s_visu.brow  = s_save.brow;
   s_visu.ecol  = s_save.ecol;
   s_visu.erow  = s_save.erow;
   s_visu.ccol  = s_save.ccol;
   s_visu.crow  = s_save.crow;
   s_visu.scol  = s_save.scol;
   s_visu.srow  = s_save.srow;
   /*---(locations)----------------------*/
   s_visu.home  = s_save.home;
   s_visu.curr  = s_save.curr;
   /*---(go to the right place)----------*/
   CTAB = s_visu.otab;
   CCOL = s_visu.ccol;
   CROW = s_visu.crow;
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                            setting                           ----===*/
/*====================------------------------------------====================*/
static void  o___SETTING_________o () { return; }

char             /* start the visual selection -----------[ twig   [ 121y9x ]-*/
VISU_start         (int a_tab, int a_col, int a_row, char a_mode)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   /*---(defenses)-----------------------*/
   rc = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);
   --rce;  if (rc < 0)                           return rce;
   /*---(status)-------------------------*/
   s_visu.live  = VISU_YES;
   s_visu.mode  = a_mode;
   /*---(locations)----------------------*/
   s_visu.otab                                                  = a_tab;
   s_visu.ocol  = s_visu.bcol  = s_visu.ecol  = s_visu.ccol  = s_visu.scol  = a_col;
   s_visu.orow  = s_visu.brow  = s_visu.erow  = s_visu.crow  = s_visu.srow  = a_row;
   /*---(locations)----------------------*/
   s_visu.home  = s_visu.curr  = tabs[s_visu.otab].sheet[s_visu.bcol][s_visu.brow];
   /*---(complete)-----------------------*/
   return 0;
}

char             /* adjust the visual selection ----------[ twig   [ 231y9x ]-*/
VISU_increase      (int a_tab, int a_col, int a_row)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   /*---(defenses)-----------------------*/
   --rce;  if (s_visu.live == VISU_NOT )             return rce;
   --rce;  if (a_tab    != s_visu.otab)             return rce;
   rc = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);
   --rce;  if (rc < 0)                           return rce;
   /*---(process)------------------------*/
   if (s_visu.mode == VISU_CUM) {
      if (a_col > s_visu.ecol)    s_visu.ecol = a_col;
      if (a_col < s_visu.bcol)    s_visu.bcol = a_col;
      if (a_row > s_visu.erow)    s_visu.erow = a_row;
      if (a_row < s_visu.brow)    s_visu.brow = a_row;
   } else {
      if (a_col >= s_visu.ocol)  { s_visu.bcol = s_visu.ocol; s_visu.ecol = a_col   ; }
      if (a_row >= s_visu.orow)  { s_visu.brow = s_visu.orow; s_visu.erow = a_row   ; }
      if (a_col <  s_visu.ocol)  { s_visu.bcol = a_col   ; s_visu.ecol = s_visu.ocol; }
      if (a_row <  s_visu.orow)  { s_visu.brow = a_row   ; s_visu.erow = s_visu.orow; }
   }
   /*---(current)------------------------*/
   s_visu.ccol  = a_col;
   s_visu.crow  = a_row;
   /*---(locations)----------------------*/
   s_visu.home  = s_visu.curr  = tabs[s_visu.otab].sheet[s_visu.bcol][s_visu.brow];
   /*---(complete)-----------------------*/
   return 0;
}

char             /* cursor update to visual selection ----[ twig   [ 121n0x ]-*/
VISU_update        (int a_tab, int a_col, int a_row)
{
   if (s_visu.live == VISU_NOT)                               return -1;
   return  VISU_increase (CTAB, CCOL, CROW);
}

char         /*--> apply a specific selection ------------[ ------ [ ------ ]-*/
VISU_set           (
      /*----------+-----------+-----------------------------------------------*/
      int         a_tab,      /* tab of selection                             */
      int         a_bcol,     /* beginning col of selection                   */
      int         a_brow,     /* beginning row of selection                   */
      int         a_ecol,     /* ending col of selection                      */
      int         a_erow)     /* ending col of selection                      */
{  /*---(design notes)--------------------------------------------------------*/
   /* if the two ends of the range are legal, this function will change the   */
   /* current selection to the boundaries passed as arguments.                */
   /*---(locals)-----------+-----------+-*//*---------------------------------*/
   char        rc          = 0;
   char        rce         = -10;
   /*---(prepare)------------------------*/
   s_visu.live  = VISU_NOT;
   VISU_clear ();
   /*---(defense: beginning legal)-------*/
   --rce;
   rc          = LOC_legal (a_tab, a_bcol, a_brow, CELL_FIXED);
   if (rc < 0)  return rce;
   /*---(defense: ending legal)----------*/
   --rce;
   rc          = LOC_legal (a_tab, a_ecol, a_erow, CELL_FIXED);
   if (rc < 0)  return rce;
   /*---(defense: ordering)--------------*/
   --rce;  if (a_bcol > a_ecol)     return rce;
   --rce;  if (a_brow > a_erow)     return rce;
   /*---(set range)----------------------*/
   s_visu.live  = VISU_YES;
   s_visu.mode  = VISU_CUM;
   /*---(locations)----------------------*/
   s_visu.otab                          = a_tab;
   s_visu.ocol  = s_visu.bcol  = s_visu.ccol  = a_bcol;
   s_visu.ecol                          = a_ecol;
   s_visu.orow  = s_visu.brow  = s_visu.crow  = a_brow;
   s_visu.erow                          = a_erow;
   /*---(locations)----------------------*/
   s_visu.home  = s_visu.curr  = tabs[s_visu.otab].sheet[s_visu.bcol][s_visu.brow];
   s_visu.curr  = s_visu.home;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> swap the corners ----------------------[ ------ [ ------ ]-*/
VISU_reverse       (void)
{
   /*---(change root)--------------------*/
   if (s_visu.ccol == s_visu.ecol && s_visu.crow == s_visu.erow) {
      s_visu.ocol = s_visu.ecol;
      s_visu.orow = s_visu.erow;
      CCOL = s_visu.ccol = s_visu.bcol;
      CROW = s_visu.crow = s_visu.brow;
   } else {
      s_visu.ocol = s_visu.bcol;
      s_visu.orow = s_visu.brow;
      CCOL = s_visu.ccol = s_visu.ecol;
      CROW = s_visu.crow = s_visu.erow;
   }
   /*---(complete)-----------------------*/
   return 0;
}


/*====================------------------------------------====================*/
/*===----                            display                           ----===*/
/*====================------------------------------------====================*/
static void  o___DISPLAY_________o () { return; }

char             /* indicate whether cell is the root ----[ leaf   [ 170n0x ]-*/
VISU_root          (int a_tab, int a_col, int a_row)
{
   if (s_visu.live == VISU_NOT)    return 0;
   if (a_tab != s_visu.otab)      return 0;
   if (a_col != s_visu.ocol)      return 0;
   if (a_row != s_visu.orow)      return 0;
   return 1;
}

char             /* indicate whether cell is selected ----[ leaf   [ 170n0x ]-*/
VISU_selected      (int a_tab, int a_col, int a_row)
{
   if (s_visu.live == VISU_NOT)    return 0;
   if (a_tab != s_visu.otab)      return 0;
   if (a_col <  s_visu.bcol)      return 0;
   if (a_col >  s_visu.ecol)      return 0;
   if (a_row <  s_visu.brow)      return 0;
   if (a_row >  s_visu.erow)      return 0;
   return 1;
}



/*====================------------------------------------====================*/
/*===----                           simplifiers                        ----===*/
/*====================------------------------------------====================*/
static void  o___SIMPLIFIERS_____o () { return; }

char       /*----: indicate whether a selection is active/live ---------------*/
VISU_islive        (void) { if (s_visu.live == VISU_YES)  return 1; return 0; }

char       /*----: simplifier for starting at current point ------------------*/
VISU_from          (void) { return VISU_start  (CTAB, CCOL, CROW, VISU_CUM); }

char       /*----: convert to whole-column selection -------------------------*/
VISU_col           (void) { s_visu.brow = 0; s_visu.erow = tabs[s_visu.otab].nrow - 1; return 0; }

char       /*----: convert to whole-row selection ----------------------------*/
VISU_row           (void) { s_visu.bcol = 0; s_visu.ecol = tabs[s_visu.otab].ncol - 1; return 0; }



/*====================------------------------------------====================*/
/*===----                            retrieval                         ----===*/
/*====================------------------------------------====================*/
static void  o___RETRIEVAL_______o () { return; }

/*   these functions only operate on the current selection ('-') as they
 *   support the CELL functions of formatting, column width setting, alignment,
 *   and merging amoung others.  these are all interactive uses.
 */

char         /*--> return selection range borders --------[ ------ [ ------ ]-*/
VISU_range          (
      /*----------+-----------+-----------------------------------------------*/
      int        *a_tab,      /* return variable for tab                      */
      int        *a_bcol,     /* return variable for beginning col            */
      int        *a_brow,     /* return variable for beginning row            */
      int        *a_ecol,     /* return variable for ending col               */
      int        *a_erow)     /* return variable for ending row               */
{  /*---(design notes)--------------------------------------------------------*/
   /* returns the borders of the entire current selection.  if nothing is     */
   /* selected, it returns the current cell as a selection.                   */
   /*---(no selection)-------------------*//*---------------------------------*/
   if (s_visu.live == 0) {
      if (a_tab  != NULL) *a_tab  = CTAB;
      if (a_bcol != NULL) *a_bcol = CCOL;
      if (a_brow != NULL) *a_brow = CROW;
      if (a_ecol != NULL) *a_ecol = CCOL;
      if (a_erow != NULL) *a_erow = CROW;
   }
   /*---(live selection)-----------------*/
   else {
      if (a_tab  != NULL) *a_tab  = s_visu.otab;
      if (a_bcol != NULL) *a_bcol = s_visu.bcol;
      if (a_brow != NULL) *a_brow = s_visu.brow;
      if (a_ecol != NULL) *a_ecol = s_visu.ecol;
      if (a_erow != NULL) *a_erow = s_visu.erow;
   }
   /*---(complete)-----------------------*/
   return 0;
}

tCELL*       /*--> return top-left cell in selection -----[-leaf---[--------]-*/
VISU_first         (
      /*----------+-----------+-----------------------------------------------*/
      int        *a_tab,      /* return holder for tab of first cell          */
      int        *a_col,      /* return holder for col of first cell          */
      int        *a_row)      /* return holder for row of first cell          */
{  /*---(design notes)--------------------------------------------------------*/
   /* if nothing selected, return current cell so this function is always     */
   /* valid.  if selection active, return top-left cell.  this function       */
   /* returns four values -- pointer, tab, col, and row of top-left cell.     */
   /*---(no selection active)------------*//*---------------------------------*/
   if (s_visu.live == 0) {
      if (a_tab != NULL)  *a_tab = CTAB;
      if (a_col != NULL)  *a_col = CCOL;
      if (a_row != NULL)  *a_row = CROW;
      return tabs[CTAB].sheet[CCOL][CROW];
   }
   /*---(set to beginning)---------------*/
   s_visu.ccol  = s_visu.bcol;
   s_visu.crow  = s_visu.brow;
   s_visu.curr  = tabs[s_visu.otab].sheet[s_visu.ccol][s_visu.crow];
   /*---(set the return)-----------------*/
   if (a_tab != NULL)  *a_tab = s_visu.otab;
   if (a_col != NULL)  *a_col = s_visu.ccol;
   if (a_row != NULL)  *a_row = s_visu.crow;
   /*---(complete)-----------------------*/
   return s_visu.curr;
}

tCELL*       /*--> return next cell in selection ---------[-leaf---[--------]-*/
VISU_next          (
      /*----------+-----------+-----------------------------------------------*/
      int        *a_tab,      /* return holder for tab of next cell           */
      int        *a_col,      /* return holder for col of next cell           */
      int        *a_row)      /* return holder for row of next cell           */
{  /*---(design notes)--------------------------------------------------------*/
   /* if nothing selected, return a sentinel as there is no next cell.  if    */
   /* selection active, return next cell in selection by moving right one     */
   /* column and when at right of selection return leftost cell in next row   */
   /* down.  at end, return a sentinel to indicate end of processing.         */
   /* returns four values -- pointer, tab, col, and row of next cell.         */
   /*---(no selection active)------------*//*---------------------------------*/
   if (s_visu.live == 0 || s_visu.ccol == -1) {
      if (a_tab != NULL) *a_tab = -1;
      if (a_col != NULL) *a_col = -1;
      if (a_row != NULL) *a_row = -1;
      return  DONE_DONE;
   }
   /*---(update position)----------------*/
   ++s_visu.ccol;
   if (s_visu.ccol > s_visu.ecol) {
      s_visu.ccol = s_visu.bcol;
      ++s_visu.crow;
   }
   /*---(check for end)------------------*/
   if (s_visu.crow > s_visu.erow) {
      s_visu.ccol     = -1;
      s_visu.crow     = -1;
      s_visu.curr     = DONE_DONE;
      if (a_tab != NULL) *a_tab = -1;
      if (a_col != NULL) *a_col = -1;
      if (a_row != NULL) *a_row = -1;
      return  DONE_DONE;
   }
   /*---(set the return)-----------------*/
   if (a_tab != NULL) *a_tab = s_visu.otab;
   if (a_col != NULL) *a_col = s_visu.ccol;
   if (a_row != NULL) *a_row = s_visu.crow;
   s_visu.curr  = tabs[s_visu.otab].sheet[s_visu.ccol][s_visu.crow];
   /*---(complete)-----------------------*/
   return s_visu.curr;
}



/*====================------------------------------------====================*/
/*===----                      location marks                          ----===*/
/*====================------------------------------------====================*/
static void  o___MARKS___________o () { return; }

char
MARK_init          (void)
{
   int         i           = 0;
   for (i = 0; i < MAX_MARK; ++i) {
      MARK_unset (S_MARK_LIST [i]);
   }
   my.mark_show = '-';
   my.mark_save = '-';
   my.mark_head = '-';
   my.mark_tail = '-';
   return 0;
}

char
MARK_unset         (char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   char        rce         = -10;
   char       *x_mark      = NULL;
   int         x_index     =   0;
   char        x_label     [10];
   /*---(check mark)---------------------*/
   x_mark = strchr (S_MARK_LIST, a_mark);
   --rce;  if (x_mark == NULL) {
      return rce;
   }
   /*---(get mark index)-----------------*/
   x_index = (int) (x_mark - S_MARK_LIST);
   --rce;  if (x_index >= MAX_MARK) {
      return rce;
   }
   /*---(clear mark)---------------------*/
   strlcpy (s_mark_info [x_index].label, "", 10);
   s_mark_info [x_index].tab  = -1;
   s_mark_info [x_index].col  = -1;
   s_mark_info [x_index].row  = -1;
   s_mark_info [x_index].ref  = NULL;
   /*---(complete)-----------------------*/
   return 0;
}

char
MARK_which         (void)
{
   int         i           = 0;
   for (i = 0; i < MAX_MARK; ++i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      if (s_mark_info [i].tab != CTAB)             continue;
      if (s_mark_info [i].col != CCOL)             continue;
      if (s_mark_info [i].row != CROW)             continue;
      return S_MARK_LIST [i];
   }
   /*---(complete)-----------------------*/
   return -1;
}

char
MARK_range         (void)
{
   /*---(design notes)-------------------*/
   /*
    *  do not include single quote register #0 in searches 
    */
   /*---(locals)-----------+-----------+-*/
   int         i           =    0;
   /*---(find next)----------------------*/
   my.mark_head = '-';
   for (i = 1; i < MAX_MARK; ++i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      my.mark_head = S_MARK_LIST [i];
      break;
   }
   /*---(find last)----------------------*/
   my.mark_tail = '-';
   for (i = MAX_MARK - 1; i >  0; --i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      my.mark_tail = S_MARK_LIST [i];
      break;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
MARK_prev          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char       *x_mark      = NULL;
   int         x_index     =    0;
   int         i           =    0;
   /*---(check mark)---------------------*/
   x_mark = strchr (S_MARK_LIST, my.mark_save);
   --rce;  if (x_mark == NULL) {
      return my.mark_tail;
   }
   /*---(get mark index)-----------------*/
   x_index = (int) (x_mark - S_MARK_LIST);
   --rce;  if (x_index >= MAX_MARK) {
      return my.mark_tail;
   }
   /*---(find next)----------------------*/
   for (i = x_index - 1; i > 0; --i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      return S_MARK_LIST [i];
   }
   /*---(complete)-----------------------*/
   return my.mark_tail;
}

char
MARK_next          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char       *x_mark      = NULL;
   int         x_index     =    0;
   int         i           =    0;
   /*---(check mark)---------------------*/
   x_mark = strchr (S_MARK_LIST, my.mark_save);
   --rce;  if (x_mark == NULL) {
      return my.mark_head;
   }
   /*---(get mark index)-----------------*/
   x_index = (int) (x_mark - S_MARK_LIST);
   --rce;  if (x_index >= MAX_MARK) {
      return my.mark_head;
   }
   /*---(find next)----------------------*/
   for (i = x_index + 1; i < MAX_MARK; ++i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      return S_MARK_LIST [i];
   }
   /*---(complete)-----------------------*/
   return my.mark_head;
}

char
MARK_set           (char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   char        rce         = -10;
   char       *x_mark      = NULL;
   int         x_index     =   0;
   char        x_label     [10];
   /*---(check mark)---------------------*/
   x_mark = strchr (S_MARK_LIST, a_mark);
   --rce;  if (x_mark == NULL) {
      return rce;
   }
   /*---(get mark index)-----------------*/
   x_index = (int) (x_mark - S_MARK_LIST);
   --rce;  if (x_index >= MAX_MARK) {
      return rce;
   }
   /*---(set mark)-----------------------*/
   if (a_mark != '\'')  my.mark_save = a_mark;
   s_mark_info [x_index].tab = CTAB;
   s_mark_info [x_index].col = CCOL;
   s_mark_info [x_index].row = CROW;
   /*---(mark label)---------------------*/
   rc = LOC_ref  (CTAB, CCOL, CROW, 0, x_label);
   --rce;  if (rc < 0) {
      return rce;
   }
   strlcpy (s_mark_info [x_index].label, x_label, 10);
   /*---(update range)-------------------*/
   MARK_range ();
   /*---(complete)-----------------------*/
   return 0;
}

char
MARK_label         (char a_mark, char *a_label)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char       *x_mark      = NULL;
   int         x_index     =   0;
   /*---(defense)------------------------*/
   --rce;  if (a_label == NULL)  return rce;
   /*---(check mark)---------------------*/
   x_mark = strchr (S_MARK_LIST, a_mark);
   --rce;  if (x_mark == NULL) {
      strlcpy (a_label, empty, 10);
      return rce;
   }
   /*---(get mark index)-----------------*/
   x_index = (int) (x_mark - S_MARK_LIST);
   --rce;  if (x_index >= MAX_MARK) {
      strlcpy (a_label, empty, 10);
      return rce;
   }
   /*---(complete)-----------------------*/
   strlcpy (a_label, s_mark_info [x_index].label, 10);
   return 0;
}

char
MARK_return        (char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   char       *x_mark      = NULL;
   int         x_index     =   0;
   int         x_tab       = CTAB;
   int         x_col       = CCOL;
   int         x_row       = CROW;
   char        x_label     [10];
   /*---(look for sequences)-------------*/
   if (a_mark == '[')  a_mark = my.mark_head;
   if (a_mark == '>')  a_mark = MARK_next ();
   if (a_mark == '<')  a_mark = MARK_prev ();
   if (a_mark == ']')  a_mark = my.mark_tail;
   /*---(check mark)---------------------*/
   x_mark = strchr (S_MARK_LIST, a_mark);
   --rce;  if (x_mark == NULL) {
      return rce;
   }
   /*---(get mark index)-----------------*/
   x_index = (int) (x_mark - S_MARK_LIST);
   --rce;  if (x_index >= MAX_MARK) {
      return rce;
   }
   /*---(check for existance)------------*/
   --rce;  if (strcmp (s_mark_info [x_index].label, "") == 0) {
      return rce;
   }
   /*---(handle current)-----------------*/
   if (a_mark == '\'') {
      x_tab = s_mark_info [x_index].tab;
      x_col = s_mark_info [x_index].col;
      x_row = s_mark_info [x_index].row;
      strlcpy (x_label, s_mark_info [x_index].label, 10);
      MARK_set ('\'');
      CTAB = x_tab;
      CCOL = x_col;
      CROW = x_row;
   }
   /*---(use mark)-----------------------*/
   else {
      my.mark_save = a_mark;
      MARK_set ('\'');
      CTAB = s_mark_info [x_index].tab;
      CCOL = s_mark_info [x_index].col;
      CROW = s_mark_info [x_index].row;
   }
   /*---(update screen)------------------*/
   if (CTAB != x_tab || CCOL <  BCOL || CCOL > ECOL)  KEYS_gz_family ('z', 'm');
   if (CTAB != x_tab || CROW <  BROW || CROW > EROW)  KEYS_gz_family ('z', 'c');
   /*---(complete)-----------------------*/
   return 0;
}

char
MARK_list          (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        rce         = -10;
   char        x_entry     [20];
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   strncpy (a_list, "-", MAX_STR);   /* special for a null list */
   /*---(walk the list)------------------*/
   strncpy (a_list, ",", MAX_STR);
   for (i = 0; i < MAX_MARK; ++i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      sprintf    (x_entry, "%s,", s_mark_info [i].label);
      strncat    (a_list, x_entry, MAX_STR);
   }
   /*---(catch empty)--------------------*/
   if (strcmp (a_list, ",") == 0)   strcpy (a_list, ".");
   /*---(complete)-----------------------*/
   return 0;
}

char
MARK_listplus      (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        rce         = -10;
   char        x_entry     [20];
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   strncpy (a_list, "-", MAX_STR);   /* special for a null list */
   /*---(walk the list)------------------*/
   strncpy (a_list, ",", MAX_STR);
   for (i = 0; i < MAX_MARK; ++i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      sprintf    (x_entry, "%c:%s,", S_MARK_LIST [i], s_mark_info [i].label);
      strncat    (a_list, x_entry, MAX_STR);
   }
   /*---(catch empty)--------------------*/
   if (strcmp (a_list, ",") == 0)   strcpy (a_list, ".");
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     file input and output                    ----===*/
/*====================------------------------------------====================*/
static void  o___FILE____________o () { return; }

char
MARK_write         (FILE *a_file, int *a_seq)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   int         i           = 0;             /* iterator -- buffer entry       */
   int         c           = 0;
   char        x_mark      = ' ';
   char       *x_point     = NULL;
   int         x_index     = -1;
   /*---(walk through marks)-------------*/
   x_mark = my.mark_head;
   while (x_mark != '-') {
      my.mark_save = x_mark;
      /*---(breaks)-----------------------*/
      if (*a_seq % 5 == 0) {
         fprintf (a_file, "#---------  ver  mark  ---loc-- \n");
      }
      /*---(check mark)---------------------*/
      x_point = strchr (S_MARK_LIST, x_mark);
      --rce;  if (x_point == NULL) {
         return rce;
      }
      /*---(get mark index)-----------------*/
      x_index = (int) (x_point - S_MARK_LIST);
      --rce;  if (x_index >= MAX_MARK) {
         return rce;
      }
      fprintf (a_file, "mark        -A-   %c    %-8.8s \n",
            S_MARK_LIST [x_index] ,
            s_mark_info [x_index].label);
      ++(*a_seq);
      x_mark = MARK_next ();
      if (x_mark == my.mark_head)  break;
   }
   /*---(complete)-----------------------*/
   fflush (a_file);
   return 0;
}

char
MARK_read          (char a_mark, char *a_label)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -11;
   char        rc          = 0;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   char       *x_point     = NULL;
   int         x_index     = 0;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   rc = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);
   /*---(check mark)---------------------*/
   x_point = strchr (S_MARK_LIST, a_mark);
   --rce;  if (x_point == NULL) {
      return rce;
   }
   /*---(get mark index)-----------------*/
   x_index = (int) (x_point - S_MARK_LIST);
   --rce;  if (x_index >= MAX_MARK) {
      return rce;
   }
   /*---(save)---------------------------*/
   s_mark_info [x_index].tab = x_tab;
   s_mark_info [x_index].col = x_col;
   s_mark_info [x_index].row = x_row;
   strlcpy (s_mark_info [x_index].label, a_label, 10);
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}




/*====================------------------------------------====================*/
/*===----                       keyboard input                         ----===*/
/*====================------------------------------------====================*/
static void  o___KEYS____________o () { return; }

char          /* PURPOSE : process keys for marks ----------------------------*/
VISU_mode          (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   /*---(defenses)-----------------------*/
   if (my.mode != MODE_VISUAL)             return -1;   /* wrong mode                    */
   if (a_minor == K_ESCAPE)  {
      my.mode  = MODE_MAP;
      VISU_clear ();
      return  0;
   }
   /*---(check for simple keys-----------*/
   --rce;  if (a_major == ' ') {
      /*---(multikey prefixes)-----------*/
      if (strchr ("gze"   , a_minor) != 0)       return a_minor;
      /*---(submodes)--------------------*/
      switch (a_minor) {
      case '"'      : my.mode    = SMOD_REGISTER;
                      return a_minor;  /* make sure double quote goes in prev char */
                      break;
      case ':'      : strncpy (command , ":", MAX_STR);
                      my.mode    = MODE_COMMAND;
                      return 0;
                      break;
      }
      /*---(actions)---------------------*/
      switch (a_minor) {
      case 'v'      : VISU_reverse ();
                      break;
      }
      /*---(actions)---------------------*/
      /*> switch (a_minor) {                                                          <* 
       *> case 'c'      : VISU_col ();               break;                           <* 
       *> case 'r'      : VISU_row ();               break;                           <* 
       *> }                                                                           <*/
      /*---(normal)----------------------*/
      rc = KEYS_basics (a_major, a_minor);
      if (rc == 0) return 0;
   }
   /*---(goto family)--------------------*/
   if (a_major == 'g') {
      rc = KEYS_gz_family  (a_major, a_minor);
      return 0;
   }
   /*---(scroll family)------------------*/
   if (a_major == 'z') {
      rc = KEYS_gz_family  (a_major, a_minor);
      return 0;
   }
   /*---(end family)---------------------*/
   if (a_major == 'e') {
      rc = KEYS_e_family   (a_major, a_minor);
      return 0;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char          /* PURPOSE : process keys for marks ----------------------------*/
MARK_mode          (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   /*---(defenses)-----------------------*/
   if (my.mode != SMOD_MARK)             return -1;   /* wrong mode                    */
   if (a_minor == K_ESCAPE)  {
      my.mode  = MODE_MAP;
      return  0;
   }
   /*---(check for setting)--------------*/
   --rce;  if (a_major == 'm') {
      switch (a_minor) {
      case '*' : MARK_init ();
                 break;
      case '#' : rc = MARK_which ();
                 if (rc < 0) {
                    my.mode = MODE_MAP;
                    return rce;
                 }
                 MARK_unset (rc);
                 MARK_range ();
                 break;
      case '@' : my.mark_show = 'y';
                 break;
      case '_' : my.mark_show = '-';
                 break;
      case '!' : sta_type = 'm';
                 break;
      case '?' : my.info_win = '\'';
                 break;
      default  : rc = MARK_set (a_minor);
                 if (rc < 0) {
                    my.mode = MODE_MAP;
                    return rce;
                 }
                 break;
      }
   }
   /*---(check for returning)------------*/
   --rce;  if (a_major == '\'') {
      rc = MARK_return (a_minor);
      if (rc < 0)  {
         my.mode = MODE_MAP;
         return rce;
      }
   }
   /*---(failure)------------------------*/
   my.mode = MODE_MAP;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*            /* unit test accessor -------------------[ leaf   [ 210y1x ]-*/
VISU__unit         (char *a_question, char a_reg)
{
   /*---(locals)-----------+-----------+-*/
   char        x_list      [MAX_STR];
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_sel            : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "sel_range"    )  == 0) {
      snprintf (unit_answer, LEN_TEXT, "s_sel range      : %c, ta=%4d, bc=%4d, br=%4d, ec=%4d, er=%4d", s_visu.mode, s_visu.otab, s_visu.bcol, s_visu.brow, s_visu.ecol, s_visu.erow);
   }
   else if (strcmp (a_question, "sel_curr")       == 0) {
      snprintf (unit_answer, LEN_TEXT, "s_sel current    : tab=%4d, col=%4d, row=%4d", s_visu.otab, s_visu.ccol, s_visu.crow);
   }
   else if (strcmp (a_question, "sel_full")       == 0) {
      if (s_visu.curr != DONE_DONE) {
         snprintf (unit_answer, LEN_TEXT, "s_sel full       : tab=%4d, col=%4d, row=%4d, ptr=%9p", s_visu.otab, s_visu.ccol, s_visu.crow, tabs[s_visu.otab].sheet[s_visu.ccol][s_visu.crow]);
      } else {
         snprintf (unit_answer, LEN_TEXT, "s_sel full       : tab=%4d, col=%4d, row=%4d, ptr=%9p", s_visu.otab, s_visu.ccol, s_visu.crow, DONE_DONE);
      }
   }
   /*---(marks)--------------------------*/
   else if (strcmp (a_question, "mark_list")      == 0) {
      MARK_listplus (my.mark_plus);
      snprintf (unit_answer, LEN_TEXT, "s_sel marks      : %-.40s", my.mark_plus);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



/*===============================[[ end code ]]===============================*/