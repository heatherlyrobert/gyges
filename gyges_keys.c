/*============================----beg-of-source---============================*/

#include   "gyges.h"


#define     MAX_MODES       30
struct cMODE_INFO {
   char        abbr;                   /* single character abbreviation       */
   char        major;                  /* major mode (y/n)                    */
   char        show;                   /* show a message line (y/n)           */
   char        three       [ 5];       /* very short name                     */
   char        terse       [10];       /* short name                          */
   char        desc        [50];       /* description of mode                 */
   char        allow       [25];       /* allowed mode transitions            */
   int         count;                  /* number of times used                */
   char        mesg        [MAX_STR];  /* informative message for display     */
} g_mode_info [MAX_MODES] = {
   /*-a-- -maj show --tla- ---terse----- ---description---------------------------------------- -----------------------,----- 123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789- */
   /*---(major modes)--------------------*/
   { 'G' , 'y', 'y', "GOD", "god"       , "god-mode allowing 3D omnicient viewing"             , ""                    ,    0, ""                                                                                        },
   { 'M' , 'y', 'y', "MAP", "map"       , "map-mode providing 2D review of object collections" , "GVSI:/\"b\'$oe\\"    ,    0, "horz(a)=0HhlL$  horz(g/z)=sh,le  vert(a)=_KkjJG  vert(g/z)=tk.jb  modes=vIFV:{ret}"      },
   { 'V' , 'y', 'y', "VIS", "visual"    , "visual selection of objects for collection action"  , "\""                  ,    0, "0HhlL$_KkjJG  gz=sh,letk.jb  dxy  !: ~uU /nN oO sS"                                      },
   { 'S' , 'y', 'y', "SRC", "source"    , "linewise review of textual content"                 , "Isrte"               ,    0, "hor=0HhlL$bBeEwW  g/z=sh,le  sel=vV\"  pul=yYdDxX  put=pP  chg=rRiIaA  fnd=fnN"          },
   { 'I' , 'y', 'y', "INP", "input"     , "linewise creation and editing of textual content"   , ""                    ,    0, ""                                                                                        },
   { ':' , 'y', '-', "CMD", "command"   , "command line capability for advanced actions"       , ""                    ,    0, ""                                                                                        },
   /*---(sub-modes)----------------------*/
   { 'e' , '-', 'y', "err", "errors"    , "display and action errors"                          , ""                    ,    0, ""                                                                                        },
   { 's' , '-', 'y', "sel", "select"    , "visual selection within text content"               , "t"                   ,    0, "0HhlL$"                                                                                  },
   { 'r' , '-', 'y', "rep", "replace"   , "linewise overtyping of content in source mode"      , ""                    ,    0, "type over character marked with special marker"                                          },
   { '"' , '-', 'y', "reg", "register"  , "selecting specific registers for data movement"     , ""                    ,    0, "regs=\"a-zA-Z-+0  pull=yYxXdD  -/+=vVcCtTsSfF  push=pPrRmMaAiIoObB  mtce=#?!g"           },
   { 't' , '-', 'y', "trg", "text reg"  , "selecting specific registers for text movement"     , ""                    ,    0, "regs=\"a-zA-Z-+0  pull=yYxXdD  -/+=vVcCtTsSfF  push=pPrRmMaAiIoObB  mtce=#?!g"           },
   { ',' , '-', 'y', "buf", "buffer"    , "moving and selecting between buffers and windows"   , ""                    ,    0, "select=0...9  modes={ret}(esc}"                                                          },
   { '@' , '-', 'y', "wdr", "wander"    , "formula creation by moving to target cells"         , ""                    ,    0, "modes={ret}{esc}"                                                                        },
   { '$' , '-', 'y', "frm", "format"    , "content formatting options"                         , ""                    ,    0, "ali=<|>[^] num=irg,as$%%p tec=#eExXbBoO tim=tdT dec=0-9 str= _-=.+"                      },
   { 'o' , '-', 'y', "obj", "object"    , "object formatting and sizing options"               , ""                    ,    0, ""                                                                                        },
   { '\'', '-', 'y', "mrk", "mark"      , "object and location marking"                        , ""                    ,    0, "set=a-zA-Z()  del=#*  hlp=?!@_  go='a-zA-Z()[<>]"                                        },
   { '\\', '-', 'y', "mnu", "menus"     , "menu system"                                        , ""                    ,    0, ""                                                                                        },
   /*---(done)---------------------------*/
   { '-' , '-', 'y', "bad", "bad mode"  , "default message when mode is not understood"        , ""                    ,    0, "mode not understood"                                                                     },
};

