/*============================----beg-of-source---============================*/
#include   "gyges.h"


static char s_nada       [5] = "";



/*====================------------------------------------====================*/
/*===----                       yCALC_exist_config                     ----===*/
/*====================------------------------------------====================*/
static void   o___EXIST___________o (void) { return; }

char         /*-> save yCALC deproot to owner --------[ leaf   [ge.722.223.20]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
api_ycalc_enabler       (void *a_owner, void *a_deproot)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tCELL      *x_owner     = NULL;
   /*---(header)-------------------------*/
   DEBUG_APIS   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_APIS   yLOG_spoint  (a_owner);
   --rce;  if (a_owner == NULL) {
      DEBUG_APIS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert)------------------------*/
   x_owner = (tCELL *) a_owner;
   DEBUG_APIS   yLOG_snote   (x_owner->label);
   /*---(check)--------------------------*/
   DEBUG_APIS   yLOG_spoint  (a_deproot);
   DEBUG_APIS   yLOG_spoint  (x_owner->ycalc);
   --rce;  if (a_deproot != NULL && x_owner->ycalc != NULL) {
      DEBUG_APIS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   x_owner->ycalc = a_deproot;
   DEBUG_APIS   yLOG_spoint  (x_owner->ycalc);
   /*---(complete)-----------------------*/
   DEBUG_APIS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.933.522.50]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
api_ycalc_pointer       (void *a_owner, char **a_source, char **a_type, double **a_value, char **a_string)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tCELL      *x_owner     = NULL;
   /*---(header)-------------------------*/
   DEBUG_APIS   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_APIS   yLOG_spoint  (a_owner);
   --rce;  if (a_owner  == NULL) {
      DEBUG_APIS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert)------------------------*/
   x_owner   = (tCELL *) a_owner;
   DEBUG_APIS   yLOG_snote   (x_owner->label);
   /*---(save)---------------------------*/
   if (a_source != NULL) {
      DEBUG_APIS   yLOG_spoint  (*a_source);
      *a_source = &x_owner->s;
      DEBUG_APIS   yLOG_spoint  (*a_source);
   }
   if (a_type  != NULL) {
      DEBUG_APIS   yLOG_spoint  (*a_type);
      *a_type   = &x_owner->t;
      DEBUG_APIS   yLOG_spoint  (*a_type);
   }
   if (a_value  != NULL) {
      DEBUG_APIS   yLOG_spoint  (*a_value);
      *a_value  = &x_owner->v_num;
      DEBUG_APIS   yLOG_spoint  (*a_value);
   }
   if (a_string != NULL) {
      DEBUG_APIS   yLOG_spoint  (*a_string);
      *a_string = &x_owner->v_str;
      DEBUG_APIS   yLOG_spoint  (*a_string);
   }
   /*---(complete)-----------------------*/
   DEBUG_APIS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
