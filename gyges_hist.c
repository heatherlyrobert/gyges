/*============================----beg-of-source---============================*/
#include   "gyges.h"



/*---(major changes)-------------*/
#define     HIST_OVERWRITE    'O'
#define     HIST_CHANGE       'S'
#define     HIST_DELETE       'D'
#define     HIST_CLEAR        'X'
/*---(minor changes)-------------*/
#define     HIST_DECIMALS     'd'
#define     HIST_ALIGN        'a'
#define     HIST_FORMAT       'f'
#define     HIST_WIDTH        'w'
#define     HIST_HEIGHT       'h'


#define     MAX_HIST    100000
struct cHIST {
   char        mode;
   char        act;
   int         nkey;
   char        addr        [LEN_LABEL];
   char        before      [LEN_RECD];
   char        after       [LEN_RECD];
};
static tHIST   s_hist      [MAX_HIST];
static int     s_nhist;
static int     s_chist;

char           hist_active;

struct {
   char        act;
   char        name         [LEN_LABEL];
   char        desc         [LEN_DESC];
} s_hist_acts [LEN_LABEL] = {
   { HIST_OVERWRITE, "overwrite"    , "" },
   { HIST_CHANGE   , "change"       , "" },
   { HIST_DELETE   , "delete"       , "" },
   { HIST_CLEAR    , "clear"        , "" },
   { HIST_DECIMALS , "decimals"     , "" },
   { HIST_ALIGN    , "align"        , "" },
   { HIST_FORMAT   , "format"       , "" },
   { HIST_WIDTH    , "width"        , "" },
   { HIST_HEIGHT   , "height"       , "" },
   { 0             , "end"          , "" },
};
int         s_hist_nact =    0;


/*====================------------------------------------====================*/
/*===----                       history recording                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___HISTORY_________o () { return; }

char         /*-> clear out the history --------------[ leaf   [gz.530.011.00]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
HIST_init          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   /*---(clear list)---------------------*/
   for (i = 0; i < MAX_HIST; ++i) {
      s_hist [i].mode  = '-';
      s_hist [i].act   = '-';
      s_hist [i].nkey  = -1;
      strcpy (s_hist [i].before, "");
      strcpy (s_hist [i].after , "");
   }
   /*---(clear vars)---------------------*/
   s_nhist       =  0;
   s_chist       = -1;
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

char*
HIST__action        (char a_act)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   for (i = 0; i < s_hist_nact; ++i) {
      if (s_hist_acts [i].act == 0)      break;
      if (s_hist_acts [i].act != a_act)  continue;
      return s_hist_acts [i].name;
   }
   return "n/f";
}

char         /*-> record a cell change ---------------[ leaf   [gz.520.101.00]*/ /*-[01.0000.204.!]-*/ /*-[--.---.---.--]-*/
HIST_debug         (void)
{
   char        x_recd      [LEN_RECD ];
   sprintf (x_recd, "#%d, %s, %c", s_chist,
         s_hist [s_chist].addr, s_hist [s_chist].act);
   DEBUG_HIST  
      DEBUG_HIST  yLOG_info    ("record"    , x_recd);
   return 0;
}

char
HIST_single        (char a_mode, char a_type, int a_tab, int a_col, int a_row, char *a_before, char *a_after)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x_label     [LEN_LABEL];
   char        t           [LEN_RECD];
   /*---(internal)-----------------------*/
   if (hist_active != 'y')        return 0;
   if (a_mode      == HIST_NONE)  return 0;
   /*---(update pointers)----------------*/
   ++s_chist;
   s_nhist = s_chist + 1;
   /*---(mode/action)--------------------*/
   s_hist [s_chist].mode  = a_mode;
   s_hist [s_chist].act   = a_type;
   s_hist [s_chist].nkey  = yVIKEYS_keys_nkey ();
   /*---(address)------------------------*/
   rc = str4gyges (a_tab, a_col, a_row, 0, x_label);
   strlcpy (s_hist [s_chist].addr  , x_label, LEN_LABEL);
   /*---(before)-------------------------*/
   strlcpy  (t, a_before, LEN_RECD);
   if (strlen (t) > 5)  strldchg (t + 5, G_KEY_SPACE, G_CHAR_SPACE, LEN_RECD);
   strlcpy (s_hist [s_chist].before, t, LEN_RECD);
   /*---(after)--------------------------*/
   strlcpy  (t, a_after , LEN_RECD);
   if (strlen (t) > 5)  strldchg (t + 5, G_KEY_SPACE, G_CHAR_SPACE, LEN_RECD);
   strlcpy (s_hist [s_chist].after , t , LEN_RECD);
   /*---(debugging)----------------------*/
   HIST_debug ();
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> record a cell change ---------------[ leaf   [gz.620.101.00]*/ /*-[01.0000.105.!]-*/ /*-[--.---.---.--]-*/
HIST_overwrite     (char a_mode, int a_tab, int a_col, int a_row, char *a_afterF)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x_label     [LEN_LABEL];
   char        x           [LEN_RECD];
   int         i           =    0;
   /*---(make label)---------------------*/
   rc = str4gyges (a_tab, a_col, a_row, 0, x_label);
   if (rc < 0)  return rc;
   if (a_afterF  != NULL)  sprintf (x, "%s", a_afterF);
   else                    sprintf (x, "´´´");
   /*---(add record)---------------------*/
   for (i = s_chist; i >= 0; --i) {
      if (s_hist [i].act != HIST_CHANGE)            continue;
      if (strcmp (x_label , s_hist [i].addr) != 0)  continue;
      s_hist [s_chist].act       = HIST_OVERWRITE;
      s_hist [s_chist].after [0] = x [0];
      s_hist [s_chist].after [1] = x [1];
      s_hist [s_chist].after [2] = x [2];
      return 0;
   }
   /*---(complete)--------------------*/
   return -1;
}