static      char        s_majors       [MAX_MODES] = "";



/*====================------------------------------------====================*/
/*===----                      mode stack handling                     ----===*/
/*====================------------------------------------====================*/
PRIV void  o___MODE_STACK______o () { return; }
#define        MAX_STACK   100
static char    s_modes    [MAX_STACK];      /* gyges mode stack               */
static int     s_nmode;                     /* depth of gyges mode stack      */
static char    s_cmode;                     /* current gyges mode             */

char
MODE_init          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        t           [5]         = "";
   /*---(check modes)--------------------*/
   for (i = 0; i < MAX_MODES; ++i) {
      if (g_mode_info [i].major != 'y')    continue;
      sprintf (t, "%c", g_mode_info [i].abbr);
      strlcat (s_majors, t, MAX_MODES);
   }
   /*---(validate mode)------------------*/
   for (i = 0; i < MAX_STACK; ++i) {
      s_modes [i] = '-';
   }
   s_nmode =  0;
   s_cmode = '-';
   /*---(complete)-----------------------*/
   return 0;
}

char
MODE_enter         (char a_mode)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         i           = 0;
   char        x_mode      = '-';
   int         x_index     = -1;
   /*---(check for dup)------------------*/
   /*> if (s_modes [s_nmode] == a_mode)  return 1;                            <*/
   /*---(validate mode)------------------*/
   for (i = 0; i < MAX_MODES; ++i) {
      if (g_mode_info[i].abbr == '-'    )  break;
      if (g_mode_info[i].abbr != a_mode )  continue;
      ++g_mode_info [i].count;
      x_mode  = a_mode;
   }
   --rce;  if (x_mode  == '-')  return rce;
   /*---(check if allowed)---------------*/
   if (s_nmode > 0)  {
      for (i = 0; i < MAX_MODES; ++i) {
         if (g_mode_info[i].abbr == '-'    )  break;
         if (g_mode_info[i].abbr != s_cmode)  continue;
         x_index = i;
      }
      --rce;  if (x_index <   0 )  return rce;
      --rce;  if (strchr (g_mode_info[x_index].allow, a_mode) == NULL)  return rce;
   }
   /*---(add mode)-----------------------*/
   --rce;  if (s_nmode >= MAX_STACK)   return rce;
   s_modes [s_nmode] = a_mode;
   ++s_nmode;
   /*---(set global mode)----------------*/
   s_cmode = a_mode;
   /*---(complete)-----------------------*/
   return 0;
}

char
MODE_return        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_mode      = '-';
   /*---(check stack)--------------------*/
   --rce;  if (s_nmode <= 0)  return rce;
   --s_nmode;
   s_modes [s_nmode] = x_mode;
   x_mode = s_modes [s_nmode - 1];
   /*---(set global mode)----------------*/
   s_cmode = x_mode;
   /*---(complete)-----------------------*/
   return 0;
}

char
MODE_curr          (void)
{
   return s_cmode;
}

char
MODE_prev          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_mode      = '-';
   /*---(check stack)--------------------*/
   --rce;  if (s_nmode <= 1)            return rce;
   /*---(grab previous)------------------*/
   x_mode = s_modes [s_nmode - 2];
   if (strchr (s_majors, x_mode) != NULL)   return x_mode;
   /*---(go back one more)---------------*/
   --rce;  if (s_nmode <= 2)            return rce;
   x_mode = s_modes [s_nmode - 3];
   /*---(complete)-----------------------*/
   return x_mode;
}

