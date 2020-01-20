/*============================----beg-of-source---============================*/
#include   "gyges.h"



#define     MAX_HIST    100000
struct cHIST {
   char        mode;
   char        act;
   int         nkey;
   char       *addr;
   char       *before;
   char       *after;
   tHIST      *h_prev;
   tHIST      *h_next;
};
static tHIST  *s_head     = NULL;
static tHIST  *s_curr     = NULL;
static tHIST  *s_tail     = NULL;
static int     s_count    = 0;
static int     s_index    = -1;

char           hist_active;

typedef struct cACTS  tACTS;
struct  cACTS{
   char        act;
   char        name         [LEN_LABEL];
   char        desc         [LEN_DESC];
};
static const tACTS s_hist_acts [] = {
   { HIST_OVERWRITE, "overwrite"    , "change contents and formatting"  },
   { HIST_CHANGE   , "change"       , "change contents only"            },
   { HIST_DELETE   , "delete"       , ""                                },
   { HIST_CLEAR    , "clear"        , ""                                },
   { HIST_ALIGN    , "align"        , "display alignment"               },
   { HIST_FORMAT   , "format"       , "display format"                  },
   { HIST_DECIMALS , "decimals"     , "display decimal places"          },
   { HIST_UNITS    , "units"        , "display units of measure"        },
   { HIST_WIDTH    , "width"        , "change column width"             },
   { HIST_HEIGHT   , "height"       , "change row height"               },
   { 0             , "end"          , "" },
};
int         s_hist_nact =    0;

char        s_action    [LEN_LABEL];

char       *g_nada      = "";
char       *g_nothing   = "´´´´´";
char       *g_default   = "??0--";
char       *g_ditto     = "~~~~~";
char       *g_before    = NULL;
char       *g_after     = NULL;



/*====================------------------------------------====================*/
/*===----                      memory allocation                       ----===*/
/*====================------------------------------------====================*/
static void  o___MEMORY__________o () { return; }

