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



/*====================------------------------------------====================*/
/*===----                          initialization                      ----===*/
/*====================------------------------------------====================*/
static void  o___INIT____________o () { return; }

/*> char         /+-> clear all selections ---------------[ shoot  [gz.311.001.02]+/ /+-[00.0000.102.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_init          (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(selection)----------------------+/                                                                                    <* 
 *>    s_visu.live = VISU_NOT;                                                                                                     <* 
 *>    VISU_clear   ();                                                                                                            <* 
 *>    VISU_save    ();                                                                                                            <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> clear the selection ----------------[ ------ [gz.742.001.13]+/ /+-[01.0000.743.A]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_clear          (void)                                                                                                     <* 
 *> {                                                                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(backup)-------------------------+/                                                                                    <* 
 *>    VISU_save   ();                                                                                                             <* 
 *>    /+---(back to original cell)----------+/                                                                                    <* 
 *>    if (s_visu.live == VISU_YES) {                                                                                              <* 
 *>       DEBUG_VISU   yLOG_note    ("return to original cell");                                                                   <* 
 *>       /+> MOVE_prep   ();                                                             <+/                                      <* 
 *>       CTAB = s_visu.otab;                                                                                                      <* 
 *>       CCOL = s_visu.ocol;                                                                                                      <* 
 *>       CROW = s_visu.orow;                                                                                                      <* 
 *>       /+> MOVE_done   ();                                                             <+/                                      <* 
 *>    }                                                                                                                           <* 
 *>    /+---(status)-------------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_note    ("turn range off");                                                                               <* 
 *>    s_visu.live  = VISU_NOT;                                                                                                    <* 
 *>    s_visu.mode  = VISU_NONE;                                                                                                   <* 
 *>    /+---(clear saved locations)----------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_note    ("initialize range coordinates");                                                                 <* 
 *>    s_visu.otab  = s_visu.ocol  = s_visu.orow  = 0;                                                                             <* 
 *>    s_visu.bcol  = s_visu.brow  = 0;                                                                                            <* 
 *>    s_visu.ecol  = s_visu.erow  = 0;                                                                                            <* 
 *>    s_visu.ccol  = s_visu.crow  = 0;                                                                                            <* 
 *>    s_visu.scol  = s_visu.srow  = 0;                                                                                            <* 
 *>    /+---(locations)----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_note    ("initialize cell pointers");                                                                     <* 
 *>    s_visu.home  = s_visu.curr  = NULL;                                                                                         <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/



/*====================------------------------------------====================*/
/*===----                            history                           ----===*/
/*====================------------------------------------====================*/
static void  o___HISTORY_________o () { return; }

/*> char         /+-> save the selection -----------------[ leaf   [gz.742.001.00]+/ /+-[00.0000.023.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_save          (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_senter  (__FUNCTION__);                                                                                   <* 
 *>    /+---(status)-------------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_snote   ("status");                                                                                       <* 
 *>    s_save.live  = VISU_NOT;                                                                                                    <* 
 *>    s_save.mode  = s_visu.mode;                                                                                                 <* 
 *>    /+---(clear saved locations)----------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_snote   ("coords");                                                                                       <* 
 *>    s_save.otab  = s_visu.otab;                                                                                                 <* 
 *>    s_save.ocol  = s_visu.ocol;                                                                                                 <* 
 *>    s_save.orow  = s_visu.orow;                                                                                                 <* 
 *>    s_save.bcol  = s_visu.bcol;                                                                                                 <* 
 *>    s_save.brow  = s_visu.brow;                                                                                                 <* 
 *>    s_save.ecol  = s_visu.ecol;                                                                                                 <* 
 *>    s_save.erow  = s_visu.erow;                                                                                                 <* 
 *>    s_save.ccol  = s_visu.ccol;                                                                                                 <* 
 *>    s_save.crow  = s_visu.crow;                                                                                                 <* 
 *>    s_save.scol  = s_visu.scol;                                                                                                 <* 
 *>    s_save.srow  = s_visu.srow;                                                                                                 <* 
 *>    /+---(locations)----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_snote   ("pointers");                                                                                     <* 
 *>    s_save.home  = NULL;                                                                                                        <* 
 *>    s_save.curr  = NULL;                                                                                                        <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_sexit   (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> restore the selection --------------[ ------ [gz.852.001.02]+/ /+-[00.0000.103.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_restore       (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_senter  (__FUNCTION__);                                                                                   <* 
 *>    /+---(status)-------------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_snote   ("status");                                                                                       <* 
 *>    s_visu.live  = VISU_YES;                                                                                                    <* 
 *>    s_visu.mode  = s_save.mode;                                                                                                 <* 
 *>    /+---(clear saved locations)----------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_snote   ("coords");                                                                                       <* 
 *>    s_visu.otab  = s_save.otab;                                                                                                 <* 
 *>    s_visu.ocol  = s_save.ocol;                                                                                                 <* 
 *>    s_visu.orow  = s_save.orow;                                                                                                 <* 
 *>    s_visu.bcol  = s_save.bcol;                                                                                                 <* 
 *>    s_visu.brow  = s_save.brow;                                                                                                 <* 
 *>    s_visu.ecol  = s_save.ecol;                                                                                                 <* 
 *>    s_visu.erow  = s_save.erow;                                                                                                 <* 
 *>    s_visu.ccol  = s_save.ccol;                                                                                                 <* 
 *>    s_visu.crow  = s_save.crow;                                                                                                 <* 
 *>    s_visu.scol  = s_save.scol;                                                                                                 <* 
 *>    s_visu.srow  = s_save.srow;                                                                                                 <* 
 *>    /+---(locations)----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_snote   ("pointers");                                                                                     <* 
 *>    s_visu.home  = s_save.home;                                                                                                 <* 
 *>    s_visu.curr  = s_save.curr;                                                                                                 <* 
 *>    /+---(go to the right place)----------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_snote   ("pointers");                                                                                     <* 
 *>    /+> MOVE_prep    ();                                                               <+/                                      <* 
 *>    CTAB = s_visu.otab;                                                                                                         <* 
 *>    CCOL = s_visu.ecol;                                                                                                         <* 
 *>    CROW = s_visu.erow;                                                                                                         <* 
 *>    /+> MOVE_done    ();                                                               <+/                                      <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_sexit   (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/



/*====================------------------------------------====================*/
/*===----                            setting                           ----===*/
/*====================------------------------------------====================*/
static void  o___SETTING_________o () { return; }

