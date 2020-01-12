/*============================----beg-of-source---============================*/
#include   "gyges.h"



static tTAB  *s_tab    = NULL;
static char   s_type   = '-';
static tNODE *s_curr   = NULL;



#define   IF_COL     if (a_type == 'c') 
#define   ELSE_ROW   else 

#define   IF_MATCH   if ((a_type == 'c' && x_curr->row == a_seq) || (a_type == 'r' && x_curr->col == a_seq)) 
#define   IF_PAST    if ((a_type == 'c' && x_curr->row >  a_seq) || (a_type == 'r' && x_curr->col >  a_seq)) 
#define   IF_EMPTY   if (x_node->n_head == NULL && ((a_type == 'c' && x_node->size == DEF_WIDTH) || (a_type == 'r' && x_node->size == DEF_HEIGHT)))

#define   IF_DEFAULT if ((a_type == 'c' && x_node->size == DEF_WIDTH) || (a_type == 'r' && x_node->size == DEF_HEIGHT))



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
   /*---(find col/row)-------------------*/
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
/*===----                      program level                           ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

short
NODE_cleanse            (tTAB *a_tab, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tNODE     *x_next       = NULL;
   tNODE     *x_node       = NULL;
   short      x_remain     =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_LOCS   yLOG_point   ("a_tab"     , a_tab);
   --rce;  if (a_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_value   ("cells"     , a_tab->count);
   IF_COL   x_remain = a_tab->C_count;
   ELSE_ROW x_remain = a_tab->R_count;
   DEBUG_LOCS   yLOG_value   ("before"    , x_remain);
   /*---(remove all rows)----------------*/
   IF_COL   x_node = a_tab->C_head;
   ELSE_ROW x_node = a_tab->R_head;
   while (x_node != NULL) {
      x_next = x_node->N_next;
      if (x_node->count <= 0) {
         IF_COL   x_node->size = DEF_WIDTH;
         ELSE_ROW x_node->size = DEF_HEIGHT;
         NODE_free (&x_node);
      }
      x_node = x_next;
   }
   IF_COL   x_remain = a_tab->C_count;
   ELSE_ROW x_remain = a_tab->R_count;
   DEBUG_LOCS   yLOG_value   ("after"     , x_remain);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return x_remain;
}



/*====================------------------------------------====================*/
/*===----                  hooking and unhooking cells                 ----===*/
/*====================------------------------------------====================*/
static void  o___HOOKING_________o () { return; }

