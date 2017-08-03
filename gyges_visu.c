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


typedef     struct cSELC    tSELC;
struct cSELC {
   char        live;         /* is the selection active: 0=no, 1=yes          */
   int         root;         /* first selected position                       */
   int         bpos;         /* start of selection in source                  */
   int         epos;         /* end of selection in source                    */
};
static tSELC  s_selc;
static tSELC  s_prev;

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
   /*---(backup)-------------------------*/
   VISU_save   ();
   /*---(back to original cell)----------*/
   if (s_visu.live == VISU_YES) {
      MOVE_prep   ();
      CTAB = s_visu.otab;
      CCOL = s_visu.ocol;
      CROW = s_visu.orow;
      MOVE_done   ();
   }
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
   MOVE_prep    ();
   CTAB = s_visu.otab;
   CCOL = s_visu.ecol;
   CROW = s_visu.erow;
   MOVE_done    ();
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
   s_visu.home  = s_visu.curr  = LOC_cell_at_loc (s_visu.otab, s_visu.bcol, s_visu.brow);
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
   s_visu.home  = s_visu.curr  = LOC_cell_at_loc (s_visu.otab, s_visu.bcol, s_visu.brow);
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
   s_visu.home  = s_visu.curr  = LOC_cell_at_loc (s_visu.otab, s_visu.bcol, s_visu.brow);
   s_visu.curr  = s_visu.home;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> swap the corners ----------------------[ ------ [ ------ ]-*/
VISU_reverse       (void)
{
   /*---(change root)--------------------*/
   MOVE_prep ();
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
   MOVE_done ();
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
VISU_col           (void) { s_visu.brow = 0; s_visu.erow = LOC_row_max (s_visu.otab) - 1; return 0; }

char       /*----: convert to whole-row selection ----------------------------*/
VISU_row           (void) { s_visu.bcol = 0; s_visu.ecol = LOC_col_max (s_visu.otab) - 1; return 0; }



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
      return LOC_cell_at_curr ();
   }
   /*---(set to beginning)---------------*/
   s_visu.ccol  = s_visu.bcol;
   s_visu.crow  = s_visu.brow;
   s_visu.curr  = LOC_cell_at_loc (s_visu.otab, s_visu.ccol, s_visu.crow);
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
   s_visu.curr  = LOC_cell_at_loc (s_visu.otab, s_visu.ccol, s_visu.crow);
   /*---(complete)-----------------------*/
   return s_visu.curr;
}



/*====================------------------------------------====================*/
/*===----                      selection on text                       ----===*/
/*====================------------------------------------====================*/
static void  o___SELECT__________o () { return; }

char
SELC_clear         (void)
{
   s_selc.live        = VISU_NOT;
   s_selc.root        = -1;
   s_selc.bpos        = -1;
   s_selc.epos        = -1;
   return 0;
}

char
SELC_save          (void)
{
   s_prev.live        = VISU_NOT;
   s_prev.root        = s_selc.root;
   s_prev.bpos        = s_selc.bpos;
   s_prev.epos        = s_selc.epos;
   return 0;
}

char
SELC_restore       (void)
{
   s_selc.live        = VISU_YES;
   s_selc.root        = s_prev.root;
   s_selc.bpos        = s_prev.bpos;
   s_selc.epos        = s_prev.epos;
   return 0;
}

