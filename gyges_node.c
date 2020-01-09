/*============================----beg-of-source---============================*/
#include   "gyges.h"



static tTAB  *s_tab    = NULL;
static char   s_type   = '-';
static tNODE *s_curr   = NULL;



#define   IF_COL     if (a_type == 'c') 
#define   ELSE_ROW   else 



/*====================------------------------------------====================*/
/*===----                       memory allocation                      ----===*/
/*====================------------------------------------====================*/
static void  o___MEMORY__________o () { return; }

char
NODE_new                (tNODE **a_new, tTAB *a_tab, char a_type, ushort a_ref)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tNODE      *x_new       = NULL;
   char        x_tries     =    0;
   tNODE      *x_prev      = NULL;
   tNODE      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_senter  (__FUNCTION__);
   DEBUG_LOCS   yLOG_spoint  (a_new);
   DEBUG_LOCS   yLOG_spoint  (a_tab);
   DEBUG_LOCS   yLOG_schar   (a_type);
   DEBUG_LOCS   yLOG_sint    (a_ref);
   /*---(prepare)------------------------*/
   if (a_new != NULL)  *a_new = NULL;
   /*---(defense)------------------------*/
   --rce;  if (a_tab == NULL) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(allocate)-----------------------*/
   while (x_new == NULL && x_tries < 10)  {
      ++x_tries;
      x_new = (tNODE *) malloc (sizeof (tNODE));
   }
   DEBUG_LOCS   yLOG_sint    (x_tries);
   DEBUG_LOCS   yLOG_spoint  (x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_LOCS   yLOG_snote   ("FAILED");
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(populate)-----------------------*/
   DEBUG_LOCS   yLOG_snote   ("populate");
   x_new->type      = a_type;
   x_new->tab       = a_tab->tab;
   x_new->ref       = a_ref;
   IF_COL    x_new->size      = DEF_WIDTH;
   ELSE_ROW  x_new->size      = DEF_HEIGHT;
   x_new->count     = 0;
   x_new->N_prev    = NULL;
   x_new->N_next    = NULL;
   x_new->n_head    = NULL;
   x_new->n_tail    = NULL;
   /*---(find position)------------------*/
   IF_COL    x_curr = a_tab->C_head;
   ELSE_ROW  x_curr = a_tab->R_head;
   --rce;  while (x_curr != NULL) {
      if (x_curr->ref >  x_new->ref)   break;
      x_prev = x_curr;
      x_curr = x_curr->N_next;
   }
   DEBUG_LOCS   yLOG_spoint  (x_prev);
   DEBUG_LOCS   yLOG_spoint  (x_curr);
   /*---(add to tab)---------------------*/
   if (x_prev == NULL && x_curr == NULL) {
      DEBUG_LOCS   yLOG_snote   ("first col/row");
      IF_COL    { a_tab->C_head  = x_new;  a_tab->C_tail  = x_new; }
      ELSE_ROW  { a_tab->R_head  = x_new;  a_tab->R_tail  = x_new; }
   } else if (x_prev == NULL && x_curr != NULL) {
      DEBUG_LOCS   yLOG_snote   ("prepend to front");
      IF_COL    { x_new->N_next = a_tab->C_head; a_tab->C_head->N_prev = x_new; a_tab->C_head = x_new; }
      ELSE_ROW  { x_new->N_next = a_tab->R_head; a_tab->R_head->N_prev = x_new; a_tab->R_head = x_new; }
   } else if (x_prev != NULL && x_curr == NULL) {
      DEBUG_LOCS   yLOG_snote   ("append to back");
      IF_COL    { x_new->N_prev = a_tab->C_tail; a_tab->C_tail->N_next = x_new; a_tab->C_tail = x_new; }
      ELSE_ROW  { x_new->N_prev = a_tab->R_tail; a_tab->R_tail->N_next = x_new; a_tab->R_tail = x_new; }
   } else {
      DEBUG_LOCS   yLOG_snote   ("insert in middle");
      x_new->N_next  = x_prev->N_next;
      x_prev->N_next = x_new;
      x_new->N_prev  = x_curr->N_prev;
      x_curr->N_prev = x_new;
   }
   /*---(update counts)------------------*/
   IF_COL   ++(a_tab->C_count);
   ELSE_ROW ++(a_tab->R_count);
   DEBUG_LOCS   yLOG_sint    (a_tab->C_count);
   DEBUG_LOCS   yLOG_sint    (a_tab->R_count);
   /*---(return)-------------------------*/
   if (a_new != NULL)  *a_new = x_new;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
NODE_free               (tNODE **a_old)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tTAB       *x_tab       = NULL;
   tNODE      *x_old       = NULL;
   char        a_type      =  '-';
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_senter  (__FUNCTION__);
   DEBUG_LOCS   yLOG_spoint  (a_old);
   /*---(defense)------------------------*/
   --rce;  if (a_old == NULL) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   x_old = *a_old;
   DEBUG_LOCS   yLOG_spoint  (x_old);
   --rce;  if (x_old == NULL) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   a_type = x_old->type;
   DEBUG_LOCS   yLOG_schar   (a_type);
   TAB_pointer (&x_tab, x_old->tab);
   DEBUG_LOCS   yLOG_spoint  (x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(check col/row is empty)----------*/
   DEBUG_LOCS   yLOG_spoint  (x_old->n_head);
   --rce;  if (x_old->n_head != NULL) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(remove from list)---------------*/
   DEBUG_GRAF   yLOG_snote   ("remove");
   if (x_old->N_prev != NULL)  x_old->N_prev->N_next = x_old->N_next;
   else {
      IF_COL                   x_tab->C_head         = x_old->N_next;
      ELSE_ROW                 x_tab->R_head         = x_old->N_next;
   }
   if (x_old->N_next != NULL)  x_old->N_next->N_prev = x_old->N_prev;
   else {
      IF_COL                   x_tab->C_tail         = x_old->N_prev;
      ELSE_ROW                 x_tab->R_tail         = x_old->N_prev;
   }
   /*---(update counters)----------------*/
   IF_COL   --(x_tab->C_count);
   ELSE_ROW --(x_tab->R_count);
   DEBUG_LOCS   yLOG_sint    (x_tab->C_count);
   DEBUG_LOCS   yLOG_sint    (x_tab->R_count);
   /*---(free)---------------------------*/
   free (x_old);
   *a_old = NULL;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       search and finding                     ----===*/
/*====================------------------------------------====================*/
static void  o___SEARCH__________o () { return; }

char         /*-> cursor with no bounce/safeties -----------------------------*/
NODE_by_cursor          (tNODE **a_found, tTAB *a_tab, char a_type, char a_move)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tNODE      *x_head      = NULL;
   tNODE      *x_tail      = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_senter  (__FUNCTION__);
   DEBUG_LOCS   yLOG_spoint  (a_found);
   DEBUG_LOCS   yLOG_spoint  (a_tab);
   DEBUG_LOCS   yLOG_schar   (a_type);
   DEBUG_LOCS   yLOG_schar   (a_move);
   /*---(prepare)------------------------*/
   if (a_found != NULL)  *a_found = NULL;
   /*---(defense)------------------------*/
   --rce;  if (a_found == NULL || a_tab == NULL) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(look for tab change)------------*/
   DEBUG_LOCS   yLOG_spoint  (s_tab);
   DEBUG_LOCS   yLOG_spoint  (s_curr);
   if (s_tab != a_tab || s_type != a_type) {
      DEBUG_LOCS   yLOG_snote   ("search change");
      s_curr  = NULL;
      s_tab   = a_tab;
      s_type  = a_type;
   }
   /*---(defense)------------------------*/
   DEBUG_LOCS   yLOG_spoint  (s_curr);
   --rce;  if (s_curr == NULL && strchr ("<>", a_move) != NULL) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(handle move)--------------------*/
   IF_COL    { x_head = a_tab->C_head; x_tail = a_tab->C_tail; }
   ELSE_ROW  { x_head = a_tab->R_head; x_tail = a_tab->R_tail; }
   switch (a_move) {
   case '[' : s_curr  = x_head;             break;
   case '<' : s_curr  = s_curr->N_prev;     break;
   case '>' : s_curr  = s_curr->N_next;     break;
   case ']' : s_curr  = x_tail;             break;
   }
   DEBUG_LOCS   yLOG_spoint  (s_tab);
   DEBUG_LOCS   yLOG_spoint  (s_curr);
   /*---(check failure)------------------*/
   --rce;  if (s_curr == NULL) {
      DEBUG_LOCS   yLOG_snote   ("failed");
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(return)-------------------------*/
   DEBUG_LOCS   yLOG_snote   ("found");
   *a_found = s_curr;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
NODE_by_index           (tNODE **a_found, tTAB *a_tab, char a_type, ushort a_ref)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tNODE      *x_head      = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_senter  (__FUNCTION__);
   DEBUG_LOCS   yLOG_spoint  (a_found);
   DEBUG_LOCS   yLOG_spoint  (a_tab);
   DEBUG_LOCS   yLOG_sint    (a_ref);
   /*---(prepare)------------------------*/
   if (a_found != NULL)  *a_found = NULL;
   /*---(defense)------------------------*/
   --rce;  if (a_tab == NULL || a_found == NULL) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   s_tab = a_tab;
   IF_COL    x_head = a_tab->C_head;
   ELSE_ROW  x_head = a_tab->R_head;
   --rce;  if (x_head == NULL) {
      s_curr = NULL;
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(walk)---------------------------*/
   s_curr = x_head;
   --rce;  while (s_curr != NULL) {
      if (s_curr->ref >= a_ref)  break;
      s_curr = s_curr->N_next;
   }
   DEBUG_LOCS   yLOG_spoint  (s_tab);
   DEBUG_LOCS   yLOG_spoint  (s_curr);
   /*---(check failure)------------------*/
   --rce;  if (s_curr == NULL ||  s_curr->ref != a_ref) {
      s_curr = NULL;
      DEBUG_LOCS   yLOG_snote   ("failed");
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(return)-------------------------*/
   DEBUG_LOCS   yLOG_snote   ("found");
   *a_found = s_curr;
   DEBUG_LOCS   yLOG_spoint  (s_curr);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
NODE_ensure             (tNODE **a_found, tTAB *a_tab, char a_type, ushort a_ref)
{  /*---(notes)--------------------------*/
   /* returns  1 if col/row already exists (for testing/debugging)
    *          0 if col/row had to be created
    *          negative in case of trouble
    */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_valid     =    0;
   tNODE      *x_found     = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "found %-10.10p, tab %-10.10p, type %c, ref %3d", a_found, a_tab, a_type, a_ref);
   /*---(defense)------------------------*/
   --rce;  if (a_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   IF_COL   x_valid = VALID_col (a_ref);
   ELSE_ROW x_valid = VALID_row (a_ref);
   DEBUG_LOCS   yLOG_value   ("x_valid"   , x_valid);
   --rce;  if (!x_valid) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find column)--------------------*/
   NODE_by_index (&x_found, a_tab, a_type, a_ref);
   --rce;  if (x_found == NULL) {
      NODE_new (&x_found, a_tab, a_type, a_ref);
      if (x_found == NULL) {
         DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   } else rc = 1;
   DEBUG_LOCS   yLOG_value   ("ref"       , x_found->ref);
   DEBUG_LOCS   yLOG_value   ("count"     , x_found->count);
   /*---(return)-------------------------*/
   if (a_found != NULL)  *a_found = x_found;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                      unit testing                            ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }

char*        /*-> unit test accessor -----------------[ light  [us.B60.2A3.F2]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
NODE__unit         (char *a_question, uchar a_tab, char a_type, ushort a_ref)
{
   /*---(locals)-------------------------*/
   char        rc          =    0;
   tTAB       *x_tab       =    0;
   tNODE      *x_node      =    0;
   tCELL      *x_curr      =    0;
   int         i           =    0;
   char        x_pre       [LEN_LABEL] = "";
   char        s           [LEN_LABEL] = "";
   char        t           [LEN_HUND]  = "";
   int         c           =    0;
   uchar       x_abbr      =  '-';
   int         x_fore      =    0;
   int         x_back      =    0;
   uchar       x_abbr2     =  '-';
   tTAB       *x_tsave     = NULL;
   tNODE      *x_nsave     = NULL;
   char        x_label     [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(parse location)-----------------*/
   IF_COL   strlcpy (x_pre, "COL", LEN_TERSE);
   ELSE_ROW strlcpy (x_pre, "ROW", LEN_TERSE);
   x_tsave = s_tab;
   x_nsave = s_curr;
   DEBUG_LOCS   yLOG_complex ("before"    , "s_tab %-10.10p, s_curr %-10.10p", s_tab, s_curr);
   strcpy  (unit_answer, "NODE             : label could not be parsed");
   if (strcmp (a_question, "current"       ) != 0)  TAB_pointer (&x_tab, a_tab);
   else                                             x_tab = s_tab;
   x_abbr = LABEL_tab (a_tab);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   /*---(overall)------------------------*/
   if      (strcmp (a_question, "counts"        ) == 0) {
      if (x_tab == NULL)  snprintf(unit_answer, LEN_FULL, "%-3.3s counts   (-) : (---)  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -", x_pre);
      else {
         for (i = 0; i < 20; ++i) {
            NODE_by_index (&x_node, x_tab, a_type, i);
            if (x_node == NULL)    strlcpy (s, "  -" , LEN_LABEL);
            else                { sprintf (s, " %2d", x_node->count); ++c; }
            strlcat (t, s, LEN_HUND);
         }
         snprintf(unit_answer, LEN_FULL, "%-3.3s counts   (%c) : (%3d)%s", x_pre, x_abbr, c, t);
      }
   }
   else if (strcmp (a_question, "details"       ) == 0 || strcmp (a_question, "current"       ) == 0) {
      if (x_tab == NULL)  snprintf(unit_answer, LEN_FULL, "%-3.3s %-7.7s  (-) : -  -=-  --    -s  -c  -f  -b -", x_pre, a_question);
      else {
         if (strcmp (a_question, "current"       ) != 0)  NODE_by_index (&x_node, x_tab, a_type, a_ref);
         else                                             { x_node = s_curr; if (x_node != NULL) a_ref = x_node->ref; }
         if (x_node == NULL)  snprintf(unit_answer, LEN_FULL, "%-3.3s %-7.7s  (%c) : -  -=-  --    -s  -c  -f  -b  []", x_pre, a_question, x_abbr);
         else {
            IF_COL   { x_curr = x_node->n_head; while (x_curr != NULL) { ++x_fore; x_curr = x_curr->c_next; } }
            ELSE_ROW { x_curr = x_node->n_head; while (x_curr != NULL) { ++x_fore; x_curr = x_curr->r_next; } }
            IF_COL   { x_curr = x_node->n_tail; while (x_curr != NULL) { ++x_back; x_curr = x_curr->c_prev; } }
            ELSE_ROW { x_curr = x_node->n_tail; while (x_curr != NULL) { ++x_back; x_curr = x_curr->r_prev; } }
            x_curr = x_node->n_head;
            while (x_curr != NULL) {
               sprintf (s, "%d", x_curr->row);
               if (x_curr != x_node->n_head)   strlcat (t, ",", LEN_HUND);
               strlcat (t, s, LEN_HUND);
               IF_COL   x_curr = x_curr->c_next;
               ELSE_ROW x_curr = x_curr->r_next;
            }
            x_abbr2 = LABEL_tab (x_node->tab);
            IF_COL   strlcpy (x_label, LABEL_col (a_ref), LEN_LABEL);
            ELSE_ROW strlcpy (x_label, LABEL_row (a_ref), LEN_LABEL);
            snprintf(unit_answer, LEN_FULL, "%-3.3s %-7.7s  (%c) : %c %2d=%-2d %-4.4s %2ds %2dc %2df %2db  [%s]", x_pre, a_question, x_abbr, x_abbr2, a_ref, x_node->ref, x_label, x_node->size, x_node->count, x_fore, x_back, t);
         }
      }
   }
   s_tab  = x_tsave;
   s_curr = x_nsave;
   DEBUG_LOCS   yLOG_complex ("after"     , "s_tab %-10.10p, s_curr %-10.10p", s_tab, s_curr);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return unit_answer;
}
