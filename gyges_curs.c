/*============================----beg-of-source---============================*/

/*===[[ MODULE ]]=============================================================*

 *   application   : s
 *   module        : s_curses
 *   purpose       : provide all code for visual interface using ncures
 *
 */
/*===[[ EXPLANATION ]]========================================================*

 *   the s spreadsheet calculator requires a user interface of some sort when
 *   being used interactively (as opposed to in filter mode).
 *
 *   the point of s is not to compete with full-on, integrated, kitchen-sink
 *   gui-based spreadsheets, it is to provide a rapid, light, clean prototyping
 *   and analysis tool;  therefore, fonts, menus, graphing, and the like are
 *   distractions.
 *
 *   s will be a console based spreedsheet that will obviously have to work in
 *   a full screen mode that can adapt to the nature and size of the terminal
 *   emulator window.  as such, this is a perfect chance to learn ncurses.
 *
 */

#include   "gyges.h"


PRIV  char      CURS_page          (void);

int     s_status_row;
int     s_status_size;
int     s_command_row;
int     s_command_size;



static char  s_mark_list   [LEN_RECD];       /* current marks                  */
static char  s_mark_plus   [LEN_RECD];       /* current marks with mark id     */



#define     MAX_MENU       500
typedef struct cMENU  tMENU;
struct  cMENU {  /* two level menu only, none of that complex shit            */
   char     one;                            /* shortcut for menu              */
   char     menu      [LEN_STR];            /* top level menu item            */
   char     next;                           /* next column                    */
   char     two;                            /* shortcut for submenu           */
   char     name      [LEN_STR];            /* name of menu item              */
   char     desc      [LEN_STR];            /* descripion, example, alternate */
};
tMENU       s_menus     [MAX_MENU] = {
   /* one  ---menu---------- next  -sc-  ---name----------  ---example------------ */
   {  'f', "file"           , ' ', '-', ""               , ""                     },
   {  't', "technical"      , ' ', '-', ""               , ""                     },
   {  'F', "format"         , ' ', '<', "left"           , "test                " },
   {  ' ', ""               , ' ', '|', "center"         , "        test        " },
   {  ' ', ""               , ' ', '>', "right"          , "               test " },
   {  ' ', ""               , ' ', '[', "left bracket"   , "[test              ]" },
   {  ' ', ""               , ' ', '^', "center bracket" , "[       test       ]" },
   {  ' ', ""               , ' ', ']', "right bracket"  , "[              test]" },
   {  ' ', ""               , ' ', '{', "right brace"    , "[  test            ]" },
   {  ' ', ""               , ' ', '}', "left brace"     , "[            test  ]" },
   {  ' ', ""               , ' ', ' ', "fill space"     , "[  test            ]" },
   {  ' ', ""               , ' ', '-', "fill dash"      , "[--test------------]" },
   {  ' ', ""               , ' ', '=', "fill equal"     , "[==test============]" },
   {  ' ', ""               , ' ', '_', "fill under"     , "[__test____________]" },
   {  ' ', ""               , ' ', '.', "fill period"    , "[..test............]" },
   {  ' ', ""               , ' ', '+', "fill plus"      , "[++test++++++++++++]" },
   {  ' ', ""               , ' ', 'i', "integer"        , "            1234567 " },
   {  ' ', ""               , ' ', 'r', "real"           , "            987.541 " },
   {  ' ', ""               , ' ', 'g', "general"        , "         987.541234 " },
   {  ' ', ""               , ' ', ',', "comma"          , "          1,234,567 " },
   {  ' ', ""               , ' ', 's', "comma/sign"     , "         +1,234,567 " },
   {  ' ', ""               , ' ', 'a', "accounting"     , "        (1,234,567) " },
   {  ' ', ""               , ' ', '$', "currency"       , "         $1,234,567 " },
   {  ' ', ""               , ' ', 'p', "percent"        , "                15p " },
   {  ' ', ""               , '-', 'P', "point/bullet"   , "                 5) " },
   {  ' ', ""               , ' ', '#', "technical"      , "  123,456.789'032 + " },
   {  ' ', ""               , ' ', 'e', "exponential"    , "          1.235e+05 " },
   {  ' ', ""               , ' ', 'E', "exp divided"    , "       +1.235 e +05 " },
   {  ' ', ""               , ' ', 'x', "hexidecimal"    , "              xaf68 " },
   {  ' ', ""               , ' ', 'X', "hex divided"    , "            x'af'68 " },
   {  ' ', ""               , ' ', 'b', "binary"         , "          b10101111 " },
   {  ' ', ""               , ' ', 'B', "binary divided" , "        b'1010'1111 " },
   {  ' ', ""               , ' ', 'o', "octal"          , "               o257 " },
   {  ' ', ""               , ' ', 'O', "octal"          , "              o'257 " },
   {  ' ', ""               , ' ', 't', "time (24h)"     , "              13:26 " },
   {  ' ', ""               , ' ', 'd', "date"           , "        2014-Dec-15 " },
   {  ' ', ""               , ' ', 'T', "timestamp"      , "  14.12.15.13.26.15 " },
   {  ' ', ""               , ' ', 'D', "date/time"      , "  2014-Dec-15 13:26 " },
   {  ' ', ""               , ' ', '0', "0 decimals"     , "                  0 " },
   {  ' ', ""               , ' ', '1', "1 decimals"     , "                0.1 " },
   {  ' ', ""               , ' ', '2', "2 decimals"     , "               0.12 " },
   {  ' ', ""               , ' ', '3', "3 decimals"     , "              0.123 " },
   {  ' ', ""               , ' ', '4', "4 decimals"     , "             0.1234 " },
   {  ' ', ""               , ' ', '5', "5 decimals"     , "            0.12345 " },
   {  ' ', ""               , ' ', '6', "6 decimals"     , "           0.123456 " },
   {  ' ', ""               , ' ', '7', "7 decimals"     , "          0.1234567 " },
   {  ' ', ""               , ' ', '8', "8 decimals"     , "         0.12345678 " },
   {  ' ', ""               , ' ', '9', "9 decimals"     , "        0.123456789 " },
   {  'S', "sizing"         , ' ', 'm', "min width"      , "4 chars             " },
   {  ' ', ""               , ' ', 'n', "normal width"   , "8 chars             " },
   {  ' ', ""               , ' ', 'N', "normal+ width"  , "12 chars            " },
   {  ' ', ""               , ' ', 'w', "wide width"     , "20 chars            " },
   {  ' ', ""               , ' ', 'w', "wide+ width"    , "50 chars            " },
   {  ' ', ""               , ' ', 'H', "narrower by 5"  , "-5 chars (rounded)  " },
   {  ' ', ""               , ' ', 'h', "narrower by 1"  , "-1 chars            " },
   {  ' ', ""               , ' ', 'l', "wider by 1"     , "+1 chars            " },
   {  ' ', ""               , ' ', 'L', "wider by 5"     , "+5 chars (rounded)  " },
   {  ' ', ""               , ' ', 'K', "shortest"       , "1 char tall         " },
   {  ' ', ""               , ' ', 'k', "shorter by 1"   , "-1 chars            " },
   {  ' ', ""               , ' ', 'j', "taller by 1"    , "+1 chars            " },
   {  ' ', ""               , ' ', 'J', "tallest"        , "5 chars             " },
   {  ' ', ""               , ' ', 'M', "merge cells"    , "                    " },
   {  ' ', ""               , ' ', 'U', "unmerge cells"  , "                    " },
   {  'D', "distribution"   , ' ', '-', ""               , "                    " },
   {  '~', "end-of-menu"    , ' ', ' ', ""               , ""                     },
   {  ' ', ""               , ' ', ' ', ""               , ""                     },
};



#define     MAX_COLORS    20
typedef  struct cCOLOR  tCOLOR;
struct cCOLOR {
   char        abbr;
   char        terse       [ 5];
   char        desc        [20];
   short       value;
};
static tCOLOR  s_colors [MAX_COLORS] = {
   {  ' ' , "trn" , "transparent"          , -1                    },
   {  'k' , "blk" , "black"                , COLOR_BLACK           },
   {  'r' , "red" , "red"                  , COLOR_RED             },
   {  'g' , "grn" , "green"                , COLOR_GREEN           },
   {  'y' , "yel" , "yellow"               , COLOR_YELLOW          },
   {  'b' , "blu" , "blue"                 , COLOR_BLUE            },
   {  'm' , "mag" , "magenta"              , COLOR_MAGENTA         },
   {  'c' , "cyn" , "cyan"                 , COLOR_CYAN            },
   {  'w' , "whi" , "white"                , COLOR_WHITE           },
   {  '-' , "---" , "end-of-colors"        , -1                    },
};

