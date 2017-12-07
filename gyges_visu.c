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


static char s_mark_head = '-';
static char s_mark_tail = '-';
static char s_mark_save = '-';

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



#define     S_MAX_MARK       150
typedef  struct cMARK  tMARK;
struct cMARK {
   char        label       [10];
   short       tab;
   short       col;
   short       row;
   tCELL      *ref;
};
tMARK       s_mark_info [S_MAX_MARK];
static char S_MARK_LIST [S_MAX_MARK] = "'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789()";


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

char         /*-> clear all selections ---------------[ shoot  [gz.311.001.02]*/ /*-[00.0000.102.!]-*/ /*-[--.---.---.--]-*/
VISU_init          (void)
{
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(selection)----------------------*/
   s_visu.live = VISU_NOT;
   VISU_clear   ();
   VISU_save    ();
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear the selection ----------------[ ------ [gz.742.001.13]*/ /*-[01.0000.743.A]-*/ /*-[--.---.---.--]-*/
VISU_clear          (void)
{
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(backup)-------------------------*/
   VISU_save   ();
   /*---(back to original cell)----------*/
   if (s_visu.live == VISU_YES) {
      DEBUG_VISU   yLOG_note    ("return to original cell");
      MOVE_prep   ();
      CTAB = s_visu.otab;
      CCOL = s_visu.ocol;
      CROW = s_visu.orow;
      MOVE_done   ();
   }
   /*---(status)-------------------------*/
   DEBUG_VISU   yLOG_note    ("turn range off");
   s_visu.live  = VISU_NOT;
   s_visu.mode  = VISU_NONE;
   /*---(clear saved locations)----------*/
   DEBUG_VISU   yLOG_note    ("initialize range coordinates");
   s_visu.otab  = s_visu.ocol  = s_visu.orow  = 0;
   s_visu.bcol  = s_visu.brow  = 0;
   s_visu.ecol  = s_visu.erow  = 0;
   s_visu.ccol  = s_visu.crow  = 0;
   s_visu.scol  = s_visu.srow  = 0;
   /*---(locations)----------------------*/
   DEBUG_VISU   yLOG_note    ("initialize cell pointers");
   s_visu.home  = s_visu.curr  = NULL;
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                            history                           ----===*/
/*====================------------------------------------====================*/
static void  o___HISTORY_________o () { return; }

char         /*-> save the selection -----------------[ leaf   [gz.742.001.00]*/ /*-[00.0000.023.!]-*/ /*-[--.---.---.--]-*/
VISU_save          (void)
{
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_senter  (__FUNCTION__);
   /*---(status)-------------------------*/
   DEBUG_VISU   yLOG_snote   ("status");
   s_save.live  = VISU_NOT;
   s_save.mode  = s_visu.mode;
   /*---(clear saved locations)----------*/
   DEBUG_VISU   yLOG_snote   ("coords");
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
   DEBUG_VISU   yLOG_snote   ("pointers");
   s_save.home  = NULL;
   s_save.curr  = NULL;
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> restore the selection --------------[ ------ [gz.852.001.02]*/ /*-[00.0000.103.!]-*/ /*-[--.---.---.--]-*/
VISU_restore       (void)
{
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_senter  (__FUNCTION__);
   /*---(status)-------------------------*/
   DEBUG_VISU   yLOG_snote   ("status");
   s_visu.live  = VISU_YES;
   s_visu.mode  = s_save.mode;
   /*---(clear saved locations)----------*/
   DEBUG_VISU   yLOG_snote   ("coords");
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
   DEBUG_VISU   yLOG_snote   ("pointers");
   s_visu.home  = s_save.home;
   s_visu.curr  = s_save.curr;
   /*---(go to the right place)----------*/
   DEBUG_VISU   yLOG_snote   ("pointers");
   MOVE_prep    ();
   CTAB = s_visu.otab;
   CCOL = s_visu.ecol;
   CROW = s_visu.erow;
   MOVE_done    ();
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                            setting                           ----===*/
/*====================------------------------------------====================*/
static void  o___SETTING_________o () { return; }

char         /*-> start the visual selection ---------[ ------ [ge.530.422.12]*/ /*-[00.0000.213.5]-*/ /*-[--.---.---.--]-*/
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

char         /*-> adjust the visual selection --------[ ------ [ge.760.324.D2]*/ /*-[01.0000.015.X]-*/ /*-[--.---.---.--]-*/
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

char         /*-> cursor update to visual selection --[ ------ [gc.412.312.11]*/ /*-[01.0000.204.!]-*/ /*-[--.---.---.--]-*/
VISU_update        (int a_tab, int a_col, int a_row)
{
   /*---(locals)-----------+------+----+-*/
   char        rc          =     0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_senter  (__FUNCTION__);
   DEBUG_VISU   yLOG_char    ("s_visu.live", s_visu.live);
   if (s_visu.live == VISU_NOT) {
      DEBUG_VISU   yLOG_sexitr  (__FUNCTION__, -1);
      return -1;
   }
   rc = VISU_increase (CTAB, CCOL, CROW);
   DEBUG_VISU   yLOG_sint    (rc);
   DEBUG_VISU   yLOG_sexit   (__FUNCTION__);
   return rc;
}

char         /*-> apply a specific selection ---------[ ------ [ge.E54.125.43]*/ /*-[01.0000.213.T]-*/ /*-[--.---.---.--]-*/
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
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   DEBUG_VISU   yLOG_value   ("a_tab"     , a_tab );
   DEBUG_VISU   yLOG_value   ("a_bcol"    , a_bcol);
   DEBUG_VISU   yLOG_value   ("a_brow"    , a_brow);
   DEBUG_VISU   yLOG_value   ("a_ecol"    , a_ecol);
   DEBUG_VISU   yLOG_value   ("a_erow"    , a_erow);
   /*---(prepare)------------------------*/
   DEBUG_VISU   yLOG_note    ("clear existing ranges");
   s_visu.live  = VISU_NOT;
   VISU_clear ();
   /*---(defense: beginning legal)-------*/
   rc  = LOC_legal (a_tab, a_bcol, a_brow, CELL_FIXED);
   DEBUG_VISU   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense: ending legal)----------*/
   rc  = LOC_legal (a_tab, a_ecol, a_erow, CELL_FIXED);
   DEBUG_VISU   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense: ordering)--------------*/
   DEBUG_VISU   yLOG_note    ("check range limits");
   --rce;  if (a_bcol > a_ecol) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_brow > a_erow) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set range)----------------------*/
   DEBUG_VISU   yLOG_note    ("set range live");
   s_visu.live  = VISU_YES;
   s_visu.mode  = VISU_FROM;
   /*---(locations)----------------------*/
   DEBUG_VISU   yLOG_note    ("set range coordinates");
   s_visu.otab  = a_tab;
   s_visu.ocol  = s_visu.bcol  = s_visu.ccol  = a_bcol;
   s_visu.ecol  = a_ecol;
   s_visu.orow  = s_visu.brow  = s_visu.crow  = a_brow;
   s_visu.erow  = a_erow;
   /*---(locations)----------------------*/
   DEBUG_VISU   yLOG_note    ("set range cell pointers");
   s_visu.home  = s_visu.curr  = LOC_cell_at_loc (s_visu.otab, s_visu.bcol, s_visu.brow);
   s_visu.curr  = s_visu.home;
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> apply a specific selection ---------[ ------ [ge.C64.087.63]*/ /*-[01.0000.013.5]-*/ /*-[--.---.---.--]-*/
VISU_mark            (void)
{
   char        rce         =  -10;
   char        rc          =    0;
   short       x_btab      =   -1;
   short       x_bcol      =   -1;
   short       x_brow      =   -1;
   short       x_etab      =   -1;
   short       x_ecol      =   -1;
   short       x_erow      =   -1;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   DEBUG_VISU   yLOG_note    ("clear selection");
   s_visu.live  = VISU_NOT;
   VISU_clear ();
   /*---(get marks)----------------------*/
   rc = MARK_address ('(', &x_btab, &x_bcol, &x_brow);
   DEBUG_VISU   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = MARK_address (')', &x_etab, &x_ecol, &x_erow);
   DEBUG_VISU   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check range)--------------------*/
   --rce;  if (x_btab != x_etab) {
      DEBUG_VISU   yLOG_note    ("btab does not match etab");
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (x_bcol >  x_ecol) {
      DEBUG_VISU   yLOG_note    ("bcol is greater than ecol");
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (x_brow >  x_erow) {
      DEBUG_VISU   yLOG_note    ("brow is greater than erow");
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set selection)------------------*/
   rc = VISU_set (x_btab, x_bcol, x_brow, x_ecol, x_erow);
   DEBUG_VISU   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> swap the corners -------------------[ ------ [gz.530.001.22]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
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

char         /*-> indicate whether cell is the root --[ leaf   [gc.220.305.40]*/ /*-[00.0000.106.L]-*/ /*-[--.---.---.--]-*/
VISU_root          (int a_tab, int a_col, int a_row)
{
   if (s_visu.live == VISU_NOT)    return 0;
   if (a_tab != s_visu.otab)       return 0;
   if (a_col != s_visu.ocol)       return 0;
   if (a_row != s_visu.orow)       return 0;
   return 1;
}

char         /*-> indicate whether cell is selected --[ leaf   [gc.320.307.60]*/ /*-[00.0000.205.#]-*/ /*-[--.---.---.--]-*/
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

char         /*-> selection active or not ------------[ ------ [gc.B50.00#.D7]*/ /*-[01.0000.104.#]-*/ /*-[--.---.---.--]-*/
VISU_islive        (void) { if (s_visu.live == VISU_YES)  return 1; return 0; }

char         /*-> start at current position ----------[ ------ [gc.B40.00#.C6]*/ /*-[01.0000.00#.3]-*/ /*-[--.---.---.--]-*/
VISU_from          (void) { return VISU_start  (CTAB, CCOL, CROW, VISU_CUM); }

char         /*-> convert to whole-column selection --[ ------ [gc.A40.009.C4]*/ /*-[01.0000.00#.2]-*/ /*-[--.---.---.--]-*/
VISU_col           (void) { s_visu.brow = 0; s_visu.erow = LOC_row_max (s_visu.otab) - 1; return 0; }

char         /*-> convert to whole-row selection -----[ ------ [gc.A40.008.C2]*/ /*-[01.0000.00#.1]-*/ /*-[--.---.---.--]-*/
VISU_row           (void) { s_visu.bcol = 0; s_visu.ecol = LOC_col_max (s_visu.otab) - 1; return 0; }



/*====================------------------------------------====================*/
/*===----                            retrieval                         ----===*/
/*====================------------------------------------====================*/
static void  o___RETRIEVAL_______o () { return; }

/*   these functions only operate on the current selection ('-') as they
 *   support the CELL functions of formatting, column width setting, alignment,
 *   and merging amoung others.  these are all interactive uses.
 */

char         /*-> return selection range borders -----[ leaf   [gc.730.107.C0]*/ /*-[01.0000.204.4]-*/ /*-[--.---.---.--]-*/
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

tCELL*       /*-> return top-left cell in selection --[ ------ [gp.630.108.72]*/ /*-[01.0000.804.4]-*/ /*-[--.---.---.--]-*/
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

tCELL*       /*-> return next cell in selection ------[ ------ [gp.940.10#.C1]*/ /*-[01.0000.804.A]-*/ /*-[--.---.---.--]-*/
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

char         /*-> tbd --------------------------------[ leaf   [gz.220.001.00]*/ /*-[00.0000.013.!]-*/ /*-[--.---.---.--]-*/
SELC_clear         (void)
{
   s_selc.live        = VISU_NOT;
   s_selc.root        = -1;
   s_selc.bpos        = -1;
   s_selc.epos        = -1;
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gz.220.001.00]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
SELC_save          (void)
{
   s_prev.live        = VISU_NOT;
   s_prev.root        = s_selc.root;
   s_prev.bpos        = s_selc.bpos;
   s_prev.epos        = s_selc.epos;
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gz.220.001.00]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
SELC_restore       (void)
{
   s_selc.live        = VISU_YES;
   s_selc.root        = s_prev.root;
   s_selc.bpos        = s_prev.bpos;
   s_selc.epos        = s_prev.epos;
   return 0;
}

char         /*-> start the visual selection ---------[ leaf   [ge.732.023.20]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
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

char         /*-> adjust the visual selection --------[ leaf   [ge.733.023.50]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
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

char         /*-> selection active or not ------------[ leaf   [gc.210.002.20]*/ /*-[00.0000.104.!]-*/ /*-[--.---.---.--]-*/
SELC_islive        (void)
{
   if (s_selc.live == VISU_YES)  return 1;
   else                          return 0;
}

int          /*-> simplifier for beginning -----------[ leaf   [gn.210.002.20]*/ /*-[00.0000.404.!]-*/ /*-[--.---.---.--]-*/
SELC_from          (void)
{
   if (s_selc.live == VISU_YES)  return s_selc.bpos;
   else                          return my.cpos;
}

int          /*-> simplifier for ending --------------[ leaf   [gn.210.002.20]*/ /*-[00.0000.404.!]-*/ /*-[--.---.---.--]-*/
SELC_to            (void)
{ 
   if (s_selc.live == VISU_YES)  return s_selc.epos;
   else                          return my.cpos;
}



/*====================------------------------------------====================*/
/*===----                      location marks                          ----===*/
/*====================------------------------------------====================*/
static void  o___MARK_PROG_______o () { return; }

char         /*-> initialize all marks ---------------[ shoot  [gz.311.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
MARK_init            (void)
{
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   MARK_purge  ('A');
   /*---(globals)------------------------*/
   my.mark_show = '-';
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> remove all marks -------------------[ ------ [gz.642.141.12]*/ /*-[01.0000.033.3]-*/ /*-[--.---.---.--]-*/
MARK_purge           (char a_scope)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   int         x_len       = 0;
   char        x_mark      = 0;
   char       *x_list      = "abcdefghijklmnopqrstuvwxyz";
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   DEBUG_MARK   yLOG_value   ("max_mark"  , S_MAX_MARK);
   x_len = strlen (S_MARK_LIST);
   DEBUG_MARK   yLOG_value   ("x_len"     , x_len);
   for (i = 0; i < x_len; ++i) {
      x_mark = S_MARK_LIST [i];
      if (a_scope == 'a' && strchr (x_list, x_mark) == NULL)  continue;
      MARK_unset (x_mark);
   }
   /*---(globals)------------------------*/
   DEBUG_MARK   yLOG_note    ("initialize globals");
   s_mark_save = '-';
   s_mark_head = '-';
   s_mark_tail = '-';
   MARK_range ();
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> shutdown marks ---------------------[ shoot  [gz.311.001.01]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
MARK_wrap            (void)
{
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   MARK_purge  ('A');
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      location marks                          ----===*/
/*====================------------------------------------====================*/
static void  o___MARK_BASICS_____o () { return; }

char         /*-> check mark validity ----------------[ leaf   [ge.833.144.30]*/ /*-[01.0000.0A4.F]-*/ /*-[--.---.---.--]-*/
MARK_valid           (char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char       *x_mark      = NULL;
   int         x_index     =   0;
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_senter  (__FUNCTION__);
   DEBUG_MARK   yLOG_sint    (a_mark);
   /*---(defenses)-----------------------*/
   --rce;  if (a_mark == '\0') {
      DEBUG_MARK   yLOG_snote   ("null is invalid");
      DEBUG_MARK   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(check mark)---------------------*/
   x_mark = strchr (S_MARK_LIST, a_mark);
   DEBUG_MARK   yLOG_spoint  (x_mark);
   --rce;  if (x_mark == NULL) {
      DEBUG_MARK   yLOG_snote   ("not valid");
      DEBUG_MARK   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert to index)---------------*/
   x_index = (int) (x_mark - S_MARK_LIST);
   DEBUG_MARK   yLOG_sint    (x_index);
   /*---(check limits)-------------------*/
   x_len = strlen (S_MARK_LIST);
   --rce;  if (x_index >= x_len) {
      DEBUG_MARK   yLOG_snote   ("over max");
      DEBUG_MARK   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_sexit   (__FUNCTION__);
   return x_index;
}

char         /*-> tbd --------------------------------[ ------ [ge.A52.153.55]*/ /*-[01.0000.033.8]-*/ /*-[--.---.---.--]-*/
MARK_set             (char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   char        rce         = -10;
   int         x_index     =   0;
   char        x_label     [10];
   char        x_prev      =   -1;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_mark);
   /*---(check mark)---------------------*/
   x_index = MARK_valid (a_mark);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set mark)-----------------------*/
   DEBUG_MARK   yLOG_note    ("check previous");
   x_prev = MARK_find (s_mark_info [0].label);
   if (a_mark != '\'') {
      if (s_mark_info [0].tab == -1)   MARK_set ('\'');
      s_mark_save = a_mark;
   } else if (x_prev >= 0) {
      s_mark_info [0].tab = s_mark_info [x_prev].tab;
      s_mark_info [0].col = s_mark_info [x_prev].col;
      s_mark_info [0].row = s_mark_info [x_prev].row;
      s_mark_save = x_prev;
   }
   /*---(mark label)---------------------*/
   DEBUG_MARK   yLOG_note    ("make and save address");
   rc = LOC_ref  (CTAB, CCOL, CROW, 0, x_label);
   --rce;  if (rc < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_note    ("save current position");
   s_mark_info [x_index].tab = CTAB;
   s_mark_info [x_index].col = CCOL;
   s_mark_info [x_index].row = CROW;
   strlcpy (s_mark_info [x_index].label, x_label, 10);
   /*---(update range)-------------------*/
   DEBUG_MARK   yLOG_note    ("update the range");
   MARK_range ();
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear a mark -----------------------[ ------ [ge.732.142.12]*/ /*-[01.0000.033.8]-*/ /*-[--.---.---.--]-*/
MARK_unset           (char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   char        rce         = -10;
   int         x_index     =   0;
   char        x_label     [10];
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_mark);
   /*---(check mark)---------------------*/
   x_index = MARK_valid (a_mark);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear mark)---------------------*/
   DEBUG_MARK   yLOG_note    ("clear values");
   strlcpy (s_mark_info [x_index].label, "", 10);
   s_mark_info [x_index].tab  = -1;
   s_mark_info [x_index].col  = -1;
   s_mark_info [x_index].row  = -1;
   s_mark_info [x_index].ref  = NULL;
   /*---(update range)-------------------*/
   DEBUG_MARK   yLOG_note    ("update the range");
   MARK_range ();
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.E83.174.A7]*/ /*-[01.0000.013.#]-*/ /*-[--.---.---.--]-*/
MARK_return        (char a_mark)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_index     =    0;
   int         x_tab       = CTAB;
   int         x_col       = CCOL;
   int         x_row       = CROW;
   char        x_label     [10];
   char        x_prev      =   -1;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_mark);
   /*---(look for sequences)-------------*/
   DEBUG_MARK   yLOG_note    ("check special shortcuts");
   switch (a_mark) {
   case '[' : a_mark = s_mark_head;   break;
   case ')' : a_mark = MARK_next ();   break;
   case '<' : a_mark = MARK_prev ();   break;
   case '>' : a_mark = s_mark_tail;   break;
   }
   --rce;  if (a_mark < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check mark)---------------------*/
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_mark);
   x_index = MARK_valid (a_mark);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for existance)------------*/
   strlcpy (x_label, s_mark_info [x_index].label, 10);
   DEBUG_MARK   yLOG_info    ("x_label"   , x_label);
   --rce;  if (strcmp (x_label, "") == 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get mark values)----------------*/
   DEBUG_MARK   yLOG_note    ("get values");
   x_tab = s_mark_info [x_index].tab;
   x_col = s_mark_info [x_index].col;
   x_row = s_mark_info [x_index].row;
   /*---(set previous)-------------------*/
   DEBUG_MARK   yLOG_note    ("set previous");
   x_prev = MARK_find (s_mark_info [0].label);
   if (a_mark != '\'') {
      if (a_mark != s_mark_save)  MARK_set ('\'');
      s_mark_save = a_mark;
   } else if (x_prev >= 0) {
      if (x_prev != s_mark_save)  MARK_set ('\'');
      s_mark_save = x_prev;
   }
   /*---(move)---------------------------*/
   DEBUG_MARK   yLOG_note    ("jump to mark");
   LOC_jump (x_tab, x_col, x_row);
   /*---(update screen)------------------*/
   if (CTAB != x_tab || CCOL <  BCOL || CCOL > ECOL)  KEYS_gz_family ('z', 'm');
   if (CTAB != x_tab || CROW <  BROW || CROW > EROW)  KEYS_gz_family ('z', 'c');
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      location marks                          ----===*/
/*====================------------------------------------====================*/
static void  o___MARK_FIND_______o () { return; }

char         /*-> tbd --------------------------------[ leaf   [gc.430.022.40]*/ /*-[01.0000.013.5]-*/ /*-[--.---.---.--]-*/
MARK_which         (void)
{
   int         i           = 0;
   int         x_len       = 0;
   x_len = strlen (S_MARK_LIST);
   for (i = 1; i < x_len; ++i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      if (s_mark_info [i].tab != CTAB)             continue;
      if (s_mark_info [i].col != CCOL)             continue;
      if (s_mark_info [i].row != CROW)             continue;
      return S_MARK_LIST [i];
   }
   /*---(complete)-----------------------*/
   return -1;
}

char         /*-> tbd --------------------------------[ leaf   [ge.732.133.30]*/ /*-[01.0000.024.#]-*/ /*-[--.---.---.--]-*/
MARK_find          (char *a_label)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         i           =    0;
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_point   ("a_label"   , a_label);
   /*---(defense)------------------------*/
   --rce;  if (a_label == NULL) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_info    ("a_label"   , a_label);
   /*---(search)-------------------------*/
   DEBUG_MARK   yLOG_note    ("searching");
   x_len = strlen (S_MARK_LIST);
   for (i = 1; i < x_len; ++i) {
      if (strcmp (s_mark_info [i].label, "")      == 0) continue;
      if (strcmp (s_mark_info [i].label, a_label) != 0) continue;
      DEBUG_MARK   yLOG_value   ("i"         , i);
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return i;
   }
   /*---(complete)-----------------------*/
   --rce;
   DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}



/*====================------------------------------------====================*/
/*===----                      location marks                          ----===*/
/*====================------------------------------------====================*/
static void  o___MARK_SEQ________o () { return; }

char         /*-> tbd --------------------------------[ leaf   [gz.640.021.20]*/ /*-[01.0000.044.!]-*/ /*-[--.---.---.--]-*/
MARK_range         (void)
{
   /*---(design notes)-------------------*/
   /*
    *  do not include single quote register #0 in searches 
    */
   /*---(locals)-----------+-----------+-*/
   int         i           =    0;
   int         x_len       = 0;
   /*---(prepare)------------------------*/
   x_len = strlen (S_MARK_LIST);
   /*---(find next)----------------------*/
   s_mark_head = '-';
   for (i = 1; i < x_len; ++i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      s_mark_head = S_MARK_LIST [i];
      break;
   }
   /*---(find last)----------------------*/
   s_mark_tail = '-';
   for (i = x_len - 1; i >  0; --i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      s_mark_tail = S_MARK_LIST [i];
      break;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.043.21]*/ /*-[01.0000.014.!]-*/ /*-[--.---.---.--]-*/
MARK_prev          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_mark      =  '-';
   int         x_index     =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(check mark)---------------------*/
   x_index = MARK_valid (s_mark_save);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   /*---(find previous)------------------*/
   DEBUG_MARK   yLOG_note    ("search for previous mark");
   for (i = x_index - 1; i > 0; --i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      DEBUG_MARK   yLOG_value   ("found"     , i);
      x_mark = S_MARK_LIST [i];
      DEBUG_MARK   yLOG_char    ("x_mark"    , x_mark);
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return x_mark;
   }
   DEBUG_MARK   yLOG_note    ("not found");
   --rce;
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}

char         /*-> tbd --------------------------------[ ------ [ge.842.053.21]*/ /*-[01.0000.014.!]-*/ /*-[--.---.---.--]-*/
MARK_next          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_mark      =  '-';
   int         x_index     =    0;
   int         i           =    0;
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(check mark)---------------------*/
   x_index = MARK_valid (s_mark_save);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   /*---(find next)----------------------*/
   DEBUG_MARK   yLOG_note    ("search for next mark");
   x_len = strlen (S_MARK_LIST);
   for (i = x_index + 1; i < x_len; ++i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      DEBUG_MARK   yLOG_value   ("found"     , i);
      x_mark = S_MARK_LIST [i];
      DEBUG_MARK   yLOG_char    ("x_mark"    , x_mark);
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return x_mark;
   }
   DEBUG_MARK   yLOG_note    ("not found");
   --rce;
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}



/*====================------------------------------------====================*/
/*===----                      location marks                          ----===*/
/*====================------------------------------------====================*/
static void  o___MARK_INFO_______o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.833.434.32]*/ /*-[01.0000.014.!]-*/ /*-[--.---.---.--]-*/
MARK_address         (char a_mark, short *a_tab, short *a_col, short *a_row)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   int         x_index     =   0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_value   ("a_mark"    , a_mark);
   /*---(check mark)---------------------*/
   x_index = MARK_valid (a_mark);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_value   ("a_mark"    , a_mark);
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   /*---(check address)------------------*/
   --rce;  if (s_mark_info [x_index].tab < 0) {
      DEBUG_MARK   yLOG_note    ("address not set");
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(parse)--------------------------*/
   rc = LOC_parse  (s_mark_info [x_index].label, a_tab, a_col, a_row, NULL);
   DEBUG_MARK   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.833.224.31]*/ /*-[01.0000.114.!]-*/ /*-[--.---.---.--]-*/
MARK_entry           (char a_mark, char *a_entry)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_index     =   0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_value   ("a_mark"    , a_mark);
   DEBUG_MARK   yLOG_point   ("a_entry"   , a_entry);
   /*---(defense)------------------------*/
   --rce;  if (a_entry == NULL) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (a_entry, "  - :           ", LEN_DESC);
   /*---(check mark)---------------------*/
   x_index = MARK_valid (a_mark);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_value   ("a_mark"    , a_mark);
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   /*---(copy)---------------------------*/
   sprintf (a_entry, "  %c : %-8.8s  ", a_mark, s_mark_info [x_index].label);
   DEBUG_MARK   yLOG_info    ("a_entry"   , a_entry);
   --rce;  if (strcmp ("", s_mark_info [x_index].label) == 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, -(rce));
      return -(rce);
   }
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.420.132.11]*/ /*-[00.0000.114.!]-*/ /*-[--.---.---.--]-*/
MARK_status        (char *a_status)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_list      [LEN_RECD]  = "";
   int         c           =    0;
   /*---(defenses)-----------------------*/
   --rce;  if (a_status  == NULL)  return rce;
   /*---(status)-------------------------*/
   MARK_listplus (x_list);
   c = strldcnt (x_list, ':', LEN_RECD);
   snprintf (a_status, 500, " %c,%c,%c,%c %2d %s ", my.mark_show, s_mark_head, s_mark_save, s_mark_tail, c, x_list);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.540.142.30]*/ /*-[01.0000.103.!]-*/ /*-[--.---.---.--]-*/
MARK_list          (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        rce         = -10;
   char        x_entry     [20];
   int         x_len       = 0;
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   strncpy (a_list, "-", LEN_RECD);   /* special for a null list */
   /*---(walk the list)------------------*/
   x_len = strlen (S_MARK_LIST);
   strncpy (a_list, ",", LEN_RECD);
   for (i = 0; i < x_len; ++i) {
      if (strcmp (s_mark_info [i].label, "") == 0) continue;
      sprintf    (x_entry, "%s,", s_mark_info [i].label);
      strncat    (a_list, x_entry, LEN_RECD);
   }
   /*---(catch empty)--------------------*/
   if (strcmp (a_list, ",") == 0)   strcpy (a_list, ".");
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.540.142.30]*/ /*-[01.0000.123.!]-*/ /*-[--.---.---.--]-*/
MARK_listplus      (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        rce         = -10;
   char        x_entry     [20];
   int         x_len       = 0;
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   strncpy (a_list, "-", LEN_RECD);   /* special for a null list */
   /*---(walk the list)------------------*/
   x_len = strlen (S_MARK_LIST);
   strncpy (a_list, ",", LEN_RECD);
   for (i = 1; i < x_len; ++i) {
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
static void  o___MARK_FILE_______o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
MARK_write           (char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   int         x_index     = 0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_value   ("a_mark"    , a_mark);
   /*---(prepare)------------------------*/
   sprintf (my.f_recd, "");
   /*---(check mark)---------------------*/
   x_index = MARK_valid (a_mark);
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index <= 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_mark);
   /*---(check if empty)-----------------*/
   DEBUG_MARK   yLOG_value   ("tab"       , s_mark_info [x_index].tab);
   --rce;  if (s_mark_info [x_index].tab < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(build record)-------------------*/
   DEBUG_MARK   yLOG_note    ("write record");
   sprintf (my.f_recd, "loc_mark    -A-  %c  %-8.8s ",
         a_mark , s_mark_info [x_index].label);
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 1;
}

char         /*-> write file tab information ---------[ leaf   [ge.320.113.10]*/ /*-[00.0000.01#.!]-*/ /*-[--.---.---.--]-*/
MARK_write_head      (FILE *a_file)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   /*---(header)-------------------------*/
   fprintf (a_file, "#===[[ LOCATION MARKS ]]=============================================================================================#\n");
   fprintf (a_file, "#---------  ver  -  address- \n");
   fflush  (a_file);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> write file tab information ---------[ leaf   [ge.420.213.30]*/ /*-[00.0000.01#.!]-*/ /*-[--.---.---.--]-*/
MARK_write_foot      (FILE *a_file, int a_count)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   /*---(header)-------------------------*/
   if (a_count == 0)  fprintf (a_file, "# no location marks\n");
   else               fprintf (a_file, "#---------  ver  -  address- \n");
   fprintf (a_file, "\n\n\n");
   fflush  (a_file);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gc.531.141.23]*/ /*-[01.0000.10#.8]-*/ /*-[--.---.---.--]-*/
MARK_writeall        (FILE *a_file)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   int         c           = 0;
   int         i           = 0;
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(search)-------------------------*/
   rc = MARK_write_head (a_file);
   x_len = strlen (S_MARK_LIST);
   for (i = 1; i < x_len; ++i) {
      rc = MARK_write (S_MARK_LIST [i]);
      if (rc <= 0)   continue;
      if (a_file != NULL)  fprintf (a_file, "%s\n", my.f_recd);
      ++c;
   }
   rc = MARK_write_foot (a_file, c);
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return c;
}

char         /*-> tbd --------------------------------[ ------ [ge.A53.264.33]*/ /*-[01.0000.113.K]-*/ /*-[--.---.---.--]-*/
MARK_read          (char a_mark, char *a_label)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -11;
   char        rc          = 0;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   int         x_index     = 0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_value   ("a_mark"    , a_mark);
   /*---(check mark)---------------------*/
   x_index = MARK_valid (a_mark);
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_mark);
   /*---(check address)------------------*/
   DEBUG_MARK   yLOG_point   ("a_label"   , a_label);
   --rce;  if (a_label == NULL) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_info    ("a_label"   , a_label);
   rc = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);
   DEBUG_MARK   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   DEBUG_MARK   yLOG_note    ("assign values to mark");
   s_mark_info [x_index].tab = x_tab;
   s_mark_info [x_index].col = x_col;
   s_mark_info [x_index].row = x_row;
   strlcpy (s_mark_info [x_index].label, a_label, 10);
   /*---(update range)-------------------*/
   DEBUG_MARK   yLOG_note    ("update the range");
   MARK_range ();
   /*---(complete)-----------------------*/
   DEBUG_MARK  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      mark command line                       ----===*/