char
MODE_not           (char a_mode)
{
   if (a_mode != s_modes [s_nmode - 1]) return -1;
   return 0;
}

char       /*----: list the current mode stack -------------------------------*/
MODE_list          (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         i           =   0;
   char        t           [10];
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   /*---(walk the list)------------------*/
   sprintf (a_list, "modes (%d)", s_nmode);
   for (i = 0; i < 8; ++i) {
      sprintf (t, " %c", s_modes [i]);
      strlcat (a_list, t, MAX_STR);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
MODE_message       (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_major     = ' ';
   char        x_minor     = ' ';
   for (i = 0; i < MAX_MODES; ++i) {
      if (g_mode_info[i].abbr == '-'   )  break;
      if (g_mode_info[i].abbr == s_cmode)  break;
   }
   if (g_mode_info [i].major == 'y')  {
      x_major = s_cmode;
   } else {
      x_major = MODE_prev ();
      x_minor = s_cmode;
   }
   if (g_mode_info[i].show == 'y') {
      sprintf (my.message, "[%c%c] %-3.3s : %s\n", x_major, x_minor, g_mode_info[i].three, g_mode_info[i].mesg);
   } else {
      sprintf (my.message, "%s\n", command);
   }
   return 0;
}



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
static char   wsave [MAX_STR];

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
   char        t           [MAX_STR];
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
   char        x_minors    [MAX_STR]  = "0HhlL$_KkjJGr";
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
   char        x_minors    [MAX_STR]  = "shcletkmjbaonf.AONF";
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
   /*---(horizontal)---------------------*/
   switch (a_minor) {
   case 's' : rc = MOVE_gz_horz (a_major, a_minor);   break;
   case 'h' : rc = MOVE_gz_horz (a_major, a_minor);   break;
   case 'c' : rc = MOVE_gz_horz (a_major, a_minor);   break;
   case 'l' : rc = MOVE_gz_horz (a_major, a_minor);   break;
   case 'e' : rc = MOVE_gz_horz (a_major, a_minor);   break;
   }
   /*---(vertical)-----------------------*/
   switch (a_minor) {
   case 't' : rc = MOVE_gz_vert (a_major, a_minor);   break;
   case 'k' : rc = MOVE_gz_vert (a_major, a_minor);   break;
   case 'm' : rc = MOVE_gz_vert (a_major, a_minor);   break;
   case 'j' : rc = MOVE_gz_vert (a_major, a_minor);   break;
   case 'b' : rc = MOVE_gz_vert (a_major, a_minor);   break;
   }
   /*---(combination)--------------------*/
   switch (a_minor) {
   case 'a' : rc = MOVE_gz_horz (a_major, 's');   /* alpha = top-left    */
              rc = MOVE_gz_vert (a_major, 't');
              break;
   case 'o' : rc = MOVE_gz_horz (a_major, 'e');   /* omega = bot-right   */
              rc = MOVE_gz_vert (a_major, 'b');
              break;
   case 'n' : rc = MOVE_gz_horz (a_major, 'e');   /* near  = top-right   */
              rc = MOVE_gz_vert (a_major, 't');
              break;
   case 'f' : rc = MOVE_gz_horz (a_major, 's');   /* far   = bot-left    */
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
   char        x_minors    [MAX_STR]  = "tKkjJbsHhlLeaonfSETBAONF";
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
   case 'o' : rc = MOVE_edges ('e');
              rc = MOVE_edges ('b');
              break;
   case 'n' : rc = MOVE_edges ('e');
              rc = MOVE_edges ('t');
              break;
   case 'f' : rc = MOVE_edges ('s');
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
   case 'O' : rc = MOVE_horz    ('$');            /* OMEGA = maximum     */
              rc = MOVE_vert    ('G');
              break;
   case 'N' : rc = MOVE_horz    ('$');            /* NEAR  = maximum     */
              rc = MOVE_vert    ('_');
              break;
   case 'F' : rc = MOVE_horz    ('0');            /* FAR   = maximum     */
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
   char        x_minors    [MAX_STR]  = "hljksetbaonfAONF";
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
   char        x_minors    [MAX_STR]  = "ypdx";
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
   case  'y' : REG_copy  ();
               break;
   case  'p' : REG_paste ('y');
               break;
   case  'd' :
   case  'x' : REG_cut   ();
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
   char        x_minors    [MAX_STR]  = "ypdx";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (MODE_MAP    )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(major mode changes)-------------*/
   if (a_minor == K_RETURN) {
      MODE_enter  (MODE_SOURCE);
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
         MODE_enter  (MODE_VISUAL);
         VISU_start  (CTAB, CCOL, CROW, VISU_FROM);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case ':'      :
         strncpy     (command , ":", MAX_STR);
         MODE_enter  (MODE_COMMAND);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case 's'      :
         EDIT_start  ("");
         MODE_enter  (MODE_INPUT  );
         MODE_input ('m', 'i');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'i';
         break;
      case '='      :
         EDIT_start  ("=");
         MODE_enter  (MODE_INPUT  );
         MODE_input ('m', 'a');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'a';
         break;
      case '#'      :
         EDIT_start  ("#");
         MODE_enter  (MODE_INPUT  );
         MODE_input ('m', 'a');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'a';
         break;
      case '+'      :
         EDIT_start  ("+");
         MODE_enter  (MODE_INPUT  );
         MODE_input ('m', 'a');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'a';
         break;
      case '-'      :
         EDIT_start  ("-");
         MODE_enter  (MODE_INPUT  );
         MODE_input ('m', 'a');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'a';
         break;
      }
      /*---(submodes)--------------------*/
      switch (a_minor) {
      case '\\'     :
         DEBUG_USER   yLOG_note    ("selected menu mode");
         MODE_enter  (SMOD_MENUS  );
         my.menu = MENU_ROOT;
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case 'F'      : MODE_enter  (SMOD_FORMAT  );
                      DEBUG_USER   yLOG_exit    (__FUNCTION__);
                      return 0;
                      break;
      case ','      : MODE_enter  (SMOD_BUFFER  );
                      DEBUG_USER   yLOG_exit    (__FUNCTION__);
                      return 0;
                      break;
      case '"'      : MODE_enter  (SMOD_REGISTER);
                      DEBUG_USER   yLOG_exit    (__FUNCTION__);
                      return '"';  /* make sure double quote goes in prev char */
                      break;
      case 'm'      : 
      case '\''     : MODE_enter  (SMOD_MARK    );
                      DEBUG_USER   yLOG_exit    (__FUNCTION__);
                      return '\'';  /* make sure single quote goes in prev char */
                      break;
      case 'E'      :
                      MODE_enter  (SMOD_ERROR   );
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
      /*> case '@'      : DEP_recalc();                   break;                      <*/
                      /*> case '[' : if (escaped) { sch = ch; special = 1; } else sch = 'x'; break;   <*/
                      /*---(new stuff)----------------*/
      case 'u'      : HIST_undo ();                   break;
      case 'U'      : HIST_redo ();                   break;
                      /*> case 'W'      : REG_bufwrite (my.reg_curr);     break;                      <*/
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
      tab  = &tabs[CTAB];
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
   --rce;  if (MODE_not (SMOD_BUFFER))             return rce;
   /*---(check for control keys)---------*/
   BUF_switch   (a_minor);
   MODE_return  ();
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
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (SMOD_MENUS  )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(mode changes)-------------------*/
   if (a_minor == 27) {
      DEBUG_USER   yLOG_note    ("escape, choose nothing");
      my.menu = ' ';
      MODE_return ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(top menu)-----------------------*/
   if (a_major == ' ') {
      my.menu = a_minor;
   } else {
      DEBUG_USER   yLOG_note    ("execute option");
      if (a_major == 'F') {
         MODE_enter  (SMOD_FORMAT);
         SMOD_format (' ', a_minor);
      }
      my.menu = ' ';
      MODE_return ();
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
   char        x_multi     [MAX_STR]   = "cdgz";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr());
   --rce;  if (MODE_not (MODE_SOURCE)) {
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
         CELL_change  (CHG_INPUT, CTAB, CCOL, CROW, g_contents);
         EDIT_pos     ('r');
         MODE_return  ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;   /* escape  */
         break;
      case  27  : case  'U' :
         DEBUG_USER   yLOG_note    ("escape, forget, and return to previous mode");
         if (tab->sheet[CCOL][CROW] != NULL && tab->sheet[tab->ccol][CROW]->s != NULL) {
            strncpy (g_contents, tab->sheet[tab->ccol][CROW]->s, MAX_STR); 
         }
         EDIT_pos     ('r');
         MODE_return  ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case  'r' : case  'R' :
         DEBUG_USER   yLOG_note    ("rR keys for replace sub-mode");
         MODE_enter   (SMOD_REPLACE);
         SMOD_replace ('m', ' ');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      case  'v' :
         DEBUG_USER   yLOG_note    ("v key for text select sub-mode");
         MODE_enter   (SMOD_SELECT );
         SELC_mode    ('m', ' ');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case '"'  :
         MODE_enter   (SMOD_TEXTREG );
         return a_minor;  /* make sure double quote goes in prev char */
         break;
      case 'E'  :
         MODE_enter   (SMOD_ERROR   );
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
                 MODE_enter  (MODE_INPUT);
                 MODE_input ('m', 'i');
                 return 'i';
                 break;
      }
      /*---(going to input)--------------*/
      switch (a_minor) {
      case 'I' : EDIT_pos   ('0');
      case 'i' : MODE_enter  (MODE_INPUT);
                 MODE_input ('m', tolower (a_minor));
                 DEBUG_USER   yLOG_exit    (__FUNCTION__);
                 return tolower (a_minor);
                 break;
      case 'A' : EDIT_pos   ('$');
      case 'a' : MODE_enter  (MODE_INPUT);
                 MODE_input ('m', tolower (a_minor));
                 DEBUG_USER   yLOG_exit    (__FUNCTION__);
                 return tolower (a_minor);
                 break;
                 /*> case '.' : MODE_enter (SMOD_WANDER); wtype = 'c'; wtab = CTAB; wcol = tabs[CTAB].ccol; wrow = tabs[CTAB].crow; wpos = my.cpos; strcpy(wref, ""); strcpy(wref2, ""); strcpy(wsave, g_contents); break;   <* 
                  *> case ':' : MODE_enter (SMOD_WANDER); wtype = 'r'; wtab = CTAB; wcol = tabs[CTAB].ccol; wrow = tabs[CTAB].crow; wpos = my.cpos; strcpy(wref, ""); strcpy(wref2, ""); strcpy(wsave, g_contents); break;   <*/
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
   char        x_majors    [MAX_STR]  = "rRm";
   static char x_append    = '-';
   static char x_saved = '\0';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (SMOD_REPLACE)) {
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
      MODE_return ();
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
         g_contents [my.cpos] = CHAR_PLACE;
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
      g_contents [my.cpos] = CHAR_PLACE;
      EDIT_done   ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(handle normal chars)------------*/
   if (a_major == 'r') {
      DEBUG_USER   yLOG_note    ("replace the marked character");
      g_contents [my.cpos] = a_minor;
      DEBUG_USER   yLOG_char    ("new  char" , g_contents [my.cpos]);
      MODE_return ();
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
      g_contents [my.cpos] = CHAR_PLACE;
   }
   /*---(correct current position)-------*/
   DEBUG_USER   yLOG_value   ("curr pos"  , my.cpos);
   DEBUG_USER   yLOG_value   ("curr end"  , my.npos);
   if (my.cpos  >= my.npos) {
      DEBUG_USER   yLOG_note    ("update the end pos");
      g_contents [my.npos    ] = CHAR_PLACE;
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
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   /*> --rce;  if (MODE_not (SMOD_ERROR)) {                                           <* 
    *>    DEBUG_USER   yLOG_note    ("not the correct mode");                         <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <*/
   /*---(mode changes)-------------------*/
   /*> if (a_minor == 27 || a_minor == 10) {                                          <* 
    *>    DEBUG_USER   yLOG_note    ("escape/return, return to previous mode");       <* 
    *>    MODE_return ();                                                             <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   if (a_minor == '?') {
      DEBUG_USER   yLOG_note    ("display errors for cell");
      my.info_win = 'E';
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   /*---(complete)-----------------------*/
   MODE_return ();

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
   char        x_majors    [MAX_STR]   = "IiaAm";
   int         i           = 0;             /* loop iterator                  */
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr());
   --rce;  if (MODE_not (MODE_INPUT )) {
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
      g_contents [my.cpos] = CHAR_PLACE;
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
      if (a_minor == 10 && MODE_prev() == MODE_MAP) {
         CELL_change  (CHG_INPUT, CTAB, CCOL, CROW, g_contents);
      }
      if (a_minor == 27 && MODE_prev() == MODE_MAP) {
         if (tab->sheet[CCOL][CROW] != NULL && tab->sheet[tab->ccol][CROW]->s != NULL) {
            strlcpy (g_contents, tab->sheet[tab->ccol][CROW]->s, MAX_STR); 
         } else {
            strlcpy (g_contents, ""                            , MAX_STR); 
         }
      }
      EDIT_pos  ('r');
      MODE_return ();
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


char       /*----: process keys for formatting mode --------------------------*/
SMOD_format        (char a_major, char a_minor)
{
   /*---(current cell -- whether good or not)---*/
   tCELL   *curr = tab->sheet[CCOL][CROW];
   /*---(check for control keys)----------------*/
   switch (a_minor) {
   case   10 :
   case   27 : VISU_clear  ();
               MODE_return ();
               return 0;   /* escape  */
   }
   /*---(check for alignment prefixes)----------*/
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

   case  'K' : CELL_height   (CHG_INPUT, 'K');  break;  /* set to min  */
   case  'k' : CELL_height   (CHG_INPUT, 'k');  break;  /* less by 1   */
   case  'j' : CELL_height   (CHG_INPUT, 'j');  break;  /* more by 1   */
   case  'J' : CELL_height   (CHG_INPUT, 'J');  break;  /* set to max  */

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

   case  '<' : CELL_align    (CHG_INPUT, '<');  break;  /* left        */
   case  '|' : CELL_align    (CHG_INPUT, '|');  break;  /* center      */
   case  '>' : CELL_align    (CHG_INPUT, '>');  break;  /* right       */
   case  '[' : CELL_align    (CHG_INPUT, '[');  break;  /* left dec    */
   case  '^' : CELL_align    (CHG_INPUT, '^');  break;  /* center dec  */
   case  ']' : CELL_align    (CHG_INPUT, ']');  break;  /* right dec   */
   case  '}' : CELL_align    (CHG_INPUT, '}');  break;  /* right +2    */
   case  '{' : CELL_align    (CHG_INPUT, '{');  break;  /* left  +2    */

   case  'M' : CELL_merge    ('m');  break;  /* merge       */
   case  'U' : CELL_merge    ('u');  break;  /* unmerge     */

   case  'i' : CELL_format   (CHG_INPUT, 'i');  break;  /* integer         */
   case  'r' : CELL_format   (CHG_INPUT, 'r');  CELL_decimals (CHG_INPUT, '3'); break;  /* real/float      */
   case  'g' : CELL_format   (CHG_INPUT, 'r');  CELL_decimals (CHG_INPUT, '6'); break;  /* real/float      */
   case  'e' : CELL_format   (CHG_INPUT, 'e');  CELL_decimals (CHG_INPUT, '3'); break;  /* exponencial     */
   case  'E' : CELL_format   (CHG_INPUT, 'E');  CELL_decimals (CHG_INPUT, '3'); break;  /* spaced exponent */
   case  'P' : CELL_format   (CHG_INPUT, 'p');  break;  /* point/bullet    */

   case  ',' : CELL_format   (CHG_INPUT, ',');  break;  /* comma           */
   case  'a' : CELL_format   (CHG_INPUT, 'a');  break;  /* accounting      */
   case  '$' : CELL_format   (CHG_INPUT, '$');  break;  /* dollar          */
   case  's' : CELL_format   (CHG_INPUT, 's');  break;  /* signed          */
   case  '#' : CELL_format   (CHG_INPUT, '#');  CELL_decimals (CHG_INPUT, '6'); break;  /* technical       */
   case  'p' : CELL_format   (CHG_INPUT, '%');  CELL_decimals (CHG_INPUT, '2'); break;  /* percent         */

   case  'o' : CELL_format   (CHG_INPUT, 'o');  break;  /* octal           */
   case  'O' : CELL_format   (CHG_INPUT, 'O');  break;  /* spaced octal    */
   case  'x' : CELL_format   (CHG_INPUT, 'x');  break;  /* hex             */
   case  'X' : CELL_format   (CHG_INPUT, 'X');  break;  /* spaced hex      */
   case  'b' : CELL_format   (CHG_INPUT, 'b');  break;  /* binary          */
   case  'B' : CELL_format   (CHG_INPUT, 'B');  break;  /* spaced binary   */

   case  't' : CELL_format   (CHG_INPUT, 't');  break;  /* time            */
   case  'T' : CELL_format   (CHG_INPUT, 'T');  break;  /* timestamp       */
   case  'd' : CELL_format   (CHG_INPUT, 'd');  break;  /* date            */
   case  'D' : CELL_format   (CHG_INPUT, 'D');  break;  /* full date       */

   case  '-' : CELL_format   (CHG_INPUT, '-');  break;  /* filled dash     */
   case  '=' : CELL_format   (CHG_INPUT, '=');  break;  /* filled equal    */
   case  '_' : CELL_format   (CHG_INPUT, '_');  break;  /* filled under    */
   case  '.' : CELL_format   (CHG_INPUT, '.');  break;  /* filled period   */
   case  '+' : CELL_format   (CHG_INPUT, '+');  break;  /* filled plus     */
   case  ' ' : CELL_format   (CHG_INPUT, ' ');  break;  /* filled empty    */

               /*> case  '"' : CELL_format   (CHG_INPUT, '"'); CELL_align    (CHG_INPUT, '"'); CELL_decimals (CHG_INPUT, '"');  break;  /+ filled empty    +/   <*/
   }
   return 0;
}

char
KEYS_unlock        (void)
{
   tab->froz_col  = '-';
   tab->froz_bcol = 0;
   tab->froz_ecol = 0;
   MOVE_horz ('r');
   tab->froz_row  = '-';
   tab->froz_brow = 0;
   tab->froz_erow = 0;
   MOVE_vert ('r');
}

char
cmd_exec           (char *a_command)
{
   /*---(locals)-----------+-----------+-*/
   char       *p           = NULL;
   char       *q           = " ";
   char        x_work      [MAX_STR]   = "";
   char        rc          = 0;
   int         x_len       = 0;
   char        x_flag      = '-';
   strncpy    (x_work, a_command, MAX_STR);
   x_len = strlen (x_work);
   p = strtok (x_work, q);
   if (p == NULL) return 0;
   if   (strncmp(p, ":w"        , MAX_STR) == 0 ||
         strncmp(p, ":wa"       , MAX_STR) == 0 ||
         strncmp(p, ":wq"       , MAX_STR) == 0 ||
         strncmp(p, ":wqa"      , MAX_STR) == 0 ||
         strncmp(p, ":write"    , MAX_STR) == 0) {
      FILE_write (my.f_title);
      if (p [2] == 'q')  done = 0;
      return 0;
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
   if (x_len == 9 && strcmp (p, ":status") == 0) {
      sta_type = x_work [8];
      return 0;
   }
   if (x_len == 9 && strcmp (p, ":screen") == 0) {
      my.scrn = x_work [8];
      return 0;
   }
   if   (strncmp(p, ":q"        , MAX_STR) == 0 ||
         strncmp(p, ":qa"       , MAX_STR) == 0) {
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
      rc = LOC_parse (p, NULL, &tab->froz_bcol, &tab->froz_brow, NULL);
      if (rc < 0) {
         KEYS_unlock ();
         return 0;
      }
      tab->froz_ecol = tab->froz_bcol;
      tab->froz_erow = tab->froz_brow;
      p = strtok (NULL  , q);
      if (p != NULL) {
         rc = LOC_parse (p, NULL, &tab->froz_ecol, &tab->froz_erow, NULL);
         if (rc < 0) {
            tab->froz_ecol = tab->froz_bcol;
            tab->froz_erow = tab->froz_brow;
         }
      }
      switch (x_flag) {
      case  'c' : tab->froz_col  = 'y';
                  BCOL = CCOL = tab->froz_ecol + 1;
                  BROW = CROW = tab->froz_erow;
                  tab->froz_brow = 0;
                  tab->froz_erow = 0;
                  break;
      case  'r' : tab->froz_row  = 'y';
                  BCOL = CCOL = tab->froz_ecol;
                  BROW = CROW = tab->froz_erow + 1;
                  tab->froz_bcol = 0;
                  tab->froz_ecol = 0;
                  break;
      case  '-' : tab->froz_col = 'y';
                  tab->froz_row = 'y';
                  BCOL = CCOL = tab->froz_ecol + 1;
                  BROW = CROW = tab->froz_erow + 1;
                  break;
      default   : KEYS_unlock ();
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
   /*> if        (strncmp(p, "o"        , MAX_STR) == 0) {                            <* 
    *>    p = strtok(NULL, q);                                                        <* 
    *>    if (p == NULL) return 0;                                                    <* 
    *>    INPT_main (p);                                                              <* 
    *> } else if (strncmp(p, "q"        , MAX_STR) == 0) {                            <* 
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
   x_len = strlen (command);
   switch (a_minor) {
   case   27 : MODE_return ();
               return 0;
   case   10 : cmd_exec (command);
               MODE_return ();
               return 0;   /* return  */
   }
   /*---(check for backspace)------------*/
   if (a_minor == 8 || a_minor == 127) {
      --x_len;
      if (x_len < 0)   x_len = 0;
      command [x_len] = '\0';
      return 0;
   }
   /*---(normal characters)--------------*/
   snprintf (x_temp, 10, "%c", a_minor);
   strcat   (command, x_temp);
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
   --rce;  if (MODE_not (SMOD_WANDER )) {
      return rce;
   }
   /*---(check for control keys)---------*/
   switch (a_curr) {
   case  ',' :
   case  ')' : post = a_curr;
   case  10  :
   case  27  : VISU_clear ();
               LOC_ref (CTAB, tabs[CTAB].ccol, tabs[CTAB].crow, 0, wref);
               CTAB = wtab;
               tabs[CTAB].ccol = wcol;
               tabs[CTAB].crow = wrow;
               tab = &tabs[CTAB];
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
               MODE_return ();
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
      LOC_ref    (CTAB, tabs[CTAB].ccol, tabs[CTAB].crow, 0, wref2);
      VISU_start (CTAB, tabs[CTAB].ccol, tabs[CTAB].crow, VISU_FROM);
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
   char        x_list      [MAX_STR];
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "keys             : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "mode_stack"   )  == 0) {
      MODE_list (x_list);
      snprintf (unit_answer, LEN_TEXT, "keys mode stack  : %s", x_list);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}


/*============================----end-of-source---============================*/
