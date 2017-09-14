/*============================----beg-of-source---============================*/
#include    "gyges.h"



char          /*-> initialize macro environment --------- [ whorl  [ ------ ]-*/
MACRO_init           (void)
{
   MACRO_reset ();
   my.macro_name     = '-';
   return 0;
}

char          /*-> check macro name for correctness ----- [ leaf   [ ------ ]-*/
MACRO_name_valid     (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (a_name == '@')                    return 1;
   --rce;  if (a_name >= 'a' && a_name <= 'z')   return 0;
   --rce;  if (a_name >= 'A' && a_name <= 'Z')   return 2;
   /*---(complete)-----------------------*/
   return rce;
}

char          /*-> fully reset macro mode --------------- [ leaf   [ ------ ]-*/
MACRO_reset          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           = 0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_senter  (__FUNCTION__);
   /*---(mode)---------------------------*/
   DEBUG_USER   yLOG_snote   ("macro_off");
   SET_MACRO_OFF;
   /*---(macro values)-------------------*/
   DEBUG_USER   yLOG_snote   ("reset values");
   my.macro_pos   =  -1;
   my.macro_len   =   0;
   my.macro_delay = '0';
   /*---(macro keystrokes)---------------*/
   DEBUG_USER   yLOG_snote   ("null macro keys");
   for (i = 0; i < LEN_RECD; ++i)  my.macro_keys [i] = '\0';
   my.macro_char  =   0;
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char          /*-> prepare a macro recording ------------ [ leaf   [ ------ ]-*/
MACRO_record_beg     (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   /*---(prepare)---------------------*/
   MACRO_reset ();
   my.macro_name = '-';
   /*---(check macro name)------------*/
   DEBUG_USER   yLOG_char    ("a_name"    , a_name);
   if (MACRO_name_valid (a_name) <  0)  {
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (a_name != '@')  my.macro_name = tolower (a_name);
   if (tolower (a_name) != a_name) {
      MACRO_fetch (my.macro_name);
      my.macro_keys [--my.macro_len] = '\0';
      my.macro_pos      = my.macro_len - 1;
      if (my.macro_pos < 0)  my.macro_char = 0;
      else                   my.macro_char = my.macro_keys [my.macro_pos];
   }
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char          /*-> complete a macro recording ----------- [ leaf   [ ------ ]-*/
MACRO_record_end     (void)
{
   MACRO_save  ();
   SET_MACRO_OFF;
   return 0;
}

char          /*-> add a key to a macro ----------------- [ leaf   [ ------ ]-*/
MACRO_record_add     (char a_key)
{
   switch (a_key) {
   case K_RETURN  :  a_key  = G_CHAR_RETURN;  break;  /* return char           */
   case K_ESCAPE  :  a_key  = G_CHAR_ESCAPE;  break;  /* escape char           */
   case K_TAB     :  a_key  = G_CHAR_TAB;     break;  /* tab char              */
   case K_BS      :  a_key  = G_CHAR_BS;      break;  /* backspace char        */
   case K_SPACE   :  a_key  = G_CHAR_SPACE;   break;  /* visual space          */
   }
   my.macro_char                  = a_key;
   my.macro_keys [my.macro_len++] = a_key;
   my.macro_keys [my.macro_len  ] = '\0';
   my.macro_pos                   = my.macro_len - 1;
   return 0;
}

char          /*-> put keys in globals ------------------ [ whorl  [ ------ ]-*/
MACRO_push_keys      (char *a_keys)
{
   if (a_keys == NULL)  strlcat (my.macro_keys, ""    , LEN_RECD);
   else                 strlcat (my.macro_keys, a_keys, LEN_RECD);
   strlcat (my.macro_keys, "Ø", LEN_RECD);
   my.macro_len  = strlen (my.macro_keys);
   if (my.macro_pos < 0)  my.macro_char = 0;
   else                   my.macro_char = my.macro_keys [my.macro_pos];
   return 0;
}

char          /*-> save current recording --------------- [ whorl  [ ------ ]-*/
MACRO_save           (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_ch        =  ' ';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   /*---(trim)---------------------------*/
   if (my.macro_len > 0)  --my.macro_len;
   my.macro_keys [my.macro_len] = '\0';
   /*---(save)---------------------------*/
   rc = CELL_macro_set (my.macro_keys);
   DEBUG_USER   yLOG_value   ("rc"        , rc);
   --rce;  if (rc    <  0) {
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char          /*-> fetch specific macro keystrokes ------ [ whorl  [ ------ ]-*/
MACRO_fetch          (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_ch        =  ' ';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   /*---(prepare)---------------------*/
   if (my.macro_name != a_name) MACRO_reset ();
   my.macro_name  = '-';
   /*---(check macro name)------------*/
   DEBUG_USER   yLOG_char    ("a_name"    , a_name);
   if (MACRO_name_valid (a_name) != 0)  {
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   my.macro_name = a_name;
   /*---(get contents)----------------*/
   rc = CELL_macro_get (my.macro_keys);
   DEBUG_USER   yLOG_value   ("rc"        , rc);
   --rce;  if (rc    <  0) {
      MACRO_reset ();
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("macro"     , my.macro_keys);
   /*---(check contents)--------------*/
   my.macro_len = strlen (my.macro_keys);
   DEBUG_USER   yLOG_value   ("len"       , my.macro_len);
   --rce;  if (my.macro_len <= 0) {
      MACRO_reset ();
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set globals)-----------------*/
   my.macro_keys [my.macro_len++] = G_CHAR_NULL;
   my.macro_keys [my.macro_len  ] = '\0';
   if (my.macro_pos < 0)  my.macro_char = 0;
   else                   my.macro_char = my.macro_keys [my.macro_pos];
   /*---(complete)--------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char          /*-> return current key in macro ---------- [ whorl  [ ------ ]-*/
MACRO_curr_key       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_ch        =  ' ';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_value   ("macro_pos" , my.macro_pos);
   --rce;  if (my.macro_pos >= my.macro_len) {
      MACRO_reset ();
      DEBUG_USER   yLOG_note    ("past the end");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_ch = my.macro_keys [my.macro_pos];
   DEBUG_USER   yLOG_char    ("x_ch"      , x_ch);
   if (x_ch <  0) {
      DEBUG_USER   yLOG_note    ("special character");
      DEBUG_USER   yLOG_value   ("256 + x_ch", 256 + x_ch);
      switch (256 + x_ch) {
      case G_CHAR_RETURN  :  x_ch = K_RETURN;  break;
      case G_CHAR_ESCAPE  :  x_ch = K_ESCAPE;  break;
      case G_CHAR_BS      :  x_ch = K_BS;      break;
      case G_CHAR_TAB     :  x_ch = K_TAB;     break;
      case G_CHAR_SPACE   :  x_ch = K_SPACE;   break;
      case G_CHAR_GROUP   :  x_ch = K_GROUP;   break;
      case G_CHAR_FIELD   :  x_ch = K_FIELD;   break;
      case G_CHAR_ALT     :                    break;
      case G_CHAR_CONTROL :                    break;
      case G_CHAR_WAIT    :                    break;
      case G_CHAR_BREAK   :                    break;
      case G_CHAR_HALT    :                    break;
      case G_CHAR_DISPLAY :                    break;
      case G_CHAR_NULL    :  x_ch = NULL;      break;
      default             :  x_ch = NULL;      break;
      }
      DEBUG_USER   yLOG_value   ("x_ch (new)", x_ch);
   }
   if (x_ch == NULL) {
      DEBUG_USER   yLOG_note    ("end of macro");
      MACRO_reset ();
   }
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return x_ch;
}

char          /*-> process macro sub-mode keys ---------- [ leaf   [ ------ ]-*/
MACRO_submode        (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_majors    [LEN_RECD]  = "q@";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr ());
   --rce;  if (yVIKEYS_mode_not (SMOD_MACRO))  {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("x_majors"   , x_majors);
   --rce;  if (strchr (x_majors, a_major) == 0) {
      DEBUG_USER   yLOG_note    ("a_major is not valid");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mode changes)-------------------*/
   if (a_minor == K_ESCAPE || a_minor == K_RETURN) {
      DEBUG_USER   yLOG_note    ("escape/return, nothing to do");
      yVIKEYS_mode_exit ();
      MACRO_reset       ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for recording)------------*/
   --rce;  if (a_major == 'q') {
      /*---(check macro name)------------*/
      if (MACRO_name_valid (a_minor) < 0)  {
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      MACRO_record_beg  (a_minor);
   }
   /*---(check for execution)------------*/
   --rce;  if (a_major == '@') {
      /*---(playback speed)--------------*/
      if (a_minor >= '0' && a_minor <= '9') {
         my.macro_delay    = a_minor;
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_major;
      }
      /*---(normal)----------------------*/
      if (a_minor == tolower (a_minor)) {
         yVIKEYS_mode_exit ();
         if (my.macro_delay    != '0'       )  SET_MACRO_DELAY;
         IF_MACRO_OFF                          SET_MACRO_RUN;
         if (a_minor != '@')  my.macro_name = a_minor;
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      /*---(debugging/playback)----------*/
      else {
         yVIKEYS_mode_exit ();
         SET_MACRO_PLAYBACK;
         my.macro_name     = tolower (a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      /*---(done)------------------------*/
   }
   /*---(errors fall through)------------*/
   --rce;
   MACRO_reset ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNIT_TEST_______o () { return; }

char*
MACRO_unit           (char *a_question, char a_macro)
{
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "macro            : question not understood");
   /*---(defenses)-----------------------*/
   a_macro = tolower (a_macro);
   if (MACRO_name_valid < 0) {
      strcpy  (unit_answer, "macro            : not a valid macro name");
      return unit_answer;
   }
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "curr"           )   == 0) {
      snprintf(unit_answer, LEN_STR, "macro curr   (%c) : %c %c %3d %02x %3d:%-.30s", my.macro_name, my.macro_mode, my.macro_delay, my.macro_pos, (uchar) my.macro_char, my.macro_len, my.macro_keys);
   }
   else if (strcmp (a_question, "info"           )   == 0) {
      snprintf(unit_answer, LEN_STR, "macro curr   (%c) : %c %c %3d %02x %3d:%-.30s", my.macro_name, my.macro_mode, my.macro_delay, my.macro_pos, (uchar) my.macro_char, my.macro_len, my.macro_keys);
   }
   else if (strcmp (a_question, "keys"           )   == 0) {
      snprintf(unit_answer, LEN_STR, "macro keys   (%c) : %-.45s", a_macro, my.macro_keys);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



/*============================----end-of-source---============================*/