/*====================------------------------------------====================*/
static void  o___MARK_COMMAND____o () { return; }

char         /*-> enter a mark directly --------------[ ------ [ge.D54.139.83]*/ /*-[02.0000.00#.O]-*/ /*-[--.---.---.--]-*/
MARK_define          (char *a_string)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_point   ("a_string"  , a_string);
   /*---(defense)------------------------*/
   --rce;  if (a_string == NULL) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_info    ("a_string"  , a_string);
   /*---(check for purge)----------------*/
   if (strcmp ("clear", a_string) == 0) {
      MARK_purge ('a');
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (strcmp ("purge", a_string) == 0) {
      MARK_purge ('A');
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(defense)------------------------*/
   x_len = strlen (a_string);
   DEBUG_MARK   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <  2) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_char    ("2nd char"  , a_string [1]);
   --rce;  if (a_string [1] != '=') {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for unset)----------------*/
   --rce;  if (x_len == 2) {
      rc = MARK_unset (a_string [0]);
      DEBUG_MARK   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(save)---------------------------*/
   rc = MARK_read (a_string [0], a_string + 2);
   --rce;  if (rc < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       keyboard input                         ----===*/
/*====================------------------------------------====================*/
static void  o___KEYS____________o () { return; }

char         /*-> tbd --------------------------------[ ------ [gz.640.181.01]*/ /*-[02.0000.104.!]-*/ /*-[--.---.---.--]-*/
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

char         /*-> process keys for marks -------------[ leaf   [ge.K85.22#.CB]*/ /*-[02.0000.102.!]-*/ /*-[--.---.---.--]-*/
VISU_submode       (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   DEBUG_VISU   yLOG_value   ("a_major"   , a_major);
   DEBUG_VISU   yLOG_value   ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_VISU   yLOG_char    ("mode"      , yVIKEYS_mode_curr ());
   --rce;  if (yVIKEYS_mode_not (MODE_VISUAL )) {
      return rce;
   }
   if (a_minor == G_KEY_ESCAPE)  {
      VISU_clear  ();
      yVIKEYS_mode_exit ();
      DEBUG_VISU   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(check for simple keys-----------*/
   --rce;  if (a_major == ' ') {
      /*---(multikey prefixes)-----------*/
      if (strchr ("gze"   , a_minor) != 0) {
         DEBUG_VISU   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(modes)-----------------------*/
      switch (a_minor) {
      case ':'      : /*--- enter command mode ------*/
         DEBUG_VISU   yLOG_note    ("chose command mode");
         yVIKEYS_mode_enter  (MODE_COMMAND);
         CMDS_start ();
         DEBUG_VISU   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case '/'      : /*--- enter search mode -------*/
         DEBUG_VISU   yLOG_note    ("chose search mode");
         yVIKEYS_mode_enter  (MODE_SEARCH);
         /*> SEARCH_start ();                                                         <*/
         DEBUG_VISU   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      }
      /*---(submodes)--------------------*/
      switch (a_minor) {
      case '"'      :
         DEBUG_VISU   yLOG_note    ("chose register sub-mode");
         yVIKEYS_mode_enter  (SMOD_REGISTER);
         DEBUG_VISU   yLOG_exit    (__FUNCTION__);
         return a_minor;  /* make sure double quote goes in prev char */
         break;
      case 'F'      : case 'f'      :
         DEBUG_VISU   yLOG_note    ("chose format sub-mode");
         yVIKEYS_mode_enter  (SMOD_FORMAT);
         DEBUG_VISU   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      }
      /*---(actions)---------------------*/
      rc = KEYS_regbasic (a_major, a_minor);
      if (rc == 0) {
         yVIKEYS_mode_exit ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      if (a_minor == 'o') {
         VISU_reverse      ();
      }
      /*---(normal)----------------------*/
      rc = KEYS_basics (a_major, a_minor);
      if (rc == 0) {
         DEBUG_VISU   yLOG_exit    (__FUNCTION__);
         return 0;
      }
   }
   /*---(goto family)--------------------*/
   if (a_major == 'g') {
      rc = KEYS_gz_family  (a_major, a_minor);
      DEBUG_VISU   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(scroll family)------------------*/
   if (a_major == 'z') {
      rc = KEYS_gz_family  (a_major, a_minor);
      DEBUG_VISU   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(end family)---------------------*/
   if (a_major == 'e') {
      rc = KEYS_e_family   (a_major, a_minor);
      DEBUG_VISU   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.LD4.225.8C]*/ /*-[02.0000.302.!]-*/ /*-[--.---.---.--]-*/
SELC_mode          (char  a_major, char  a_minor)
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
   if (a_minor == G_KEY_ESCAPE)  {
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

char         /*-> process keys for marks -------------[ leaf   [ge.UD8.24#.JA]*/ /*-[03.0000.102.E]-*/ /*-[--.---.---.--]-*/
MARK_submode       (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *x_majors    = "m'@";
   static char x_prev      =  '-';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   DEBUG_USER   yLOG_char    ("x_prev"    , x_prev);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr ());
   --rce;  if (yVIKEYS_mode_not (SMOD_MARK   )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(exit mode)----------------------*/
   if (a_minor == G_KEY_ESCAPE) {
      DEBUG_USER   yLOG_note    ("escape means leave");
      yVIKEYS_mode_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(major check)--------------------*/
   DEBUG_USER   yLOG_char    ("x_majors"  , x_majors);
   --rce;  if (strchr (x_majors, a_major) == NULL) {
      DEBUG_USER   yLOG_note    ("major not valid");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mode change)--------------------*/
   if (a_minor == '*') {
      DEBUG_USER   yLOG_note    ("enter visual mode from < to >");
      yVIKEYS_mode_exit ();
      yVIKEYS_mode_enter  (MODE_VISUAL);
      rc = VISU_mark    ();
      if (rc < 0) {
         yVIKEYS_mode_exit ();
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      MARK_return  ('>');
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(common quick)-------------------*/
   --rce;  if (strchr("#_!", a_minor) != NULL) {
      switch (a_minor) {
      case '#' :
         DEBUG_USER   yLOG_note    ("unset mark under cursor");
         rc = MARK_which ();
         if (rc < 0) {
            yVIKEYS_mode_exit ();
            DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         MARK_unset (rc);
         break;
      case '_' :
         DEBUG_USER   yLOG_note    ("toggle mark show and hide");
         if (my.mark_show == 'y')   my.mark_show = '-';
         else                       my.mark_show = 'y';
         break;
      case '!' :
         DEBUG_USER   yLOG_note    ("use mark status bar");
         my.layout_status = G_STATUS_MARK;
         break;
      }
      yVIKEYS_mode_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(common complex)-----------------*/
   --rce;  if (strchr("?@", a_minor) != NULL) {
      switch (a_minor) {
      case '?' :
         DEBUG_USER   yLOG_note    ("show mark info window");
         my.info_win      = G_INFO_MARK;
         return a_major;
         break;
      case '@' :
         DEBUG_USER   yLOG_note    ("enter edit/wander feature");
         rc = MARK_which ();
         if (rc < 0) {
            yVIKEYS_mode_exit ();
            DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         x_prev = rc;
         return a_minor;
         break;
      }
   }
   /*---(check for setting)--------------*/
   --rce;  if (a_major == 'm') {
      DEBUG_USER   yLOG_note    ("handling mark (m)");
      rc = MARK_set (a_minor);
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         yVIKEYS_mode_exit ();
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_prev = a_minor;
      yVIKEYS_mode_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for returning)------------*/
   --rce;  if (a_major == '\'') {
      DEBUG_USER   yLOG_note    ("handling return (')");
      rc = MARK_return (a_minor);
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      if (rc < 0)  {
         yVIKEYS_mode_exit ();
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_prev = a_minor;
      yVIKEYS_mode_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for wander)---------------*/
   --rce;  if (a_major == '@') {
      DEBUG_USER   yLOG_note    ("handling wander (@)");
      if (a_minor == G_KEY_RETURN) {
         MARK_set (x_prev);
         yVIKEYS_mode_exit ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      KEYS_basics (' ', a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   /*---(failure)------------------------*/
   --rce;
   yVIKEYS_mode_exit ();
   DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*        /*-> unit test accessor -----------------[ light  [us.940.221.74]*/ /*-[02.0000.00#.#]-*/ /*-[--.---.---.--]-*/
MARK__unit         (char *a_question, char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        x_list      [LEN_RECD];
   int         x_index     = 0;
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "mark             : question not understood");
   /*---(defense)------------------------*/
   switch (a_mark) {
   case  0   :
      x_index = 0;  break;
   default   :
      x_index = MARK_valid (a_mark);
      if (x_index < 0) {
         snprintf (unit_answer, LEN_UNIT, "mark invalid     : %c not defined", a_mark);
      }
   }
   /*---(questions)----------------------*/
   if      (strcmp (a_question, "mark_list"   )   == 0) {
      MARK_listplus (x_list);
      snprintf (unit_answer, LEN_UNIT, "mark list        : %-.80s", x_list);
   }
   else if (strcmp (a_question, "mark_info"   )   == 0) {
      snprintf (unit_answer, LEN_UNIT, "mark info        : %c %-8.8s %4d %4d %4d", a_mark, s_mark_info [x_index].label, s_mark_info [x_index].tab, s_mark_info [x_index].col, s_mark_info [x_index].row);
   }
   else if (strcmp (a_question, "mark_range"  )   == 0) {
      snprintf (unit_answer, LEN_UNIT, "mark range       : %c to %c", s_mark_head, s_mark_tail);
   }
   else if (strcmp (a_question, "mark_entry"  )   == 0) {
      MARK_entry (a_mark, x_list);
      snprintf (unit_answer, LEN_UNIT, "mark entry       :%s:", x_list);
   }
   else if (strcmp (a_question, "mark_status" )   == 0) {
      MARK_status (x_list);
      snprintf (unit_answer, LEN_UNIT, "mark status      :%s:", x_list);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}

char*        /*-> unit test accessor -----------------[ light  [us.941.231.71]*/ /*-[03.0000.00#.#]-*/ /*-[--.---.---.--]-*/
VISU__unit         (char *a_question, char a_reg)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_col       =    0;
   int         x_row       =    0;
   int         x_count     =    0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   DEBUG_VISU   yLOG_info    ("a_question", a_question);
   DEBUG_VISU   yLOG_char    ("a_reg"     , a_reg);
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "visu             : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "visu_range"   )  == 0) {
      snprintf (unit_answer, LEN_UNIT, "VISU range       : %c, ta=%4d, bc=%4d, br=%4d, ec=%4d, er=%4d", s_visu.mode, s_visu.otab, s_visu.bcol, s_visu.brow, s_visu.ecol, s_visu.erow);
   }
   else if (strcmp (a_question, "visu_curr"    )  == 0) {
      snprintf (unit_answer, LEN_UNIT, "VISU current     : tab=%4d, col=%4d, row=%4d", s_visu.otab, s_visu.ccol, s_visu.crow);
   }
   else if (strcmp (a_question, "visu_count"   )  == 0) {
      x_count = 0;
      for (x_col = s_visu.bcol; x_col <= s_visu.ecol; ++x_col) {
         for (x_row = s_visu.brow; x_row <= s_visu.erow; ++x_row) {
            if (LOC_cell_at_loc (s_visu.otab, x_col, x_row) != NULL) ++x_count;
         }
      }
      snprintf (unit_answer, LEN_UNIT, "VISU count       : %d", x_count);
   }
   else if (strcmp (a_question, "visu_full"    )  == 0) {
      if (s_visu.curr != DONE_DONE) {
         snprintf (unit_answer, LEN_UNIT, "VISU full        : tab=%4d, col=%4d, row=%4d, ptr=%9p", s_visu.otab, s_visu.ccol, s_visu.crow, LOC_cell_at_loc (s_visu.otab, s_visu.ccol, s_visu.crow));
      } else {
         snprintf (unit_answer, LEN_UNIT, "VISU full        : tab=%4d, col=%4d, row=%4d, ptr=%9p", s_visu.otab, s_visu.ccol, s_visu.crow, DONE_DONE);
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return unit_answer;
}



/*===============================[[ end code ]]===============================*/