char
NODE_hook_cell          (tTAB *a_tab, char a_type, ushort a_ref, ushort a_seq, tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tNODE     *x_node       = NULL;
   tCELL     *x_prev       = NULL;
   tCELL     *x_curr       = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "tab %-10.10p, type %c, ref %3d, row %3d, cell %-10.10p", a_tab, a_type, a_ref, a_seq, a_cell);
   /*---(defense)------------------------*/
   --rce;  if (a_tab == NULL && a_cell == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find col/row)-------------------*/
   NODE_ensure (&x_node, a_tab, a_type, a_ref);
   DEBUG_LOCS   yLOG_point   ("ensure"    , x_node);
   --rce;  if (x_node == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find cell position)-------------*/
   x_curr = x_node->n_head;
   --rce;  while (x_curr != NULL) {
      IF_MATCH {
         DEBUG_LOCS   yLOG_note    ("already exists");
         DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      IF_PAST break;
      x_prev = x_curr;
      IF_COL   x_curr = x_curr->c_next;
      ELSE_ROW x_curr = x_curr->r_next;
   }
   DEBUG_LOCS   yLOG_point   ("x_prev"    , x_prev);
   DEBUG_LOCS   yLOG_point   ("x_curr"    , x_curr);
   /*---(add cell to col)----------------*/
   if (x_prev == NULL && x_curr == NULL) {
      DEBUG_LOCS   yLOG_snote   ("first cell in col/row");
      x_node->n_head         = a_cell;
      x_node->n_tail         = a_cell;
   } else if (x_prev == NULL && x_curr != NULL) {
      DEBUG_LOCS   yLOG_snote   ("prepend to front");
      IF_COL    { a_cell->c_prev = NULL; a_cell->c_next = x_node->n_head; x_node->n_head->c_prev = a_cell; }
      ELSE_ROW  { a_cell->r_prev = NULL; a_cell->r_next = x_node->n_head; x_node->n_head->r_prev = a_cell; }
      x_node->n_head         = a_cell;
   } else if (x_prev != NULL && x_curr == NULL) {
      DEBUG_LOCS   yLOG_snote   ("append to back");
      IF_COL    { a_cell->c_next = NULL; a_cell->c_prev = x_node->n_tail; x_node->n_tail->c_next = a_cell; }
      ELSE_ROW  { a_cell->r_next = NULL; a_cell->r_prev = x_node->n_tail; x_node->n_tail->r_next = a_cell; }
      x_node->n_tail         = a_cell;
   } else {
      DEBUG_LOCS   yLOG_snote   ("insert in middle");
      IF_COL    { a_cell->c_next = x_prev->c_next; x_prev->c_next = a_cell; a_cell->c_prev = x_curr->c_prev; x_curr->c_prev = a_cell; }
      ELSE_ROW  { a_cell->r_next = x_prev->r_next; x_prev->r_next = a_cell; a_cell->r_prev = x_curr->r_prev; x_curr->r_prev = a_cell; }
   }
   /*---(update counts)------------------*/
   ++(x_node->count);
   DEBUG_LOCS   yLOG_value   ("count"     , x_node->count);
   /*---(update col ties)----------------*/
   IF_COL   a_cell->col      = a_ref;
   ELSE_ROW a_cell->row      = a_ref;
   IF_COL   a_cell->C_parent = x_node;
   ELSE_ROW a_cell->R_parent = x_node;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
NODE_unhook_cell        (char a_type, tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tNODE      *x_node      = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "type %c, cell %-10.10p", a_type, a_cell);
   /*---(defense)------------------------*/
   --rce;  if (a_cell == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find col/row)-------------------*/
   IF_COL   x_node = a_cell->C_parent;
   ELSE_ROW x_node = a_cell->R_parent;
   DEBUG_LOCS   yLOG_point   ("x_node"    , x_node);
   --rce;  if (x_node == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(remove from list)---------------*/
   DEBUG_GRAF   yLOG_note    ("remove from col/row");
   IF_COL {
      if (a_cell->c_prev != NULL)  a_cell->c_prev->c_next = a_cell->c_next;
      else                         x_node->n_head         = a_cell->c_next;
      if (a_cell->c_next != NULL)  a_cell->c_next->c_prev = a_cell->c_prev;
      else                         x_node->n_tail         = a_cell->c_prev;
   }
   ELSE_ROW {
      if (a_cell->r_prev != NULL)  a_cell->r_prev->r_next = a_cell->r_next;
      else                         x_node->n_head         = a_cell->r_next;
      if (a_cell->r_next != NULL)  a_cell->r_next->r_prev = a_cell->r_prev;
      else                         x_node->n_tail         = a_cell->r_prev;
   }
   /*---(update counters)----------------*/
   --(x_node->count);
   DEBUG_LOCS   yLOG_sint    (x_node->count);
   /*---(update col ties)----------------*/
   IF_COL   a_cell->col      = UNHOOKED;
   ELSE_ROW a_cell->row      = UNHOOKED;
   IF_COL   a_cell->C_parent = NULL;
   ELSE_ROW a_cell->R_parent = NULL;
   /*---(check for freeing)--------------*/
   DEBUG_LOCS   yLOG_point   ("n_head"    , x_node->n_head);
   DEBUG_LOCS   yLOG_value   ("count"     , x_node->count);
   DEBUG_LOCS   yLOG_value   ("size"      , x_node->size);
   DEBUG_LOCS   yLOG_value   ("def_width" , DEF_WIDTH);
   DEBUG_LOCS   yLOG_value   ("def_height", DEF_HEIGHT);
   --rce;  IF_EMPTY {
      rc = NODE_free (&x_node);
      DEBUG_LOCS   yLOG_value   ("free"      , rc);
      if (rc < 0) {
         DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    work with counts                          ----===*/
/*====================------------------------------------====================*/
static void  o___USAGE___________o () { return; }

short        /*-> indicate if col/row is used --------[ ------ [gn.210.212.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
NODE_used                (char a_index, char a_type, short a_ref)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tTAB       *x_tab       = NULL;
   tNODE      *x_found     = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%2d, %c, %3d", a_index, a_type, a_ref);
   /*---(defense)------------------------*/
   --rce;  if (TAB_legal (a_index) != 1) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert to pointer)-------------*/
   TAB_pointer (&x_tab, a_index);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find col/row)-------------------*/
   NODE_by_index (&x_found, x_tab, a_type, a_ref);
   --rce;  if (x_found == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_value   ("count"     , x_found->count);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return x_found->count;
}

short        /*-> find smallest used col/row in tab --[ ------ [gn.210.212.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
NODE_min_used        (char a_index, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tTAB       *x_tab       = NULL;
   tNODE      *x_found     = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%2d, %c", a_index, a_type);
   /*---(defense)------------------------*/
   --rce;  if (TAB_legal (a_index) != 1) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert to pointer)-------------*/
   TAB_pointer (&x_tab, a_index);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find col/row)-------------------*/
   NODE_by_cursor (&x_found, x_tab, a_type, '[');
   --rce;  if (x_found == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_value   ("min used"  , x_found->ref);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return x_found->ref;
}