/*> char         /+-> start the visual selection ---------[ ------ [ge.530.422.12]+/ /+-[00.0000.213.5]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_start         (int a_tab, int a_col, int a_row, char a_mode)                                                              <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         = -10;                                                                                              <* 
 *>    char        rc          = 0;                                                                                                <* 
 *>    /+---(defenses)-----------------------+/                                                                                    <* 
 *>    rc = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);                                                                           <* 
 *>    --rce;  if (rc < 0)                           return rce;                                                                   <* 
 *>    /+---(status)-------------------------+/                                                                                    <* 
 *>    s_visu.live  = VISU_YES;                                                                                                    <* 
 *>    s_visu.mode  = a_mode;                                                                                                      <* 
 *>    /+---(locations)----------------------+/                                                                                    <* 
 *>    s_visu.otab                                                  = a_tab;                                                       <* 
 *>    s_visu.ocol  = s_visu.bcol  = s_visu.ecol  = s_visu.ccol  = s_visu.scol  = a_col;                                           <* 
 *>    s_visu.orow  = s_visu.brow  = s_visu.erow  = s_visu.crow  = s_visu.srow  = a_row;                                           <* 
 *>    /+---(locations)----------------------+/                                                                                    <* 
 *>    s_visu.home  = s_visu.curr  = LOC_cell_at_loc (s_visu.otab, s_visu.bcol, s_visu.brow);                                      <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> adjust the visual selection --------[ ------ [ge.760.324.D2]+/ /+-[01.0000.015.X]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_increase      (int a_tab, int a_col, int a_row)                                                                           <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         = -10;                                                                                              <* 
 *>    char        rc          = 0;                                                                                                <* 
 *>    /+---(defenses)-----------------------+/                                                                                    <* 
 *>    --rce;  if (s_visu.live == VISU_NOT )             return rce;                                                               <* 
 *>    --rce;  if (a_tab    != s_visu.otab)             return rce;                                                                <* 
 *>    rc = LOC_legal (a_tab, a_col, a_row, CELL_FIXED);                                                                           <* 
 *>    --rce;  if (rc < 0)                           return rce;                                                                   <* 
 *>    /+---(process)------------------------+/                                                                                    <* 
 *>    if (s_visu.mode == VISU_CUM) {                                                                                              <* 
 *>       if (a_col > s_visu.ecol)    s_visu.ecol = a_col;                                                                         <* 
 *>       if (a_col < s_visu.bcol)    s_visu.bcol = a_col;                                                                         <* 
 *>       if (a_row > s_visu.erow)    s_visu.erow = a_row;                                                                         <* 
 *>       if (a_row < s_visu.brow)    s_visu.brow = a_row;                                                                         <* 
 *>    } else {                                                                                                                    <* 
 *>       if (a_col >= s_visu.ocol)  { s_visu.bcol = s_visu.ocol; s_visu.ecol = a_col   ; }                                        <* 
 *>       if (a_row >= s_visu.orow)  { s_visu.brow = s_visu.orow; s_visu.erow = a_row   ; }                                        <* 
 *>       if (a_col <  s_visu.ocol)  { s_visu.bcol = a_col   ; s_visu.ecol = s_visu.ocol; }                                        <* 
 *>       if (a_row <  s_visu.orow)  { s_visu.brow = a_row   ; s_visu.erow = s_visu.orow; }                                        <* 
 *>    }                                                                                                                           <* 
 *>    /+---(current)------------------------+/                                                                                    <* 
 *>    s_visu.ccol  = a_col;                                                                                                       <* 
 *>    s_visu.crow  = a_row;                                                                                                       <* 
 *>    /+---(locations)----------------------+/                                                                                    <* 
 *>    s_visu.home  = s_visu.curr  = LOC_cell_at_loc (s_visu.otab, s_visu.bcol, s_visu.brow);                                      <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> cursor update to visual selection --[ ------ [gc.412.312.11]+/ /+-[01.0000.204.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_update        (int a_tab, int a_col, int a_row)                                                                           <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+------+----+-+/                                                                                    <* 
 *>    char        rc          =     0;                                                                                            <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_senter  (__FUNCTION__);                                                                                   <* 
 *>    DEBUG_VISU   yLOG_char    ("s_visu.live", s_visu.live);                                                                     <* 
 *>    if (s_visu.live == VISU_NOT) {                                                                                              <* 
 *>       DEBUG_VISU   yLOG_sexitr  (__FUNCTION__, -1);                                                                            <* 
 *>       return -1;                                                                                                               <* 
 *>    }                                                                                                                           <* 
 *>    rc = VISU_increase (CTAB, CCOL, CROW);                                                                                      <* 
 *>    DEBUG_VISU   yLOG_sint    (rc);                                                                                             <* 
 *>    DEBUG_VISU   yLOG_sexit   (__FUNCTION__);                                                                                   <* 
 *>    return rc;                                                                                                                  <* 
 *> }                                                                                                                              <*/