#define     MAX_COLOR_INFO  100
typedef  struct cCOLOR_INFO  tCOLOR_INFO;
struct cCOLOR_INFO {
   char        terse       [10];            /* short description              */
   char        ctype;                       /* cell type, if it applies       */
   char        desc        [50];            /* description/reason             */
   char        fg_color;                    /* foreground color               */
   char        bg_color;                    /* background color               */
   char        bold;                        /* bold y/n                       */
   int         value;                       /* curses attribute value         */
};
static tCOLOR_INFO  s_color_info [MAX_COLOR_INFO] = {
   /*---terse--- -ty- ---description---------------------------------------- -fg-- -bg-- --b- --v- */
   /*---(window)---------*/
   { "t_norm"   , ' ', "normal title color"                                 , 'k' , 'k' , 'y',   0 },
   { "t_err"    , ' ', "title color when in error"                          , 'w' , 'r' , '-',   0 },
   { "s_norm"   , ' ', "normal status color"                                , 'k' , 'k' , 'y',   0 },
   { "s_err"    , ' ', "status color when in error"                         , 'w' , 'r' , '-',   0 },
   { "w_keys"   , ' ', "window keystoke display"                            , 'r' , ' ' , 'y',   0 },
   { "w_mesg"   , ' ', "window message"                                     , 'w' , ' ' , '-',   0 },
   { "menu"     , ' ', "menus and menu entries "                            , 'k' , 'y' , 'b',   0 },
   /*---(modes)----------*/
   { "m_map"    , ' ', "map mode (2d review of sheet/cell collection"       , 'k' , 'y' , 'b',   0 },
   { "m_src"    , ' ', "source mode (single cell review)"                   , 'k' , 'g' , 'b',   0 },
   { "m_input"  , ' ', "input mode"                                         , 'k' , 'b' , 'b',   0 },
   { "m_repl"   , ' ', "replace sub-mode"                                   , 'k' , 'c' , 'b',   0 },
   { "m_wand"   , ' ', "wander mode"                                        , 'k' , 'r' , 'b',   0 },
   /*---(row/col header)-*/
   { "h_cur"    , ' ', "current row/column header"                          , 'y' , 'k' , 'y',   0 },
   { "h_loc"    , ' ', "locked row/column header"                           , 'b' , 'y' , 'y',   0 },
   { "h_use"    , ' ', "row/column with entries"                            , 'y' , ' ' , '-',   0 },
   { "h_not"    , ' ', "row/column with no entries"                         , 'b' , ' ' , '-',   0 },
   /*---(selection)------*/
   { "curr"     , ' ', "current cell"                                       , 'k' , 'y' , 'b',   0 },
   { "root"     , ' ', "root of visual selection"                           , 'y' , 'k' , 'y',   0 },
   { "visu"     , ' ', "selected, not root/curr"                            , 'y' , 'y' , 'y',   0 },
   { "mark"     , ' ', "location marks"                                     , 'w' , 'c' , 'y',   0 },
   /*---(dep type)-------*/
   { "reqs"     , ' ', "value required from cell"                           , 'm' , 'm' , 'b',   0 },
   { "pros"     , ' ', "value provided to cell"                             , 'g' , 'g' , 'b',   0 },
   { "like"     , ' ', "formula is copy/variation"                          , 'b' , 'b' , 'b',   0 },
   /*---(danger signs)---*/
   { "fdang"    , 'f', "complex numeric formula"                            , 'r' , ' ' , 'y',   0 },
   { "mdang"    , 'm', "complex string formula"                             , 'r' , ' ' , 'y',   0 },
   /*---(cell types)-----*/
   { "num"      , 'n', "numeric literal"                                    , 'b' , ' ' , 'y',   0 },
   { "for"      , 'f', "numeric formula"                                    , 'g' , ' ' , 'y',   0 },
   { "flike"    , 'l', "numeric formula (copy)"                             , 'g' , ' ' , '-',   0 },
   { "str"      , 's', "string literal"                                     , 'y' , ' ' , 'y',   0 },
   { "mod"      , 'm', "string formula"                                     , 'm' , ' ' , 'y',   0 },
   { "mlike"    , 'L', "string formula (copy)"                              , 'm' , ' ' , '-',   0 },
   { "range"    , 'p', "range pointer"                                      , 'c' , ' ' , 'y',   0 },
   { "addr"     , 'p', "address pointer"                                    , 'c' , ' ' , 'y',   0 },
   { "blank"    , '-', "blank cell"                                         , 'b' , ' ' , 'y',   0 },
   { "def"      , ' ', "default for unidentified cells"                     , 'y' , ' ' , 'y',   0 },
   /*---(other)----------*/
   { "warn"     , 'w', "cell warning"                                       , 'w' , 'r' , 'y',   0 },
   { "error"    , 'E', "cell error"                                         , 'w' , 'r' , 'y',   0 },
   { "found"    , ' ', "cell found in most recent search"                   , 'g' , 'w' , 'y',   0 },
   /*---(end)------------*/
   { "-----"    , ' ', "end-of-colors"                                      , '-' , '-' , '-',   0 },
};


/*===[[ COLOR DEFINITIONS ]]==================================================*/
/*---(window)-------------------------*/
static int  S_COLOR_TITLE      = COLOR_PAIR(40) | A_BOLD;
static int  S_COLOR_TITLEE     = COLOR_PAIR(61);
static int  S_COLOR_STATUS     = COLOR_PAIR(40) | A_BOLD;
static int  S_COLOR_STATUSE    = COLOR_PAIR(61);
static int  S_COLOR_KEYS       = COLOR_PAIR(71) | A_BOLD;
static int  S_COLOR_MESSAGE    = COLOR_PAIR(77);
static int  S_COLOR_MENU       = COLOR_PAIR(43) | A_BLINK;
/*---(edit modes)---------------------*/
static int  S_COLOR_CONTENT    = COLOR_PAIR(43) | A_BLINK;
static int  S_COLOR_SOURCE     = COLOR_PAIR(42) | A_BLINK;
static int  S_COLOR_INPUT      = COLOR_PAIR(47) | A_BLINK;
static int  S_COLOR_REPLACE    = COLOR_PAIR(47) | A_BLINK;
static int  S_COLOR_WANDER     = COLOR_PAIR(41) | A_BLINK;
/*---(cells)--------------------------*/
static int  S_COLOR_CURRENT    = COLOR_PAIR(43) | A_BLINK;
static int  S_COLOR_VISUAL     = COLOR_PAIR(23) | A_BOLD;
static int  S_COLOR_ROOT       = COLOR_PAIR(33) | A_BOLD;
static int  S_COLOR_MARK       = COLOR_PAIR(33) | A_BOLD;

static int  S_COLOR_REQS       = COLOR_PAIR(25) | A_BOLD;
static int  S_COLOR_PROS       = COLOR_PAIR(22) | A_BOLD;
static int  S_COLOR_LIKE       = COLOR_PAIR(24) | A_BOLD;

static int  S_COLOR_ERROR      = COLOR_PAIR(61) | A_BOLD;

static int  S_COLOR_POINTER    = COLOR_PAIR(76) | A_BOLD;
static int  S_COLOR_ADDRESS    = COLOR_PAIR(76) | A_BOLD;

static int  S_COLOR_FSIMPLE    = COLOR_PAIR(72) | A_BOLD;
static int  S_COLOR_FDANGER    = COLOR_PAIR(71) | A_BOLD;
static int  S_COLOR_FLIKE      = COLOR_PAIR(72)           ;

static int  S_COLOR_STRING     = COLOR_PAIR(73) | A_BOLD;
static int  S_COLOR_FSTRING    = COLOR_PAIR(75) | A_BOLD;
static int  S_COLOR_FSTRDAG    = COLOR_PAIR(75) | A_BOLD;
static int  S_COLOR_MLIKE      = COLOR_PAIR(75)          ;

static int  S_COLOR_NUMBER     = COLOR_PAIR(74) | A_BOLD;
static int  S_COLOR_NULL       = COLOR_PAIR(70) | A_BOLD;

static int  S_COLOR_NORMAL     = COLOR_PAIR(73) | A_BOLD;
/*---(row and column headers)---------*/
static int  S_COLOR_HEADY      = COLOR_PAIR(33) | A_BOLD;
static int  S_COLOR_HEADL      = COLOR_PAIR(43) | A_BOLD;
static int  S_COLOR_HEADF      = COLOR_PAIR(73);
static int  S_COLOR_HEADN      = COLOR_PAIR(74);


char        msg_type  = '-';
char        sta_error = '-';

static char s_label   [10]  = "";
static int  s_start         = 0;
static int  s_space         = 0;


