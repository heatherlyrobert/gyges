/*============================----beg-of-source---============================*/
#include   "gyges.h"


static char   s_nada       [5] = "";
static char  *s_root     = "ROOT";

/*> typedef  struct cVARS  tVARS;                                                     <* 
 *> static struct cVARS {                                                             <* 
 *>    char        name        [LEN_LABEL];                                           <* 
 *>    char        label       [LEN_LABEL];                                           <* 
 *> };                                                                                <* 
 *> tVARS    s_vars   [100];                                                          <* 
 *> int      s_nvar       = 0;                                                        <*/

/*> char                                                                              <* 
 *> api_ycalc_addvar       (char *a_name, char *a_label)                              <* 
 *> {                                                                                 <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                       <* 
 *>    char        rce         =  -10;                                                <* 
 *>    char        rc          =    0;                                                <* 
 *>    char        x_dir       =  '-';                                                <* 
 *>    char        x_name      [LEN_LABEL] = "";                                      <* 
 *>    int         l           =    0;                                                <* 
 *>    tCELL      *x_cell      = NULL;                                                <* 
 *>    int         u, x, y;                                                           <* 
 *>    char        x_label     [LEN_LABEL] = "";                                      <* 
 *>    /+---(header)-------------------------+/                                       <* 
 *>    DEBUG_YCALC   yLOG_senter  (__FUNCTION__);                                      <* 
 *>    /+---(defense)------------------------+/                                       <* 
 *>    DEBUG_YCALC   yLOG_spoint  (a_name);                                            <* 
 *>    --rce;  if (a_name == NULL) {                                                  <* 
 *>       DEBUG_YCALC   yLOG_sexitr  (__FUNCTION__, rce);                              <* 
 *>       return rce;                                                                 <* 
 *>    }                                                                              <* 
 *>    DEBUG_YCALC   yLOG_snote   (a_name);                                            <* 
 *>    DEBUG_YCALC   yLOG_spoint  (a_label);                                           <* 
 *>    --rce;  if (a_label == NULL) {                                                 <* 
 *>       DEBUG_YCALC   yLOG_sexitr  (__FUNCTION__, rce);                              <* 
 *>       return rce;                                                                 <* 
 *>    }                                                                              <* 
 *>    DEBUG_YCALC   yLOG_snote   (a_label);                                           <* 
 *>    /+---(parse name)---------------------+/                                       <* 
 *>    l = strlen (x_name);                                                           <* 
 *>    DEBUG_YCALC   yLOG_sint    (l);                                                 <* 
 *>    --rce;  if (l < 2) {                                                           <* 
 *>       DEBUG_YCALC   yLOG_sexitr  (__FUNCTION__, rce);                              <* 
 *>       return rce;                                                                 <* 
 *>    }                                                                              <* 
 *>    x_dir = x_name [0];                                                            <* 
 *>    DEBUG_YCALC   yLOG_schar   (x_dir);                                             <* 
 *>    --rce;  if (strchr ("дезж", x_dir) == NULL) {                                  <* 
 *>       DEBUG_YCALC   yLOG_sexitr  (__FUNCTION__, rce);                              <* 
 *>       return rce;                                                                 <* 
 *>    }                                                                              <* 
 *>    ystrlcpy (x_name, a_name + 1, LEN_LABEL);                                       <* 
 *>    ystrltrim (x_name, ySTR_BOTH, LEN_LABEL);                                       <* 
 *>    /+---(fix label)----------------------+/                                       <* 
 *>    rc  = ystr2gyges (a_label, &u, &x, &y, NULL, NULL, 0, YSTR_USABLE);             <* 
 *>    DEBUG_YCALC   yLOG_sint    (rc);                                                <* 
 *>    --rce;  if (rc < 0) {                                                          <* 
 *>       DEBUG_YCALC   yLOG_sexitr  (__FUNCTION__, rce);                              <* 
 *>       return rce;                                                                 <* 
 *>    }                                                                              <* 
 *>    switch (x_dir) {                                                               <* 
 *>    case 'д'  :  --y;  break;                                                      <* 
 *>    case 'е'  :  ++y;  break;                                                      <* 
 *>    case 'з'  :  --x;  break;                                                      <* 
 *>    case 'ж'  :  ++x;  break;                                                      <* 
 *>    }                                                                              <* 
 *>    rc = ystr4gyges (u, x, y, 0, 0, x_label, YSTR_USABLE);                          <* 
 *>    DEBUG_YCALC   yLOG_sint    (rc);                                                <* 
 *>    --rce;  if (rc < 0) {                                                          <* 
 *>       DEBUG_YCALC   yLOG_sexitr  (__FUNCTION__, rce);                              <* 
 *>       return rce;                                                                 <* 
 *>    }                                                                              <* 
 *>    DEBUG_YCALC   yLOG_snote   (x_label);                                           <* 
 *>    /+---(add)----------------------------+/                                       <* 
 *>    ystrlcpy (s_vars [s_nvar].name , x_name , LEN_LABEL);                           <* 
 *>    ystrlcpy (s_vars [s_nvar].label, x_label, LEN_LABEL);                           <* 
 *>    ++s_nvar;                                                                      <* 
 *>    /+---(complete)-----------------------+/                                       <* 
 *>    DEBUG_YCALC   yLOG_sexit   (__FUNCTION__);                                      <* 
 *>    return 0;                                                                      <* 
 *> }                                                                                 <*/


