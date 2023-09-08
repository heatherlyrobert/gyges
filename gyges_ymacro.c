/*============================----beg-of-source---============================*/
#include   "gyges.h"

static char  *s_valid  = ".0123456789abcdefghijklmnopqrstuvwxyzèéêëìíîïðñòóôõö÷øùúûüýþÿ ¤";
static char  *s_regs   = "¶0123456789abcdefghijklmnopqrstuvwxyzèéêëìíîïðñòóôõö÷øùúûüýþÿ";


char
api_ymacro_init             (void)
{
   /*--(locals)------------+-----+-----+-*/
   char        l, i;
   char       *x_locals    = "·¯>{!·0123456789·?·abcdefghijklmnopqrstuvwxyz";
   char        t           [LEN_LABEL] = "";
   /*---(macros)-------------------------*/
   l = strlen (s_valid);
   CELL_overwrite (YMAP_NONE , 37, 0, 0, "abbr"    , "<-0-·");
   CELL_overwrite (YMAP_NONE , 37, 1, 0, "keys"    , "<-0-·");
   for (i = 0; i < l; ++i) {
      sprintf (t, "Ö@%c", s_valid [i]);
      CELL_overwrite (YMAP_NONE , 37, 0, i + 1, t   , "|?0-·");
   }
   COL_resize (37, 0,  5);
   COL_resize (37, 1, 30);
   COL_resize (37, 2,  3);
   /*---(registers)----------------------*/
   l = strlen (s_regs);
   CELL_overwrite (YMAP_NONE , 37, 0, 70, "sreg"    , "<-0-·");
   CELL_overwrite (YMAP_NONE , 37, 1, 70, "content" , "<-0-·");
   for (i = 0; i < l; ++i) {
      sprintf (t, "Ö%c", s_regs [i]);
      CELL_overwrite (YMAP_NONE , 37, 0, i + 71, t   , "|?0-·");
   }
   /*---(agrios locals)------------------*/
   CELL_overwrite (YMAP_NONE , 37, 3, 0, "agrios"  , "<-0-·");
   CELL_overwrite (YMAP_NONE , 37, 3, 1, "curr"    , ">?0-·");
   CELL_overwrite (YMAP_NONE , 37, 3, 2, "lvl"     , ">?0-·");
   CELL_overwrite (YMAP_NONE , 37, 3, 3, "exec"    , ">?0-·");
   CELL_overwrite (YMAP_NONE , 37, 3, 4, "code"    , ">?0-·");
   CELL_overwrite (YMAP_NONE , 37, 3, 5, "next"    , ">?0-·");
   CELL_overwrite (YMAP_NONE , 37, 3, 7, "args"    , ">?0-·");
   CELL_overwrite (YMAP_NONE , 37, 3,18, "rc"      , ">?0-·");
   CELL_overwrite (YMAP_NONE , 37, 3,20, "locals"  , ">?0-·");
   CELL_overwrite (YMAP_NONE , 37, 3,47, "temp"    , "<?0-·");
   CELL_overwrite (YMAP_NONE , 37, 4, 0, "vars"    , "<-0-·");
   CELL_overwrite (YMAP_NONE , 37, 4, 4, "····"    , "|?0-·");
   CELL_overwrite (YMAP_NONE , 37, 5, 0, "stack"   , "<-0-·");
   CELL_overwrite (YMAP_NONE , 37, 6, 0, "<"       , "<-0-·");
   CELL_overwrite (YMAP_NONE , 37, 7, 0, "<"       , "<-0-·");
   CELL_overwrite (YMAP_NONE , 37, 8, 0, "<"       , "<-0-·");
   CELL_overwrite (YMAP_NONE , 37, 9, 0, "<"       , "<-0-·");
   CELL_overwrite (YMAP_NONE , 37,10, 0, "<"       , "<-0-·");
   CELL_overwrite (YMAP_NONE , 37, 5, 2, "0"       , "|?0-·");
   CELL_overwrite (YMAP_NONE , 37, 4,47, "····················", "<?0-·");
   CELL_overwrite (YMAP_NONE , 37, 5,47, "<"       , "<?0-·");
   CELL_overwrite (YMAP_NONE , 37, 6,47, "<"       , "<?0-·");
   l = strlen (x_locals);
   for (i = 0; i < l; ++i) {
      if (x_locals [i] == '·')  continue;
      sprintf (t, "Öç%c", x_locals [i]);
      CELL_overwrite (YMAP_NONE , 37, 4, i + 1, t   , "<?0-·");
   }
   CELL_overwrite (YMAP_NONE , 37, 4, 1, "Ö •"     , "<?0-·");
   COL_resize (37, 4,  5);
   CELL_overwrite (YMAP_NONE , 37, 3,50, "sum"     , ">?0-·");
   CELL_overwrite (YMAP_NONE , 37, 4,50, "Öç®"     , "<?0-·");
   CELL_overwrite (YMAP_NONE , 37, 3,51, "int"     , ">?0-·");
   CELL_overwrite (YMAP_NONE , 37, 4,51, "Öçã"     , "<?0-·");
   return 0;
}

