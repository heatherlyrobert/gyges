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


/*  k=black, r=red, g=green, y=yellow, b=blue, m=magenta, c=cyan, w=grey */

#define     MAX_COLORS      100
struct cCOLOR_INFO {
   char        terse       [10];            /* short description              */
   char        ctype;                       /* cell type, if it applies       */
   char        desc        [50];            /* description/reason             */
   char        bg_color;                    /* background color               */
   char        fg_color;                    /* foreground color               */
   char        bold;                        /* bold y/n                       */
   int         value;                       /* curses attribute value         */
} s_color_info [MAX_COLORS] = {
   /*---terse--- -ty- ---description---------------------------------------- -bg-- -fg-- --b- --v- */
   /*---(window)---------*/
   { "t_norm"   , ' ', "normal title color"                                 , '-' , '-' , '-',   0 },
   { "t_err"    , ' ', "title color when in error"                          , '-' , '-' , '-',   0 },
   { "s_norm"   , ' ', "normal status color"                                , '-' , '-' , '-',   0 },
   { "s_err"    , ' ', "status color when in error"                         , '-' , '-' , '-',   0 },
   { "w_keys"   , ' ', "window keystoke display"                            , '-' , '-' , '-',   0 },
   { "w_mesg"   , ' ', "window message"                                     , '-' , '-' , '-',   0 },
   /*---(modes)----------*/
   { "m_map"    , ' ', "map mode (2d review of sheet/cell collection"       , '-' , '-' , '-',   0 },
   { "m_src"    , ' ', "source mode (single cell review)"                   , '-' , '-' , '-',   0 },
   { "m_input"  , ' ', "input mode"                                         , '-' , '-' , '-',   0 },
   { "m_wand"   , ' ', "wander mode"                                        , '-' , '-' , '-',   0 },
   /*---(row/col header)-*/
   { "h_cur"    , ' ', "current row/column header"                          , '-' , '-' , '-',   0 },
   { "h_loc"    , ' ', "locked row/column header"                           , '-' , '-' , '-',   0 },
   { "h_use"    , ' ', "row/column with entries"                            , '-' , '-' , '-',   0 },
   { "h_not"    , ' ', "row/column with no entries"                         , '-' , '-' , '-',   0 },
   /*---(selection)------*/
   { "curr"     , ' ', "current cell"                                       , '-' , '-' , '-',   0 },
   { "root"     , ' ', "root of visual selection"                           , '-' , '-' , '-',   0 },
   { "visu"     , ' ', "selected, not root/curr"                            , '-' , '-' , '-',   0 },
   /*---(dep type)-------*/
   { "reqs"     , ' ', "value required from cell"                           , '-' , '-' , '-',   0 },
   { "pros"     , ' ', "value provided to cell"                             , '-' , '-' , '-',   0 },
   { "like"     , ' ', "formula is copy/variation"                          , '-' , '-' , '-',   0 },
   /*---(danger signs)---*/
   { "fdang"    , 'f', "complex numeric formula"                            , '-' , '-' , '-',   0 },
   { "mdang"    , 'm', "complex string formula"                             , '-' , '-' , '-',   0 },
   /*---(cell types)-----*/
   { "num"      , 'n', "numeric literal"                                    , '-' , '-' , '-',   0 },
   { "for"      , 'f', "numeric formula"                                    , '-' , '-' , '-',   0 },
   { "flike"    , 'l', "numeric formula (copy)"                             , '-' , '-' , '-',   0 },
   { "str"      , 's', "string literal"                                     , '-' , '-' , '-',   0 },
   { "mod"      , 'm', "string formula"                                     , '-' , '-' , '-',   0 },
   { "mlike"    , 'L', "string formula (copy)"                              , '-' , '-' , '-',   0 },
   { "range"    , 'p', "range pointer"                                      , '-' , '-' , '-',   0 },
   { "addr"     , 'p', "address pointer"                                    , '-' , '-' , '-',   0 },
   { "blank"    , '-', "blank cell"                                         , '-' , '-' , '-',   0 },
   { "def"      , '-', "default for unidentified cells"                     , '-' , '-' , '-',   0 },
   /*---(other)----------*/
   { "warn"     , 'w', "cell warning"                                       , '-' , '-' , '-',   0 },
   { "error"    , 'E', "cell error"                                         , '-' , '-' , '-',   0 },
   { "found"    , '-', "cell found in most recent search"                   , '-' , '-' , '-',   0 },
};