char         /*-> tbd --------------------------------[ leaf   [gz.210.101.00]*/ /*-[00.0000.10.--]-*/ /*-[--.---.---.--]-*/
CURS_info_request  (char a_type)
{
   my.info_win = a_type;
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.320.001.05]*/ /*-[00.0000.40.--]-*/ /*-[--.---.---.--]-*/
CURS_screen_reset    (void)
{
   KEYS_basics   (' ', 'r');
   KEYS_bcol     (BCOL);
   CURS_col_head ();
   KEYS_brow     (BROW);
   CURS_row_head ();
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      color configuration                     ----===*/
/*====================------------------------------------====================*/
PRIV void  o___COLORS__________o () { return; }
int         s_color_try = 0;
int         s_color_err = 0;

int          /*-> tbd --------------------------------[ leaf   [gn.422.112.20]*/ /*-[01.0000.01.--]-*/ /*-[--.---.---.--]-*/
COLOR_find                (char a_abbr)
{
   DEBUG_GRAF  yLOG_senter  (__FUNCTION__);
   DEBUG_GRAF  yLOG_schar   (a_abbr);
   int         i           = 0;
   for (i = 0; i < MAX_COLORS; ++i) { 
      if (s_colors [i].abbr == '-')              break;
      if (s_colors [i].abbr != a_abbr)           continue;
      DEBUG_GRAF  yLOG_svalue  ("found"     , s_colors [i].value);
      DEBUG_GRAF  yLOG_sinfo   ("name"      , s_colors [i].terse);
      DEBUG_GRAF  yLOG_sexit   (__FUNCTION__);
      return s_colors [i].value;
   }
   DEBUG_GRAF  yLOG_snote   ("FAIL, not found");
   DEBUG_GRAF  yLOG_sexit   (__FUNCTION__);
   return -2;
}

char         /*-> tbd --------------------------------[ ------ [gc.842.031.62]*/ /*-[02.0000.01.--]-*/ /*-[--.---.---.--]-*/
COLOR_setup               (void)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   int         i           = 0;
   int         x_fg        = 0;
   int         x_bg        = 0;
   for (i = 0; i < MAX_COLOR_INFO; ++i) {
      DEBUG_GRAF  yLOG_value   ("ENTRY#----", i);
      /*---(filter)----------------------*/
      if (s_color_info [i].terse [0] == '-')              break;
      /*---(get colors)------------------*/
      DEBUG_GRAF  yLOG_info    ("name"      , s_color_info [i].terse);
      s_color_try += 2;
      x_fg = COLOR_find  (s_color_info [i].fg_color);
      if (x_fg == -2) {
         ++s_color_err;
         x_fg  = COLOR_find  ('w');
      }
      x_bg = COLOR_find  (s_color_info [i].bg_color);
      if (x_bg == -2) {
         ++s_color_err;
         x_bg  = COLOR_find  (' ');
      }
      init_pair (i + 20, x_fg, x_bg);
      if (s_color_info [i].bold == 'y') {
         s_color_info [i].value =  COLOR_PAIR (i + 20) | A_BOLD;
         DEBUG_GRAF  yLOG_value   ("bold"      , s_color_info [i].value);
      } else if (s_color_info [i].bold == 'b') {
         s_color_info [i].value =  COLOR_PAIR (i + 20) | A_BLINK;
         DEBUG_GRAF  yLOG_value   ("blink"     , s_color_info [i].value);
      } else {
         s_color_info [i].value =  COLOR_PAIR (i + 20);
         DEBUG_GRAF  yLOG_value   ("normal"    , s_color_info [i].value);
      }
   }
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return  0;
}

int          /*-> tbd --------------------------------[ leaf   [gn.532.112.30]*/ /*-[01.0000.01.--]-*/ /*-[--.---.---.--]-*/
COLOR_assign              (char *a_terse)
{
   DEBUG_GRAF  yLOG_senter  (__FUNCTION__);
   int         i           = 0;
   ++s_color_try;
   for (i = 0; i < MAX_COLOR_INFO; ++i) {
      /*---(filter)----------------------*/
      if (s_color_info [i].terse [0] == '-')              break;
      if (s_color_info [i].terse [0] != a_terse [0])      continue;
      if (strcmp (s_color_info [i].terse, a_terse) != 0)  continue;
      DEBUG_GRAF  yLOG_svalue  ("found"     , s_color_info [i].desc);
      DEBUG_GRAF  yLOG_sexit   (__FUNCTION__);
      return s_color_info [i].value;
   }
   ++s_color_err;
   DEBUG_GRAF  yLOG_snote   ("not found, returning default");
   DEBUG_GRAF  yLOG_sexit   (__FUNCTION__);
   return s_color_info [0].value;
}

char         /*-> tbd --------------------------------[ shoot  [gc.D92.001.02]*/ /*-[00.0000.01.--]-*/ /*-[--.---.---.--]-*/
COLOR_init                (void)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(setup colors)-------------------*/
   DEBUG_GRAF  yLOG_note    ("base color setup");
   COLOR_setup   ();
   /*---(summary)------------------------*/
   DEBUG_GRAF  yLOG_value   ("attempts"  , s_color_try);
   DEBUG_GRAF  yLOG_value   ("errors"    , s_color_err);
   /*---(prepare)------------------------*/
   DEBUG_GRAF  yLOG_note    ("color configuration");
   s_color_try        = 0;
   s_color_err        = 0;
   /*---(window)-------------------------*/
   S_COLOR_TITLE      = COLOR_assign ("t_norm" );
   S_COLOR_TITLEE     = COLOR_assign ("t_err"  );
   S_COLOR_STATUS     = COLOR_assign ("s_norm" );
   S_COLOR_STATUSE    = COLOR_assign ("s_err"  );
   S_COLOR_KEYS       = COLOR_assign ("w_keys" );
   S_COLOR_MESSAGE    = COLOR_assign ("w_mesg" );
   S_COLOR_MENU       = COLOR_assign ("menu"   );
   /*---(edit modes)---------------------*/
   S_COLOR_CONTENT    = COLOR_assign ("m_map"  );
   S_COLOR_SOURCE     = COLOR_assign ("m_src"  );
   S_COLOR_INPUT      = COLOR_assign ("m_input");
   S_COLOR_REPLACE    = COLOR_assign ("m_repl" );
   S_COLOR_WANDER     = COLOR_assign ("m_wand" );
   /*---(selection)----------------------*/
   S_COLOR_CURRENT    = COLOR_assign ("curr"   );
   S_COLOR_ROOT       = COLOR_assign ("root"   );
   S_COLOR_VISUAL     = COLOR_assign ("visu"   );
   S_COLOR_MARK       = COLOR_assign ("mark"   );
   /*---(deps)---------------------------*/
   S_COLOR_REQS       = COLOR_assign ("reqs"   );
   S_COLOR_PROS       = COLOR_assign ("pros"   );
   S_COLOR_LIKE       = COLOR_assign ("like"   );
   /*---(danger)-------------------------*/
   S_COLOR_FDANGER    = COLOR_assign ("fdang"  );
   S_COLOR_FSTRDAG    = COLOR_assign ("mdang"  );
   /*---(cells)--------------------------*/
   S_COLOR_NUMBER     = COLOR_assign ("num"    );
   S_COLOR_FSIMPLE    = COLOR_assign ("for"    );
   S_COLOR_FLIKE      = COLOR_assign ("flike"  );
   S_COLOR_STRING     = COLOR_assign ("str"    );
   S_COLOR_FSTRING    = COLOR_assign ("mod"    );
   S_COLOR_MLIKE      = COLOR_assign ("mlike"  );
   S_COLOR_POINTER    = COLOR_assign ("range"  );
   S_COLOR_ADDRESS    = COLOR_assign ("addr"   );
   S_COLOR_NULL       = COLOR_assign ("blank"  );
   S_COLOR_NORMAL     = COLOR_assign ("def"    );
   S_COLOR_ERROR      = COLOR_assign ("error"  );
   /*---(row and column headers)---------*/
   S_COLOR_HEADY      = COLOR_assign ("h_cur"  );
   S_COLOR_HEADL      = COLOR_assign ("h_loc"  );
   S_COLOR_HEADF      = COLOR_assign ("h_use"  );
   S_COLOR_HEADN      = COLOR_assign ("h_not"  );
   /*---(summary)------------------------*/
   DEBUG_GRAF  yLOG_value   ("attempts"  , s_color_try);
   DEBUG_GRAF  yLOG_value   ("errors"    , s_color_err);
   /*---(complete)-----------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return  0;
}



/*====================------------------------------------====================*/
/*===----                   specific areas of screen                   ----===*/
/*====================------------------------------------====================*/
PRIV void  o___SPECIFIC________o () { return; }