short        /*-> find largest used col/row in tab ---[ ------ [gn.210.212.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
NODE_max_used        (char a_index, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tTAB       *x_tab       = NULL;
   tNODE      *x_found     = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%2d, %c", a_index, a_type);
   /*---(defense)------------------------*/
   --rce;  if (TAB_legal (a_index) != 1) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert to pointer)-------------*/
   TAB_pointer (&x_tab, a_index);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find col/row)-------------------*/
   NODE_by_cursor (&x_found, x_tab, a_type, ']');
   --rce;  while (x_found == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_value   ("max used"  , x_found->ref);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return x_found->ref;
}

short
NODE_max             (char a_index, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tTAB       *x_tab       = NULL;
   int         x_max       =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%2d, %c", a_index, a_type);
   /*---(defense)------------------------*/
   --rce;  if (TAB_legal (a_index) != 1) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert to pointer)-------------*/
   TAB_pointer (&x_tab, a_index);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find max)-----------------------*/
   DEBUG_LOCS   yLOG_value   ("max"       , x_tab->ncol);
   DEBUG_LOCS   yLOG_value   ("max"       , x_tab->nrow);
   IF_COL   x_max = x_tab->ncol;
   ELSE_ROW x_max = x_tab->nrow;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return x_max;
}

short
NODE_max_adjust      (char a_index, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tTAB       *x_tab       = NULL;
   tNODE      *x_found     = NULL;
   int         x_max       =    0;
   int         x_new       =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%2d, %c", a_index, a_type);
   /*---(defense)------------------------*/
   --rce;  if (TAB_legal (a_index) != 1) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert to pointer)-------------*/
   TAB_pointer (&x_tab, a_index);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find max)-----------------------*/
   NODE_by_cursor (&x_found, x_tab, a_type, ']');
   if    (x_found == NULL) x_max = 1;
   else                    x_max = x_found->ref;
   DEBUG_LOCS   yLOG_value   ("x_max"     , x_max);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set new max)--------------------*/
   IF_COL {
      x_new = (trunc (x_max /  26) + 1) *  26;
      if (x_new >  MAX_col ())  x_new = MAX_col ();
   }
   ELSE_ROW {
      x_new = (trunc (x_max / 100) + 1) * 100;
      if (x_new >  MAX_row ())  x_new = MAX_row ();
   }
   --x_new;
   DEBUG_LOCS   yLOG_value   ("x_new"     , x_new);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return x_new;
}



/*====================------------------------------------====================*/
/*===----                   work with sizes                            ----===*/
/*====================------------------------------------====================*/
static void  o___SIZING__________o () { return; }