/*===[[ COLOR DEFINITIONS ]]==================================================*/
/*---(window)-------------------------*/
static int  S_COLOR_TITLE      = COLOR_PAIR(40) | A_BOLD;
static int  S_COLOR_TITLEE     = COLOR_PAIR(61);
static int  S_COLOR_STATUS     = COLOR_PAIR(40) | A_BOLD;
static int  S_COLOR_STATUSE    = COLOR_PAIR(61);
static int  S_COLOR_KEYS       = COLOR_PAIR(71) | A_BOLD;
static int  S_COLOR_MESSAGE    = COLOR_PAIR(77);
/*---(edit modes)---------------------*/
static int  S_COLOR_CONTENT    = COLOR_PAIR(43) | A_BLINK;
static int  S_COLOR_SOURCE     = COLOR_PAIR(42) | A_BLINK;
static int  S_COLOR_INPUT      = COLOR_PAIR(47) | A_BLINK;
static int  S_COLOR_WANDER     = COLOR_PAIR(41) | A_BLINK;
/*---(cells)--------------------------*/
static int  S_COLOR_CURRENT    = COLOR_PAIR(43) | A_BLINK;
static int  S_COLOR_VISUAL     = COLOR_PAIR(23) | A_BOLD;
static int  S_COLOR_ROOT       = COLOR_PAIR(33) | A_BOLD;

static int  S_COLOR_REQS       = COLOR_PAIR(25) | A_BOLD;
static int  S_COLOR_PROS       = COLOR_PAIR(22) | A_BOLD;
static int  S_COLOR_LIKE       = COLOR_PAIR(24) | A_BOLD;

static int  S_COLOR_ERROR      = COLOR_PAIR(61) | A_BOLD;

static int  S_COLOR_POINTER    = COLOR_PAIR(76) | A_BOLD;

static int  S_COLOR_FSIMPLE    = COLOR_PAIR(72) | A_BOLD;
static int  S_COLOR_FDANGER    = COLOR_PAIR(71) | A_BOLD;
static int  S_COLOR_FLIKE      = COLOR_PAIR(72)           ;

static int  S_COLOR_STRING     = COLOR_PAIR(73) | A_BOLD;
static int  S_COLOR_FSTRING    = COLOR_PAIR(75) | A_BOLD;
static int  S_COLOR_FSTRDAG    = COLOR_PAIR(75) | A_BOLD;
static int  S_COLOR_MLIKE      = COLOR_PAIR(75)          ;

static int  S_COLOR_FCOPY      = COLOR_PAIR(34) | A_BOLD | A_BLINK;

static int  S_COLOR_NUMBER     = COLOR_PAIR(74) | A_BOLD;
static int  S_COLOR_LABEL      = COLOR_PAIR(73);
static int  S_COLOR_NULL       = COLOR_PAIR(70) | A_BOLD;

static int  S_COLOR_NORMAL     =  COLOR_PAIR(73) | A_BOLD;
/*---(row and column headers)---------*/
static int  S_COLOR_HEADY      = COLOR_PAIR(33) | A_BOLD;
static int  S_COLOR_HEADL      = COLOR_PAIR(43) | A_BOLD;
static int  S_COLOR_HEADF      = COLOR_PAIR(73);
static int  S_COLOR_HEADN      = COLOR_PAIR(74);


char        msg_type  = '-';
char        sta_type  = 'v';
char        sta_error = '-';


/*====================------------------------------------====================*/
/*===----                      public functions                        ----===*/
/*====================------------------------------------====================*/