char         /*-> tbd --------------------------------[ ------ [gz.MG1.171.O7]*/ /*-[02.I000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_formula       (tCELL *a_curr)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------------------------*/
   char      msg[500]  = "";                   /* temporary display message   */
   int       len       = 0;
   int       w         = 0;
   int       x_beg     = 0;
   int       x_end     = 0;
   int       x_gap     = 0;
   int       x_len     = 0;
   /*---(clear line)---------------------*/
   if (sta_error == 'y')  attron (S_COLOR_TITLEE);
   else                   attron (S_COLOR_TITLE);
   mvprintw (row_formula, 0, "%*.*s", my.x_full, my.x_full, g_empty);
   /*---(location)-----------------------*/
   /*> strcpy(label, p_tab->cols[CCOL].l);                                         <* 
    *> if (label[0] == '-') label[0] = ' ';                                           <*/
   /*> mvprintw (row_formula,  0, "%c%c [%s%d ]", mode, (VISU_islive()) ? 'v' : ' ', label, CROW + 1);   <*/
   /*---(label)------------------------------*/
   if (a_curr != NULL)    strcpy  (s_label, a_curr->label);
   else                   LOC_ref (CTAB, CCOL, CROW, 0, s_label);
   /*---(length)-----------------------------*/
   len = strlen (g_contents);
   /*---(display)----------------------------*/
   switch (my.layout_formula) {
   case G_FORMULA_DEBUG :
      /*---(1st 12 chars)---*/
      mvprintw (row_formula,  0, "%c  %c%c %-6.6s", yVIKEYS_mode_curr(), (VISU_islive()) ? 'v' : ' ', my.reg_curr, s_label);
      /*---(2nd 13 chars)---*/
      if (a_curr != NULL)  mvprintw (row_formula, 12, " %02d %c %c %c %c  ", LOC_col_width (CTAB, CCOL), a_curr->t, a_curr->f, a_curr->d, a_curr->a);
      else                 mvprintw (row_formula, 12, " %02d - - - -  "    , LOC_col_width (CTAB, CCOL));
      /*---(3rd  5 chars)---*/
      mvprintw (row_formula, 25, "%4d", len);
      /*---(4th 14 chars)---*/
      attrset (0);
      if (yURG_debugmode () == 'y')  attron (S_COLOR_TITLEE);
      else                           attron (S_COLOR_TITLE );
      mvprintw (row_formula, my.x_full - 14, " %s of gyges", VER_NUM);
      s_start  = 30;
      break;
   case G_FORMULA_SMALL :
      /*---(1st  6 chars)---*/
      mvprintw (row_formula,  0, "%-6.6s", s_label);
      /*---(2nd  5 chars)---*/
      attrset (0);
      if (yURG_debugmode () == 'y')  attron (S_COLOR_TITLEE);
      else                           attron (S_COLOR_TITLE );
      mvprintw (row_formula, my.x_full - 5, " %s", VER_NUM);
      s_start  =  7;
      break;
   case G_FORMULA_TINY  :
      s_start  =  1;
      break;
   }
   s_space  = my.apos;
   /*---(set color)------------------------*/
   attrset (0);
   switch (yVIKEYS_mode_curr()) {
   case MODE_SOURCE  : attron (S_COLOR_SOURCE );  break;
   case MODE_INPUT   : attron (S_COLOR_INPUT  );  break;
   case SMOD_SELECT  : attron (S_COLOR_SOURCE );  break;
   case SMOD_TEXTREG : attron (S_COLOR_SOURCE );  break;
   case SMOD_WANDER  : attron (S_COLOR_WANDER );  break;
   case SMOD_REPLACE : attron (S_COLOR_REPLACE);  break;
   default           : attron (S_COLOR_CONTENT);  break;
   }
   /*---(contents)-------------------------*/
   w  = s_space - len;
   if (w < 0) w = 0;
   mvprintw (row_formula, s_start, "%*.*s", s_space, s_space, g_empty);
   snprintf (msg, 500, "%s",    g_contents);
   mvprintw (row_formula, s_start, "%-*.*s", s_space, s_space, msg + my.bpos);
   /*---(highlight off)------------------*/
   attrset (0);
   /*---(show selection on top)------------*/
   if ((yVIKEYS_mode_curr() == SMOD_SELECT || yVIKEYS_mode_curr() == SMOD_TEXTREG) && SELC_islive) {
      x_beg = SELC_from ();
      x_end = SELC_to   ();
      if (x_beg < my.epos || x_end > my.bpos) {
         if (x_beg < my.bpos)  x_beg = my.bpos;
         if (x_end > my.epos)  x_end = my.epos;
         x_gap = x_beg - my.bpos;
         x_len = x_end - x_beg + 1;
         attron (S_COLOR_VISUAL);
         mvprintw (row_formula, s_start + x_gap, "%-*.*s", x_len, x_len, msg + x_beg);
         attrset (0);
      }
   }
   /*---(boundary markers)---------------*/
   attron (S_COLOR_VISUAL );
   sprintf (msg, "%c", G_CHAR_NULL );
   /*---(beginning)---------*/
   if      (my.npos == 0)            mvprintw (row_formula, s_start - 1, msg);
   else if (my.bpos == 0)            mvprintw (row_formula, s_start - 1, " ");
   else                              mvprintw (row_formula, s_start - 1, "<");
   /*---(ending)------------*/
   if      (my.npos == 0)            mvprintw (row_formula, s_start + s_space, msg);
   else if (my.epos == my.npos - 1)  mvprintw (row_formula, s_start + s_space, " ");
   else                              mvprintw (row_formula, s_start + s_space, ">");
   /*---(done)--------------*/
   attrset (0);
   /*---(complete)-----------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gc.HB1.152.98]*/ /*-[02.3000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_status        (tCELL *a_curr)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   int         l           = 0;             /* string length                  */
   int         i           = 0;             /* iterator -- keys               */
   char        msg[500]  = "n/a";                /* temporary display message   */
   char        t  [500]  = "";                   /* temporary display message   */
   char        rpn[LEN_RECD] = "";
   if (s_status_size <= 0)  return 0;
   switch (my.layout_status) {
   case G_STATUS_CELL     : /* cell details */
      if (a_curr != NULL) {
         strncpy (rpn , "+", LEN_RECD);
         if (a_curr != NULL && a_curr->rpn != NULL) strncpy (rpn, a_curr->rpn, LEN_RECD);
         snprintf (msg, 500, "[ rpn %-20.20s ][ reqs=%-20.20s ][ pros=%-20.20s ][ like=%-20.20s ]", rpn, reqs, deps, like);
      }
      break;
   case G_STATUS_DEPS     : /* cell details */
      if (a_curr != NULL) {
         snprintf (msg, 500, "[ reqs=%-40.40s ][ pros=%-40.40s ]", reqs, deps);
      }
      break;
   case G_STATUS_BUFFER   : /* buffer contents */
      LOC_tab_status (CTAB, msg);
      break;
   case G_STATUS_RPN      : /* rpn contents */
      if (a_curr != NULL && a_curr->rpn != NULL) strncpy (rpn, a_curr->rpn, LEN_RECD);
      else                                       strncpy (rpn, "((null))", LEN_RECD);
      snprintf (msg, 500, "[ %-100.100s ]", rpn);
      break;
   case G_STATUS_REGS     : /* register contents */
      REG_list     (my.reg_curr  , bufc);
      snprintf (msg, 500, "[ reg %-100.100s ]", bufc);
      break;
   case G_STATUS_TREG     :
      TREG_entry (REG_CURR, msg);
      break;
   case G_STATUS_MARK     :
      MARK_status (msg);
      break;
   case G_STATUS_KEYLOG   :
      KEYS_status (msg);
      break;
   case G_STATUS_HISTORY  :
      snprintf (msg, 500, "[ nhist : %4d, chist : %4d, top : %s ]", nhist, chist, hist [chist].act);
      break;
   case G_STATUS_MODELIST :
      yVIKEYS_mode_list (msg);
      break;
   case G_STATUS_VISUAL   :
      VISU_status (msg);
      break;
   case G_STATUS_ERRORS   :
      snprintf (msg, 500, "errors (%3d)", nerror);
      break;
   case G_STATUS_TAB      :
      LOC_tab_name (CTAB, t);
      snprintf (msg, 500, "[ tab : %c, %s ][ %dc x %dr ]", CTAB, t, NCOL, NROW);
   case G_STATUS_FILE     :
   default  :
      snprintf (msg, 500, " %-20.20s%*.*s%30.30s %-4.4s ", my.f_name, my.x_full - 57, my.x_full - 57, g_empty, ver_txt, ver_num);
      break;
   }
   IF_MACRO_ON {
      snprintf (msg, 500, "macro %c %c %c %3d %02x %3d:%s", my.macro_name, my.macro_mode, my.macro_delay, my.macro_pos, (uchar) my.macro_char, my.macro_len, my.macro_keys);
   }
   if      (sta_error         == 'y')           attron (S_COLOR_STATUSE);
   else                                         attron (S_COLOR_STATUS);
   IF_MACRO_ON                                  attron (S_COLOR_STATUSE);
   mvprintw(s_status_row, 0, "%*.*s", my.x_full, my.x_full, g_empty);
   mvprintw(s_status_row, 0, msg);
   attrset    (0);
   IF_MACRO_ON {
      attron   (S_COLOR_CONTENT);
      mvprintw (s_status_row, 23 + my.macro_pos, "%c", my.macro_keys [my.macro_pos]);
      attrset  (0);
   }
   sta_error = '-';
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gc.321.002.11]*/ /*-[00.2000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_message       (void)
{
   if (s_command_size <= 0 && yVIKEYS_mode_not (MODE_COMMAND))  return 0;
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   attron  (S_COLOR_MESSAGE);
   mvprintw (s_command_row, 0, "%*.*s", my.x_full, my.x_full, g_empty);
   mvprintw (s_command_row, 0, my.message);
   attroff (S_COLOR_MESSAGE);
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.220.101.41]*/ /*-[00.0000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_col_color     (short a_col)
{
   if      (a_col == CCOL)                       attron  (S_COLOR_HEADY   );
   else if (FR_COL == 'y' && a_col <= FR_ECOL)   attron  (S_COLOR_HEADL   );
   else if (LOC_col_used (CTAB, a_col) >  0)     attron  (S_COLOR_HEADF   );
   else                                          attron  (S_COLOR_HEADN   );
   return 0;
}

char         /*-> update the column labels -----------[ ------ [gz.D91.061.A5]*/ /*-[02.3000.32.--]-*/ /*-[--.---.---.--]-*/
CURS_col_head      (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;                  /* iterator -- columns       */
   int         w           = 0;                  /* column width              */
   int         wa          = 0;                  /* adjusted column width     */
   int         cw          = my.x_left;          /* cumulative column width   */
   char        msg         [500]       = "";     /* temporary display message */
   char        label       [3]         = "";     /* column lable              */
   /*---(begin)--------------------------*/
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(process locked)-----------------*/
   if (FR_COL == 'y') {
      for (i = FR_BCOL; i <=  FR_ECOL; ++i) {
         if (i >= NCOL) break;
         /*---(prepare)---------------------*/
         w     = LOC_col_width (CTAB, i);
         wa    = w - 4;
         cw    = LOC_col_xpos  (CTAB, i) + w;
         LOC_col_label (CTAB, i, label);
         /*---(output)----------------------*/
         snprintf(msg, 500, "\[%*.*s%s\]", wa, wa, g_dashes, label);
         CURS_col_color  (i);
         mvprintw (row_chead, LOC_col_xpos  (CTAB, i), msg);
         attrset (0);
      }
   }
   /*---(process oolumns)----------------*/
   KEYS_pcol ();
   for (i = BCOL; i <=  ECOL; ++i) {
      if (i >= NCOL) break;
      /*---(prepare)---------------------*/
      w     = LOC_col_width (CTAB, i);
      wa    = w - 4;
      cw    = LOC_col_xpos  (CTAB, i) + w;
      LOC_col_label (CTAB, i, label);
      /*---(output)----------------------*/
      snprintf(msg, 500, "\[%*.*s%s\]", wa, wa, g_dashes, label);
      CURS_col_color  (i);
      mvprintw (row_chead, LOC_col_xpos  (CTAB, i), msg);
      attrset (0);
   }
   /*---(fill in right side)-------------*/
   if (cw < (my.x_full - my.x_right)) {
      w     = my.x_full - cw;
      wa    = w - 4;
      LOC_col_label (CTAB, ECOL + 1, label);
      if (ECOL < NCOL - 1){
         if      (w == 1) snprintf(msg, 500, ">");
         else if (w == 2) snprintf(msg, 500, "\[>");
         else if (w == 3) snprintf(msg, 500, "\[->");
         else             snprintf(msg, 500, "\[%*.*s%s>", wa, wa, g_dashes, label);
      } else              snprintf(msg, 500, "%*.*s ", w, w, g_empty);
      CURS_col_color  (ECOL + 1);
      mvprintw (row_chead, cw, msg);
      attrset (0);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.220.101.41]*/ /*-[00.0000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_row_color       (short a_row)
{
   if      (a_row == CROW)                       attron  (S_COLOR_HEADY   );
   else if (FR_ROW == 'y' && a_row <= FR_EROW)   attron  (S_COLOR_HEADL   );
   else if (LOC_row_used (CTAB, a_row) >  0)     attron  (S_COLOR_HEADF   );
   else                                          attron  (S_COLOR_HEADN   );
   return 0;
}

char         /*-> update row labels ------------------[ ------ [gz.741.041.33]*/ /*-[02.2000.22.--]-*/ /*-[--.---.---.--]-*/
CURS_row_head      (void)
{
   /*---(locals)-----------------------------*/
   int       i         = 0;                    /* iterator -- rows            */
   int       j         = 0;                    /* iterator -- rows            */
   int       h         = 0;                    /* row height                  */
   int       ch        = row_main;             /* cumulative column width     */
   /*---(header)-----------------------------*/
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(process locked rows)----------------*/
   if (FR_ROW == 'y') {
      for (i = FR_BROW; i <=  FR_EROW; ++i) {
         if (i >= NROW)  break;
         /*---(prepare)----------------------------*/
         CURS_row_color  (i);
         mvprintw (LOC_row_ypos (CTAB, i), 0, "%4d ", i + 1);
         attrset (0);
      }
   }
   KEYS_prow ();
   /*---(process rows)-----------------------*/
   for (i = BROW; i <=  EROW; ++i) {
      if (i >= NROW)  break;
      /*---(prepare)----------------------------*/
      CURS_row_color  (i);
      mvprintw (LOC_row_ypos (CTAB, i), 0, "%4d ", i + 1);
      attrset (0);
   }
   /*---(complete)---------------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gz.860.161.50]*/ /*-[03.3000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_menuroot      (char a_menu)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   int         x_count     = 0;
   int         x_row       = 2;
   int         x_col       = 1;
   char        x_line      [LEN_RECD];
   char       *x_title     = " - -root-menu- ";
   /*---(header)-------------------------*/
   attron (S_COLOR_TITLE);
   mvprintw  (x_row, x_col, x_title);
   attrset (0);
   /*---(show options)-------------------*/
   for (i = 0; i < MAX_MENU; ++i) {
      /*---(filter)----------------------*/
      if (s_menus [i].one == '~') break;
      if (s_menus [i].one == ' ') continue;
      if (a_menu != MENU_ROOT && s_menus [i].one != a_menu)  continue;
      /*---(display)---------------------*/
      ++x_row;
      if (s_menus [i].one == a_menu)  attron (S_COLOR_CURRENT);
      else                            attron (S_COLOR_VISUAL);
      sprintf (x_line, " %c %-11.11s ",
            s_menus [i].one, s_menus [i].menu);
      mvprintw  (x_row, x_col, x_line);
      attrset (0);
   }
   ++x_row;
   /*---(footer)-------------------------*/
   attron (S_COLOR_TITLE);
   mvprintw  (x_row, x_col, x_title);
   attrset (0);
   /*---(complete)---------------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gz.B80.171.60]*/ /*-[03.5000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_menusub       (char a_menu)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =  0;
   int         x_count     =  0;
   int         x_row       =  4;
   int         x_col       =  1;
   char        x_line      [LEN_RECD];
   char       *x_title     = " - --submenu--------------------------";
   char        x_menu      = '-';
   /*---(header)-------------------------*/
   attron (S_COLOR_TITLE);
   mvprintw  (x_row, x_col, x_title);
   attrset (0);
   /*---(show options)-------------------*/
   for (i = 0; i < MAX_MENU; ++i) {
      /*---(filter)----------------------*/
      if (s_menus [i].one == '~'   )  break;
      if (s_menus [i].one == a_menu)  x_menu = 'y';
      if (s_menus [i].one != a_menu && s_menus [i].one != ' ')  x_menu = '-';
      if (x_menu          != 'y'   )  continue;
      /*---(display)---------------------*/
      ++x_row;
      attron (S_COLOR_VISUAL);
      sprintf (x_line, " %c %-15.15s%-20.20s",
            s_menus [i].two, s_menus [i].name, s_menus [i].desc);
      mvprintw  (x_row, x_col, x_line);
      attrset (0);
      if (s_menus [i].next == '-' || x_row > 32) { 
         if (x_col > 20) break;
         ++x_row;
         attron (S_COLOR_TITLE);
         mvprintw  (x_row, x_col, x_title);
         attrset (0);
         x_col += strlen (x_title) + 2;
         x_row = 4;
         attron (S_COLOR_TITLE);
         mvprintw  (x_row, x_col, x_title);
         attrset (0);
      }
   }
   ++x_row;
   /*---(footer)-------------------------*/
   attron (S_COLOR_TITLE);
   mvprintw  (x_row, x_col, x_title);
   attrset (0);
   /*---(complete)---------------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.FC0.061.C1]*/ /*-[02.I000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_info_cell       (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 3;
   char        x_temp      [LEN_RECD];
   char        x_line      [LEN_RECD];
   int         x_len       = 0;
   char       *x_title     = " ------------------cell details-------------------- ";
   tCELL      *x_curr      = LOC_cell_at_curr ();
   /*---(header)-------------------------*/
   attron (S_COLOR_TITLE);
   mvprintw   ( i++, 10, x_title);
   attrset    (0);
   /*---(show variables)-----------------*/
   attron     (S_COLOR_CURRENT);
   sprintf    (x_line, " %-5s tab=%3d, col=%3d, row=%3d                    ", s_label, CTAB, CCOL, CROW);
   mvprintw   ( i++, 10, x_line);
   if (x_curr == NULL) {
      sprintf    (x_line, " %-50.50s ", " s --- (null)");
      mvprintw   ( i++, 10, x_line);
   } else {
      if (x_curr->l < 43)  sprintf    (x_temp, "s %3d [%-s]"     , x_curr->l, x_curr->s);
      else                 sprintf    (x_temp, "s %3d [%-42.42s>", x_curr->l, x_curr->s);
      sprintf    (x_line, " %-50.50s ", x_temp);
      mvprintw   ( i++, 10, x_line);
      sprintf    (x_temp, "v     = %-16.6lf", x_curr->v_num);
      sprintf    (x_line, " %-50.50s ", x_temp);
      mvprintw   ( i++, 10, x_line);
      if (x_curr->v_str == NULL) {
         sprintf    (x_line, " %-50.50s ", "m --- (null)");
         mvprintw   ( i++, 10, x_line);
      } else {
         x_len = strlen (x_curr->v_str);
         if (x_len     < 43)  sprintf    (x_temp, "m %3d [%-s]"     , x_len, x_curr->v_str);
         else                 sprintf    (x_temp, "m %3d [%-42.42s>", x_len, x_curr->v_str);
         sprintf    (x_line, " %-50.50s ", x_temp);
         mvprintw   ( i++, 10, x_line);
      }
      sprintf    (x_line, " t     %c%-43.43s ", x_curr->t, " ");
      mvprintw   ( i++, 10, x_line);
      sprintf    (x_line, " a     %c%-43.43s ", x_curr->a, " ");
      mvprintw   ( i++, 10, x_line);
      sprintf    (x_line, " d     %c%-43.43s ", x_curr->d, " ");
      mvprintw   ( i++, 10, x_line);
      sprintf    (x_line, " f     %c%-43.43s ", x_curr->f, " ");
      mvprintw   ( i++, 10, x_line);
      if (x_curr->p == NULL) {
         sprintf    (x_line, " %-50.50s ", "p --- (null)");
         mvprintw   ( i++, 10, x_line);
      } else {
         x_len = strlen (x_curr->p);
         if (x_len     < 43)  sprintf    (x_temp, "p %3d [%-s]"     , x_len, x_curr->p);
         else                 sprintf    (x_temp, "p %3d [%-42.42s>", x_len, x_curr->p);
         sprintf    (x_line, " %-50.50s ", x_temp);
         mvprintw   ( i++, 10, x_line);
      }
      sprintf    (x_line, " nrpn  %-4d%-40.40s ", x_curr->nrpn     , " ");
      mvprintw   ( i++, 10, x_line);
      sprintf    (x_line, " npro  %-4d%-40.40s ", x_curr->nprovide , " ");
      mvprintw   ( i++, 10, x_line);
      sprintf    (x_line, " nreq  %-4d%-40.40s ", x_curr->nrequire , " ");
      mvprintw   ( i++, 10, x_line);
      sprintf    (x_line, " nerr  %-4d%-40.40s ", x_curr->nerror   , " ");
      mvprintw   ( i++, 10, x_line);
   }
   attrset    (0);
   /*---(footer)-------------------------*/
   attron     (S_COLOR_TITLE);
   mvprintw   ( i++, 10, x_title);
   attrset    (0);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.A70.061.41]*/ /*-[01.7000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_listbufs      (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_line      [LEN_RECD];
   int         x_row       = 4;
   int         x_col       = 10;
   char       *x_title     = " # ---name-------- col rows -c- t     # ---name-------- col rows -c- t ";
   char       *x_leave     = "         please PRESS ANY KEY to escape this information window        ";
   /*---(header)-------------------------*/
   attron    (S_COLOR_VISUAL);
   mvprintw  (x_row++, x_col, x_leave);
   attrset   (0);
   attron    (S_COLOR_TITLE);
   mvprintw  (x_row++, x_col, x_title);
   attrset   (0);
   /*---(show marks)---------------------*/
   for (i = 0; i < 18; ++i) {
      /*---(left)------------------------*/
      LOC_tab_line (i, x_line);
      if (i == CTAB     )  attron (S_COLOR_CURRENT);
      else                 attron (S_COLOR_VISUAL);
      mvprintw   (x_row  , x_col, x_line);
      attrset (0);
      /*---(separator)-------------------*/
      attron (S_COLOR_TITLE);
      mvprintw   (x_row  , x_col + 34, "   ");
      attrset (0);
      /*---(upper case)------------------*/
      LOC_tab_line (i + 18, x_line);
      if (i + 18 == CTAB)  attron (S_COLOR_CURRENT);
      else                 attron (S_COLOR_VISUAL);
      mvprintw   (x_row++, x_col + 37, x_line);
      attrset (0);
      /*---(done)------------------------*/
   }
   /*---(footer)-------------------------*/
   attron (S_COLOR_TITLE);
   mvprintw   (x_row++, x_col, x_title);
   attrset (0);
   attron    (S_COLOR_VISUAL);
   mvprintw  (x_row++, x_col, x_leave);
   attrset   (0);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.EA0.0H1.51]*/ /*-[03.7000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_list_mark       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         i           =    0;
   char        x_marks     [LEN_RECD] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789()";
   char        x_mark      =  '-';
   int         x_len       =    0;
   int         x_wide      =    0;
   int         x_row       =    0;
   int         x_col       =   -1;
   cint        x_top       =    5;
   cint        x_left      =   10;
   cint        x_rows      =   26;
   cint        x_buf       =    3;
   char        x_line      [LEN_RECD];
   char        x_label     [10];
   char       *x_equals    = "=============================================";
   char       *x_title     = " -m- -address-- ";
   char       *x_footer    = " press ANY KEY to escape this window ";
   /*---(prepare)------------------------*/
   x_len  = strlen (x_marks);
   x_wide = strlen (x_title) + x_buf;
   /*---(show marks)---------------------*/
   for (i = 0; i < x_len; ++i) {
      /*---(position)--------------------*/
      x_col = trunc (i / x_rows);
      x_row = i % x_rows;
      /*---(title)-----------------------*/
      if (x_row == 0)  {
         attron (S_COLOR_TITLE);
         mvprintw   ( x_top  - 1    , x_left + x_col * x_wide, x_title);
         mvprintw   ( x_top + x_rows, x_left + x_col * x_wide, x_title);
         attrset (0);
         if (x_col > 0) {
            attron (S_COLOR_TITLE);
            mvprintw   (x_top - 1     , x_left + x_col * x_wide - x_buf, "   ");
            mvprintw   (x_top + x_rows, x_left + x_col * x_wide - x_buf, "   ");
            attrset (0);
         }
      }
      /*---(separator)-------------------*/
      if (x_col > 0) {
         attron (S_COLOR_TITLE);
         mvprintw   (x_top + x_row, x_left + x_col * x_wide - x_buf, "   ");
         attrset (0);
      }
      /*---(lower case)------------------*/
      x_mark = x_marks [i];
      rc = MARK_entry (x_mark, x_line);
      if (rc == 0)  attron (S_COLOR_CURRENT);
      else          attron (S_COLOR_VISUAL);
      mvprintw   (x_top + x_row, x_left + x_col * x_wide, x_line);
      attrset (0);
      /*---(done)------------------------*/
   }
   /*---(footer)-------------------------*/
   x_len = (strlen (x_title) * ++x_col) + (--x_col * x_buf) - strlen (x_footer);
   sprintf (x_line, "%.*s%s%.*s",
         x_len / 2, x_equals,
         x_footer,
         x_len - x_len / 2, x_equals);
   attron (S_COLOR_SOURCE);
   mvprintw   (x_top + x_rows + 1, x_left, x_line);
   attrset (0);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.860.061.11]*/ /*-[01.5000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_info_layout   (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_line      [LEN_RECD];
   int         x_row       = 4;
   int         x_col       = 10;
   char       *x_title     = " ---command---------- ---description-------------------------- ";
   char       *x_leave     = "    please PRESS ANY KEY to escape this information window     ";
   /*---(header)-------------------------*/
   attron    (S_COLOR_VISUAL);
   mvprintw  (x_row++, x_col, x_leave);
   attrset   (0);
   attron    (S_COLOR_TITLE);
   mvprintw  (x_row++, x_col, x_title);
   attrset   (0);
   /*---(show registers)-----------------*/
   attron    (S_COLOR_CURRENT);
   for (i = 0; i < MAX_LAYOUT; ++i) {
      /*---(lower case)------------------*/
      if (PROG_layout_entry  (i, x_line) < 0) break;
      mvprintw  (x_row++, x_col, x_line);
   }
   attrset   (0);
   /*---(footer)-------------------------*/
   attron    (S_COLOR_TITLE);
   mvprintw  (x_row++, x_col, x_title);
   attrset   (0);
   attron    (S_COLOR_VISUAL);
   mvprintw  (x_row++, x_col, x_leave);
   attrset   (0);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.860.081.21]*/ /*-[01.3000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_listreg       (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_line      [LEN_RECD];
   char        x_reg       = ' ';
   char        x_reglist   [LEN_RECD] = "\"abcdefghijklmnopqrstuvwxyz";
   int         x_count     = 0;
   int         x_row       = 4;
   int         x_col       = 10;
   char       *x_title     = " -r- tab space count cell toplef botrig reach -min-- -max-- -t- ";
   /*---(header)-------------------------*/
   attron    (S_COLOR_TITLE);
   mvprintw  (x_row, x_col, x_title);
   attrset   (0);
   /*---(prepare)------------------------*/
   x_count = strlen (x_reglist);
   /*---(show unnamed)-------------------*/
   /*---(show registers)-----------------*/
   for (i = 0; i < x_count; ++i) {
      ++x_row;
      /*---(lower case)------------------*/
      x_reg = x_reglist [i];
      REG_entry  (x_reg, x_line);
      if (x_line  [6] != '-')  attron (S_COLOR_CURRENT);
      else                     attron (S_COLOR_VISUAL);
      mvprintw  (x_row, x_col, x_line);
      attrset   (0);
   }
   ++x_row;
   /*---(footer)-------------------------*/
   attron    (S_COLOR_TITLE);
   mvprintw  (x_row, x_col, x_title);
   attrset   (0);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.860.081.21]*/ /*-[01.3000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_listtreg      (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_line      [LEN_RECD];
   char        x_reg       = ' ';
   char        x_reglist   [LEN_RECD] = "\"abcdefghijklmnopqrstuvwxyz";
   int         x_count     = 0;
   int         x_row       = 4;
   int         x_col       = 10;
   char       *x_title     = " -r- len -data------------------------------------- -label- beg end  s  ";
   /*---(header)-------------------------*/
   attron    (S_COLOR_TITLE);
   mvprintw  (x_row, x_col, x_title);
   attrset   (0);
   /*---(prepare)------------------------*/
   x_count = strlen (x_reglist);
   /*---(show unnamed)-------------------*/
   /*---(show registers)-----------------*/
   for (i = 0; i < x_count; ++i) {
      ++x_row;
      /*---(lower case)------------------*/
      x_reg = x_reglist [i];
      TREG_entry  (x_reg, x_line);
      if (x_line  [7] != '-')  attron (S_COLOR_CURRENT);
      else                     attron (S_COLOR_VISUAL);
      mvprintw  (x_row, x_col, x_line);
      attrset   (0);
   }
   ++x_row;
   /*---(footer)-------------------------*/
   attron    (S_COLOR_TITLE);
   mvprintw  (x_row, x_col, x_title);
   attrset   (0);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.750.171.21]*/ /*-[01.3000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_listerror     (tCELL *a_cell)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_line      [LEN_RECD];
   char       *x_title     = " no p sq --------------- s ---------------------------------------- ";
   int         x_row       = 4;
   int         x_col       = 10;
   int         x_count     = 0;
   char        rc          = 0;
   /*---(header)-------------------------*/
   attron    (S_COLOR_TITLE);
   mvprintw  (x_row, x_col, x_title);
   attrset   (0);
   /*---(prepare)------------------------*/
   rc = ERROR_entry (a_cell, '*', x_line);
   /*---(show registers)-----------------*/
   while (rc == 0) {
      ++x_row;
      attron (S_COLOR_CURRENT);
      mvprintw  (x_row, x_col, x_line);
      attrset   (0);
      rc = ERROR_entry (a_cell, '-', x_line);
      if (x_row > 10) break;
   }
   /*---(footer)-------------------------*/
   ++x_row;
   attron    (S_COLOR_TITLE);
   mvprintw  (x_row, x_col, x_title);
   attrset   (0);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      screen management                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___SCREEN__________o () { return; }