/*> char         /+-> apply a specific selection ---------[ ------ [ge.E54.125.43]+/ /+-[01.0000.213.T]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_set           (                                                                                                           <* 
 *>       /+----------+-----------+-----------------------------------------------+/                                               <* 
 *>       int         a_tab,      /+ tab of selection                             +/                                               <* 
 *>       int         a_bcol,     /+ beginning col of selection                   +/                                               <* 
 *>       int         a_brow,     /+ beginning row of selection                   +/                                               <* 
 *>       int         a_ecol,     /+ ending col of selection                      +/                                               <* 
 *>       int         a_erow)     /+ ending col of selection                      +/                                               <* 
 *> {  /+---(design notes)--------------------------------------------------------+/                                               <* 
 *>    /+ if the two ends of the range are legal, this function will change the   +/                                               <* 
 *>    /+ current selection to the boundaries passed as arguments.                +/                                               <* 
 *>    /+---(locals)-----------+-----------+-+//+---------------------------------+/                                               <* 
 *>    char        rc          = 0;                                                                                                <* 
 *>    char        rce         = -10;                                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    DEBUG_VISU   yLOG_value   ("a_tab"     , a_tab );                                                                           <* 
 *>    DEBUG_VISU   yLOG_value   ("a_bcol"    , a_bcol);                                                                           <* 
 *>    DEBUG_VISU   yLOG_value   ("a_brow"    , a_brow);                                                                           <* 
 *>    DEBUG_VISU   yLOG_value   ("a_ecol"    , a_ecol);                                                                           <* 
 *>    DEBUG_VISU   yLOG_value   ("a_erow"    , a_erow);                                                                           <* 
 *>    /+---(prepare)------------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_note    ("clear existing ranges");                                                                        <* 
 *>    s_visu.live  = VISU_NOT;                                                                                                    <* 
 *>    VISU_clear ();                                                                                                              <* 
 *>    /+---(defense: beginning legal)-------+/                                                                                    <* 
 *>    rc  = LOC_legal (a_tab, a_bcol, a_brow, CELL_FIXED);                                                                        <* 
 *>    DEBUG_VISU   yLOG_value   ("rc"        , rc);                                                                               <* 
 *>    --rce;  if (rc < 0) {                                                                                                       <* 
 *>       DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(defense: ending legal)----------+/                                                                                    <* 
 *>    rc  = LOC_legal (a_tab, a_ecol, a_erow, CELL_FIXED);                                                                        <* 
 *>    DEBUG_VISU   yLOG_value   ("rc"        , rc);                                                                               <* 
 *>    --rce;  if (rc < 0) {                                                                                                       <* 
 *>       DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(defense: ordering)--------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_note    ("check range limits");                                                                           <* 
 *>    --rce;  if (a_bcol > a_ecol) {                                                                                              <* 
 *>       DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    --rce;  if (a_brow > a_erow) {                                                                                              <* 
 *>       DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(set range)----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_note    ("set range live");                                                                               <* 
 *>    s_visu.live  = VISU_YES;                                                                                                    <* 
 *>    s_visu.mode  = VISU_FROM;                                                                                                   <* 
 *>    /+---(locations)----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_note    ("set range coordinates");                                                                        <* 
 *>    s_visu.otab  = a_tab;                                                                                                       <* 
 *>    s_visu.ocol  = s_visu.bcol  = s_visu.ccol  = a_bcol;                                                                        <* 
 *>    s_visu.ecol  = a_ecol;                                                                                                      <* 
 *>    s_visu.orow  = s_visu.brow  = s_visu.crow  = a_brow;                                                                        <* 
 *>    s_visu.erow  = a_erow;                                                                                                      <* 
 *>    /+---(locations)----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_note    ("set range cell pointers");                                                                      <* 
 *>    s_visu.home  = s_visu.curr  = LOC_cell_at_loc (s_visu.otab, s_visu.bcol, s_visu.brow);                                      <* 
 *>    s_visu.curr  = s_visu.home;                                                                                                 <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> apply a specific selection ---------[ ------ [ge.C64.087.63]+/ /+-[01.0000.013.5]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_mark            (void)                                                                                                    <* 
 *> {                                                                                                                              <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    char        rc          =    0;                                                                                             <* 
 *>    short       x_btab      =   -1;                                                                                             <* 
 *>    short       x_bcol      =   -1;                                                                                             <* 
 *>    short       x_brow      =   -1;                                                                                             <* 
 *>    short       x_etab      =   -1;                                                                                             <* 
 *>    short       x_ecol      =   -1;                                                                                             <* 
 *>    short       x_erow      =   -1;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(prepare)------------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_note    ("clear selection");                                                                              <* 
 *>    s_visu.live  = VISU_NOT;                                                                                                    <* 
 *>    VISU_clear ();                                                                                                              <* 
 *>    /+---(get marks)----------------------+/                                                                                    <* 
 *>    /+> rc = MARK_address ('(', &x_btab, &x_bcol, &x_brow);                            <+/                                      <* 
 *>    DEBUG_VISU   yLOG_value   ("rc"        , rc);                                                                               <* 
 *>    --rce;  if (rc <  0) {                                                                                                      <* 
 *>       DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+> rc = MARK_address (')', &x_etab, &x_ecol, &x_erow);                            <+/                                      <* 
 *>    DEBUG_VISU   yLOG_value   ("rc"        , rc);                                                                               <* 
 *>    --rce;  if (rc <  0) {                                                                                                      <* 
 *>       DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(check range)--------------------+/                                                                                    <* 
 *>    --rce;  if (x_btab != x_etab) {                                                                                             <* 
 *>       DEBUG_VISU   yLOG_note    ("btab does not match etab");                                                                  <* 
 *>       DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    --rce;  if (x_bcol >  x_ecol) {                                                                                             <* 
 *>       DEBUG_VISU   yLOG_note    ("bcol is greater than ecol");                                                                 <* 
 *>       DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    --rce;  if (x_brow >  x_erow) {                                                                                             <* 
 *>       DEBUG_VISU   yLOG_note    ("brow is greater than erow");                                                                 <* 
 *>       DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(set selection)------------------+/                                                                                    <* 
 *>    rc = VISU_set (x_btab, x_bcol, x_brow, x_ecol, x_erow);                                                                     <* 
 *>    DEBUG_VISU   yLOG_value   ("rc"        , rc);                                                                               <* 
 *>    --rce;  if (rc <  0) {                                                                                                      <* 
 *>       DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> swap the corners -------------------[ ------ [gz.530.001.22]+/ /+-[01.0000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_reverse       (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(change root)--------------------+/                                                                                    <* 
 *>    MOVE_prep ();                                                                                                               <* 
 *>    if (s_visu.ccol == s_visu.ecol && s_visu.crow == s_visu.erow) {                                                             <* 
 *>       s_visu.ocol = s_visu.ecol;                                                                                               <* 
 *>       s_visu.orow = s_visu.erow;                                                                                               <* 
 *>       CCOL = s_visu.ccol = s_visu.bcol;                                                                                        <* 
 *>       CROW = s_visu.crow = s_visu.brow;                                                                                        <* 
 *>    } else {                                                                                                                    <* 
 *>       s_visu.ocol = s_visu.bcol;                                                                                               <* 
 *>       s_visu.orow = s_visu.brow;                                                                                               <* 
 *>       CCOL = s_visu.ccol = s_visu.ecol;                                                                                        <* 
 *>       CROW = s_visu.crow = s_visu.erow;                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    /+> MOVE_done ();                                                                  <+/                                      <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/


/*====================------------------------------------====================*/
/*===----                            display                           ----===*/
/*====================------------------------------------====================*/
static void  o___DISPLAY_________o () { return; }