char
CURS_formula       (tCELL *a_curr)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------------------------*/
   char      msg[500]  = "";                   /* temporary display message   */
   char      label[3]  = "";                   /* temporary display message   */
   int       len       = 0;
   int       w         = 0;
   /*---(clear line)---------------------*/
   if (sta_error == 'y')  attron (S_COLOR_TITLEE);
   else                   attron (S_COLOR_TITLE);
   mvprintw (row_formula, 0, "%*.*s", my.x_full, my.x_full, empty);
   /*---(location)-----------------------*/
   /*> strcpy(label, tab->cols[CCOL].l);                                         <* 
    *> if (label[0] == '-') label[0] = ' ';                                           <*/
   /*> mvprintw (row_formula,  0, "%c%c [%s%d ]", mode, (SEL_islive()) ? 'v' : ' ', label, CROW + 1);   <*/
   if (a_curr != NULL)    strcpy  (label, a_curr->label);
   else                   LOC_ref (CTAB, CCOL, CROW, 0, label);
   mvprintw (row_formula,  0, "%c%c %c%c %-6.6s", mode, (SEL_islive()) ? 'v' : ' ', creg, creg_lock, label);
   /*---(version)------------------------*/
   mvprintw (row_formula, my.x_full - 15, " %s of gyges", VER_NUM);
   /*---(characteristics)--------------------*/
   if (a_curr != NULL) {
      mvprintw (row_formula, 12, " %02d %c %c %c %c  "   , tab->cols[CCOL].w, a_curr->t, a_curr->f, a_curr->d, a_curr->a);
   } else {
      mvprintw (row_formula, 12, " %02d                []", tab->cols[CCOL].w);
   }
   /*---(length)-----------------------------*/
   len = strlen(contents);
   mvprintw (row_formula, 25, "%4d:", len);
   if (sta_error == 'y')  attroff(S_COLOR_TITLEE);
   else                   attroff(S_COLOR_TITLE);
   /*---(set color)--------------------------*/
   if      (mode == MODE_SOURCE)  attron (S_COLOR_SOURCE );
   else if (mode == MODE_INPUT )  attron (S_COLOR_INPUT  );
   else if (mode == MODE_WANDER)  attron (S_COLOR_WANDER );
   else                           attron (S_COLOR_CONTENT);
   /*---(contents)---------------------------*/
   w  = my.apos - strlen(contents);
   if (w < 0) w = 0;
   mvprintw (row_formula, 30, "%*.*s", my.apos, my.apos, empty);
   if (w >  0) snprintf(msg, 500, "[%s]",    contents);
   if (w == 0) snprintf(msg, 500, "[%*.*s]", my.apos, my.apos, contents + my.bpos);
   mvprintw (row_formula, 30, "%s%*.*s", msg, w, w, empty);
   /*---(clear color)--------------------*/
   if      (mode == MODE_SOURCE)  attroff(S_COLOR_SOURCE );
   else if (mode == MODE_INPUT )  attroff(S_COLOR_INPUT  );
   else if (mode == MODE_WANDER)  attroff(S_COLOR_WANDER );
   else                           attroff(S_COLOR_CONTENT);
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
   case 'R' : /* buffer contents */
      REG_list     (creg  , bufc);
      snprintf (msg, 500, "[ %-100.100s ]", bufc);
      break;
   case 'k' : /* keylog */
      KEYS_status (msg);
      break;
   case 'u' :
      snprintf (msg, 500, "[ nhist : %4d, chist : %4d, top : %s ]", nhist, chist, hist [chist].act);
      break;
   case 'v' : /* file version */
   default  :
      if (ver_ctrl == 'y')  snprintf (msg, 500, "[ file : %-40.40s ][ %dc x %dr ][ version : %-5.5s, %-40.40s ]", f_name, tab->ncol, tab->nrow, ver_num, ver_txt);
      else                  snprintf (msg, 500, "[ file : %-40.40s ][ %dc x %dr ][ version : not  controlled                                ]", f_name, tab->ncol, tab->nrow);
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
   mvprintw (row_message, 0, message);
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

