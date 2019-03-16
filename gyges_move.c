/*============================----beg-of-source---============================*/

#include   "gyges.h"


struct cACCESSOR my;



char    g_contents      [LEN_RECD] = "";
char    message [LEN_RECD] = "";
char    special = 0;


static  int   s_begc  = 0;
static  int   s_begr  = 0;
static  int   s_endc  = 0;
static  int   s_endr  = 0;



/*====================------------------------------------====================*/
/*===----                    unit testing accessor                     ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;}

char*        /*-> unit test accessor -----------------[ leaf   [gs.930.201.A0]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
MOVE_unit          (char *a_question, int a_num)
{
   /*---(prepare)------------------------*/
   strcpy  (unit_answer, "s_move unit      : question not understood");
   /*---(defenses)-----------------------*/
   if      (a_num < 0) {
      snprintf (unit_answer, LEN_FULL, "s_move error     : tab ref (%d) is negative", a_num);
   }
   else if (a_num >= MAX_TABS) {
      snprintf (unit_answer, LEN_FULL, "s_move error     : tab ref (%d vs %d) is too high", a_num, MAX_TABS);
   }
   /*---(screen location)----------------*/
   else if (strcmp(a_question, "row_stats"     ) == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move row stat  : num=%4d, scr=%4d, beg=%4d, cur=%4d, end=%4d", NROW, my.y_avail, BROW, CROW, EROW);
   }
   else if (strcmp(a_question, "col_stats"     ) == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move col stat  : num=%4d, scr=%4d, beg=%4d, cur=%4d, end=%4d", NCOL, my.x_avail, BCOL, CCOL, ECOL);
   }
   /*---(current position)---------------*/
   else if (strcmp(a_question, "tab_cur"       ) == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move tab cur   : tab=%4d, col=%4d, row=%4d", CTAB, CCOL, CROW);
   }
   /*> else if (strcmp(a_question, "tab_beg"       ) == 0) {                                                                                  <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab beg   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].bcol, s_tabs [a_num].brow);   <* 
    *> }                                                                                                                                      <*/
   /*> else if (strcmp(a_question, "tab_pos"       ) == 0) {                                                                                  <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab pos   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ccol, s_tabs [a_num].crow);   <* 
    *> }                                                                                                                                      <*/
   /*> else if (strcmp(a_question, "tab_max" )       == 0) {                                                                                  <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab max   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ncol, s_tabs [a_num].nrow);   <* 
    *> }                                                                                                                                      <*/
   else if (strcmp(a_question, "edit_con")       == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move edit con  : :%-*.*s:", my.apos, my.apos, g_contents + my.bpos);
   }
   else if (strcmp(a_question, "edit_pos")       == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move edit pos  : n=%3d, a=%3d, b=%3d, c=%3d, e=%3d", my.npos, my.apos, my.bpos, my.cpos, my.epos);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



/*============================----end-of-source---============================*/
