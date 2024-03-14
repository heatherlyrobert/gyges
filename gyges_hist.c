/*============================----beg-of-source---============================*/
#include   "gyges.h"



#define     MAX_HIST    100000
/*> struct cHIST {                                                                    <* 
 *>    char        mode;                                                              <* 
 *>    char        act;                                                               <* 
 *>    int         nkey;                                                              <* 
 *>    char       *addr;                                                              <* 
 *>    char       *before;                                                            <* 
 *>    char       *after;                                                             <* 
 *>    tHIST      *h_prev;                                                            <* 
 *>    tHIST      *h_next;                                                            <* 
 *> };                                                                                <*/
/*> static tHIST  *s_head     = NULL;                                                 <* 
 *> static tHIST  *s_curr     = NULL;                                                 <* 
 *> static tHIST  *s_tail     = NULL;                                                 <* 
 *> static int     s_count    = 0;                                                    <* 
 *> static int     s_index    = -1;                                                   <*/






/*====================------------------------------------====================*/
/*===----                       unit testing                           ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }

char*        /*-> unit test accessor -----------------[ ------ [gs.950.221.M1]*/ /*-[03.0000.00#.#]-*/ /*-[--.---.---.--]-*/
HIST__unit         (char *a_question, int a_ref)
{
   /*---(locals)-----------+-----------+-*/
   int         rc          = -1;
   char        x_name      [LEN_LABEL];
   int         x_tab, x_col, x_row;
   char        b           [LEN_LABEL] = " 0[]";
   char        a           [LEN_LABEL] = " 0[]";
   tHIST      *x_curr      = NULL;
   int         x_index     = -1;
   int         x_fore      =    0;
   int         x_back      =    0;
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "HIST             : question not understood");
   /*---(selection)----------------------*/
   if        (strcmp (a_question, "count"     )    == 0) {
      /*> x_curr = s_head; while (x_curr != NULL) { ++x_fore; x_curr = x_curr->h_next; }                                     <* 
       *> x_curr = s_tail; while (x_curr != NULL) { ++x_back; x_curr = x_curr->h_prev; }                                     <* 
       *> snprintf (unit_answer, LEN_FULL, "HIST count       : %4dn, %4df, %4db, %4di", s_count, x_fore, x_back, s_index);   <*/
   } else if (strcmp (a_question, "current"   )    == 0) {
      /*> if      (s_curr == NULL)    snprintf (unit_answer, LEN_FULL, "HIST curr   (--) : -       ,  -t,   -c,   -r, - -           0[]              0[]");                                                                         <* 
       *> else {                                                                                                                                                                                                                    <* 
       *>    rc = ystr2gyges (s_curr->addr, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_USABLE);                                                                                                                                     <* 
       *>    if (strlen (s_curr->before) < 13)  sprintf  (b, "%2d[%s]"      , strlen (s_curr->before), s_curr->before);                                                                                                             <* 
       *>    else                               sprintf  (b, "%2d[%-12.12s>", strlen (s_curr->before), s_curr->before);                                                                                                             <* 
       *>    if (strlen (s_curr->after ) < 13)  sprintf  (a, "%2d[%s]"      , strlen (s_curr->after ), s_curr->after );                                                                                                             <* 
       *>    else                               sprintf  (a, "%2d[%-12.12s>", strlen (s_curr->after ), s_curr->after );                                                                                                             <* 
       *>    snprintf (unit_answer, LEN_FULL, "HIST curr   (%2d) : %-8.8s, %2dt, %3dc, %3dr, %c %-10.10s %-16.16s %s", s_index, s_curr->addr, x_tab, x_col, x_row, s_curr->mode, HIST__action (s_curr->mode, s_curr->act), b, a);   <* 
       *> }                                                                                                                                                                                                                         <*/
   } else if (strcmp (a_question, "entry"     )    == 0) {
      snprintf (unit_answer, LEN_FULL, "HIST entry  (%2d) : %s", a_ref, yMAP_mundo_detail (a_ref));
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}