char
HIST__new          (char a_mode, char a_type, int a_tab, int a_col, int a_row)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tHIST      *x_new       = NULL;
   char        x_tries     =    0;
   int         x_len       =    0;
   char        x_label     [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   DEBUG_HIST  yLOG_complex ("a_mode"    , "%c, %c, %2dt, %3dc, %4dr", a_mode, a_type, a_tab, a_col, a_row);
   /*---(internal)-----------------------*/
   DEBUG_HIST  yLOG_char    ("active"    , hist_active);
   if (hist_active != 'y') {
      DEBUG_HIST  yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_mode      == HIST_NONE) {
      DEBUG_HIST  yLOG_note    ("history is not requested on this action");
      DEBUG_HIST  yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(prune future)-------------------*/
   rc = HIST__prune ();
   /*---(allocate)-----------------------*/
   while (x_new == NULL && x_tries < 10)  {
      ++x_tries;
      x_new = (tHIST *) malloc (sizeof (tHIST));
   }
   DEBUG_HIST   yLOG_value   ("x_tries"   , x_tries);
   DEBUG_HIST   yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_HIST   yLOG_note    ("FAILED");
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(populate)-----------------------*/
   DEBUG_HIST   yLOG_note    ("populate");
   x_new->mode    = a_mode;
   x_new->act     = a_type;
   x_new->nkey    = yVIKEYS_keys_nkey ();
   str4gyges (a_tab, a_col, a_row, 0, 0, x_label, YSTR_USABLE);
   x_new->addr    = strdup (x_label);
   x_new->before  = g_before;
   x_new->after   = g_after;
   x_new->h_prev  = NULL;
   x_new->h_next  = NULL;
   /*---(tie to master list)-------------*/
   if (s_head == NULL) {
      DEBUG_HIST   yLOG_note    ("nothing in list, make first");
      s_head         = x_new;
   } else  {
      DEBUG_HIST   yLOG_note    ("append to list");
      s_tail->h_next = x_new;
      x_new->h_prev  = s_tail;
   }
   s_tail        = x_new;
   /*---(update counts)------------------*/
   ++s_count;
   DEBUG_HIST   yLOG_value   ("s_count"   , s_count);
   /*---(update current)-----------------*/
   s_curr   = s_tail;
   s_index  = s_count - 1;
   DEBUG_HIST   yLOG_point   ("s_curr"    , s_curr);
   DEBUG_HIST   yLOG_value   ("s_index"   , s_index);
   /*---(clear)--------------------------*/
   g_before  = NULL;
   g_after   = NULL;
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
HIST__delete            (tHIST *a_curr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   DEBUG_HIST   yLOG_spoint  (a_curr);
   --rce;  if (a_curr == NULL) {
      DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST   yLOG_sint    (s_count);
   /*---(free content)-------------------*/
   if (a_curr->addr   != g_nada)      free (a_curr->addr);
   if (a_curr->before != g_nothing)   free (a_curr->before);
   if (a_curr->after  != g_nothing)   free (a_curr->after);
   /*---(out of linked list)-------------*/
   if (a_curr->h_next != NULL)   a_curr->h_next->h_prev = a_curr->h_prev;
   else                          s_tail                 = a_curr->h_prev;
   if (a_curr->h_prev != NULL)   a_curr->h_prev->h_next = a_curr->h_next;
   else                          s_head                 = a_curr->h_next;
   --s_count;
   DEBUG_HIST   yLOG_sint    (s_count);
   /*---(clear links)--------------------*/
   a_curr->h_prev = NULL;
   a_curr->h_next = NULL;
   /*---(update current)-----------------*/
   if (s_curr == a_curr) {
      s_curr   = s_tail;
      s_index  = s_count - 1;
   }
   /*---(free entry)---------------------*/
   free (a_curr);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
HIST__prune             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tHIST      *x_curr      = NULL;
   tHIST      *x_next      = NULL;
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   DEBUG_HIST  yLOG_point   ("s_curr"    , s_curr);
   --rce;  if (s_curr == NULL) {
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_curr = s_curr->h_next;
   DEBUG_HIST  yLOG_point   ("x_curr"    , x_curr);
   while (x_curr != NULL) {
      x_next = x_curr->h_next;
      HIST__delete (x_curr);
      x_curr = x_next;
      DEBUG_HIST  yLOG_point   ("x_curr"    , x_curr);
   }
   s_curr->h_next = NULL;
   s_tail = s_curr;
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
HIST__purge             (void)
{
   tHIST      *x_curr      = NULL;
   tHIST      *x_next      = NULL;
   x_curr = s_tail;
   while (x_curr != NULL) {
      x_next = x_curr->h_prev;
      HIST__delete (x_next);
      x_curr = x_next;
   }
   s_head  = NULL;
   s_tail  = NULL;
   s_curr  = NULL;
   s_count = 0;
   s_index = 0;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       search/find entries                    ----===*/
/*====================------------------------------------====================*/
static void  o___SEARCH__________o () { return; }

char
HIST__find              (int a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tHIST      *x_curr      =    0;
   int         c           =    0;
   /*---(defense)---------------------*/
   --rce;  if (s_head == NULL)  {
      s_curr  = NULL;
      s_index = -1;
      return rce;
   }
   /*---(walk entries)----------------*/
   x_curr = s_head;
   while (x_curr != NULL) {
      if (a_index == c) break;
      ++c;
      x_curr = x_curr->h_next;
   }
   /*---(check for miss)-----------------*/
   --rce;  if (x_curr == NULL) {
      s_curr  = NULL;
      s_index = -1;
      return rce;
   }
   /*---(save current)-------------------*/
   s_curr  = x_curr;
   s_index = c;
   /*---(complete)-----------------------*/
   return 0;
}

char
HIST__cursor            (char a_move)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   DEBUG_HIST   yLOG_char    ("a_move"    , a_move);
   /*---(defenses)-----------------------*/
   DEBUG_HIST   yLOG_point   ("s_head"    , s_head);
   --rce;  if (s_head == NULL) {
      s_curr  = NULL;
      s_index = -1;
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST   yLOG_point   ("s_curr"    , s_curr);
   --rce;  if (s_curr == NULL && strchr ("<>", a_move) != NULL) {
      s_index = -1;
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST   yLOG_value   ("s_index"   , s_index);
   /*---(handle move)--------------------*/
   DEBUG_HIST   yLOG_note    ("check special shortcuts");
   switch (a_move) {
   case '[' :
      s_curr  = s_head;
      s_index = 0;
      break;
   case '<' :
      s_curr  = s_curr->h_prev;
      --s_index;
      break;
   case '>' :
      s_curr  = s_curr->h_next;
      ++s_index;
      break;
   case ']' :
      s_curr  = s_tail;
      s_index = s_count - 1;
      break;
   }
   /*---(safeties)-----------------------*/
   DEBUG_HIST   yLOG_point   ("s_curr"    , s_curr);
   --rce;  if (s_curr == NULL) {
      s_index = -1;
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(output)-------------------------*/
   DEBUG_HIST   yLOG_point   ("s_curr"    , s_curr);
   DEBUG_HIST   yLOG_value   ("s_index"   , s_index);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     shared helpers                           ----===*/
/*====================------------------------------------====================*/
static void  o___HELPERS_________o () { return; }

char
HIST__concat       (char *a_format, char *a_content, char **a_final)
{
   /*---(locals)-----------+-----+-----+-*/
   char       *p           = NULL;
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   /*---(format)----------------------*/
   DEBUG_HIST  yLOG_point   ("a_format"  , a_format);
   if (a_format == NULL)                           p = g_nothing;
   else {
      DEBUG_HIST  yLOG_info    ("a_format"  , a_format);
      if      (strcmp (a_format, "") == 0)         p = g_nothing;
      else if (strcmp (a_format, g_nothing) == 0)  p = g_nothing;
      else                                         p = a_format;
   }
   DEBUG_HIST  yLOG_complex ("format"    , "%-10.10sn, %-10.10sa, %10.10sp", g_nothing, a_format, p);
   /*---(content)---------------------*/
   DEBUG_HIST  yLOG_point   ("a_content" , a_content);
   if      (a_content  == NULL)                   *a_final  = g_nothing;
   else {
      DEBUG_HIST  yLOG_info    ("a_content" , a_content);
      if      (strcmp (a_content , "") == 0)         *a_final  = g_nothing;
      else    { sprintf (t, "%s··%s", p, a_content); *a_final  = strdup (t); }
   }
   DEBUG_HIST  yLOG_complex ("before"    , "%-10.10sn, %-10.10sa, %10.10sx", g_nothing, a_content, *a_final);
   /*---(show)------------------------*/
   DEBUG_HIST  yLOG_info    ("*a_final"  , *a_final);
   /*---(complete)--------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return 0;
}

char*
HIST__action        (char a_mode, char a_act)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   int         j           = 0;
   strcpy (s_action, "n/f");
   for (i = 0; i < s_hist_nact; ++i) {
      if (s_hist_acts [i].act == 0)      break;
      if (s_hist_acts [i].act != a_act)  continue;
      switch (a_mode) {
      case HIST_BEG :
         strcpy (s_action, s_hist_acts [i].name);
         break;
      case HIST_ADD :
         strcpy (s_action, s_hist_acts [i].name);
         for (j = 0; s_action [j] != '\0'; ++j)  s_action [j] = toupper (s_action [j]);
         break;
      }
      break;
   }
   return s_action;
}



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char         /*-> clear out the history --------------[ leaf   [gz.530.011.00]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
HIST_init          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   /*---(clear vars)---------------------*/
   s_head        = NULL;
   s_tail        = NULL;
   s_curr        = NULL;
   s_count       =  0;
   s_index       = -1;
   hist_active   = 'y';
   /*---(check actions)------------------*/
   s_hist_nact = 0;
   for (i = 0; i < LEN_LABEL; ++i) {
      if (s_hist_acts [i].act == 0)  break;
      ++s_hist_nact;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
HIST_wrap               (void)
{
   HIST__purge ();
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       history additions                      ----===*/
/*====================------------------------------------====================*/
static void  o___ADDS____________o () { return; }

char         /*-> record a cell change ---------------[ leaf   [gz.520.101.00]*/ /*-[01.0000.204.!]-*/ /*-[--.---.---.--]-*/
HIST_change        (char a_mode, int a_tab, int a_col, int a_row, char *a_before, char *a_beforeF, char *a_after)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   /*---(formatting)---------------------*/
   HIST__concat (a_beforeF, a_before, &g_before);
   if (g_before == g_nothing)   HIST__concat ("??0--"  , a_after , &g_after );
   else                         HIST__concat (a_beforeF, a_after , &g_after );
   /*---(add record)---------------------*/
   rc = HIST__new (a_mode, HIST_CHANGE, a_tab, a_col, a_row);
   /*---(complete)--------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> record a cell change ---------------[ leaf   [gz.620.101.00]*/ /*-[01.0000.105.!]-*/ /*-[--.---.---.--]-*/
HIST_overwrite     (int a_tab, int a_col, int a_row, char *a_after, char *a_afterF)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_label     [LEN_LABEL];
   char        x           [LEN_RECD];
   int         i           =    0;
   tHIST      *x_curr      = NULL;
   tHIST      *x_index     = NULL;
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   /*---(make label)---------------------*/
   rc = str4gyges (a_tab, a_col, a_row, 0, 0, x_label, YSTR_USABLE);
   if (rc < 0)  return rc;
   /*---(find label)---------------------*/
   x_curr  = s_curr;
   x_index = s_index;
   HIST__cursor (']');
   while (s_curr != NULL)  {
      DEBUG_HIST  yLOG_complex ("checking"  , "%-10.10s %c %c", s_curr->addr, s_curr->mode, s_curr->act);
      if (s_curr->act == HIST_CHANGE) {
         if (strcmp (x_label , s_curr->addr) == 0) {
            DEBUG_HIST  yLOG_value   ("found"     , s_index);
            break;
         }
      }
      HIST__cursor ('<');
   }
   --rce;  if (s_curr == NULL) {
      s_curr  = x_curr;
      s_index = x_index;
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(change after)-------------------*/
   if (s_curr->after  != g_nothing)   free (s_curr->after);
   HIST__concat (a_afterF , a_after , &g_after );
   s_curr->after   = g_after;
   s_curr->act     = HIST_OVERWRITE;
   /*---(reset currents)-----------------*/
   s_curr  = x_curr;
   s_index = x_index;
   /*---(complete)--------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return 0;
} 

char         /*-> record a cell change ---------------[ leaf   [gz.620.101.00]*/ /*-[01.0000.105.!]-*/ /*-[--.---.---.--]-*/
HIST_delete        (char a_mode, int a_tab, int a_col, int a_row, char *a_before, char *a_beforeF)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x           [LEN_RECD];
   char        s           [LEN_RECD];
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   /*---(add record)---------------------*/
   HIST__concat (a_beforeF, a_before, &g_before);
   HIST__concat (NULL     , NULL    , &g_after );
   rc = HIST__new (a_mode, HIST_DELETE, a_tab, a_col, a_row);
   /*---(complete)--------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> record a cell change ---------------[ leaf   [gz.620.101.00]*/ /*-[01.0000.105.!]-*/ /*-[--.---.---.--]-*/
HIST_clear         (char a_mode, int a_tab, int a_col, int a_row, char *a_before, char *a_beforeF)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x           [LEN_RECD];
   char        s           [LEN_RECD];
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   /*---(add record)---------------------*/
   HIST__concat (a_beforeF, a_before, &g_before);
   HIST__concat (NULL     , NULL    , &g_after );
   rc = HIST__new (a_mode, HIST_CLEAR, a_tab, a_col, a_row);
   /*---(complete)--------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return rc;
}

char
HIST_format        (char a_mode, char a_act, int a_tab, int a_col, int a_row, char a_before, char a_after)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        s           [LEN_RECD];
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   --rce;  if (strchr (HIST_FORMATS, a_act) == NULL) {
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(add record)------------------*/
   sprintf (s, "%c", a_before);
   g_before = strdup  (s);
   sprintf (t, "%c", a_after );
   g_after  = strdup  (t);
   rc = HIST__new (a_mode, a_act, a_tab, a_col, a_row);
   /*---(complete)--------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> record a cell change ---------------[ leaf   [gz.520.101.00]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
HIST_size          (char a_mode, char a_act, int a_tab, int a_col, int a_row, int a_before, int a_after)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        s           [LEN_RECD];
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   --rce;  if (strchr (HIST_SIZES  , a_act) == NULL) {
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(add record)------------------*/
   sprintf (s, "%d", a_before);
   g_before = strdup  (s);
   sprintf (t, "%d", a_after );
   g_after  = strdup  (t);
   rc = HIST__new (a_mode, a_act, a_tab, a_col, a_row);
   /*---(complete)--------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                       undo functions                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNDO____________o () { return; }

char         /*-> take a change away -----------------[ leaf   [gc.J76.043.IA]*/ /*-[03.0000.113.!]-*/ /*-[--.---.---.--]-*/
HIST__undo_single       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_source    [LEN_RECD];
   char        x_format    [LEN_LABEL];
   int         x_tab, x_col, x_row;
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   /*---(identify location)--------------*/
   DEBUG_HIST  yLOG_info    ("label"     , s_curr->addr);
   rc = str2gyges (s_curr->addr, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_ADAPT);
   DEBUG_HIST  yLOG_value   ("str2gyges"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST  yLOG_complex ("jump to"   , "c=%4d, r=%4d, t=%4d", x_col, x_row, x_tab);
   /*---(get to right location)----------*/
   rc = yVIKEYS_jump  (x_tab, x_col, x_row, 0);
   DEBUG_HIST  yLOG_value   ("jump"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(handle request)-----------------*/
   DEBUG_HIST  yLOG_char    ("action"    , s_curr->act);
   DEBUG_HIST  yLOG_info    ("before"    , s_curr->before);
   switch (s_curr->act) {
   case HIST_OVERWRITE :
   case HIST_CLEAR     :
   case HIST_DELETE    :
      sprintf (x_format, "%-5.5s", s_curr->before);
      DEBUG_HIST  yLOG_info    ("x_format"  , x_format);
      if (strlen (s_curr->before) <= 6)  strlcpy (x_source, "", LEN_RECD);
      else                                        strlcpy (x_source, s_curr->before + 7, LEN_RECD);
      DEBUG_HIST  yLOG_info    ("x_source"  , x_source);
      strldchg (x_source, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
      CELL_overwrite (HIST_NONE, x_tab, x_col, x_row, x_source, x_format);
      break;
   case HIST_CHANGE    :
      if (strlen (s_curr->before) <= 6)  strlcpy (x_source, "", LEN_RECD);
      else                                        strlcpy (x_source, s_curr->before + 7, LEN_RECD);
      strldchg (x_source, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
      CELL_change  (NULL, HIST_NONE, x_tab, x_col, x_row, x_source);
      break;
   case HIST_DECIMALS  :
      CELL_visual   (CHANGE_DECIMAL, HIST_NONE, s_curr->before[0]);
      break;
   case HIST_ALIGN     :
      CELL_visual   (CHANGE_ALIGN  , HIST_NONE, s_curr->before[0]);
      break;
   case HIST_FORMAT    :
      CELL_visual   (CHANGE_FORMAT , HIST_NONE, s_curr->before[0]);
      break;
   case HIST_WIDTH     :
      CELL_visual   (CHANGE_WIDTH  , HIST_NONE, -(atoi (s_curr->before)));
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> take a change away -----------------[ leaf   [gc.J76.043.IA]*/ /*-[03.0000.113.!]-*/ /*-[--.---.---.--]-*/
HIST_undo          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_mode      =  '-';
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_HIST  yLOG_value   ("s_count"     , s_count);
   --rce;  if (s_count <  0) {
      DEBUG_HIST  yLOG_note    ("no history to undo");
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST  yLOG_point   ("s_curr"      , s_curr);
   --rce;  if (s_curr == NULL) {
      DEBUG_HIST  yLOG_note    ("current is not set");
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST  yLOG_value   ("s_index"     , s_index);
   --rce;  if (s_index <  0) {
      DEBUG_HIST  yLOG_note    ("current is already at beginning");
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   hist_active = '-';
   /*---(process)------------------------*/
   while (rc == 0) {
      /*---(save mode)-------------------*/
      x_mode = s_curr->mode;
      DEBUG_HIST  yLOG_char    ("curr mode" , x_mode);
      /*---(process current)-------------*/
      rc = HIST__undo_single ();
      if (rc <  0) {
         DEBUG_HIST  yLOG_note    ("error in undo processing");
         DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
      /*---(update position)-------------*/
      HIST__cursor ('<');
      /*---(check for breakpoint)--------*/
      if (x_mode == HIST_BEG) {
         DEBUG_HIST  yLOG_note    ("hit start of undo chain, done");
         break;
      }
      /*---(next)------------------------*/
   }
   /*---(reset)--------------------------*/
   hist_active = 'y';
   /*---(complete)-----------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> put a change back ------------------[ leaf   [gc.B61.042.EA]*/ /*-[02.0000.113.!]-*/ /*-[--.---.---.--]-*/
HIST__redo_single       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_source    [LEN_RECD];
   char        x_format    [LEN_LABEL];
   int         x_tab, x_col, x_row;
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   /*---(identify location)--------------*/
   DEBUG_HIST  yLOG_info    ("label"     , s_curr->addr);
   rc = str2gyges (s_curr->addr, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_ADAPT);
   DEBUG_HIST  yLOG_value   ("str2gyges"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST  yLOG_complex ("jump to"   , "c=%4d, r=%4d, t=%4d", x_col, x_row, x_tab);
   /*---(get to right location)----------*/
   rc = yVIKEYS_jump  (x_tab, x_col, x_row, 0);
   DEBUG_HIST  yLOG_value   ("jump"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(handle request)-----------------*/
   DEBUG_HIST  yLOG_char    ("action"    , s_curr->act);
   DEBUG_HIST  yLOG_info    ("after"     , s_curr->after);
   switch (s_curr->act) {
   case HIST_OVERWRITE :
   case HIST_CLEAR     :
   case HIST_DELETE    :
      sprintf (x_format, "%-5.5s", s_curr->after);
      if (strlen (s_curr->after ) <= 6)  strlcpy (x_source, "", LEN_RECD);
      else                                        strlcpy (x_source, s_curr->after  + 7, LEN_RECD);
      strldchg (x_source, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
      CELL_overwrite (HIST_NONE, x_tab, x_col, x_row, x_source, x_format);
      break;
   case HIST_CHANGE    :
      if (strlen (s_curr->after ) <= 6)  strlcpy (x_source, "", LEN_RECD);
      else                                        strlcpy (x_source, s_curr->after  + 7, LEN_RECD);
      CELL_change  (NULL, HIST_NONE, x_tab, x_col, x_row, x_source);
      strldchg (x_source, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
      break;
   case HIST_DECIMALS  :
      CELL_visual   (CHANGE_DECIMAL, HIST_NONE, s_curr->after [0]);
      break;
   case HIST_ALIGN     :
      CELL_visual   (CHANGE_ALIGN  , HIST_NONE, s_curr->after [0]);
      break;
   case HIST_FORMAT    :
      CELL_visual   (CHANGE_FORMAT , HIST_NONE, s_curr->after [0]);
      break;
   case HIST_WIDTH     :
      CELL_visual   (CHANGE_WIDTH  , HIST_NONE, -(atoi (s_curr->after )));
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> take a change away -----------------[ leaf   [gc.J76.043.IA]*/ /*-[03.0000.113.!]-*/ /*-[--.---.---.--]-*/
HIST_redo          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_mode      =  '-';
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_HIST  yLOG_value   ("s_count"     , s_count);
   --rce;  if (s_count <  0) {
      DEBUG_HIST  yLOG_note    ("no history to undo");
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST  yLOG_value   ("s_index"     , s_index);
   --rce;  if (s_index >= s_count - 1) {
      DEBUG_HIST  yLOG_note    ("current is already at maximum");
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   hist_active = '-';
   if (s_curr == NULL && s_index == -1)  HIST__cursor ('[');
   else                                  HIST__cursor ('>');
   /*---(process)------------------------*/
   while (rc == 0) {
      /*---(process current)-------------*/
      rc = HIST__redo_single ();
      if (rc <  0) {
         DEBUG_HIST  yLOG_note    ("error in redo processing");
         DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
      /*---(update position)-------------*/
      DEBUG_HIST  yLOG_value   ("s_index"     , s_index);
      if (s_curr == NULL) {
         DEBUG_HIST  yLOG_note    ("hit end of of history, done");
         HIST__cursor (']');
         s_index = s_count - 1;
         break;
      }
      /*---(check for breakpoint)--------*/
      if (s_curr->h_next == NULL || s_curr->h_next->mode == HIST_BEG) {
         DEBUG_HIST  yLOG_note    ("hit end of undo chain, done");
         break;
      }
      /*---(next)------------------------*/
      HIST__cursor ('>');
   }
   /*---(reset)--------------------------*/
   hist_active = 'y';
   /*---(complete)-----------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      display and reorting                    ----===*/
/*====================------------------------------------====================*/
static void  o___DISPLAY_________o () { return; }

char         /*-> list history -----------------------[ leaf   [ge.740.042.20]*/ /*-[03.0000.103.!]-*/ /*-[--.---.---.--]-*/
HIST_list          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         c           = 0;
   char        x_name      [100]       = "";
   char        rce         = -10;
   FILE       *x_file      = NULL;
   tHIST      *x_curr      = NULL;
   /*---(open)---------------------------*/
   /*> snprintf (x_name, 95, "%s.s_hist", my.f_title);                                <*/
   strlcpy (x_name, "gyges_history.txt", 100);
   x_file = fopen(x_name, "w");
   --rce;
   if (x_file == NULL)      return rce;
   /*---(print)--------------------------*/
   fprintf (x_file, "details of history\n\n");
   fprintf (x_file, "s_count = %d\n" , s_count);
   fprintf (x_file, "s_index = %d\n" , s_index);
   x_curr = s_head;
   while (x_curr != NULL) {
      if (c % 5 == 0)  fprintf (x_file, "\n-ref- § mod § nkey § ---action----- § ---addr--- § ---before----------------------------------------- § ---after------------------------------------------ §\n");
      fprintf (x_file, "%-5d §  %c  § %-4d § %c.%-12.12s § %-10.10s § %-50.50s § %-50.50s §\n",
            c                , x_curr->mode  , x_curr->nkey  ,
            x_curr->act   , HIST__action (x_curr->mode, x_curr->act),
            x_curr->addr  , x_curr->before, x_curr->after );
      ++c;
      x_curr = x_curr->h_next;
   }
   fprintf (x_file, "\ndone\n");
   /*---(close)--------------------------*/
   fclose (x_file);
   /*---(complete)-----------------------*/
   return 0;
}


char
CURS_status_history  (char *a_list)
{
   if (s_curr != NULL) {
      snprintf (a_list, LEN_FULL, "mundo   %3d/%3d %c %-7.7s [%-25.25s] [%-25.25s]", s_index + 1, s_count, s_curr->act, s_curr->addr, s_curr->before, s_curr->after);
   } else {
      snprintf (a_list, LEN_FULL, "mundo   %3d/%3d %c %-7.7s [%-25.25s] [%-25.25s]", s_index + 1, s_count, '-', "-", "-", "-");
   }
   return 0;
}



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
      x_curr = s_head; while (x_curr != NULL) { ++x_fore; x_curr = x_curr->h_next; }
      x_curr = s_tail; while (x_curr != NULL) { ++x_back; x_curr = x_curr->h_prev; }
      snprintf (unit_answer, LEN_FULL, "HIST count       : %4dn, %4df, %4db, %4di", s_count, x_fore, x_back, s_index);
   } else if (strcmp (a_question, "current"   )    == 0) {
      if      (s_curr == NULL)    snprintf (unit_answer, LEN_FULL, "HIST curr   (--) : -       ,  -t,   -c,   -r, - -           0[]              0[]");
      else {
         rc = str2gyges (s_curr->addr, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_USABLE);
         if (strlen (s_curr->before) < 13)  sprintf  (b, "%2d[%s]"      , strlen (s_curr->before), s_curr->before);
         else                               sprintf  (b, "%2d[%-12.12s>", strlen (s_curr->before), s_curr->before);
         if (strlen (s_curr->after ) < 13)  sprintf  (a, "%2d[%s]"      , strlen (s_curr->after ), s_curr->after );
         else                               sprintf  (a, "%2d[%-12.12s>", strlen (s_curr->after ), s_curr->after );
         snprintf (unit_answer, LEN_FULL, "HIST curr   (%2d) : %-8.8s, %2dt, %3dc, %3dr, %c %-10.10s %-16.16s %s", s_index, s_curr->addr, x_tab, x_col, x_row, s_curr->mode, HIST__action (s_curr->mode, s_curr->act), b, a);
      }
   } else if (strcmp (a_question, "entry"     )    == 0) {
      x_curr  = s_curr;
      x_index = s_index;
      HIST__find (a_ref);
      if      (s_curr == NULL)    snprintf (unit_answer, LEN_FULL, "HIST entry  (%2d) : -       ,  -t,   -c,   -r, - -           0[]              0[]", a_ref);
      else {
         rc = str2gyges (s_curr->addr, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_USABLE);
         if (strlen (s_curr->before) < 13)  sprintf  (b, "%2d[%s]"      , strlen (s_curr->before), s_curr->before);
         else                               sprintf  (b, "%2d[%-12.12s>", strlen (s_curr->before), s_curr->before);
         if (strlen (s_curr->after ) < 13)  sprintf  (a, "%2d[%s]"      , strlen (s_curr->after ), s_curr->after );
         else                               sprintf  (a, "%2d[%-12.12s>", strlen (s_curr->after ), s_curr->after );
         snprintf (unit_answer, LEN_FULL, "HIST entry  (%2d) : %-8.8s, %2dt, %3dc, %3dr, %c %-10.10s %-16.16s %s", a_ref, s_curr->addr, x_tab, x_col, x_row, s_curr->mode, HIST__action (s_curr->mode, s_curr->act), b, a);
      }
      s_curr  = x_curr;
      s_index = x_index;
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}