char         /*-> capture keyboard input during macro [ leaf   [gc.320.012.00]*/ /*-[00.0001.10.--]-*/ /*-[--.---.---.--]-*/
CURS_playback      (void)
{
   char        ch          = ' ';
   IF_MACRO_OFF    return 0;
   IF_MACRO_MOVING nodelay (stdscr, TRUE );
   ch = getch ();
   IF_MACRO_MOVING nodelay (stdscr, FALSE);
   return ch;
}

char         /*-> present screen elements ------------[ leaf   [gc.IC1.021.AS]*/ /*-[05.2001.20.--]-*/ /*-[--.---.---.--]-*/
CURS_main          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         ch          = 0;
   tCELL      *curr        = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(initialize)---------------------*/
   switch (my.layout_formula) {
   case G_FORMULA_DEBUG : my.apos = my.x_full - 29 - 15 -  2;
                          break;
   case G_FORMULA_SMALL : my.apos = my.x_full -  6 -  5 -  2;
                          break;
   case G_FORMULA_TINY  : my.apos = my.x_full -  0 -  0 -  2;
                          break;
   }
   curr    = LOC_cell_at_curr ();
   strncpy (reqs , "+", LEN_RECD);
   strncpy (deps , "+", LEN_RECD);
   strncpy (like , "+", LEN_RECD);
   strncpy (s_mark_list, "+", LEN_RECD);
   strncpy (s_mark_plus, "+", LEN_RECD);
   DEP_disp_reqs  (curr, reqs);
   DEP_disp_pros  (curr, deps);
   DEP_disp_like  (curr, like);
   MARK_list      (s_mark_list);
   MARK_listplus  (s_mark_plus);
   /*---(update cells)-------------------*/
   CURS_formula   (curr);
   CURS_status    (curr);
   CURS_col_head  ();
   CURS_row_head  ();
   CURS_page      ();
   CURS_message   ();
   switch (my.info_win) {
   case G_INFO_BUFS   : CURS_listbufs    ();
                        break;
   case G_INFO_MARK   : CURS_list_mark   ();
                        break;
   case G_INFO_REGS   : CURS_listreg     ();
                        break;
   case G_INFO_TREG   : CURS_listtreg    ();
                        break;
   case G_INFO_CELL   : CURS_info_cell   ();
                        break;
   case G_INFO_ERROR  : CURS_listerror   (curr);
                        break;
   case G_INFO_LAYOUT : CURS_info_layout ();
                        break;
   }
   if (my.menu != MENU_NONE) {
      CURS_menuroot (my.menu);
      if (my.menu != MENU_ROOT)  CURS_menusub  (my.menu);
   }
   /*---(label/keys)---------------------*/
   if (my.layout_formula == G_FORMULA_DEBUG || my.layout_formula == G_FORMULA_SMALL) {
      attron   (S_COLOR_KEYS);
      mvprintw (row_chead, 0, g_cmd);
      attroff  (S_COLOR_KEYS);
   } else {
      mvprintw (row_chead, 0, s_label + 1);
   }
   /*---(cursor pos)---------------------*/
   if (my.info_win != G_INFO_NONE) 
      move ( 4, 10);
   else if (my.menu     != MENU_NONE) 
      move ( 2,  1);
   else if (yVIKEYS_mode_curr() == MODE_SOURCE || yVIKEYS_mode_curr() == MODE_INPUT || yVIKEYS_mode_curr() == SMOD_REPLACE || yVIKEYS_mode_curr() == SMOD_SELECT)
      move (row_formula, s_start + my.cpos - my.bpos);
   else
      move (LOC_row_ypos (CTAB, CROW), LOC_col_xpos (CTAB, CCOL) + LOC_col_width (CTAB, CCOL) - 1);
   /*---(refresh)------------------------*/
   my.info_win = G_INFO_NONE;
   refresh ();
   IF_MACRO_NOT_PLAYING {
      ch = getch ();
      DEBUG_GRAF  yLOG_value   ("key"       , ch);
   }
   /*---(check layout)-------------------*/
   if (ch == KEY_RESIZE) {
      CURS_size ();
      MOVE_horz  ('r');
      MOVE_vert  ('r');
   }
   /*---(complete)--------------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return ch;
}

char         /*-> display an individual cell ---------[ ------ [gc.D70.532.S5]*/ /*-[02.2000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_cell          (int a_col, int a_row, short a_ypos, short a_xpos, short a_wide)
{
   /*---(locals)---------------------------*/
   tCELL      *x_curr      = LOC_cell_at_loc (CTAB, a_col, a_row);
   char        label     [LEN_RECD] = "zzz";
   char        l         [LEN_RECD]  = "";
   /*---(check for merges)-----------------*/
   /*> if (x_curr != NULL && x_curr->a == '+')  return 0;                                 <*/
   /*---(identify cell)--------------------*/
   if (x_curr != NULL) {
      LOC_label  (x_curr, l);
      sprintf    (label, ",%s,", l);
   } else {
      LOC_ref    (CTAB, a_col, a_row, 0, l);
      sprintf    (label, ",%s,", l);
   }
   /*---(current)--------------------------*/
   if      (a_col == CCOL && a_row == CROW)     attron (S_COLOR_CURRENT);
   /*---(visual-range)---------------------*/
   else if (VISU_root     (CTAB, a_col, a_row)) attron (S_COLOR_ROOT   );
   else if (VISU_selected (CTAB, a_col, a_row)) attron (S_COLOR_VISUAL );
   /*---(marks)----------------------------*/
   else if (my.mark_show  == 'y' && strstr (s_mark_list, label) != NULL)  attron (S_COLOR_MARK     );
   /*---(content-based)--------------------*/
   else if (x_curr != NULL) {
      /*---(trouble)--------------------------*/
      if      (x_curr->t == CTYPE_ERROR)        attron (S_COLOR_ERROR  );
      else if (x_curr->t == CTYPE_WARN )        attron (S_COLOR_ERROR  );
      /*---(related)--------------------------*/
      else if (strstr (reqs, label) != NULL)    attron (S_COLOR_REQS   );
      else if (strstr (deps, label) != NULL)    attron (S_COLOR_PROS   );
      else if (strstr (like, label) != NULL)    attron (S_COLOR_LIKE   );
      /*---(pointers)-------------------------*/
      else if (x_curr->t == CTYPE_RANGE)        attron (S_COLOR_POINTER);
      else if (x_curr->t == CTYPE_ADDR )        attron (S_COLOR_POINTER);
      /*---(numbers)--------------------------*/
      else if (x_curr->t == CTYPE_NUM  )        attron (S_COLOR_NUMBER );
      else if (x_curr->t == CTYPE_FORM ) {
         if   (x_curr->nrequire < 5)            attron (S_COLOR_FSIMPLE);
         else                                   attron (S_COLOR_FDANGER);
      }
      else if (x_curr->t == CTYPE_FLIKE)        attron (S_COLOR_FLIKE  );
      /*---(strings)--------------------------*/
      else if (x_curr->t == CTYPE_STR  )        attron (S_COLOR_STRING );
      else if (x_curr->t == CTYPE_MOD  ) {
         if   (x_curr->nrequire < 5)            attron (S_COLOR_FSTRING);
         else                                   attron (S_COLOR_FSTRDAG);
      }
      else if (x_curr->t == CTYPE_MLIKE)        attron (S_COLOR_MLIKE  );
      /*---(constants)------------------------*/
      else if (x_curr->t == CTYPE_BLANK)        attron (S_COLOR_NULL   );
      else                                      attron (S_COLOR_STRING );
   }
   /*---(display cell)---------------------*/
   if (x_curr == NULL || x_curr->p == NULL) 
      mvprintw (a_ypos, a_xpos, "%-*.*s", a_wide, a_wide, g_empty);
   else
      mvprintw (a_ypos, a_xpos, "%-*.*s", a_wide, a_wide, x_curr->p);
   /*---(highlight off)--------------------*/
   attrset (0);
   /*---(complete)-------------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      private functions                       ----===*/
