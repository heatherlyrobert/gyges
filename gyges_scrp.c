/*============================----beg-of-source---============================*/
#include    "gyges.h"






/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___PROGRAM_________o () { return; }

char          /*-> initialize macro environment --------- [ whorl  [ ------ ]-*/
MACRO_init           (void)
{
   MACRO_reset ();
   my.macro_name     = '-';
   return 0;
}



/*====================------------------------------------====================*/
/*===----                           utility                            ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UTILITY_________o () { return; }

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
   DEBUG_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(mode)---------------------------*/
   DEBUG_SCRP   yLOG_snote   ("macro_off");
   SET_MACRO_OFF;
   /*---(macro values)-------------------*/
   DEBUG_SCRP   yLOG_snote   ("reset values");
   my.macro_pos   =  -1;
   my.macro_len   =   0;
   my.macro_delay = '0';
   /*---(macro keystrokes)---------------*/
   DEBUG_SCRP   yLOG_snote   ("null macro keys");
   for (i = 0; i < LEN_RECD; ++i)  my.macro_keys [i] = K_NULL;
   my.macro_char  =   0;
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       saving and fetching                    ----===*/
/*====================------------------------------------====================*/
PRIV void  o___SAVING__________o () { return; }

char          /*-> save current recording --------------- [ whorl  [ ------ ]-*/
MACRO_save           (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_ch        =  ' ';
   int         x_len       =    0;
   char        x_macro     [LEN_RECD]  = "";
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(trim)---------------------------*/
   strlcpy (x_macro, my.macro_keys, LEN_RECD);
   x_len = strlen (x_macro);
   if (x_len > 0)  x_macro [--x_len] = K_NULL;
   /*---(save)---------------------------*/
   rc = CELL_macro_set (x_macro);
   DEBUG_SCRP   yLOG_value   ("rc"        , rc);
   --rce;  if (rc    <  0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear)--------------------------*/
   MACRO_reset ();
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
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
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)---------------------*/
   if (my.macro_name != a_name) MACRO_reset ();
   IF_MACRO_RECORDING           MACRO_reset ();
   my.macro_name  = '-';
   /*---(check macro name)------------*/
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   if (MACRO_name_valid (a_name) != 0)  {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   my.macro_name = a_name;
   /*---(get contents)----------------*/
   rc = CELL_macro_get (my.macro_keys);
   DEBUG_SCRP   yLOG_value   ("rc"        , rc);
   --rce;  if (rc    <  0) {
      MACRO_reset ();
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_info    ("macro"     , my.macro_keys);
   /*---(check contents)--------------*/
   my.macro_len = strlen (my.macro_keys);
   DEBUG_SCRP   yLOG_value   ("len"       , my.macro_len);
   --rce;  if (my.macro_len <= 0) {
      MACRO_reset ();
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set globals)-----------------*/
   my.macro_keys [my.macro_len++] = G_CHAR_NULL;
   my.macro_keys [my.macro_len  ] = K_NULL;
   if (my.macro_pos < 0)  my.macro_char = 0;
   else                   my.macro_char = my.macro_keys [my.macro_pos];
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         macro recording                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___RECORD__________o () { return; }

char          /*-> prepare a macro recording ------------ [ leaf   [ ------ ]-*/
MACRO_record_beg     (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)---------------------*/
   MACRO_reset ();
   my.macro_name = '-';
   /*---(check macro name)------------*/
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   if (MACRO_name_valid (a_name) <  0)  {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert name)----------------*/
   if (a_name != '@')  my.macro_name = tolower (a_name);
   /*---(handle append)---------------*/
   if (tolower (a_name) != a_name) {
      MACRO_fetch (my.macro_name);
      my.macro_keys [--my.macro_len] = K_NULL;
      my.macro_pos      = my.macro_len - 1;
      if (my.macro_pos < 0)  my.macro_char = 0;
      else                   my.macro_char = my.macro_keys [my.macro_pos];
   }
   /*---(add placeholder)-------------*/
   my.macro_keys [my.macro_len++] = G_CHAR_PLACE;
   my.macro_keys [my.macro_len  ] = K_NULL;
   /*---(turn on record)--------------*/
   SET_MACRO_RECORD;
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char          /*-> add a key to a macro ----------------- [ leaf   [ ------ ]-*/
MACRO_record_add     (char a_key)
{
   IF_MACRO_RECORDING {
      switch (a_key) {
      case K_RETURN  :  a_key  = G_CHAR_RETURN;  break;  /* return char           */
      case K_ESCAPE  :  a_key  = G_CHAR_ESCAPE;  break;  /* escape char           */
      case K_TAB     :  a_key  = G_CHAR_TAB;     break;  /* tab char              */
      case K_BS      :  a_key  = G_CHAR_BS;      break;  /* backspace char        */
      case K_SPACE   :  a_key  = G_CHAR_SPACE;   break;  /* visual space          */
      }
      my.macro_char                    = a_key;
      my.macro_keys [my.macro_len - 1] = a_key;
      my.macro_keys [my.macro_len++  ] = G_CHAR_PLACE;
      my.macro_keys [my.macro_len    ] = K_NULL;
      my.macro_pos                   = my.macro_len - 2;
   }
   return 0;
}

char          /*-> put keys in globals ------------------ [ whorl  [ ------ ]-*/
MACRO_record_addstr  (char *a_keys)
{
   /*---(look for suffix)----------------*/
   if (my.macro_keys [my.macro_len - 1] == (schar) G_CHAR_PLACE) {
      my.macro_keys [--my.macro_len  ] =  K_NULL;
   }
   if (my.macro_keys [my.macro_len - 1] == (schar) G_CHAR_NULL ) {
      my.macro_keys [--my.macro_len  ] =  K_NULL;
   }
   /*---(add keys)-----------------------*/
   if (a_keys == NULL)  strlcat (my.macro_keys, ""    , LEN_RECD);
   else                 strlcat (my.macro_keys, a_keys, LEN_RECD);
   /*---(add suffix)---------------------*/
   my.macro_len  = strlen (my.macro_keys);
   my.macro_keys [my.macro_len  ] =  G_CHAR_PLACE;
   my.macro_keys [++my.macro_len] =  K_NULL;
   /*---(update pos/char)----------------*/
   my.macro_pos = my.macro_len - 2;
   if (my.macro_pos < 0)  my.macro_char = 0;
   else                   my.macro_char = my.macro_keys [my.macro_pos];
   /*---(complete)-----------------------*/
   return 0;
}

char          /*-> complete a macro recording ----------- [ leaf   [ ------ ]-*/
MACRO_record_end     (void)
{
   IF_MACRO_RECORDING {
      if (my.macro_len >= 2 && my.macro_keys [my.macro_len - 2] == 'q') {
         my.macro_keys [my.macro_len - 2] = G_CHAR_NULL;
         my.macro_keys [my.macro_len - 1] = K_NULL;
         --my.macro_len;
      } else if (my.macro_len >= 1) {
         my.macro_keys [my.macro_len - 1] = G_CHAR_NULL;
      }
      my.macro_pos  = 0;
      my.macro_char = my.macro_keys [my.macro_pos];
      MACRO_save  ();
      SET_MACRO_OFF;
   }
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         macro execution                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___EXECUTE_________o () { return; }

char          /*-> set or adjust delay value ------------ [ whorl  [ ------ ]-*/
MACRO_delay_set      (char a_delay)
{
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_senter  (__FUNCTION__);
   DEBUG_SCRP   yLOG_schar   (a_delay);
   /*---(set)----------------------------*/
   if (a_delay >= '0' && a_delay <= '9') {
      DEBUG_SCRP   yLOG_note    ("set abs");
      my.macro_delay = a_delay;
   }
   if (a_delay >= '>') {
      DEBUG_SCRP   yLOG_note    ("increase");
      if (my.macro_delay > '0')  --my.macro_delay;
   }
   if (a_delay >= '>') {
      DEBUG_SCRP   yLOG_note    ("decrease");
      if (my.macro_delay < '9')  ++my.macro_delay;
   }
   /*---(new value)-------------------*/
   DEBUG_SCRP   yLOG_schar   (a_delay);
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char          /*-> prepare a macro execution ------------ [ leaf   [ ------ ]-*/
MACRO_exec_beg       (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   /*---(check macro name)------------*/
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   if (MACRO_name_valid (a_name) <  0)  {
      my.macro_name = '-';
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(normal)----------------------*/
   if (a_name == tolower (a_name)) {
      DEBUG_SCRP   yLOG_note    ("normal execution");
      if (my.macro_delay    != '0'       )  SET_MACRO_DELAY;
      IF_MACRO_OFF                          SET_MACRO_RUN;
      if (a_name != '@')  my.macro_name = a_name;
   }
   /*---(debugging/playback)----------*/
   else {
      DEBUG_SCRP   yLOG_note    ("debug execution");
      SET_MACRO_PLAYBACK;
      my.macro_name     = tolower (a_name);
   }
   /*---(macro name)------------------*/
   DEBUG_SCRP   yLOG_char    ("macro_name", my.macro_name);
   /*---(get macro)-------------------*/
   MACRO_fetch (my.macro_name);
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
MACRO_exec_adv       (void)
{
   IF_MACRO_OFF   return 0;
   ++my.macro_pos;
   if (my.macro_pos < 0)  my.macro_char = 0;
   else                   my.macro_char = my.macro_keys [my.macro_pos];
   return 0;
}

char          /*-> return current key in macro ---------- [ whorl  [ ------ ]-*/
MACRO_exec_key       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_ch        =  ' ';
   /*---(header)-------------------------*/
   IF_MACRO_OFF   return 0;
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_SCRP   yLOG_value   ("macro_pos" , my.macro_pos);
   --rce;  if (my.macro_pos >= my.macro_len) {
      MACRO_reset ();
      DEBUG_SCRP   yLOG_note    ("past the end");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_ch = my.macro_keys [my.macro_pos];
   DEBUG_SCRP   yLOG_char    ("x_ch"      , x_ch);
   if (x_ch <  0) {
      DEBUG_SCRP   yLOG_note    ("special character");
      DEBUG_SCRP   yLOG_value   ("256 + x_ch", 256 + x_ch);
      /*---(translate special)-----------*/
      switch (256 + x_ch) {
      case G_CHAR_RETURN  :  x_ch = K_RETURN;  break;
      case G_CHAR_ESCAPE  :  x_ch = K_ESCAPE;  break;
      case G_CHAR_BS      :  x_ch = K_BS;      break;
      case G_CHAR_TAB     :  x_ch = K_TAB;     break;
      case G_CHAR_SPACE   :  x_ch = K_SPACE;   break;
      case G_CHAR_GROUP   :  x_ch = K_GROUP;   break;
      case G_CHAR_FIELD   :  x_ch = K_FIELD;   break;
      case G_CHAR_ALT     :  x_ch = K_SPACE;   break;
      case G_CHAR_CONTROL :  x_ch = K_SPACE;   break;
      }
      DEBUG_SCRP   yLOG_value   ("x_ch (new)", x_ch);
      /*---(handle controls)-------------*/
      if (x_ch < 0) {
         x_ch = MACRO_exec_control (x_ch);
      }
   }
   if (x_ch == NULL) {
      DEBUG_SCRP   yLOG_note    ("end of macro");
      MACRO_reset ();
   }
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return x_ch;
}

char
MACRO_exec_control   (char a_key)
{
   if (a_key >= 0)  return 0;
   switch (256 + a_key) {
   case G_CHAR_WAIT    : sleep (1);            a_key = K_SPACE;  break;
   case G_CHAR_BREAK   : SET_MACRO_PLAYBACK;   a_key = K_SPACE;  break;
   case G_CHAR_HALT    :                       a_key = K_NULL;   break;
   case G_CHAR_DISPLAY : CURS_main ();         a_key = K_SPACE;  break;
   case G_CHAR_NULL    :                       a_key = K_NULL;   break;
   default             :                       a_key = K_NULL;   break;
   }
   return a_key;
}

char
MACRO_exec_wait      (void)
{
   struct timespec x_delay;
   IF_MACRO_OFF   return 0;
   IF_MACRO_DELAY {
      DEBUG_SCRP   yLOG_note    ("process a delay");
      switch (my.macro_delay) {
      case '9' : x_delay.tv_sec = 3; x_delay.tv_nsec =         0; break;
      case '8' : x_delay.tv_sec = 2; x_delay.tv_nsec =         0; break;
      case '7' : x_delay.tv_sec = 1; x_delay.tv_nsec = 750000000; break;
      case '6' : x_delay.tv_sec = 1; x_delay.tv_nsec = 500000000; break;
      case '5' : x_delay.tv_sec = 1; x_delay.tv_nsec = 250000000; break;
      case '4' : x_delay.tv_sec = 1; x_delay.tv_nsec =         0; break;
      case '3' : x_delay.tv_sec = 0; x_delay.tv_nsec = 750000000; break;
      case '2' : x_delay.tv_sec = 0; x_delay.tv_nsec = 500000000; break;
      case '1' : x_delay.tv_sec = 0; x_delay.tv_nsec = 250000000; break;
      case '0' : x_delay.tv_sec = 0; x_delay.tv_nsec =         0; break;
      default  : x_delay.tv_sec = 0; x_delay.tv_nsec =         0; break;
      }
      nanosleep (&x_delay, NULL);
   }
   return 0;
}

char
MACRO_exec_playback  (char a_key)
{
   IF_MACRO_OFF   return 0;
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_SCRP   yLOG_value   ("a_key"     , a_key);
   switch (a_key) {
   case '.'      :
      DEBUG_SCRP   yLOG_note    ("dot (.)");
      IF_MACRO_PLAYBACK {
         DEBUG_SCRP   yLOG_note    ("change playback to delay");
         SET_MACRO_DELAY;
         DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
         return -1;
      }
      IF_MACRO_DELAY {
         DEBUG_SCRP   yLOG_note    ("change delay to playback");
         SET_MACRO_PLAYBACK;
         DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
         return -1;
      }
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return -1;
      break;
   case '<'      :
      DEBUG_SCRP   yLOG_note    ("less (<)");
      if (my.macro_delay > '0')  --my.macro_delay;
      DEBUG_SCRP   yLOG_value   ("delay"     , my.macro_delay);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return -1;
      break;
   case '>'      :
      DEBUG_SCRP   yLOG_note    ("more (>)");
      if (my.macro_delay < '9')  ++my.macro_delay;
      DEBUG_SCRP   yLOG_value   ("delay"     , my.macro_delay);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return -1;
      break;
   case K_ESCAPE :
      DEBUG_SCRP   yLOG_note    ("escape");
      MACRO_reset ();
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return -1;
      break;
   case K_RETURN :
      DEBUG_SCRP   yLOG_note    ("return");
      SET_MACRO_RUN;
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return  0;
      break;
   }
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     sub-mode key handling                    ----===*/
/*====================------------------------------------====================*/
PRIV void  o___MODE____________o () { return; }

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
      DEBUG_USER   yLOG_note    ("macro recording");
      /*---(check macro name)------------*/
      if (MACRO_name_valid (a_minor) < 0)  {
         yVIKEYS_mode_exit ();
         DEBUG_USER   yLOG_note    ("bad name");
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(start recording)-------------*/
      yVIKEYS_mode_exit ();
      MACRO_record_beg  (a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for execution)------------*/
   --rce;  if (a_major == '@') {
      DEBUG_USER   yLOG_note    ("macro execution");
      /*---(playback speed)--------------*/
      if (a_minor >= '0' && a_minor <= '9') {
         MACRO_delay_set (a_minor);
         DEBUG_USER   yLOG_char    ("delay"     , a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_major;
      }
      /*---(check macro name)------------*/
      if (MACRO_name_valid (a_minor) < 0)  {
         yVIKEYS_mode_exit ();
         DEBUG_USER   yLOG_note    ("bad name");
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(execute)---------------------*/
      yVIKEYS_mode_exit ();
      MACRO_exec_beg    (a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
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