/*> char         /+-> indicate whether cell is the root --[ leaf   [gc.220.305.40]+/ /+-[00.0000.106.L]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_root          (int a_tab, int a_col, int a_row)                                                                           <* 
 *> {                                                                                                                              <* 
 *>    if (s_visu.live == VISU_NOT)    return 0;                                                                                   <* 
 *>    if (a_tab != s_visu.otab)       return 0;                                                                                   <* 
 *>    if (a_col != s_visu.ocol)       return 0;                                                                                   <* 
 *>    if (a_row != s_visu.orow)       return 0;                                                                                   <* 
 *>    return 1;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> indicate whether cell is selected --[ leaf   [gc.320.307.60]+/ /+-[00.0000.205.#]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_selected      (int a_tab, int a_col, int a_row)                                                                           <* 
 *> {                                                                                                                              <* 
 *>    if (s_visu.live == VISU_NOT)    return 0;                                                                                   <* 
 *>    if (a_tab != s_visu.otab)      return 0;                                                                                    <* 
 *>    if (a_col <  s_visu.bcol)      return 0;                                                                                    <* 
 *>    if (a_col >  s_visu.ecol)      return 0;                                                                                    <* 
 *>    if (a_row <  s_visu.brow)      return 0;                                                                                    <* 
 *>    if (a_row >  s_visu.erow)      return 0;                                                                                    <* 
 *>    return 1;                                                                                                                   <* 
 *> }                                                                                                                              <*/



/*====================------------------------------------====================*/
/*===----                           simplifiers                        ----===*/
/*====================------------------------------------====================*/
static void  o___SIMPLIFIERS_____o () { return; }

/*> char         /+-> selection active or not ------------[ ------ [gc.B50.00#.D7]+/ /+-[01.0000.104.#]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_islive        (void) { if (s_visu.live == VISU_YES)  return 1; return 0; }                                                <*/