/*====================------------------------------------====================*/

PRIV int     /*-> tbd --------------------------------[ ------ [gn.860.371.25]*/ /*-[03.0000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_line          (int a_ch, int a_brow, int a_erow)
{
   /*---(locals)-----------+-----------+-*/
   int         y_cur       = 0;
   int         x_cur       = 0;
   int         x_avail     = my.x_full - 1;
   int         i           = 0;
   short       x_ypos      = 0;
   short       x_xpos      = 0;
   int         x_wide      = 0;
   for (y_cur = a_brow; y_cur <= a_erow; ++y_cur) {
      /*---(prepare)---------------------*/
      a_ch   += LOC_row_height (CTAB, y_cur);
      x_ypos  = LOC_row_ypos   (CTAB, y_cur);
      /*---(cycle normal columns)--------*/
      for (x_cur = ECOL; x_cur >= BCOL; --x_cur) {
         x_xpos  = LOC_col_xpos   (CTAB, x_cur);
         x_wide  = LOC_col_width  (CTAB, x_cur);
         CURS_cell (x_cur, y_cur, x_ypos, x_xpos, x_wide);
      }
      /*---(cycle locked columns)--------*/
      if (FR_COL == 'y') {
         for (x_cur = FR_ECOL; x_cur >= FR_BCOL; --x_cur) {
            x_xpos  = LOC_col_xpos   (CTAB, x_cur);
            x_wide  = LOC_col_width  (CTAB, x_cur);
            CURS_cell (x_cur, y_cur, x_ypos, x_xpos, x_wide);
         }
      }
      /*---(fill in at end)--------------*/
      x_xpos  = LOC_col_xpos (CTAB, ECOL) + LOC_col_width (CTAB, ECOL);
      x_wide = x_avail - x_xpos + 1;
      if (x_wide > 0)  CURS_cell (ECOL + 1, y_cur, x_ypos, x_xpos, x_wide);
   }
   /*---(complete)-----------------------*/
   return a_ch;
}

