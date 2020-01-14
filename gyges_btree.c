/*============================----beg-of-source---============================*/
#include   "gyges.h"


/*
 *  cells are created once, updated sometimes, and calculated frequently
 *
 *
 *
 *
 *
 *
 *
 *
 */

static tCELL   *s_broot  = NULL;
static char     s_levels =    0;

static char     s_result =    0;
static char     s_path   [LEN_DESC]  = "";
static char     s_depth  =    0;
static char     s_last   [LEN_LABEL] = "";
static tCELL   *s_save   = NULL;




/*====================------------------------------------====================*/
/*===----                        shared helpers                        ----===*/
/*====================------------------------------------====================*/
static void  o___HELPERS_________o () { return; }

long
BTREE_label2key         (char *a_label)
{
   int       x_tab, x_col, x_row;
   char      rc;
   rc = str2gyges (a_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_USABLE);
   if (rc < 0)  return -1;
   return x_row + (x_col * 10000) + (x_tab * 10000000);
}

long
BTREE_coord2key         (int a_tab, int a_col, int a_row, char *a_label)
{
   char      rc;
   rc = str4gyges (a_tab, a_col, a_row, NULL, NULL, a_label, YSTR_USABLE);
   if (rc < 0)  return -1;
   if (a_tab < 0 && a_col < 0 && a_row < 0)  return -1;
   return a_row + (a_col * 10000) + (a_tab * 10000000);
}



/*====================------------------------------------====================*/
/*===----                         sort the cells                       ----===*/
/*====================------------------------------------====================*/
static void  o___SORT____________o () { return; }

static  s_swaps      = 0;
static  s_comps      = 0;
static  s_teles      = 0;

