/*============================----beg-of-source---============================*/

#include   "gyges.h"



/*====================------------------------------------====================*/
/*===----                           key handling                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___KEY_MGMT________o () { return; }
static char   wtype;
static int    wtab;
static int    wcol;
static int    wrow;
static int    wpos;
static char   wref  [20];
static char   wref2 [20];
static char   wsave [LEN_RECD];

char
KEYS_init          (void)
{
   nkeylog = 0;
   strcpy (keylog, "");
}

char
KEYS_record        (int a_curr)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [10];
   int         x_key       =0;
   x_key = '_';
   if (a_curr >= ' ' && a_curr <= '~')  x_key = a_curr;
   sprintf (t, "%c:%02x ", x_key, a_curr);
   strcat  (keylog, t);
   ++nkeylog;
   return 0;
}

char
KEYS_status        (char *a_msg)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [LEN_RECD];
   int         x_key       = 0;
   int         x_len       = 0;             /* string length                  */
   int         i           = 0;             /* iterator -- keys               */
   int         x_start     = 0;             /* iterator -- keys               */
   x_len = strlen (keylog) - 1;
   x_start = x_len - (20 * 5);
   if (x_start < 0) x_start = 0;
   snprintf (a_msg, 500, "[ keys %4d : %-100.100s ]", nkeylog, keylog + x_start);
   return 0;
}

char
KEYS_modes         (
      )
{
}