char             /* start the visual selection -----------[ twig   [ 121y9x ]-*/
SELC_start         (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   --rce;  if (my.cpos <  0      ) {
      DEBUG_USER   yLOG_note    ("my.cpos before zero/start");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (my.cpos >= my.npos) {
      DEBUG_USER   yLOG_note    ("my.cpos after end/my.npos");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(change settings)----------------*/
   s_selc.root  = my.cpos;
   s_selc.bpos  = my.cpos;
   s_selc.epos  = my.cpos;
   s_selc.live  = VISU_YES;
   DEBUG_USER   yLOG_value   ("live"      , s_selc.live);
   DEBUG_USER   yLOG_value   ("bpos"      , s_selc.bpos);
   DEBUG_USER   yLOG_value   ("epos"      , s_selc.bpos);
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char             /* adjust the visual selection ----------[ twig   [ 231y9x ]-*/
SELC_increase      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   --rce;  if (my.cpos <  0      ) {
      DEBUG_USER   yLOG_note    ("my.cpos before zero/start");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (my.cpos >= my.npos) {
      DEBUG_USER   yLOG_note    ("my.cpos after end/my.npos");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(process)------------------------*/
   DEBUG_USER   yLOG_value   ("live"      , s_selc.live);
   DEBUG_USER   yLOG_value   ("cpos"      , my.cpos);
   DEBUG_USER   yLOG_value   ("bpos"      , s_selc.bpos);
   DEBUG_USER   yLOG_value   ("epos"      , s_selc.epos);
   if      (my.cpos <  s_selc.bpos)   s_selc.bpos = my.cpos;
   else if (my.cpos >  s_selc.epos)   s_selc.epos = my.cpos;
   else if (my.cpos >  s_selc.epos)   s_selc.epos = my.cpos;
   DEBUG_USER   yLOG_value   ("bpos"      , s_selc.bpos);
   DEBUG_USER   yLOG_value   ("epos"      , s_selc.epos);
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /*----: indicate whether a selection is active/live ---------------*/
SELC_islive        (void)
{
   if (s_selc.live == VISU_YES)  return 1;
   else                          return 0;
}

int        /*----: simplifier for beginning ----------------------------------*/
SELC_from          (void)
{
   if (s_selc.live == VISU_YES)  return s_selc.bpos;
   else                          return my.cpos;
}

int        /*----: simplifier for ending -------------------------------------*/
SELC_to            (void)
{ 
   if (s_selc.live == VISU_YES)  return s_selc.epos;
   else                          return my.cpos;
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
      strlcpy (a_label, g_empty, 10);
      return rce;
   }
   /*---(get mark index)-----------------*/
   x_index = (int) (x_mark - S_MARK_LIST);
   --rce;  if (x_index >= MAX_MARK) {
      strlcpy (a_label, g_empty, 10);
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
   strncpy (a_list, "-", LEN_RECD);   /* special for a null list */
   /*---(walk the list)------------------*/
   strncpy (a_list, ",", LEN_RECD);
   for (i = 0; i < MAX_MARK; ++i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      sprintf    (x_entry, "%s,", s_mark_info [i].label);
      strncat    (a_list, x_entry, LEN_RECD);
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
   strncpy (a_list, "-", LEN_RECD);   /* special for a null list */
   /*---(walk the list)------------------*/
   strncpy (a_list, ",", LEN_RECD);
   for (i = 0; i < MAX_MARK; ++i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      sprintf    (x_entry, "%c:%s,", S_MARK_LIST [i], s_mark_info [i].label);
      strncat    (a_list, x_entry, LEN_RECD);
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

char
VISU_status        (char *a_msg)
{
   /*---(locals)-----------+-----------+-*/
   char        x_orig      [LEN_LABEL] = "";
   char        x_beg       [LEN_LABEL] = "";
   char        x_end       [LEN_LABEL] = "";
   char        x_curr      [LEN_LABEL] = "";
   char        x_sorig     [LEN_LABEL] = "";
   char        x_sbeg      [LEN_LABEL] = "";
   char        x_send      [LEN_LABEL] = "";
   char        x_scurr     [LEN_LABEL] = "";
   /*---(make labels)--------------------*/
   LOC_ref  (s_visu.otab, s_visu.ocol, s_visu.orow, 0, x_orig);
   LOC_ref  (s_visu.otab, s_visu.bcol, s_visu.brow, 0, x_beg );
   LOC_ref  (s_visu.otab, s_visu.ecol, s_visu.erow, 0, x_end );
   LOC_ref  (s_visu.otab, s_visu.ccol, s_visu.crow, 0, x_curr);
   LOC_ref  (s_save.otab, s_save.ocol, s_save.orow, 0, x_sorig);
   LOC_ref  (s_save.otab, s_save.bcol, s_save.brow, 0, x_sbeg );
   LOC_ref  (s_save.otab, s_save.ecol, s_save.erow, 0, x_send );
   LOC_ref  (s_save.otab, s_save.ccol, s_save.crow, 0, x_scurr);
   /*---(make string)--------------------*/
   snprintf (a_msg, 500, "[ %d   curr o=%-6.6s b=%-6.6s e=%-6.6s c=%-6.6s   save o=%-6.6s b=%-6.6s e=%-6.6s c=%-6.6s]", 
         s_visu.live,
         x_orig , x_beg  , x_end  , x_curr ,
         x_sorig, x_sbeg , x_send , x_scurr);
   return 0;
}

char          /* PURPOSE : process keys for marks ----------------------------*/
VISU_mode          (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   /*---(defenses)-----------------------*/
   --rce;  if (yVIKEYS_mode_not (MODE_VISUAL )) {
      return rce;
   }
   if (a_minor == K_ESCAPE)  {
      VISU_clear  ();
      yVIKEYS_mode_exit ();
      return  0;
   }
   /*---(check for simple keys-----------*/
   --rce;  if (a_major == ' ') {
      /*---(multikey prefixes)-----------*/
      if (strchr ("gze"   , a_minor) != 0)       return a_minor;
      /*---(submodes)--------------------*/
      switch (a_minor) {
      case '"'      : yVIKEYS_mode_enter  (SMOD_REGISTER);
                      return a_minor;  /* make sure double quote goes in prev char */
                      break;
      case 'F'      : yVIKEYS_mode_enter  (SMOD_FORMAT);
                      return 0;
                      break;
      case ':'      : strncpy (g_command , ":", LEN_RECD);
                      yVIKEYS_mode_enter  (MODE_COMMAND);
                      return 0;
                      break;
      }
      /*---(actions)---------------------*/
      switch (a_minor) {
      case 'v'      :
         VISU_reverse      ();
         break;
      case 'y'      :
         REG_copy          ();
         /*> VISU_clear        ();                                                    <*/
         yVIKEYS_mode_exit ();
         return  0;
         break;
      case 'x'      :
         REG_cut           ();
         /*> VISU_clear        ();                                                    <*/
         yVIKEYS_mode_exit ();
         return  0;
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

char      SELC_mode          (char  a_major, char  a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_majors    [LEN_RECD]   = "";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr());
   --rce;  if (yVIKEYS_mode_not (SMOD_SELECT)) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check for escape)---------------*/
   if (a_minor == K_ESCAPE)  {
      SELC_clear ();
      DEBUG_USER   yLOG_value   ("live"      , s_selc.live);
      yVIKEYS_mode_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(check for start mark)-----------*/
   --rce;  if (a_major == 'm') {
      SELC_start ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for simple keys-----------*/
   --rce;  if (a_major == ' ') {
      /*---(submodes)--------------------*/
      switch (a_minor) {
      case '"'      : 
         yVIKEYS_mode_enter (SMOD_TEXTREG );
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;  /* make sure double quote goes in prev char */
         break;
      case  'y' : case  'Y' :
         DEBUG_USER   yLOG_note    ("yank selection text");
         TREG_copy   ();
         SELC_clear  ();
         yVIKEYS_mode_exit ();
         break;
      case  'x' : case  'X' :
         DEBUG_USER   yLOG_note    ("delete selection text");
         TREG_copy   ();
         TREG_clear  ();
         SELC_clear  ();
         yVIKEYS_mode_exit ();
         break;
      case  'd' : case  'D' :
         DEBUG_USER   yLOG_note    ("delete selection text");
         TREG_copy   ();
         TREG_delete ();
         SELC_clear  ();
         yVIKEYS_mode_exit ();
         break;
      case  'p' :
         DEBUG_USER   yLOG_note    ("paste after selection text");
         TREG_paste  ('>');
         SELC_clear  ();
         yVIKEYS_mode_exit ();
         break;
      case  'P' :
         DEBUG_USER   yLOG_note    ("paste before selection text");
         TREG_paste  ('<');
         SELC_clear  ();
         yVIKEYS_mode_exit ();
         break;
      }
      /*---(actions)---------------------*/
      switch (a_minor) {
         /*> case 'v'      : VISU_reverse ();                                            <* 
          *>                 break;                                                      <*/
         /*> case 'x'      : REG_cut   ();                                               <* 
          *>                 break;                                                      <*/
      }
      /*---(basic movement)--------------*/
      switch (a_minor) {
      case '0' : EDIT_pos ('0');    break;
      case 'H' : EDIT_pos ('l');    break;
      case 'h' : EDIT_pos ('-');    break;
      case 'l' : EDIT_pos ('+');    break;
      case 'L' : EDIT_pos ('m');    break;
      case '$' : EDIT_pos ('$');    break;
      }
      /*---(word movement)---------------*/
      switch (a_minor) {
      case 'w' : EDIT_pos ('w');    break;
      case 'b' : EDIT_pos ('W');    break;
      case 'e' : EDIT_pos ('e');    break;
      }
      /*---(normal)----------------------*/
      SELC_increase ();
   }
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char          /* PURPOSE : process keys for marks ----------------------------*/
MARK_mode          (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   /*---(defenses)-----------------------*/
   --rce;  if (yVIKEYS_mode_not (SMOD_MARK   )) {
      return rce;
   }
   if (a_minor == K_ESCAPE)  {
      yVIKEYS_mode_exit ();
      return  0;
   }
   /*---(check for setting)--------------*/
   --rce;  if (a_major == 'm') {
      switch (a_minor) {
      case '*' : MARK_init ();
                 break;
      case '#' : rc = MARK_which ();
                 if (rc < 0) {
                    yVIKEYS_mode_exit ();
                    return rce;
                 }
                 MARK_unset (rc);
                 MARK_range ();
                 break;
      case '@' : my.mark_show = 'y';
                 break;
      case '_' : my.mark_show = '-';
                 break;
      case '!' : my.layout_status = G_STATUS_MARK;
                 break;
      case '?' : my.info_win = G_INFO_MARK;
                 break;
      default  : rc = MARK_set (a_minor);
                 if (rc < 0) {
                    yVIKEYS_mode_exit ();
                    return rce;
                 }
                 break;
      }
   }
   /*---(check for returning)------------*/
   --rce;  if (a_major == '\'') {
      rc = MARK_return (a_minor);
      if (rc < 0)  {
         yVIKEYS_mode_exit ();
         return rce;
      }
   }
   /*---(failure)------------------------*/
   yVIKEYS_mode_exit ();
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
   char        x_list      [LEN_RECD];
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_sel            : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "sel_range"    )  == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_sel range      : %c, ta=%4d, bc=%4d, br=%4d, ec=%4d, er=%4d", s_visu.mode, s_visu.otab, s_visu.bcol, s_visu.brow, s_visu.ecol, s_visu.erow);
   }
   else if (strcmp (a_question, "sel_curr")       == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_sel current    : tab=%4d, col=%4d, row=%4d", s_visu.otab, s_visu.ccol, s_visu.crow);
   }
   else if (strcmp (a_question, "sel_full")       == 0) {
      if (s_visu.curr != DONE_DONE) {
         snprintf (unit_answer, LEN_UNIT, "s_sel full       : tab=%4d, col=%4d, row=%4d, ptr=%9p", s_visu.otab, s_visu.ccol, s_visu.crow, LOC_cell_at_loc (s_visu.otab, s_visu.ccol, s_visu.crow));
      } else {
         snprintf (unit_answer, LEN_UNIT, "s_sel full       : tab=%4d, col=%4d, row=%4d, ptr=%9p", s_visu.otab, s_visu.ccol, s_visu.crow, DONE_DONE);
      }
   }
   /*---(marks)--------------------------*/
   else if (strcmp (a_question, "mark_list")      == 0) {
      MARK_listplus (my.mark_plus);
      snprintf (unit_answer, LEN_UNIT, "s_sel marks      : %-.40s", my.mark_plus);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



/*===============================[[ end code ]]===============================*/