/*> char                                                                              <* 
 *> api_ycalc_findvar      (char *a_name, int *u, int *x, int *y)                     <* 
 *> {                                                                                 <* 
 *>    char        rce         =  -10;                                                <* 
 *>    int         i           =    0;                                                <* 
 *>    --rce;  if (a_name == NULL)  return rce;                                       <* 
 *>    --rce;  if (u   == NULL)     return rce;                                       <* 
 *>    --rce;  if (x   == NULL)     return rce;                                       <* 
 *>    --rce;  if (y   == NULL)     return rce;                                       <* 
 *>    --rce;  for (i = 0; i < s_nvar; ++i) {                                         <* 
 *>       if (strcmp (a_name, s_vars [i].name) != 0)  continue;                       <* 
 *>       return ystr2gyges (s_vars [i].label, u, x, y, NULL, NULL, 0, YSTR_USABLE);   <* 
 *>    }                                                                              <* 
 *>    return rce;                                                                    <* 
 *> }                                                                                 <*/



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
   DEBUG_YCALC   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YCALC   yLOG_spoint  (a_owner);
   --rce;  if (a_owner == NULL) {
      DEBUG_YCALC   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert)------------------------*/
   x_owner = (tCELL *) a_owner;
   DEBUG_YCALC   yLOG_snote   (x_owner->label);
   /*---(check)--------------------------*/
   DEBUG_YCALC   yLOG_spoint  (a_deproot);
   DEBUG_YCALC   yLOG_spoint  (x_owner->ycalc);
   --rce;  if (a_deproot == x_owner->ycalc) {
      DEBUG_YCALC   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_deproot != NULL && x_owner->ycalc != NULL) {
      DEBUG_YCALC   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   x_owner->ycalc = a_deproot;
   DEBUG_YCALC   yLOG_spoint  (x_owner->ycalc);
   /*---(complete)-----------------------*/
   DEBUG_YCALC   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.933.522.50]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