PRIV char    /*-> redisplay all cell data on screen --[ ------ [gz.531.081.12]*/ /*-[01.1000.01.--]-*/ /*-[--.---.---.--]-*/
CURS_page          (void)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   int         y_cur       = 0;
   int         x_cur       = 0;
   int       end   = 0;
   int       avail = my.x_full - 1;
   int       left  = 0;
   int       xmax  = 0;
   int         ch          = 0;
   int         i           = 0;
   /*---(cycle rows)---------------------*/
   if (FR_ROW == 'y')  ch = CURS_line (ch, FR_BROW, FR_EROW);
   ch = CURS_line (ch, BROW, EROW);
   /*---(fill in bottom)-----------------*/
   for (y_cur = ch; y_cur < my.y_avail; ++y_cur) {
      mvprintw (LOC_row_ypos (CTAB, y_cur), 5, "%*.*s", my.x_avail, my.x_avail, g_empty);
   }
   /*---(complete)------------------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       support functions                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___SUPPORT_________o () { return; }

char         /*-> resize screen ----------------------[ leaf   [gz.851.021.40]*/ /*-[00.0000.12.--]-*/ /*-[--.---.---.--]-*/
CURS_size         (void)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------------------------*/
   int    y;
   int    x;
   /*---(get dimensions)--------------*/
   getmaxyx (stdscr, y, x);
   /*---(set positions)---------------*/
   row_buffers  = 0;
   row_formula  = 0;
   row_chead    = 1;
   row_main     = 2;
   s_status_row   = y - 2;
   if (my.layout_status  != G_STATUS_HIDE ) s_status_size  = 1;
   else                                     s_status_size  = 0;
   s_command_row  = y - 1;
   if (my.layout_command != G_COMMAND_HIDE) s_command_size = 1;
   else                                     s_command_size = 0;
   col_header   = 0;
   col_far      = x - 1;
   /*---(critical numbers)------------*/
   my.y_full    = y;
   my.y_avail   = y - row_main - s_status_size - s_command_size;
   /*---(column markers)--------------*/
   my.x_full    =  x;
   my.x_left    =  5;
   my.x_right   =  0;
   my.x_avail   = x - my.x_left - my.x_right;
   /*---(clear the screen)------------*/
   clear();
   touchwin (stdscr);
   /*---(complete)--------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> initiate curses --------------------[ ------ [gz.421.001.02]*/ /*-[00.0000.10.--]-*/ /*-[--.---.---.--]-*/
CURS_begin         (void)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(initialize)------------------*/
   initscr();       /* fire up ncurses with a default screen (stdscr)         */
   raw();           /* read key-by-key rather than waiting for \n (raw mode)  */
   noecho();        /* don't automatically echo keypresses to the screen      */
   ESCDELAY = 0;    /* so escape responds immediately                         */
   /*---(get window size)-------------*/
   CURS_size   ();
   /*---(colors)----------------------*/
   start_color ();
   use_default_colors();
   COLOR_init  ();
   /*---(complete)--------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> shutdown curses --------------------[ leaf   [gz.211.001.00]*/ /*-[00.0000.10.--]-*/ /*-[--.---.---.--]-*/
CURS_end           (void)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   endwin();        /* shut down ncurses                                      */
   /*---(complete)--------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*============================----end-of-source---============================*/