char
NODE_size                (char a_tab, char a_type, short a_ref)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tTAB       *x_tab       = NULL;
   tNODE      *x_node      = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%2dt, %c, %3dr", a_tab, a_type, a_ref);
   /*---(defense)------------------------*/
   rc = TAB_pointer (&x_tab, a_tab);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find col/row)-------------------*/
   NODE_by_index (&x_node, a_tab, a_type, a_ref);
   DEBUG_LOCS   yLOG_point   ("x_node"    , x_node);
   --rce;  if (x_node == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_value   ("size"      , x_node->size);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return x_node->size;
}

char
NODE_resize             (char a_tab, char a_type, short a_ref, char a_size)
{  /*---(notes)--------------------------*/
   /*
    * metis § ----- § if col/row set back to default and empty, remove it
    *
    */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tTAB       *x_tab       = NULL;
   tNODE      *x_node      = NULL;
   char        x_alt       =  '-';
   tCELL      *x_curr      = NULL;
   short       c           =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%2dt, %c, %3dr, %3ds", a_tab, a_type, a_ref, a_size);
   /*---(defense)------------------------*/
   rc = TAB_pointer (&x_tab, a_tab);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find col/row)-------------------*/
   NODE_by_index (&x_node, a_tab, a_type, a_ref);
   DEBUG_LOCS   yLOG_point   ("x_node"    , x_node);
   --rce;  if (x_node == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(statistics)---------------------*/
   DEBUG_LOCS   yLOG_value   ("count"     , x_node->count);
   DEBUG_LOCS   yLOG_value   ("size"      , x_node->size);
   /*---(limits)-------------------------*/
   if (a_size  < MIN_WIDTH)    a_size = MIN_WIDTH;
   if (a_size  > MAX_WIDTH)    a_size = MAX_WIDTH;
   DEBUG_LOCS   yLOG_value   ("a_size*"   , a_size);
   if (a_size == x_node->size) {
      DEBUG_LOCS   yLOG_note    ("already set to new size");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check if default size)----------*/
   IF_DEFAULT {
      DEBUG_LOCS   yLOG_note    ("making default size");
      if (x_node->count == 0) {
         NODE_free (&x_node);
         DEBUG_LOCS   yLOG_point   ("x_node"    , x_node);
         if (x_node == NULL) {
            DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
            return 0;
         }
      }
   }
   /*---(update all col/row)-------------*/
   x_curr = x_node->n_head;
   while (x_curr != NULL) {
      /*---(update merged cells)---------*/
      if (x_curr->type == YCALC_DATA_MERGED)  yCALC_calc_from (x_curr->ycalc);
      /*---(update printable)------------*/
      api_ycalc_printer (x_curr);
      ++c;
      /*---(next)------------------------*/
      IF_COL   x_curr = x_curr->r_next;
      ELSE_ROW x_curr = x_curr->c_next;
   }
   /*---(history)----------------------*/
   /*> IF_COL   HIST_size   (a_mode, HIST_WIDTH   , a_tab, a_ref, x_prev, x_size);    <* 
    *> ELSE_ROW HIST_size   (a_mode, HIST_HEIGHT  , a_tab, a_ref, x_prev, x_size);    <*/
   /*---(reset headers)------------------*/
   yVIKEYS_map_refresh ();
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return c;
}

