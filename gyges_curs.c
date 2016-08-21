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

#define     MAX_MENU       500
typedef struct cMENU  tMENU;
struct  cMENU {  /* two level menu only, none of that complex shit            */
   char     a_one;                          /* shortcut for menu              */
   char     a_menu      [LEN_STR];          /* top level menu item            */
   char     a_group;                        /* selection group                */
   char     a_two;                          /* shortcut for submenu           */
   char     a_name      [LEN_STR];          /* name of menu item              */
   char     a_desc      [LEN_STR];          /* descripion, example, alternate */
};
tMENU       menus       [MAX_MENU] = {
   /* one  ---menu---------- grp -sc-  ---name----------  ---example------------ */
   {  'f', "format"         ,  1, '<', "left"           , "test                " },
   {  ' ', ""               ,  1, '|', "center"         , "        test        " },
   {  ' ', ""               ,  1, '>', "right"          , "               test " },
   {  ' ', ""               ,  1, '[', "left bracket"   , "[test              ]" },
   {  ' ', ""               ,  1, '^', "center bracket" , "[       test       ]" },
   {  ' ', ""               ,  1, ']', "right bracket"  , "[              test]" },
   {  ' ', ""               ,  1, '{', "right brace"    , "[  test            ]" },
   {  ' ', ""               ,  1, '}', "left brace"     , "[            test  ]" },
   {  ' ', ""               ,  2, ' ', "fill space"     , "[  test            ]" },
   {  ' ', ""               ,  2, '-', "fill dash"      , "[--test------------]" },
   {  ' ', ""               ,  2, '=', "fill equal"     , "[==test============]" },
   {  ' ', ""               ,  2, '_', "fill under"     , "[__test____________]" },
   {  ' ', ""               ,  2, '.', "fill period"    , "[..test............]" },
   {  ' ', ""               ,  2, '+', "fill plus"      , "[++test++++++++++++]" },
   {  ' ', ""               ,  3, 'i', "integer"        , "            1234567 " },
   {  ' ', ""               ,  3, 'r', "real"           , "            987.541 " },
   {  ' ', ""               ,  3, 'g', "general"        , "         987.541234 " },
   {  ' ', ""               ,  3, ',', "comma"          , "          1,234,567 " },
   {  ' ', ""               ,  3, 's', "comma/sign"     , "         +1,234,567 " },
   {  ' ', ""               ,  3, 'a', "accounting"     , "        (1,234,567) " },
   {  ' ', ""               ,  3, '$', "currency"       , "         $1,234,567 " },
   {  ' ', ""               ,  3, '%', "percent"        , "                15% " },
   {  ' ', ""               ,  3, 'p', "point/bullet"   , "                 5) " },
   {  ' ', ""               ,  3, '#', "technical"      , "  123,456.789'032 + " },
   {  ' ', ""               ,  3, 'e', "exponential"    , "          1.235e+05 " },
   {  ' ', ""               ,  3, 'E', "exp divided"    , "       +1.235 e +05 " },
   {  ' ', ""               ,  3, 'x', "hexidecimal"    , "              xaf68 " },
   {  ' ', ""               ,  3, 'X', "hex divided"    , "            x'af'68 " },
   {  ' ', ""               ,  3, 'b', "binary"         , "          b10101111 " },
   {  ' ', ""               ,  3, 'B', "binary divided" , "        b'1010'1111 " },
   {  ' ', ""               ,  3, 'o', "octal"          , "               o257 " },
   {  ' ', ""               ,  3, 'O', "octal"          , "              o'257 " },
   {  ' ', ""               ,  3, 't', "time (24h)"     , "              13:26 " },
   {  ' ', ""               ,  3, 'd', "date"           , "        2014-Dec-15 " },
   {  ' ', ""               ,  3, 'T', "timestamp"      , "  14.12.15.13.26.15 " },
   {  ' ', ""               ,  3, 'D', "date/time"      , "  2014-Dec-15 13:26 " },
   {  ' ', ""               ,  4, '0', "0 decimals"     , "                  0 " },
   {  ' ', ""               ,  4, '1', "1 decimals"     , "                0.1 " },
   {  ' ', ""               ,  4, '2', "2 decimals"     , "               0.12 " },
   {  ' ', ""               ,  4, '3', "3 decimals"     , "              0.123 " },
   {  ' ', ""               ,  4, '4', "4 decimals"     , "             0.1234 " },
   {  ' ', ""               ,  4, '5', "5 decimals"     , "            0.12345 " },
   {  ' ', ""               ,  4, '6', "6 decimals"     , "           0.123456 " },
   {  ' ', ""               ,  4, '7', "7 decimals"     , "          0.1234567 " },
   {  ' ', ""               ,  4, '8', "8 decimals"     , "         0.12345678 " },
   {  ' ', ""               ,  4, '9', "9 decimals"     , "        0.123456789 " },
   {  '~', "end-of-menu"    ,  1, ' ', ""               , ""                     },
   {  ' ', ""               ,  1, ' ', ""               , ""                     },
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
   { "curr"     , ' ', "current cell"                                       , 'b' , 'y' , 'b',   0 },
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
char        sta_type  = 'v';
char        sta_error = '-';

static char s_label   [10]  = "";
static int  s_start         = 0;
static int  s_space         = 0;


/*====================------------------------------------====================*/
/*===----                      color configuration                     ----===*/
/*====================------------------------------------====================*/
PRIV void  o___COLORS__________o () { return; }
int         s_color_try = 0;
int         s_color_err = 0;

int
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

char
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

int
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

char
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

char
CURS_formula       (tCELL *a_curr)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------------------------*/
   char      msg[500]  = "";                   /* temporary display message   */
   int       len       = 0;
   int       w         = 0;
   char      x_zilch   = 216;
   int       x_beg     = 0;
   int       x_end     = 0;
   int       x_gap     = 0;
   int       x_len     = 0;
   /*---(clear line)---------------------*/
   if (sta_error == 'y')  attron (S_COLOR_TITLEE);
   else                   attron (S_COLOR_TITLE);
   mvprintw (row_formula, 0, "%*.*s", my.x_full, my.x_full, empty);
   /*---(location)-----------------------*/
   /*> strcpy(label, tab->cols[CCOL].l);                                         <* 
    *> if (label[0] == '-') label[0] = ' ';                                           <*/
   /*> mvprintw (row_formula,  0, "%c%c [%s%d ]", mode, (VISU_islive()) ? 'v' : ' ', label, CROW + 1);   <*/
   /*---(label)------------------------------*/
   if (a_curr != NULL)    strcpy  (s_label, a_curr->label);
   else                   LOC_ref (CTAB, CCOL, CROW, 0, s_label);
   /*---(length)-----------------------------*/
   len = strlen (g_contents);
   /*---(display)----------------------------*/
   switch (my.scrn) {
   case SCRN_DEBUG :
      /*---(1st 12 chars)---*/
      mvprintw (row_formula,  0, "%c  %c%c %-6.6s", MODE_curr(), (VISU_islive()) ? 'v' : ' ', my.reg_curr, s_label);
      /*---(2nd 13 chars)---*/
      if (a_curr != NULL)  mvprintw (row_formula, 12, " %02d %c %c %c %c  "   , tab->cols[CCOL].w, a_curr->t, a_curr->f, a_curr->d, a_curr->a);
      else                 mvprintw (row_formula, 12, " %02d                []", tab->cols[CCOL].w);
      /*---(3rd  5 chars)---*/
      mvprintw (row_formula, 25, "%4d", len);
      /*---(4th 14 chars)---*/
      mvprintw (row_formula, my.x_full - 14, " %s of gyges", VER_NUM);
      s_start  = 30;
      break;
   case SCRN_SMALL :
      /*---(1st  6 chars)---*/
      mvprintw (row_formula,  0, "%-6.6s", s_label);
      /*---(2nd  5 chars)---*/
      mvprintw (row_formula, my.x_full - 5, " %s", VER_NUM);
      s_start  =  7;
      break;
   case SCRN_TINY  :
      s_start  =  1;
      break;
   }
   s_space  = my.apos;
   /*---(set color)------------------------*/
   attrset (0);
   switch (MODE_curr()) {
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
   mvprintw (row_formula, s_start, "%*.*s", s_space, s_space, empty);
   snprintf (msg, 500, "%s",    g_contents);
   mvprintw (row_formula, s_start, "%-*.*s", s_space, s_space, msg + my.bpos);
   /*---(highlight off)------------------*/
   attrset (0);
   /*---(show selection on top)------------*/
   if ((MODE_curr() == SMOD_SELECT || MODE_curr() == SMOD_TEXTREG) && SELC_islive) {
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
   sprintf (msg, "%c", 216);
   if      (my.npos == 0)            mvprintw (row_formula, s_start - 1, msg);
   else if (my.bpos == 0)            mvprintw (row_formula, s_start - 1, " ");
   else                              mvprintw (row_formula, s_start - 1, "<");
   if      (my.epos == my.npos - 1)  mvprintw (row_formula, s_start + s_space, " ");
   else                              mvprintw (row_formula, s_start + s_space, ">");
   attrset (0);
   /*---(complete)-----------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
CURS_status        (tCELL *a_curr)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   int         l           = 0;             /* string length                  */
   int         i           = 0;             /* iterator -- keys               */
   char        msg[500]  = "";                   /* temporary display message   */
   char        rpn[MAX_STR] = "";
   switch (sta_type) {
   case 'c' : /* cell details */
      if (a_curr != NULL) {
         strncpy (rpn , "+", MAX_STR);
         if (a_curr != NULL && a_curr->rpn != NULL) strncpy (rpn, a_curr->rpn, MAX_STR);
         snprintf (msg, 500, "[ rpn %-20.20s ][ reqs=%-20.20s ][ pros=%-20.20s ][ like=%-20.20s ]", rpn, reqs, deps, like);
      }
      break;
   case 'd' : /* cell details */
      if (a_curr != NULL) {
         snprintf (msg, 500, "[ reqs=%-40.40s ][ pros=%-40.40s ]", reqs, deps);
      }
      break;
   case 'r' : /* buffer contents */
      if (a_curr != NULL && a_curr->rpn != NULL) strncpy (rpn, a_curr->rpn, MAX_STR);
      else                                       strncpy (rpn, "((null))", MAX_STR);
      snprintf (msg, 500, "[ %-100.100s ]", rpn);
      break;
   case '"' : /* register contents */
      REG_list     (my.reg_curr  , bufc);
      snprintf (msg, 500, "[ reg %-100.100s ]", bufc);
      break;
   case 'k' : /* keylog */
      KEYS_status (msg);
      break;
   case 'u' :
      snprintf (msg, 500, "[ nhist : %4d, chist : %4d, top : %s ]", nhist, chist, hist [chist].act);
      break;
   case 'm' :
      snprintf (msg, 500, "marks (%c,%c,%c,%c) %s", my.mark_show, my.mark_head, my.mark_save, my.mark_tail, my.mark_plus);
      break;
   case 'M' :
      MODE_list (msg);
      break;
   case 't' : /* text register */
      TREG_entry (REG_CURR, msg);
      break;
   case 'v' : /* file version */
   default  :
      if (ver_ctrl == 'y')  snprintf (msg, 500, "[ file : %-40.40s ][ %dc x %dr ][ version : %-5.5s, %-40.40s ]", my.f_name, tab->ncol, tab->nrow, ver_num, ver_txt);
      else                  snprintf (msg, 500, "[ file : %-40.40s ][ %dc x %dr ][ version : not  controlled                                ]", my.f_name, tab->ncol, tab->nrow);
      break;
   }
   if (sta_error == 'y')  attron (S_COLOR_STATUSE);
   else                   attron (S_COLOR_STATUS);
   mvprintw(row_status, 0, "%*.*s", my.x_full, my.x_full, empty);
   mvprintw(row_status, 0, msg);
   if (sta_error == 'y')  attroff(S_COLOR_STATUSE);
   else                   attroff(S_COLOR_STATUS);
   sta_error = '-';
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
CURS_message       (void)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   attron  (S_COLOR_MESSAGE);
   mvprintw (row_message, 0, "%*.*s", my.x_full, my.x_full, empty);
   mvprintw (row_message, 0, my.message);
   attroff (S_COLOR_MESSAGE);
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> update the column labels --------------[ ------ [ ------ ]-*/
CURS_colhead       (void)
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
   if (tab->froz_col == 'y') {
      for (i = tab->froz_bcol; i <=  tab->froz_ecol; ++i) {
         if (i >= NCOL) break;
         /*---(prepare)---------------------*/
         w     = tab->cols[i].w;
         wa    = w - 4;
         cw    = tab->cols[i].x + w;
         strcpy(label, tab->cols[i].l);
         /*---(output)----------------------*/
         snprintf(msg, 500, "\[%*.*s%s\]", wa, wa, dashes, label);
         if      (i == CCOL          )  attron  (S_COLOR_HEADY   );
         else if (i <= tab->froz_ecol)  attron  (S_COLOR_HEADL   );
         else if (tab->cols[i].c >  0)  attron  (S_COLOR_HEADF   );
         else                           attron  (S_COLOR_HEADN   );
         mvprintw (row_chead, tab->cols[i].x, msg);
         if      (i == CCOL          )  attroff (S_COLOR_HEADY   );
         else if (i <= tab->froz_ecol)  attroff (S_COLOR_HEADL   );
         else if (tab->cols[i].c >  0)  attroff (S_COLOR_HEADF   );
         else                           attroff (S_COLOR_HEADN   );
      }
   }
   /*---(process oolumns)----------------*/
   for (i = BCOL; i <=  ECOL; ++i) {
      if (i >= NCOL) break;
      /*---(prepare)---------------------*/
      w     = tab->cols[i].w;
      wa    = w - 4;
      cw    = tab->cols[i].x + w;
      strcpy(label, tab->cols[i].l);
      /*---(output)----------------------*/
      snprintf(msg, 500, "\[%*.*s%s\]", wa, wa, dashes, label);
      if      (i == CCOL          )  attron  (S_COLOR_HEADY   );
      else if (tab->cols[i].c >  0)  attron  (S_COLOR_HEADF   );
      else                           attron  (S_COLOR_HEADN   );
      mvprintw (row_chead, tab->cols[i].x, msg);
      if      (i == CCOL          )  attroff (S_COLOR_HEADY   );
      else if (tab->cols[i].c >  0)  attroff (S_COLOR_HEADF   );
      else                           attroff (S_COLOR_HEADN   );
   }
   /*---(fill in right side)-------------*/
   if (cw < (my.x_full - my.x_right)) {
      w     = my.x_full - cw;
      wa    = w - 4;
      strcpy(label, tab->cols[ECOL + 1].l);
      if (ECOL < NCOL - 1){
         if      (w == 1) snprintf(msg, 500, ">");
         else if (w == 2) snprintf(msg, 500, "\[>");
         else if (w == 3) snprintf(msg, 500, "\[->");
         else             snprintf(msg, 500, "\[%*.*s%s>", wa, wa, dashes, label);
      } else              snprintf(msg, 500, "%*.*s ", w, w, empty);
      if (tab->cols[ECOL + 1].c == 0)  attron  (S_COLOR_HEADN   );
      mvprintw (row_chead, cw, msg);
      if (tab->cols[ECOL + 1].c == 0)  attroff (S_COLOR_HEADN   );
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> update row labels ---------------------[ ------ [ ------ ]-*/
CURS_rowhead       (void)
{
   /*---(locals)-----------------------------*/
   int       i         = 0;                    /* iterator -- rows            */
   int       j         = 0;                    /* iterator -- rows            */
   int       h         = 0;                    /* row height                  */
   int       ch        = row_main;             /* cumulative column width     */
   /*---(header)-----------------------------*/
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(process locked rows)----------------*/
   if (tab->froz_row == 'y') {
      for (i = tab->froz_brow; i <=  tab->froz_erow; ++i) {
         if (i >= NROW)  break;
         /*---(prepare)----------------------------*/
         h     = tab->rows[i].h;
         ch   += h;
         if      (i == CROW          )  attron  (S_COLOR_HEADY   );
         else if (i <= tab->froz_erow)  attron  (S_COLOR_HEADL   );
         else if (tab->rows[i].c >  0)  attron  (S_COLOR_HEADF   );
         else                           attron  (S_COLOR_HEADN   );
         for (j = 0; j < h; ++j) {
            /*> if (ch + j > my.y_avail)  break;                                         <*/
            switch (j) {
            case  0 : mvprintw (tab->rows[i].y    , 0, "%4d", i + 1);   break;
            default : mvprintw (tab->rows[i].y + j, 0, "   .");         break;
            }
         }
         if      (i == CROW          )  attroff (S_COLOR_HEADY   );
         else if (i <= tab->froz_erow)  attroff (S_COLOR_HEADL   );
         else if (tab->rows[i].c >  0)  attroff (S_COLOR_HEADF   );
         else                           attroff (S_COLOR_HEADN   );
      }
   }
   /*---(process rows)-----------------------*/
   for (i = BROW; i <=  EROW; ++i) {
      if (i >= NROW)  break;
      /*---(prepare)----------------------------*/
      h     = tab->rows[i].h;
      ch   += h;
      if      (i == CROW          )  attron  (S_COLOR_HEADY   );
      else if (tab->rows[i].c >  0)  attron  (S_COLOR_HEADF   );
      else                           attron  (S_COLOR_HEADN   );
      for (j = 0; j < h; ++j) {
         /*> if (ch + j > my.y_avail)  break;                                         <*/
         switch (j) {
         case  0 : mvprintw (tab->rows[i].y    , 0, "%4d", i + 1);   break;
         default : mvprintw (tab->rows[i].y + j, 0, "   .");         break;
         }
      }
      if      (i == CROW          )  attroff (S_COLOR_HEADY   );
      else if (tab->rows[i].c >  0)  attroff (S_COLOR_HEADF   );
      else                           attroff (S_COLOR_HEADN   );
   }
   /*---(complete)---------------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
CURS_listmark      (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_line      [MAX_STR];
   char        x_label     [10];
   char        x_mark      = ' ';
   char        x_marklist  [MAX_STR] = "'abcdefghijklmnopqrstuvwxyz";
   /*---(header)-------------------------*/
   attron (S_COLOR_TITLE);
   mvprintw   ( 3, 10, " -m- --fixed---      -m- --moving-- ");
   attrset (0);
   /*---(show marks)---------------------*/
   for (i = 'a'; i <= 'z'; ++i) {
      /*---(lower case)------------------*/
      MARK_label (i, x_label);
      sprintf    (x_line, "  %c : %-8.8s  ", i, x_label);
      if (x_label [0] != '\0')  attron (S_COLOR_CURRENT);
      else                      attron (S_COLOR_VISUAL);
      mvprintw   ( 4 + (i - 'a'), 10, x_line);
      attrset (0);
      /*---(separator)-------------------*/
      attron (S_COLOR_TITLE);
      mvprintw   ( 4 + (i - 'a'), 26, "    ");
      attrset (0);
      /*---(upper case)------------------*/
      MARK_label (toupper (i), x_label);
      sprintf    (x_line, "  %c : %-8.8s  ", toupper (i), x_label);
      if (x_label [0] != '\0')  attron (S_COLOR_CURRENT);
      else                      attron (S_COLOR_VISUAL);
      mvprintw   ( 4 + (i - 'a'), 30, x_line);
      attrset (0);
      /*---(done)------------------------*/
   }
   /*---(footer)-------------------------*/
   attron (S_COLOR_TITLE);
   mvprintw   ( 3 + 27, 10, " -m- --fixed---      -m- --moving-- ");
   attrset (0);
   /*---(complete)-----------------------*/
   return 0;
}

char
CURS_listreg       (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_line      [MAX_STR];
   char        x_reg       = ' ';
   char        x_reglist   [MAX_STR] = "\"abcdefghijklmnopqrstuvwxyz";
   int         x_count     = 0;
   int         x_row       = 4;
   int         x_col       = 10;
   /*---(header)-------------------------*/
   attron    (S_COLOR_TITLE);
   mvprintw  (x_row, x_col, " -r- tab space count cell toplef botrig reach -min-- -max-- -t- ");
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
   mvprintw  (x_row, x_col, " -r- tab space count cell toplef botrig reach -min-- -max-- -t- ");
   attrset   (0);
   /*---(complete)-----------------------*/
   return 0;
}

char
CURS_listtreg      (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_line      [MAX_STR];
   char        x_reg       = ' ';
   char        x_reglist   [MAX_STR] = "\"abcdefghijklmnopqrstuvwxyz";
   int         x_count     = 0;
   int         x_row       = 4;
   int         x_col       = 10;
   /*---(header)-------------------------*/
   attron    (S_COLOR_TITLE);
   mvprintw  (x_row, x_col, " -r- len -data------------------------------------- -label- beg end  s  ");
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
   mvprintw  (x_row, x_col, " -r- len -data------------------------------------- -label- beg end  s  ");
   attrset   (0);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      screen management                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___SCREEN__________o () { return; }

int                /* PURPOSE : presents the outer window frame elements      */
CURS_main          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         ch          = 0;
   tCELL      *curr        = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(initialize)---------------------*/
   switch (my.scrn) {
   case SCRN_DEBUG : my.apos = my.x_full - 29 - 15 -  2;
                     break;
   case SCRN_SMALL : my.apos = my.x_full -  6 -  5 -  2;
                     break;
   case SCRN_TINY  : my.apos = my.x_full -  0 -  0 -  2;
                     break;
   }
   curr    = tab->sheet[CCOL][CROW];
   strncpy (reqs , "+", MAX_STR);
   strncpy (deps , "+", MAX_STR);
   strncpy (like , "+", MAX_STR);
   strncpy (my.mark_list, "+", MAX_STR);
   strncpy (my.mark_plus, "+", MAX_STR);
   DEP_requires   (curr, reqs);
   DEP_provides   (curr, deps);
   DEP_like       (curr, like);
   MARK_list      (my.mark_list);
   MARK_listplus  (my.mark_plus);
   /*---(update cells)-------------------*/
   CURS_formula   (curr);
   CURS_status    (curr);
   CURS_message   ();
   CURS_colhead   ();
   CURS_rowhead   ();
   CURS_page      ();
   switch (my.info_win) {
   case '\'' : CURS_listmark ();
               break;
   case '"'  : CURS_listreg  ();
               break;
   case 't'  : CURS_listtreg ();
               break;
   }
   /*---(command)------------------------*/
   if (my.scrn == SCRN_DEBUG || my.scrn == SCRN_SMALL) {
      attron   (S_COLOR_KEYS);
      mvprintw (row_chead, 0, cmd);
      attroff  (S_COLOR_KEYS);
   } else {
      mvprintw (row_chead, 0, s_label + 1);
   }
   /*---(cursor pos)---------------------*/
   if (my.info_win != '-') 
      move ( 4, 10);
   else if (MODE_curr() == MODE_SOURCE || MODE_curr() == MODE_INPUT || MODE_curr() == SMOD_REPLACE || MODE_curr() == SMOD_SELECT)
      move (row_formula, s_start + my.cpos - my.bpos);
   else
      move (tab->rows[CROW].y, tab->cols[CCOL].x + tab->cols[CCOL].w - 1);
   /*---(refresh)------------------------*/
   my.info_win = '-';
   refresh ();
   ch = getch ();
   DEBUG_GRAF  yLOG_value   ("key"       , ch);
   /*> if (ch == 3) {                                                                 <* 
    *>    endwin();                                                                   <* 
    *>    exit(-1);                                                                   <* 
    *> }                                                                              <*/
   if (ch == KEY_RESIZE) {
      CURS_size ();
      MOVE_horz  ('r');
      MOVE_vert  ('r');
   }
   /*---(complete)--------------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return ch;
}

char               /* PURPOSE : display an individual cell                    */
CURS_cell          (int a_col, int a_row)
{
   /*---(locals)---------------------------*/
   tCELL    *curr      = tab->sheet[a_col][a_row];
   tCELL    *next      = NULL;
   char      label[MAX_STR] = "zzz";
   char      l[MAX_STR]  = "";
   int       i         = 0;
   uint      xmax      = 0;
   int       xcol      = 0;
   char      xdisp [MAX_STR];
   /*---(check for merges)-----------------*/
   /*> if (curr != NULL && curr->a == '+')  return 0;                                 <*/
   /*---(identify cell)--------------------*/
   if (curr != NULL) {
      LOC_label  (curr, l);
      sprintf    (label, ",%s,", l);
   } else {
      LOC_ref    (CTAB, a_col, a_row, 0, l);
      sprintf    (label, ",%s,", l);
   }
   /*---(current)--------------------------*/
   if      (a_col == CCOL && a_row == CROW)     attron (S_COLOR_CURRENT);
   /*---(visual-range)---------------------*/
   else if (VISU_root     (CTAB, a_col, a_row))  attron (S_COLOR_ROOT   );
   else if (VISU_selected (CTAB, a_col, a_row))  attron (S_COLOR_VISUAL );
   /*---(marks)----------------------------*/
   else if (my.mark_show  == 'y' && strstr (my.mark_list, label) != NULL)  attron (S_COLOR_MARK     );
   /*---(content-based)--------------------*/
   else if (curr != NULL) {
      /*---(trouble)--------------------------*/
      if      (curr->t == CTYPE_ERROR)          attron (S_COLOR_ERROR  );
      else if (curr->t == CTYPE_WARN )          attron (S_COLOR_ERROR  );
      /*---(related)--------------------------*/
      else if (strstr (reqs, label) != NULL)    attron (S_COLOR_REQS   );
      else if (strstr (deps, label) != NULL)    attron (S_COLOR_PROS   );
      else if (strstr (like, label) != NULL)    attron (S_COLOR_LIKE   );
      /*---(pointers)-------------------------*/
      else if (curr->t == CTYPE_RANGE)          attron (S_COLOR_POINTER);
      else if (curr->t == CTYPE_ADDR )          attron (S_COLOR_POINTER);
      /*---(numbers)--------------------------*/
      else if (curr->t == CTYPE_NUM  )          attron (S_COLOR_NUMBER );
      else if (curr->t == CTYPE_FORM ) {
         if   (curr->nrequire < 5)              attron (S_COLOR_FSIMPLE);
         else                                   attron (S_COLOR_FDANGER);
      }
      else if (curr->t == CTYPE_FLIKE)          attron (S_COLOR_FLIKE  );
      /*---(strings)--------------------------*/
      else if (curr->t == CTYPE_STR  )          attron (S_COLOR_STRING );
      else if (curr->t == CTYPE_MOD  ) {
         if   (curr->nrequire < 5)              attron (S_COLOR_FSTRING);
         else                                   attron (S_COLOR_FSTRDAG);
      }
      else if (curr->t == CTYPE_MLIKE)          attron (S_COLOR_MLIKE  );
      /*---(constants)------------------------*/
      else if (curr->t == CTYPE_BLANK)          attron (S_COLOR_NULL   );
      else                                      attron (S_COLOR_STRING );
   }
   /*---(check max width)------------------*/
   /*> xmax = tab->cols[a_col].w;                                                     <* 
    *> if (curr != NULL) {                                                            <* 
    *>    xcol = curr->col + 1;                                                       <* 
    *>    next = tab->sheet[xcol][a_row];                                             <* 
    *>    while (next != NULL && xcol <= ECOL && next->t == CTYPE_MERGE) {            <* 
    *>       xmax += tab->cols[next->col].w;                                          <* 
    *>       ++xcol;                                                                  <* 
    *>       next = tab->sheet[xcol][a_row];                                          <* 
    *>    }                                                                           <* 
    *> }                                                                              <*/
   /*---(check for current)----------------*/
   /*> if (curr!= NULL && a_row == CROW) {                                            <* 
    *>    xcol = curr->col + 1;                                                       <* 
    *>    next = tab->sheet[xcol][a_row];                                             <* 
    *>    while  (next != NULL) {                                                     <* 
    *>       if (next->t != CTYPE_MERGE) break;                                       <* 
    *>       if (xcol == CCOL) {                                                      <* 
    *>          attron (S_COLOR_CURRENT);                                             <* 
    *>          break;                                                                <* 
    *>       }                                                                        <* 
    *>       ++xcol;                                                                  <* 
    *>       next = tab->sheet[xcol][a_row];                                          <* 
    *>    }                                                                           <* 
    *> }                                                                              <*/
   /*---(display cell)---------------------*/
   if (curr == NULL || curr->p == NULL) 
      mvprintw (tab->rows[a_row].y + i, tab->cols[a_col].x, "%*.*s", tab->cols[a_col].w, tab->cols[a_col].w, empty);
   else
      mvprintw (tab->rows[a_row].y + i, tab->cols[a_col].x, curr->p);
   /*> for (i = 0; i < tab->rows[a_row].h; ++i) {                                                                                <* 
    *>    if (curr == NULL || curr->p == NULL) {                                                                                 <* 
    *>       mvprintw (tab->rows[a_row].y + i, tab->cols[a_col].x, "%*.*s", tab->cols[a_col].w, tab->cols[a_col].w, empty);      <* 
    *>    } else {                                                                                                               <* 
    *>       if (i == 0) {                                                                                                       <* 
    *>          sprintf (xdisp, "%-*.*s", xmax, xmax, curr->p);                                                                  <* 
    *>          if (xmax < strlen(curr->p)) xdisp[xmax - 1] = '>';                                                               <* 
    *>          mvprintw (tab->rows[a_row].y + i, tab->cols[a_col].x, xdisp);                                                    <* 
    *>       } else {                                                                                                            <* 
    *>          mvprintw (tab->rows[a_row].y + i, tab->cols[a_col].x, "%*.*s", tab->cols[a_col].w, tab->cols[a_col].w, empty);   <* 
    *>       }                                                                                                                   <* 
    *>    }                                                                                                                      <* 
    *> }                                                                                                                         <*/
   /*---(highlight off)--------------------*/
   attrset (0);
   /*---(complete)-------------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      private functions                       ----===*/
/*====================------------------------------------====================*/

PRIV char       /* PURPOSE : redisplay all cell data on the screen --------*/
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
   if (tab->froz_row == 'y') {
      for (y_cur = tab->froz_brow; y_cur <= tab->froz_erow; ++y_cur) {
         /*---(prepare)---------------------*/
         ch += tab->rows[y_cur].h;
         /*---(cycle normal columns)-----------*/
         for (x_cur = ECOL; x_cur >= BCOL; --x_cur) {
            CURS_cell (x_cur, y_cur);
         }
         /*---(cycle locked columns)-----------*/
         if (tab->froz_col == 'y') {
            for (x_cur = tab->froz_ecol; x_cur >= tab->froz_bcol; --x_cur) {
               CURS_cell (x_cur, y_cur);
            }
         }
         /*---(fill in at end)-----------*/
         end  = tab->cols[ECOL].x + tab->cols[ECOL].w;
         left = avail - end + 1;
         if (left > 0) {
            for (i = 0; i < tab->rows[y_cur].h; ++i) {
               mvprintw (tab->rows[y_cur].y + i, end, "%*.*s", left, left, empty);
            }
         }
      }
   }
   /*---(cycle rows)---------------------*/
   for (y_cur = BROW; y_cur <= EROW; ++y_cur) {
      /*---(prepare)---------------------*/
      ch += tab->rows[y_cur].h;
      /*---(cycle normal columns)-----------*/
      for (x_cur = ECOL; x_cur >= BCOL; --x_cur) {
         CURS_cell (x_cur, y_cur);
      }
      /*---(cycle locked columns)-----------*/
      if (tab->froz_col == 'y') {
         for (x_cur = tab->froz_ecol; x_cur >= tab->froz_bcol; --x_cur) {
            CURS_cell (x_cur, y_cur);
         }
      }
      /*---(fill in at end)-----------*/
      end  = tab->cols[ECOL].x + tab->cols[ECOL].w;
      left = avail - end + 1;
      if (left > 0) {
         for (i = 0; i < tab->rows[y_cur].h; ++i) {
            mvprintw (tab->rows[y_cur].y + i, end, "%*.*s", left, left, empty);
         }
      }
   }
   /*---(fill in bottom)-----------------*/
   for (y_cur = ch; y_cur < my.y_avail; ++y_cur) {
      mvprintw (tab->rows[y_cur].y, 5, "%*.*s", my.x_avail, my.x_avail, empty);
   }
   /*---(complete)------------------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       support functions                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___SUPPORT_________o () { return; }

char       /* PURPOSE : shutdown curses                               */
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
   row_status   = y - 2;
   row_message  = y - 1;
   col_header   = 0;
   col_far      = x - 1;
   /*---(critical numbers)------------*/
   my.y_full    = y;
   my.y_avail   = y - row_main - 2;
   /*---(column markers)--------------*/
   my.x_full    =  x;
   my.x_left    =  5;
   my.x_right   =  0;
   my.x_avail   = x - my.x_left - my.x_right;
   /*---(clear the screen)------------*/
   clear();
   /*---(complete)--------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char               /* PURPOSE : initialize curses                             */
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

char               /* PURPOSE : shutdown curses                               */
CURS_end           (void)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   endwin();        /* shut down ncurses                                      */
   /*---(complete)--------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*============================----end-of-source---============================*/
