/*============================----beg-of-source---============================*/
#include   "gyges.h"


static char s_nada       [5] = "";



/*====================------------------------------------====================*/
/*===----                       yCALC_exist_config                     ----===*/
/*====================------------------------------------====================*/
static void   o___EXIST___________o (void) { return; }

char         /*-> save yCALC deproot to owner --------[ leaf   [ge.722.223.20]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
extern_ycalc_enabler     (void *a_owner, void *a_deproot)
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
extern_ycalc_pointer    (void *a_owner, char **a_source, char **a_type, double **a_value, char **a_string)
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
extern_ycalc_reaper     (void **a_owner)
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
   DEBUG_APIS   yLOG_char    ("type"      , x_owner->t);
   --rce;  if (x_owner->t != YCALC_DATA_BLANK) {
      DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(delete)-------------------------*/
   rc = CELL_delete (CHG_INPUT, x_owner->tab, x_owner->col, x_owner->row);
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
extern_ycalc_named      (char *a_label, char a_force, void **a_owner, void **a_deproot)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_owner     = NULL;
   short       x_col       =   0;           /* working col value              */
   short       x_row       =   0;           /* working row value              */
   short       x_tab       =   0;           /* working tab value              */
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
      DEBUG_PROG   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(search)-------------------------*/
   rc      = str2gyges (a_label, &x_col, &x_row, &x_tab, NULL, 0);
   DEBUG_APIS   yLOG_point   ("str2gyges" , rc);
   --rce;  if (rc < 0) {
      DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_owner = LOC_cell_at_loc  (x_tab, x_col, x_row);
   DEBUG_APIS   yLOG_point   ("x_owner"    , x_owner);
   --rce;  if (x_owner == NULL && a_force == YCALC_LOOK) {
      DEBUG_APIS   yLOG_note    ("owner does not exist and only in look mode");
      DEBUG_APIS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(new cell)-----------------------*/
   --rce;  if (x_owner == NULL) {
      DEBUG_APIS   yLOG_note    ("create a new one");
      rc = CELL_change (&x_owner, CHG_INPUT, x_tab, x_col, x_row, "");
      if (rc < 0) {
         DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_APIS   yLOG_point   ("x_owner"    , x_owner);
   }
   --rce;  if (x_owner == NULL) {
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
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
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gc.320.621.10]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
extern_ycalc_whos_at    (int x, int y, int z, char a_force, void **a_owner, void **a_deproot)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x_label     [LEN_LABEL];
   /*---(legal)--------------------------*/
   rc = str4gyges (x, y, z, 0, x_label);
   if (rc == 0)  rc = ycalc__mock_named (x_label, YCALC_LOOK, a_owner, a_deproot);
   /*---(complete)-----------------------*/
   return rc;
}

char*
extern_ycalc_labeler    (void *a_owner)
{
   tCELL      *x_owner     = NULL;
   DEBUG_DEPS   yLOG_senter  (__FUNCTION__);
   DEBUG_DEPS   yLOG_spoint  (a_owner);
   if (a_owner == NULL) {
      DEBUG_DEPS   yLOG_sexit   (__FUNCTION__);
      return s_nada;
   }
   x_owner = (tCELL *) a_owner;
   DEBUG_DEPS   yLOG_snote   (x_owner->label);
   DEBUG_DEPS   yLOG_sexit   (__FUNCTION__);
   return x_owner->label;
}



/*====================------------------------------------====================*/
/*===----                       yCALC_value_config                     ----===*/
/*====================------------------------------------====================*/
static void   o___VALUE___________o (void) { return; }

char         /*-> tbd --------------------------------[ leaf   [ge.742.422.B0]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
extern_ycalc_valuer     (void *a_owner, char *a_type, double *a_value, char **a_string)
{
   char        rce         =  -10;
   tCELL     *x_owner      = NULL;
   DEBUG_DEPS   yLOG_senter  (__FUNCTION__);
   DEBUG_DEPS   yLOG_spoint  (a_owner);
   if (a_type   != NULL)  *a_type   = '-';
   if (a_value  != NULL)  *a_value  = 0.0;
   if (a_string != NULL)  *a_string = s_nada;
   --rce;  if (a_owner  == NULL) {
      DEBUG_DEPS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   x_owner    = (tCELL *) a_owner;
   if (a_type   != NULL) {
      *a_type   = x_owner->t;
      DEBUG_DEPS   yLOG_schar   (*a_type);
   }
   if (a_value  != NULL) {
      *a_value  = x_owner->v_num;
      DEBUG_DEPS   yLOG_sdouble (*a_value);
   }
   if (a_string != NULL) {
      if      (x_owner->t     == 'E')     *a_string   = s_nada;
      else if (x_owner->t     == 's' && x_owner->s != NULL)  *a_string = x_owner->v_str;
      else if (x_owner->v_str != NULL)  *a_string = x_owner->v_str;
      else                               *a_string = s_nada;
      DEBUG_DEPS   yLOG_snote   (*a_string);
   }
   DEBUG_DEPS   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
extern_ycalc_address    (void *a_owner, int *x, int *y, int *z)
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
ycalc__mock_special     (void *a_owner, char a_what, double *a_value, char **a_string)
{
   char        rce         =  -10;
   tCELL      *x_owner     = NULL;
   DEBUG_DEPS   yLOG_senter  (__FUNCTION__);
   DEBUG_DEPS   yLOG_spoint  (a_owner);
   DEBUG_DEPS   yLOG_schar   (a_what);
   if (a_value  != NULL)  *a_value  = 0.0;
   if (a_string != NULL)  *a_string = s_nada;
   --rce;  if (a_owner   == NULL) {
      DEBUG_DEPS   yLOG_sexitr  (__FUNCTION__, rce);
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
   DEBUG_DEPS   yLOG_snote   (*a_string);
   DEBUG_DEPS   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       yVIKEYS                                ----===*/
/*====================------------------------------------====================*/
static void   o___YVIKEYS_________o (void) { return; }

char
extern_vikeys_saver    (char *a_contents)
{
   CELL_change  (NULL, CHG_INPUT, CTAB, CCOL, CROW, a_contents);
   return 0;
}
