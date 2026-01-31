/*============================----beg-of-source---============================*/
#include   "gyges.h"



/*===[[ GNU GENERAL PUBLIC LICENSE (GPL) ]]===================================*/
/*´´·········1·········2·········3·········4·········5·········6·········7·········8  */

#define  P_COPYRIGHT   \
   "copyright (c) 2010 robert.s.heatherly at balsashrike at gmail dot com"

#define  P_LICENSE     \
   "the only place you could have gotten this code is my github, my website,¦"   \
   "or illegal sharing. given that, you should be aware that this is GPL licensed."

#define  P_COPYLEFT    \
   "the GPL COPYLEFT REQUIREMENT means any modifications or derivative works¦"   \
   "must be released under the same GPL license, i.e, must be free and open."

#define  P_INCLUDE     \
   "the GPL DOCUMENTATION REQUIREMENT means that you must include the original¦" \
   "copyright notice and the full licence text with any resulting anything."

#define  P_AS_IS       \
   "the GPL NO WARRANTY CLAUSE means the software is provided without any¦"      \
   "warranty and the author cannot be held liable for damages."

#define  P_THEFT    \
   "if you knowingly violate the spirit of these ideas, i suspect you might "    \
   "find any number of freedom-minded hackers may take it quite personally ;)"

/*´´·········1·········2·········3·········4·········5·········6·········7·········8  */
/*===[[ GNU GENERAL PUBLIC LICENSE (GPL) ]]===================================*/


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
api_ysort_label2key     (char *a_label)
{
   int       x_tab, x_col, x_row;
   char      rc;
   rc = ystr2gyges (a_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_USABLE);
   if (rc < 0)  return -1;
   return x_row + (x_col * 10000) + (x_tab * 10000000);
}

long
api_ysort_coord2key     (int a_tab, int a_col, int a_row, char *a_label)
{
   char      rc;
   rc = ystr4gyges (a_tab, a_col, a_row, NULL, NULL, a_label, YSTR_USABLE);
   if (rc < 0)  return -1;
   if (a_tab < 0 && a_col < 0 && a_row < 0)  return -1;
   return a_row + (a_col * 10000) + (a_tab * 10000000);
}



/*====================------------------------------------====================*/
/*===----                         sort the cells                       ----===*/
/*====================------------------------------------====================*/
static void  o___YSORT___________o () { return; }