char         /*--> process basic movements ---------------[--------[--------]-*/
KEYS_basics        (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_minors    [LEN_RECD]  = "0HhlL$_KkjJGr";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   --rce;  if (a_major != ' ') {
      DEBUG_USER   yLOG_note    ("a_major is not a space");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("x_minors"  , x_minors);
   --rce;  if (strchr (x_minors, a_minor) == 0) {
      DEBUG_USER   yLOG_note    ("a_minor is not valid");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   MOVE_prep    ();
   /*---(basic horizontal)---------------*/
   switch (a_minor) {
   case '0'      : MOVE_horz ('0');       break;
   case 'H'      : MOVE_horz ('H');       break;
   case 'h'      : MOVE_horz ('h');       break;
   case 'l'      : MOVE_horz ('l');       break;
   case 'L'      : MOVE_horz ('L');       break;
   case '$'      : MOVE_horz ('$');       break;
   }
   /*---(basic vertical)-----------------*/
   switch (a_minor) {
   case '_'      : MOVE_vert ('_');       break;
   case 'K'      : MOVE_vert ('K');       break;
   case 'k'      : MOVE_vert ('k');       break;
   case 'j'      : MOVE_vert ('j');       break;
   case 'J'      : MOVE_vert ('J');       break;
   case 'G'      : MOVE_vert ('G');       break;
   }
   /*---(special)------------------------*/
   switch (a_minor) {
   case 'r'      : break;
   }
   /*---(clean-up)-----------------------*/
   MOVE_done    ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
KEYS_gz_family     (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          =   0;                /* generic return code       */
   char        x_minors    [LEN_RECD]  = "shcletkmjbazudno.ifSHLETKJBNOIFAZUD";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   --rce;  if (a_major != 'g' && a_major != 'z') {
      DEBUG_USER   yLOG_note    ("a_major is not g or z");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("x_minors"  , x_minors);
   --rce;  if (strchr (x_minors, a_minor) == 0) {
      DEBUG_USER   yLOG_note    ("a_minor is not valid");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   MOVE_prep    ();
   /*---(horizontal, simple)-------------*/
   if (strchr ("shcle", a_minor) != NULL)
      rc = MOVE_gz_horz (a_major, a_minor);
   /*---(horizontal, complex)------------*/
   switch (a_minor) {
   case 'S' : case 'A' :
      rc = MOVE_gz_horz ('g', 's');
      rc = MOVE_gz_horz ('z', 'e');
      break;
   case 'H' : case 'D' :
      rc = MOVE_gz_horz ('g', 's');
      rc = MOVE_gz_horz ('z', 'c');
      break;
   case 'L' : case 'U' :
      rc = MOVE_gz_horz ('g', 'e');
      rc = MOVE_gz_horz ('z', 'c');
      break;
   case 'E' : case 'Z' :
      rc = MOVE_gz_horz ('g', 'e');
      rc = MOVE_gz_horz ('z', 's');
      break;
   }
   /*---(vertical, simple)---------------*/
   if (strchr ("tkmjb", a_minor) != NULL)
      rc = MOVE_gz_vert (a_major, a_minor);
   /*---(vertical, complex)--------------*/
   switch (a_minor) {
   case 'T' : case 'A' :
      rc = MOVE_gz_vert ('g', 't');
      rc = MOVE_gz_vert ('z', 'b');
      break;
   case 'K' : case 'U' :
      rc = MOVE_gz_vert ('g', 't');
      rc = MOVE_gz_vert ('z', 'm');
      break;
   case 'J' : case 'D' :
      rc = MOVE_gz_vert ('g', 'b');
      rc = MOVE_gz_vert ('z', 'm');
      break;
   case 'B' : case 'Z' :
      rc = MOVE_gz_vert ('g', 'b');
      rc = MOVE_gz_vert ('z', 't');
      break;
   }
   /*---(combination)--------------------*/
   switch (a_minor) {
   case 'a' : rc = MOVE_gz_horz (a_major, 's');   /* alpha = top-left    */
              rc = MOVE_gz_vert (a_major, 't');
              break;
   case 'z' : rc = MOVE_gz_horz (a_major, 'e');   /* omega = bot-right   */
              rc = MOVE_gz_vert (a_major, 'b');
              break;
   case 'u' : rc = MOVE_gz_horz (a_major, 'e');   /* near  = top-right   */
              rc = MOVE_gz_vert (a_major, 't');
              break;
   case 'd' : rc = MOVE_gz_horz (a_major, 's');   /* far   = bot-left    */
              rc = MOVE_gz_vert (a_major, 'b');
              break;
   case '.' : rc = MOVE_gz_horz (a_major, 'c');   /* core  = cen-mid     */
              rc = MOVE_gz_vert (a_major, 'm');
              break;
   }
   /*---(frozen)-------------------------*/
   switch (a_minor) {
      /*> case '_' : /+ go to frozen header rows +/     break;                           <* 
       *> case '0' : /+ go to frozen cols        +/     break;                           <*/
   }
   /*---(clean-up)-----------------------*/
   MOVE_done    ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
KEYS_e_family      (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   /*-------------------------------------(-horz--)(-vert--)(-zoom--)(-edge--)*/
   char        x_minors    [LEN_RECD]  = "SsHhclLeETtKkmjJbBNnOo.iIfFaAzZ+uUdD";
   char        x_nothere   [LEN_RECD]  = "NnOo.iIfF";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   --rce;  if (a_major != 'e') {
      DEBUG_USER   yLOG_note    ("a_major is not e");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("x_minors"  , x_minors);
   --rce;  if (strchr (x_minors, a_minor) == 0) {
      DEBUG_USER   yLOG_note    ("a_minor is not valid");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   MOVE_prep    ();
   /*---(simple ends)--------------------*/
   switch (a_minor) {
   case 'k' : rc = MOVE_ends  (a_minor);     break;
   case 'j' : rc = MOVE_ends  (a_minor);     break;
   case 'h' : rc = MOVE_ends  (a_minor);     break;
   case 'l' : rc = MOVE_ends  (a_minor);     break;
   }
   /*---(vertical edges)-----------------*/
   switch (a_minor) {
   case 't' : rc = MOVE_edges (a_minor);     break;
   case 'K' : rc = MOVE_edges (a_minor);     break;
   case 'J' : rc = MOVE_edges (a_minor);     break;
   case 'b' : rc = MOVE_edges (a_minor);     break;
   }
   /*---(horizontal edges)---------------*/
   switch (a_minor) {
   case 's' : rc = MOVE_edges (a_minor);     break;
   case 'H' : rc = MOVE_edges (a_minor);     break;
   case 'L' : rc = MOVE_edges (a_minor);     break;
   case 'e' : rc = MOVE_edges (a_minor);     break;
   }
   /*---(combination)--------------------*/
   switch (a_minor) {
   case 'a' : rc = MOVE_edges ('s');
              rc = MOVE_edges ('t');
              break;
   case 'z' : rc = MOVE_edges ('e');
              rc = MOVE_edges ('b');
              break;
   case 'u' : rc = MOVE_edges ('e');
              rc = MOVE_edges ('t');
              break;
   case 'd' : rc = MOVE_edges ('s');
              rc = MOVE_edges ('b');
              break;
   }
   /*---(maximums)-----------------------*/
   switch (a_minor) {
   case 'S' : rc = MOVE_horz    ('0');
              break;
   case 'E' : rc = MOVE_horz    ('$');
              break;
   case 'T' : rc = MOVE_vert    ('_');
              break;
   case 'B' : rc = MOVE_vert    ('G');
              break;
   }
   /*---(max combos)---------------------*/
   switch (a_minor) {
   case 'A' : rc = MOVE_horz    ('0');            /* ALPHA = origin      */
              rc = MOVE_vert    ('_');
              break;
   case 'Z' : rc = MOVE_horz    ('$');            /* OMEGA = maximum     */
              rc = MOVE_vert    ('G');
              break;
   case 'U' : rc = MOVE_horz    ('$');            /* NEAR  = maximum     */
              rc = MOVE_vert    ('_');
              break;
   case 'D' : rc = MOVE_horz    ('0');            /* FAR   = maximum     */
              rc = MOVE_vert    ('G');
              break;
   }
   /*---(clean-up)-----------------------*/
   MOVE_done    ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
KEYS_c_family      (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   char        x_minors    [LEN_RECD]  = "hljksetbaonfAONF";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   --rce;  if (a_major != 'c') {
      DEBUG_USER   yLOG_note    ("a_major is not c");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("x_minors"  , x_minors);
   --rce;  if (strchr (x_minors, a_minor) == 0) {
      DEBUG_USER   yLOG_note    ("a_minor is not valid");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   MOVE_prep    ();
   /*---(move cursor)--------------------*/
   switch (a_minor) {
   case 's' : rc = MOVE_gz_horz ('g', 's');
              break;
   case 'e' : rc = MOVE_gz_horz ('g', 'e');
              break;
   case 'h' :
   case 'l' : rc = MOVE_gz_horz ('g', 'c');
              break;
   case 't' : rc = MOVE_gz_vert ('g', 't');
              break;
   case 'b' : rc = MOVE_gz_vert ('g', 'b');
              break;
   case 'j' :
   case 'k' : rc = MOVE_gz_vert ('g', 'm');
              break;
   case 'A' : rc = MOVE_gz_horz ('g', 's');
              rc = MOVE_gz_vert ('g', 't');
              break;
   case 'a' :
   case 'o' :
   case 'n' :
   case 'f' : rc = MOVE_gz_horz ('g', 'c');
              rc = MOVE_gz_vert ('g', 'm');
              break;
   }
   /*---(horz/vert)----------------------*/
   switch (a_minor) {
   case 's' :
   case 'h' : rc = MOVE_gz_horz ('z', 'e');
              break;
   case 'e' :
   case 'l' : rc = MOVE_gz_horz ('z', 's');
              break;
   case 'b' :
   case 'j' : rc = MOVE_gz_vert ('z', 't');
              break;
   case 't' :
   case 'k' : rc = MOVE_gz_vert ('z', 'b');
              break;
   }
   /*---(half diagonals)-----------------*/
   switch (a_minor) {
   case 'a' : rc = MOVE_gz_horz ('z', 'e');
              rc = MOVE_gz_vert ('z', 'b');
              break;
   case 'o' : rc = MOVE_gz_horz ('z', 's');
              rc = MOVE_gz_vert ('z', 't');
              break;
   case 'n' : rc = MOVE_gz_horz ('z', 's');
              rc = MOVE_gz_vert ('z', 'b');
              break;
   case 'f' : rc = MOVE_gz_horz ('z', 'e');
              rc = MOVE_gz_vert ('z', 't');
              break;
   }
   /*---(update)-------------------------*/
   MOVE_done    ();
   MOVE_prep    ();
   /*---(move cursor)--------------------*/
   switch (a_minor) {
   case 's' : case 'e' :
   case 'h' : case 'l' : rc = MOVE_gz_horz ('g', 'c');
                         break;
   case 't' : case 'b' :
   case 'j' : case 'k' : rc = MOVE_gz_vert ('g', 'm');
                         break;
   case 'a' : case 'o' :
   case 'n' : case 'f' : rc = MOVE_gz_horz ('g', 'c');
                         rc = MOVE_gz_vert ('g', 'm');
                         break;
   }
   /*---(clean-up)-----------------------*/
   MOVE_done    ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> process keystrokes in map mode --------[--------[--------]-*/
KEYS_regbasic       (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   char        x_minors    [LEN_RECD]  = "ypdx";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   --rce;  if (a_major != ' ') {
      DEBUG_USER   yLOG_note    ("a_major is not empty");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("x_minors"  , x_minors);
   --rce;  if (strchr (x_minors, a_minor) == 0) {
      DEBUG_USER   yLOG_note    ("a_minor is not valid");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(process)------------------------*/
   switch (a_minor) {
   case  'y' :
      DEBUG_USER   yLOG_note    ("call REG_copy");
      REG_copy  ();
      break;
   case  'p' :
      DEBUG_USER   yLOG_note    ("call REG_paste");
      REG_paste ('y');
      break;
   case  'd' :
   case  'x' :
      DEBUG_USER   yLOG_note    ("call REG_cut");
      REG_cut   ();
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> process keystrokes in normal mode -----[--------[--------]-*/
MODE_map           (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   char        x_minors    [LEN_RECD]  = "ypdx";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr ());
   --rce;  if (yVIKEYS_mode_not (MODE_MAP    )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(major mode changes)-------------*/
   if (a_minor == K_RETURN) {
      yVIKEYS_mode_enter  (MODE_SOURCE);
      EDIT_pos    ('0');
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   if (a_minor == K_ESCAPE) {
      VISU_clear ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(single key)---------------------*/
   --rce;
   if (a_major == ' ') {
      /*---(multikey prefixes)-----------*/
      if (strchr ("gzced"  , a_minor) != 0) {
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(mode switch)-----------------*/
      switch (a_minor) {
      case 'v'      :
         yVIKEYS_mode_enter  (MODE_VISUAL);
         VISU_start  (CTAB, CCOL, CROW, VISU_FROM);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case 'V'      :
         yVIKEYS_mode_enter  (MODE_VISUAL);
         VISU_restore ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case ':'      :
         strncpy     (g_command , ":", LEN_RECD);
         yVIKEYS_mode_enter  (MODE_COMMAND);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case 's'      :
         EDIT_start  ("");
         yVIKEYS_mode_enter  (MODE_INPUT  );
         MODE_input ('m', 'i');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'i';
         break;
      case '='      :
         EDIT_start  ("=");
         yVIKEYS_mode_enter  (MODE_INPUT  );
         MODE_input ('m', 'a');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'a';
         break;
      case '#'      :
         EDIT_start  ("#");
         yVIKEYS_mode_enter  (MODE_INPUT  );
         MODE_input ('m', 'a');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'a';
         break;
      case '+'      :
         EDIT_start  ("+");
         yVIKEYS_mode_enter  (MODE_INPUT  );
         MODE_input ('m', 'a');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'a';
         break;
      case '-'      :
         EDIT_start  ("-");
         yVIKEYS_mode_enter  (MODE_INPUT  );
         MODE_input ('m', 'a');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'a';
         break;
      }
      /*---(submodes)--------------------*/
      switch (a_minor) {
      case '\\'     :
         DEBUG_USER   yLOG_note    ("selected menu mode");
         yVIKEYS_mode_enter  (SMOD_MENUS  );
         my.menu = MENU_ROOT;
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case 'F'      : yVIKEYS_mode_enter  (SMOD_FORMAT  );
                      DEBUG_USER   yLOG_exit    (__FUNCTION__);
                      return 0;
                      break;
      case ','      : yVIKEYS_mode_enter  (SMOD_BUFFER  );
                      DEBUG_USER   yLOG_exit    (__FUNCTION__);
                      return 0;
                      break;
      case '"'      : yVIKEYS_mode_enter  (SMOD_REGISTER);
                      DEBUG_USER   yLOG_exit    (__FUNCTION__);
                      return '"';  /* make sure double quote goes in prev char */
                      break;
      case 'm'      : 
      case '\''     : yVIKEYS_mode_enter  (SMOD_MARK    );
                      DEBUG_USER   yLOG_exit    (__FUNCTION__);
                      return '\'';  /* make sure single quote goes in prev char */
                      break;
      case 'E'      :
                      yVIKEYS_mode_enter  (SMOD_ERROR   );
                      DEBUG_USER   yLOG_exit    (__FUNCTION__);
                      return SMOD_ERROR;  /* make sure mode indicator goes also       */
                      break;
      }
      /*---(normal)----------------------*/
      rc = KEYS_basics   (a_major, a_minor);
      if (rc == 0) {
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      rc = KEYS_regbasic (a_major, a_minor);
      if (rc == 0) {
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      /*---(special)------------------*/
      switch (a_minor) {
      case K_CTRL_L : clear ();                       break;
      case 'P'      : DEP_writeall (); KEYS_pcol (); KEYS_prow (); HIST_list ();  break;
                      /*---(clearing cells)-----------*/
                      /*> case 'x'      : REG_cut   ();                   break;                      <*/
                      /*> case 'd'      : CELL_erase ();                  break;                      <*/
                      /*---(formatting)---------------*/
      case '<'      : CELL_align (CHG_INPUT, '<');               break;
      case '|'      : CELL_align (CHG_INPUT, '|');               break;
      case '>'      : CELL_align (CHG_INPUT, '>');               break;
                      /*---(selection)----------------*/
                      /*> case 'v'      : VISU_start (CTAB, CCOL, CROW, VISU_FROM);   break;          <*/
                      /*> case 'V'      : VISU_start (CTAB, CCOL, CROW, VISU_CUM);    break;          <*/
                      /*> case 'y'      : REG_copy  ();                   break;                      <*/
                      /*> case 'p'      : REG_paste ('y');                break;                      <*/
                      /*---(modes and multikey)-------*/
      case '@'      : SEQ_calc_full ();               break;
                      /*> case '[' : if (escaped) { sch = ch; special = 1; } else sch = 'x'; break;   <*/
                      /*---(new stuff)----------------*/
      case 'u'      : HIST_undo ();                   break;
      case 'U'      : HIST_redo ();                   break;
                      /*> case 'W'      : REG_bufwrite (my.reg_curr);     break;                      <*/
      case '?'      : my.info_win = G_INFO_CELL;      break;
      default       : /* unknown problem */
                      DEBUG_USER   yLOG_exit    (__FUNCTION__);
                      return rce;
                      break;
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(special family)------------------------*/
   /*> --rce;                                                                         <* 
    *> if (a_major == 'c') {                                                           <* 
    *>    switch (a_minor) {                                                           <* 
    *>    case 's'      : KEYS_col ("cs");                break;                      <* 
    *>    case 'h'      : KEYS_col ("ch");                break;                      <* 
    *>    case 'l'      : KEYS_col ("cl");                break;                      <* 
    *>    case 'e'      : KEYS_col ("ce");                break;                      <* 
    *>    case 't'      : KEYS_row ("ct");                break;                      <* 
    *>    case 'b'      : KEYS_row ("cb");                break;                      <* 
    *>    default       : return rce;                     break;                      <* 
    *>    }                                                                           <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*> else if (a_major == 'x') {                                                      <* 
    *>    sch = ' ';                                                                  <* 
    *>    ch  = 24;                                                                   <* 
    *> }                                                                              <*/
   /*---(goto family)--------------------*/
   if (a_major == 'g') {
      rc = KEYS_gz_family  (a_major, a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(scroll family)------------------*/
   if (a_major == 'z') {
      rc = KEYS_gz_family  (a_major, a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(end family)---------------------*/
   if (a_major == 'e') {
      rc = KEYS_e_family   (a_major, a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(end family)---------------------*/
   if (a_major == 'c') {
      rc = KEYS_c_family   (a_major, a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(delete family)-------------------------*/
   /*> --rce;                                                                         <* 
    *> if (a_major == 'd') {                                                           <* 
    *>    switch (a_minor) {                                                           <* 
    *>    case 'd' : --NROW;          break;                                          <* 
    *>    case 'w' : --NCOL;     break;                                               <* 
    *>    default  : return rce;                         break;                       <* 
    *>    }                                                                           <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(complete)------------------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> process keystrokes in visual mode -----[--------[--------]-*/
KEYS_visual   (int a_prev, int a_curr)
{
   /*---(complete)------------------------------*/
   return 0;
}

PRIV char       /* PURPOSE : remove the current character -----------------*/
KEYS__del          (char a_key)
{
   int       i    = 0;                       /* loop iterator                 */
   /*---(check room for backspace)-------*/
   EDIT_prep ();
   if (my.npos == 0) {
      return 0;
   }
   if (a_key == 'x') {
      if (my.cpos >= my.npos)  return -2;
   }
   if (a_key == 'X') {
      --(my.cpos);
      if (my.cpos < 0) {
         my.cpos = 0;
         return -1;
      }
   }
   /*---(pull back text)-----------------*/
   for (i = my.cpos; i <= my.npos; ++i) {
      g_contents[i] = g_contents[i + 1];
   }
   EDIT_done ();
   /*---(adjust for delete)--------------*/
   /*> if (my.cpos >=  my.npos) my.cpos = my.npos - 1;                                <*/
   /*---(complete)-----------------------*/
   return 0;
}


char
BUF_switch         (int a_tab)
{
   int xtab = a_tab - '0';
   if (xtab >= 0 && xtab < NTAB) {
      CTAB = xtab;
      BCOL = 0;
      ECOL = 0;
      BROW = 0;
      EROW = 0;
      MOVE_horz ('r');
      MOVE_vert ('r');
      MOVE_horz ('0');
      MOVE_vert ('_');
      CURS_size ();
   }
   return 0;
}

char          /* PURPOSE : process keys for buffer movement ------------------*/
SMOD_buffer   (char a_major, char a_minor)
{
   /*---(design notes)-------------------*/
   /*
    *   this should imitate our RBUF capability in vimm
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (yVIKEYS_mode_not (SMOD_BUFFER))             return rce;
   /*---(check for control keys)---------*/
   BUF_switch   (a_minor);
   yVIKEYS_mode_exit  ();
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> menu sub-mode -------------------------[--------[--------]-*/
SMOD_menus         (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   static char x_append    = '-';
   static char x_saved = '\0';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr ());
   --rce;  if (yVIKEYS_mode_not (SMOD_MENUS  )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(mode changes)-------------------*/
   if (a_minor == 27) {
      DEBUG_USER   yLOG_note    ("escape, choose nothing");
      my.menu = ' ';
      yVIKEYS_mode_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(top menu)-----------------------*/
   if (a_major == ' ') {
      my.menu = a_minor;
   } else {
      DEBUG_USER   yLOG_note    ("execute option");
      if (a_major == 'F') {
         yVIKEYS_mode_enter  (SMOD_FORMAT);
         SMOD_format (' ', a_minor);
      }
      my.menu = ' ';
      yVIKEYS_mode_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return a_minor;
}



/*====================------------------------------------====================*/
/*===----                        content editing                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___CONTENT_________o () { return; }

char          /* PURPOSE : process keys for cell edit mode -------------------*/
MODE_source   (char a_major, char a_minor)
{
   /*---(design notes)-------------------*/
   /*
    *   this should imitate the vi-normal mode, but only horizontally.
    *   all formulas and contents should be relatively short or they
    *   should be broken up (formulas) or in a database (text).  so,
    *   forget wordwrap, 'g', and 'z' commands.
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_multi     [LEN_RECD]   = "cdgz";
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr());
   --rce;  if (yVIKEYS_mode_not (MODE_SOURCE)) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check for control keys)---------*/
   /*---(range corrections)--------------*/
   /*> my.npos  = strlen (g_contents);                                                <* 
    *> if (my.cpos <  0      )   my.cpos = 0;                                         <* 
    *> if (my.cpos >= my.npos)   my.cpos = my.npos - 1;                               <*/
   /*---(single letter)------------------*/
   if (a_major == ' ') {
      /*---(mode changes)----------------*/
      switch (a_minor) {
      case  10  :
         DEBUG_USER   yLOG_note    ("enter, save, and return to previous mode");
         CELL_change  (NULL, CHG_INPUT, CTAB, CCOL, CROW, g_contents);
         EDIT_pos     ('r');
         yVIKEYS_mode_exit  ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;   /* escape  */
         break;
      case  27  : case  'U' :
         DEBUG_USER   yLOG_note    ("escape, forget, and return to previous mode");
         x_curr = LOC_cell_at_curr ();
         if (x_curr != NULL && x_curr->s != NULL) {
            strncpy (g_contents, x_curr->s, LEN_RECD); 
         }
         EDIT_pos     ('r');
         yVIKEYS_mode_exit  ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case  'r' : case  'R' :
         DEBUG_USER   yLOG_note    ("rR keys for replace sub-mode");
         yVIKEYS_mode_enter   (SMOD_REPLACE);
         SMOD_replace ('m', ' ');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      case  'v' :
         DEBUG_USER   yLOG_note    ("v key for text select sub-mode");
         yVIKEYS_mode_enter   (SMOD_SELECT );
         SELC_mode    ('m', ' ');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case '"'  :
         yVIKEYS_mode_enter   (SMOD_TEXTREG );
         return a_minor;  /* make sure double quote goes in prev char */
         break;
      case 'E'  :
         yVIKEYS_mode_enter   (SMOD_ERROR   );
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return SMOD_ERROR;  /* make sure mode indicator goes also       */
         break;
      }
      /*---(multikey prefixes)-----------*/
      if (strchr (x_multi, a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("captured a multikey prefix a_minor");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(cut/copy/paste)--------------*/
      switch (a_minor) {
      case  'y' : case  'Y' :
         DEBUG_USER   yLOG_note    ("yank selection text");
         TREG_copy   ();
         break;
      case  'd' :
         DEBUG_USER   yLOG_note    ("delete selection text");
         TREG_copy   ();
         TREG_delete ();
         EDIT_done   ();
         break;
      case  'p' :
         DEBUG_USER   yLOG_note    ("paste after selection text");
         TREG_paste  ('>');
         EDIT_done   ();
         break;
      case  'P' :
         DEBUG_USER   yLOG_note    ("paste before selection text");
         TREG_paste  ('<');
         EDIT_done   ();
         break;
      }
      /*---(basic movement)--------------*/
      switch (a_minor) {
      case '0' : EDIT_pos ('0');    break;
      case 'H' : EDIT_pos ('l');    break;
      case 'h' : EDIT_pos ('-');    break;
      case 'l' : EDIT_pos ('+');    break;
      case 'L' : EDIT_pos ('m');    break;
      case '$' : EDIT_pos ('$');    break;
      }
      /*---(word movement)---------------*/
      switch (a_minor) {
      case 'w' : EDIT_pos ('w');    break;
      case 'b' : EDIT_pos ('W');    break;
      case 'e' : EDIT_pos ('e');    break;
      }
      /*---(changes)---------------------*/
      switch (a_minor) {
      case 'x' :
         TREG_copy   ();
         KEYS__del   ('x');
         break;
      case 'X' :
         TREG_copy   ();
         KEYS__del   ('X');
         break;
      case 'D' : g_contents[my.cpos] = '\0';
                 EDIT_done ();
                 break;
      case 'S' : EDIT_start  ("");
                 yVIKEYS_mode_enter  (MODE_INPUT);
                 MODE_input ('m', 'i');
                 return 'i';
                 break;
      }
      /*---(going to input)--------------*/
      switch (a_minor) {
      case 'I' : EDIT_pos   ('0');
      case 'i' : yVIKEYS_mode_enter  (MODE_INPUT);
                 MODE_input ('m', tolower (a_minor));
                 DEBUG_USER   yLOG_exit    (__FUNCTION__);
                 return tolower (a_minor);
                 break;
      case 'A' : EDIT_pos   ('$');
      case 'a' : yVIKEYS_mode_enter  (MODE_INPUT);
                 MODE_input ('m', tolower (a_minor));
                 DEBUG_USER   yLOG_exit    (__FUNCTION__);
                 return tolower (a_minor);
                 break;
                 /*> case '.' : yVIKEYS_mode_enter (SMOD_WANDER); wtype = 'c'; wtab = CTAB; wcol = tabs[CTAB].ccol; wrow = tabs[CTAB].crow; wpos = my.cpos; strcpy(wref, ""); strcpy(wref2, ""); strcpy(wsave, g_contents); break;   <* 
                  *> case ':' : yVIKEYS_mode_enter (SMOD_WANDER); wtype = 'r'; wtab = CTAB; wcol = tabs[CTAB].ccol; wrow = tabs[CTAB].crow; wpos = my.cpos; strcpy(wref, ""); strcpy(wref2, ""); strcpy(wsave, g_contents); break;   <*/
      }
      /*---(multikey)--------------------*/
   } else if (a_major == 'g') {
      switch (a_minor) {
      case 's' : EDIT_pos ('T');    break;
      case 'h' : EDIT_pos ('t');    break;
      case 'c' : EDIT_pos ('.');    break;
      case 'l' : EDIT_pos ('b');    break;
      case 'e' : EDIT_pos ('B');    break;
      }
   } else if (a_major == 'z') {
      switch (a_minor) {
      case 's' : EDIT_pos ('^');    break;
      case 'h' : EDIT_pos ('k');    break;
      case 'c' : EDIT_pos (',');    break;
      case 'l' : EDIT_pos ('j');    break;
      case 'e' : EDIT_pos ('v');    break;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> replace sub-mode ----------------------[--------[--------]-*/
SMOD_replace  (char a_major, char a_minor)
{  /*---(design notes)-------------------*/
   /*
    *   very limited sub-mode of SOURCE mode.  allows a user to "type over"
    *   existing text including past the end of the line.  there are no
    *   special keys except ESCAPE to return to SOURCE mode.
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_majors    [LEN_RECD]  = "rRm";
   static char x_append    = '-';
   static char x_saved = '\0';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr ());
   --rce;  if (yVIKEYS_mode_not (SMOD_REPLACE)) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("x_majors"   , x_majors);
   --rce;  if (strchr (x_majors, a_major) == 0) {
      DEBUG_USER   yLOG_note    ("a_major is not valid");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   EDIT_prep   ();
   /*---(mode changes)-------------------*/
   if (a_minor == 27 || a_minor == 10) {
      DEBUG_USER   yLOG_note    ("escape/return, return to source mode");
      if (x_append == 'y') {
         g_contents [my.cpos] = '\0';
      }
      if (x_saved != '\0') {
         g_contents [my.cpos] = x_saved;
      }
      x_append = '-';
      EDIT_done   ();
      yVIKEYS_mode_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(handle keys)--------------------*/
   DEBUG_USER   yLOG_value   ("curr pos"  , my.cpos);
   DEBUG_USER   yLOG_char    ("curr char" , g_contents [my.cpos]);
   /*---(check for backspace)------------*/
   if (a_major == 'R' && (a_minor == 8 || a_minor == 127)) {
      DEBUG_USER   yLOG_note    ("handle a backspace/delete");
      if (my.cpos > 0) {
         g_contents [my.cpos] = x_saved;
         --(my.cpos);
         x_saved = g_contents [my.cpos];
         g_contents [my.cpos] = G_CHAR_PLACE;
      }
      EDIT_done   ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   /*---(filter crazy chars)-------------*/
   if (a_minor < 32 || a_minor >= 127) {
      DEBUG_USER   yLOG_note    ("unacceptable character");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   /*---(check for mark)-----------------*/
   if (a_major == 'm') {
      DEBUG_USER   yLOG_note    ("mark replacement position and save existing");
      x_saved = g_contents [my.cpos];
      g_contents [my.cpos] = G_CHAR_PLACE;
      EDIT_done   ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(handle normal chars)------------*/
   if (a_major == 'r') {
      DEBUG_USER   yLOG_note    ("replace the marked character");
      g_contents [my.cpos] = a_minor;
      DEBUG_USER   yLOG_char    ("new  char" , g_contents [my.cpos]);
      yVIKEYS_mode_exit ();
      EDIT_done   ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_major == 'R') {
      DEBUG_USER   yLOG_note    ("replace the marked character");
      g_contents [my.cpos] = a_minor;
      DEBUG_USER   yLOG_char    ("new  char" , g_contents [my.cpos]);
      ++(my.cpos);
      x_saved = g_contents [my.cpos];
      g_contents [my.cpos] = G_CHAR_PLACE;
   }
   /*---(correct current position)-------*/
   DEBUG_USER   yLOG_value   ("curr pos"  , my.cpos);
   DEBUG_USER   yLOG_value   ("curr end"  , my.npos);
   if (my.cpos  >= my.npos) {
      DEBUG_USER   yLOG_note    ("update the end pos");
      g_contents [my.npos    ] = G_CHAR_PLACE;
      g_contents [my.npos + 1] = '\0';
      x_append = 'y';
   }
   /*---(wrap up)------------------------*/
   EDIT_done   ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return a_major;
}

char         /*--> review cell errors --------------------[--------[--------]-*/
SMOD_error         (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   static char x_append    = '-';
   static char x_saved = '\0';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr ());
   /*> --rce;  if (yVIKEYS_mode_not (SMOD_ERROR)) {                                           <* 
    *>    DEBUG_USER   yLOG_note    ("not the correct mode");                         <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <*/
   /*---(mode changes)-------------------*/
   /*> if (a_minor == 27 || a_minor == 10) {                                          <* 
    *>    DEBUG_USER   yLOG_note    ("escape/return, return to previous mode");       <* 
    *>    yVIKEYS_mode_exit ();                                                             <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   if (a_minor == '?') {
      DEBUG_USER   yLOG_note    ("display errors for cell");
      my.info_win = G_INFO_ERROR;
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   /*---(complete)-----------------------*/
   yVIKEYS_mode_exit ();

   DEBUG_USER   yLOG_exit    (__FUNCTION__);

   return 0;

   return a_major;
}

char               /* PURPOSE : process keys for input mode ------------------*/
MODE_input         (char  a_major, char  a_minor)
{
   /*---(design notes)-------------------*/
   /*
    *   this should imitate a very basic vi-input mode by handling
    *   all characters, ignoring new line, and popping out with escape
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_majors    [LEN_RECD]   = "IiaAm";
   int         i           = 0;             /* loop iterator                  */
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr());
   --rce;  if (yVIKEYS_mode_not (MODE_INPUT )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("x_majors"   , x_majors);
   --rce;  if (strchr (x_majors, a_major) == 0) {
      DEBUG_USER   yLOG_note    ("a_major is not valid");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   EDIT_prep   ();
   /*---(check for initial mark)---------*/
   if (a_major == 'm') {
      DEBUG_USER   yLOG_note    ("mark replacement position and save existing");
      if (a_minor == 'a' && g_contents [my.cpos] != '\0')  ++(my.cpos);
      DEBUG_USER   yLOG_value   ("total pos" , my.npos);
      DEBUG_USER   yLOG_value   ("new pos"   , my.cpos);
      for (i = my.npos; i >= my.cpos; --i)  g_contents[i + 1] = g_contents[i];
      g_contents [my.cpos] = G_CHAR_PLACE;
      EDIT_done   ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_minor;
   }
   /*---(mode changes)-------------------*/
   if (a_minor == 27 || a_minor == 10) {
      DEBUG_USER   yLOG_note    ("escape/return, return to source mode");
      for (i = my.cpos; i <= my.npos; ++i)  g_contents[i] = g_contents[i + 1];
      if (a_major == 'a')  --(my.cpos);
      EDIT_done   ();
      if (a_minor == 10 && yVIKEYS_mode_prev() == MODE_MAP) {
         CELL_change  (NULL, CHG_INPUT, CTAB, CCOL, CROW, g_contents);
      }
      if (a_minor == 27 && yVIKEYS_mode_prev() == MODE_MAP) {
         x_curr = LOC_cell_at_curr ();
         if (x_curr != NULL && x_curr->s != NULL) {
            strlcpy (g_contents, x_curr->s, LEN_RECD); 
         } else {
            strlcpy (g_contents, ""                            , LEN_RECD); 
         }
      }
      EDIT_pos  ('r');
      yVIKEYS_mode_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for backspace)------------*/
   if (a_minor == 8 || a_minor == 127) {
      DEBUG_USER   yLOG_note    ("handle a backspace/delete");
      if (my.cpos > 0) {
         --(my.cpos);
         DEBUG_USER   yLOG_value   ("curr pos"  , my.cpos);
         for (i = my.cpos; i <= my.npos; ++i)  g_contents[i] = g_contents[i + 1];
         EDIT_done   ();
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   /*---(handle new character)-----------*/
   DEBUG_USER   yLOG_note    ("move remaining chars to the right");
   for (i = my.npos; i >= my.cpos; --i)  g_contents[i + 1] = g_contents[i];
   DEBUG_USER   yLOG_note    ("add the character");
   g_contents [my.cpos] = a_minor;
   ++(my.cpos);
   /*---(wrap up)------------------------*/
   EDIT_done   ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return a_major;
}

char       /*----: process keys for god mode ---------------------------------*/
MODE_god           (char a_prev, char a_curr)
{
   return 0;
}

char       /*----: process keys for formatting sub-mode ----------------------*/
SMOD_format        (char a_major, char a_minor)
{
   /*---(check for control keys)---------*/
   switch (a_minor) {
   case   10 : case   27 :
      yVIKEYS_mode_exit ();
      return 0;   /* escape  */
   }
   /*---(column widths)------------------*/
   switch (a_minor) {
   case  'm' : CELL_width    (CHG_INPUT, 'm');  break;  /* smallest    */
   case  'h' : CELL_width    (CHG_INPUT, 'h');  break;  /* less by 1   */
   case  'H' : CELL_width    (CHG_INPUT, 'H');  break;  /* less by 5   */
   case  'l' : CELL_width    (CHG_INPUT, 'l');  break;  /* more by 1   */
   case  'L' : CELL_width    (CHG_INPUT, 'L');  break;  /* more by 5   */
   case  'n' : CELL_width    (CHG_INPUT, 'n');  break;  /* normal      */
   case  'N' : CELL_width    (CHG_INPUT, 'N');  break;  /* normal-plus */
   case  'w' : CELL_width    (CHG_INPUT, 'w');  break;  /* wide        */
   case  'W' : CELL_width    (CHG_INPUT, 'W');  break;  /* largest     */
   }
   /*---(decimal places)-----------------*/
   switch (a_minor) {
   case  '0' : CELL_decimals (CHG_INPUT, '0');  break;  /* none        */
   case  '1' : CELL_decimals (CHG_INPUT, '1');  break;  /* less by 1   */
   case  '2' : CELL_decimals (CHG_INPUT, '2');  break;  /* exactly 2   */
   case  '3' : CELL_decimals (CHG_INPUT, '3');  break;  /* exactly 3   */
   case  '4' : CELL_decimals (CHG_INPUT, '4');  break;  /* exactly 4   */
   case  '5' : CELL_decimals (CHG_INPUT, '5');  break;  /* exactly 5   */
   case  '6' : CELL_decimals (CHG_INPUT, '6');  break;  /* exactly 6   */
   case  '7' : CELL_decimals (CHG_INPUT, '7');  break;  /* exactly 7   */
   case  '8' : CELL_decimals (CHG_INPUT, '8');  break;  /* exactly 8   */
   case  '9' : CELL_decimals (CHG_INPUT, '9');  break;  /* exactly 9   */
   }
   /*---(horizontal alignments)----------*/
   switch (a_minor) {
   case  '<' : CELL_align    (CHG_INPUT, '<');  break;  /* left        */
   case  '|' : CELL_align    (CHG_INPUT, '|');  break;  /* center      */
   case  '>' : CELL_align    (CHG_INPUT, '>');  break;  /* right       */
   case  '[' : CELL_align    (CHG_INPUT, '[');  break;  /* left dec    */
   case  '^' : CELL_align    (CHG_INPUT, '^');  break;  /* center dec  */
   case  ']' : CELL_align    (CHG_INPUT, ']');  break;  /* right dec   */
   case  '}' : CELL_align    (CHG_INPUT, '}');  break;  /* right +2    */
   case  '{' : CELL_align    (CHG_INPUT, '{');  break;  /* left  +2    */
   }
   /*---(merging)------------------------*/
   switch (a_minor) {
   case  'M' : CELL_merge_visu ();              break;  /* merge       */
   case  'U' : CELL_unmerge_visu ();            break;  /* unmerge     */
   }
   /*---(integer formats)----------------*/
   switch (a_minor) {
   case  'i' : CELL_format   (CHG_INPUT, 'i');  break;  /* integer         */
   case  'f' : CELL_format   (CHG_INPUT, 'f');  CELL_decimals (CHG_INPUT, '3'); break;  /* real/float      */
   case  'g' : CELL_format   (CHG_INPUT, 'f');  CELL_decimals (CHG_INPUT, '6'); break;  /* real/float      */
   case  'e' : CELL_format   (CHG_INPUT, 'e');  CELL_decimals (CHG_INPUT, '3'); break;  /* exponencial     */
   case  'E' : CELL_format   (CHG_INPUT, 'E');  CELL_decimals (CHG_INPUT, '3'); break;  /* spaced exponent */
   }
   /*---(comma formats)------------------*/
   switch (a_minor) {
   case  'c' : CELL_format   (CHG_INPUT, 'c');  break;  /* comma           */
   case  'C' : CELL_format   (CHG_INPUT, 'C');  break;  /* comma           */
   case  'a' : CELL_format   (CHG_INPUT, 'a');  break;  /* accounting      */
   case  'A' : CELL_format   (CHG_INPUT, 'A');  break;  /* accounting      */
   case  '$' : CELL_format   (CHG_INPUT, '$');  break;  /* dollar          */
   case  's' : CELL_format   (CHG_INPUT, 's');  break;  /* signed          */
   case  'S' : CELL_format   (CHG_INPUT, 'S');  break;  /* signed          */
   case  '#' : CELL_format   (CHG_INPUT, '#');  CELL_decimals (CHG_INPUT, '6'); break;  /* technical       */
   case  'p' : CELL_format   (CHG_INPUT, 'p');  break;  /* bullet point    */
   case  'P' : CELL_format   (CHG_INPUT, 'P');  break;  /* bullet point    */
   }
   /*---(roman numerals)-----------------*/
   switch (a_minor) {
   case  'r' : CELL_format   (CHG_INPUT, 'r');  break;  /* roman           */
   case  'R' : CELL_format   (CHG_INPUT, 'R');  break;  /* roman upper     */
   }
   /*---(special formats)----------------*/
   switch (a_minor) {
   case  'o' : CELL_format   (CHG_INPUT, 'o');  break;  /* octal           */
   case  'O' : CELL_format   (CHG_INPUT, 'O');  break;  /* spaced octal    */
   case  'x' : CELL_format   (CHG_INPUT, 'x');  break;  /* hex             */
   case  'X' : CELL_format   (CHG_INPUT, 'X');  break;  /* spaced hex      */
   case  'b' : CELL_format   (CHG_INPUT, 'b');  break;  /* binary          */
   case  'B' : CELL_format   (CHG_INPUT, 'B');  break;  /* spaced binary   */
   case  'z' : CELL_format   (CHG_INPUT, 'z');  break;  /* base-62         */
   case  'Z' : CELL_format   (CHG_INPUT, 'Z');  break;  /* spaced base-62  */
   }
   /*---(time and date)------------------*/
   switch (a_minor) {
   case  't' : CELL_format   (CHG_INPUT, 't');  break;  /* time            */
   case  'T' : CELL_format   (CHG_INPUT, 'T');  break;  /* timestamp       */
   case  'd' : CELL_format   (CHG_INPUT, 'd');  break;  /* date            */
   case  'D' : CELL_format   (CHG_INPUT, 'D');  break;  /* full date       */
   }
   /*---(string fills)-------------------*/
   switch (a_minor) {
   case  '-' : CELL_format   (CHG_INPUT, '-');  break;  /* filled dash     */
   case  '=' : CELL_format   (CHG_INPUT, '=');  break;  /* filled equal    */
   case  '_' : CELL_format   (CHG_INPUT, '_');  break;  /* filled under    */
   case  '.' : CELL_format   (CHG_INPUT, '.');  break;  /* filled period   */
   case  '+' : CELL_format   (CHG_INPUT, '+');  break;  /* filled plus     */
   case  '/' : CELL_format   (CHG_INPUT, '/');  break;  /* filled divs     */
   case  '"' : CELL_format   (CHG_INPUT, '"');  break;  /* filled euro quot*/
   case  '!' : CELL_format   (CHG_INPUT, ' ');  break;  /* filled empty    */
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
KEYS_unlock        (void)
{
   FR_COL  = '-';
   FR_BCOL = 0;
   FR_ECOL = 0;
   MOVE_horz ('r');
   FR_ROW  = '-';
   FR_BROW = 0;
   FR_EROW = 0;
   MOVE_vert ('r');
}

char
cmd_exec           (char *a_command)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *p           = NULL;
   char       *q           = " ";
   char        x_work      [LEN_RECD]   = "";
   int         x_len       = 0;
   char        x_flag      = '-';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_point   ("a_command" , a_command);
   --rce;  if (a_command == NULL) {
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("a_command" , a_command);
   strncpy    (x_work, a_command, LEN_RECD);
   x_len = strlen (x_work);
   p = strtok (x_work, q);
   DEBUG_USER   yLOG_point   ("p"         , p);
   --rce;  if (p == NULL)  {
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("p"         , p);
   /*---(file commands)------------------*/
   if   (strncmp(p, ":w"        , LEN_RECD) == 0 ||
         strncmp(p, ":wa"       , LEN_RECD) == 0 ||
         strncmp(p, ":wq"       , LEN_RECD) == 0 ||
         strncmp(p, ":wqa"      , LEN_RECD) == 0 ||
         strncmp(p, ":write"    , LEN_RECD) == 0) {
      FILE_write ();
      if (p [2] == 'q')  done = 0;
      return 0;
   }
   if (x_len >=  7 && strcmp (p, ":file") == 0) {
      DEBUG_USER   yLOG_note    ("rename the file");
      rc = FILE_rename (p + 6);
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   if (x_len >=  10 && strcmp (p, ":resize") == 0) {
      DEBUG_USER   yLOG_note    ("resize a tab");
      rc = LOC_tab_resize (p + 8);
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      CURS_screen_reset ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   if (strlen (p) == 4 && strcmp (p, ":ver") == 0) {
      ver_ctrl = 'y';
      FILE_version (NULL, ver_num);
      return 0;
   }
   if (strlen (p) == 5 && strncmp (p, ":ver", 4) == 0) {
      rc = FILE_bump (p[4], ver_num);
      if (rc >= 0) {
         if (x_len >= 7)  strcpy (ver_txt, p + 6);
         else             strcpy (ver_txt, "----------");
         return 0;
      }
      if (p[4] == 't') {
         if (x_len >= 7)  strcpy (ver_txt, p + 6);
         else             strcpy (ver_txt, "----------");
         return 0;
      }
      return -1;
   }
   if (strlen (p) == 7 && strcmp (p, ":errors") == 0) {
      ERROR_list ();
      return 0;
   }
   /*---(layout commands)----------------*/
   if (x_len >= 12 && strcmp (p, ":formula") == 0) {
      PROG_layout_set ("cmd", "formula"  , x_work + 9);
      return 0;
   }
   if (x_len >= 11 && strcmp (p, ":status") == 0) {
      PROG_layout_set ("cmd", "status"   , x_work + 8);
      return 0;
   }
   if (x_len >= 12 && strcmp (p, ":command") == 0) {
      PROG_layout_set ("cmd", "command"  , x_work + 9);
      return 0;
   }
   if (x_len >= 11 && strcmp (p, ":layout" ) == 0) {
      PROG_layout_set ("cmd", "layout"   , x_work + 8);
      return 0;
   }
   if   (strncmp(p, ":q"        , LEN_RECD) == 0 ||
         strncmp(p, ":qa"       , LEN_RECD) == 0) {
      done = 0;
      return 0;
   }
   if (strcmp (p, ":lock")  == 0 ||
         strcmp (p, ":lockr") == 0 ||
         strcmp (p, ":lockc") == 0) {
      x_flag = '-';
      if (strlen (p) == 6)  x_flag = p[5];
      KEYS_unlock ();
      p = strtok (NULL  , q);
      if (p == NULL) {
         KEYS_unlock ();
         return 0;
      }
      rc = LOC_parse (p, NULL, &FR_BCOL, &FR_BROW, NULL);
      if (rc < 0) {
         KEYS_unlock ();
         return 0;
      }
      FR_ECOL = FR_BCOL;
      FR_EROW = FR_BROW;
      p = strtok (NULL  , q);
      if (p != NULL) {
         rc = LOC_parse (p, NULL, &FR_ECOL, &FR_EROW, NULL);
         if (rc < 0) {
            FR_ECOL = FR_BCOL;
            FR_EROW = FR_BROW;
         }
      }
      switch (x_flag) {
      case  'c' :
         FR_COL  = 'y';
         BCOL = CCOL = FR_ECOL + 1;
         BROW = CROW = FR_EROW;
         FR_BROW = 0;
         FR_EROW = 0;
         break;
      case  'r' :
         FR_ROW  = 'y';
         BCOL = CCOL = FR_ECOL;
         BROW = CROW = FR_EROW + 1;
         FR_BCOL = 0;
         FR_ECOL = 0;
         break;
      case  '-' :
         FR_COL  = 'y';
         FR_ROW  = 'y';
         BCOL = CCOL = FR_ECOL + 1;
         BROW = CROW = FR_EROW + 1;
         break;
      default   :
         KEYS_unlock ();
         break;
      }
      MOVE_horz ('r');
      MOVE_vert ('r');
      return 0;
   }
   if   (strncmp(p, ":!"        , 2      ) == 0) {
      system (a_command + 2);
      return 0;
   }
   sta_error = 'y';
   /*> if        (strncmp(p, "o"        , LEN_RECD) == 0) {                            <* 
    *>    p = strtok(NULL, q);                                                        <* 
    *>    if (p == NULL) return 0;                                                    <* 
    *>    INPT_main ();                                                              <* 
    *> } else if (strncmp(p, "q"        , LEN_RECD) == 0) {                            <* 
    *>    done = 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(complete)-----------------------*/
   return 0;
}

char       /*----: process keys for input/append mode ------------------------*/
MODE_command       (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   int         x_len       = 0;
   char        x_temp      [11]        = "";
   /*---(check for control keys)---------*/
   x_len = strlen (g_command);
   switch (a_minor) {
   case   27 : yVIKEYS_mode_exit ();
               return 0;
   case   10 : cmd_exec (g_command);
               yVIKEYS_mode_exit ();
               return 0;   /* return  */
   }
   /*---(check for backspace)------------*/
   if (a_minor == 8 || a_minor == 127) {
      --x_len;
      if (x_len < 0)   x_len = 0;
      g_command [x_len] = '\0';
      return 0;
   }
   /*---(normal characters)--------------*/
   snprintf (x_temp, 10, "%c", a_minor);
   strcat   (g_command, x_temp);
   /*---(complete)-----------------------*/
   return 0;
}


char       /*----: process keys for wander mode ------------------------------*/
SMOD_wander        (char a_prev, char a_curr)
{
   /*---(design notes)-------------------*/
   /*
    *   this should allow keyboard selection of cells and ranges for formulas
    *   much like excel does
    */
   char    post = ' ';
   char    rce  = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (yVIKEYS_mode_not (SMOD_WANDER )) {
      return rce;
   }
   /*---(check for control keys)---------*/
   switch (a_curr) {
   case  ',' :
   case  ')' : post = a_curr;
   case  10  :
   case  27  : VISU_clear ();
               LOC_ref (CTAB, CCOL, CROW, 0, wref);
               CTAB = wtab;
               CCOL = wcol;
               CROW = wrow;
               my.cpos = wpos;
               strcpy (g_contents, wsave);
               if (strcmp (wref2, "") != 0) {
                  strcat (g_contents, wref2);
                  strcat (g_contents, ":");
               }
               strcat (g_contents, wref);
               my.npos = strlen(g_contents);
               if (post != ' ') {
                  g_contents[my.npos]   = post;
                  g_contents[++my.npos] = '\0';
               }
               my.cpos = my.npos;
               yVIKEYS_mode_exit ();
               return  0;   /* escape -- back to source mode */
   }
   /*---(basic movement)-----------*/
   /*> switch (a_curr) {                                                              <* 
    *> case '_'      : KEYS_row(" _");    break;                                      <* 
    *> case 'K'      : KEYS_row(" K");    break;                                      <* 
    *> case 'k'      : KEYS_row(" k");    break;                                      <* 
    *> case 'j'      : KEYS_row(" j");    break;                                      <* 
    *> case 'J'      : KEYS_row(" J");    break;                                      <* 
    *> case 'G'      : KEYS_row(" G");    break;                                      <* 
    *> case '{'      : KEYS_row(" {");    break;                                      <* 
    *> case '}'      : KEYS_row(" }");    break;                                      <* 
    *> case K_CTRL_B : KEYS_row("^b"); clear(); break;                                <* 
    *> case K_CTRL_F : KEYS_row("^f"); clear(); break;                                <* 
    *> case '0'      : KEYS_col (" 0");     break;                                    <* 
    *> case 'H'      : KEYS_col (" H");     break;                                    <* 
    *> case 'h'      : KEYS_col (" h");     break;                                    <* 
    *> case 'l'      : KEYS_col (" l");     break;                                    <* 
    *> case 'L'      : KEYS_col (" L");     break;                                    <* 
    *> case '$'      : KEYS_col (" $");     break;                                    <* 
    *> case 'b'      : KEYS_col ("es");     break;                                    <* 
    *> case 'e'      : KEYS_col ("ee");     break;                                    <* 
    *> case 'c'      : VISU_col();          break;                                    <* 
    *> case 'r'      : VISU_row();          break;                                    <* 
    *> }                                                                              <*/
   if (a_curr == ':') {
      LOC_ref    (CTAB, CCOL, CROW, 0, wref2);
      VISU_start (CTAB, CCOL, CROW, VISU_FROM);
   }
   /*---(complete)-----------------------*/
   return  0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*            /* unit test accessor -------------------[ leaf   [ 210y1x ]-*/
KEYS__unit         (char *a_question)
{
   /*---(locals)-----------+-----------+-*/
   char        x_list      [LEN_RECD];
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "keys             : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "mode_stack"   )  == 0) {
      yVIKEYS_mode_list (x_list);
      snprintf (unit_answer, LEN_UNIT, "keys mode stack  : %s", x_list);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}


/*============================----end-of-source---============================*/