char
api_ysrc_saver          (char a_reg, char *a_content)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char       *p           = NULL;
   char        n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_YSRC     yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YSRC     yLOG_char    ("a_reg"     , a_reg);
   --rce;  if (a_reg == 0) {
      DEBUG_YSRC     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (a_reg == '"')  a_reg = '¶';
   DEBUG_YSRC     yLOG_point   ("a_content" , a_content);
   --rce;  if (a_content == NULL) {
      DEBUG_YSRC     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YSRC     yLOG_info    ("a_content" , a_content);
   /*---(find)---------------------------*/
   p = strchr (s_regs, a_reg);
   DEBUG_YSRC     yLOG_point   ("p"         , p);
   --rce;  if (p == NULL) {
      DEBUG_YSRC     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   n = p - s_regs;
   /*---(update)-------------------------*/
   CELL_change (NULL, YMAP_NONE , 37, 1, 71 + n, a_content);
   /*---(complete)-----------------------*/
   DEBUG_YSRC     yLOG_exit    (__FUNCTION__);
   return 0;
}


/*====================------------------------------------====================*/
/*===----                     macro enablers                           ----===*/
/*====================------------------------------------====================*/
static void   o___MACRO___________o (void) { return; }

char         /*-> tbd --------------------------------[ ------ [ge.540.136.51]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
api_ymacro__saver       (int y, char *a_keys)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (y < 1)           return rce;
   --rce;  if (a_keys == NULL)  return rce;
   /*---(update)-------------------------*/
   CELL_change (NULL, YMAP_NONE , 37, 1, y, a_keys);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.540.136.51]*/ /*-[01.0000.104.!]-*/ /*-[--.---.---.--]-*/
api_ymacro_get          (char a_name, char *a_keys)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *p           = NULL;
   short       x_row       =    0;
   tCELL      *x_curr      = NULL;
   int         l           =    0;
   char        x_label     [LEN_LABEL] = "";
   int         u, x, y;
   /*---(header)-------------------------*/
   DEBUG_YMACRO   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YMACRO   yLOG_value   ("a_name"    , a_name);
   --rce;  if (a_name == 0) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   p = strchr (s_valid, a_name);
   DEBUG_YMACRO   yLOG_point   ("p"         , p);
   --rce;  if (p == NULL) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMACRO   yLOG_point   ("a_keys"    , a_keys);
   --rce;  if (a_keys    == NULL) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMACRO   yLOG_info    ("a_keys"    , a_keys);
   /*---(check alternate)----------------*/
   l = strlen (a_keys);
   DEBUG_YMACRO   yLOG_value   ("l"         , l);
   if (l > 0) {
      if (strcmp (a_keys, "(cur)") == 0) {
         u  = CTAB;
         x  = CCOL;
         y  = CROW;
         rc = 0;
      } else {
         rc  = str2gyges (a_keys, &u, &x, &y, NULL, NULL, 0, YSTR_LEGAL);
         DEBUG_YMACRO   yLOG_value   ("str2gyges" , rc);
      }
      if (rc >= 0) {
         strlcpy (x_label, a_keys, LEN_LABEL);
         DEBUG_YMACRO   yLOG_complex ("current"   , "%2du, %3dx, %3dy", u, x, y);
      }
      DEBUG_YMACRO   yLOG_note    ("done looking for alternate");
   }
   /*---(get normally)-------------------*/
   x_row = p - s_valid + 1;
   DEBUG_YMACRO   yLOG_value   ("x_row"     , x_row);
   x_curr = LOC_cell_at_loc (37, 1, x_row);
   DEBUG_YMACRO   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr    == NULL) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  switch (x_curr->type) {
   case YCALC_DATA_STR :
      strlcpy (a_keys, x_curr->source, LEN_RECD);
      break;
   case YCALC_DATA_SFORM : case YCALC_DATA_SLIKE :
      strlcpy (a_keys, x_curr->v_str , LEN_RECD);
      break;
   default :
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      strlcpy (a_keys, ""            , LEN_RECD);
      return rce;
      break;
   }
   /*---(save here)----------------------*/
   if (strcmp (x_label, "") != NULL) {
      CELL_change (NULL, YMAP_NONE , u, x, y, a_keys);
   }
   /*---(complete)-----------------------*/
   DEBUG_YMACRO   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.540.153.42]*/ /*-[01.0000.105.!]-*/ /*-[--.---.---.--]-*/