char
api_ysort__common       (char a_type, void *a_one  , void *a_two)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YSORT   yLOG_senter  (__FUNCTION__);
   /*---(check pointers)-----------------*/
   DEBUG_YSORT   yLOG_spoint  (a_one);
   --rce;  if (a_one == NULL && a_type != 'i') {
      return rce;
   }
   DEBUG_YSORT   yLOG_spoint  (a_two);
   --rce;  if (a_two == NULL) {
      return rce;
   }
   --rce;  if (a_one == a_two) {
      DEBUG_YSORT   yLOG_snote   ("same, no action");
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YSORT   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
api_ysort__cursor       (uchar a_type, void *a_head, void *a_tail, void *a_beg, void **a_new, char a_action)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   tCELL      *x_beg       = NULL;
   tCELL      *x_new       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YSORT   yLOG_enter   (__FUNCTION__);
   /*---(check pointers)-----------------*/
   DEBUG_YSORT   yLOG_point   ("a_beg"     , a_beg);
   DEBUG_YSORT   yLOG_point   ("a_new"     , a_new);
   --rce;  if (a_new == NULL) {
      DEBUG_YSORT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_new = (tCELL *) a_head;
   /*---(prepare)------------------------*/
   DEBUG_YSORT   yLOG_note    ("cast");
   if (a_beg != NULL) {
      x_beg    = (tCELL *) a_beg;
      DEBUG_YSORT   yLOG_info    ("x_beg"     , x_beg->d_label);
   }
   /*---(update)-------------------------*/
   DEBUG_YSORT   yLOG_char    ("a_action"  , a_action);
   switch (a_action) {
   case YDLST_HEAD : x_new = (tCELL *) a_head;                   break;
   case YDLST_PREV : if (x_beg != NULL)  x_new = x_beg->d_prev;  break;
   case YDLST_NEXT : if (x_beg != NULL)  x_new = x_beg->d_next;  break;
   case YDLST_TAIL : x_new = (tCELL *) a_tail;                   break;
   }
   /*---(save back)----------------------*/
   if (x_new != NULL) {
      DEBUG_YSORT   yLOG_note    (x_new->d_label);
   }
   *a_new = x_new;
   /*---(complete)-----------------------*/
   DEBUG_YSORT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_ysort__checker      (uchar a_type, uchar a_lvl, void *a_one, void *a_two, uchar a_order)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_one       = NULL;
   tCELL      *x_two       = NULL;
   long        x_key       =   -1;
   /*---(header)-------------------------*/
   DEBUG_YSORT   yLOG_enter   (__FUNCTION__);
   DEBUG_YSORT   yLOG_complex ("args"      , "%c, %2d, %p, %p, %c", a_type, a_lvl, a_one, a_two, a_order);
   /*---(defense)------------------------*/
   rc = api_ysort__common ('c', a_one, a_two);
   --rce;  if (rc < 0) {
      DEBUG_YSORT   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(prepare)------------------------*/
   x_one    = (tCELL *) a_one;
   /*---(handle object)------------------*/
   if (a_type == tolower (a_type)) {
      DEBUG_YSORT   yLOG_note    ("normal, sorting compare");
      x_two    = (tCELL *) a_two;
      x_key    = x_two->d_key;
   } else {
      DEBUG_YSORT   yLOG_note    ("search compare to value");
      x_key = * ((long *) a_two);
   }
   DEBUG_YSORT   yLOG_value   ("x_one->d_key", x_one->d_key);
   DEBUG_YSORT   yLOG_value   ("x_key"     , x_key);
   if      (x_one->d_key <  x_key)  rc = -1;
   else if (x_one->d_key >  x_key)  rc =  1;
   else if (x_one->d_key == x_key)  rc =  0;
   DEBUG_YSORT   yLOG_value   ("compare"   , rc);
   /*---(complete)-----------------------*/
   DEBUG_YSORT   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
api_ysort__unlinker     (uchar a_type, void **a_head, void **a_tail, void *a_two)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_head      = NULL;
   tCELL      *x_tail      = NULL;
   tCELL      *x_two       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YSORT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   rc = api_ysort__common ('i', NULL, a_two);
   --rce;  if (rc < 0) {
      DEBUG_YSORT   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(prepare)------------------------*/
   x_head   = * ((tCELL **) a_head);
   x_tail   = * ((tCELL **) a_tail);
   DEBUG_YSORT   yLOG_complex ("pointers"  , "head %p, tail %p", x_head, x_tail);
   x_two    = (tCELL *) a_two;
   /*---(unlink current from list)-------*/
   DEBUG_YSORT   yLOG_note    ("unlink");
   if (x_two->d_next != NULL)   x_two->d_next->d_prev = x_two->d_prev;
   else                         x_tail                = x_two->d_prev;
   if (x_two->d_prev != NULL)   x_two->d_prev->d_next = x_two->d_next;
   else                         x_head                = x_two->d_next;
   /*---(ground pointers)----------------*/
   DEBUG_YSORT   yLOG_note    ("ground pointers");
   x_two->d_next = NULL;
   x_two->d_prev = NULL;
   /*---(save back)----------------------*/
   DEBUG_YSORT   yLOG_note    ("save back");
   *a_head = x_head;
   *a_tail = x_tail;
   /*---(complete)-----------------------*/
   DEBUG_YSORT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_ysort__linker       (uchar a_type, void **a_head, void **a_tail, void *a_one, void *a_two)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_head      = NULL;
   tCELL      *x_tail      = NULL;
   tCELL      *x_one       = NULL;
   tCELL      *x_two       = NULL;
   /*---(header)-------------------------*/
   DEBUG_YSORT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   rc = api_ysort__common ('i', a_one, a_two);
   --rce;  if (rc < 0) {
      DEBUG_YSORT   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(prepare)------------------------*/
   x_head   = * ((tCELL **) a_head);
   x_tail   = * ((tCELL **) a_tail);
   x_one    = (tCELL *) a_one;
   x_two    = (tCELL *) a_two;
   /*---(insert back in)-----------------*/
   if (a_one == NULL) {
      if (x_head == NULL) {
         DEBUG_YSORT   yLOG_note    ("add first");
         x_head         = x_two;
         x_two->d_prev  = NULL;
      } else {
         DEBUG_YSORT   yLOG_note    ("append to tail");
         x_tail->d_next = x_two;
         x_two->d_prev  = x_tail;
      }
      x_tail       = x_two;
      x_two->d_next    = NULL;
   } else {
      DEBUG_YSORT   yLOG_note    ("insert before");
      if (x_one->d_prev != NULL)   x_one->d_prev->d_next = x_two;
      else                         x_head                = x_two;
      x_two->d_prev   = x_one->d_prev;
      x_two->d_next   = x_one;
      x_one->d_prev   = x_two;
   }
   /*---(save back)----------------------*/
   DEBUG_YSORT   yLOG_note    ("save back");
   *a_head = x_head;
   *a_tail = x_tail;
   /*---(output)-------------------------*/
   DEBUG_YSORT   yLOG_exit    (__FUNCTION__);
   /*---(complete)-----------------------*/
   return 0;
}

char
api_ysort__forker       (uchar a_type, void *a_node, void **a_left, void **a_right)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   tCELL      *x_left      = NULL;
   tCELL      *x_right     = NULL;
   /*---(header)-------------------------*/
   DEBUG_YSORT   yLOG_enter   (__FUNCTION__);
   DEBUG_YSORT   yLOG_complex ("args"      , "%c, %p, %p, %p", a_type, a_node, a_left, a_right);
   /*---(prepare)------------------------*/
   x_curr   = (tCELL *) a_node;
   DEBUG_YSORT   yLOG_point   ("x_curr"    , x_curr);
   DEBUG_YSORT   yLOG_info    ("label"     , x_curr->d_label);
   /*---(set)----------------------------*/
   if (a_type == tolower (a_type)) {
      DEBUG_YSORT   yLOG_note    ("saving run");
      x_left  = * ((tCELL **) a_left);
      x_right = * ((tCELL **) a_right);
      if (a_left  != NULL)  x_curr->b_left  = x_left;
      if (a_right != NULL)  x_curr->b_right = x_right;
   }
   /*---(get)----------------------------*/
   else {
      if (a_left  != NULL)  *a_left  = x_curr->b_left;
      if (a_right != NULL)  *a_right = x_curr->b_right;
   }
   /*---(output)-------------------------*/
   DEBUG_YSORT   yLOG_exit    (__FUNCTION__);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        searching the btree                   ----===*/
/*====================------------------------------------====================*/
static void  o___SEARCH__________o () { return; }

char
api_ysort_by_label      (tCELL **a_found, char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   void       *x_node      = NULL;
   long        x_key       =   -1;
   /*---(header)-------------------------*/
   DEBUG_YSORT   yLOG_enter   (__FUNCTION__);
   x_key   = api_ysort_label2key (a_label);
   DEBUG_YSORT   yLOG_value   ("x_key"     , x_key);
   --rce;  if (x_key < 0) {
      DEBUG_YSORT   yLOG_note    ("bad label");
      s_result = -1;
      DEBUG_YSORT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(search)-------------------------*/
   DEBUG_YSORT   yLOG_note    ("dive into btree");
   rc = ySORT_search (YSORT_NONE, rcell, &x_key, &x_node);
   /*---(return)-------------------------*/
   DEBUG_YSORT   yLOG_note    ("found");
   if (a_found != NULL)  *a_found = (tCELL *) x_node;
   /*---(complete)-----------------------*/
   DEBUG_YSORT   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
api_ysort_by_coord      (tCELL **a_found, int a_tab, int a_col, int a_row)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   void       *x_node      = NULL;
   char        x_label     [LEN_LABEL] = "";
   long        x_key       =   -1;
   /*---(header)-------------------------*/
   DEBUG_YSORT   yLOG_enter   (__FUNCTION__);
   /*---(short-cut)----------------------*/
   x_key   = api_ysort_coord2key (a_tab, a_col, a_row, x_label);
   DEBUG_YSORT   yLOG_value   ("x_key"     , x_key);
   --rce;  if (x_key < 0) {
      DEBUG_YSORT   yLOG_note    ("bad coordinates");
      s_result = -1;
      DEBUG_YSORT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(search)-------------------------*/
   DEBUG_YSORT   yLOG_note    ("dive into btree");
   rc = ySORT_search (YSORT_NONE, rcell, &x_key, &x_node);
   /*---(return)-------------------------*/
   DEBUG_YSORT   yLOG_note    ("found");
   if (a_found != NULL)  *a_found = (tCELL *) x_node;
   /*---(complete)-----------------------*/
   DEBUG_YSORT   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                        reporting out                         ----===*/
/*====================------------------------------------====================*/
static void  o___REPORT__________o () { return; }

static FILE *s_file    = NULL;
llong        s_prev    = -999;

char
api_ysort_dumper        (uchar a_type, int a_lvl, tCELL *a_node, char *a_path, char *a_pre)
{
   char        x_pre       [LEN_RECD] = "";
   int         i           =    0;
   int         x_len       =    0;
   if (a_node == NULL)  return 0;
   /*---(prefix and label)---------------*/
   x_len = 10 - strlen (a_node->d_label);
   fprintf (s_file, "%s%s%*.*s", a_pre, a_node->d_label, x_len, x_len, "··········");
   /*---(spacer and suffix)--------------*/
   ystrlcpy (x_pre, "", LEN_RECD);
      for (i = 15; i > a_lvl; --i)  strcat (x_pre, "···");
      -printf (s_file, "%s %2d %-20.20s %2dt, %3dc, %4dr, %20lld", x_pre, a_lvl, a_path, a_node->d_tab, a_node->d_col, a_node->d_row, a_node->d_key);
   if      (a_node->d_key < 0)         fprintf (s_file, "   buffer/register");
   else if (s_prev == a_node->d_key)   fprintf (s_file, "   DUP");
   else                              fprintf (s_file, "   -");
   s_prev = a_node->d_key;
   fprintf (s_file, "\n");
   return 0;
}

char
api_ysort_btree_dump    (FILE *a_file)
{
   /*---(locals)-----------+-----+-----+-*/
   int         c           =    0;
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   s_file = a_file;
   s_prev = -999;
   fprintf (s_file, "yVIKEYS, btree cell structure                                                                  (:dump btree)\n");
   fprintf (s_file, "count = %d all, %d linked\n", ACEL, NCEL);
   ySORT_walk  (YSORT_NONE, rcell, api_ysort_dumper);
   fprintf (s_file, "\n");
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        simple driver                         ----===*/
/*====================------------------------------------====================*/
static void  o___DRIVER__________o () { return; }

char
api_ysort_update        (void)
{
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YSORT   yLOG_enter   (__FUNCTION__);
   DEBUG_YSORT   yLOG_char    ("my.btree"  , my.btree);
   if (my.btree != 'y') {
      DEBUG_YSORT   yLOG_note    ("nothing to do");
      DEBUG_YSORT   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (rc == 0) {
      rc = ySORT_gnome   ('-', YSORT_ASCEND, &hcell, &tcell);
      DEBUG_YSORT   yLOG_value   ("gnome"     , rc);
   }
   if (rc == 0) {
      rc = ySORT_treeify ('-', hcell, tcell, ncell, &rcell);
      DEBUG_YSORT   yLOG_value   ("treeify"   , rc);
   }
   DEBUG_YSORT   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                       program level                          ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
api_ysort_init          (void)
{
   rcell     = NULL;
   s_levels  = 0;
   ySORT_config  (YSORT_GNOME, api_ysort__cursor, api_ysort__checker, api_ysort__unlinker, api_ysort__linker, NULL, NULL, api_ysort__forker);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      unit testing                            ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }

char*        /*-> unit test accessor -----------------[ light  [us.B60.2A3.F2]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
api_ysort__unit         (char *a_question, int n)
{
   /*---(locals)-------------------------*/
   tCELL      *x_curr      = NULL;
   int         c           =    0;
   int         x_buckets   = 0;
   int         x_slots     = 0;
   int         x_max       = 0;
   int         x_sorts     = 0;
   int         x_comps     = 0;
   int         x_swaps     = 0;
   int         x_teles     = 0;
   int         x_result    = 0;
   void       *x_void      = NULL;
   tCELL      *x_last      = NULL;
   int         x_depth     = 0;
   char        x_path      [LEN_LABEL] = "";
   char        t           [LEN_LABEL] = "";
   /*---(parse location)-----------------*/
   strcpy  (unit_answer, "YSORT            : label could not be parsed");
   /*---(overall)------------------------*/
   if      (strcmp (a_question, "gnome"         ) == 0) {
      ySORT_gnome_stats (&x_sorts, &x_comps, &x_swaps, &x_teles);
      snprintf (unit_answer, LEN_FULL, "YSORT gnome      : %4d#, %4dc, %4ds, %4dt", x_sorts, x_comps, x_swaps, x_teles);
   }
   else if (strcmp (a_question, "troll"         ) == 0) {
      ySORT_troll_stats (&x_buckets, &x_slots, &x_max, &x_sorts, &x_comps, &x_swaps, &x_teles);
      snprintf (unit_answer, LEN_FULL, "YSORT troll      : %2db, %4ds, %2dx, %4d#, %4dc, %4ds, %4dt", x_buckets, x_slots, x_max, x_sorts, x_comps, x_swaps, x_teles);
   }
   else if (strcmp (a_question, "treeify"       ) == 0) {
      /*> ySORT_treeify_stats (&x_sorts, &x_comps, &x_swaps, &x_teles);                                                                        <* 
       *> snprintf (unit_answer, LEN_FULL, "YSORT treeify    : %4d#, %4dl, %4dc, %4ds, %4dt", x_sorts, x_levels, x_comps, x_swaps, x_teles);   <*/
   }
   else if (strcmp (a_question, "result"        ) == 0) {
      /*> ySORT_search_stats (&x_result, &x_last, &x_depth, x_path);                  <*/
      ySORT_search_stats (&x_result, &x_last   , &x_depth, x_path);
      /*> x_last = (tCELL *) x_void;                                                  <*/
      if (x_last != NULL)  ystrlcpy (t, x_last->d_label, LEN_LABEL);
      else                 ystrlcpy (t, "-"            , LEN_LABEL);
      snprintf (unit_answer, LEN_FULL, "YSORT result     : %2d %-8.8s  %2d %s", x_result, t, x_depth, x_path);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}