/*> char         /+-> start at current position ----------[ ------ [gc.B40.00#.C6]+/ /+-[01.0000.00#.3]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_from          (void) { return VISU_start  (CTAB, CCOL, CROW, VISU_CUM); }                                                 <*/

/*> char         /+-> convert to whole-column selection --[ ------ [gc.A40.009.C4]+/ /+-[01.0000.00#.2]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_col           (void) { s_visu.brow = 0; s_visu.erow = LOC_row_max (s_visu.otab) - 1; return 0; }                          <*/

/*> char         /+-> convert to whole-row selection -----[ ------ [gc.A40.008.C2]+/ /+-[01.0000.00#.1]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_row           (void) { s_visu.bcol = 0; s_visu.ecol = LOC_col_max (s_visu.otab) - 1; return 0; }                          <*/



/*====================------------------------------------====================*/
/*===----                            retrieval                         ----===*/
/*====================------------------------------------====================*/
static void  o___RETRIEVAL_______o () { return; }

/*   these functions only operate on the current selection ('-') as they
 *   support the CELL functions of formatting, column width setting, alignment,
 *   and merging amoung others.  these are all interactive uses.
 */

/*> char         /+-> return selection range borders -----[ leaf   [gc.730.107.C0]+/ /+-[01.0000.204.4]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_range          (                                                                                                          <* 
 *>       /+----------+-----------+-----------------------------------------------+/                                               <* 
 *>       int        *a_tab,      /+ return variable for tab                      +/                                               <* 
 *>       int        *a_bcol,     /+ return variable for beginning col            +/                                               <* 
 *>       int        *a_brow,     /+ return variable for beginning row            +/                                               <* 
 *>       int        *a_ecol,     /+ return variable for ending col               +/                                               <* 
 *>       int        *a_erow)     /+ return variable for ending row               +/                                               <* 
 *> {  /+---(design notes)--------------------------------------------------------+/                                               <* 
 *>    /+ returns the borders of the entire current selection.  if nothing is     +/                                               <* 
 *>    /+ selected, it returns the current cell as a selection.                   +/                                               <* 
 *>    /+---(no selection)-------------------+//+---------------------------------+/                                               <* 
 *>    if (s_visu.live == 0) {                                                                                                     <* 
 *>       if (a_tab  != NULL) *a_tab  = CTAB;                                                                                      <* 
 *>       if (a_bcol != NULL) *a_bcol = CCOL;                                                                                      <* 
 *>       if (a_brow != NULL) *a_brow = CROW;                                                                                      <* 
 *>       if (a_ecol != NULL) *a_ecol = CCOL;                                                                                      <* 
 *>       if (a_erow != NULL) *a_erow = CROW;                                                                                      <* 
 *>    }                                                                                                                           <* 
 *>    /+---(live selection)-----------------+/                                                                                    <* 
 *>    else {                                                                                                                      <* 
 *>       if (a_tab  != NULL) *a_tab  = s_visu.otab;                                                                               <* 
 *>       if (a_bcol != NULL) *a_bcol = s_visu.bcol;                                                                               <* 
 *>       if (a_brow != NULL) *a_brow = s_visu.brow;                                                                               <* 
 *>       if (a_ecol != NULL) *a_ecol = s_visu.ecol;                                                                               <* 
 *>       if (a_erow != NULL) *a_erow = s_visu.erow;                                                                               <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> tCELL*       /+-> return top-left cell in selection --[ ------ [gp.630.108.72]+/ /+-[01.0000.804.4]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_first         (                                                                                                           <* 
 *>       /+----------+-----------+-----------------------------------------------+/                                               <* 
 *>       int        *a_tab,      /+ return holder for tab of first cell          +/                                               <* 
 *>       int        *a_col,      /+ return holder for col of first cell          +/                                               <* 
 *>       int        *a_row)      /+ return holder for row of first cell          +/                                               <* 
 *> {  /+---(design notes)--------------------------------------------------------+/                                               <* 
 *>    /+ if nothing selected, return current cell so this function is always     +/                                               <* 
 *>    /+ valid.  if selection active, return top-left cell.  this function       +/                                               <* 
 *>    /+ returns four values -- pointer, tab, col, and row of top-left cell.     +/                                               <* 
 *>    /+---(no selection active)------------+//+---------------------------------+/                                               <* 
 *>    if (s_visu.live == 0) {                                                                                                     <* 
 *>       if (a_tab != NULL)  *a_tab = CTAB;                                                                                       <* 
 *>       if (a_col != NULL)  *a_col = CCOL;                                                                                       <* 
 *>       if (a_row != NULL)  *a_row = CROW;                                                                                       <* 
 *>       return LOC_cell_at_curr ();                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(set to beginning)---------------+/                                                                                    <* 
 *>    s_visu.ccol  = s_visu.bcol;                                                                                                 <* 
 *>    s_visu.crow  = s_visu.brow;                                                                                                 <* 
 *>    s_visu.curr  = LOC_cell_at_loc (s_visu.otab, s_visu.ccol, s_visu.crow);                                                     <* 
 *>    /+---(set the return)-----------------+/                                                                                    <* 
 *>    if (a_tab != NULL)  *a_tab = s_visu.otab;                                                                                   <* 
 *>    if (a_col != NULL)  *a_col = s_visu.ccol;                                                                                   <* 
 *>    if (a_row != NULL)  *a_row = s_visu.crow;                                                                                   <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return s_visu.curr;                                                                                                         <* 
 *> }                                                                                                                              <*/