api_ycalc_reaper        (void **a_owner)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_owner     = NULL;
   /*---(header)-------------------------*/
   DEBUG_APIS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_APIS   yLOG_point   ("a_owner"   , a_owner);
   --rce;  if (a_owner  == NULL) {
      DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_APIS   yLOG_point   ("*a_owner"  , *a_owner);
   --rce;  if (*a_owner  == NULL) {
      DEBUG_APIS   yLOG_exitr   (__FUNCTION__, -rce);
      return -rce;
   }
   /*---(check deproot)------------------*/
   x_owner = (tCELL *) *a_owner;
   DEBUG_APIS   yLOG_point   ("ycalc"     , x_owner->ycalc);
   --rce;  if (x_owner->ycalc  != NULL) {
      DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check type)---------------------*/
   DEBUG_APIS   yLOG_info    ("label"     , x_owner->label);
   DEBUG_APIS   yLOG_char    ("type"      , x_owner->t);
   --rce;  if (x_owner->t != YCALC_DATA_BLANK) {
      DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(delete)-------------------------*/
   /*> rc = CELL__delete (CHG_INPUT, x_owner->tab, x_owner->col, x_owner->row);       <*/
   rc = CELL__delete (CHG_NOHIST, x_owner->tab, x_owner->col, x_owner->row);
   --rce;  if (rc < 0) {
      DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   *a_owner = NULL;
   /*---(complete)-----------------------*/
   DEBUG_APIS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       yCALC_label_config                     ----===*/
/*====================------------------------------------====================*/
static void   o___LABEL___________o (void) { return; }

char         /*-> tbd --------------------------------[ ------ [ge.I75.497.D1]*/ /*-[02.0000.00#.!]-*/ /*-[--.---.---.--]-*/
api_ycalc_named         (char *a_label, char a_force, void **a_owner, void **a_deproot)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_owner     = NULL;
   int         x_col       =   0;           /* working col value              */
   int         x_row       =   0;           /* working row value              */
   int         x_tab       =   0;           /* working tab value              */
   static char    x_sforce =  '?';
   static char   *x_label  [LEN_LABEL];
   static tCELL  *x_saved  = NULL;
   /*---(header)-------------------------*/
   DEBUG_APIS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (a_owner   != NULL)  *a_owner   = NULL;
   if (a_deproot != NULL)  *a_deproot = NULL;
   /*---(defense)------------------------*/
   DEBUG_APIS   yLOG_point   ("a_label"   , a_label);
   --rce;  if (a_label == NULL) {
      DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_APIS   yLOG_info    ("a_label"   , a_label);
   /*---(shortcut)-----------------------*/
   if (strcmp (x_label, a_label) == 0 && a_force == x_sforce) {
      DEBUG_APIS   yLOG_note    ("short-cut");
      if (a_owner   != NULL) {
         *a_owner   = x_saved;
         DEBUG_APIS   yLOG_point   ("*a_owner"  , *a_owner);
      }
      if (a_deproot != NULL) {
         *a_deproot = x_saved->ycalc;
         DEBUG_APIS   yLOG_point   ("*a_deproot", *a_deproot);
      }
      DEBUG_APIS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(root)---------------------------*/
   if (strcmp ("ROOT", a_label) == 0) {
      if (my.root == NULL) {
         rc = CELL__new (&my.root, UNLINKED);
         DEBUG_PROG   yLOG_value   ("rc"        , rc);
         --rce;  if (rc < 0) {
            DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         --rce;  if (my.root == NULL) {
            DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         strcpy (my.root->label, "ROOT");
      }
      x_owner = my.root;
   }
   /*---(search)-------------------------*/
   else {
      rc      = str2gyges (a_label, &x_col, &x_row, &x_tab, NULL, 0);
      DEBUG_APIS   yLOG_value   ("str2gyges" , rc);
      --rce;  if (rc < 0) {
         DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_APIS   yLOG_complex ("loc"        , "%3dx, %3dy, %3dz", x_col, x_row, x_tab);
      x_owner = LOC_cell_at_loc  (x_col, x_row, x_tab);
      DEBUG_APIS   yLOG_point   ("x_owner"    , x_owner);
      --rce;  if (x_owner == NULL && a_force == YCALC_LOOK) {
         DEBUG_APIS   yLOG_note    ("owner does not exist and only in look mode");
         DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(new cell)-----------------------*/
      --rce;  if (x_owner == NULL) {
         DEBUG_APIS   yLOG_note    ("create a new one");
         rc = CELL__create (&x_owner, x_tab, x_col, x_row);
         if (rc < 0) {
            DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         DEBUG_APIS   yLOG_point   ("x_owner"    , x_owner);
      }
      --rce;  if (x_owner == NULL) {
         DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(save)---------------------------*/
   DEBUG_APIS   yLOG_complex ("loc_final"  , "%3dx, %3dy, %3dz", x_owner->col, x_owner->row, x_owner->tab);
   x_sforce = a_force;
   x_saved  = x_owner;
   strlcpy (x_label, a_label, LEN_LABEL);
   DEBUG_APIS   yLOG_point   ("x_saved"   , x_saved);
   /*---(handle normal)------------------*/
   DEBUG_APIS   yLOG_note    ("success");
   if (a_owner   != NULL) {
      *a_owner   = x_owner;
      DEBUG_APIS   yLOG_point   ("*a_owner"  , *a_owner);
   }
   if (a_deproot != NULL) {
      *a_deproot = x_owner->ycalc;
      DEBUG_APIS   yLOG_point   ("*a_deproot", *a_deproot);
   }
   /*---(complete)-----------------------*/
   DEBUG_APIS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gc.320.621.10]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
api_ycalc_whos_at       (int x, int y, int z, char a_force, void **a_owner, void **a_deproot)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x_label     [LEN_LABEL];
   /*---(legal)--------------------------*/
   rc = str4gyges (x, y, z, 0, x_label);
   if (rc == 0)  rc = api_ycalc_named (x_label, YCALC_LOOK, a_owner, a_deproot);
   /*---(complete)-----------------------*/
   return rc;
}

char*
api_ycalc_labeler       (void *a_owner)
{
   tCELL      *x_owner     = NULL;
   DEBUG_APIS   yLOG_senter  (__FUNCTION__);
   DEBUG_APIS   yLOG_spoint  (a_owner);
   if (a_owner == NULL) {
      DEBUG_APIS   yLOG_sexit   (__FUNCTION__);
      return s_nada;
   }
   x_owner = (tCELL *) a_owner;
   DEBUG_APIS   yLOG_snote   (x_owner->label);
   DEBUG_APIS   yLOG_sexit   (__FUNCTION__);
   return x_owner->label;
}



/*====================------------------------------------====================*/
/*===----                       yCALC_value_config                     ----===*/
/*====================------------------------------------====================*/
static void   o___VALUE___________o (void) { return; }

char         /*-> tbd --------------------------------[ leaf   [ge.742.422.B0]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
api_ycalc_valuer        (void *a_owner, char *a_type, double *a_value, char **a_string)
{
   char        rce         =  -10;
   tCELL     *x_owner      = NULL;
   DEBUG_APIS   yLOG_senter  (__FUNCTION__);
   DEBUG_APIS   yLOG_spoint  (a_owner);
   if (a_type   != NULL)  *a_type   = '-';
   if (a_value  != NULL)  *a_value  = 0.0;
   if (a_string != NULL)  *a_string = s_nada;
   --rce;  if (a_owner  == NULL) {
      DEBUG_APIS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   x_owner    = (tCELL *) a_owner;
   if (a_type   != NULL) {
      *a_type   = x_owner->t;
      DEBUG_APIS   yLOG_schar   (*a_type);
   }
   if (a_value  != NULL) {
      *a_value  = x_owner->v_num;
      DEBUG_APIS   yLOG_sdouble (*a_value);
   }
   if (a_string != NULL) {
      if        (x_owner->t     == 'E') {
         *a_string   = s_nada;
      } else if (x_owner->t     == 's'){
         if (x_owner->s != NULL)  *a_string = x_owner->s;
         else                     *a_string = s_nada;
      } else if (x_owner->v_str != NULL) {
         *a_string = x_owner->v_str;
      } else {
         *a_string = s_nada;
      }
      DEBUG_APIS   yLOG_snote   (*a_string);
   }
   DEBUG_APIS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
api_ycalc_address       (void *a_owner, int *x, int *y, int *z)
{
   tCELL      *x_owner     = NULL;
   if (x != NULL)  *x   = 0;
   if (y != NULL)  *y   = 0;
   if (z != NULL)  *z   = 0;
   if (a_owner == NULL)  return -1;
   x_owner    = (tCELL     *) a_owner;
   str2gyges (x_owner->label, x, y, z, NULL, 0);
   return 0;
}

char
api_ycalc_special       (void *a_owner, char a_what, double *a_value, char **a_string)
{
   char        rce         =  -10;
   tCELL      *x_owner     = NULL;
   DEBUG_APIS   yLOG_senter  (__FUNCTION__);
   DEBUG_APIS   yLOG_spoint  (a_owner);
   DEBUG_APIS   yLOG_schar   (a_what);
   if (a_value  != NULL)  *a_value  = 0.0;
   if (a_string != NULL)  *a_string = s_nada;
   --rce;  if (a_owner   == NULL) {
      DEBUG_APIS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   x_owner   = (tCELL     *) a_owner;
   --rce;  switch (a_what) {
   case G_SPECIAL_SOURCE :
      if (a_string != NULL)  *a_string = x_owner->s;
      break;
   case G_SPECIAL_PRINT  :
      if (a_string != NULL)  *a_string = x_owner->p;
      break;
   case G_SPECIAL_TYPE   :
      if (a_value  != NULL)  *a_value  = x_owner->t;
      break;
   }
   DEBUG_APIS   yLOG_snote   (*a_string);
   DEBUG_APIS   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       yCALC_value_config                     ----===*/
/*====================------------------------------------====================*/
static void   o___PRINTING________o (void) { return; }

static int     s_merges =    0;
static tCELL  *s_owners [20];
static int     s_widths [20];

char         /*-> determine full print width ---------[ ------ [gz.530.321.23]*/ /*-[01.0000.015.!]-*/ /*-[--.---.---.--]-*/
api__ycalc_width        (void *a_owner, int *a_width, int *a_merge)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   tCELL      *x_owner     = NULL;
   char        x_label     [LEN_LABEL];
   int         x, y, z;
   int         i           =    0;
   int         w           =    0;
   /*---(header)-------------------------*/
   DEBUG_APIS   yLOG_enter   (__FUNCTION__);
   /*---(legal)--------------------------*/
   DEBUG_APIS   yLOG_point   ("a_owner"   , a_owner);
   if (a_owner == NULL) {
      DEBUG_APIS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_APIS   yLOG_point   ("a_width"   , a_width);
   if (a_width == NULL) {
      DEBUG_APIS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_APIS   yLOG_point   ("a_merge"   , a_merge);
   if (a_merge == NULL) {
      DEBUG_APIS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(initialize)---------------------*/
   x_owner  = (tCELL *) a_owner;
   DEBUG_APIS   yLOG_value   ("tab"       , x_owner->tab);
   DEBUG_APIS   yLOG_value   ("col"       , x_owner->col);
   w        = COL_width (x_owner->tab, x_owner->col);
   DEBUG_APIS   yLOG_value   ("w"         , w);
   *a_width = w;
   *a_merge = 0;
   s_owners [*a_merge] = x_owner;
   s_widths [*a_merge] = w;
   /*---(look for mergse)----------------*/
   rc = str2gyges (x_owner->label, &x, &y, &z, 0, 0);
   DEBUG_APIS   yLOG_value   ("str2gyges" , rc);
   if (rc < 0)  {
      DEBUG_APIS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_APIS   yLOG_complex ("owner"     , "%-10p, %-5s, %3dx, %3dy, %3dz, %3dw", x_owner, x_owner->label, x, y, z, w);
   for (i = x + 1; i < x + 20; ++i) {
      rc = str4gyges (i, y, z, 0, x_label);
      if (rc < 0)  break;
      rc = api_ycalc_whos_at (i, y, z, YCALC_LOOK, &x_owner, NULL);
      if (rc < 0)                              break;
      if (x_owner == NULL)                     break;
      if (x_owner->t != YCALC_DATA_MERGED)     break;
      w         = COL_width (x_owner->tab, x_owner->col);
      DEBUG_APIS   yLOG_complex ("owner"     , "%-10p, %-5s, %3dx, %3dy, %3dz, %3dw", x_owner, x_label, i, y, z, w);
      *a_width += w;
      ++(*a_merge);
      s_owners [*a_merge] = x_owner;
      s_widths [*a_merge] = w;
   }
   DEBUG_APIS   yLOG_value   ("*a_width"  , *a_width);
   /*---(complete)-----------------------*/
   DEBUG_APIS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> parse print into merged cells ------[ ------ [gz.641.351.22]*/ /*-[22.0000.015.!]-*/ /*-[--.---.---.--]-*/
api__ycalc_parse        (char *a_full, int a_merge)
{
   /*---(locals)-----------+-----+-----+-*/
   tCELL      *x_owner     = NULL;
   int         i           = 0;
   int         w           = 0;             /* available printing width       */
   int         wa          = 0;             /* adjusted width                 */
   char       *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_APIS   yLOG_enter   (__FUNCTION__);
   DEBUG_APIS   yLOG_point   ("a_full"    , a_full);
   if (a_full == NULL) {
      DEBUG_APIS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_APIS   yLOG_value   ("a_merge"   , a_merge);
   for (i = 0; i <= a_merge; ++i) {
      x_owner = s_owners [i];
      DEBUG_APIS  yLOG_point   ("x_owner"   , x_owner);
      if (x_owner == NULL)  break;
      w       = s_widths [i];
      DEBUG_APIS  yLOG_value   ("w"         , w);
      while (p == NULL)  p = (char*) malloc (w + 1);
      DEBUG_APIS  yLOG_point   ("p"         , p);
      sprintf (p, "%-*.*s", w, w, a_full + wa);
      DEBUG_APIS  yLOG_point   ("x_owner->p", x_owner->p);
      if (x_owner->p != NULL) {
         free (x_owner->p);
         x_owner->p = NULL;
      }
      x_owner->p = p;
      p    = NULL;
      wa   += w;
   }
   /*---(complete)-----------------------*/
   DEBUG_APIS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_ycalc_printer       (void *a_owner)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tCELL      *x_owner     = NULL;
   char        s           [LEN_RECD];
   char        t           [LEN_RECD];
   char        x_out       [LEN_RECD];
   char        x_len       =    0;
   int         w           =    0;
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_APIS   yLOG_enter   (__FUNCTION__);
   DEBUG_APIS   yLOG_point   ("a_owner"   , a_owner);
   /*---(defense)------------------------*/
   --rce;  if (a_owner   == NULL) {
      DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_owner   = (tCELL     *) a_owner;
   /*---(contents)-----------------------*/
   DEBUG_APIS   yLOG_char    ("type"      , x_owner->t);
   DEBUG_APIS   yLOG_char    ("decs"      , x_owner->d);
   DEBUG_APIS   yLOG_char    ("format"    , x_owner->f);
   DEBUG_APIS   yLOG_char    ("align"     , x_owner->a);
   /*---(get size)-----------------------*/
   api__ycalc_width (x_owner, &w, &c);
   DEBUG_APIS   yLOG_value   ("w"         , w);
   --rce;  if (w <= 0) {
      DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_APIS   yLOG_value   ("c"         , c);
   /*---(contents)-----------------------*/
   if (strchr (YCALC_GROUP_NUM, x_owner->t) != NULL) {
      DEBUG_APIS   yLOG_value   ("value"     , x_owner->v_num);
      strl4main (x_owner->v_num, s, x_owner->d - '0', x_owner->f, LEN_RECD);
      DEBUG_APIS   yLOG_info    ("string"    , s);
      strlpad (s, t, ' ', x_owner->a, w - 1);
      DEBUG_APIS   yLOG_info    ("trim/pad"  , t);
      sprintf (x_out, "%s ", t);
      DEBUG_APIS   yLOG_info    ("final"     , x_out);
   } else if (strchr (YCALC_GROUP_STR, x_owner->t) != NULL) {
      if (x_owner->v_str != NULL)  strlcpy (s, x_owner->v_str, LEN_RECD);
      else                         strlcpy (s, x_owner->s    , LEN_RECD);
      DEBUG_APIS   yLOG_info    ("string"    , s);
      strlpad (s, t, x_owner->f, x_owner->a, w - 1);
      DEBUG_APIS   yLOG_info    ("trim/pad"  , t);
      sprintf (x_out, "%s ", t);
      DEBUG_APIS   yLOG_info    ("final"     , x_out);
   } else if (strchr (YCALC_GROUP_POINT, x_owner->t) != NULL) {
      strlcpy (s, x_owner->s, LEN_RECD);
      DEBUG_APIS   yLOG_info    ("pointer"   , s);
      strlpad (s, t, x_owner->f, x_owner->a, w - 1);
      DEBUG_APIS   yLOG_info    ("trim/pad"  , t);
      sprintf (x_out, "%s ", t);
      DEBUG_APIS   yLOG_info    ("final"     , x_out);
   } else if (YCALC_DATA_ERROR == x_owner->t) {
      if (x_owner->v_str != NULL)  strlcpy (s, x_owner->v_str, LEN_RECD);
      else                         strlcpy (s, "#?/???"      , LEN_RECD);
      DEBUG_APIS   yLOG_info    ("pointer"   , s);
      strlpad (s, t, ' ', '<', w - 1);
      DEBUG_APIS   yLOG_info    ("trim/pad"  , t);
      sprintf (x_out, "%s ", t);
      DEBUG_APIS   yLOG_info    ("final"     , x_out);
   } else if (x_owner->t == YCALC_DATA_BLANK) {
      strlcpy (s, "-", LEN_RECD);
      DEBUG_APIS   yLOG_info    ("blank"     , s);
      strlpad (s, t, ' ', '>', w - 1);
      DEBUG_APIS   yLOG_info    ("trim/pad"  , t);
      sprintf (x_out, "%s ", t);
      DEBUG_APIS   yLOG_info    ("final"     , x_out);
   } else if (x_owner->t == YCALC_DATA_INTERN) {
      strlcpy (s, x_owner->s, LEN_RECD);
      DEBUG_APIS   yLOG_info    ("range"     , s);
      strlpad (s, t, x_owner->f, x_owner->a, w - 1);
      DEBUG_APIS   yLOG_info    ("trim/pad"  , t);
      sprintf (x_out, "%s ", t);
      DEBUG_APIS   yLOG_info    ("final"     , x_out);
   } else {
      DEBUG_APIS   yLOG_note    ("non-printing type");
      DEBUG_APIS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(parse)--------------------------*/
   api__ycalc_parse (x_out, c);
   /*---(complete)-----------------------*/
   DEBUG_APIS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char*        /*-> unit testing accessor --------------[ light  [us.IA0.2A5.X3]*/ /*-[02.0000.00#.#]-*/ /*-[--.---.---.--]-*/
api_ycalc__unit    (char *a_question, char *a_label)
{  /*---(design notes)--------------------------------------------------------*/
   /*
    *  i formerly used cell pointers as the second argument, but it was hugely
    *  problematic when testing cells that are created and destroyed regularly.
    *  switching to labels adds an enormous safety factor and takes complexity
    *  out of the unit testing script.  more safety, more resilience, all good.
    */
   /*---(locals)-----------+-----+-----+-*/
   int         len         = 0;
   tCELL      *x_cell      = NULL;
   char        rc          = 0;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   int         x_count     =    0;
   char        x_list      [LEN_RECD];
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "ycalc error      : question not understood");
   /*---(identify the cell pointer)------*/
   if (a_label == NULL || strcmp ("root", a_label) == 0) {
      /*> x_cell = s_root;                                                            <*/
      sprintf (unit_answer, "ycalc error      : can not call on dependency s_root");
      return unit_answer;
   } else {
      rc     = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);
      if (rc < 0) {
         sprintf (unit_answer, "ycalc error      : label <%s> not legal", a_label);
         return unit_answer;
      }
      rc     = LOC_legal (x_col, x_row, x_tab, CELL_FIXED);
      if (rc < 0) {
         sprintf (unit_answer, "ycalc error      : label <%s> not in-range", a_label);
         return unit_answer;
      }
      x_cell = LOC_cell_at_loc  (x_col, x_row, x_tab);
      /*> if (x_cell == NULL) {                                                         <* 
       *>    sprintf (unit_answer, "s_celln          : label <%s> is NULL", a_label);   <* 
       *>    return unit_answer;                                                        <* 
       *> }                                                                             <*/
   }
   /*---(ycalc information)--------------*/
   if (strcmp(a_question, "ycalc_rpn"    )  == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_UNIT, "ycalc rpn        : (----) - -");
      else if (x_cell->ycalc == NULL)  snprintf(unit_answer, LEN_UNIT, "ycalc rpn        : (----) %c -", x_cell->t);
      else {
         yCALC_show_rpn  (x_cell->ycalc, &x_count, x_list);
         if (x_count      == 0)        snprintf(unit_answer, LEN_UNIT, "ycalc rpn        : (%4d) %c ."     , 0, x_cell->t);
         else                          snprintf(unit_answer, LEN_UNIT, "ycalc rpn        : (%4d) %c %s"    , x_count, x_cell->t, x_list);
      }
   }
   if (strcmp(a_question, "ycalc_reqs"   )  == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_UNIT, "ycalc reqs       : (----) -");
      else if (x_cell->ycalc == NULL)  snprintf(unit_answer, LEN_UNIT, "ycalc reqs       : (----) -");
      else {
         yCALC_show_reqs (x_cell->ycalc, &x_count, x_list);
         if (x_count      == 0)        snprintf(unit_answer, LEN_UNIT, "ycalc reqs       : (%4d) ."     , 0);
         else                          snprintf(unit_answer, LEN_UNIT, "ycalc reqs       : (%4d) %s"    , x_count, x_list);
      }
   }
   if (strcmp(a_question, "ycalc_pros"   )  == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_UNIT, "ycalc pros       : (----) -");
      else if (x_cell->ycalc == NULL)  snprintf(unit_answer, LEN_UNIT, "ycalc pros       : (----) -");
      else {
         yCALC_show_pros (x_cell->ycalc, &x_count, x_list);
         if (x_count      == 0)        snprintf(unit_answer, LEN_UNIT, "ycalc pros       : (%4d) ."     , 0);
         else                          snprintf(unit_answer, LEN_UNIT, "ycalc pros       : (%4d) %s"    , x_count, x_list);
      }
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