api_ymacro_set          (char a_name, char *a_keys)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *p           = NULL;
   short       x_row       =    0;
   int         x_len       =    0;
   tCELL      *x_curr      = NULL;
   int         l           =    0;
   char        x_label     [LEN_LABEL] = "";
   int         u, x, y;
   /*---(header)-------------------------*/
   DEBUG_YMACRO   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YMACRO   yLOG_char    ("a_name"    , a_name);
   --rce;  if (a_name == 0) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   p = strchr (s_valid, a_name);
   DEBUG_YMACRO   yLOG_point   ("p"         , p);
   --rce;  if (p == NULL) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMACRO   yLOG_point   ("a_keys"    , a_keys);
   --rce;  if (a_keys    == NULL) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMACRO   yLOG_info    ("a_keys"    , a_keys);
   /*---(check alternate)----------------*/
   l = strlen (a_keys);
   DEBUG_YMACRO   yLOG_value   ("l"         , l);
   if (l > 0) {
      if (strcmp (a_keys, "(cur)") == 0) {
         u  = CTAB;
         x  = CCOL;
         y  = CROW;
         rc = 0;
      } else {
         rc  = str2gyges (a_keys, &u, &x, &y, NULL, NULL, 0, YSTR_LEGAL);
         DEBUG_YMACRO   yLOG_value   ("str2gyges" , rc);
      }
      if (rc >= 0) {
         strlcpy (x_label, a_keys, LEN_LABEL);
         DEBUG_YMACRO   yLOG_complex ("current"   , "%2du, %3dx, %3dy", u, x, y);
         x_curr = LOC_cell_at_loc (u, x, y);
         DEBUG_YMACRO   yLOG_point   ("x_curr"    , x_curr);
         if (x_curr != NULL) {
            --rce;  switch (x_curr->type) {
            case YCALC_DATA_STR :
               DEBUG_YMACRO   yLOG_note    ("string source");
               strlcpy (a_keys, x_curr->source, LEN_RECD);
               break;
            case YCALC_DATA_SFORM : case YCALC_DATA_SLIKE :
               DEBUG_YMACRO   yLOG_note    ("string formula source");
               strlcpy (a_keys, x_curr->v_str , LEN_RECD);
               break;
            default :
               strlcpy (a_keys, ""            , LEN_RECD);
               break;
            }
         }
      }
   }
   /*---(save)---------------------------*/
   DEBUG_YMACRO   yLOG_info    ("a_keys"    , a_keys);
   x_row = p - s_valid + 1;
   DEBUG_YMACRO   yLOG_value   ("x_row"     , x_row);
   api_ymacro__saver (x_row, a_keys);
   /*---(complete)-----------------------*/
   DEBUG_YMACRO   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       agrios enablers                        ----===*/