/*> tCELL*       /+-> return next cell in selection ------[ ------ [gp.940.10#.C1]+/ /+-[01.0000.804.A]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_next          (                                                                                                           <* 
 *>       /+----------+-----------+-----------------------------------------------+/                                               <* 
 *>       int        *a_tab,      /+ return holder for tab of next cell           +/                                               <* 
 *>       int        *a_col,      /+ return holder for col of next cell           +/                                               <* 
 *>       int        *a_row)      /+ return holder for row of next cell           +/                                               <* 
 *> {  /+---(design notes)--------------------------------------------------------+/                                               <* 
 *>    /+ if nothing selected, return a sentinel as there is no next cell.  if    +/                                               <* 
 *>    /+ selection active, return next cell in selection by moving right one     +/                                               <* 
 *>    /+ column and when at right of selection return leftost cell in next row   +/                                               <* 
 *>    /+ down.  at end, return a sentinel to indicate end of processing.         +/                                               <* 
 *>    /+ returns four values -- pointer, tab, col, and row of next cell.         +/                                               <* 
 *>    /+---(no selection active)------------+//+---------------------------------+/                                               <* 
 *>    if (s_visu.live == 0 || s_visu.ccol == -1) {                                                                                <* 
 *>       if (a_tab != NULL) *a_tab = -1;                                                                                          <* 
 *>       if (a_col != NULL) *a_col = -1;                                                                                          <* 
 *>       if (a_row != NULL) *a_row = -1;                                                                                          <* 
 *>       return  DONE_DONE;                                                                                                       <* 
 *>    }                                                                                                                           <* 
 *>    /+---(update position)----------------+/                                                                                    <* 
 *>    ++s_visu.ccol;                                                                                                              <* 
 *>    if (s_visu.ccol > s_visu.ecol) {                                                                                            <* 
 *>       s_visu.ccol = s_visu.bcol;                                                                                               <* 
 *>       ++s_visu.crow;                                                                                                           <* 
 *>    }                                                                                                                           <* 
 *>    /+---(check for end)------------------+/                                                                                    <* 
 *>    if (s_visu.crow > s_visu.erow) {                                                                                            <* 
 *>       s_visu.ccol     = -1;                                                                                                    <* 
 *>       s_visu.crow     = -1;                                                                                                    <* 
 *>       s_visu.curr     = DONE_DONE;                                                                                             <* 
 *>       if (a_tab != NULL) *a_tab = -1;                                                                                          <* 
 *>       if (a_col != NULL) *a_col = -1;                                                                                          <* 
 *>       if (a_row != NULL) *a_row = -1;                                                                                          <* 
 *>       return  DONE_DONE;                                                                                                       <* 
 *>    }                                                                                                                           <* 
 *>    /+---(set the return)-----------------+/                                                                                    <* 
 *>    if (a_tab != NULL) *a_tab = s_visu.otab;                                                                                    <* 
 *>    if (a_col != NULL) *a_col = s_visu.ccol;                                                                                    <* 
 *>    if (a_row != NULL) *a_row = s_visu.crow;                                                                                    <* 
 *>    s_visu.curr  = LOC_cell_at_loc (s_visu.otab, s_visu.ccol, s_visu.crow);                                                     <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return s_visu.curr;                                                                                                         <* 
 *> }                                                                                                                              <*/

/*> tCELL*       /+-> return first cell in selection -----[ ------ [gp.630.108.72]+/ /+-[01.0000.804.4]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_firstreal       (int *a_tab, int *a_col, int *a_row)                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(unselected)---------------------+/                                                                                    <* 
 *>    if (s_visu.live == 0) {                                                                                                     <* 
 *>       s_visu.otab  = CTAB;                                                                                                     <* 
 *>       s_visu.bcol  = s_visu.ocol  = 0;                                                                                         <* 
 *>       s_visu.ccol  = 0;                                                                                                        <* 
 *>       s_visu.ecol  = NCOL - 1;                                                                                                 <* 
 *>       s_visu.brow  = s_visu.orow  = 0;                                                                                         <* 
 *>       s_visu.crow  = 0;                                                                                                        <* 
 *>       s_visu.erow  = NROW - 1;                                                                                                 <* 
 *>    }                                                                                                                           <* 
 *>    /+---(or, selected)-------------------+/                                                                                    <* 
 *>    else {                                                                                                                      <* 
 *>       s_visu.ccol  = s_visu.bcol;                                                                                              <* 
 *>       s_visu.crow  = s_visu.brow;                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(get very first)-----------------+/                                                                                    <* 
 *>    s_visu.curr  = LOC_cell_at_loc (s_visu.otab, s_visu.ccol, s_visu.crow);                                                     <* 
 *>    /+---(top-left good)------------------+/                                                                                    <* 
 *>    if (s_visu.curr != NULL) {                                                                                                  <* 
 *>       if (a_tab != NULL)  *a_tab = s_visu.otab;                                                                                <* 
 *>       if (a_col != NULL)  *a_col = s_visu.ccol;                                                                                <* 
 *>       if (a_row != NULL)  *a_row = s_visu.crow;                                                                                <* 
 *>    }                                                                                                                           <* 
 *>    /+---(top-left null)------------------+/                                                                                    <* 
 *>    else s_visu.curr = VISU_nextreal (a_tab, a_col, a_row);                                                                     <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return s_visu.curr;                                                                                                         <* 
 *> }                                                                                                                              <*/