char
BTREE__swap             (tCELL *a_one, tCELL *a_two)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(beginning)----------------------*/
   DEBUG_SORT   yLOG_senter  (__FUNCTION__);
   DEBUG_SORT   yLOG_spoint  (a_one);
   --rce;  if (a_one == NULL) {
      DEBUG_SORT   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SORT   yLOG_spoint  (a_two);
   --rce;  if (a_two == NULL) {
      DEBUG_SORT   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_one == a_two) {
      DEBUG_SORT   yLOG_snote   ("same, no action");
      DEBUG_SORT   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(cut two from list)--------------*/
   DEBUG_SORT   yLOG_snote   ("unlink");
   if (a_two->m_next != NULL)   a_two->m_next->m_prev = a_two->m_prev;
   else                         tcell                 = a_two->m_prev;
   if (a_two->m_prev != NULL)   a_two->m_prev->m_next = a_two->m_next;
   else                         hcell                 = a_two->m_next;
   /*---(insert before one)--------------*/
   DEBUG_SORT   yLOG_snote   ("insert");
   if (a_one->m_prev != NULL)   a_one->m_prev->m_next = a_two;
   else                       hcell             = a_two;
   a_two->m_prev = a_one->m_prev;
   a_two->m_next = a_one;
   a_one->m_prev = a_two;
   /*---(complete)-----------------------*/
   DEBUG_SORT   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
BTREE_dgnome            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_match     =    0;
   char        x_flag      =  '-';
   tCELL      *x_base      = NULL;          /* current position               */
   tCELL      *x_tele      = NULL;          /* teleport point                 */
   tCELL      *x_comp      = NULL;          /* comparison item                */
   char        x_swap      =  '-';
   /*---(header)-------------------------*/
   DEBUG_SORT   yLOG_enter   (__FUNCTION__);
   /*---(check list)---------------------*/
   DEBUG_SORT   yLOG_point   ("head"      , hcell);
   --rce;  if (hcell == NULL) {
      DEBUG_SORT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SORT   yLOG_point   ("->m_next"    , hcell->m_next);
   --rce;  if (hcell->m_next == NULL) {
      DEBUG_SORT   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(prepare)------------------------*/
   s_swaps = 0;
   s_comps = 0;
   s_teles = 0;
   x_base  = hcell->m_next;
   if (x_base != NULL) {
      x_comp = x_base->m_prev;
      x_tele = x_base->m_next;
   }
   /*---(do the gnome walk)--------------*/
   while (x_base != NULL) {
      DEBUG_SORT   yLOG_complex ("pointers"  , "base %-10.10p, comp %-10.10p, tele %-10.10p", x_base, x_comp, x_tele);
      /*---(beginning)-------------------*/
      if (x_comp == NULL) {
         DEBUG_SORT   yLOG_note    ("bounce off beginning");
         if (x_swap == 'y') {
            ++s_swaps;
            BTREE__swap (hcell, x_base);
            x_swap = '-';
         }
         ++s_teles;
         x_base = x_tele;
         if (x_base != NULL) {
            x_comp = x_base->m_prev;
            x_tele = x_base->m_next;
         }
         continue;
      }
      /*---(compare)---------------------*/
      DEBUG_SORT   yLOG_complex ("compare"   , "base key %10d, comp key %10d", x_base->key, x_comp->key);
      ++s_comps;
      if (x_comp->key < x_base->key) {
         x_match = -1;
         x_flag  = 'y';
      } else {
         x_match =  1;
         x_flag  = '#';
      }
      /*> x_match = strcmp (x_comp->key, x_base->key);                                          <* 
       *> x_flag  = (x_match <= 0) ? 'y' : '#';                                       <*/
      DEBUG_SORT   yLOG_complex ("compare"   , "%10d v %-10d   %c %4d   %4d %4d %4d", x_comp->key, x_base->key, x_flag, x_match, s_comps, s_teles, s_swaps);
      if (x_match <= 0) {
         if (x_swap == 'y') {
            ++s_swaps;
            BTREE__swap (x_comp->m_next, x_base);
            x_swap = '-';
         }
         ++s_teles;
         x_base = x_tele;
         if (x_base != NULL) {
            x_comp = x_base->m_prev;
            x_tele = x_base->m_next;
         }
         continue;
      }
      /*---(swap)------------------------*/
      x_swap = 'y';
      x_comp = x_comp->m_prev;
      /*---(next)------------------------*/
   }
   DEBUG_SORT   yLOG_value   ("size"       , ncell);
   DEBUG_SORT   yLOG_value   ("compares"   , s_comps);
   DEBUG_SORT   yLOG_value   ("teleports"  , s_teles);
   DEBUG_SORT   yLOG_value   ("swaps"      , s_swaps);
   /*---(complete)-----------------------*/
   DEBUG_SORT   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        build the btree                       ----===*/
/*====================------------------------------------====================*/
static void  o___BUILD___________o () { return; }

int 
BTREE__depth       (int a_size)
{
   int         c           =    0;
   if (a_size <= 0)  return 0;
   while (a_size > 1) {
      a_size /= 2;
      ++c;
   }
   ++c;
   return c;
}

int 
BTREE__span        (int a_levels)
{
   if (a_levels <= 0)  return 0;
   return pow (2, a_levels) - 1;
}

tCELL*
BTREE__nextlevel   (int n, int a_lvl, int a_pos, int a_dist, char a_dir, tCELL *a_node)
{
   /*---(locals)-----------+-----+-----+-*/
   tCELL      *x_node      = NULL;
   int         x_pos       =    0;
   int         c           =    0;
   /*---(pre-defense)--------------------*/
   if (a_dist == 0)   return NULL;
   /*---(header)-------------------------*/
   DEBUG_SORT   yLOG_enter   (__FUNCTION__);
   /*---(depth)--------------------------*/
   if (a_lvl + 1 > s_levels)  s_levels = a_lvl + 1;
   /*---(check new position)-------------*/
   x_node = a_node;
   switch (a_dir) {
   case 'L' :
      x_pos = a_pos - a_dist;
      if (x_pos <= 0) {
         DEBUG_SORT   yLOG_note    ("too far left, skip level");
         x_node = BTREE__nextlevel (n, a_lvl, a_pos, a_dist / 2, 'L', x_node);
         DEBUG_SORT   yLOG_exit    (__FUNCTION__);
         return x_node;
      }
      break;
   case 'R' :
      x_pos = a_pos + a_dist;
      if (x_pos > ncell) {
         DEBUG_SORT   yLOG_note    ("too far right, skip level");
         x_node = BTREE__nextlevel (n, a_lvl, a_pos, a_dist / 2, 'R', x_node);
         DEBUG_SORT   yLOG_exit    (__FUNCTION__);
         return x_node;
      }
      break;
   default  :
      DEBUG_SORT   yLOG_note    ("a_dir not LR, skipping");
      DEBUG_SORT   yLOG_exit    (__FUNCTION__);
      return NULL;
      break;
   }
   /*---(get to new node)----------------*/
   while (c < a_dist) {
      if (a_dir == 'L')   x_node = x_node->m_prev;
      if (a_dir == 'R')   x_node = x_node->m_next;
      ++c;
   }
   /*---(recurse)------------------------*/
   DEBUG_SORT   yLOG_complex ("a_btree"   , "%2d %c %4d %4d %d", a_lvl, a_dir, a_dist, x_pos, x_node->key);
   x_node->b_left  = BTREE__nextlevel (n, a_lvl + 1, x_pos, a_dist / 2, 'L', x_node);
   x_node->b_right = BTREE__nextlevel (n, a_lvl + 1, x_pos, a_dist / 2, 'R', x_node);
   /*---(complete)-----------------------*/
   DEBUG_SORT   yLOG_exit    (__FUNCTION__);
   return x_node;
}

char
BTREE_build             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_lvl       =    0;
   int         x_span      =    0;
   int         x_ctr       =    0;
   int         c           =    0;
   int         n           =  -1;
   /*---(header)-------------------------*/
   DEBUG_SORT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   x_lvl  = BTREE__depth (ncell);
   DEBUG_SORT   yLOG_value   ("est depth" , x_lvl);
   x_span = BTREE__span (x_lvl);
   DEBUG_SORT   yLOG_value   ("max span"  , x_span);
   DEBUG_SORT   yLOG_double  ("est usage" , ncell / 2.0 + 1);
   x_ctr  = (ncell / 2) + 1;
   DEBUG_SORT   yLOG_value   ("center"    , x_ctr);
   s_broot = hcell;
   c = 1;
   while (c < x_ctr) {
      s_broot = s_broot->m_next;
      ++c;
   }
   DEBUG_SORT   yLOG_point   ("ROOT"      , s_broot);
   s_broot->b_left  = BTREE__nextlevel (n, 1, x_ctr, x_span / 4 + 1, 'L', s_broot);
   s_broot->b_right = BTREE__nextlevel (n, 1, x_ctr, x_span / 4 + 1, 'R', s_broot);
   DEBUG_SORT   yLOG_value   ("real depth", s_levels);
   /*---(complete)-----------------------*/
   DEBUG_SORT   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        searching the btree                   ----===*/
/*====================------------------------------------====================*/
static void  o___SEARCH__________o () { return; }

char
BTREE__searchdown       (tCELL **a_found, tCELL *a_node, char *a_dir, long a_key)
{
   /*---(locals)-----------+-----+-----+-*/
   int         rc          =    0;
   /*---(defense)------------------------*/
   if (a_node == NULL)  return -1;
   /*---(prepare)------------------------*/
   ++s_depth;
   strlcat (s_path, a_dir, LEN_DESC);
   /*---(compare)------------------------*/
   if      (a_node->key == a_key)  rc =  0;
   else if (a_node->key >  a_key)  rc =  1;
   else                            rc = -1;
   DEBUG_SORT   yLOG_complex ("compare"   , "%s  %-20d  %4d", a_dir, a_node->key, rc);
   /*---(no match)-----------------------*/
   if (rc >  0)  return BTREE__searchdown (a_found, a_node->b_left , "L", a_key);
   if (rc <  0)  return BTREE__searchdown (a_found, a_node->b_right, "R", a_key);
   /*---(match)--------------------------*/
   *a_found = a_node;
   return 0;
}

char
BTREE_by_label          (tCELL **a_found, char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_node      = NULL;
   long        x_key       =   -1;
   /*---(header)-------------------------*/
   DEBUG_DATA   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   s_depth = 0;
   strlcpy (s_path, "", LEN_DESC);
   if (a_found != NULL)  *a_found = NULL;
   /*---(short-cut)----------------------*/
   if (s_save != NULL && strcmp (s_last, a_label) == 0) {
      DEBUG_DATA   yLOG_note    ("shortcut");
      s_result =  1;
      strlcpy (s_path, "-", LEN_DESC);
      if (a_found != NULL)  *a_found = s_save;
      DEBUG_DATA   yLOG_exit    (__FUNCTION__);
      return 1;
   }
   x_key   = BTREE_label2key (a_label);
   DEBUG_DATA   yLOG_value   ("x_key"     , x_key);
   --rce;  if (x_key < 0) {
      DEBUG_DATA   yLOG_note    ("bad label");
      s_result = -1;
      DEBUG_DATA   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(search)-------------------------*/
   DEBUG_DATA   yLOG_note    ("dive into btree");
   rc = BTREE__searchdown (&x_node, s_broot, "@", x_key);
   DEBUG_DATA   yLOG_value   ("max depth" , s_levels);
   DEBUG_DATA   yLOG_value   ("s_depth"   , s_depth);
   DEBUG_DATA   yLOG_info    ("s_path"    , s_path);
   /*---(check)--------------------------*/
   --rce;  if (x_node == NULL) {
      DEBUG_DATA   yLOG_note    ("not found");
      s_result = -1;
      DEBUG_DATA   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(return)-------------------------*/
   DEBUG_DATA   yLOG_note    ("found");
   if (a_found != NULL)  *a_found = x_node;
   strlcpy (s_last, a_label, LEN_LABEL);
   s_save   = x_node;
   s_result = 0;
   /*---(complete)-----------------------*/
   DEBUG_DATA   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
BTREE_by_coord          (tCELL **a_found, int a_tab, int a_col, int a_row)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_node      = NULL;
   char        x_label     [LEN_LABEL] = "";
   long        x_key       =   -1;
   /*---(header)-------------------------*/
   DEBUG_DATA   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   s_depth = 0;
   strlcpy (s_path, "", LEN_DESC);
   if (a_found != NULL)  *a_found = NULL;
   /*---(short-cut)----------------------*/
   x_key   = BTREE_coord2key (a_tab, a_col, a_row, x_label);
   DEBUG_DATA   yLOG_value   ("x_key"     , x_key);
   --rce;  if (x_key < 0) {
      DEBUG_DATA   yLOG_note    ("bad coordinates");
      s_result = -1;
      DEBUG_DATA   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (s_save != NULL && strcmp (s_last, x_label) == 0) {
      DEBUG_DATA   yLOG_note    ("shortcut");
      s_result =  1;
      strlcpy (s_path, "-", LEN_DESC);
      if (a_found != NULL)  *a_found = s_save;
      DEBUG_DATA   yLOG_exit    (__FUNCTION__);
      return 1;
   }
   /*---(search)-------------------------*/
   DEBUG_DATA   yLOG_note    ("dive into btree");
   rc = BTREE__searchdown (&x_node, s_broot, "@", x_key);
   DEBUG_DATA   yLOG_value   ("max depth" , s_levels);
   DEBUG_DATA   yLOG_value   ("s_depth"   , s_depth);
   DEBUG_DATA   yLOG_info    ("s_path"    , s_path);
   /*---(check)--------------------------*/
   --rce;  if (x_node == NULL) {
      DEBUG_DATA   yLOG_note    ("not found");
      s_result = -1;
      DEBUG_DATA   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   DEBUG_DATA   yLOG_note    ("found");
   if (a_found != NULL)  *a_found = x_node;
   strlcpy (s_last, x_label, LEN_LABEL);
   s_save   = x_node;
   s_result = 0;
   /*---(complete)-----------------------*/
   DEBUG_DATA   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        reporting out                         ----===*/
/*====================------------------------------------====================*/
static void  o___REPORT__________o () { return; }

char
BTREE__display        (int a_lvl, tCELL *a_node)
{
   char        x_pre       [LEN_RECD] = "";
   int         i           =    0;
   if (a_node == NULL)  return 0;
   if (a_lvl > 20)      return 0;
   for (i = 0; i < a_lvl; ++i)  strcat (x_pre, "  ");
   BTREE__display      (a_lvl + 1, a_node->b_left);
   printf ("%s%d\n", x_pre, a_node->key);
   BTREE__display      (a_lvl + 1, a_node->b_right);
   return 0;
}

char
BTREE_list              (void)
{
   BTREE__display      (0, s_broot);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        simple driver                         ----===*/
/*====================------------------------------------====================*/
static void  o___DRIVER__________o () { return; }

char
BTREE_update            (void)
{
   char        rc          =    0;
   if (my.btree != 'y')  return 0;
   if (rc == 0)  rc = BTREE_dgnome  ();
   if (rc == 0)  rc = BTREE_build   ();
   return rc;
}


/*====================------------------------------------====================*/
/*===----                      unit testing                            ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }

char*        /*-> unit test accessor -----------------[ light  [us.B60.2A3.F2]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
BTREE__unit        (char *a_question, int n)
{
   /*---(locals)-------------------------*/
   tCELL      *x_curr      = NULL;
   int         c           =    0;
   char        t           [LEN_LABEL] = "";
   /*---(parse location)-----------------*/
   strcpy  (unit_answer, "BTREE            : label could not be parsed");
   /*---(overall)------------------------*/
   if      (strcmp (a_question, "entry"         ) == 0) {
      x_curr = hcell;
      DEBUG_HIST   yLOG_value   ("n"         , n);
      while (x_curr != NULL) {
         /*> DEBUG_HIST   yLOG_complex ("entry"     , "%2d %-10.10s %10p %10p %10p", c, x_curr->text, x_curr, x_curr->m_next, x_curr->m_prev);   <*/
         /*> printf ("entry  %2d %-10.10s %10p %10p\n", c, x_curr->text, x_curr, x_curr->m_next);   <*/
         if (c == n) {
            /*> printf ("found\n");                                                   <*/
            /*> DEBUG_HIST   yLOG_point   ("found"     , x_curr);                     <*/
            break;
         }
         x_curr = x_curr->m_next;
         ++c;
      }
      /*> printf ("checking...%p\n", x_curr);                                         <*/
      /*> DEBUG_HIST   yLOG_point   ("found"     , x_curr);                           <*/
      if (x_curr == NULL)  snprintf (unit_answer, LEN_FULL, "BTREE entry (%2d) : -          -t   -c   -r  -", n);
      else {
         strl4comma (x_curr->key, t, 0, 'c', '-', LEN_LABEL);
         snprintf (unit_answer, LEN_FULL, "BTREE entry (%2d) : %-8.8s %3dt %3dc %3dr  %s", n, x_curr->label, x_curr->tab, x_curr->col, x_curr->row, t);
      }
   }
   else if (strcmp (a_question, "result"        ) == 0) {
      snprintf (unit_answer, LEN_FULL, "BTREE result     : %2d %-8.8s  %2d %s", s_result, s_last, s_depth, s_path);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}