/*====================------------------------------------====================*/
static void   o___AGRIOS__________o (void) { return; }

char
api_ymacro_getter        (char a_type, char *r_label, char *r_contents, char *r_next)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         u, x, y;
   char        x_early     [LEN_LABEL] = "";
   char        x_label     [LEN_LABEL] = "";
   tCELL      *x_curr      = NULL;
   double      v           =  0.0;
   char        x_final     [LEN_RECD]  = "";
   int         o           =    0;
   char        x_mute      =    0;
   /*---(header)-------------------------*/
   DEBUG_YMACRO   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YMACRO   yLOG_point   ("r_label"   , r_label);
   --rce;  if (r_label == NULL) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMACRO   yLOG_info    ("r_label"   , r_label);
   /*---(check for deref)----------------*/
   if (r_label [0] == '*') {
      rc = yCALC_variable (r_label + 1, x_early, NULL);
      DEBUG_YMACRO   yLOG_value   ("variable"  , rc);
      if (rc < 0)  strlcpy (x_early, r_label + 1, LEN_LABEL);
      x_curr = LOC_cell_labeled (x_early);
      switch (x_curr->type) {
      case YCALC_DATA_STR :
         strlcpy (x_early, x_curr->source, LEN_LABEL);
         break;
      case YCALC_DATA_SFORM : case YCALC_DATA_SLIKE :
         strlcpy (x_early, x_curr->v_str , LEN_LABEL);
         break;
      case YCALC_DATA_ADDR  : case YCALC_DATA_CADDR :
         strlcpy (x_early, x_curr->v_str + 1, LEN_LABEL);
         break;
      default :
         break;
      }
   } else {
      strlcpy (x_early, r_label, LEN_LABEL);
   }
   /*---(check for variable)-------------*/
   rc = yCALC_variable (x_early, x_label, NULL);
   DEBUG_YMACRO   yLOG_value   ("variable"  , rc);
   if (rc < 0)  strlcpy (x_label, x_early, LEN_LABEL);
   /*---(get current)--------------------*/
   DEBUG_YMACRO   yLOG_info    ("str2gyges" , x_label);
   rc  = str2gyges (x_label, &u, &x, &y, NULL, NULL, 0, YSTR_LEGAL);
   DEBUG_YMACRO   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get next)-----------------------*/
   switch (a_type) {
   case ' ' :
      DEBUG_YMACRO   yLOG_note    ("function call, make next == label");
      strlcpy (r_next, x_label, LEN_LABEL);
      break;
   case 'Ø' :
      DEBUG_YMACRO   yLOG_note    ("relative move to current");
      DEBUG_YMACRO   yLOG_info    ("contents"  , r_contents);
      rc = yCALC_variable (r_contents, x_label, NULL);
      if (rc <  0) {
         o = atoi (r_contents);
      } else {
         x_curr = LOC_cell_labeled (x_label);
         if (strchr (YCALC_GROUP_NUM, x_curr->type) != NULL)  o = x_curr->v_num;
         else                                                 o = 0;
      }
      DEBUG_YMACRO   yLOG_value   ("offset"    , o);
      y += o;
      DEBUG_YMACRO   yLOG_value   ("new y"     , y);
      rc = str4gyges (u, x, y, 0, 0, r_label, YSTR_LEGAL);
      DEBUG_YMACRO   yLOG_value   ("str4gyges" , rc);
      --rce;  if (rc < 0) {
         DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_YMACRO   yLOG_info    ("r_label"   , r_label);
      /* intentional fall-thru */
   case '™' : default  :
      DEBUG_YMACRO   yLOG_note    ("advance next next");
      rc = str4gyges (u, x, y + 1, 0, 0, r_next, YSTR_LEGAL);
      DEBUG_YMACRO   yLOG_value   ("str4gyges" , rc);
      --rce;  if (rc < 0) {
         DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      break;
   }
   DEBUG_YMACRO   yLOG_info    ("r_next"    , r_next);
   /*---(get contents)-------------------*/
   x_curr = LOC_cell_at_loc (u, x, y);
   DEBUG_YMACRO   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr == NULL) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = str4gyges (u, x, y, 0, 0, x_label, YSTR_LEGAL);
   DEBUG_YMACRO   yLOG_char    ("type"      , x_curr->type);
   --rce;  switch (x_curr->type) {
   case YCALC_DATA_STR   :
      DEBUG_YMACRO   yLOG_note    ("string literal");
      strlcpy (r_contents, x_curr->source, LEN_RECD);
      break;
   case YCALC_DATA_SFORM : case YCALC_DATA_SLIKE :
      DEBUG_YMACRO   yLOG_note    ("string formula");
      strlcpy (r_contents, x_curr->v_str , LEN_RECD);
      break;
   default :
      DEBUG_YMACRO   yLOG_note    ("unusable cell type");
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
      break;
   }
   DEBUG_YMACRO   yLOG_info    ("r_contents", r_contents);
   /*---(update in system)---------------*/
   x_mute = yLOGS_mute_check ();
   yLOGS_mute ();
   sprintf (my.cagrios, ",%s,", x_label);
   CELL_overwrite  (YMAP_NONE , 37, 5,  3, x_label   , "<?0-·");
   CELL_overwrite  (YMAP_NONE , 37, 5,  4, r_contents, "<?0-·");
   CELL_overwrite  (YMAP_NONE , 37, 5,  5, r_next    , "<?0-·");
   if (x_mute)  yLOGS_unmute ();
   /*---(complete)-----------------------*/
   DEBUG_YMACRO   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_ymacro_forcer        (char r_type, char *a_target, char *a_contents)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         u, x, y;
   char        x_early     [LEN_LABEL] = "";
   char        x_label     [LEN_LABEL] = "";
   tCELL      *x_curr      = NULL;
   tCELL      *x_temp      = NULL;
   double      v           =  0.0;
   char        x_final     [LEN_RECD]  = "";
   /*---(header)-------------------------*/
   DEBUG_YMACRO   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YMACRO   yLOG_point   ("a_contents", a_contents);
   --rce;  if (a_contents == NULL) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMACRO   yLOG_info    ("a_contents", a_contents);
   DEBUG_YMACRO   yLOG_point   ("a_target"  , a_target);
   --rce;  if (a_target == NULL) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMACRO   yLOG_info    ("a_target"  , a_target);
   /*---(check for deref)----------------*/
   --rce;  if (strchr ("*á", a_target [0]) != NULL) {
      DEBUG_YMACRO   yLOG_note    ("found a pointer deref request");
      rc = yCALC_variable (a_target + 1, x_early, NULL);
      DEBUG_YMACRO   yLOG_value   ("variable"  , rc);
      if (rc < 0)  strlcpy (x_early, a_target + 1, LEN_LABEL);
      x_curr = LOC_cell_labeled (x_early);
      DEBUG_YMACRO   yLOG_point   ("x_curr"    , x_curr);
      if (x_curr == NULL) {
         DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_YMACRO   yLOG_char    ("->type"    , x_curr->type);
      switch (x_curr->type) {
      case YCALC_DATA_STR :
         DEBUG_YMACRO   yLOG_note    ("use the string contents");
         strlcpy (x_early, x_curr->source, LEN_LABEL);
         break;
      case YCALC_DATA_SFORM : case YCALC_DATA_SLIKE :
         DEBUG_YMACRO   yLOG_note    ("use results of string formula");
         strlcpy (x_early, x_curr->v_str , LEN_LABEL);
         break;
      case YCALC_DATA_ADDR  :
         DEBUG_YMACRO   yLOG_note    ("use the actual address");
         strlcpy (x_early, x_curr->source + 1, LEN_LABEL);
         break;
      case YCALC_DATA_CADDR :
         DEBUG_YMACRO   yLOG_note    ("use the calculated address");
         strlcpy (x_early, x_curr->v_str    , LEN_LABEL);
         break;
      default :
         DEBUG_YMACRO   yLOG_note    ("this type can not be used");
         DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
         break;
      }
   } else {
      DEBUG_YMACRO   yLOG_note    ("normal target");
      strlcpy (x_early, a_target, LEN_LABEL);
   }
   DEBUG_YMACRO   yLOG_info    ("x_early"   , x_early);
   /*---(check for variable)-------------*/
   rc = yCALC_variable (x_early, x_label, NULL);
   DEBUG_YMACRO   yLOG_value   ("rc"        , rc);
   if (rc < 0)  strlcpy (x_label, x_early, LEN_LABEL);
   /*---(get target)---------------------*/
   DEBUG_YMACRO   yLOG_info    ("x_label"   , x_label);
   rc  = str2gyges (x_label, &u, &x, &y, NULL, NULL, 0, YSTR_USABLE);
   DEBUG_YMACRO   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(force value)--------------------*/
   if (r_type == 'Û') {
      DEBUG_YMACRO   yLOG_note    ("formula result forcing");
      /*---(cram in temp)----------------*/
      CELL_change  (&x_temp, YMAP_NONE , 37, 4, 47, a_contents);
      DEBUG_YMACRO   yLOG_point   ("x_temp"    , x_temp);
      --rce;  if (x_temp == NULL) {
         DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(cram in temp)----------------*/
      DEBUG_YMACRO   yLOG_char    ("type"      , x_temp->type);
      --rce;  if (strchr (YCALC_GROUP_NUM, x_temp->type) != NULL) {
         v = x_temp->v_num;
         DEBUG_YMACRO   yLOG_value   ("v"         , x_temp->v_num);
         sprintf (x_final, "%lf" , v);
      } else if (strchr (YCALC_GROUP_STR, x_temp->type) != NULL) {
         strlcpy (x_final, x_temp->v_str , LEN_RECD);
      } else {
         DEBUG_YMACRO   yLOG_note    ("unusable cell type");
         DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(force contents)-----------------*/
   else {
      DEBUG_YMACRO   yLOG_note    ("actual formla forcing");
      if (strncmp (a_contents, "´+", 2) == 0) {
         DEBUG_YMACRO   yLOG_note    ("concatenation");
         x_curr = LOC_cell_labeled (x_label);
         DEBUG_YMACRO   yLOG_point   ("x_curr"    , x_curr);
         if (x_curr == NULL) {
            DEBUG_YMACRO   yLOG_note    ("handle null starting cell");
            strlcpy (x_final, "", LEN_RECD);
         } else {
            DEBUG_YMACRO   yLOG_note    ("handle existing cell");
            strlcpy (x_final, x_curr->source, LEN_RECD);
            DEBUG_YMACRO   yLOG_info    ("start"     , x_final);
         }
         strlcat (x_final, a_contents + 2, LEN_RECD);
         DEBUG_YMACRO   yLOG_info    ("x_final"   , x_final);
      } else if (a_contents [0] == '´') {
         DEBUG_YMACRO   yLOG_note    ("literal");
         strlcpy (x_final, a_contents + 1, LEN_RECD);
      } else {
         DEBUG_YMACRO   yLOG_note    ("pure replacement");
         strlcpy (x_final, a_contents, LEN_RECD);
      }
   }
   /*---(overwrite target)---------------*/
   strldchg     (x_final, '²', ' ', LEN_RECD);
   CELL_change  (NULL, YMAP_BEG  , u, x, y, x_final);
   /*---(wipe temp)----------------------*/
   if (r_type == 'Û')  CELL_change  (NULL, YMAP_NONE , 37, 4, 47, "···");
   /*---(complete)-----------------------*/
   DEBUG_YMACRO   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_ymacro_pusher        (char a_dir, char a_level, char *a_args)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        t           [LEN_LABEL] = "";
   char        x_args      [LEN_LABEL] = "";
   char        x_contents  [LEN_RECD]  = "";
   tCELL      *x_copy      = NULL;
   tCELL      *x_rc        = NULL;
   int         u           =   37;
   int         x, y;
   char       *p           = NULL;
   char       *r           = NULL;
   int         uv, xv, yv;
   int         n           =    0;
   /*---(header)-------------------------*/
   DEBUG_YMACRO   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YMACRO   yLOG_char    ("a_dir"     , a_dir);
   --rce;  if (a_dir == 0 || strchr ("><", a_dir) == NULL) {
      DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(handle push)--------------------*/
   --rce;  if (a_dir == '>') {
      /*---(push vars to right)----------*/
      for (x = 10; x >=  5; --x) {
         for (y = 2; y < 46; ++y) {
            x_copy = LOC_cell_at_loc (u, x, y);
            if (x_copy         == NULL)  continue;
            if (x_copy->source == NULL)  continue;
            DEBUG_YMACRO   yLOG_complex ("current"   , "%2du, %3dx, %3dy, %p", u, x, y, x_copy);
            CELL_overwrite  (YMAP_NONE , u, x + 1, y, strdup (x_copy->source), "<?0-·");
            if (x == 5 && y != 18)  CELL_change (NULL, YMAP_NONE, u, x, y, "");
         }
      }
      sprintf (t, "%d", a_level);
      CELL_overwrite  (YMAP_NONE , u, 5, 2, t, "<?0-·");
      /*---(pass args)-------------------*/
      if (a_args != NULL)  strlcpy (x_args, a_args, LEN_LABEL);
      DEBUG_YMACRO   yLOG_info    ("x_args"    , x_args);
      p = strtok_r (x_args, ",", &r);
      DEBUG_YMACRO   yLOG_point   ("p"         , p);
      while (p != NULL) {
         /*---(check for variable)-------------*/
         DEBUG_YMACRO   yLOG_info    ("p"         , p);
         rc = yCALC_variable (p, t, NULL);
         DEBUG_YMACRO   yLOG_value   ("var"       , rc);
         if (rc < 0) {
            /*---(then stuff it)------------------*/
            CELL_overwrite  (YMAP_NONE , 37, 5,  7 + n, p, "<?0-·");
            /*---(next)---------------------------*/
            p = strtok_r (NULL, ",", &r);
            DEBUG_YMACRO   yLOG_value   ("p"         , p);
            ++n;
            continue;
         }
         DEBUG_YMACRO   yLOG_info    ("t"         , t);
         /*---(get local)----------------------*/
         if (p [0] == 'ç') {
            /*---(get current)--------------------*/
            DEBUG_YMACRO   yLOG_info    ("str2gyges" , t);
            rc  = str2gyges (t, &uv, &xv, &yv, NULL, NULL, 0, YSTR_LEGAL);
            DEBUG_YMACRO   yLOG_value   ("rc"        , rc);
            --rce;  if (rc < 0) {
               DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
               return rce;
            }
            /*---(make correct address)-----------*/
            rc = str4gyges (uv, xv + 1, yv, 0, 0, t, YSTR_LEGAL);
            DEBUG_YMACRO   yLOG_value   ("str4gyges" , rc);
            if (rc < 0) {
               DEBUG_YMACRO   yLOG_exitr   (__FUNCTION__, rce);
               return rce;
            }
            DEBUG_YMACRO   yLOG_info    ("t*"        , t);
         }
         /*---(get copy)-----------------------*/
         strlcpy (x_contents, "", LEN_RECD);
         x_copy = LOC_cell_labeled (t);
         DEBUG_YMACRO   yLOG_point   ("x_copy"    , x_copy);
         if (x_copy != NULL && x_copy->source != NULL) {
            strlcpy (x_contents, x_copy->source, LEN_RECD);
         }
         CELL_overwrite  (YMAP_NONE , 37, 5,  7 + n, x_contents, "<?0-·");
         /*---(next)---------------------------*/
         p = strtok_r (NULL, ",", &r);
         DEBUG_YMACRO   yLOG_value   ("p"         , p);
         ++n;
      }
   }
   /*---(handle pop)---------------------*/
   else if (a_dir == '<') {
      /*---(copy return ç?)--------------*/
      /*> x_copy = LOC_cell_at_loc (u,  5, 18);                                          <* 
       *> DEBUG_YMACRO   yLOG_point   ("x_copy"    , x_copy);                              <* 
       *> if (x_copy != NULL && x_copy->source != NULL) {                                <* 
       *>    CELL_overwrite  (YMAP_NONE , u, 6, 18, strdup (x_copy->source), "<?0-·");   <* 
       *> }                                                                              <*/
      /*---(pop vars to left)------------*/
      x_rc   = LOC_cell_at_loc (u, 5, 18);
      if (x_rc != NULL && x_rc->type == YCALC_DATA_BLANK)  x_rc = NULL;
      DEBUG_YMACRO   yLOG_point   ("x_rc"      , x_rc);
      for (x =  6; x <= 11; ++x) {
         for (y = 2; y < 46; ++y) {
            /*> if (y == 18) {                                                        <* 
             *>    DEBUG_YMACRO   yLOG_note    ("found return code line");            <* 
             *> }                                                                     <*/
            if (x_rc != NULL && y == 18) {
               DEBUG_YMACRO   yLOG_note    ("leave return code alone");
               CELL_change (NULL, YMAP_NONE, u, x, y, "");
               continue;
            }
            x_copy = LOC_cell_at_loc (u, x, y);
            if (x_copy != NULL && x_copy->source != NULL) {
               DEBUG_YMACRO   yLOG_complex ("current"   , "%2du, %3dx, %3dy, %p", u, x, y, x_copy);
               CELL_overwrite  (YMAP_NONE , u, x - 1, y, strdup (x_copy->source), "<?0-·");
            } else {
               CELL_change (NULL, YMAP_NONE, u, x - 1, y, "");
            }
            CELL_change (NULL, YMAP_NONE, u, x, y, "");
         }
      }
   }
   /*---(clear out)----------------------*/
   else if (a_dir == '#') {
      for (x =  5; x <= 11; ++x) {
         for (y = 2; y < 46; ++y) {
            x_copy = LOC_cell_at_loc (u, x, y);
            if (x_copy         == NULL)  continue;
            if (x_copy->source == NULL)  continue;
            DEBUG_YMACRO   yLOG_complex ("current"   , "%2du, %3dx, %3dy, %p", u, x, y, x_copy);
            CELL_change (NULL, YMAP_NONE, u, x, y, "");
         }
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YMACRO   yLOG_exit    (__FUNCTION__);
   return 0;
}

char api_ymacro_ball         (void)  { my.ball = 'y'; strcpy (my.cagrios, "");  return 0; }
char api_ymacro_noball       (void)  { my.ball = '-'; strcpy (my.cagrios, "");  return 0; }