char         /*-> record a cell change ---------------[ leaf   [gz.520.101.00]*/ /*-[01.0000.204.!]-*/ /*-[--.---.---.--]-*/
HIST_change        (char a_mode, int a_tab, int a_col, int a_row, char *a_before, char *a_beforeF, char *a_after)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x           [LEN_RECD];
   char        s           [LEN_RECD];
   char        t           [LEN_RECD];
   /*---(add record)---------------------*/
   if (a_beforeF != NULL)  sprintf (x, "%s", a_beforeF);
   else                    sprintf (x, "´´´");
   if (a_before  != NULL)  sprintf (s, "%s  %s", x, a_before);
   else                    sprintf (s, "%s  ", x);
   if (a_after   != NULL)  sprintf (t, "´´´  %s", a_after);
   else                    sprintf (t, "´´´  ");
   rc = HIST_single (a_mode, HIST_CHANGE, a_tab, a_col, a_row, s, t);
   /*---(complete)--------------------*/
   return rc;
}

char         /*-> record a cell change ---------------[ leaf   [gz.620.101.00]*/ /*-[01.0000.105.!]-*/ /*-[--.---.---.--]-*/
HIST_delete        (char a_mode, int a_tab, int a_col, int a_row, char *a_before, char *a_beforeF)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x           [LEN_RECD];
   char        s           [LEN_RECD];
   char        t           [LEN_RECD];
   /*---(add record)---------------------*/
   if (a_beforeF != NULL)  sprintf (x, "%s", a_beforeF);
   else                    sprintf (x, "´´´");
   if (a_before  != NULL)  sprintf (s, "%s  %s", x, a_before);
   else                    sprintf (s, "%s  "  , x);
   sprintf (t, "---  ");
   rc = HIST_single (a_mode, HIST_DELETE, a_tab, a_col, a_row, s, t);
   /*---(complete)--------------------*/
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
   /*---(add record)---------------------*/
   if (a_beforeF != NULL)  sprintf (x, "%s", a_beforeF);
   else                    sprintf (x, "´´´");
   if (a_before  != NULL)  sprintf (s, "%s  %s", x, a_before);
   else                    sprintf (s, "%s  "  , x);
   sprintf (t, "???  ");
   rc = HIST_single (a_mode, HIST_CLEAR, a_tab, a_col, a_row, s, t);
   /*---(complete)--------------------*/
   return rc;
}

char         /*-> record a cell change ---------------[ leaf   [gz.520.101.00]*/ /*-[01.0000.304.!]-*/ /*-[--.---.---.--]-*/
HIST_decimals      (char a_mode, int a_tab, int a_col, int a_row, char a_before, char a_after)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        s           [LEN_RECD];
   char        t           [LEN_RECD];
   /*---(add record)------------------*/
   sprintf (s, "%c", a_before);
   sprintf (t, "%c", a_after );
   rc = HIST_single (a_mode, HIST_DECIMALS, a_tab, a_col, a_row, s, t);
   /*---(complete)--------------------*/
   return rc;
}

char         /*-> record a cell change ---------------[ leaf   [gz.520.101.00]*/ /*-[01.0000.304.!]-*/ /*-[--.---.---.--]-*/
HIST_align         (char a_mode, int a_tab, int a_col, int a_row, char a_before, char a_after)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        s           [LEN_RECD];
   char        t           [LEN_RECD];
   /*---(add record)------------------*/
   sprintf (s, "%c", a_before);
   sprintf (t, "%c", a_after );
   rc = HIST_single (a_mode, HIST_ALIGN, a_tab, a_col, a_row, s, t);
   /*---(complete)--------------------*/
   return rc;
}