int                /* PURPOSE : presents the outer window frame elements      */
CURS_main          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         ch          = 0;
   tCELL      *curr        = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(initialize)---------------------*/
   my.apos = my.x_full - 30 - 15 - 2;
   curr    = tab->sheet[CCOL][CROW];
   strncpy (reqs, "+", MAX_STR);
   strncpy (deps, "+", MAX_STR);
   strncpy (like, "+", MAX_STR);
   DEP_requires (curr, reqs);
   DEP_provides (curr, deps);
   DEP_like     (curr, like);
   /*---(update cells)-------------------*/
   CURS_formula   (curr);
   CURS_status    (curr);
   CURS_message   ();
   CURS_colhead   ();
   CURS_rowhead   ();
   CURS_page      ();
   /*---(command)------------------------*/
   attron   (S_COLOR_KEYS);
   mvprintw (row_chead, 0, cmd);
   attroff  (S_COLOR_KEYS);
   /*---(cursor pos)---------------------*/
   if (mode != MODE_SOURCE && mode != MODE_INPUT)
      move (tab->rows[CROW].y, tab->cols[CCOL].x + tab->cols[CCOL].w - 1);
   else
      move (row_formula, 31 + my.cpos - my.bpos);
   /*---(refresh)------------------------*/
   refresh ();
   ch = getch ();
   DEBUG_GRAF  yLOG_value   ("key"       , ch);
   /*> if (ch == 3) {                                                                 <* 
    *>    endwin();                                                                   <* 
    *>    exit(-1);                                                                   <* 
    *> }                                                                              <*/
   if (ch == KEY_RESIZE) {
      CURS_size ();
      KEYS_col  (" r");
      KEYS_row  (" r");
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
   char      high      = 0;
   int       i         = 0;
   uint      xmax      = 0;
   int       xcol      = 0;
   char      xdisp [MAX_STR];
   /*---(check for merges)-----------------*/
   if (curr != NULL && curr->a == '+')  return 0;
   /*---(identify cell)--------------------*/
   if (curr != NULL) {
      LOC_label  (curr, l);
      sprintf    (label, ",%s,", l);
   }
   /*---(current)--------------------------*/
   if      (a_col == CCOL && a_row == CROW)     { high= 1; attron (S_COLOR_CURRENT); }
   /*---(visual-range)---------------------*/
   /*> else if (SEL_root     (CTAB, a_col, a_row))  { high=12; attron (S_COLOR_ROOT   ); }   <*/
   else if (SEL_root     (CTAB, a_col, a_row))  { high=12; attron (S_COLOR_VISUAL ); }
   else if (SEL_selected (CTAB, a_col, a_row))  { high= 2; attron (S_COLOR_VISUAL ); }
   /*---(content-based)--------------------*/
   else if (curr != NULL) {
      /*---(trouble)--------------------------*/
      if      (curr->t == CTYPE_ERROR)          { high=31; attron (S_COLOR_ERROR  ); }
      else if (curr->t == CTYPE_WARN )          { high=31; attron (S_COLOR_ERROR  ); }
      /*---(related)--------------------------*/
      else if (strstr (reqs, label) != NULL)    { high= 7; attron (S_COLOR_REQS   ); }
      else if (strstr (deps, label) != NULL)    { high= 8; attron (S_COLOR_PROS   ); }
      else if (strstr (like, label) != NULL)    { high=10; attron (S_COLOR_LIKE   ); }
      /*---(pointers)-------------------------*/
      else if (curr->t == CTYPE_RANGE)          { high=32; attron (S_COLOR_POINTER); }
      else if (curr->t == CTYPE_ADDR )          { high=32; attron (S_COLOR_POINTER); }
      /*---(numbers)--------------------------*/
      else if (curr->t == CTYPE_NUM  )          { high= 5; attron (S_COLOR_NUMBER ); }
      else if (curr->t == CTYPE_FORM ) {
         if   (curr->nrequire < 3)              { high= 3; attron (S_COLOR_FSIMPLE); }
         else                                   { high= 4; attron (S_COLOR_FDANGER); }
      }
      else if (curr->t == CTYPE_FLIKE)          { high= 9; attron (S_COLOR_FLIKE  ); }
      /*---(strings)--------------------------*/
      else if (curr->t == CTYPE_STR  )          { high= 6; attron (S_COLOR_STRING ); }
      else if (curr->t == CTYPE_MOD  ) {
         if      (curr->nrequire < 3)           { high=16; attron (S_COLOR_FSTRDAG); }
         else                                   { high=11; attron (S_COLOR_FSTRING); }
      }
      else if (curr->t == CTYPE_MLIKE)          { high= 9; attron (S_COLOR_MLIKE  ); }
      /*---(constants)------------------------*/
      /*> else if (curr->t == 'l')                  { high=13; attron (S_COLOR_LABEL  ); }   <*/
      else if (curr->t == CTYPE_BLANK)          { high=14; attron (S_COLOR_NULL   ); }
      else                                      { high= 6; attron (S_COLOR_STRING ); }
   }
   /*---(check max width)------------------*/
   xmax = tab->cols[a_col].w;
   if (curr != NULL) {
      xcol = curr->col + 1;
      next = tab->sheet[xcol][a_row];
      while (next != NULL && xcol <= ECOL && next->a == '+') {
         xmax += tab->cols[next->col].w;
         ++xcol;
         next = tab->sheet[xcol][a_row];
      }
   }
   /*---(check for current)----------------*/
   if (curr!= NULL && a_row == CROW) {
      xcol = curr->col + 1;
      next = tab->sheet[xcol][a_row];
      while  (next != NULL) {
         if (next->a != '+') break;
         if (xcol == CCOL) {
            high = 1;
            attron (S_COLOR_CURRENT);
            break;
         }
         ++xcol;
         next = tab->sheet[xcol][a_row];
      }
   }
   /*---(display cell)---------------------*/
   for (i = 0; i < tab->rows[a_row].h; ++i) {
      if (curr == NULL || curr->p == NULL) {
         mvprintw (tab->rows[a_row].y + i, tab->cols[a_col].x, "%*.*s", tab->cols[a_col].w, tab->cols[a_col].w, empty);
      } else {
         if (i == 0) {
            sprintf (xdisp, "%-*.*s", xmax, xmax, curr->p);
            if (xmax < strlen(curr->p)) xdisp[xmax - 1] = '>';
            mvprintw (tab->rows[a_row].y + i, tab->cols[a_col].x, xdisp);
         } else {
            mvprintw (tab->rows[a_row].y + i, tab->cols[a_col].x, "%*.*s", tab->cols[a_col].w, tab->cols[a_col].w, empty);
         }
      }
   }
   /*---(highlight off)--------------------*/
   if      (high ==  1) attroff (S_COLOR_CURRENT);
   else if (high ==  2) attroff (S_COLOR_VISUAL );
   else if (high == 12) attroff (S_COLOR_ROOT   );
   else if (high ==  9) attroff (S_COLOR_FLIKE  );
   else if (high == 15) attroff (S_COLOR_FCOPY  );
   else if (high ==  3) attroff (S_COLOR_FSIMPLE);
   else if (high ==  4) attroff (S_COLOR_FDANGER);
   else if (high == 11) attroff (S_COLOR_FSTRING);
   else if (high == 16) attroff (S_COLOR_FSTRDAG);
   else if (high ==  5) attroff (S_COLOR_NUMBER );
   else if (high ==  6) attroff (S_COLOR_STRING );
   else if (high == 13) attroff (S_COLOR_LABEL  );
   else if (high == 14) attroff (S_COLOR_NULL   );
   else if (high ==  7) attroff (S_COLOR_REQS   );
   else if (high ==  8) attroff (S_COLOR_PROS   );
   else if (high == 10) attroff (S_COLOR_LIKE   );
   else if (high == 31) attroff (S_COLOR_ERROR  );
   else if (high == 32) attroff (S_COLOR_POINTER);
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
   /*---(transparent)--------------------*/
   init_pair (70, COLOR_BLACK  , -1           );   /* null/blank             */
   init_pair (71, COLOR_RED    , -1           );   /* dangerous formulas     */
   init_pair (72, COLOR_GREEN  , -1           );   /* numeric formulas       */
   init_pair (73, COLOR_YELLOW , -1           );   /* strings                */
   init_pair (74, COLOR_BLUE   , -1           );   /* numbers                */
   init_pair (75, COLOR_MAGENTA, -1           );   /* string formulas        */
   init_pair (76, COLOR_CYAN   , -1           );   /* pointers               */
   init_pair (77, COLOR_WHITE  , -1           );
   /*---(same color)---------------------*/
   init_pair (20, COLOR_BLACK  , COLOR_BLACK  );
   init_pair (21, COLOR_RED    , COLOR_RED    );
   init_pair (22, COLOR_GREEN  , COLOR_GREEN  );   /* provides dependencies  */
   init_pair (23, COLOR_YELLOW , COLOR_YELLOW );   /* visual selection       */
   init_pair (24, COLOR_BLUE   , COLOR_BLUE   );   /* like     dependencies  */
   init_pair (25, COLOR_MAGENTA, COLOR_MAGENTA);   /* requires dependencies  */
   init_pair (26, COLOR_CYAN   , COLOR_CYAN   );
   init_pair (27, COLOR_WHITE  , COLOR_WHITE  );
   /*---(color on black)-----------------*/
   init_pair (30, COLOR_BLACK  , COLOR_BLACK  );
   init_pair (31, COLOR_RED    , COLOR_BLACK  );
   init_pair (32, COLOR_GREEN  , COLOR_BLACK  );
   init_pair (33, COLOR_YELLOW , COLOR_BLACK  );
   init_pair (34, COLOR_BLUE   , COLOR_BLACK  );
   init_pair (35, COLOR_MAGENTA, COLOR_BLACK  );
   init_pair (36, COLOR_CYAN   , COLOR_BLACK  );
   init_pair (37, COLOR_WHITE  , COLOR_BLACK  );
   /*---(black on color)-----------------*/
   init_pair (40, COLOR_BLACK  , COLOR_BLACK  );
   init_pair (41, COLOR_BLACK  , COLOR_RED    );
   init_pair (42, COLOR_BLACK  , COLOR_GREEN  );
   init_pair (43, COLOR_BLACK  , COLOR_YELLOW );
   init_pair (44, COLOR_BLACK  , COLOR_BLUE   );
   init_pair (45, COLOR_BLACK  , COLOR_MAGENTA);
   init_pair (46, COLOR_BLACK  , COLOR_CYAN   );
   init_pair (47, COLOR_BLACK  , COLOR_WHITE  );
   /*---(color on white)-----------------*/
   init_pair (50, COLOR_BLACK  , COLOR_WHITE  );
   init_pair (51, COLOR_RED    , COLOR_WHITE  );
   init_pair (52, COLOR_GREEN  , COLOR_WHITE  );
   init_pair (53, COLOR_YELLOW , COLOR_WHITE  );
   init_pair (54, COLOR_BLUE   , COLOR_WHITE  );
   init_pair (55, COLOR_MAGENTA, COLOR_WHITE  );
   init_pair (56, COLOR_CYAN   , COLOR_WHITE  );
   init_pair (57, COLOR_WHITE  , COLOR_WHITE  );
   /*---(white on color)-----------------*/
   init_pair (60, COLOR_WHITE  , COLOR_BLACK  );
   init_pair (61, COLOR_WHITE  , COLOR_RED    );
   init_pair (62, COLOR_WHITE  , COLOR_GREEN  );
   init_pair (63, COLOR_WHITE  , COLOR_YELLOW );
   init_pair (64, COLOR_WHITE  , COLOR_BLUE   );
   init_pair (65, COLOR_WHITE  , COLOR_MAGENTA);
   init_pair (66, COLOR_WHITE  , COLOR_CYAN   );
   init_pair (67, COLOR_WHITE  , COLOR_WHITE  );
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