/*> tCELL*       /+-> return first cell in selection -----[ ------ [gp.630.108.72]+/ /+-[01.0000.804.4]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_nextreal        (int *a_tab, int *a_col, int *a_row)                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(defense)------------------------+/                                                                                    <* 
 *>    if (s_visu.live == 0)  return NULL;                                                                                         <* 
 *>    s_visu.curr = NULL;                                                                                                         <* 
 *>    while (s_visu.curr == NULL && s_visu.crow <= s_visu.erow) {                                                                 <* 
 *>       /+---(update position)-------------+/                                                                                    <* 
 *>       ++s_visu.ccol;                                                                                                           <* 
 *>       if (s_visu.ccol > s_visu.ecol) {                                                                                         <* 
 *>          s_visu.ccol = s_visu.bcol;                                                                                            <* 
 *>          ++s_visu.crow;                                                                                                        <* 
 *>       }                                                                                                                        <* 
 *>       /+---(get cell)--------------------+/                                                                                    <* 
 *>       s_visu.curr  = LOC_cell_at_loc (s_visu.otab, s_visu.ccol, s_visu.crow);                                                  <* 
 *>    }                                                                                                                           <* 
 *>    /+---(check for end)------------------+/                                                                                    <* 
 *>    if (s_visu.crow > s_visu.erow) {                                                                                            <* 
 *>       s_visu.ccol     = -1;                                                                                                    <* 
 *>       s_visu.crow     = -1;                                                                                                    <* 
 *>       s_visu.curr     = DONE_DONE;                                                                                             <* 
 *>       if (a_tab != NULL) *a_tab = -1;                                                                                          <* 
 *>       if (a_col != NULL) *a_col = -1;                                                                                          <* 
 *>       if (a_row != NULL) *a_row = -1;                                                                                          <* 
 *>       return  DONE_DONE;                                                                                                       <* 
 *>    }                                                                                                                           <* 
 *>    /+---(set the return)-----------------+/                                                                                    <* 
 *>    if (a_tab != NULL)  *a_tab = s_visu.otab;                                                                                   <* 
 *>    if (a_col != NULL)  *a_col = s_visu.ccol;                                                                                   <* 
 *>    if (a_row != NULL)  *a_row = s_visu.crow;                                                                                   <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return s_visu.curr;                                                                                                         <* 
 *> }                                                                                                                              <*/


/*====================------------------------------------====================*/
/*===----                       keyboard input                         ----===*/
/*====================------------------------------------====================*/
static void  o___KEYS____________o () { return; }