api_ycalc_pointer       (void *a_owner, char **r_source, char **r_type, double **r_value, char **r_string)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tCELL      *x_owner     = NULL;
   /*---(header)-------------------------*/
   DEBUG_YCALC   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YCALC   yLOG_spoint  (a_owner);
   --rce;  if (a_owner  == NULL) {
      DEBUG_YCALC   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert)------------------------*/
   x_owner   = (tCELL *) a_owner;
   DEBUG_YCALC   yLOG_snote   (x_owner->label);
   /*---(save)---------------------------*/
   if (r_source != NULL) {
      DEBUG_YCALC   yLOG_snote   ("r_source");
      DEBUG_YCALC   yLOG_spoint  (*r_source);
      *r_source = &x_owner->source;
      DEBUG_YCALC   yLOG_spoint  (*r_source);
   }
   if (r_type  != NULL) {
      DEBUG_YCALC   yLOG_snote   ("r_type");
      DEBUG_YCALC   yLOG_spoint  (*r_type);
      *r_type   = &x_owner->type;
      DEBUG_YCALC   yLOG_spoint  (*r_type);
   }
   if (r_value  != NULL) {
      DEBUG_YCALC   yLOG_snote   ("r_value");
      DEBUG_YCALC   yLOG_spoint  (*r_value);
      *r_value  = &x_owner->v_num;
      DEBUG_YCALC   yLOG_spoint  (*r_value);
   }
   if (r_string != NULL) {
      DEBUG_YCALC   yLOG_snote   ("r_string");
      DEBUG_YCALC   yLOG_spoint  (*r_string);
      *r_string = &x_owner->v_str;
      DEBUG_YCALC   yLOG_spoint  (*r_string);
   }
   /*---(complete)-----------------------*/
   DEBUG_YCALC   yLOG_sexit   (__FUNCTION__);
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
   DEBUG_YCALC   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YCALC   yLOG_point   ("a_owner"   , a_owner);
   --rce;  if (a_owner  == NULL) {
      DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YCALC   yLOG_point   ("*a_owner"  , *a_owner);
   --rce;  if (*a_owner  == NULL) {
      DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, -rce);
      return -rce;
   }
   /*---(check deproot)------------------*/
   x_owner = (tCELL *) *a_owner;
   DEBUG_YCALC   yLOG_point   ("ycalc"     , x_owner->ycalc);
   --rce;  if (x_owner->ycalc  != NULL) {
      DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check type)---------------------*/
   DEBUG_YCALC   yLOG_info    ("label"     , x_owner->label);
   DEBUG_YCALC   yLOG_char    ("type"      , x_owner->type);
   --rce;  if (x_owner->type != YCALC_DATA_BLANK && x_owner->type != YCALC_DATA_GARBAGE) {
      DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(delete)-------------------------*/
   rc = CELL__delete (YMAP_NONE, x_owner->tab, x_owner->col, x_owner->row);
   --rce;  if (rc < 0) {
      DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   *a_owner = NULL;
   /*---(complete)-----------------------*/
   DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
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
   int         x_tab       =   0;           /* working tab value              */
   int         x_col       =   0;           /* working col value              */
   int         x_row       =   0;           /* working row value              */
   static char    x_sforce =  '?';
   static char   *x_label  [LEN_LABEL];
   static tCELL  *x_saved  = NULL;
   static char    x_rc     =    0;
   /*---(header)-------------------------*/
   DEBUG_YCALC   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (a_owner   != NULL)  *a_owner   = NULL;
   if (a_deproot != NULL)  *a_deproot = NULL;
   /*---(defense)------------------------*/
   DEBUG_YCALC   yLOG_point   ("a_label"   , a_label);
   --rce;  if (a_label == NULL) {
      DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YCALC   yLOG_info    ("a_label"   , a_label);
   /*---(root)---------------------------*/
   --rce;  if (strcmp (s_root, a_label) == 0) {
      if (my.root == NULL) {
         DEBUG_YCALC   yLOG_note    ("create the ROOT");
         rc = CELL__new_root (&(my.root));
         DEBUG_YCALC  yLOG_value   ("rc"        , rc);
         --rce;  if (rc < 0) {
            DEBUG_YCALC  yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         DEBUG_YCALC  yLOG_point   ("my.root"   , my.root);
         --rce;  if (my.root == NULL) {
            DEBUG_YCALC  yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         my.root->label = strdup (s_root);
      }
      x_owner = my.root;
   }
   /*---(search)-------------------------*/
   else {
      rc  = ystr2gyges (a_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_USABLE);
      DEBUG_YCALC   yLOG_value   ("address"   , rc);
      if (rc < 0) {
         DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_YCALC   yLOG_complex ("loc"        , "%3db, %3dx, %3dy", x_tab, x_col, x_row);
      x_owner = LOC_cell_at_loc  (x_tab, x_col, x_row);
      DEBUG_YCALC   yLOG_point   ("x_owner"    , x_owner);
      --rce;  if (x_owner == NULL && a_force == YCALC_LOOK) {
         DEBUG_YCALC   yLOG_note    ("owner does not exist and only in look mode");
         DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(new cell)-----------------------*/
      --rce;  if (x_owner == NULL) {
         DEBUG_YCALC   yLOG_note    ("create a new one");
         rc = CELL__create (&x_owner, x_tab, x_col, x_row);
         if (rc < 0) {
            DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         DEBUG_YCALC   yLOG_point   ("x_owner"    , x_owner);
      }
      --rce;  if (x_owner == NULL) {
         DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(save)---------------------------*/
   DEBUG_YCALC   yLOG_complex ("loc_final"  , "%3db, %3dx, %3dy", x_owner->tab, x_owner->col, x_owner->row);
   /*> if (strcmp ("ROOT", a_label) != 0) {                                           <* 
    *>    x_sforce = a_force;                                                         <* 
    *>    x_saved  = x_owner;                                                         <* 
    *>    r_rc     = 0;                                                               <* 
    *>    ystrlcpy (x_label, a_label, LEN_LABEL);                                      <* 
    *>    DEBUG_YCALC   yLOG_point   ("x_saved"   , x_saved);                          <* 
    *> }                                                                              <*/
   /*---(handle normal)------------------*/
   DEBUG_YCALC   yLOG_note    ("success");
   if (a_owner   != NULL) {
      *a_owner   = x_owner;
      DEBUG_YCALC   yLOG_point   ("*a_owner"  , *a_owner);
   }
   if (a_deproot != NULL) {
      *a_deproot = x_owner->ycalc;
      DEBUG_YCALC   yLOG_point   ("*a_deproot", *a_deproot);
   }
   /*---(complete)-----------------------*/
   DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gc.320.621.10]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
api_ycalc_whos_at       (int u, int x, int y, int z, char a_force, void **a_owner, void **a_deproot)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x_label     [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_YCALC  yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (a_owner   != NULL)  *a_owner   = NULL;
   if (a_deproot != NULL)  *a_deproot = NULL;
   /*---(legal)--------------------------*/
   rc = ystr4gyges (u, x, y, z, 0, x_label, YSTR_USABLE);
   DEBUG_YCALC  yLOG_complex ("gyges"     , "%-10.10s, %2du, %3dx, %4dy, %drc", x_label, u, x, y, rc);
   if (rc == 0)  rc = api_ycalc_named (x_label, a_force, a_owner, a_deproot);
   /*---(complete)-----------------------*/
   DEBUG_YCALC  yLOG_exit    (__FUNCTION__);
   return rc;
}

char*
api_ycalc_labeler       (void *a_owner)
{
   tCELL      *x_owner     = NULL;
   DEBUG_YCALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YCALC   yLOG_point   ("a_owner"   , a_owner);
   if (a_owner == NULL) {
      DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
      return s_nada;
   }
   x_owner = (tCELL *) a_owner;
   DEBUG_YCALC   yLOG_point   ("root"      , my.root);
   DEBUG_YCALC   yLOG_point   ("->label"   , x_owner->label);
   if (x_owner->label == NULL) {
      DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
      return s_nada;
   }
   DEBUG_YCALC   yLOG_info    ("->label"   , x_owner->label);
   DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
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
   DEBUG_YCALC   yLOG_senter  (__FUNCTION__);
   DEBUG_YCALC   yLOG_spoint  (a_owner);
   if (a_type   != NULL)  *a_type   = '-';
   if (a_value  != NULL)  *a_value  = 0.0;
   if (a_string != NULL)  *a_string = s_nada;
   --rce;  if (a_owner  == NULL) {
      DEBUG_YCALC   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   x_owner    = (tCELL *) a_owner;
   if (a_type   != NULL) {
      *a_type   = x_owner->type;
      DEBUG_YCALC   yLOG_schar   (*a_type);
   }
   if (a_value  != NULL) {
      *a_value  = x_owner->v_num;
      DEBUG_YCALC   yLOG_sdouble (*a_value);
   }
   if (a_string != NULL) {
      if        (x_owner->type     == 'E') {
         *a_string   = s_nada;
      } else if (x_owner->type     == 's'){
         *a_string = x_owner->source;
      } else if (x_owner->v_str != NULL) {
         *a_string = x_owner->v_str;
      } else {
         *a_string = s_nada;
      }
      DEBUG_YCALC   yLOG_snote   (*a_string);
   }
   DEBUG_YCALC   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
api_ycalc_address       (void *a_owner, int *u, int *x, int *y, int *z)
{
   char        rc          =    0;
   tCELL      *x_owner     = NULL;
   DEBUG_YCALC  yLOG_enter   (__FUNCTION__);
   DEBUG_YCALC  yLOG_note    ("defaulting");
   if (u != NULL)  *u   = 0;
   if (x != NULL)  *x   = 0;
   if (y != NULL)  *y   = 0;
   if (z != NULL)  *z   = 0;
   DEBUG_YCALC  yLOG_point   ("a_owner"   , a_owner);
   if (a_owner == NULL) {
      DEBUG_YCALC  yLOG_note    ("owner null");
      DEBUG_YCALC  yLOG_exitr   (__FUNCTION__, -1);
      return -1;
   }
   x_owner    = (tCELL     *) a_owner;
   DEBUG_YCALC  yLOG_point   ("label"     , x_owner->label);
   DEBUG_YCALC  yLOG_note    (x_owner->label);
   rc = ystr2gyges (x_owner->label, u, x, y, z, NULL, 0, YSTR_USABLE);
   DEBUG_YCALC  yLOG_exitr   (__FUNCTION__, rc);
   return rc;
}

char
api_ycalc_special       (void *a_owner, char a_what, double *a_value, char **a_string)
{
   char        rce         =  -10;
   tCELL      *x_owner     = NULL;
   DEBUG_YCALC   yLOG_senter  (__FUNCTION__);
   DEBUG_YCALC   yLOG_spoint  (a_owner);
   DEBUG_YCALC   yLOG_schar   (a_what);
   if (a_value  != NULL)  *a_value  = 0.0;
   if (a_string != NULL)  *a_string = s_nada;
   --rce;  if (a_owner   == NULL) {
      DEBUG_YCALC   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   x_owner   = (tCELL     *) a_owner;
   --rce;  switch (a_what) {
   case G_SPECIAL_SOURCE :
      if (a_string != NULL)  *a_string = x_owner->source;
      break;
   case G_SPECIAL_PRINT  :
      if (a_string != NULL)  *a_string = x_owner->print;
      break;
   case G_SPECIAL_TYPE   :
      if (a_value  != NULL)  *a_value  = x_owner->type;
      break;
   }
   DEBUG_YCALC   yLOG_snote   (*a_string);
   DEBUG_YCALC   yLOG_sexit   (__FUNCTION__);
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
   int         b, x, y, z;
   int         i           =    0;
   int         w           =    0;
   /*---(header)-------------------------*/
   DEBUG_YCALC   yLOG_enter   (__FUNCTION__);
   /*---(legal)--------------------------*/
   DEBUG_YCALC   yLOG_point   ("a_owner"   , a_owner);
   if (a_owner == NULL) {
      DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_YCALC   yLOG_point   ("a_width"   , a_width);
   if (a_width == NULL) {
      DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_YCALC   yLOG_point   ("a_merge"   , a_merge);
   if (a_merge == NULL) {
      DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(initialize)---------------------*/
   x_owner  = (tCELL *) a_owner;
   DEBUG_YCALC   yLOG_value   ("tab"       , x_owner->tab);
   DEBUG_YCALC   yLOG_value   ("col"       , x_owner->col);
   DEBUG_YCALC   yLOG_point   ("C_parent"  , x_owner->C_parent);
   w         = x_owner->C_parent->size;
   DEBUG_YCALC   yLOG_value   ("w"         , w);
   *a_width = w;
   *a_merge = 0;
   s_owners [*a_merge] = x_owner;
   s_widths [*a_merge] = w;
   /*---(look for mergse)----------------*/
   rc = ystr2gyges (x_owner->label, &b, &x, &y, &z, NULL, 0, YSTR_USABLE);
   DEBUG_YCALC   yLOG_value   ("ystr2gyges" , rc);
   if (rc < 0)  {
      DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_YCALC   yLOG_complex ("owner"     , "%-10p, %-5s, %3db, %3dx, %3dy, %3dz, %3dw", x_owner, x_owner->label, b, x, y, z, w);
   for (i = x + 1; i < x + 20; ++i) {
      DEBUG_YCALC   yLOG_bullet  (i          , "check for merges to right");
      /*---(filter non-valid)--*/
      rc = VALID_col (i);
      /*> rc = ystr4gyges (b, i, y, z, 0, x_label, YSTR_USABLE);                       <*/
      if (rc < 0)  break;
      rc = api_ysort_by_coord (&x_owner, b, i, y);
      /*> rc = api_ycalc_whos_at (b, i, y, z, YCALC_LOOK, &x_owner, NULL);            <*/
      if (rc < 0 || x_owner == NULL)              break;
      if (x_owner->type != YCALC_DATA_MERGED)     break;
      /*> w         = COL_size (x_owner->tab, x_owner->col);                          <*/
      w         = x_owner->C_parent->size;
      DEBUG_YCALC   yLOG_complex ("owner"     , "%-10p, %-5s, %3db, %3dx, %3dy, %3dz, %3dw", x_owner, x_owner->label, b, i, y, z, w);
      *a_width += w;
      ++(*a_merge);
      s_owners [*a_merge] = x_owner;
      s_widths [*a_merge] = w;
   }
   DEBUG_YCALC   yLOG_value   ("*a_width"  , *a_width);
   /*---(complete)-----------------------*/
   DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
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
   DEBUG_YCALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YCALC   yLOG_point   ("a_full"    , a_full);
   if (a_full == NULL) {
      DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_YCALC   yLOG_value   ("a_merge"   , a_merge);
   for (i = 0; i <= a_merge; ++i) {
      x_owner = s_owners [i];
      DEBUG_YCALC  yLOG_point   ("x_owner"   , x_owner);
      if (x_owner == NULL)  break;
      w       = s_widths [i];
      DEBUG_YCALC  yLOG_value   ("w"         , w);
      while (p == NULL)  p = (char*) malloc (w + 1);
      DEBUG_YCALC  yLOG_point   ("p"         , p);
      sprintf (p, "%-*.*s", w, w, a_full + wa);
      DEBUG_YCALC  yLOG_info    ("p"      , p);
      DEBUG_YCALC  yLOG_point   ("->print", x_owner->print);
      if (x_owner->print != NULL) {
         free (x_owner->print);
         x_owner->print = NULL;
      }
      x_owner->print = p;
      p    = NULL;
      wa   += w;
   }
   /*---(complete)-----------------------*/
   DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_ycalc_printer       (void *a_owner)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_owner     = NULL;
   double      x_value     =  0.0;
   char        s           [LEN_RECD];
   char        t           [LEN_RECD];
   char        x_out       [LEN_RECD];
   char        x_len       =    0;
   int         w           =    0;
   int         c           =    0;
   char        x_type      =  '-';
   /*---(header)-------------------------*/
   DEBUG_YCALC   yLOG_enter   (__FUNCTION__);
   DEBUG_YCALC   yLOG_point   ("a_owner"   , a_owner);
   /*---(defense)------------------------*/
   --rce;  if (a_owner   == NULL) {
      DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_owner   = (tCELL     *) a_owner;
   /*---(filter unhomed)-----------------*/
   DEBUG_YCALC   yLOG_complex ("loc"       , "%-10.10p %-7.7s, %2d, %3dc %-10.10p, %4dr %-10.10p", x_owner, x_owner->label, x_owner->tab, x_owner->col, x_owner->C_parent, x_owner->row, x_owner->R_parent);
   --rce;  if (x_owner->C_parent == NULL || x_owner->R_parent == NULL) {
      DEBUG_YCALC   yLOG_note    ("live, but C_parent || R_parent are null, FUCKED");
      DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(filter unhooked)----------------*/
   --rce;  if (x_owner->tab  == UNHOOKED || x_owner->col  == UNHOOKED || x_owner->row  == UNHOOKED) {
      DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(contents)-----------------------*/
   DEBUG_YCALC   yLOG_char    ("type"      , x_owner->type);
   DEBUG_YCALC   yLOG_char    ("decs"      , x_owner->decs);
   DEBUG_YCALC   yLOG_char    ("format"    , x_owner->format);
   DEBUG_YCALC   yLOG_char    ("align"     , x_owner->align);
   /*---(reset printing errors)----------*/
   if (x_owner->type == YCALC_DATA_ERROR && x_owner->print != NULL && strncmp (x_owner->print, "#p/", 3) == 0) {
      DEBUG_YCALC   yLOG_note    ("found an error");
      x_type        = x_owner->type;
      x_owner->type = x_owner->print [9];  /*    #p/ali  (=)  */
   }
   /*---(get size)-----------------------*/
   api__ycalc_width (x_owner, &w, &c);
   DEBUG_YCALC   yLOG_value   ("w"         , w);
   --rce;  if (w <= 0) {
      DEBUG_YCALC   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YCALC   yLOG_value   ("c"         , c);
   /*---(contents)-----------------------*/
   if (strchr (YCALC_GROUP_NUM, x_owner->type) != NULL) {
      DEBUG_YCALC   yLOG_note    ("number-type");
      DEBUG_YCALC   yLOG_value   ("value"     , x_owner->v_num);
      x_value = x_owner->v_num;
      rc = ystrl4main (x_value, s, x_owner->decs - '0', x_owner->format, x_owner->unit, LEN_RECD);
      DEBUG_YCALC   yLOG_value   ("strl4main" , rc);
      DEBUG_YCALC   yLOG_info    ("string"    , s);
      if      (rc < 0)                  strcpy (t, s);
      else if (x_owner->align == '?')   rc = ystrlpad (s, t, '!', '>', w - 1);
      else                              rc = ystrlpad (s, t, '!', x_owner->align, w - 1);
   } else if (x_owner->type == (uchar) YCALC_DATA_VAR) {
      DEBUG_YCALC   yLOG_note    ("variable-type");
      ystrlcpy (s, x_owner->source, LEN_RECD);
      DEBUG_YCALC   yLOG_info    ("variable"  , s);
      rc = ystrlpad (s, t, '?', x_owner->align, w - 1);
   } else if (strchr (YCALC_GROUP_STR, x_owner->type) != NULL) {
      DEBUG_YCALC   yLOG_note    ("string-type");
      if (x_owner->v_str != NULL)  ystrlcpy (s, x_owner->v_str , LEN_RECD);
      else                         ystrlcpy (s, x_owner->source, LEN_RECD);
      DEBUG_YCALC   yLOG_info    ("string"    , s);
      if (x_owner->align == '?')  rc = ystrlpad (s, t, x_owner->format, '<'       , w - 1);
      else                        rc = ystrlpad (s, t, x_owner->format, x_owner->align, w - 1);
   } else if (x_owner->type == YCALC_DATA_CADDR || x_owner->type == YCALC_DATA_RLIKE) {
      DEBUG_YCALC   yLOG_note    ("calc-address-type");
      strcpy (s, "!");
      if (x_owner->v_str != NULL)  ystrlcat (s, x_owner->v_str, LEN_RECD);
      else                         ystrlcat (s, "???"        , LEN_RECD);
      DEBUG_YCALC   yLOG_info    ("string"    , s);
      if (x_owner->align == '?')  rc = ystrlpad (s, t, x_owner->format, '<'       , w - 1);
      else                        rc = ystrlpad (s, t, x_owner->format, x_owner->align, w - 1);
   } else if (strchr (YCALC_GROUP_POINT, x_owner->type) != NULL) {
      DEBUG_YCALC   yLOG_note    ("pointer-type");
      ystrlcpy (s, x_owner->source, LEN_RECD);
      DEBUG_YCALC   yLOG_info    ("pointer"   , s);
      rc = ystrlpad (s, t, x_owner->format, x_owner->align, w - 1);
   } else if (YCALC_DATA_ERROR == x_owner->type) {
      DEBUG_YCALC   yLOG_note    ("error-type");
      if (x_owner->v_str != NULL)  ystrlcpy (s, x_owner->v_str, LEN_RECD);
      else                         ystrlcpy (s, "#?/???"      , LEN_RECD);
      DEBUG_YCALC   yLOG_info    ("pointer"   , s);
      rc = ystrlpad (s, t, '!', '<', w - 1);
   } else if (x_owner->type == YCALC_DATA_BLANK) {
      DEBUG_YCALC   yLOG_note    ("blank-type");
      ystrlcpy (s, "-", LEN_RECD);
      DEBUG_YCALC   yLOG_info    ("blank"     , s);
      rc = ystrlpad (s, t, '!', '>', w - 1);
   } else if (x_owner->type == YCALC_DATA_INTERN) {
      DEBUG_YCALC   yLOG_note    ("internal-type");
      ystrlcpy (s, x_owner->source, LEN_RECD);
      DEBUG_YCALC   yLOG_info    ("range"     , s);
      rc = ystrlpad (s, t, x_owner->format, x_owner->align, w - 1);
   } else {
      DEBUG_YCALC   yLOG_note    ("non-printing type");
      DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for errors)---------------*/
   DEBUG_YCALC   yLOG_info    ("trim/pad"  , t);
   DEBUG_YCALC   yLOG_value   ("strlpad"   , rc);
   if (rc < 0) {
      ystrltrim (t, ySTR_BOTH, LEN_LABEL);
      sprintf (t, "%s  (%c)", t, x_owner->type);
      x_owner->type = YCALC_DATA_ERROR;
      strcpy (s, t);
      rc = ystrlpad (s, t, '!', '<', w - 1);
   }
   sprintf (x_out, "%s ", t);
   DEBUG_YCALC   yLOG_info    ("final"     , x_out);
   /*---(parse)--------------------------*/
   api__ycalc_parse (x_out, c);
   /*---(handle fixed present errors)----*/
   if (x_type == YCALC_DATA_ERROR && x_owner->type != YCALC_DATA_ERROR) {
      yCALC_handle (x_owner->label);
   }
   /*---(complete)-----------------------*/
   DEBUG_YCALC   yLOG_exit    (__FUNCTION__);
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
   char        x_label     [LEN_LABEL];
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
      rc     = ystr2gyges (a_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_USABLE);
      if (rc < 0) {
         sprintf (unit_answer, "ycalc error      : label <%s> not legal", a_label);
         return unit_answer;
      }
      rc = ystr4gyges (x_tab, x_col, x_row, 0, 0, x_label, YSTR_USABLE);
      if (rc < 0) {
         sprintf (unit_answer, "ycalc error      : label <%s> not in-range", a_label);
         return unit_answer;
      }
      x_cell = LOC_cell_at_loc  (x_tab, x_col, x_row);
      /*> if (x_cell == NULL) {                                                         <* 
       *>    sprintf (unit_answer, "s_celln          : label <%s> is NULL", a_label);   <* 
       *>    return unit_answer;                                                        <* 
       *> }                                                                             <*/
   }
   /*---(ycalc information)--------------*/
   if (strcmp(a_question, "ycalc_rpn"    )  == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_FULL, "ycalc rpn        : (----) - -");
      else if (x_cell->ycalc == NULL)  snprintf(unit_answer, LEN_FULL, "ycalc rpn        : (----) %c -", x_cell->type);
      else {
         yCALC_show_rpn  (x_cell->ycalc, &x_count, x_list);
         if (x_count      == 0)        snprintf(unit_answer, LEN_FULL, "ycalc rpn        : (%4d) %c ."     , 0, x_cell->type);
         else                          snprintf(unit_answer, LEN_FULL, "ycalc rpn        : (%4d) %c %s"    , x_count, x_cell->type, x_list);
      }
   }
   if (strcmp(a_question, "ycalc_reqs"   )  == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_FULL, "ycalc reqs       : (----) -");
      else if (x_cell->ycalc == NULL)  snprintf(unit_answer, LEN_FULL, "ycalc reqs       : (----) -");
      else {
         yCALC_show_reqs (x_cell->ycalc, &x_count, x_list);
         if (x_count      == 0)        snprintf(unit_answer, LEN_FULL, "ycalc reqs       : (%4d) ."     , 0);
         else                          snprintf(unit_answer, LEN_FULL, "ycalc reqs       : (%4d) %s"    , x_count, x_list);
      }
   }
   if (strcmp(a_question, "ycalc_pros"   )  == 0) {
      if      (x_cell        == NULL)  snprintf(unit_answer, LEN_FULL, "ycalc pros       : (----) -");
      else if (x_cell->ycalc == NULL)  snprintf(unit_answer, LEN_FULL, "ycalc pros       : (----) -");
      else {
         yCALC_show_pros (x_cell->ycalc, &x_count, x_list);
         if (x_count      == 0)        snprintf(unit_answer, LEN_FULL, "ycalc pros       : (%4d) ."     , 0);
         else                          snprintf(unit_answer, LEN_FULL, "ycalc pros       : (%4d) %s"    , x_count, x_list);
      }
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