char
NODE_reset               (char a_tab, char a_type, short a_ref)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x_size      =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%2dt, %c, %3dr", a_tab, a_type, a_ref);
   /*---(reset size)---------------------*/
   IF_COL   x_size = DEF_WIDTH;
   ELSE_ROW x_size = DEF_HEIGHT;
   rc = NODE_resize (a_tab, a_type, a_ref, x_size);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> change the col width ---------------[ ------ [gc.320.312.31]*/ /*-[00.0000.404.5]-*/ /*-[--.---.---.--]-*/
NODE_multisize       (char *a_label, char a_type, char a_size, char a_count)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_tab       =    0;
   short       x_col       =    0;
   short       x_row       =    0;
   short       x_ref       =    0;
   short       x_off       =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%-10.10p, %c, %2ds, %2dc", a_label, a_type, a_size, a_count);
   /*---(defense)------------------------*/
   rc = str2gyges (a_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_USABLE);
   DEBUG_LOCS   yLOG_value   ("str2gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rc;
   }
   DEBUG_LOCS   yLOG_complex ("coord"     , "%s, %2dt, %3dc, %4dr", a_label, x_tab, x_col, x_row);
   /*---(prepare)------------------------*/
   if (a_count == 0)  a_count = 1;
   DEBUG_LOCS   yLOG_value   ("a_count"   , a_count);
   /*---(resize)-------------------------*/
   IF_COL   x_ref = x_col;
   ELSE_ROW x_ref = x_row;
   for (x_off = 0; x_off < a_count; ++x_off) {
      rc = NODE_resize (x_tab, a_type, x_ref + x_off, a_size);
      DEBUG_INPT  yLOG_value   ("risize"    , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> change cell column width -----------[ ------ [gc.E91.292.69]*/ /*-[02.0000.303.Y]-*/ /*-[--.---.---.--]-*/
NODE_visual        (char a_tab, char a_type, short a_ref, char a_mode, char a_key)
{  /*---(design notes)-------------------*/
   /*  update all cells to new width, either a standard size, or a specific   */
   /*  value communicated as a negative number.                               */
   /*---(locals)-----------+-----------+-*/
   int         x_size      = 0;
   /*---(adjust)----------------------*/
   x_size  = NODE_size (a_tab, a_type, a_ref);
   if (a_key <   0) {
      x_size                 = -(a_key);
   } else {
      switch (a_key) {
      case  'm' : x_size     = 0;                           break;
      case  'd' : x_size     = DEF_WIDTH;                   break;
      case  'n' : x_size     = 8;                           break;
      case  'N' : x_size     = 12;                          break;
      case  'w' : x_size     = 20;                          break;
      case  'W' : x_size     = 50;                          break;
      case  'h' : x_size    -= 1;                           break;
      case  'l' : x_size    += 1;                           break;
      case  'H' : x_size     = ((x_size  / 5) * 5);         break;
      case  'L' : x_size     = (((x_size  / 5) + 1) * 5);   break;
      }
   }
   /*---(set width)--------------------*/
   NODE_resize  (a_tab, a_type, a_ref, x_size);
   /*---(reset headers)---------------*/
   yVIKEYS_map_refresh ();
   /*---(complete)---------------------------*/
   DEBUG_CELL  yLOG_exit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     freezing columns                         ----===*/
/*====================------------------------------------====================*/
static void  o___FREEZING________o () { return; }

char         /*-> change the frozen cols -------------[ ------ [gc.430.323.31]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
NODE_freeze           (int a_tab, char a_type, int a_beg, int a_end)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tTAB       *x_tab       = NULL;
   char        x_valid     =    0;
   int         x_temp      =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%3dt, %c, %3db, %3de", a_tab, a_type, a_beg, a_end);
   /*---(defense)------------------------*/
   TAB_pointer (&x_tab, a_tab);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(beginning)----------------------*/
   IF_COL   x_valid = LEGAL_COL (a_tab, a_beg);
   ELSE_ROW x_valid = LEGAL_ROW (a_tab, a_beg);
   DEBUG_LOCS   yLOG_value   ("x_valid"   , x_valid);
   --rce;  if (!x_valid) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(ending)-------------------------*/
   IF_COL   x_valid = LEGAL_COL (a_tab, a_end);
   ELSE_ROW x_valid = LEGAL_ROW (a_tab, a_end);
   DEBUG_LOCS   yLOG_value   ("x_valid"   , x_valid);
   --rce;  if (!x_valid) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(swap, if needed)----------------*/
   if (a_beg  >  a_end) {
      DEBUG_LOCS   yLOG_note    ("must swap beg and end");
      x_temp = a_beg;
      a_beg  = a_end;
      a_end  = x_temp;
   }
   /*---(save)---------------------------*/
   IF_COL {
      x_tab->froz_col    = 'y';
      x_tab->froz_bcol   = a_beg;
      x_tab->froz_ecol   = a_end;
   } ELSE_ROW {
      x_tab->froz_row    = 'y';
      x_tab->froz_brow   = a_beg;
      x_tab->froz_erow   = a_end;
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear the frozen cols --------------[ ------ [gc.320.112.11]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
NODE_unfreeze         (int a_tab, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tTAB       *x_tab       = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%3dt, %c", a_tab, a_type);
   /*---(defense)------------------------*/
   TAB_pointer (&x_tab, a_tab);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   IF_COL {
      x_tab->froz_col    = '-';
      x_tab->froz_bcol   = 0;
      x_tab->froz_ecol   = 0;
   } ELSE_ROW {
      x_tab->froz_row    = '-';
      x_tab->froz_brow   = 0;
      x_tab->froz_erow   = 0;
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    yparse record handling                    ----===*/
/*====================------------------------------------====================*/
static void  o___YPARSE__________o () { return; }

char
NODE_reader          (void)
{
   /*> /+---(locals)-----------+-----------+-+/                                       <* 
    *> char        rce         =  -11;                                                <* 
    *> char        rc          =    0;                                                <* 
    *> char        x_verb      [LEN_LABEL];                                           <* 
    *> char        x_label     [LEN_LABEL];                                           <* 
    *> int         x_size      =    0;                                                <* 
    *> int         x_count     =    0;                                                <* 
    *> char        x_type      =  '-';                                                <* 
    *> /+---(header)-------------------------+/                                       <* 
    *> DEBUG_INPT   yLOG_enter   (__FUNCTION__);                                      <* 
    *> /+---(get verb)-----------------------+/                                       <* 
    *> rc = yPARSE_popstr (x_verb);                                                   <* 
    *> DEBUG_INPT   yLOG_value   ("pop verb"  , rc);                                  <* 
    *> DEBUG_INPT   yLOG_info    ("x_verb"    , x_verb);                              <* 
    *> --rce;  if (strcmp ("width", x_verb) != 0) {                                   <* 
    *>    DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> /+---(label)--------------------------+/                                       <* 
    *> rc = yPARSE_popstr (x_label);                                                  <* 
    *> DEBUG_INPT   yLOG_value   ("pop max"   , rc);                                  <* 
    *> DEBUG_INPT   yLOG_info    ("max"       , x_label);                             <* 
    *> /+---(size)---------------------------+/                                       <* 
    *> rc = yPARSE_popint  (&x_size);                                                 <* 
    *> DEBUG_INPT   yLOG_value   ("pop width" , rc);                                  <* 
    *> DEBUG_INPT   yLOG_value   ("width"     , x_size);                              <* 
    *> rc = yPARSE_popint  (&x_count);                                                <* 
    *> DEBUG_INPT   yLOG_value   ("pop count" , rc);                                  <* 
    *> DEBUG_INPT   yLOG_value   ("count"     , x_count);                             <* 
    *> /+---(resize)-------------------------+/                                       <* 
    *> rc = COL_resize (x_label, x_size, x_count);                                    <* 
    *> DEBUG_INPT   yLOG_value   ("resize"    , rc);                                  <* 
    *> if (rc < 0) {                                                                  <* 
    *>    DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);                               <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> /+---(complete)-----------------------+/                                       <* 
    *> DEBUG_INPT  yLOG_exit    (__FUNCTION__);                                       <* 
    *> return 1;                                                                      <*/
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
NODE_writer              (int a_tab, int a_col)
{
   /*> /+---(locals)-----------+-----------+-+/                                       <* 
    *> char        rce         =  -10;                                                <* 
    *> char        rc          =    0;                                                <* 
    *> char        c           =    0;                                                <* 
    *> int         x_max       =    0;                                                <* 
    *> int         i           =    0;                                                <* 
    *> int         x_def       =    0;                                                <* 
    *> int         x_size      =    0;                                                <* 
    *> int         x_prev      =    0;                                                <* 
    *> char        x_label     [LEN_LABEL];                                           <* 
    *> /+---(header)-------------------------+/                                       <* 
    *> DEBUG_OUTP   yLOG_enter   (__FUNCTION__);                                      <* 
    *> /+---(clear output)-------------------+/                                       <* 
    *> yPARSE_outclear  ();                                                           <* 
    *> /+---(prepare tab)--------------------+/                                       <* 
    *> DEBUG_OUTP   yLOG_value   ("a_tab"     , a_tab);                               <* 
    *> rc = LEGAL_TAB (a_tab);                                                        <* 
    *> --rce; if (rc == 0) {                                                          <* 
    *>    DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> rc = LEGAL_COL (a_tab, a_col);                                                 <* 
    *> --rce;  if (rc == 0) {                                                         <* 
    *>    DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> /+---(filter)----------------------+/                                          <* 
    *> x_size = COL_width  (a_tab, a_col);                                            <* 
    *> DEBUG_OUTP   yLOG_value   ("x_size"    , x_size);                              <* 
    *> /+---(check default)------------------+/                                       <* 
    *> x_def  = TAB_colwide (a_tab);                                                  <* 
    *> DEBUG_OUTP   yLOG_value   ("x_def"     , x_def);                               <* 
    *> --rce;  if (x_size == x_def ) {                                                <* 
    *>    DEBUG_OUTP   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <* 
    *> /+---(check prev)---------------------+/                                       <* 
    *> if (a_col > 0)  x_prev = COL_width  (a_tab, a_col - 1);                        <* 
    *> --rce;  if (x_size == x_prev) {                                                <* 
    *>    DEBUG_OUTP   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <* 
    *> /+---(check repeats)---------------+/                                          <* 
    *> x_max = COL_max (a_tab) - 1;                                                   <* 
    *> DEBUG_OUTP   yLOG_value   ("x_max"     , x_max);                               <* 
    *> c = 1;                                                                         <* 
    *> for (i = a_col + 1; i <= x_max; ++i) {                                         <* 
    *>    if (x_size != COL_width  (a_tab, i))  break;                                <* 
    *>    ++c;                                                                        <* 
    *> }                                                                              <* 
    *> DEBUG_OUTP   yLOG_value   ("c"         , c);                                   <* 
    *> /+---(write)-----------------------+/                                          <* 
    *> rc = str4gyges (a_tab, a_col, 0, 0, 0, x_label, YSTR_USABLE);                   <* 
    *> yPARSE_fullwrite ("width", x_label, x_size, c);                                <* 
    *> /+---(complete)-----------------------+/                                       <* 
    *> DEBUG_OUTP  yLOG_exit    (__FUNCTION__);                                       <* 
    *> return c;                                                                      <*/
}

char
NODE_writer_all          (void)
{
   /*> /+---(locals)-----------+-----------+-+/                                       <* 
    *> char        rc          =    0;                                                <* 
    *> int         x_ntab      =    0;                                                <* 
    *> int         x_tab       =    0;                                                <* 
    *> int         x_ncol      =    0;                                                <* 
    *> int         x_col       =    0;                                                <* 
    *> int         c           =    0;                                                <* 
    *> /+---(walk)---------------------------+/                                       <* 
    *> yPARSE_verb_begin ("width");                                                   <* 
    *> x_ntab = TAB_max ();                                                           <* 
    *> if (x_ntab > 35)  x_ntab = 35;                                                 <* 
    *> for (x_tab = 0; x_tab <= x_ntab; ++x_tab) {                                    <* 
    *>    if (!LEGAL_TAB (x_tab))    continue;                                        <* 
    *>    x_ncol = COL_max (x_tab);                                                   <* 
    *>    for (x_col = 0; x_col < x_ncol; ++x_col) {                                  <* 
    *>       rc = COL_writer   (x_tab, x_col);                                        <* 
    *>       if (rc <= 0)    continue;                                                <* 
    *>       ++c;                                                                     <* 
    *>       yPARSE_verb_break (c);                                                   <* 
    *>    }                                                                           <* 
    *> }                                                                              <* 
    *> yPARSE_verb_end   (c);                                                         <* 
    *> /+---(complete)-----------------------+/                                       <* 
    *> return c;                                                                      <*/
}



/*====================------------------------------------====================*/
/*===----                      unit testing                            ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

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
               IF_COL   sprintf (s, "%d", x_curr->row);
               ELSE_ROW sprintf (s, "%d", x_curr->col);
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