/*> char         /+-> tbd --------------------------------[ ------ [gz.640.181.01]+/ /+-[02.0000.104.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_status        (char *a_msg)                                                                                               <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        x_orig      [LEN_LABEL] = "";                                                                                   <* 
 *>    char        x_beg       [LEN_LABEL] = "";                                                                                   <* 
 *>    char        x_end       [LEN_LABEL] = "";                                                                                   <* 
 *>    char        x_curr      [LEN_LABEL] = "";                                                                                   <* 
 *>    char        x_sorig     [LEN_LABEL] = "";                                                                                   <* 
 *>    char        x_sbeg      [LEN_LABEL] = "";                                                                                   <* 
 *>    char        x_send      [LEN_LABEL] = "";                                                                                   <* 
 *>    char        x_scurr     [LEN_LABEL] = "";                                                                                   <* 
 *>    /+---(make labels)--------------------+/                                                                                    <* 
 *>    LOC_ref  (s_visu.otab, s_visu.ocol, s_visu.orow, 0, x_orig);                                                                <* 
 *>    LOC_ref  (s_visu.otab, s_visu.bcol, s_visu.brow, 0, x_beg );                                                                <* 
 *>    LOC_ref  (s_visu.otab, s_visu.ecol, s_visu.erow, 0, x_end );                                                                <* 
 *>    LOC_ref  (s_visu.otab, s_visu.ccol, s_visu.crow, 0, x_curr);                                                                <* 
 *>    LOC_ref  (s_save.otab, s_save.ocol, s_save.orow, 0, x_sorig);                                                               <* 
 *>    LOC_ref  (s_save.otab, s_save.bcol, s_save.brow, 0, x_sbeg );                                                               <* 
 *>    LOC_ref  (s_save.otab, s_save.ecol, s_save.erow, 0, x_send );                                                               <* 
 *>    LOC_ref  (s_save.otab, s_save.ccol, s_save.crow, 0, x_scurr);                                                               <* 
 *>    /+---(make string)--------------------+/                                                                                    <* 
 *>    snprintf (a_msg, 500, "[ %d   curr o=%-6.6s b=%-6.6s e=%-6.6s c=%-6.6s   save o=%-6.6s b=%-6.6s e=%-6.6s c=%-6.6s]",        <* 
 *>          s_visu.live,                                                                                                          <* 
 *>          x_orig , x_beg  , x_end  , x_curr ,                                                                                   <* 
 *>          x_sorig, x_sbeg , x_send , x_scurr);                                                                                  <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> process keys for marks -------------[ leaf   [ge.K85.22#.CB]+/ /+-[02.0000.102.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> VISU_submode       (char a_major, char a_minor)                                                                                <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         = -10;                                                                                              <* 
 *>    char        rc          =   0;                                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    DEBUG_VISU   yLOG_value   ("a_major"   , a_major);                                                                          <* 
 *>    DEBUG_VISU   yLOG_value   ("a_minor"   , a_minor);                                                                          <* 
 *>    /+---(defenses)-----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_char    ("mode"      , yVIKEYS_mode_curr ());                                                             <* 
 *>    --rce;  if (yVIKEYS_mode_not (MODE_VISUAL )) {                                                                              <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    if (a_minor == G_KEY_ESCAPE)  {                                                                                             <* 
 *>       VISU_clear  ();                                                                                                          <* 
 *>       yVIKEYS_mode_exit ();                                                                                                    <* 
 *>       DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                                <* 
 *>       return  0;                                                                                                               <* 
 *>    }                                                                                                                           <* 
 *>    /+---(check for simple keys-----------+/                                                                                    <* 
 *>    --rce;  if (a_major == ' ') {                                                                                               <* 
 *>       /+---(multikey prefixes)-----------+/                                                                                    <* 
 *>       if (strchr ("gze"   , a_minor) != 0) {                                                                                   <* 
 *>          DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return a_minor;                                                                                                       <* 
 *>       }                                                                                                                        <* 
 *>       /+---(modes)-----------------------+/                                                                                    <* 
 *>       switch (a_minor) {                                                                                                       <* 
 *>       case ':'      : /+--- enter command mode ------+/                                                                        <* 
 *>          DEBUG_VISU   yLOG_note    ("chose command mode");                                                                     <* 
 *>          yVIKEYS_mode_enter  (MODE_COMMAND);                                                                                   <* 
 *>          yVIKEYS_cmds_start ();                                                                                                <* 
 *>          DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return 0;                                                                                                             <* 
 *>          break;                                                                                                                <* 
 *>       case '/'      : /+--- enter search mode -------+/                                                                        <* 
 *>          DEBUG_VISU   yLOG_note    ("chose search mode");                                                                      <* 
 *>          yVIKEYS_mode_enter  (MODE_SEARCH);                                                                                    <* 
 *>          /+> SEARCH_start ();                                                         <+/                                      <* 
 *>          DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return 0;                                                                                                             <* 
 *>          break;                                                                                                                <* 
 *>       }                                                                                                                        <* 
 *>       /+---(submodes)--------------------+/                                                                                    <* 
 *>       switch (a_minor) {                                                                                                       <* 
 *>       case '"'      :                                                                                                          <* 
 *>          DEBUG_VISU   yLOG_note    ("chose register sub-mode");                                                                <* 
 *>          yVIKEYS_mode_enter  (SMOD_REGISTER);                                                                                  <* 
 *>          DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return a_minor;  /+ make sure double quote goes in prev char +/                                                       <* 
 *>          break;                                                                                                                <* 
 *>       case 'F'      : case 'f'      :                                                                                          <* 
 *>          DEBUG_VISU   yLOG_note    ("chose format sub-mode");                                                                  <* 
 *>          yVIKEYS_mode_enter  (SMOD_FORMAT);                                                                                    <* 
 *>          DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return a_minor;                                                                                                       <* 
 *>          break;                                                                                                                <* 
 *>       }                                                                                                                        <* 
 *>       /+---(actions)---------------------+/                                                                                    <* 
 *>       rc = KEYS_regbasic (a_major, a_minor);                                                                                   <* 
 *>       if (rc == 0) {                                                                                                           <* 
 *>          yVIKEYS_mode_exit ();                                                                                                 <* 
 *>          DEBUG_USER   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>       if (a_minor == 'o') {                                                                                                    <* 
 *>          VISU_reverse      ();                                                                                                 <* 
 *>       }                                                                                                                        <* 
 *>       /+---(normal)----------------------+/                                                                                    <* 
 *>       rc = KEYS_basics (a_major, a_minor);                                                                                     <* 
 *>       if (rc == 0) {                                                                                                           <* 
 *>          DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    /+---(goto family)--------------------+/                                                                                    <* 
 *>    if (a_major == 'g') {                                                                                                       <* 
 *>       rc = KEYS_gz_family  (a_major, a_minor);                                                                                 <* 
 *>       DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                                <* 
 *>       return 0;                                                                                                                <* 
 *>    }                                                                                                                           <* 
 *>    /+---(scroll family)------------------+/                                                                                    <* 
 *>    if (a_major == 'z') {                                                                                                       <* 
 *>       rc = KEYS_gz_family  (a_major, a_minor);                                                                                 <* 
 *>       DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                                <* 
 *>       return 0;                                                                                                                <* 
 *>    }                                                                                                                           <* 
 *>    /+---(end family)---------------------+/                                                                                    <* 
 *>    if (a_major == 'e') {                                                                                                       <* 
 *>       rc = KEYS_e_family   (a_major, a_minor);                                                                                 <* 
 *>       DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                                <* 
 *>       return 0;                                                                                                                <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_VISU   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/




/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }


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
