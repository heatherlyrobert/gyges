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

char         /*-> re-entrant cursor with no bounce/safeties ------------------*/
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
   /*---(defense)------------------------*/
   --rce;  if (a_found == NULL || a_tab == NULL) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(look for tab change)------------*/
   DEBUG_LOCS   yLOG_spoint  (s_curr);
   if (s_tab != a_tab || s_type != a_type) {
      DEBUG_LOCS   yLOG_snote   ("search change");
      *a_found = s_curr = NULL;
      s_tab    = a_tab;
      s_type   = a_type;
   } else if (*a_found != NULL && strchr ("<>", a_move) != NULL) {
      DEBUG_LOCS   yLOG_snote   ("continuing search");
      s_curr   = *a_found;
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
   DEBUG_LOCS   yLOG_spoint  (x_head);
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

char
NODE_init            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(command line commands)----------*/
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "colreset"    , ""    , ""     , COL_cleanse_curr           , "make all columns default size"        );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "colwide"     , ""    , "sii"  , COL_multisize              , "change the size of columns"           );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "rowreset"    , ""    , ""     , ROW_cleanse_curr           , "make all rows default size"           );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "rowtall"     , ""    , "sii"  , ROW_multisize              , "change the size of columns"           );
   /*---(add yparse specification)-------*/
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return rc;
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
      DEBUG_LOCS   yLOG_note    ("first cell in col/row");
      IF_COL    { a_cell->c_prev = NULL; }
      ELSE_ROW  { a_cell->r_prev = NULL; }
      x_node->n_head         = a_cell;
      x_node->n_tail         = a_cell;
   } else if (x_prev == NULL && x_curr != NULL) {
      DEBUG_LOCS   yLOG_note    ("prepend to front");
      IF_COL    { a_cell->c_prev = NULL; a_cell->c_next = x_node->n_head; x_node->n_head->c_prev = a_cell; }
      ELSE_ROW  { a_cell->r_prev = NULL; a_cell->r_next = x_node->n_head; x_node->n_head->r_prev = a_cell; }
      x_node->n_head         = a_cell;
   } else if (x_prev != NULL && x_curr == NULL) {
      DEBUG_LOCS   yLOG_note    ("append to back");
      IF_COL    { a_cell->c_next = NULL; a_cell->c_prev = x_node->n_tail; x_node->n_tail->c_next = a_cell; }
      ELSE_ROW  { a_cell->r_next = NULL; a_cell->r_prev = x_node->n_tail; x_node->n_tail->r_next = a_cell; }
      x_node->n_tail         = a_cell;
   } else {
      DEBUG_LOCS   yLOG_note    ("insert in middle");
      IF_COL    { a_cell->c_next = x_prev->c_next; x_prev->c_next = a_cell; a_cell->c_prev = x_curr->c_prev; x_curr->c_prev = a_cell; }
      ELSE_ROW  { a_cell->r_next = x_prev->r_next; x_prev->r_next = a_cell; a_cell->r_prev = x_curr->r_prev; x_curr->r_prev = a_cell; }
   }
   /*---(update col ties)----------------*/
   ++(x_node->count);
   IF_COL   a_cell->col      = a_ref;
   ELSE_ROW a_cell->row      = a_ref;
   IF_COL   a_cell->C_parent = x_node;
   ELSE_ROW a_cell->R_parent = x_node;
   IF_COL   { DEBUG_LOCS   yLOG_complex ("C_parent"  , "%-10.10p, %c, %3dr, %3dc", a_cell->C_parent, a_cell->C_parent->type, a_cell->C_parent->ref, a_cell->C_parent->count); }
   ELSE_ROW { DEBUG_LOCS   yLOG_complex ("R_parent"  , "%-10.10p, %c, %3dr, %3dc", a_cell->R_parent, a_cell->R_parent->type, a_cell->R_parent->ref, a_cell->R_parent->count); }
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
      a_cell->c_prev = NULL;
      a_cell->c_next = NULL;
   }
   ELSE_ROW {
      if (a_cell->r_prev != NULL)  a_cell->r_prev->r_next = a_cell->r_next;
      else                         x_node->n_head         = a_cell->r_next;
      if (a_cell->r_next != NULL)  a_cell->r_next->r_prev = a_cell->r_prev;
      else                         x_node->n_tail         = a_cell->r_prev;
      a_cell->r_prev = NULL;
      a_cell->r_next = NULL;
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
   --rce;  if (!TAB_live (a_index)) {
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
   --rce;  if (!TAB_live (a_index)) {
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
   --rce;  if (!TAB_live (a_index)) {
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
   --rce;  if (!TAB_live (a_index)) {
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
NODE__max_set           (char a_index, char a_type, short a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tTAB       *x_tab       = NULL;
   tNODE      *x_found     = NULL;
   int         x_max       =    0;
   int         x_new       =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%2d, %c, %4d", a_index, a_type, a_max);
   /*---(defense)------------------------*/
   --rce;  if (!TAB_live (a_index)) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find max)-----------------------*/
   x_max = NODE_max_used (a_index, a_type);
   if (x_max < 0)  x_max = 0;
   DEBUG_LOCS   yLOG_value   ("x_max"     , x_max);
   /*> --rce;  if (x_max < 0) {                                                       <* 
    *>    DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <*/
   /*---(convert to pointer)-------------*/
   TAB_pointer (&x_tab, a_index);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set new max)--------------------*/
   if (a_max == -1) {
      IF_COL   a_max = (trunc (x_max /  26) + 1) *  26 - 1;
      ELSE_ROW a_max = (trunc (x_max / 100) + 1) * 100 - 1;
   }
   DEBUG_LOCS   yLOG_value   ("a_max"     , a_max);
   /*---(check usage)--------------------*/
   if (a_max < x_max)  a_max = x_max;
   /*---(check limits)-------------------*/
   IF_COL   if (a_max >  MAX_col ())  a_max = MAX_col ();
   ELSE_ROW if (a_max >  MAX_row ())  a_max = MAX_row ();
   DEBUG_LOCS   yLOG_value   ("a_max"     , a_max);
   /*---(set max)------------------------*/
   IF_COL   x_tab->ncol = a_max;
   ELSE_ROW x_tab->nrow = a_max;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return a_max;
}

short
NODE_max_set            (char a_index, char a_type, short a_max)
{
   return NODE__max_set (a_index, a_type, a_max);
}

short
NODE_max_adjust         (char a_index, char a_type)
{
   return NODE__max_set (a_index, a_type, -1);
}



/*====================------------------------------------====================*/
/*===----                   work with sizes                            ----===*/
/*====================------------------------------------====================*/
static void  o___SIZING__________o () { return; }

char
NODE_size                (char a_index, char a_type, short a_ref)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tTAB       *x_tab       = NULL;
   tNODE      *x_node      = NULL;
   char        x_size      =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%2dt, %c, %3dr", a_index, a_type, a_ref);
   /*---(defense)------------------------*/
   rc = TAB_pointer (&x_tab, a_index);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_complex ("tab"       , "%c, %2d, %s", x_tab->abbr, x_tab->tab, x_tab->name);
   /*---(find col/row)-------------------*/
   NODE_by_index (&x_node, x_tab, a_type, a_ref);
   DEBUG_LOCS   yLOG_point   ("x_node"    , x_node);
   if (x_node != NULL) {
      DEBUG_LOCS   yLOG_complex ("node"      , "%c, %2dr, %2ds", x_node->type, x_node->ref, x_node->size);
      x_size = x_node->size;
   } else {
      IF_COL   x_size = DEF_WIDTH;
      ELSE_ROW x_size = DEF_HEIGHT;
   }
   DEBUG_LOCS   yLOG_value   ("x_size"    , x_size);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return x_size;
}

char
NODE__resize            (char a_index, char a_type, short a_ref, char a_size, char a_key, char a_mode)
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
   char        x_prev      =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%2dt, %c, %3dr, %3ds, %c, %c", a_index, a_type, a_ref, a_size, a_key, a_mode);
   /*---(defense)------------------------*/
   rc = TAB_pointer (&x_tab, a_index);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find col/row)-------------------*/
   NODE_ensure (&x_node, x_tab, a_type, a_ref);
   DEBUG_LOCS   yLOG_point   ("x_node"    , x_node);
   --rce;  if (x_node == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(statistics)---------------------*/
   DEBUG_LOCS   yLOG_value   ("count"     , x_node->count);
   DEBUG_LOCS   yLOG_value   ("size"      , x_node->size);
   x_prev = x_node->size;
   /*---(check for default)--------------*/
   if (a_size == -1) {
      IF_COL   a_size = DEF_WIDTH;
      ELSE_ROW a_size = DEF_HEIGHT;
   }
   /*---(check for keys)-----------------*/
   else if (a_key != 0) {
      a_size = x_node->size;
      /*---(width)-------------*/
      switch (a_key) {
      case  'm' : a_size  = 0;                           break;
      case  'n' : a_size  = DEF_WIDTH;                   break;
      case  'N' : a_size  = 12;                          break;
      case  'w' : a_size  = 20;                          break;
      case  'W' : a_size  = 50;                          break;
      case  'H' : a_size  = (((x_prev  / 5) - 1) * 5);   break;
      case  'h' : a_size -= 1;                           break;
      case  'l' : a_size += 1;                           break;
      case  'L' : a_size  = (((x_prev  / 5) + 1) * 5);   break;
      }
      /*---(height)------------*/
      switch (a_key) {
      case  'J' : a_size  = 0;                           break;
      case  'j' : a_size -= 1;                           break;
      case  'k' : a_size += 1;                           break;
      case  'K' : a_size  = 4;                           break;
      }
      /*---(done)--------------*/
   }
   DEBUG_LOCS   yLOG_value   ("a_size*"   , a_size);
   /*---(limits)-------------------------*/
   IF_COL {
      if (a_size  < MIN_WIDTH)    a_size = MIN_WIDTH;
      if (a_size  > MAX_WIDTH)    a_size = MAX_WIDTH;
   } ELSE_ROW {
      if (a_size  < MIN_HEIGHT)   a_size = MIN_HEIGHT;
      if (a_size  > MAX_HEIGHT)   a_size = MAX_HEIGHT;
   }
   DEBUG_LOCS   yLOG_value   ("a_size*"   , a_size);
   if (a_size == x_node->size) {
      DEBUG_LOCS   yLOG_note    ("already set to new size (no action)");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(update)-------------------------*/
   x_node->size = a_size;
   IF_COL   HIST_size   (a_mode, HIST_WIDTH   , a_index, a_ref, CROW, x_prev, a_size);
   ELSE_ROW HIST_size   (a_mode, HIST_HEIGHT  , a_index, CCOL, a_ref, x_prev, a_size);
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
      IF_COL   x_curr = x_curr->c_next;
      ELSE_ROW x_curr = x_curr->r_next;
   }
   /*---(reset headers)------------------*/
   yVIKEYS_map_refresh ();
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return c;
}

char NODE_resize     (char a_index, char a_type, short a_ref, char a_size) { return NODE__resize (a_index, a_type, a_ref, a_size, 0, HIST_BEG); }

char NODE_reset      (char a_index, char a_type, short a_ref) { return NODE__resize (a_index, a_type, a_ref, -1, 0, HIST_BEG); }

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
      return rce;
   }
   DEBUG_LOCS   yLOG_complex ("coord"     , "%s, %2dt, %3dc, %4dr", a_label, x_tab, x_col, x_row);
   /*---(prepare)------------------------*/
   if (a_count == 0)  a_count = 1;
   DEBUG_LOCS   yLOG_value   ("a_count"   , a_count);
   /*---(resize)-------------------------*/
   IF_COL   x_ref = x_col;
   ELSE_ROW x_ref = x_row;
   for (x_off = 0; x_off < a_count; ++x_off) {
      if (x_off == 0)  rc = NODE__resize (x_tab, a_type, x_ref + x_off, a_size, 0, HIST_BEG);
      else             rc = NODE__resize (x_tab, a_type, x_ref + x_off, a_size, 0, HIST_ADD);
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
NODE_multikey      (char a_type, char a_key)
{  /*---(design notes)-------------------*/
   /*  update all cells to new width, either a standard size, or a specific   */
   /*  value communicated as a negative number.                               */
   /*---(locals)-----------+-----------+-*/
   int         b, xb, xe, yb, ye;
   int         x_size      = 0;
   short       x_pos, x_beg, x_end;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%c, %c", a_type, a_key);
   if (a_key < 0)   {
      x_size = -a_key;
      a_key  = 0;
   }
   /*---(get coordinates)-------------*/
   yVIKEYS_visu_coords (&b, &xb, &xe, &yb, &ye, NULL);
   DEBUG_LOCS   yLOG_complex ("visual"    , "%2dt, %3d to %3dc, %4d to %4dr", b, xb, xe, yb, ye);
   IF_COL   { x_beg = xb; x_end = xe; }
   ELSE_ROW { x_beg = yb; x_end = ye; }
   DEBUG_LOCS   yLOG_complex ("range"     , "%4db..%4de", x_beg, x_end);
   for (x_pos = x_beg; x_pos <= x_end; ++x_pos) {
      if (x_pos == x_beg)  NODE__resize (b, a_type, x_pos, x_size, a_key, HIST_BEG);
      else                 NODE__resize (b, a_type, x_pos, x_size, a_key, HIST_ADD);
   }
   /*---(complete)---------------------------*/
   DEBUG_LOCS  yLOG_exit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     freezing columns                         ----===*/
/*====================------------------------------------====================*/
static void  o___FREEZING________o () { return; }

char         /*-> change the frozen cols -------------[ ------ [gc.430.323.31]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
NODE_freeze           (int a_index, char a_type, int a_beg, int a_end)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tTAB       *x_tab       = NULL;
   char        x_valid     =    0;
   int         x_temp      =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%3dt, %c, %3db, %3de", a_index, a_type, a_beg, a_end);
   /*---(defense)------------------------*/
   TAB_pointer (&x_tab, a_index);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(beginning)----------------------*/
   IF_COL   x_valid = LEGAL_COL (a_index, a_beg);
   ELSE_ROW x_valid = LEGAL_ROW (a_index, a_beg);
   DEBUG_LOCS   yLOG_value   ("x_valid"   , x_valid);
   --rce;  if (!x_valid) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(ending)-------------------------*/
   IF_COL   x_valid = LEGAL_COL (a_index, a_end);
   ELSE_ROW x_valid = LEGAL_ROW (a_index, a_end);
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
NODE_unfreeze         (int a_index, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tTAB       *x_tab       = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "%3dt, %c", a_index, a_type);
   /*---(defense)------------------------*/
   TAB_pointer (&x_tab, a_index);
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
NODE_reader          (int c, uchar *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   uchar       x_label     [LEN_LABEL];
   int         x_size      =    0;
   int         x_count     =    0;
   char        a_type      =  '-';
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(get verb)-----------------------*/
   DEBUG_INPT   yLOG_info    ("a_verb"    , a_verb);
   --rce;  if (strcmp ("width" , a_verb) == 0)   a_type = IS_COL;
   else if    (strcmp ("height", a_verb) == 0)   a_type = IS_ROW;
   else {
      DEBUG_INPT   yLOG_note    ("verb not understood");;
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(read all fields)----------------*/
   rc = yPARSE_vscanf (a_verb, x_label, &x_size, &x_count);
   DEBUG_INPT   yLOG_value   ("vscanf"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   DEBUG_INPT   yLOG_info    ("max"       , x_label);
   /*---(size)---------------------------*/
   DEBUG_INPT   yLOG_value   ("width"     , x_size);
   DEBUG_INPT   yLOG_value   ("count"     , x_count);
   /*---(resize)-------------------------*/
   IF_COL   rc = COL_multisize (x_label, x_size, x_count);
   ELSE_ROW rc = ROW_multisize (x_label, x_size, x_count);
   DEBUG_INPT   yLOG_value   ("multisize" , rc);
   if (rc < 0) {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 1;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
NODE_writer              (int c, char a_type, tNODE *a_node, short n)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_max       =    0;
   int         i           =    0;
   int         x_size      =    0;
   int         x_prev      =    0;
   char        x_label     [LEN_LABEL];
   char        x_verb      [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(verb)------------------------*/
   IF_COL   strlcpy (x_verb, "width"   , LEN_LABEL);
   ELSE_ROW strlcpy (x_verb, "height"  , LEN_LABEL);
   DEBUG_OUTP   yLOG_info    ("x_verb"    , x_verb);
   /*---(filter)----------------------*/
   x_size = a_node->size;
   DEBUG_OUTP   yLOG_value   ("x_size"    , x_size);
   /*---(write)-----------------------*/
   IF_COL   rc = str4gyges (a_node->tab, a_node->ref, 0, 0, 0, x_label, YSTR_USABLE);
   ELSE_ROW rc = str4gyges (a_node->tab, 0, a_node->ref, 0, 0, x_label, YSTR_USABLE);
   yPARSE_vprintf (c, x_verb, x_label, x_size, n);
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return n;
}

char
NODE_writer_driver       (char a_index, char a_type, short a_ref)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   char        rc2         =    0;
   tTAB       *x_tab       = NULL;
   char        x_ntab      =    0;
   tNODE      *x_prev      = NULL;
   tNODE      *x_node      = NULL;
   tNODE      *x_save      = NULL;
   short       n           =    0;
   short       c           =    0;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   DEBUG_OUTP   yLOG_complex ("args"      , "%2dt, %c, %4d", a_index, a_type, a_ref);
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(verb)---------------------------*/
   /*> IF_COL   yPARSE_verb_begin ("width");                                          <*/
   /*> ELSE_ROW yPARSE_verb_begin ("height");                                         <*/
   /*---(head)---------------------------*/
   if (a_index >= 0)  rc = TAB_by_index  (&x_tab, a_index);
   else               rc = TAB_by_cursor (&x_tab, &x_ntab, YDLST_HEAD);
   DEBUG_OUTP   yLOG_complex ("tab"       , "%3drc, %-10.10p, %2d", rc, x_tab, x_ntab);
   /*---(walk)---------------------------*/
   while (x_tab != NULL && rc == 0 && x_ntab < 36) {
      /*---(specific col/row)--*/
      if (a_ref >= 0) {
         rc = NODE_by_index  (&x_node, x_tab, a_type, a_ref);
         x_prev == x_node;
         rc2 = NODE_by_cursor (&x_prev, x_tab, a_type, '<');
         if (x_prev != NULL)  DEBUG_OUTP   yLOG_complex ("prev"      , "%3drc, %-10.10p, %2ds, %4d#, %2dn, %2dc", rc, x_prev, x_prev->size, x_prev->ref, n, c);
         if (rc2 == 0 && x_prev != NULL && x_prev != x_node && x_prev->ref + 1 == x_node->ref && x_prev->size == x_node->size) {
            DEBUG_OUTP   yLOG_note    ("x_prev matches size, so record already written");
            break;
         }
      }
      /*---(or, start at head)-*/
      else rc = NODE_by_cursor (&x_node, x_tab, a_type, '[');
      /*---(check node)--------*/
      if (x_node != NULL)  DEBUG_OUTP   yLOG_complex ("col"       , "%3drc, %-10.10p, %2ds, %4d#, %2dn, %2dc", rc, x_node, x_node->size, x_node->ref, n, c);
      else                 DEBUG_OUTP   yLOG_note    ("col is null");
      /*---(loop col/row)------*/
      while (x_node != NULL && rc == 0) {
         /*---(filter)------------*/
         IF_DEFAULT {
            rc = NODE_by_cursor (&x_node, x_tab, a_type, '>');
            continue;
         }
         /*---(prepare)-----------*/
         x_save = x_node;
         n      = 0;
         /*---(find similar)------*/
         while (x_node != NULL && rc == 0 && x_node->size == x_save->size && x_node->ref == x_save->ref + n) {
            ++n;
            rc = NODE_by_cursor (&x_node, x_tab, a_type, '>');
            if (x_node != NULL)  DEBUG_OUTP   yLOG_complex ("col"       , "%3drc, %-10.10p, %2ds, %4d#, %2dn, %2dc", rc, x_node, x_node->size, x_node->ref, n, c);
            else                 DEBUG_OUTP   yLOG_note    ("col is null");
         }
         /*---(write)-------------*/
         NODE_writer (c, a_type, x_save, n);
         ++c;
         /*> yPARSE_verb_break (c);                                                   <*/
         if (a_ref >= 0)  break;
         /*---(done-)-------------*/
      }
      /*---(next tab)----------*/
      if (a_index >= 0) break;
      rc = TAB_by_cursor (&x_tab, &x_ntab, YDLST_NEXT);
      DEBUG_OUTP   yLOG_complex ("tab"       , "%3drc, %-10.10p, %2d", rc, x_tab, x_ntab);
      /*---(done)--------------*/
   }
   /*> if (c > 0)  yPARSE_verb_end   (c);                                             <*/
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return c;
}

char NODE_writer_one (char a_index, char a_type, short a_ref) { return NODE_writer_driver (a_index, a_type, a_ref); }
char NODE_writer_all (char a_type) { return NODE_writer_driver (-1, a_type, -1); }



/*====================------------------------------------====================*/
/*===----                     yvikeys mapper                           ----===*/
/*====================------------------------------------====================*/
static void  o___MAPPER__________o () { return; }

char
NODE_map_init              (char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   tMAPPED    *x_map       = NULL;
   /*---(prepare)------------------------*/
   IF_COL   x_map   = &g_xmap;
   ELSE_ROW x_map   = &g_ymap;
   /*---(clear)--------------------------*/
   x_map->ubeg = x_map->ucur = x_map->uend  = x_map->ulen  = x_map->utend = 0;
   x_map->gbeg = x_map->gcur = x_map->gend = 0;
   /*---(complete)-----------------------*/
   return 0;
}

char
NODE_map_clear          (tMAPPED **a_map, tTAB **a_tab, int *a_max, int *a_curr, tNODE **a_node, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tMAPPED    *x_map       = NULL;
   tTAB       *x_tab       = NULL;
   tNODE      *x_node      = NULL;
   int         i           =    0;
   int         x_len       =    0;
   int         x_max       =    0;
   int         x_curr      =    0;
   int         x_opps      =    0;
   char        x_type      =  '-';
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   IF_COL   x_map   = &g_xmap;
   ELSE_ROW x_map   = &g_ymap;
   /*---(clear)--------------------------*/
   for (i= 0; i < LEN_HUGE; ++i)  x_map->map  [i] =  YVIKEYS_EMPTY;
   x_map->umin = x_map->gmin = x_map->gamin = x_map->glmin = x_map->gprev = -1;
   x_map->umax = x_map->gmax = x_map->gamax = x_map->glmax = x_map->gnext = -1;
   /*---(screen)-------------------------*/
   IF_COL   yVIKEYS_view_bounds (YVIKEYS_MAIN, NULL, NULL, &x_len, NULL, NULL, NULL);
   ELSE_ROW yVIKEYS_view_bounds (YVIKEYS_MAIN, NULL, NULL, NULL, NULL, NULL, &x_len);
   x_map->uavail = x_len;
   /*---(get tab)------------------------*/
   rc = TAB_by_index (&x_tab, CTAB);
   DEBUG_MAP    yLOG_complex ("tab"       , "%3drc, %-10.10p", rc, x_tab);
   --rce;  if (rc < 0 || x_tab == NULL) {
      DEBUG_MAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(maximum)------------------------*/
   IF_COL    x_max   = NCOL;
   ELSE_ROW  x_max   = NROW;
   DEBUG_MAP    yLOG_complex ("map"       , "%-10.10p, max %4d", x_map, x_max);
   /*---(find the opposite node)---------*/
   IF_COL   { x_type = IS_ROW; x_curr = g_xmap.gcur; x_opps = g_ymap.gcur; }
   ELSE_ROW { x_type = IS_COL; x_curr = g_ymap.gcur; x_opps = g_xmap.gcur; }
   rc = NODE_by_index (&x_node, x_tab, x_type, x_opps);
   DEBUG_MAP    yLOG_complex ("node"      , "%3drc, %-10.10p", rc, x_node);
   if (rc == 0 && x_node != NULL) {
      DEBUG_MAP    yLOG_complex ("node more" , "%c %4d %4d", x_node->type, x_node->ref, x_node->count);
   }
   /*---(clear used)---------------------*/
   for (i = 0; i <  LEN_HUGE; ++i) x_map->used [i] = '\0';
   for (i = 0; i <= x_max   ; ++i) x_map->used [i] = '·';
   /*---(save)---------------------------*/
   if (a_map   != NULL)  *a_map   = x_map;
   if (a_tab   != NULL)  *a_tab   = x_tab;
   if (a_max   != NULL)  *a_max   = x_max;
   if (a_curr  != NULL)  *a_curr  = x_curr;
   if (a_node  != NULL)  *a_node  = x_node;
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
NODE_map_mapper            (tMAPPED *a_map, tTAB *a_tab, int a_max, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tNODE      *x_node      = NULL;
   int         x_ref       =    0;
   int         x_pos       =    0;
   char        x_size      =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(big mins)--------------------*/
   a_map->umin = 0;
   a_map->gmin = 0;
   /*---(fill main map)------------------*/
   rc = NODE_by_cursor (&x_node, a_tab, a_type, '[');
   for (x_ref = 0; x_ref <= a_max; ++x_ref) {
      /*---(get size)----------*/
      if (rc != 0 || x_node == NULL) {
         IF_COL   x_size = DEF_WIDTH;
         ELSE_ROW x_size = DEF_HEIGHT;
      } else {
         if (x_ref < x_node->ref) {
            IF_COL   x_size = DEF_WIDTH;
            ELSE_ROW x_size = DEF_HEIGHT;
         } else {
            x_size = x_node->size;
            rc = NODE_by_cursor (&x_node, a_tab, a_type, '>');
         }
      }
      /*---(fill)--------------*/
      for (i = 0; i < x_size; ++i)  a_map->map  [x_pos++] = x_ref;
   }
   /*---(big maxs)--------------------*/
   a_map->gmax = a_max;
   a_map->umax = x_pos - 1;
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
NODE_map_display           (tMAPPED *a_map, char a_type, char a_section, uchar *a_out)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        i           =    0;
   char        x_off       =    0;
   char       *x_markers   = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
   char        x_max       =   80;
   /*---(break out)----------------------*/
   x_off = a_section * x_max;
   for (i = 0; i < x_max; ++i) {
      if (a_map->map [i + x_off] == YVIKEYS_EMPTY)
         a_out [i] = '·';
      else if (i > 0 && a_map->map [i + x_off] == a_map->map [i + x_off - 1])
         a_out [i] = '-';
      else if (a_map->map [i + x_off] < 62)
         a_out [i] = x_markers [a_map->map [i + x_off]];
      else
         a_out [i] = '¬';
   }
   a_out [x_max] = '\0';
   /*---(complete)-----------------------*/
   return 0;
}

char
NODE_map_used       (tMAPPED *a_map, tNODE *a_node, int a_max, int a_curr, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_cell      = NULL;
   int         n           =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "a_map %-10.10p, a_node %-10.10p, a_max %3d, a_curr %3d, type %c", a_map, a_node, a_max, a_curr, a_type);
   /*---(check node)---------------------*/
   if (a_node == NULL) {
      DEBUG_MAP    yLOG_note    ("node null, none used");
      DEBUG_MAP    yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_MAP    yLOG_complex ("node"      , "%c %4d %4d", a_node->type, a_node->ref, a_node->count);
   /*---(mark used)----------------------*/
   x_cell = a_node->n_head;
   while (rc == 0 && x_cell != NULL) {
      DEBUG_MAP    yLOG_info    ("cell"      , x_cell->label);
      IF_COL   n = x_cell->col;
      ELSE_ROW n = x_cell->row;
      a_map->used [n] = '+';
      IF_COL   x_cell = x_cell->r_next;
      ELSE_ROW x_cell = x_cell->c_next;
   }
   /*---(mark current)-------------------*/
   if (a_map->used [a_curr] == '+')  a_map->used [a_curr] = 'Ï';
   else                              a_map->used [a_curr] = 'o';
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
NODE_map_absolute          (tMAPPED *a_map, tTAB *a_tab, int a_max, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tNODE      *x_node      = NULL;
   char        x_min       =   -1;
   char        x_max       =   -1;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   a_map->gamin = -1;
   a_map->gamax = -1;
   /*---(find min)-----------------------*/
   rc = NODE_by_cursor (&x_node, a_tab, a_type, '[');
   while (rc == 0 && x_node != NULL) {
      if (x_node->count > 0) {
         x_min = x_node->ref;
         break;
      }
      rc = NODE_by_cursor (&x_node, a_tab, a_type, '>');
   }
   /*---(find max)-----------------------*/
   rc = NODE_by_cursor (&x_node, a_tab, a_type, ']');
   while (rc == 0 && x_node != NULL) {
      if (x_node->count > 0)  {
         x_max = x_node->ref;
         break;
      }
      rc = NODE_by_cursor (&x_node, a_tab, a_type, '<');
   }
   /*---(update)-------------------------*/
   a_map->gamin = x_min;
   a_map->gamax = x_max;
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
NODE_map_local             (tMAPPED *a_map, int a_max, int a_curr, tNODE *a_node, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         n           =    0;
   char        x_type      =  '-';
   int         x_curr      =    0;
   int         x_opps      =    0;
   short       x_prev      =   -1;               /* prev filled               */
   short       x_next      =   -1;               /* next filled               */
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   a_map->glmin = -1;
   a_map->gprev = -1;
   a_map->gnext = -1;
   a_map->glmax = -1;
   /*---(check node)---------------------*/
   if (a_node == NULL) {
      DEBUG_MAP    yLOG_note    ("node null, none used");
      DEBUG_MAP    yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(glmin)--------------------------*/
   DEBUG_MAP    yLOG_point   ("n_head"    , a_node->n_head);
   if (a_node->n_head != NULL) {
      IF_COL   a_map->glmin = a_node->n_head->col;
      ELSE_ROW a_map->glmin = a_node->n_head->row;
   }
   /*---(glmax)--------------------------*/
   DEBUG_MAP    yLOG_point   ("n_tail"    , a_node->n_tail);
   if (a_node->n_tail != NULL) {
      IF_COL   a_map->glmax = a_node->n_tail->col;
      ELSE_ROW a_map->glmax = a_node->n_tail->row;
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
NODE_map_char       (uchar *a_map, short a_curr)
{
   /*---(locals)-----------+-----+-----+-*/
   uchar       c           =  '·';
   uchar      *p           = NULL;
   uchar      *x_spaces    = " ·o";
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_senter  (__FUNCTION__);
   c = a_map [a_curr];
   DEBUG_MAP    yLOG_sint    (c);
   p = strchr (x_spaces, c);
   DEBUG_MAP    yLOG_spoint  (p);
   if (c == 0 || p != NULL)    c = ' ';
   else                        c = '+';
   DEBUG_MAP    yLOG_sint    (c);
   DEBUG_MAP    yLOG_sexit   (__FUNCTION__);
   return c;
}

char
NODE_map_ends       (uchar *a_map, int a_curr, int *a_prev, int *a_next)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   short       x_max       =    0;
   short       i           =    0;
   short       x_found     =    0;
   uchar       p           =  '·';
   uchar       c           =  '·';
   uchar       n           =  '·';
   /*---(prepare)------------------------*/
   if (a_prev != NULL)  *a_prev = -1;
   if (a_next != NULL)  *a_next = -1;
   /*---(max)----------------------------*/
   --rce;  if (a_map  == NULL)         return rce;
   x_max = strllen (a_map, LEN_HUGE) - 1;
   --rce;  if (x_max  <= 0   )         return rce;
   --rce;  if (a_curr <  0    )        return rce;
   --rce;  if (a_curr >  x_max)        return rce;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(prev)---------------------------*/
   DEBUG_MAP    yLOG_note    ("working previous");
   x_found = -1;
   c = NODE_map_char (a_map, a_curr);
   if (a_curr > 0) {
      p = NODE_map_char (a_map, a_curr - 1);
      if (c == ' ' || (c == '+' && p == ' ')) {
         DEBUG_MAP    yLOG_note    ("start in spaces");
         for (i = a_curr - 1; i >= 0; --i) {
            c = NODE_map_char (a_map, i);
            if (c == ' ')  continue;
            x_found = i;
            break;
         }
      } else {
         DEBUG_MAP    yLOG_note    ("start in filled");
         for (i = a_curr - 1; i >= 0; --i) {
            if (a_map [i] == '+')  continue;
            x_found = i + 1;
            break;
         }
      }
      if (x_found < 0)      x_found = 0;
      if (a_prev != NULL)   *a_prev = x_found;
   }
   /*---(prev)---------------------------*/
   DEBUG_MAP    yLOG_note    ("working next");
   x_found = -1;
   c = NODE_map_char (a_map, a_curr);
   if (a_curr < x_max) {
      n = NODE_map_char (a_map, a_curr + 1);
      if (c == ' ' || (c == '+' && n == ' ')) {
         DEBUG_MAP    yLOG_note    ("start in spaces");
         for (i = a_curr + 1; i <= x_max; ++i) {
            c = NODE_map_char (a_map, i);
            if (c == ' ')  continue;
            x_found = i;
            break;
         }
      } else {
         DEBUG_MAP    yLOG_note    ("start in filled");
         for (i = a_curr + 1; i <= x_max; ++i) {
            if (a_map [i] == '+')  continue;
            x_found = i - 1;
            break;
         }
      }
      if (x_found < 0)      x_found = x_max;
      if (a_next != NULL)  *a_next = x_found;
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}


char
NODE_map_update            (char a_type, char a_req)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   tMAPPED    *x_map       = NULL;
   tTAB       *x_tab       = NULL;
   int         x_max       =    0;
   int         x_curr      =    0;
   tNODE      *x_node      = NULL;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   if (a_req == YVIKEYS_INIT)  NODE_map_init (a_req);
   if (rc == 0)  rc = NODE_map_clear    (&x_map, &x_tab, &x_max, &x_curr, &x_node, a_type);
   if (rc == 0)  rc = NODE_map_mapper   ( x_map,  x_tab,  x_max, a_type);
   if (rc == 0)  rc = NODE_map_used     ( x_map, x_node,  x_max,  x_curr, a_type);
   if (rc == 0)  rc = NODE_map_absolute ( x_map,  x_tab,  x_max, a_type);
   if (rc == 0)  rc = NODE_map_local    ( x_map,  x_max, x_curr, x_node, a_type);
   if (rc == 0)  rc = NODE_map_ends     ( x_map->used, x_curr, &(x_map->gprev), &(x_map->gnext));
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return rc;
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
   tMAPPED    *x_map       = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(parse location)-----------------*/
   IF_COL   strlcpy (x_pre, "COL", LEN_TERSE);
   ELSE_ROW strlcpy (x_pre, "ROW", LEN_TERSE);
   IF_COL   x_map   = &g_xmap;
   ELSE_ROW x_map   = &g_ymap;
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
      if (x_tab == NULL)  snprintf(unit_answer, LEN_FULL, "%-3.3s %-7.7s  (-) : -  -=-  --    -s  -c  -f  -b  []", x_pre, a_question);
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
   else if (strcmp (a_question, "map"           ) == 0) {
      NODE_map_display (x_map, a_type, a_ref, t);
      snprintf(unit_answer, LEN_FULL, "%-3.3s map     (%2d) : %s", x_pre, a_ref, t);
   }
   else if (strcmp (a_question, "used"          ) == 0) {
      snprintf(unit_answer, LEN_FULL, "%-3.3s used         : [%.80s]", x_pre, x_map->used);
   }
   else if (strcmp (a_question, "abs"           ) == 0) {
      snprintf (unit_answer, LEN_FULL, "%-3.3s abs          : umin  %5d, gmin  %5d, gamin %5d      gamax %5d, gmax  %5d, umax  %5d", x_pre, x_map->umin, x_map->gmin, x_map->gamin, x_map->gamax, x_map->gmax, x_map->umax);
   }
   else if (strcmp (a_question, "local"         ) == 0) {
      snprintf (unit_answer, LEN_FULL, "%-3.3s local        : glmin %5d, gprev %5d                                gnext %5d, glmax %5d", x_pre, x_map->glmin, x_map->gprev, x_map->gnext, x_map->glmax);
   }
   /*---(wrap-up)------------------------*/
   s_tab  = x_tsave;
   s_curr = x_nsave;
   DEBUG_LOCS   yLOG_complex ("after"     , "s_tab %-10.10p, s_curr %-10.10p", s_tab, s_curr);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return unit_answer;
}