char         /*-> record a cell change ---------------[ leaf   [gz.520.101.00]*/ /*-[01.0000.304.!]-*/ /*-[--.---.---.--]-*/
HIST_format        (char a_mode, int a_tab, int a_col, int a_row, char a_before, char a_after)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        s           [LEN_RECD];
   char        t           [LEN_RECD];
   /*---(add record)------------------*/
   sprintf (s, "%c", a_before);
   sprintf (t, "%c", a_after );
   rc = HIST_single (a_mode, HIST_FORMAT, a_tab, a_col, a_row, s, t);
   /*---(complete)--------------------*/
   return rc;
}

char         /*-> record a cell change ---------------[ leaf   [gz.520.101.00]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
HIST_width         (char a_mode, int a_tab, int a_col, int a_row, int a_before, int a_after)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        s           [LEN_RECD];
   char        t           [LEN_RECD];
   /*---(add record)------------------*/
   sprintf (s, "%d", a_before);
   sprintf (t, "%d", a_after );
   rc = HIST_single (a_mode, HIST_WIDTH, a_tab, a_col, a_row, s, t);
   /*---(complete)--------------------*/
   return rc;
}

char         /*-> record a cell change ---------------[ leaf   [gz.520.101.00]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
HIST_height        (char a_mode, int a_tab, int a_col, int a_row, int a_before, int a_after)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        s           [LEN_RECD];
   char        t           [LEN_RECD];
   /*---(add record)------------------*/
   sprintf (s, "%d", a_before);
   sprintf (t, "%d", a_after );
   rc = HIST_single (a_mode, HIST_HEIGHT, a_tab, a_col, a_row, s, t);
   /*---(complete)--------------------*/
   return rc;
}

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
   DEBUG_HIST  yLOG_info    ("label"     , s_hist [s_chist].addr);
   rc = str2gyges (s_hist [s_chist].addr, &x_tab, &x_col, &x_row, NULL, 0);
   DEBUG_HIST  yLOG_value   ("str2gyges"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST  yLOG_complex ("jump to"   , "c=%4d, r=%4d, t=%4d", x_col, x_row, x_tab);
   /*---(get to right location)----------*/
   rc = yVIKEYS_jump  (x_col, x_row, x_tab);
   DEBUG_HIST  yLOG_value   ("jump"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(handle request)-----------------*/
   DEBUG_HIST  yLOG_char    ("action"    , s_hist [s_chist].act);
   DEBUG_HIST  yLOG_info    ("before"    , s_hist [s_chist].before);
   switch (s_hist [s_chist].act) {
   case HIST_OVERWRITE :
   case HIST_CLEAR     :
   case HIST_DELETE    :
      sprintf (x_format, "%c%c%c", s_hist [s_chist].before[0], s_hist [s_chist].before[1], s_hist [s_chist].before[2]);
      if (strlen (s_hist [s_chist].before) <= 5)  strlcpy (x_source, "", LEN_RECD);
      else                                        strlcpy (x_source, s_hist [s_chist].before + 5, LEN_RECD);
      strldchg (x_source, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
      CELL_overwrite (HIST_NONE, x_tab, x_col, x_row, x_source, x_format);
      break;
   case HIST_CHANGE    :
      if (strlen (s_hist [s_chist].before) <= 5)  strlcpy (x_source, "", LEN_RECD);
      else                                        strlcpy (x_source, s_hist [s_chist].before + 5, LEN_RECD);
      strldchg (x_source, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
      CELL_change  (NULL, HIST_NONE, x_tab, x_col, x_row, x_source);
      break;
   case HIST_DECIMALS  :
      CELL_visual   (CHANGE_DECIMAL, HIST_NONE, s_hist[s_chist].before[0]);
      break;
   case HIST_ALIGN     :
      CELL_visual   (CHANGE_ALIGN  , HIST_NONE, s_hist[s_chist].before[0]);
      break;
   case HIST_FORMAT    :
      CELL_visual   (CHANGE_FORMAT , HIST_NONE, s_hist[s_chist].before[0]);
      break;
   case HIST_WIDTH     :
      CELL_visual   (CHANGE_WIDTH  , HIST_NONE, -(atoi (s_hist[s_chist].before)));
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
   DEBUG_HIST  yLOG_value   ("s_nhist"     , s_nhist);
   --rce;  if (s_nhist <  0) {
      DEBUG_HIST  yLOG_note    ("no history to undo");
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST  yLOG_value   ("s_chist"     , s_chist);
   --rce;  if (s_chist <  0) {
      DEBUG_HIST  yLOG_note    ("current is already at beginning");
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   hist_active = '-';
   /*---(process)------------------------*/
   while (rc == 0) {
      /*---(save mode)-------------------*/
      x_mode = s_hist [s_chist].mode;
      DEBUG_HIST  yLOG_char    ("curr mode" , x_mode);
      /*---(process current)-------------*/
      rc = HIST__undo_single ();
      if (rc <  0) {
         DEBUG_HIST  yLOG_note    ("error in undo processing");
         DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
      /*---(update position)-------------*/
      --s_chist;
      DEBUG_HIST  yLOG_value   ("s_chist"     , s_chist);
      if (s_chist <  0) {
         DEBUG_HIST  yLOG_note    ("hit beginning of history, done");
         break;
      }
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
   DEBUG_HIST  yLOG_info    ("label"     , s_hist [s_chist].addr);
   rc = str2gyges (s_hist [s_chist].addr, &x_tab, &x_col, &x_row, NULL, 0);
   DEBUG_HIST  yLOG_value   ("str2gyges"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST  yLOG_complex ("jump to"   , "c=%4d, r=%4d, t=%4d", x_col, x_row, x_tab);
   /*---(get to right location)----------*/
   rc = yVIKEYS_jump  (x_col, x_row, x_tab);
   DEBUG_HIST  yLOG_value   ("jump"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(handle request)-----------------*/
   DEBUG_HIST  yLOG_char    ("action"    , s_hist [s_chist].act);
   DEBUG_HIST  yLOG_info    ("after"     , s_hist [s_chist].after);
   switch (s_hist [s_chist].act) {
   case HIST_OVERWRITE :
   case HIST_CLEAR     :
   case HIST_DELETE    :
      sprintf (x_format, "%c%c%c", s_hist [s_chist].after [0], s_hist [s_chist].after [1], s_hist [s_chist].after [2]);
      if (strlen (s_hist [s_chist].after ) <= 5)  strlcpy (x_source, "", LEN_RECD);
      else                                        strlcpy (x_source, s_hist [s_chist].after  + 5, LEN_RECD);
      strldchg (x_source, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
      CELL_overwrite (HIST_NONE, x_tab, x_col, x_row, x_source, x_format);
      break;
   case HIST_CHANGE    :
      if (strlen (s_hist [s_chist].after ) <= 5)  strlcpy (x_source, "", LEN_RECD);
      else                                        strlcpy (x_source, s_hist [s_chist].after  + 5, LEN_RECD);
      CELL_change  (NULL, HIST_NONE, x_tab, x_col, x_row, x_source);
      strldchg (x_source, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
      break;
   case HIST_DECIMALS  :
      CELL_visual   (CHANGE_DECIMAL, HIST_NONE, s_hist[s_chist].after [0]);
      break;
   case HIST_ALIGN     :
      CELL_visual   (CHANGE_ALIGN  , HIST_NONE, s_hist[s_chist].after [0]);
      break;
   case HIST_FORMAT    :
      CELL_visual   (CHANGE_FORMAT , HIST_NONE, s_hist[s_chist].after [0]);
      break;
   case HIST_WIDTH     :
      CELL_visual   (CHANGE_WIDTH  , HIST_NONE, -(atoi (s_hist[s_chist].after )));
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
   DEBUG_HIST  yLOG_value   ("s_nhist"     , s_nhist);
   --rce;  if (s_nhist <  0) {
      DEBUG_HIST  yLOG_note    ("no history to undo");
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST  yLOG_value   ("s_chist"     , s_chist);
   --rce;  if (s_chist >= s_nhist - 1) {
      DEBUG_HIST  yLOG_note    ("current is already at maximum");
      DEBUG_HIST  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   hist_active = '-';
   ++s_chist;
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
      DEBUG_HIST  yLOG_value   ("s_chist"     , s_chist);
      if (s_chist >= s_nhist - 1) {
         DEBUG_HIST  yLOG_note    ("hit end of of history, done");
         s_chist = s_nhist - 1;
         break;
      }
      /*---(check for breakpoint)--------*/
      x_mode = s_hist [s_chist + 1].mode;
      DEBUG_HIST  yLOG_char    ("curr mode" , x_mode);
      if (x_mode == HIST_BEG) {
         DEBUG_HIST  yLOG_note    ("hit end of undo chain, done");
         break;
      }
      /*---(next)------------------------*/
      ++s_chist;
   }
   /*---(reset)--------------------------*/
   hist_active = 'y';
   /*---(complete)-----------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> list history -----------------------[ leaf   [ge.740.042.20]*/ /*-[03.0000.103.!]-*/ /*-[--.---.---.--]-*/
HIST_list          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_name      [100]       = "";
   char        rce         = -10;
   FILE       *x_file      = NULL;
   /*---(open)---------------------------*/
   /*> snprintf (x_name, 95, "%s.s_hist", my.f_title);                                <*/
   strlcpy (x_name, "gyges_history.txt", 100);
   x_file = fopen(x_name, "w");
   --rce;
   if (x_file == NULL)      return rce;
   /*---(print)--------------------------*/
   fprintf (x_file, "details of history\n\n");
   fprintf (x_file, "s_nhist = %d\n" , s_nhist);
   fprintf (x_file, "s_chist = %d\n" , s_chist);
   for (i = 0; i < s_nhist; ++i) {
      if (i % 5 == 0)  fprintf (x_file, "\n-ref- § mod § nkey § ---action----- § ---addr--- § ---before----------------------------------------- § ---after------------------------------------------ §\n");
      fprintf (x_file, "%-5d §  %c  § %-4d § %c.%-12.12s § %-10.10s § %-50.50s § %-50.50s §\n",
            i                , s_hist [i].mode  , s_hist [i].nkey  ,
            s_hist [i].act   , HIST__action (s_hist [i].act),
            s_hist [i].addr  , s_hist [i].before, s_hist [i].after );
   }
   fprintf (x_file, "\ndone\n");
   /*---(close)--------------------------*/
   fclose (x_file);
   /*---(complete)-----------------------*/
   return 0;
}


char  CURS_status_history  (char *a_list) { snprintf (a_list, LEN_STR, "[ s_nhist : %4d, s_chist : %4d, top : %s ]", s_nhist, s_chist, s_hist [s_chist].act); }

char*        /*-> unit test accessor -----------------[ ------ [gs.950.221.M1]*/ /*-[03.0000.00#.#]-*/ /*-[--.---.---.--]-*/
HIST__unit         (char *a_question, int a_ref)
{
   /*---(locals)-----------+-----------+-*/
   int         rc          = -1;
   char        x_name      [LEN_LABEL];
   int         x_tab, x_col, x_row;
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "HIST             : question not understood");
   /*---(selection)----------------------*/
   if        (strcmp (a_question, "history"   )    == 0) {
      if      (s_nhist == 0    )  snprintf (unit_answer, LEN_UNIT, "HIST count       : n=%4d, c=%4d, n/a", s_nhist, s_chist);
      if      (s_chist <  0    )  snprintf (unit_answer, LEN_UNIT, "HIST count       : n=%4d, c=%4d, n/a", s_nhist, s_chist);
      else                        snprintf (unit_answer, LEN_UNIT, "HIST count       : n=%4d, c=%4d, %c" , s_nhist, s_chist, s_hist[s_chist].act);
   } else if (strcmp (a_question, "entry"     )    == 0) {
      if      (a_ref <  0    )    snprintf (unit_answer, LEN_UNIT, "HIST entry       : %4d too small", a_ref);
      else if (a_ref >= s_nhist)  snprintf (unit_answer, LEN_UNIT, "HIST entry       : %4d too large", a_ref);
      else {
         rc = str2gyges (s_hist [s_chist].addr, &x_tab, &x_col, &x_row, NULL, 0);
         snprintf (unit_answer, LEN_UNIT, "HIST entry       : %4d, t=%4d, c=%4d, r=%4d, %c", a_ref, x_tab, x_col, x_row, s_hist[a_ref].act);
      }
   } else if (strcmp (a_question, "before"    )    == 0) {
      if      (a_ref <  0    )    snprintf (unit_answer, LEN_UNIT, "HIST before      : %4d too small", a_ref);
      else if (a_ref >= s_nhist)  snprintf (unit_answer, LEN_UNIT, "HIST before      : %4d too large", a_ref);
      else                        snprintf (unit_answer, LEN_UNIT, "HIST before      : %4d :%s:", a_ref, s_hist[a_ref].before);
   } else if (strcmp (a_question, "after"     )    == 0) {
      if      (a_ref <  0    )    snprintf (unit_answer, LEN_UNIT, "HIST after       : %4d too small", a_ref);
      else if (a_ref >= s_nhist)  snprintf (unit_answer, LEN_UNIT, "HIST after       : %4d too large", a_ref);
      else                        snprintf (unit_answer, LEN_UNIT, "HIST after       : %4d :%s:", a_ref, s_hist[a_ref].after);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}
