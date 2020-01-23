/*============================----beg-of-source---============================*/
#include   "gyges.h"


/*
 *  used a s_master array rather than another data structure because the max
 *  count is fixed and small.  a linked list would cost space and complexity.
 *  
 *
 *
 *
 */


static  uchar  *s_valids  = NULL;
static  int     s_nvalid  = 38;

static  tTAB   *s_master  [MAX_TABS];
static  tTAB   *s_curr    = NULL;
static  char    s_all     = 0;
static  char    s_count   = 0;
static  char    s_index   = -1;

tTAB    s_grounded;



/*====================------------------------------------====================*/
/*===----                       shared helpers                         ----===*/
/*====================------------------------------------====================*/
static void  o___HELPERS_________o () { return; }

char
TAB__name_check         (char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_len       =    0;
   char        i           =    0;
   char       *x_valid     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._";
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_senter  (__FUNCTION__);
   DEBUG_LOCS   yLOG_spoint  (a_name);
   --rce;  if (a_name == NULL || a_name [0] == '\0') {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_snote   (a_name);
   x_len = strlen (a_name);
   DEBUG_LOCS   yLOG_sint    (x_len);
   --rce;  if (x_len < 3 || x_len > 18) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   for (i = 0; i < x_len; ++i) {
      if (strchr (x_valid, a_name [i]) != NULL)  continue;
      DEBUG_LOCS   yLOG_sint    (i);
      DEBUG_LOCS   yLOG_sint    (a_name [i]);
      DEBUG_LOCS   yLOG_schar   (a_name [i]);
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
TAB_ground              (void)
{
   CTAB      = -1;
   my.ptab   = &s_grounded;
   PTAB      = &s_grounded;
   /*---(cols)---------*/
   NCOL      = PTAB->ncol       = 0;
   CCOL      = PTAB->ccol       = 0;
   BCOL      = PTAB->bcol       = 0;
   ECOL      = PTAB->ecol       = 0;
   FR_COL    = PTAB->froz_col   = '-';
   FR_BCOL   = PTAB->froz_bcol  = 0;
   FR_ECOL   = PTAB->froz_ecol  = 0;
   /*---(rows)---------*/
   NROW      = PTAB->nrow       = 0;
   CROW      = PTAB->crow       = 0;
   BROW      = PTAB->brow       = 0;
   EROW      = PTAB->erow       = 0;
   FR_ROW    = PTAB->froz_row   = '-';
   FR_BROW   = PTAB->froz_brow  = 0;
   FR_EROW   = PTAB->froz_erow  = 0;
   return 0;
}

char
TAB_retrieve            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(restore values)-----------------*/
   DEBUG_LOCS   yLOG_value   ("CTAB"      , CTAB);
   --rce;  if (CTAB < 0 || !TAB_live (CTAB)) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(switch tab)---------------------*/
   p_tab     = s_master [CTAB];
   PTAB      = s_master [CTAB];
   /*---(cols)---------*/
   NCOL      = PTAB->ncol;
   CCOL      = PTAB->ccol;
   BCOL      = PTAB->bcol;
   ECOL      = PTAB->ecol;
   FR_COL    = PTAB->froz_col;
   FR_BCOL   = PTAB->froz_bcol;
   FR_ECOL   = PTAB->froz_ecol;
   /*---(rows)---------*/
   NROW      = PTAB->nrow;
   CROW      = PTAB->crow;
   BROW      = PTAB->brow;
   EROW      = PTAB->erow;
   FR_ROW    = PTAB->froz_row;
   FR_BROW   = PTAB->froz_brow;
   FR_EROW   = PTAB->froz_erow;
   /*---(reposition)---------------------*/
   g_bmap.gcur = CTAB;
   g_xmap.gcur = CCOL;
   g_ymap.gcur = CROW;
   g_bmap.gcur = CTAB;
   g_xmap.gcur = 0;
   g_ymap.gcur = 0;
   g_zmap.gcur = 0;
   MAP_mapper (YVIKEYS_UPDATE);
   /*> yVIKEYS_jump (CTAB, CCOL, CROW, 0);                                            <*/
   yVIKEYS_jump (CTAB, 0, 0, 0);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
TAB_save               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(save values)--------------------*/
   DEBUG_LOCS   yLOG_point   ("PTAB"      , PTAB);
   --rce;  if (PTAB == NULL || PTAB == &s_grounded) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_value   ("CTAB"      , CTAB);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       memory allocation                      ----===*/
/*====================------------------------------------====================*/
static void  o___MEMORY__________o () { return; }

char 
TAB_new                 (tTAB **a_new, char a_index, uchar *a_name, uchar *a_size)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tTAB       *x_new       = NULL;
   char        x_tries     =    0;
   int         x_len       =    0;
   uchar       x_abbr      =  '-';
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_LOCS   yLOG_value   ("a_index"   , a_index);
   --rce;  if (!VALID_tab (a_index)) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_abbr = LABEL_tab (a_index);
   DEBUG_LOCS   yLOG_char    ("x_abbr"    , x_abbr);
   DEBUG_LOCS   yLOG_point   ("s_master"  , s_master [a_index]);
   if (s_master [a_index] != NULL) {
      DEBUG_LOCS   yLOG_note    ("tab already exists, no worries");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return 1;
   }
   /*---(allocate)-----------------------*/
   while (x_new == NULL && x_tries < 10)  {
      ++x_tries;
      x_new = (tTAB *) malloc (sizeof (tTAB));
   }
   DEBUG_LOCS   yLOG_value   ("x_tries"   , x_tries);
   DEBUG_LOCS   yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_LOCS   yLOG_note    ("FAILED");
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(populate)-----------------------*/
   DEBUG_LOCS   yLOG_note    ("populate");
   x_new->tab      = a_index;
   x_new->abbr     = x_abbr;
   x_new->name     = g_tbd;
   x_new->type     = G_TAB_NORMAL;
   /*---(tie to master list)-------------*/
   DEBUG_LOCS   yLOG_note    ("attach");
   s_master [a_index] = x_new;
   /*---(columns)------------------------*/
   DEBUG_LOCS   yLOG_note    ("clear column data");
   x_new->C_head    = NULL;
   x_new->C_tail    = NULL;
   x_new->C_count   = 0;
   x_new->ncol      = 0;
   x_new->bcol      = 0;
   x_new->ccol      = 0;
   x_new->ecol      = 0;
   x_new->froz_col  = '-';
   x_new->froz_bcol = 0;
   x_new->froz_ecol = 0;
   /*---(rows)---------------------------*/
   DEBUG_LOCS   yLOG_note    ("clear row data");
   x_new->R_head    = NULL;
   x_new->R_tail    = NULL;
   x_new->R_count   = 0;
   x_new->nrow      = 0;
   x_new->brow      = 0;
   x_new->crow      = 0;
   x_new->erow      = 0;
   x_new->froz_row  = '-';
   x_new->froz_brow = 0;
   x_new->froz_erow = 0;
   /*---(update counts)------------------*/
   DEBUG_LOCS   yLOG_note    ("update counts");
   x_new->count     = 0;
   ++s_all;
   if (strchr ("®¯", s_valids [a_index]) == NULL)   ++s_count;
   DEBUG_LOCS   yLOG_value   ("s_all"     , s_all);
   DEBUG_LOCS   yLOG_value   ("s_count"   , s_count);
   /*---(update current)-----------------*/
   s_curr  = s_master [a_index];
   s_index = a_index;
   DEBUG_LOCS   yLOG_point   ("s_curr"    , s_curr);
   DEBUG_LOCS   yLOG_value   ("s_index "  , s_index);
   /*---(rename)-------------------------*/
   TAB_rename (a_index, a_name);
   TAB_retype (a_index, G_TAB_NORMAL);
   if (a_size == NULL)  TAB_resize (a_index, "z100");
   else   {
      rc = TAB_resize (a_index, a_size);
      if (rc < 0)  TAB_resize (a_index, "z100");
   }
   /*---(focus)--------------------------*/
   if (my.ptab == NULL || my.ptab == &s_grounded) TAB_switch (s_curr->tab);;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char 
TAB_free                (char a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        n           =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_LOCS   yLOG_value   ("a_index"   , a_index);
   --rce;  if (!VALID_tab (a_index)) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_point   ("s_master"  , s_master [a_index]);
   --rce;  if (s_master [a_index] == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(depopulate)---------------------*/
   DEBUG_LOCS   yLOG_note    ("depopulate");
   if (s_master [a_index]->name != NULL && s_master [a_index]->name != g_tbd)  free (s_master [a_index]->name);
   s_master [a_index]->name = NULL;
   /*---(remove from master)-------------*/
   DEBUG_LOCS   yLOG_note    ("detach");
   free (s_master [a_index]);
   s_master [a_index] = NULL;
   /*---(update counts)------------------*/
   --s_all;
   if (strchr ("®¯", s_valids [a_index]) == NULL)   --s_count;
   DEBUG_LOCS   yLOG_value   ("s_all"     , s_all);
   DEBUG_LOCS   yLOG_value   ("s_count"   , s_count);
   /*---(update current)-----------------*/
   if (a_index == s_index) {
      s_index = -1;
      s_curr  = NULL;
      DEBUG_LOCS   yLOG_point   ("s_curr"    , s_curr);
      DEBUG_LOCS   yLOG_value   ("s_index "  , s_index);
   }
   if (a_index == CTAB) {
      n = TAB_first_filled ();
      if (n >= 0)  TAB_switch (n);
      else         TAB_ground ();
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
TAB_first_filled        (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        i           =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(check all tabs)-----------------*/
   for (i = 0; i < s_nvalid; ++i) {
      DEBUG_LOCS   yLOG_complex ("checking"  , "%2d %c %-10.10p", i, s_valids [i], s_master [i]);
      if (s_master [i] == NULL)  continue;
      DEBUG_LOCS   yLOG_value   ("i"         , i);
      DEBUG_INPT   yLOG_exit    (__FUNCTION__);
      return i;
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return -1;
}

char
TAB_first_open          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        i           =    0;
   char        n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(check all tabs)-----------------*/
   for (i = 0; i < s_nvalid; ++i) {
      DEBUG_LOCS   yLOG_complex ("checking"  , "%2d %c %-10.10p", i, s_valids [i], s_master [i]);
      if (s_master [i] != NULL)  continue;
      n = i;
      break;
   }
   DEBUG_LOCS   yLOG_value   ("n"         , n);
   --rce;  if (n < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return n;
}

char
TAB_new_in_open         (uchar *a_name, uchar *a_size)
{
   return TAB_new (NULL, TAB_first_open (), a_name, a_size);
}

char
TAB_new_in_abbr         (uchar a_abbr, uchar *a_name, uchar *a_size)
{
   return TAB_new (NULL, INDEX_tab (a_abbr), a_name, a_size);
}

char
TAB_free_all_empties    (void)
{
   /*---(locals)-----------+-----------+-*/
   char        i           =    0;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(initialize s_tabs)----------------*/
   for (i = 0; i < MAX_TABS; ++i) {
      DEBUG_PROG   yLOG_complex ("tab"       , "%2d, %-10.10p", i, s_master [i]);
      if (s_master [i] == NULL)    continue;
      rc = TAB_free (i);
      DEBUG_PROG   yLOG_value   ("free"      , rc);
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
TAB_free_from_abbr      (uchar a_abbr)
{
   switch (a_abbr) {
   case '*':  return  TAB_free_all_empties ();        break;
   case '.':  return  TAB_free (CTAB);                break;
   default :  return  TAB_free (INDEX_tab (a_abbr));  break;
   }
}



/*====================------------------------------------====================*/
/*===----                       search and finding                     ----===*/
/*====================------------------------------------====================*/
static void  o___SEARCH__________o () { return; }

char
TAB_live                (char a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (VALID_tab (a_index) != 1)         return 0;
   --rce;  if (s_master [a_index]  == NULL)      return 0;
   /*---(complete)-----------------------*/
   return 1;
}

char
TAB_by_cursor           (tTAB **a_found, char *a_start, char a_move)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_beg       =    0;
   char        x_end       =    0;
   char        x_dir       =    0;
   char        n           =   -1;
   char        i           =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_senter  (__FUNCTION__);
   DEBUG_LOCS   yLOG_schar   (a_move);
   /*---(prepare)------------------------*/
   DEBUG_LOCS   yLOG_spoint  (a_found);
   if (a_found != NULL)   *a_found = NULL;
   DEBUG_LOCS   yLOG_spoint  (a_start);
   if (a_start != NULL)    s_index = *a_start;
   /*---(defenses)-----------------------*/
   DEBUG_LOCS   yLOG_spoint  (s_curr);
   DEBUG_LOCS   yLOG_sint    (s_index);
   --rce;  if (s_index < 0 && strchr ("<>", a_move) != NULL) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(handle move)--------------------*/
   --rce;  switch (a_move) {
   case '_' : x_beg =  0;           x_end = s_nvalid - 1;  x_dir = '>'; break;
   case '[' : x_beg =  0;           x_end = s_nvalid - 3;  x_dir = '>'; break;
   case '>' : x_beg = s_index + 1;  x_end = s_nvalid - 1;  x_dir = '>'; break;
   case '<' : x_beg = s_index - 1;  x_end = 0;             x_dir = '<'; break;
   case ']' : x_beg = s_nvalid - 3; x_end = 0;             x_dir = '<'; break;
   case '~' : x_beg = s_nvalid - 1; x_end = 0;             x_dir = '<'; break;
   default  :
              DEBUG_LOCS   yLOG_snote   ("movement unknown");
              DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
              return rce;
   }
   DEBUG_LOCS   yLOG_sint    (x_beg);
   DEBUG_LOCS   yLOG_sint    (x_end);
   DEBUG_LOCS   yLOG_sint    (x_dir);
   /*---(walk)---------------------------*/
   if (x_dir == '>') {
      for (i = x_beg; i <= x_end; ++i) {
         if (s_master [i] != NULL) {
            n = i;
            break;
         }
      }
   } else if (x_dir == '<') {
      for (i = x_beg; i >= x_end; --i) {
         if (s_master [i] != NULL)  {
            n = i;
            break;
         }
      }
   }
   /*---(success)------------------------*/
   DEBUG_LOCS   yLOG_sint    (n);
   --rce;  if (n >= 0) {
      DEBUG_LOCS   yLOG_snote   ("found");
      s_index = n;
      s_curr  = s_master [s_index];
      rc      = 0;
   }
   /*---(safeties)-----------------------*/
   else if (strchr ("<>", a_move) != NULL) {
      DEBUG_LOCS   yLOG_snote   ("bounced off end");
      s_curr  = s_master [s_index];
      n       = s_index;
      rc      = 1;
   } else {
      DEBUG_LOCS   yLOG_snote   ("FAILED");
      s_curr  = NULL;
      s_index = -1;
      rc      = rce;
   }
   /*---(save)---------------------------*/
   if (a_found != NULL)  *a_found = s_curr;
   if (a_start != NULL)  *a_start = n;
   /*---(output)-------------------------*/
   DEBUG_LOCS   yLOG_spoint  (s_curr);
   DEBUG_LOCS   yLOG_sint    (s_index);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_sexit   (__FUNCTION__);
   return rc;
}

char
TAB_by_index            (tTAB **a_found, char a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_senter  (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (a_found != NULL)  *a_found = NULL;
   /*---(defenses)-----------------------*/
   DEBUG_LOCS   yLOG_spoint  (s_nvalid);
   --rce;  if (a_index < 0 || a_index >= s_nvalid) {
      s_curr  = NULL;
      s_index = -1;
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(check)--------------------------*/
   DEBUG_LOCS   yLOG_spoint  (s_master [a_index]);
   --rce;  if (s_master [a_index] == NULL) {
      s_curr  = NULL;
      s_index = -1;
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(set current)--------------------*/
   s_curr  = s_master [a_index];
   s_index = a_index;
   /*---(save)---------------------------*/
   if (a_found != NULL)  *a_found = s_curr;
   /*---(output)-------------------------*/
   DEBUG_LOCS   yLOG_spoint  (s_curr);
   DEBUG_LOCS   yLOG_sint    (s_index);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
TAB_by_abbr             (tTAB **a_found, uchar a_abbr)
{
   uchar *p = strchr (s_valids, a_abbr);
   if (p == NULL)  return TAB_by_index (a_found, -1);
   else            return TAB_by_index (a_found, p - s_valids);
}

char
TAB_by_name             (tTAB **a_found, uchar *a_regex)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         x_first     =   -1;
   int         x_matches   =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (a_found != NULL)  *a_found = NULL;
   /*---(defense)--------------------s---*/
   DEBUG_LOCS   yLOG_point   ("a_regex"   , a_regex);
   --rce;  if (a_regex == NULL || a_regex [0] == 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_delim   ("a_regex"   , a_regex);
   /*---(compile search)-----------------*/
   rc = yREGEX_comp (a_regex);
   DEBUG_SRCH   yLOG_value   ("comp"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_SRCH   yLOG_note    ("could not compile search");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check all tabs)-----------------*/
   for (i = 0; i < s_nvalid; ++i) {
      DEBUG_LOCS   yLOG_complex ("checking"  , "%2d %c %-10.10p", i, s_valids [i], s_master [i]);
      if (s_master [i] == NULL)  continue;
      DEBUG_INPT   yLOG_info    ("tab"       , s_master [i]->name);
      rc = yREGEX_exec (s_master [i]->name);
      DEBUG_INPT   yLOG_value   ("exec"      , rc);
      if (rc <= 0)   continue;
      ++x_matches;
      if (x_matches == 1)  x_first = i;
   }
   DEBUG_LOCS   yLOG_value   ("x_matches" , x_matches);
   --rce;  if (x_matches <= 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(make switch)--------------------*/
   TAB_by_index (a_found, x_first);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return x_matches;
}

char
TAB_pointer             (tTAB **a_found, char a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_senter  (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (a_found != NULL)   *a_found = NULL;
   /*---(defenses)-----------------------*/
   DEBUG_LOCS   yLOG_sint    (s_nvalid);
   --rce;  if (a_index < 0 || a_index >= s_nvalid) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(check)--------------------------*/
   DEBUG_LOCS   yLOG_spoint  (s_master [a_index]);
   --rce;  if (s_master [a_index] == NULL) {
      DEBUG_LOCS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(set current)--------------------*/
   if (a_found != NULL)  *a_found = s_master [a_index];
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
TAB_ensure              (tTAB **a_found, char a_tab)
{  /*---(notes)--------------------------*/
   /* returns  1 if col/row already exists (for testing/debugging)
    *          0 if col/row had to be created
    *          negative in case of trouble
    */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_valid     =    0;
   tTAB       *x_tab       = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "found %-10.10p, tab %2d", a_found, a_tab);
   /*---(prepare)------------------------*/
   if (a_found != NULL)  *a_found = NULL;
   /*---(find col/row)-------------------*/
   rc = TAB_by_index (&x_tab, a_tab);
   DEBUG_LOCS   yLOG_value   ("by_name"   , rc);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (x_tab == NULL) {
      TAB_new (&x_tab, a_tab, NULL, NULL);
      DEBUG_LOCS   yLOG_point   ("new"       , rc);
      DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
      if (x_tab == NULL) {
         DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   } else rc = 1;
   /*---(return)-------------------------*/
   if (a_found != NULL)  *a_found = x_tab;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                      hooking and unhooking                   ----===*/
/*====================------------------------------------====================*/
static void  o___HOOKING_________o () { return; }

char
TAB_hook_cell           (tTAB **a_found, char a_index, tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =    0;
   tTAB       *x_tab       = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_complex ("args"      , "found %-10.10p, tab %2d, cell %-10.10p", a_found, a_index, a_cell);
   /*---(defense)------------------------*/
   --rce;  if (a_cell == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_cell->tab != UNHOOKED) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(make sure tab is there)---------*/
   rc = TAB_by_index (&x_tab, a_index);
   DEBUG_LOCS   yLOG_value   ("ensure"    , rc);
   DEBUG_LOCS   yLOG_point   ("x_tab"     , x_tab);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strchr (G_TAB_LOCKED, x_tab->type) != NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(assign)-------------------------*/
   a_cell->tab = x_tab->tab;
   ++(x_tab->count);
   DEBUG_LOCS   yLOG_value   ("count"     , x_tab->count);
   /*---(return)-------------------------*/
   if (a_found != NULL)  *a_found = x_tab;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
TAB_unhook_cell         (tCELL *a_cell)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =    0;
   tTAB       *x_tab       = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_point   ("a_cell"    , a_cell);
   /*---(defense)------------------------*/
   --rce;  if (a_cell == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_cell->tab == UNHOOKED) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get tab)------------------------*/
   rc = TAB_pointer (&x_tab, a_cell->tab);
   --rce;  if (rc < 0) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strchr (G_TAB_LOCKED, x_tab->type) != NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(assign)-------------------------*/
   a_cell->tab = UNHOOKED;
   --(x_tab->count);
   DEBUG_LOCS   yLOG_value   ("count"     , x_tab->count);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         program level                        ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
TAB_init                (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        i           =    0;
   /*---(set valids)---------------------*/
   s_valids = LIST_tabs ();
   s_nvalid = strlen (s_valids);
   NTAB     = s_nvalid;
   /*---(ground tab pointers)------------*/
   for (i = 0; i < s_nvalid; ++i)   s_master [i] = NULL;
   /*---(ground searches)----------------*/
   my.ptab  = NULL;
   CTAB     = -1;
   s_curr   = NULL;
   s_all    = 0;
   s_count  = 0;
   s_index  = -1;
   s_grounded.type = 'N';
   s_grounded.abbr = '©';
   TAB_ground ();
   /*---(create base tabs)---------------*/
   /*> TAB_new_in_abbr ('0', NULL, NULL);                                             <* 
    *> TAB_new_in_abbr ('®', NULL, NULL);                                             <* 
    *> TAB_new_in_abbr ('¯', NULL, NULL);                                             <*/
   /*---(add buffer commands)------------*/
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "bnew"        , ""    , "ss"   , TAB_new_in_open            , "open a new tab in next open slot"     );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "bmake"       , ""    , "css"  , TAB_new_in_abbr            , "open a new tab in specific slot"      );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "bfree"       , ""    , "css"  , TAB_free_from_abbr         , "free a old tab in specific slot"      );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "buf"         , ""    , "c"    , TAB_switch_key             , "switch buffer"                        );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "btitle"      , ""    , "s"    , TAB_rename_curr            , "rename current buffer"                );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "bsize"       , ""    , "s"    , TAB_resize_curr            , "change a buffer size"                 );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_BUFFERS, "bbrowse"     , "bb"  , "a"    , TAB_browse                 , "find buffer by name"                  );
   /*---(add status options)-------------*/
   rc = yVIKEYS_view_option (YVIKEYS_STATUS, "buffer" , TAB_status_curr     , "details of current buffer"                  );
   /*---(add yparse specification)-------*/
   /*> rc = yPARSE_handler (FILE_TABS    , "tab"       , 4.1, "NLLsssc-----", -1, TAB_reader      , TAB_writer_all  , "------------" , "name,min,max,wid,tal,dep,t"        , "gyges tabs (v-axis)"      );   <*/
   rc = yPARSE_handler (FILE_TABS    , "tab"       , 4.1, "NLc---------", -1, TAB_reader      , TAB_writer_all  , "------------" , "name,min,max,wid,tal,dep,t"        , "gyges tabs (v-axis)"      );
   /*---(complete)-----------------------*/
   return rc;
}

char
TAB_cleanse             (tTAB *a_tab)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =    0;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_LOCS   yLOG_point   ("a_tab"     , a_tab);
   --rce;  if (a_tab == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(cells)--------------------------*/
   DEBUG_PROG   yLOG_value   ("cells"     , a_tab->count);
   --rce;  if (a_tab->count > 0) {
      DEBUG_PROG   yLOG_note    ("BAD NEWS, celss are on this tab");
      rc = rce;
   }
   /*---(columns)------------------------*/
   DEBUG_PROG   yLOG_value   ("cols (bef)", a_tab->C_count);
   n = COL_cleanse   (a_tab);
   DEBUG_PROG   yLOG_value   ("cleanse"   , n);
   --rce;  if (n > 0) {
      DEBUG_PROG   yLOG_note    ("BAD NEWS, columns are left over");
      rc = rce;
   }
   DEBUG_PROG   yLOG_value   ("cols (aft)", a_tab->C_count);
   /*---(rows)---------------------------*/
   DEBUG_PROG   yLOG_value   ("rows (bef)", a_tab->R_count);
   n = ROW_cleanse   (a_tab);
   DEBUG_PROG   yLOG_value   ("cleanse"   , n);
   --rce;  if (n > 0) {
      DEBUG_PROG   yLOG_note    ("BAD NEWS, columns are left over");
      rc = rce;
   }
   DEBUG_PROG   yLOG_value   ("rows (aft)", a_tab->R_count);
   /*---(free)---------------------------*/
   DEBUG_PROG   yLOG_value   ("free"      , rc);
   if (rc == 0) {
      TAB_free (a_tab->tab);
   }
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                          tab naming                          ----===*/
/*====================------------------------------------====================*/
static void  o___NAMES___________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.320.223.21]*/ /*-[00.0000.404.A]-*/ /*-[--.---.---.--]-*/
TAB_name             (char a_index, char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (!TAB_live (a_index))              return rce;
   --rce;  if (a_name  == NULL)                  return rce;
   /*---(return)-------------------------*/
   strlcpy (a_name, s_master [a_index]->name, LEN_LABEL);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.330.225.41]*/ /*-[00.0000.104.C]-*/ /*-[--.---.---.--]-*/
TAB_rename           (char a_index, char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_name      [LEN_LABEL] = "";
   /*---(defense)------------------------*/
   --rce;  if (!TAB_live (a_index))              return rce;
   /*---(name check)---------------------*/
   --rce;  switch (a_index) {
   case 36 :
      strlcpy (x_name, "summary", LEN_LABEL);
      break;
   case 37 :
      strlcpy (x_name, "system" , LEN_LABEL);
      break;
   default :
      if (TAB__name_check (a_name) < 0)          return rce;
      strlcpy (x_name, a_name   , LEN_LABEL);
      break;

   }
   /*---(set name)-----------------------*/
   if (s_master [a_index]->name != NULL && s_master [a_index]->name != g_tbd)  free (s_master [a_index]->name);
   s_master [a_index]->name = strdup (x_name);
   /*---(complete)-----------------------*/
   return 0;
}

char  TAB_rename_curr      (char *a_name) { return TAB_rename (CTAB, a_name); }



/*====================------------------------------------====================*/
/*===----                          switching                           ----===*/
/*====================------------------------------------====================*/
static void  o___SWITCHING_______o () { return; }

char         /*-> tbd --------------------------------[ ------ [gc.E82.112.31]*/ /*-[02.0000.073.!]-*/ /*-[--.---.---.--]-*/
TAB_switch             (char a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(begin)--------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(defense)---------------------*/
   DEBUG_LOCS   yLOG_value   ("a_index"   , a_index);
   --rce;  if (!TAB_live (a_index)) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save values)-----------------*/
   TAB_save     ();
   /*---(update tab)------------------*/
   CTAB      = a_index;
   /*---(retrieve tab)-------------------*/
   TAB_retrieve ();
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gc.220.112.31]*/ /*-[00.0000.103.!]-*/ /*-[--.---.---.--]-*/
TAB_switch_key         (uchar a_key)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   static char x_curr      =   -1;
   static char x_prev      =   -2;
   char        x_temp      =    0;
   tTAB       *x_tab       = NULL;
   char        x_seq       =   -1;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   DEBUG_LOCS   yLOG_char    ("a_key"     , a_key);
   DEBUG_LOCS   yLOG_value   ("x_curr"    , x_curr);
   DEBUG_LOCS   yLOG_value   ("x_prev"    , x_prev);
   /*---(previous tabs)------------------*/
   --rce;  if (a_key == ',') {
      x_temp  = x_curr;
      x_curr  = x_prev;
      x_prev  = x_temp;
   }
   /*---(relative tabs)------------------*/
   else if (a_key > 0 && strchr ("_[<>]~", a_key) != NULL) {
      x_seq  = x_curr;
      rc     = TAB_by_cursor (&x_tab, &x_seq, a_key);
      if (rc != 0) {
         DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_prev = x_curr;
      x_curr = x_seq;
   }
   /*---(absolute tabs)------------------*/
   else {
      rc     = TAB_by_abbr (&x_tab, a_key);
      if (rc < 0) {
         DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_prev = x_curr;
      x_curr = x_tab->tab;
   }
   /*---(done)---------------------------*/
   DEBUG_LOCS   yLOG_value   ("x_curr"    , x_curr);
   DEBUG_LOCS   yLOG_value   ("x_prev"    , x_prev);
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return TAB_switch (x_curr);
}

char
TAB_browse              (char *a_entry)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tTAB       *x_match     = NULL;
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(compile search)-----------------*/
   rc = TAB_by_name (&x_match, a_entry);
   DEBUG_LOCS   yLOG_value   ("search"    , rc);
   --rce;  if (rc <= 0)  {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(make switch)--------------------*/
   TAB_switch (x_match->tab);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                        updating sizing                       ----===*/
/*====================------------------------------------====================*/
static void  o___COUNTS__________o () { return; }

int          /*-> indicate if tab is used ------------[ ------ [gn.210.212.11]*/ /*-[00.0000.304.!]-*/ /*-[--.---.---.--]-*/
TAB_used             (char a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (!TAB_live (a_index))              return rce;
   /*---(complete)-----------------------*/
   return s_master [a_index]->count;
}



/*====================------------------------------------====================*/
/*===----                        updating sizing                       ----===*/
/*====================------------------------------------====================*/
static void  o___SIZING__________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.320.223.22]*/ /*-[00.0000.00#.D]-*/ /*-[--.---.---.--]-*/
TAB_size             (char a_index, char *a_max)
{
   char        rce         =  -10;
   --rce;  if (!TAB_live (a_index))              return rce;
   --rce;  if (a_max   == NULL)                  return rce;
   str4gyges (a_index, s_master [a_index]->ncol, s_master [a_index]->nrow, 0, 0, a_max, YSTR_CHECK);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.632.153.22]*/ /*-[01.0000.104.U]-*/ /*-[--.---.---.--]-*/
TAB_resize           (char a_index, char *a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   int         x_max       =    0;
   char        x_type      =  '-';
   char        x_meth      =  '-';
   /*---(header)-------------------------*/
   DEBUG_LOCS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_LOCS   yLOG_value   ("a_index"   , a_index);
   --rce;  if (!TAB_live (a_index))  {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_point   ("a_max"     , a_max);
   --rce;  if (a_max == NULL) {
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOCS   yLOG_info    ("a_max"     , a_max);
   /*---(locked or fixed)----------------*/
   x_type = s_master [a_index]->type;
   DEBUG_LOCS   yLOG_char    ("x_type"    , x_type);
   if (strchr (G_TAB_TYPES, x_type) == NULL) {
      DEBUG_LOCS   yLOG_note    ("tab locked, no adjustment");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check specific size)------------*/
   rc = str2gyges  (a_max, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_USABLE);
   DEBUG_LOCS   yLOG_value   ("str2gyges" , rc);
   /*---(check short-cuts)---------------*/
   --rce;  if      (rc >= 0)                  x_meth = 'f';
   else if (strcmp (a_max, "min"    ) == 0)   x_meth = 'm';
   else if (strcmp (a_max, "auto"   ) == 0)   x_meth = 'a';
   else if (strcmp (a_max, "full"   ) == 0)   x_meth = '*';
   else if (strcmp (a_max, ""       ) == 0)   x_meth = '-';
   else {
      DEBUG_LOCS   yLOG_note    ("max or meth not understood");
      DEBUG_LOCS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (x_meth == '-' && x_type == 'f') {
      DEBUG_LOCS   yLOG_note    ("fixed tab, no update requested");
      DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check short-cuts)---------------*/
   if (x_meth == 'a' || x_type == 'a') {
      DEBUG_LOCS   yLOG_note    ("auto treatment");
      if (a_index < 36 && x_type == 'f')  s_master [a_index]->type = '-';
      x_col = COL_max_adjust (a_index);
      x_row = ROW_max_adjust (a_index);
   }
   else if (x_meth == 'f') {
      DEBUG_LOCS   yLOG_note    ("fixed size");
      s_master [a_index]->type = 'f';
      x_max = COL_max_used   (a_index);
      if (x_max > x_col)  x_col = x_max;
      x_max = ROW_max_used   (a_index);
      if (x_max > x_row)  x_row = x_max;
   }
   else if (x_meth == 'm') {
      DEBUG_LOCS   yLOG_note    ("minimum size");
      s_master [a_index]->type = 'f';
      x_col = COL_max_used   (a_index);
      x_row = ROW_max_used   (a_index);
   }
   else if (x_meth == '*') {
      DEBUG_LOCS   yLOG_note    ("full size");
      s_master [a_index]->type = 'f';
      x_col = MAX_col ();
      x_row = MAX_row ();
   }
   else {
      DEBUG_LOCS   yLOG_note    ("normal/auto treatment");
      x_col = COL_max_adjust (a_index);
      x_row = ROW_max_adjust (a_index);
   }
   DEBUG_LOCS   yLOG_complex ("coord"     , "%3dc, %4dr", x_col, x_row);
   /*---(adjust current)-----------------*/
   if (a_index == CTAB) {
      DEBUG_LOCS   yLOG_note    ("tab is current, adjusting");
      NCOL = x_col;
      NROW = x_row;
   }
   /*---(adjust tab)---------------------*/
   DEBUG_LOCS   yLOG_note    ("fix tab settings");
   s_master [a_index]->ncol = x_col;
   s_master [a_index]->nrow = x_row;
   /*---(complete)-----------------------*/
   DEBUG_LOCS   yLOG_exit    (__FUNCTION__);
   return 0;
}
char TAB_resize_curr      (char *a_max) { return TAB_resize (CTAB, a_max); }



/*====================------------------------------------====================*/
/*===----                         other characteristics                ----===*/
/*====================------------------------------------====================*/
static void  o___CHARS___________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.220.122.11]*/ /*-[00.0000.10#.!]-*/ /*-[--.---.---.--]-*/
TAB_type             (char a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_type      =  '-';
   /*---(defense)------------------------*/
   --rce;  if (!TAB_live (a_index))              return rce;
   /*---(complete)-----------------------*/
   return s_master [a_index]->type;
}

char         /*-> tbd --------------------------------[ ------ [ge.320.222.11]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
TAB_retype           (char a_index, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_type      =  '-';
   /*---(defense)------------------------*/
   --rce;  if (!TAB_live (a_index))              return rce;
   x_type = s_master [a_index]->type;
   /*---(system tabs)--------------------*/
   if (a_index >= 36) {
      x_type  = G_TAB_AUTO;
   }
   /*---(lock)---------------------------*/
   else if (a_type == G_TAB_LOCK) {
      if (x_type == '-')   x_type = 'N';
      else                 x_type = toupper (x_type);
   }
   /*---(unlock)-------------------------*/
   else if (a_type == G_TAB_UNLOCK) {
      if (x_type == 'N')   x_type = '-';
      else                 x_type = tolower (x_type);
   }
   /*---(set type)-----------------------*/
   else {
      if (strchr (G_TAB_TYPES, a_type) == NULL)  return rce;
      if (a_index < 36 && a_type == 'a')         return rce;
      x_type = a_type;
   }
   /*---(save)---------------------------*/
   s_master [a_index]->type = x_type;
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        status information                    ----===*/
/*====================------------------------------------====================*/
static void  o___STATUS__________o () { return; }

/*
 * large (L)
 * 123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-12
 * G 16 bigger_tab   - 0b2     0f31    0a1     0a1     0f33     42n   5c  21r 0z100  ´
 *                                                                  |
 * medium (M)                                                       |
 * 123456789-123456789-123456789-123456789-123456789-               |
 * G 16 bigger_tab   0b2     0f31     42n   5c  21r ´               |
 *                                         |                        |
 * small (S)                               |                        |
 * 123456789-123456789-123456789-          |                        |
 * G bigger_tab   0f31     42n ´           |                        |
 *
 */

char         /*-> tbd --------------------------------[ leaf   [ge.632.233.70]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
TAB_line           (char a_index, char a_size, char *a_list)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   uchar       x_abbr      =  '0';
   tTAB       *x_tab       = NULL;
   char        x_beg       [LEN_LABEL] = "";
   char        x_cur       [LEN_LABEL] = "";
   char        x_end       [LEN_LABEL] = "";
   char        x_siz       [LEN_LABEL] = "";
   char        x_min       [LEN_LABEL] = "";
   char        x_max       [LEN_LABEL] = "";
   char        x_count     [LEN_LABEL] = "";
   /*---(beginning)----------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_complex ("args"      , "%2dt, %c, %-10.10p", a_index, a_size, a_list);
   /*---(defense)---------------------*/
   --rce;  if (a_list  == NULL) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(prepare defaults)------------*/
   switch (a_size) {
   case 'L' : snprintf (a_list, LEN_FULL, "´ -  -            - -       -       -       -       -         -n   -c   -r -       ´"); break;
   case 'M' : snprintf (a_list, LEN_FULL, "´ -  -            -       -         -n   -c   -r ´");                                   break;
   case 'S' : snprintf (a_list, LEN_FULL, "´ -            -         -n ´");                                                        break;
   default  :
              DEBUG_REGS   yLOG_note    ("line size indicator unknown");
              DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
              return 0;
   }
   /*---(bad tab)---------------------*/
   --rce;  if (!VALID_tab (a_index)) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   a_list [0] = x_abbr = LABEL_tab (a_index);
   /*---(handle empties)--------------*/
   if (!TAB_live (a_index)) {
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   rc = TAB_by_index (&x_tab, a_index);
   --rce;  if (x_tab  == NULL) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return 0;
   }
   /*---(generate stats)--------------*/
   str4gyges  (s_curr->tab, s_curr->bcol, s_curr->brow, 0, 0, x_beg, YSTR_USABLE);
   str4gyges  (s_curr->tab, s_curr->ccol, s_curr->crow, 0, 0, x_cur, YSTR_USABLE);
   str4gyges  (s_curr->tab, s_curr->ecol, s_curr->erow, 0, 0, x_end, YSTR_USABLE);
   str4gyges  (s_curr->tab, s_curr->ncol, s_curr->nrow, 0, 0, x_siz, YSTR_USABLE);
   str4gyges  (s_curr->tab, COL_min_used (s_curr->tab), ROW_min_used (s_curr->tab), 0, 0, x_min, YSTR_USABLE);
   str4gyges  (s_curr->tab, COL_max_used (s_curr->tab), ROW_max_used (s_curr->tab), 0, 0, x_max, YSTR_USABLE);
   /*---(finish)----------------------*/
   switch (a_size) {
   case 'L' :
      snprintf (a_list, LEN_FULL, "%c %-2d %-12.12s %c %-7.7s %-7.7s %-7.7s %-7.7s %-7.7s %3dn %3dc %3dr %-7.7s ´", x_tab->abbr, x_tab->tab, x_tab->name, x_tab->type, x_min, x_max, x_beg, x_cur, x_end, x_tab->count, x_tab->C_count, x_tab->R_count, x_siz);
      break;
   case 'M' :
      snprintf (a_list, LEN_FULL, "%c %-2d %-12.12s %-7.7s %-7.7s %3dn %3dc %3dr ´", x_tab->abbr, x_tab->tab, x_tab->name, x_min, x_max, x_tab->count, x_tab->C_count, x_tab->R_count);
      break;
   case 'S' :
      snprintf (a_list, LEN_FULL, "%c %-12.12s %-7.7s %3dn ´", x_tab->abbr, x_tab->name, x_max, x_tab->count);
      break;
   }
   /*---(complete)--------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.632.233.40]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
TAB_status         (char a_index, char *a_list)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        t           [LEN_FULL]   = "";
   /*---(beginning)---------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(get info)--------------------*/
   rc = TAB_line (a_index, yVIKEYS_status_width (), t);
   sprintf (a_list, "%s", t);
   /*---(complete)--------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  TAB_status_curr    (char *a_list) { return TAB_status (CTAB, a_list); }

/*
 *
 *    0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ®¯
 *    0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ ®¯
 *
 *    01234 56789 ABCDE FGHIJ KLMNO PQRST UVWXYZ ®¯
 *
 *    0---- -----   ----- ----- ----- ----- ------   ®¯
 *
 * large (L)
 * 123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-12
 * G ´ 34 2 ´ 012 345 ´ 678 9AB ´ CDE FGH ´ IJK LMN ´ OPQ RST ´ UVW XYZ ´ ®¯
 * 0 ´  1 2 ´ 0-- --- ´ --- --- ´ --- --- ´ --- --- ´ --- --- ´ --- --- ´ ®¯
 *
 * medium (M)                                                       |
 * 123456789-123456789-123456789-123456789-123456789-               |
 * 0 ´ 0----- ------ ------ ------ ------ ------ ®¯
 *
 * small (S)                               |                        |
 * 123456789-123456789-123456789-          |                        |
 * 0 ´ 0 ®¯
 *
 */


char
TAB_inventory            (char a_size, char *a_list)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i           =    0;
   char        x_curr      =  '-';
   char        x_pre       [LEN_LABEL] = "";
   char        s           [LEN_LABEL] = "";
   char        t           [LEN_HUND]  = "";
   char        c           =    0;
   char        a           =    0;
   /*---(prefix)-------------------------*/
   x_curr = LABEL_tab (CTAB);
   /*---(inventory)----------------------*/
   for (i = 0; i < s_nvalid; ++i)  {
      /*---(abbr)--------------*/
      if (s_master [i] == NULL)  strlcpy (s, "-", LEN_TERSE);
      else                       sprintf (s, "%c", LABEL_tab (i));
      /*---(concat)------------*/
      switch (a_size) {
      case 'L' :
         if (i > 0 && i % 6 == 0)  strlcat (t, " ´", LEN_HUND);
         if (i > 0 && i % 3 == 0)  strlcat (t, " " , LEN_HUND);
         strlcat (t, s, LEN_HUND);
         break;
      case 'M' :
         if (i > 0 && i % 6 == 0)  strlcat (t, " ", LEN_HUND);
         strlcat (t, s, LEN_HUND);
         break;
      case 'S' :
         if (i > 0 && i % 6 == 0)  strlcat (t, " ", LEN_HUND);
         if (s [0] != '-')         strlcat (t, s, LEN_HUND);
         break;
      }
      /*---(counts)------------*/
      if (s [0] != '-')  {
         if (i < 36)  ++c;
         else         ++a;
      }
   }
   /*---(prefix)-------------------------*/
   switch (a_size) {
   case 'L' :
      sprintf (x_pre, "%c ´ %2d %1d ´ ", x_curr, c, a);
      break;
   case 'M' : case 'S' :
      sprintf (x_pre, "%c ´ ", x_curr);
      break;
   }
   /*---(prefix)-------------------------*/
   strltrim (t, ySTR_SINGLE, LEN_HUND);
   sprintf (a_list, "%s%s", x_pre, t);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    yparse record handling                    ----===*/
/*====================------------------------------------====================*/
static void  o___YPARSE__________o () { return; }

char
TAB_reader           (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   char        x_verb      [LEN_LABEL];
   char        x_name      [LEN_LABEL];
   char        x_label     [LEN_LABEL];
   int         x_len       =    0;
   int         x_tab       =    0;
   int         x_size      =    0;
   char        x_type      =  '-';
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(get verb)-----------------------*/
   rc = yPARSE_popstr (x_verb);
   DEBUG_INPT   yLOG_value   ("pop verb"  , rc);
   DEBUG_INPT   yLOG_info    ("x_verb"    , x_verb);
   --rce;  if (strcmp ("tab", x_verb) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get name)-----------------------*/
   rc = yPARSE_popstr (x_name);
   DEBUG_INPT   yLOG_value   ("pop name"  , rc);
   DEBUG_INPT   yLOG_info    ("x_name"    , x_name);
   x_len = strlen (x_name);
   DEBUG_INPT   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <= 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(max)----------------------------*/
   rc = yPARSE_popstr (x_label);
   DEBUG_INPT   yLOG_value   ("pop max"   , rc);
   DEBUG_INPT   yLOG_info    ("max"       , x_label);
   rc = str2gyges (x_label, &x_tab, NULL, NULL, NULL, NULL, 0, YSTR_ADAPT);
   DEBUG_INPT   yLOG_value   ("str2gyges" , rc);
   --rce;  if (rc < 0 || x_tab >= 36) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_value   ("tab"       , x_tab);
   /*---(make tab)-----------------------*/
   rc = TAB_new_in_abbr (LABEL_tab (x_tab), x_name, x_label);
   DEBUG_INPT   yLOG_value   ("TAB_new"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(type)---------------------------*/
   rc = yPARSE_popchar (&x_type);
   DEBUG_INPT   yLOG_value   ("pop type"  , rc);
   DEBUG_INPT   yLOG_char    ("x_type"    , x_type);
   rc = TAB_retype (x_tab, x_type);
   DEBUG_INPT   yLOG_value   ("TAB_retype", rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 1;
}

char
TAB_writer            (char a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   char        x_type      =  '-';
   char        x_name      [LEN_LABEL];
   char        x_max       [LEN_LABEL];
   /*---(prepare)------------------------*/
   yPARSE_outclear  ();
   /*---(defense)------------------------*/
   if (!VALID_tab (a_tab))         return  0;
   if (s_master [a_tab] == NULL)   return  0;
   if (a_tab >= 36)                return  0;
   /*---(prepare)------------------------*/
   TAB_name    (a_tab, x_name);
   x_type = TAB_type (a_tab);
   str4gyges (a_tab, COL_max (a_tab), ROW_max (a_tab), 0, 0, x_max, YSTR_USABLE);
   /*---(write)--------------------------*/
   rc = yPARSE_fullwrite ("tab", x_name, x_max, x_type);
   if (rc < 0)   return rc;
   /*---(complete)-----------------------*/
   return 1;
}

char
TAB_writer_all          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   int         i           =    0;
   char        c           =    0;
   /*---(walk)---------------------------*/
   yPARSE_verb_begin ("tab");
   for (i = 0; i < MAX_TABS; ++i) {
      rc = TAB_writer   (i);
      if (rc <  1)  continue;
      ++c;
      yPARSE_verb_break (c);
   }
   yPARSE_verb_end   (c);
   /*---(complete)-----------------------*/
   return c;
}



/*====================------------------------------------====================*/
/*===----                     yvikeys mapper                           ----===*/
/*====================------------------------------------====================*/
static void  o___MAPPER__________o () { return; }

char
TAB_map_init            (char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   tMAPPED    *x_map       = NULL;
   /*---(prepare)------------------------*/
   x_map   = &g_bmap;
   /*---(clear)--------------------------*/
   x_map->ubeg = x_map->ucur = x_map->uend  = x_map->ulen  = x_map->utend = 0;
   x_map->gbeg = x_map->gcur = x_map->gend = 0;
   /*---(complete)-----------------------*/
   return 0;
}

char
TAB_map_clear              (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tMAPPED    *x_map       = NULL;
   int         i           =    0;
   /*---(prepare)------------------------*/
   x_map   = &g_bmap;
   /*---(clear)--------------------------*/
   for (i= 0; i < LEN_HUGE; ++i)  x_map->map [i] =  YVIKEYS_EMPTY;
   x_map->gmin = x_map->gamin = x_map->glmin = x_map->gprev = -1;
   x_map->gmax = x_map->gamax = x_map->glmax = x_map->gnext = -1;
   /*---(complete)-----------------------*/
   return 0;
}

char
TAB_map_mapper            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tMAPPED    *x_map       = NULL;
   int         i           =    0;
   int         x_pos       =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   x_map   = &g_bmap;
   /*---(defaults)--------------------*/
   x_map->umin = -1;
   x_map->gmin =  0;
   /*---(get tab)------------------------*/
   for (i = 0; i < MAX_TABS; ++i) {
      DEBUG_MAP    yLOG_point   ("used"      , s_master [i]);
      if (s_master [i] == NULL)  continue;
      x_map->map  [x_pos++] = i;
   }
   /*---(fixes)-----------------------*/
   x_map->umax = x_pos - 1;
   if (x_map->umax >= 0)  x_map->umin =  0;
   x_map->gmax = MAX_TABS - 1;
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
TAB_map_display           (char a_section, uchar *a_out)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tMAPPED    *x_map       = NULL;
   char        i           =    0;
   /*---(prepare)------------------------*/
   x_map   = &g_bmap;
   /*---(break out)----------------------*/
   for (i = 0; i < MAX_TABS; ++i) {
      if (x_map->map [i] == YVIKEYS_EMPTY)   a_out [i] = '·';
      else                                   a_out [i] = LABEL_tab (x_map->map [i]);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
TAB_map_absolute          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tMAPPED    *x_map       = NULL;
   tTAB       *x_tab       = NULL;
   char        x_ntab      =    0;
   char        x_min       =   -1;
   char        x_max       =   -1;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   x_map   = &g_bmap;
   x_map->gamin = -1;
   x_map->gamax = -1;
   /*---(find min)-----------------------*/
   rc = TAB_by_cursor (&x_tab, &x_ntab, '_');
   if (rc == 0 && x_tab != NULL) {
      x_min = x_ntab;
   }
   /*---(find max)-----------------------*/
   rc = TAB_by_cursor (&x_tab, &x_ntab, '~');
   if (rc == 0 && x_tab != NULL) {
      x_max = x_ntab;
   }
   /*---(update)-------------------------*/
   x_map->gamin = x_min;
   x_map->gamax = x_max;
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
TAB_map_local             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tMAPPED    *x_map       = NULL;
   tTAB       *x_tab       = NULL;
   char        x_ntab      =    0;
   int         x_curr      =    0;
   int         x_opps      =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   x_map   = &g_bmap;
   x_map->glmin = -1;
   x_map->gprev = -1;
   x_map->gnext = -1;
   x_map->glmax = -1;
   /*---(find min)-----------------------*/
   rc = TAB_by_cursor (&x_tab, &x_ntab, '[');
   if (rc == 0 && x_tab != NULL) {
      x_map->glmin = x_ntab;
   }
   /*---(find max)-----------------------*/
   rc = TAB_by_cursor (&x_tab, &x_ntab, ']');
   if (rc == 0 && x_tab != NULL) {
      x_map->glmax = x_ntab;
   }
   /*---(find prev)----------------------*/
   x_ntab = x_map->gcur;
   rc = TAB_by_cursor (&x_tab, &x_ntab, '<');
   if (rc == 0 && x_tab != NULL) {
      x_map->gprev = x_ntab;
   }
   /*---(find next)----------------------*/
   x_ntab = x_map->gcur;
   rc = TAB_by_cursor (&x_tab, &x_ntab, '>');
   if (rc == 0 && x_tab != NULL) {
      x_map->gnext = x_ntab;
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
TAB_map_update             (char a_req)
{
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   if (a_req == YVIKEYS_INIT)  TAB_map_init (a_req);
   TAB_map_clear    ();
   TAB_map_mapper   ();
   TAB_map_absolute ();
   TAB_map_local    ();
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      unit testing                            ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }


char*        /*-> unit test accessor -----------------[ light  [us.B60.2A3.F2]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
TAB__unit          (char *a_question, int a_index)
{
   /*---(locals)-------------------------*/
   char        rc          =    0;
   int         i           =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   char        x_abs       =    0;
   uchar       x_abbr      =  '-';
   char        x_pref      [LEN_LABEL]   = "";
   char        s           [LEN_TERSE]   = "";
   char        t           [LEN_HUND]    = "";
   char        a           =    0;
   char        c           =    0;
   tTAB       *x_curr      = NULL;
   int         x_index     =   -1;
   tMAPPED    *x_map       = NULL;
   /*---(prepare)------------------------*/
   x_map   = &g_bmap;
   /*---(parse location)-----------------*/
   strcpy  (unit_answer, "TAB              : label could not be parsed");
   /*---(prepare data)-------------------*/
   strcpy  (unit_answer, "TAB              : locations could not be prepared");
   if (x_tab > 37) {
      x_abbr  = a_index;
      a_index = INDEX_tab (x_abbr);
   } else {
      x_abbr  = LABEL_tab (a_index);
   }
   /*---(overall)------------------------*/
   strcpy  (unit_answer, "TAB              : question not understood");
   if      (strcmp(a_question, "count"         ) == 0) {
      snprintf(unit_answer, LEN_FULL, "TAB count        : %2dn, %2da, %2dc", NTAB, s_all, s_count);
   }
   else if (strcmp(a_question, "entry") == 0 || strcmp(a_question, "current") == 0 || strcmp(a_question, "focus") == 0) {
      x_index = s_index;
      x_curr  = s_curr;
      if (a_question [0] == 'e') {
         strlcpy (x_pref, "entry", LEN_LABEL);
         TAB_by_index (NULL, a_index);
      } else if (a_question [0] == 'c') {
         strlcpy (x_pref, "curr " , LEN_LABEL);
         a_index = s_index;
      } else {
         strlcpy (x_pref, "focus" , LEN_LABEL);
         a_index = CTAB;
      }
      TAB_line (a_index, 'L', t);
      t [strlen (t) - 2] = '\0';
      strltrim (t, ySTR_TAIL, LEN_HUND);
      snprintf (unit_answer, LEN_FULL, "TAB %-5.5s    (%c) : %s", x_pref, t [0], t + 2);
      s_index = x_index;
      s_curr  = x_curr;
   }
   else if (strcmp(a_question, "inventory"     ) == 0) {
      TAB_inventory ('L', t);
      snprintf (unit_answer, LEN_FULL, "TAB inventory    : %s", t);
   }
   else if (strcmp (a_question, "map"           ) == 0) {
      TAB_map_display (a_index, t);
      snprintf(unit_answer, LEN_FULL, "TAB map     (%2d) : [%s]", a_index, t);
   }
   else if (strcmp (a_question, "abs"           ) == 0) {
      snprintf (unit_answer, LEN_FULL, "TAB abs          : umin  %5d, gmin  %5d, gamin %5d      gamax %5d, gmax  %5d, umax  %5d", x_map->umin, x_map->gmin, x_map->gamin, x_map->gamax, x_map->gmax, x_map->umax);
   }
   else if (strcmp (a_question, "local"         ) == 0) {
      snprintf (unit_answer, LEN_FULL, "TAB local        : glmin %5d, gprev %5d          gcur %5d            gnext %5d, glmax %5d", x_map->glmin, x_map->gprev, x_map->gcur, x_map->gnext, x_map->glmax);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}
