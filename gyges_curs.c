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


/*============================---- METIS TASKS ---============================*/
/*
 *
 * task  gyges  ncurses     dw3<�  draw/clear partial cell at end of row/col to keep screen garbage out 
 *
 */

static  char      CURS_page          (void);

int     s_status_row;
int     s_status_size;
int     s_command_row;
int     s_command_size;

static tCELL  *s_curr      = NULL;
static tCELL  *s_prev      = NULL;


static int  s_cursor_x   = 0;
static int  s_cursor_y   = 0;
static char s_coloration = 'F';


tCURR g_curr;

static char  s_mark_list   [LEN_RECD];       /* current marks                  */
/*> static char  s_mark_plus   [LEN_RECD];       /+ current marks with mark id     +/   <*/



#define     MAX_MENU       500
/*> typedef struct cMENU  tMENU;                                                        <* 
 *> struct  cMENU {  /+ two level menu only, none of that complex shit            +/    <* 
 *>    char     one;                            /+ shortcut for menu              +/    <* 
 *>    char     menu      [LEN_FULL];            /+ top level menu item            +/   <* 
 *>    char     next;                           /+ next column                    +/    <* 
 *>    char     two;                            /+ shortcut for submenu           +/    <* 
 *>    char     name      [LEN_FULL];            /+ name of menu item              +/   <* 
 *>    char     desc      [LEN_FULL];            /+ descripion, example, alternate +/   <* 
 *> };                                                                                  <*/
/*> tMENU       s_menus     [MAX_MENU] = {                                                  <* 
 *>    /+ one  ---menu---------- next  -sc-  ---name----------  ---example------------ +/   <* 
 *>    {  'f', "file"           , ' ', '-', ""               , ""                     },    <* 
 *>    {  't', "technical"      , ' ', '-', ""               , ""                     },    <* 
 *>    {  'F', "format"         , ' ', '<', "left"           , "test                " },    <* 
 *>    {  ' ', ""               , ' ', '|', "center"         , "        test        " },    <* 
 *>    {  ' ', ""               , ' ', '>', "right"          , "               test " },    <* 
 *>    {  ' ', ""               , ' ', '[', "left bracket"   , "[test              ]" },    <* 
 *>    {  ' ', ""               , ' ', '^', "center bracket" , "[       test       ]" },    <* 
 *>    {  ' ', ""               , ' ', ']', "right bracket"  , "[              test]" },    <* 
 *>    {  ' ', ""               , ' ', '{', "right brace"    , "[  test            ]" },    <* 
 *>    {  ' ', ""               , ' ', '}', "left brace"     , "[            test  ]" },    <* 
 *>    {  ' ', ""               , ' ', ' ', "fill space"     , "[  test            ]" },    <* 
 *>    {  ' ', ""               , ' ', '-', "fill dash"      , "[--test------------]" },    <* 
 *>    {  ' ', ""               , ' ', '=', "fill equal"     , "[==test============]" },    <* 
 *>    {  ' ', ""               , ' ', '_', "fill under"     , "[__test____________]" },    <* 
 *>    {  ' ', ""               , ' ', '.', "fill period"    , "[..test............]" },    <* 
 *>    {  ' ', ""               , ' ', '+', "fill plus"      , "[++test++++++++++++]" },    <* 
 *>    {  ' ', ""               , ' ', 'i', "integer"        , "            1234567 " },    <* 
 *>    {  ' ', ""               , ' ', 'r', "real"           , "            987.541 " },    <* 
 *>    {  ' ', ""               , ' ', 'g', "general"        , "         987.541234 " },    <* 
 *>    {  ' ', ""               , ' ', ',', "comma"          , "          1,234,567 " },    <* 
 *>    {  ' ', ""               , ' ', 's', "comma/sign"     , "         +1,234,567 " },    <* 
 *>    {  ' ', ""               , ' ', 'a', "accounting"     , "        (1,234,567) " },    <* 
 *>    {  ' ', ""               , ' ', '$', "currency"       , "         $1,234,567 " },    <* 
 *>    {  ' ', ""               , ' ', 'p', "percent"        , "                15p " },    <* 
 *>    {  ' ', ""               , '-', 'P', "point/bullet"   , "                 5) " },    <* 
 *>    {  ' ', ""               , ' ', '#', "technical"      , "  123,456.789'032 + " },    <* 
 *>    {  ' ', ""               , ' ', 'e', "exponential"    , "          1.235e+05 " },    <* 
 *>    {  ' ', ""               , ' ', 'E', "exp divided"    , "       +1.235 e +05 " },    <* 
 *>    {  ' ', ""               , ' ', 'x', "hexidecimal"    , "              xaf68 " },    <* 
 *>    {  ' ', ""               , ' ', 'X', "hex divided"    , "            x'af'68 " },    <* 
 *>    {  ' ', ""               , ' ', 'b', "binary"         , "          b10101111 " },    <* 
 *>    {  ' ', ""               , ' ', 'B', "binary divided" , "        b'1010'1111 " },    <* 
 *>    {  ' ', ""               , ' ', 'o', "octal"          , "               o257 " },    <* 
 *>    {  ' ', ""               , ' ', 'O', "octal"          , "              o'257 " },    <* 
 *>    {  ' ', ""               , ' ', 't', "time (24h)"     , "              13:26 " },    <* 
 *>    {  ' ', ""               , ' ', 'd', "date"           , "        2014-Dec-15 " },    <* 
 *>    {  ' ', ""               , ' ', 'T', "timestamp"      , "  14.12.15.13.26.15 " },    <* 
 *>    {  ' ', ""               , ' ', 'D', "date/time"      , "  2014-Dec-15 13:26 " },    <* 
 *>    {  ' ', ""               , ' ', '0', "0 decimals"     , "                  0 " },    <* 
 *>    {  ' ', ""               , ' ', '1', "1 decimals"     , "                0.1 " },    <* 
 *>    {  ' ', ""               , ' ', '2', "2 decimals"     , "               0.12 " },    <* 
 *>    {  ' ', ""               , ' ', '3', "3 decimals"     , "              0.123 " },    <* 
 *>    {  ' ', ""               , ' ', '4', "4 decimals"     , "             0.1234 " },    <* 
 *>    {  ' ', ""               , ' ', '5', "5 decimals"     , "            0.12345 " },    <* 
 *>    {  ' ', ""               , ' ', '6', "6 decimals"     , "           0.123456 " },    <* 
 *>    {  ' ', ""               , ' ', '7', "7 decimals"     , "          0.1234567 " },    <* 
 *>    {  ' ', ""               , ' ', '8', "8 decimals"     , "         0.12345678 " },    <* 
 *>    {  ' ', ""               , ' ', '9', "9 decimals"     , "        0.123456789 " },    <* 
 *>    {  'S', "sizing"         , ' ', 'm', "min width"      , "4 chars             " },    <* 
 *>    {  ' ', ""               , ' ', 'n', "normal width"   , "8 chars             " },    <* 
 *>    {  ' ', ""               , ' ', 'N', "normal+ width"  , "12 chars            " },    <* 
 *>    {  ' ', ""               , ' ', 'w', "wide width"     , "20 chars            " },    <* 
 *>    {  ' ', ""               , ' ', 'w', "wide+ width"    , "50 chars            " },    <* 
 *>    {  ' ', ""               , ' ', 'H', "narrower by 5"  , "-5 chars (rounded)  " },    <* 
 *>    {  ' ', ""               , ' ', 'h', "narrower by 1"  , "-1 chars            " },    <* 
 *>    {  ' ', ""               , ' ', 'l', "wider by 1"     , "+1 chars            " },    <* 
 *>    {  ' ', ""               , ' ', 'L', "wider by 5"     , "+5 chars (rounded)  " },    <* 
 *>    {  ' ', ""               , ' ', 'K', "shortest"       , "1 char tall         " },    <* 
 *>    {  ' ', ""               , ' ', 'k', "shorter by 1"   , "-1 chars            " },    <* 
 *>    {  ' ', ""               , ' ', 'j', "taller by 1"    , "+1 chars            " },    <* 
 *>    {  ' ', ""               , ' ', 'J', "tallest"        , "5 chars             " },    <* 
 *>    {  ' ', ""               , ' ', 'M', "merge cells"    , "                    " },    <* 
 *>    {  ' ', ""               , ' ', 'U', "unmerge cells"  , "                    " },    <* 
 *>    {  'D', "distribution"   , ' ', '-', ""               , "                    " },    <* 
 *>    {  '~', "end-of-menu"    , ' ', ' ', ""               , ""                     },    <* 
 *>    {  ' ', ""               , ' ', ' ', ""               , ""                     },    <* 
 *> };                                                                                      <*/



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
static int  S_COLOR_TEXREG     = COLOR_PAIR(47) | A_BLINK;
static int  S_COLOR_REPLACE    = COLOR_PAIR(47) | A_BLINK;
static int  S_COLOR_WANDER     = COLOR_PAIR(41) | A_BLINK;
/*---(cells)--------------------------*/
static int  S_COLOR_CURRENT    = COLOR_PAIR(43) | A_BLINK;
static int  S_COLOR_VISUAL     = COLOR_PAIR(23) | A_BOLD;
static int  S_COLOR_ROOT       = COLOR_PAIR(33) | A_BOLD;
static int  S_COLOR_MARK       = COLOR_PAIR(33) | A_BOLD;
static int  S_COLOR_SEARCH     = COLOR_PAIR(33) | A_BOLD;

static int  S_COLOR_REQS       = COLOR_PAIR(25) | A_BOLD;
static int  S_COLOR_PROS       = COLOR_PAIR(22) | A_BOLD;
static int  S_COLOR_LIKE       = COLOR_PAIR(24) | A_BOLD;

static int  S_COLOR_ERROR      = COLOR_PAIR(61) | A_BOLD;
static int  S_COLOR_WARN       = COLOR_PAIR(61) | A_BOLD;
static int  S_COLOR_FOUND      = COLOR_PAIR(61) | A_BOLD;

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
static int  S_COLOR_HCURR      = COLOR_PAIR(33) | A_BOLD;
static int  S_COLOR_HUSED      = COLOR_PAIR(43) | A_BOLD;
static int  S_COLOR_HLOCK      = COLOR_PAIR(73);
static int  S_COLOR_HNORM      = COLOR_PAIR(74);


char        sta_error = '-';

static char s_label   [10]  = "";
static int  s_start         = 0;
static int  s_space         = 0;


char         /*-> tbd --------------------------------[ leaf   [gz.210.101.00]*/ /*-[00.0000.104.!]-*/ /*-[--.---.---.--]-*/
CURS_info_request  (char a_type)
{
   my.info_win = a_type;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                   specific areas of screen                   ----===*/
/*====================------------------------------------====================*/
static void  o___SPECIFIC________o () { return; }

char  CURS_status_cell     (char *a_list) { snprintf (a_list, LEN_FULL, "[ rpn =%-20.20s ][ reqs=%-40.40s ][ pros=%-40.40s ][ like=%-40.40s ]", my.rpn_list, my.reqs_list, my.deps_list, my.like_list); }
char  CURS_status_deps     (char *a_list) { snprintf (a_list, LEN_FULL, "[ reqs=%-40.40s ][ pros=%-40.40s ]", my.reqs_list, my.deps_list); }
char  CURS_status_rpn      (char *a_list) { snprintf (a_list, LEN_FULL, "[ rpn =%-80.80s ]", my.rpn_list); }
char  CURS_status_tab      (char *a_list) { char t [LEN_LABEL]; TAB_name (CTAB, t); snprintf (a_list, LEN_FULL, "[ tab : %c, %s ][ %dc x %dr ]", CTAB, t, NCOL, NROW); }
char  CURS_status_error    (char *a_list) { snprintf (a_list, LEN_FULL, "errors (%3d)", nerror); };
char  CURS_status_detail   (char *a_list) { snprintf (a_list, LEN_FULL, "%-8.8s %2dt, %3dc, %4dr, t=%c a=%c f=%c d=%c u=%c len=%3d", g_curr.label, g_curr.tab, g_curr.col, g_curr.row, g_curr.type, g_curr.align, g_curr.format, g_curr.decs, g_curr.unit, g_curr.len); }


char         /*-> tbd --------------------------------[ ------ [gz.220.101.41]*/ /*-[00.0000.014.!]-*/ /*-[--.---.---.--]-*/
CURS_col_color     (short a_col)
{
   if      (a_col == CCOL)                       attron  (S_COLOR_HCURR   );
   else if (FR_COL == 'y' && a_col <= FR_ECOL)   attron  (S_COLOR_HLOCK   );
   else if (COL_used (CTAB, a_col) >  0)         attron  (S_COLOR_HUSED   );
   else                                          attron  (S_COLOR_HNORM   );
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.220.101.41]*/ /*-[00.0000.014.!]-*/ /*-[--.---.---.--]-*/
CURS_row_color       (short a_row)
{
   if      (a_row == CROW)                       attron  (S_COLOR_HCURR   );
   else if (FR_ROW == 'y' && a_row <= FR_EROW)   attron  (S_COLOR_HLOCK   );
   else if (ROW_used (CTAB, a_row) >  0)         attron  (S_COLOR_HUSED   );
   else                                          attron  (S_COLOR_HNORM   );
   return 0;
}

char         /*-> update the row labels --------------[ ------ [gz.D91.061.A5]*/ /*-[02.3000.323.!]-*/ /*-[--.---.---.--]-*/
DRAW_yaxis         (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   int         x_tall      = 0;
   int         x_left      = 0;
   int         x_bott      = 0;
   uchar       x_label     [LEN_TERSE] = "";     /* column label              */
   /*---(begin)--------------------------*/
   DEBUG_WIND  yLOG_enter   (__FUNCTION__);
   yVIKEYS_view_size     (YVIKEYS_YAXIS, &x_left, NULL, &x_bott, &x_tall, NULL);
   DEBUG_WIND  yLOG_complex ("size"      , "%3dl, %3db, %3dt", x_left, x_bott, x_tall);
   DEBUG_WIND  yLOG_complex ("rows"      , "%3db, %3de, %3dn", BROW, EROW, NROW);
   /*---(process rows)------------------*/
   for (i = 0; i < x_tall; ++i) {
      /*---(prepare)------------*/
      if (BROW + i <= NROW)   strlcpy (x_label, LABEL_row (BROW + i), LEN_TERSE);
      else                    strlcpy (x_label, "����"       , LEN_TERSE);
      /*---(show)---------------*/
      CURS_row_color  (BROW + i);
      mvprintw (x_bott - x_tall + 1 + i, x_left, "%-4.4s", x_label);
      /*---(spacer)-------------*/
      attrset (0);
      mvprintw (x_bott - x_tall + 1 + i, x_left + 4, " ");
      /*---(done)---------------*/
   }
   /*---(complete)----------------------*/
   DEBUG_WIND  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> update the column labels -----------[ ------ [gz.D91.061.A5]*/ /*-[02.3000.323.!]-*/ /*-[--.---.---.--]-*/
DRAW_xaxis         (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_pref      = 0;
   int         x_left      = 0;
   int         x_wide      = 0;
   int         x_bott      = 0;
   int         i           = 0;                  /* iterator -- columns       */
   int         w           = 0;                  /* column width              */
   int         x_fill      = 0;                  /* adjusted column width     */
   int         x_cum       = 0;
   int         x_avail     = 0;
   char        x_label     [LEN_TERSE] = "";     /* column label              */
   int         x_pos       = 0;                  /* adjusted column width     */
   char        x_disp      [500]       = "";     /* temporary display message */
   /*---(begin)--------------------------*/
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(begin)--------------------------*/
   yVIKEYS_view_size     (YVIKEYS_YAXIS, NULL   , &x_pref, NULL   , NULL, NULL);
   yVIKEYS_view_size     (YVIKEYS_XAXIS, &x_left, &x_wide, &x_bott, NULL, NULL);
   x_cum = x_pref;
   sprintf (x_disp, "�_%c_�     ", LABEL_tab (CTAB));
   mvprintw (x_bott, x_left, "%*.*s", x_cum, x_cum, x_disp);
   DEBUG_WIND  yLOG_complex ("size"      , "%3dp, %3dl, %3dw, %3db", x_pref, x_left, x_wide, x_bott);
   DEBUG_WIND  yLOG_complex ("cols"      , "%3db, %3de, %3dn", BCOL, ECOL, NCOL);
   /*---(normal cols)--------------------*/
   while (x_cum < x_wide) {
      if (BCOL + i > NCOL) break;
      /*---(prepare)---------------------*/
      x_avail = x_wide - x_cum;
      w      = COL_size (CTAB, BCOL + i);
      x_fill = w - 4;
      if (x_avail < w)  x_fill = x_avail - 4;
      strlcpy (x_label, LABEL_col (BCOL + i), LEN_TERSE);
      /*---(format)----------------------*/
      DEBUG_WIND  yLOG_complex ("curr"      , "%3d, %s, %3dw, %3dwa, %3dav", i, x_label, w, x_fill, x_avail);
      if      (x_avail == 1) snprintf (x_disp, 500, ">");
      else if (x_avail == 2) snprintf (x_disp, 500, "\[>");
      else if (x_avail == 3) snprintf (x_disp, 500, "\[->");
      else if (x_avail <  w) snprintf (x_disp, 500, "\[%*.*s%s>", x_fill, x_fill, g_dashes, x_label);
      else                   snprintf (x_disp, 500, "\[%*.*s%s\]", x_fill, x_fill, g_dashes, x_label);
      /*> if (x_disp [5] == '-')  x_disp [5] = G_CHAR_SPACE;                          <*/
      /*---(draw))-----------------------*/
      CURS_col_color  (BCOL + i);
      mvprintw (x_bott, x_cum, x_disp);
      attrset (0);
      /*---(next)------------------------*/
      x_cum += w;
      ++i;
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> update buffer display --------------[ ------ [gz.D91.061.A5]*/ /*-[02.3000.323.!]-*/ /*-[--.---.---.--]-*/
CURS_bufsum        (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   int         x_left      = 0;
   int         x_wide      = 0;
   int         x_bott      = 0;
   uchar       t           [LEN_HUND]  = "";
   uchar       s           [LEN_HUND]  = "";
   short       x_offset    =    0;
   uchar       x_disp      [LEN_RECD]  = "";
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(begin)--------------------------*/
   yVIKEYS_view_size     (YVIKEYS_BUFFER, &x_left, &x_wide, &x_bott, NULL, NULL);
   DEBUG_WIND  yLOG_complex ("size"      , "%3dl, %3dw, %3db", x_left, x_wide, x_bott);
   TAB_inventory ('L', t);
   /*---(inventory)----------------------*/
   strlcpy (s, t + 9, LEN_HUND);
   sprintf (x_disp, "buffers %s ����� ����� ����� ����� ����� ����� ����� �����", t + 2);
   strlcpy (a_list, x_disp, LEN_RECD);
   /*---(current)------------------------*/
   x_offset  = 17;
   x_offset += CTAB;
   x_offset += (CTAB / 6) * 2;
   x_offset += (CTAB / 3);
   /*---(complete)-----------------------*/
   DEBUG_GRAF  yLOG_exit    (__FUNCTION__);
   return x_offset;
}

char         /*-> update buffer display --------------[ ------ [gz.D91.061.A5]*/ /*-[02.3000.323.!]-*/ /*-[--.---.---.--]-*/
CURS_bufdet        (char *a_list)
{
}

/*> char         /+-> tbd --------------------------------[ leaf   [gz.B80.171.60]+/ /+-[03.5000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> CURS_menusub       (char a_menu)                                                                                               <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    int         i           =  0;                                                                                               <* 
 *>    int         x_count     =  0;                                                                                               <* 
 *>    int         x_row       =  4;                                                                                               <* 
 *>    int         x_col       =  1;                                                                                               <* 
 *>    char        x_line      [LEN_RECD];                                                                                         <* 
 *>    char       *x_title     = " - --submenu--------------------------";                                                         <* 
 *>    char        x_menu      = '-';                                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    attron (S_COLOR_TITLE);                                                                                                     <* 
 *>    mvprintw  (x_row, x_col, x_title);                                                                                          <* 
 *>    attrset (0);                                                                                                                <* 
 *>    /+---(show options)-------------------+/                                                                                    <* 
 *>    for (i = 0; i < MAX_MENU; ++i) {                                                                                            <* 
 *>       /+---(filter)----------------------+/                                                                                    <* 
 *>       if (s_menus [i].one == '~'   )  break;                                                                                   <* 
 *>       if (s_menus [i].one == a_menu)  x_menu = 'y';                                                                            <* 
 *>       if (s_menus [i].one != a_menu && s_menus [i].one != ' ')  x_menu = '-';                                                  <* 
 *>       if (x_menu          != 'y'   )  continue;                                                                                <* 
 *>       /+---(display)---------------------+/                                                                                    <* 
 *>       ++x_row;                                                                                                                 <* 
 *>       attron (S_COLOR_VISUAL);                                                                                                 <* 
 *>       sprintf (x_line, " %c %-15.15s%-20.20s",                                                                                 <* 
 *>             s_menus [i].two, s_menus [i].name, s_menus [i].desc);                                                              <* 
 *>       mvprintw  (x_row, x_col, x_line);                                                                                        <* 
 *>       attrset (0);                                                                                                             <* 
 *>       if (s_menus [i].next == '-' || x_row > 32) {                                                                             <* 
 *>          if (x_col > 20) break;                                                                                                <* 
 *>          ++x_row;                                                                                                              <* 
 *>          attron (S_COLOR_TITLE);                                                                                               <* 
 *>          mvprintw  (x_row, x_col, x_title);                                                                                    <* 
 *>          attrset (0);                                                                                                          <* 
 *>          x_col += strlen (x_title) + 2;                                                                                        <* 
 *>          x_row = 4;                                                                                                            <* 
 *>          attron (S_COLOR_TITLE);                                                                                               <* 
 *>          mvprintw  (x_row, x_col, x_title);                                                                                    <* 
 *>          attrset (0);                                                                                                          <* 
 *>       }                                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    ++x_row;                                                                                                                    <* 
 *>    /+---(footer)-------------------------+/                                                                                    <* 
 *>    attron (S_COLOR_TITLE);                                                                                                     <* 
 *>    mvprintw  (x_row, x_col, x_title);                                                                                          <* 
 *>    attrset (0);                                                                                                                <* 
 *>    /+---(complete)---------------------------+/                                                                                <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ ------ [gz.FC0.061.C1]+/ /+-[02.I000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> CURS_info_cell       (void)                                                                                                    <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    int         i           = 3;                                                                                                <* 
 *>    char        x_temp      [LEN_RECD];                                                                                         <* 
 *>    char        x_line      [LEN_RECD];                                                                                         <* 
 *>    int         x_len       = 0;                                                                                                <* 
 *>    char       *x_title     = " ------------------cell details-------------------- ";                                           <* 
 *>    tCELL      *x_curr      = LOC_cell_at_curr ();                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    attron (S_COLOR_TITLE);                                                                                                     <* 
 *>    mvprintw   ( i++, 10, x_title);                                                                                             <* 
 *>    attrset    (0);                                                                                                             <* 
 *>    /+---(show variables)-----------------+/                                                                                    <* 
 *>    attron     (S_COLOR_CURRENT);                                                                                               <* 
 *>    sprintf    (x_line, " %-5s tab=%3d, col=%3d, row=%3d                    ", s_label, CTAB, CCOL, CROW);                      <* 
 *>    mvprintw   ( i++, 10, x_line);                                                                                              <* 
 *>    if (x_curr == NULL) {                                                                                                       <* 
 *>       sprintf    (x_line, " %-50.50s ", " s --- (null)");                                                                      <* 
 *>       mvprintw   ( i++, 10, x_line);                                                                                           <* 
 *>    } else {                                                                                                                    <* 
 *>       if (x_curr->len < 43)  sprintf    (x_temp, "s %3d [%-s]"     , x_curr->len, x_curr->source);                                      <* 
 *>       else                 sprintf    (x_temp, "s %3d [%-42.42s>", x_curr->len, x_curr->source);                                      <* 
 *>       sprintf    (x_line, " %-50.50s ", x_temp);                                                                               <* 
 *>       mvprintw   ( i++, 10, x_line);                                                                                           <* 
 *>       sprintf    (x_temp, "v     = %-16.6lf", x_curr->v_num);                                                                  <* 
 *>       sprintf    (x_line, " %-50.50s ", x_temp);                                                                               <* 
 *>       mvprintw   ( i++, 10, x_line);                                                                                           <* 
 *>       if (x_curr->v_str == NULL) {                                                                                             <* 
 *>          sprintf    (x_line, " %-50.50s ", "m --- (null)");                                                                    <* 
 *>          mvprintw   ( i++, 10, x_line);                                                                                        <* 
 *>       } else {                                                                                                                 <* 
 *>          x_len = strlen (x_curr->v_str);                                                                                       <* 
 *>          if (x_len     < 43)  sprintf    (x_temp, "m %3d [%-s]"     , x_len, x_curr->v_str);                                   <* 
 *>          else                 sprintf    (x_temp, "m %3d [%-42.42s>", x_len, x_curr->v_str);                                   <* 
 *>          sprintf    (x_line, " %-50.50s ", x_temp);                                                                            <* 
 *>          mvprintw   ( i++, 10, x_line);                                                                                        <* 
 *>       }                                                                                                                        <* 
 *>       sprintf    (x_line, " t     %c%-43.43s ", x_curr->type, " ");                                                               <* 
 *>       mvprintw   ( i++, 10, x_line);                                                                                           <* 
 *>       sprintf    (x_line, " a     %c%-43.43s ", x_curr->align, " ");                                                               <* 
 *>       mvprintw   ( i++, 10, x_line);                                                                                           <* 
 *>       sprintf    (x_line, " d     %c%-43.43s ", x_curr->decs, " ");                                                               <* 
 *>       mvprintw   ( i++, 10, x_line);                                                                                           <* 
 *>       sprintf    (x_line, " f     %c%-43.43s ", x_curr->format, " ");                                                               <* 
 *>       mvprintw   ( i++, 10, x_line);                                                                                           <* 
 *>       if (x_curr->print == NULL) {                                                                                                 <* 
 *>          sprintf    (x_line, " %-50.50s ", "p --- (null)");                                                                    <* 
 *>          mvprintw   ( i++, 10, x_line);                                                                                        <* 
 *>       } else {                                                                                                                 <* 
 *>          x_len = strlen (x_curr->print);                                                                                           <* 
 *>          if (x_len     < 43)  sprintf    (x_temp, "p %3d [%-s]"     , x_len, x_curr->print);                                       <* 
 *>          else                 sprintf    (x_temp, "p %3d [%-42.42s>", x_len, x_curr->print);                                       <* 
 *>          sprintf    (x_line, " %-50.50s ", x_temp);                                                                            <* 
 *>          mvprintw   ( i++, 10, x_line);                                                                                        <* 
 *>       }                                                                                                                        <* 
 *>       sprintf    (x_line, " nrpn  %-4d%-40.40s ", x_curr->nrpn     , " ");                                                     <* 
 *>       mvprintw   ( i++, 10, x_line);                                                                                           <* 
 *>       sprintf    (x_line, " npro  %-4d%-40.40s ", x_curr->nprovide , " ");                                                     <* 
 *>       mvprintw   ( i++, 10, x_line);                                                                                           <* 
 *>       sprintf    (x_line, " nreq  %-4d%-40.40s ", x_curr->nrequire , " ");                                                     <* 
 *>       mvprintw   ( i++, 10, x_line);                                                                                           <* 
 *>       sprintf    (x_line, " nerr  %-4d%-40.40s ", x_curr->nerror   , " ");                                                     <* 
 *>       mvprintw   ( i++, 10, x_line);                                                                                           <* 
 *>    }                                                                                                                           <* 
 *>    attrset    (0);                                                                                                             <* 
 *>    /+---(footer)-------------------------+/                                                                                    <* 
 *>    attron     (S_COLOR_TITLE);                                                                                                 <* 
 *>    mvprintw   ( i++, 10, x_title);                                                                                             <* 
 *>    attrset    (0);                                                                                                             <* 
*>    /+---(complete)-----------------------+/                                                                                    <* 
*>    return 0;                                                                                                                   <* 
*> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ ------ [gz.A70.061.41]+/ /+-[01.7000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> CURS_listbufs      (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    int         i           = 0;                                                                                                <* 
 *>    char        x_line      [LEN_RECD];                                                                                         <* 
 *>    int         x_row       = 4;                                                                                                <* 
 *>    int         x_col       = 10;                                                                                               <* 
 *>    char       *x_title     = " # ---name-------- col rows -c- t     # ---name-------- col rows -c- t ";                        <* 
 *>    char       *x_leave     = "         please PRESS ANY KEY to escape this information window        ";                        <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    attron    (S_COLOR_VISUAL);                                                                                                 <* 
 *>    mvprintw  (x_row++, x_col, x_leave);                                                                                        <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    attron    (S_COLOR_TITLE);                                                                                                  <* 
 *>    mvprintw  (x_row++, x_col, x_title);                                                                                        <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    /+---(show marks)---------------------+/                                                                                    <* 
 *>    for (i = 0; i < 18; ++i) {                                                                                                  <* 
 *>       /+---(left)------------------------+/                                                                                    <* 
 *>       TAB_line (i, x_line);                                                                                                <* 
 *>       if (i == CTAB     )  attron (S_COLOR_CURRENT);                                                                           <* 
 *>       else                 attron (S_COLOR_VISUAL);                                                                            <* 
 *>       mvprintw   (x_row  , x_col, x_line);                                                                                     <* 
 *>       attrset (0);                                                                                                             <* 
 *>       /+---(separator)-------------------+/                                                                                    <* 
 *>       attron (S_COLOR_TITLE);                                                                                                  <* 
 *>       mvprintw   (x_row  , x_col + 34, "   ");                                                                                 <* 
 *>       attrset (0);                                                                                                             <* 
 *>       /+---(upper case)------------------+/                                                                                    <* 
 *>       TAB_line (i + 18, x_line);                                                                                           <* 
 *>       if (i + 18 == CTAB)  attron (S_COLOR_CURRENT);                                                                           <* 
 *>       else                 attron (S_COLOR_VISUAL);                                                                            <* 
 *>       mvprintw   (x_row++, x_col + 37, x_line);                                                                                <* 
 *>       attrset (0);                                                                                                             <* 
 *>       /+---(done)------------------------+/                                                                                    <* 
 *>    }                                                                                                                           <* 
 *>    /+---(footer)-------------------------+/                                                                                    <* 
 *>    attron (S_COLOR_TITLE);                                                                                                     <* 
 *>    mvprintw   (x_row++, x_col, x_title);                                                                                       <* 
 *>    attrset (0);                                                                                                                <* 
 *>    attron    (S_COLOR_VISUAL);                                                                                                 <* 
 *>    mvprintw  (x_row++, x_col, x_leave);                                                                                        <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ ------ [gz.EA0.0H1.51]+/ /+-[03.7000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> CURS_list_mark       (void)                                                                                                    <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                    <* 
 *>    char        rc          =    0;                                                                                             <* 
 *>    int         i           =    0;                                                                                             <* 
 *>    char        x_marks     [LEN_RECD] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789()";                    <* 
 *>    char        x_mark      =  '-';                                                                                             <* 
 *>    int         x_len       =    0;                                                                                             <* 
 *>    int         x_wide      =    0;                                                                                             <* 
 *>    int         x_row       =    0;                                                                                             <* 
 *>    int         x_col       =   -1;                                                                                             <* 
 *>    cint        x_top       =    5;                                                                                             <* 
 *>    cint        x_left      =   10;                                                                                             <* 
 *>    cint        x_rows      =   26;                                                                                             <* 
 *>    cint        x_buf       =    3;                                                                                             <* 
 *>    char        x_line      [LEN_RECD];                                                                                         <* 
 *>    char        x_label     [10];                                                                                               <* 
 *>    char       *x_equals    = "=============================================";                                                  <* 
 *>    char       *x_title     = " -m- -address-- ";                                                                               <* 
 *>    char       *x_footer    = " press ANY KEY to escape this window ";                                                          <* 
 *>    /+---(prepare)------------------------+/                                                                                    <* 
 *>    x_len  = strlen (x_marks);                                                                                                  <* 
 *>    x_wide = strlen (x_title) + x_buf;                                                                                          <* 
 *>    /+---(show marks)---------------------+/                                                                                    <* 
 *>    for (i = 0; i < x_len; ++i) {                                                                                               <* 
 *>       /+---(position)--------------------+/                                                                                    <* 
 *>       x_col = trunc (i / x_rows);                                                                                              <* 
 *>       x_row = i % x_rows;                                                                                                      <* 
 *>       /+---(title)-----------------------+/                                                                                    <* 
 *>       if (x_row == 0)  {                                                                                                       <* 
 *>          attron (S_COLOR_TITLE);                                                                                               <* 
 *>          mvprintw   ( x_top  - 1    , x_left + x_col * x_wide, x_title);                                                       <* 
 *>          mvprintw   ( x_top + x_rows, x_left + x_col * x_wide, x_title);                                                       <* 
 *>          attrset (0);                                                                                                          <* 
 *>          if (x_col > 0) {                                                                                                      <* 
 *>             attron (S_COLOR_TITLE);                                                                                            <* 
 *>             mvprintw   (x_top - 1     , x_left + x_col * x_wide - x_buf, "   ");                                               <* 
 *>             mvprintw   (x_top + x_rows, x_left + x_col * x_wide - x_buf, "   ");                                               <* 
 *>             attrset (0);                                                                                                       <* 
 *>          }                                                                                                                     <* 
 *>       }                                                                                                                        <* 
 *>       /+---(separator)-------------------+/                                                                                    <* 
 *>       if (x_col > 0) {                                                                                                         <* 
 *>          attron (S_COLOR_TITLE);                                                                                               <* 
 *>          mvprintw   (x_top + x_row, x_left + x_col * x_wide - x_buf, "   ");                                                   <* 
 *>          attrset (0);                                                                                                          <* 
 *>       }                                                                                                                        <* 
 *>       /+---(lower case)------------------+/                                                                                    <* 
 *>       x_mark = x_marks [i];                                                                                                    <* 
 *>       rc = MARK_entry (x_mark, x_line);                                                                                        <* 
 *>       if (rc == 0)  attron (S_COLOR_CURRENT);                                                                                  <* 
 *>       else          attron (S_COLOR_VISUAL);                                                                                   <* 
 *>       mvprintw   (x_top + x_row, x_left + x_col * x_wide, x_line);                                                             <* 
 *>       attrset (0);                                                                                                             <* 
 *>       /+---(done)------------------------+/                                                                                    <* 
 *>    }                                                                                                                           <* 
 *>    /+---(footer)-------------------------+/                                                                                    <* 
 *>    x_len = (strlen (x_title) * ++x_col) + (--x_col * x_buf) - strlen (x_footer);                                               <* 
 *>    sprintf (x_line, "%.*s%s%.*s",                                                                                              <* 
 *>          x_len / 2, x_equals,                                                                                                  <* 
 *>          x_footer,                                                                                                             <* 
 *>          x_len - x_len / 2, x_equals);                                                                                         <* 
 *>    attron (S_COLOR_SOURCE);                                                                                                    <* 
 *>    mvprintw   (x_top + x_rows + 1, x_left, x_line);                                                                            <* 
 *>    attrset (0);                                                                                                                <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ ------ [gz.860.061.11]+/ /+-[01.5000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> CURS_info_layout   (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    int         i           = 0;                                                                                                <* 
 *>    char        x_line      [LEN_RECD];                                                                                         <* 
 *>    int         x_row       = 4;                                                                                                <* 
 *>    int         x_col       = 10;                                                                                               <* 
 *>    char       *x_title     = " ---command---------- ---description-------------------------- ";                                <* 
 *>    char       *x_leave     = "    please PRESS ANY KEY to escape this information window     ";                                <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    attron    (S_COLOR_VISUAL);                                                                                                 <* 
 *>    mvprintw  (x_row++, x_col, x_leave);                                                                                        <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    attron    (S_COLOR_TITLE);                                                                                                  <* 
 *>    mvprintw  (x_row++, x_col, x_title);                                                                                        <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    /+---(show registers)-----------------+/                                                                                    <* 
 *>    attron    (S_COLOR_CURRENT);                                                                                                <* 
 *>    for (i = 0; i < MAX_LAYOUT; ++i) {                                                                                          <* 
 *>       /+---(lower case)------------------+/                                                                                    <* 
 *>       if (PROG_layout_entry  (i, x_line) < 0) break;                                                                           <* 
 *>       mvprintw  (x_row++, x_col, x_line);                                                                                      <* 
 *>    }                                                                                                                           <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    /+---(footer)-------------------------+/                                                                                    <* 
 *>    attron    (S_COLOR_TITLE);                                                                                                  <* 
 *>    mvprintw  (x_row++, x_col, x_title);                                                                                        <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    attron    (S_COLOR_VISUAL);                                                                                                 <* 
 *>    mvprintw  (x_row++, x_col, x_leave);                                                                                        <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ ------ [gz.860.081.21]+/ /+-[01.3000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> CURS_listreg       (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    int         i           = 0;                                                                                                <* 
 *>    char        x_line      [LEN_RECD];                                                                                         <* 
 *>    char        x_reg       = ' ';                                                                                              <* 
 *>    char        x_reglist   [LEN_RECD] = "\"abcdefghijklmnopqrstuvwxyz";                                                        <* 
 *>    int         x_count     = 0;                                                                                                <* 
 *>    int         x_row       = 4;                                                                                                <* 
 *>    int         x_col       = 10;                                                                                               <* 
 *>    char       *x_title     = " -r- tab space count cell toplef botrig reach -min-- -max-- -t- ";                               <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    attron    (S_COLOR_TITLE);                                                                                                  <* 
 *>    mvprintw  (x_row, x_col, x_title);                                                                                          <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    /+---(prepare)------------------------+/                                                                                    <* 
 *>    x_count = strlen (x_reglist);                                                                                               <* 
 *>    /+---(show unnamed)-------------------+/                                                                                    <* 
 *>    /+---(show registers)-----------------+/                                                                                    <* 
 *>    for (i = 0; i < x_count; ++i) {                                                                                             <* 
 *>       ++x_row;                                                                                                                 <* 
 *>       /+---(lower case)------------------+/                                                                                    <* 
 *>       x_reg = x_reglist [i];                                                                                                   <* 
 *>       REG_entry  (x_reg, x_line);                                                                                              <* 
 *>       if (x_line  [6] != '-')  attron (S_COLOR_CURRENT);                                                                       <* 
 *>       else                     attron (S_COLOR_VISUAL);                                                                        <* 
 *>       mvprintw  (x_row, x_col, x_line);                                                                                        <* 
 *>       attrset   (0);                                                                                                           <* 
 *>    }                                                                                                                           <* 
 *>    ++x_row;                                                                                                                    <* 
 *>    /+---(footer)-------------------------+/                                                                                    <* 
 *>    attron    (S_COLOR_TITLE);                                                                                                  <* 
 *>    mvprintw  (x_row, x_col, x_title);                                                                                          <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ ------ [gz.860.081.21]+/ /+-[01.3000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> CURS_listtreg      (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    int         i           = 0;                                                                                                <* 
 *>    char        x_line      [LEN_RECD];                                                                                         <* 
 *>    char        x_reg       = ' ';                                                                                              <* 
 *>    char        x_reglist   [LEN_RECD] = "\"abcdefghijklmnopqrstuvwxyz";                                                        <* 
 *>    int         x_count     = 0;                                                                                                <* 
 *>    int         x_row       = 4;                                                                                                <* 
 *>    int         x_col       = 10;                                                                                               <* 
 *>    char       *x_title     = " -r- len -data------------------------------------- -label- beg end  s  ";                       <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    attron    (S_COLOR_TITLE);                                                                                                  <* 
 *>    mvprintw  (x_row, x_col, x_title);                                                                                          <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    /+---(prepare)------------------------+/                                                                                    <* 
 *>    x_count = strlen (x_reglist);                                                                                               <* 
 *>    /+---(show unnamed)-------------------+/                                                                                    <* 
 *>    /+---(show registers)-----------------+/                                                                                    <* 
 *>    for (i = 0; i < x_count; ++i) {                                                                                             <* 
 *>       ++x_row;                                                                                                                 <* 
 *>       /+---(lower case)------------------+/                                                                                    <* 
 *>       x_reg = x_reglist [i];                                                                                                   <* 
 *>       TREG_entry  (x_reg, x_line);                                                                                             <* 
 *>       if (x_line  [7] != '-')  attron (S_COLOR_CURRENT);                                                                       <* 
 *>       else                     attron (S_COLOR_VISUAL);                                                                        <* 
 *>       mvprintw  (x_row, x_col, x_line);                                                                                        <* 
 *>       attrset   (0);                                                                                                           <* 
 *>    }                                                                                                                           <* 
 *>    ++x_row;                                                                                                                    <* 
 *>    /+---(footer)-------------------------+/                                                                                    <* 
 *>    attron    (S_COLOR_TITLE);                                                                                                  <* 
 *>    mvprintw  (x_row, x_col, x_title);                                                                                          <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ ------ [gz.750.171.21]+/ /+-[01.3000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> CURS_listerror     (tCELL *a_cell)                                                                                             <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    int         i           = 0;                                                                                                <* 
 *>    char        x_line      [LEN_RECD];                                                                                         <* 
 *>    char       *x_title     = " no p sq --------------- s ---------------------------------------- ";                           <* 
 *>    int         x_row       = 4;                                                                                                <* 
 *>    int         x_col       = 10;                                                                                               <* 
 *>    int         x_count     = 0;                                                                                                <* 
 *>    char        rc          = 0;                                                                                                <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    attron    (S_COLOR_TITLE);                                                                                                  <* 
 *>    mvprintw  (x_row, x_col, x_title);                                                                                          <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    /+---(prepare)------------------------+/                                                                                    <* 
 *>    rc = ERROR_entry (a_cell, '*', x_line);                                                                                     <* 
 *>    /+---(show registers)-----------------+/                                                                                    <* 
 *>    while (rc == 0) {                                                                                                           <* 
 *>       ++x_row;                                                                                                                 <* 
 *>       attron (S_COLOR_CURRENT);                                                                                                <* 
 *>       mvprintw  (x_row, x_col, x_line);                                                                                        <* 
 *>       attrset   (0);                                                                                                           <* 
 *>       rc = ERROR_entry (a_cell, '-', x_line);                                                                                  <* 
 *>       if (x_row > 10) break;                                                                                                   <* 
 *>    }                                                                                                                           <* 
 *>    /+---(footer)-------------------------+/                                                                                    <* 
 *>    ++x_row;                                                                                                                    <* 
 *>    attron    (S_COLOR_TITLE);                                                                                                  <* 
 *>    mvprintw  (x_row, x_col, x_title);                                                                                          <* 
 *>    attrset   (0);                                                                                                              <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/



/*====================------------------------------------====================*/
/*===----                      screen management                       ----===*/
/*====================------------------------------------====================*/
void  o___SCREEN__________o () { return; }

/*> char         /+-> capture keyboard input -------------[ leaf   [gc.320.012.00]+/ /+-[00.0001.102.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> CURS_playback      (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    char        ch          = ' ';                                                                                              <* 
 *>    IF_MACRO_OFF    return 0;                                                                                                   <* 
 *>    IF_MACRO_MOVING nodelay (stdscr, TRUE );                                                                                    <* 
 *>    ch = getch ();                                                                                                              <* 
 *>    IF_MACRO_MOVING nodelay (stdscr, FALSE);                                                                                    <* 
 *>    return ch;                                                                                                                  <* 
 *> }                                                                                                                              <*/


char         /*-> set color model --------------------[ ------ [gc.D70.532.S5]*/ /*-[02.2000.015.!]-*/ /*-[--.---.---.--]-*/
DRAW_coloration      (char *a_opt)
{
   if       (a_opt == NULL)                s_coloration = '-';
   else if  (strcmp (a_opt, "min" ) == 0)  s_coloration = '-';
   else if  (strcmp (a_opt, "full") == 0)  s_coloration = 'F';
   else if  (strcmp (a_opt, "max" ) == 0)  s_coloration = 'F';
   return 0;
}

char         /*-> set full color screen --------------[ ------ [gc.D70.532.S5]*/ /*-[02.2000.015.!]-*/ /*-[--.---.---.--]-*/
CURS_color_min       (int a_col, int a_row, tCELL *a_curr)
{
   /*---(current)------------------------*/
   if      (a_col == CCOL && a_row == CROW)             attron (S_COLOR_HCURR  );
   else if (a_curr != NULL && a_curr->note == 's')         attron (S_COLOR_HUSED  );
   /*---(visual-range)-------------------*/
   else if (yVIKEYS_root   (CTAB, a_col, a_row, NULL))  attron (S_COLOR_ROOT   );
   else if (yVIKEYS_visual (CTAB, a_col, a_row, NULL))  attron (S_COLOR_VISUAL );
   else                                                 attron (S_COLOR_HNORM  );
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> set full color screen --------------[ ------ [gc.D70.532.S5]*/ /*-[02.2000.015.!]-*/ /*-[--.---.---.--]-*/
CURS_color_full    (int a_col, int a_row, tCELL *a_curr)
{
   /*---(locals)---------------------------*/
   char        label     [LEN_RECD] = "zzz";
   char        l         [LEN_RECD]  = "";
   /*---(identify cell)--------------------*/
   if (a_curr != NULL) {
      LOC_label  (a_curr, l);
      sprintf    (label, ",%s,", l);
   } else {
      str4gyges  (CTAB, a_col, a_row, 0, 0, l, YSTR_CHECK);
      sprintf    (label, ",%s,", l);
   }
   /*---(current)------------------------*/
   if      (a_col == CCOL && a_row == CROW)             attron (S_COLOR_CURRENT);
   else if (a_curr != NULL && a_curr->note == 's')         attron (S_COLOR_SEARCH );
   /*---(visual-range)-------------------*/
   else if (yVIKEYS_root   (CTAB, a_col, a_row, NULL))        attron (S_COLOR_ROOT   );
   else if (yVIKEYS_visual (CTAB, a_col, a_row, NULL))        attron (S_COLOR_VISUAL );
   /*---(marks)--------------------------*/
   else if (my.mark_show  == 'y' &&
         strstr (s_mark_list, label) != NULL)           attron (S_COLOR_MARK     );
   /*---(content-based)------------------*/
   else if (a_curr != NULL) {
      /*---(related)---------------------*/
      if      (strstr (my.reqs_list, label) != NULL)    attron (S_COLOR_REQS   );
      else if (strstr (my.deps_list, label) != NULL)    attron (S_COLOR_PROS   );
      else if (strstr (my.like_list, label) != NULL)    attron (S_COLOR_LIKE   );
      /*---(trouble)---------------------*/
      else if (a_curr->type == YCALC_DATA_ERROR)           attron (S_COLOR_ERROR  );
      /*---(pointers)--------------------*/
      else if (a_curr->type == YCALC_DATA_RANGE)           attron (S_COLOR_POINTER);
      else if (a_curr->type == YCALC_DATA_ADDR )           attron (S_COLOR_POINTER);
      /*---(numbers)---------------------*/
      else if (a_curr->type == YCALC_DATA_NUM  )           attron (S_COLOR_NUMBER );
      else if (a_curr->type == YCALC_DATA_NFORM) {
         if   (yCALC_nreq (a_curr->ycalc)  < 5)         attron (S_COLOR_FSIMPLE);
         else                                           attron (S_COLOR_FDANGER);
      }
      else if (a_curr->type == YCALC_DATA_NLIKE)           attron (S_COLOR_FLIKE  );
      /*> else if (a_curr->type == YCALC_DATA_NLIKE)           attron (S_COLOR_LIKE  );   <*/
      /*---(strings)---------------------*/
      else if (a_curr->type == YCALC_DATA_STR  )           attron (S_COLOR_STRING );
      else if (a_curr->type == YCALC_DATA_SFORM) {
         if   (yCALC_nreq (a_curr->ycalc)  < 5)         attron (S_COLOR_FSTRING);
         else                                           attron (S_COLOR_FSTRDAG);
      }
      else if (a_curr->type == YCALC_DATA_SLIKE)           attron (S_COLOR_MLIKE  );
      /*> else if (a_curr->type == YCALC_DATA_SLIKE)           attron (S_COLOR_LIKE  );   <*/
      /*---(constants)-------------------*/
      else if (a_curr->type == YCALC_DATA_BLANK)           attron (S_COLOR_NULL   );
      else                                              attron (S_COLOR_STRING );
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> display an individual cell ---------[ ------ [gc.D70.532.S5]*/ /*-[02.2000.015.!]-*/ /*-[--.---.---.--]-*/
CURS_cell          (int a_col, int a_row, short a_ypos, short a_xpos, short a_wide)
{
   /*---(locals)-------------------------*/
   tCELL      *x_curr      = LOC_cell_at_loc (CTAB, a_col, a_row);
   /*---(save cursor position)-----------*/
   if (a_col == CCOL && a_row == CROW) {
      s_cursor_x = a_xpos + a_wide - 1;
      s_cursor_y = a_ypos;
   }
   /*---(set color)----------------------*/
   if (s_coloration == 'F')  CURS_color_full (a_col, a_row, x_curr);
   else                      CURS_color_min  (a_col, a_row, x_curr);
   /*---(display cell)-------------------*/
   if (x_curr == NULL || x_curr->print == NULL)  {
      DEBUG_GRAF_M  yLOG_complex ("CURS_cell" , "%2dc, %2dr, %3dx, %3dy, %2dw, :%s:", a_col, a_row, a_xpos, a_ypos, a_wide, "");
      mvprintw (a_ypos, a_xpos, "%-*.*s", a_wide, a_wide, g_empty);
   } else {
      DEBUG_GRAF_M  yLOG_complex ("CURS_cell" , "%2dc, %2dr, %3dx, %3dy, %2dw, :%s:", a_col, a_row, a_xpos, a_ypos, a_wide, x_curr->print);
      mvprintw (a_ypos, a_xpos, "%-*.*s", a_wide, a_wide, x_curr->print);
   }
   /*---(highlight off)------------------*/
   attrset (0);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gn.860.371.25]*/ /*-[03.0000.014.!]-*/ /*-[--.---.---.--]-*/
DRAW_main          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         ch          = 0;
   tCELL      *x_curr      = NULL;
   static tCELL  *x_save      =    1;
   int         x_left, x_wide;
   int         x_bott, x_tall;
   int         y_cur       = 0;
   int         x_cur       = 0;
   short       y_pos       = 0;
   short       x_pos       = 0;
   int         w           = 0;
   int         cw          = 0;
   /*---(update globals)-----------------*/
   x_curr    = LOC_cell_at_curr ();
   /*> if (x_curr != x_save) {                                                                   <* 
    *>    if (x_curr != NULL) {                                                                  <* 
    *>       yCALC_disp_reqs (x_curr->ycalc, my.reqs_list);                                      <* 
    *>       yCALC_disp_pros (x_curr->ycalc, my.deps_list);                                      <* 
    *>       yCALC_disp_like (x_curr->ycalc, my.like_list);                                      <* 
    *>       /+> if (x_curr->rpn != NULL)  strlcpy (my.rpn_list, x_curr->rpn, LEN_RECD);   <*    <* 
    *>        *> else                      strncpy (my.rpn_list , "n/a", LEN_RECD);        <+/   <* 
    *>    } else {                                                                               <* 
    *>       strncpy (my.reqs_list, "n/a", LEN_RECD);                                            <* 
    *>       strncpy (my.deps_list, "n/a", LEN_RECD);                                            <* 
    *>       strncpy (my.like_list, "n/a", LEN_RECD);                                            <* 
    *>       strncpy (my.rpn_list , "n/a", LEN_RECD);                                            <* 
    *>    }                                                                                      <* 
    *> }                                                                                         <*/
   x_save = x_curr;
   /*> REG_list   (my.reg_curr  , my.reg_list);                                       <*/
   strncpy (s_mark_list, "+", LEN_RECD);
   yVIKEYS_hint_marklist  (s_mark_list);
   /*---(display all)--------------------*/
   yVIKEYS_view_size     (YVIKEYS_MAIN, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   for (y_cur = BROW; y_cur <= EROW; ++y_cur) {
      /*---(prepare)---------------------*/
      y_pos = x_bott - x_tall + 1 + (y_cur - BROW);
      x_pos = x_left;
      cw    = 0;
      /*---(cycle normal columns)--------*/
      for (x_cur = BCOL; x_cur <= ECOL; ++x_cur) {
         w  = COL_size  (CTAB, x_cur);
         if (cw + w > x_wide)  w = x_wide - cw;
         CURS_cell (x_cur, y_cur, y_pos, x_pos, w);
         cw    += w;
         x_pos += w;
      }
      /*---(fill in at end)--------------*/
      if (x_pos < x_wide) {
         w     = x_wide - x_pos;
         if (ECOL < NCOL - 1) {
            CURS_cell (ECOL + 1, y_cur, y_pos, x_pos, w);
         }
      }
   }
   /*> w = g_xmap.tend - x_pos;                                                       <* 
    *> if (w > 0)  CURS_cell (ECOL + 1, y_cur, y_pos, x_pos, w);                      <*/
   move (s_cursor_y, s_cursor_x);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       support functions                      ----===*/
/*====================------------------------------------====================*/
static void  o___SUPPORT_________o () { return; }

char         /*-> resize screen ----------------------[ leaf   [gz.851.021.40]*/ /*-[00.0000.123.!]-*/ /*-[--.---.---.--]-*/
CURS_size         (void)
{
   DEBUG_GRAF  yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------------------------*/
   int    y;
   int    x;
   /*---(get dimensions)--------------*/
   getmaxyx (stdscr, y, x);
   /*---(set positions)---------------*/
   row_formula  = 0;
   row_main     = 2;
   s_status_row   = y - 2;
   if (my.layout_status  != G_STATUS_HIDE ) s_status_size  = 1;
   else                                     s_status_size  = 0;
   s_command_row  = y - 1;
   if (my.layout_command != G_COMMAND_HIDE) s_command_size = 1;
   else                                     s_command_size = 0;
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

char         /*-> initiate curses --------------------[ ------ [gz.421.001.02]*/ /*-[00.0000.102.!]-*/ /*-[--.---.---.--]-*/
DRAW_init          (void)
{
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(initialize)------------------*/
   yVIKEYS_view_basic    (YVIKEYS_MAIN , YVIKEYS_FLAT, YVIKEYS_TOPLEF, 0, DRAW_main);
   yVIKEYS_view_defsize  (YVIKEYS_YAXIS    , 5, 0);
   yVIKEYS_view_simple   (YVIKEYS_XAXIS    , 0, DRAW_xaxis  );
   yVIKEYS_view_simple   (YVIKEYS_YAXIS    , 0, DRAW_yaxis  );
   /*> yVIKEYS_view_simple   (YVIKEYS_BUFFER   , 0, DRAW_buffer );                    <*/
   yVIKEYS_cmds_direct   (":layout gyges");
   yVIKEYS_cmds_add      (YVIKEYS_M_VIEW  , "coloration"  , "col" , "s"    , DRAW_coloration            , "" );
   /*---(get window size)-------------*/
   /*> CURS_size   ();                                                                <*/
   /*---(colors)----------------------*/
   /*> COLOR_init  ();                                                                <*/
   /*---(window)---------*/
   S_COLOR_TITLE      = yCOLOR_curs_value ("title"    );
   S_COLOR_TITLEE     = yCOLOR_curs_value ("error"    );
   S_COLOR_STATUS     = yCOLOR_curs_value ("status"   );
   S_COLOR_STATUSE    = yCOLOR_curs_value ("error"    );
   S_COLOR_KEYS       = yCOLOR_curs_value ("keys"     );
   S_COLOR_MESSAGE    = yCOLOR_curs_value ("command"  );
   S_COLOR_MENU       = yCOLOR_curs_value ("menu"     );
   /*---(trouble)--------*/
   S_COLOR_ERROR      = yCOLOR_curs_value ("error"    );
   S_COLOR_WARN       = yCOLOR_curs_value ("warn"     );
   S_COLOR_FOUND      = yCOLOR_curs_value ("found"    );
   /*---(formula)--------*/
   S_COLOR_CONTENT    = yCOLOR_curs_value ("map"      );
   S_COLOR_SOURCE     = yCOLOR_curs_value ("source"   );
   S_COLOR_INPUT      = yCOLOR_curs_value ("input"    );
   S_COLOR_TEXREG     = yCOLOR_curs_value ("textreg"  );
   S_COLOR_REPLACE    = yCOLOR_curs_value ("replace"  );
   S_COLOR_WANDER     = yCOLOR_curs_value ("wander"   );
   /*---(row and column headers)---------*/
   S_COLOR_HCURR      = yCOLOR_curs_value ("h_current");
   S_COLOR_HLOCK      = yCOLOR_curs_value ("h_locked" );
   S_COLOR_HUSED      = yCOLOR_curs_value ("h_used"   );
   S_COLOR_HNORM      = yCOLOR_curs_value ("h_normal" );
   /*---(selection)----------------------*/
   S_COLOR_CURRENT    = yCOLOR_curs_value ("curr"     );
   S_COLOR_ROOT       = yCOLOR_curs_value ("root"     );
   S_COLOR_VISUAL     = yCOLOR_curs_value ("visu"     );
   S_COLOR_MARK       = yCOLOR_curs_value ("mark"     );
   S_COLOR_SEARCH     = yCOLOR_curs_value ("srch"     );
   /*---(dep type)-------*/
   S_COLOR_REQS       = yCOLOR_curs_add   ("reqs"     , ' ', "value required from cell"                           , 'm' , 'm' , 'b');
   S_COLOR_PROS       = yCOLOR_curs_add   ("pros"     , ' ', "value provided to cell"                             , 'g' , 'g' , 'b');
   S_COLOR_LIKE       = yCOLOR_curs_add   ("like"     , ' ', "formula is copy/variation"                          , 'g' , ' ' , '-');
   /*---(danger signs)---*/
   S_COLOR_FDANGER    = yCOLOR_curs_add   ("fdang"    , 'f', "complex numeric formula"                            , 'r' , ' ' , 'y');
   S_COLOR_FSTRDAG    = yCOLOR_curs_add   ("mdang"    , 'm', "complex string formula"                             , 'r' , ' ' , 'y');
   /*---(cell types)-----*/
   S_COLOR_NUMBER     = yCOLOR_curs_add   ("num"      , 'n', "numeric literal"                                    , 'b' , ' ' , 'y');
   S_COLOR_FSIMPLE    = yCOLOR_curs_add   ("for"      , 'f', "numeric formula"                                    , 'g' , ' ' , 'y');
   S_COLOR_FLIKE      = yCOLOR_curs_add   ("flike"    , 'l', "numeric formula (copy)"                             , 'w' , ' ' , '-');
   S_COLOR_STRING     = yCOLOR_curs_add   ("str"      , 's', "string literal"                                     , 'y' , ' ' , 'y');
   S_COLOR_FSTRING    = yCOLOR_curs_add   ("mod"      , 'm', "string formula"                                     , 'm' , ' ' , 'y');
   S_COLOR_MLIKE      = yCOLOR_curs_add   ("mlike"    , 'L', "string formula (copy)"                              , 'w' , ' ' , '-');
   S_COLOR_POINTER    = yCOLOR_curs_add   ("range"    , 'p', "range pointer"                                      , 'c' , ' ' , 'y');
   S_COLOR_ADDRESS    = yCOLOR_curs_add   ("addr"     , 'p', "address pointer"                                    , 'c' , ' ' , 'y');
   S_COLOR_NULL       = yCOLOR_curs_add   ("blank"    , '-', "blank cell"                                         , 'b' , ' ' , 'y');
   S_COLOR_NORMAL     = yCOLOR_curs_add   ("def"      , ' ', "default for unidentified cells"                     , 'y' , ' ' , 'y');
   /*---(complete)--------------------*/
   DEBUG_PROG  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> shutdown curses --------------------[ leaf   [gz.211.001.00]*/ /*-[00.0000.101.!]-*/ /*-[--.---.---.--]-*/
DRAW_wrap          (void)
{
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*> endwin();        /+ shut down ncurses                                      +/   <*/
   /*---(complete)--------------------*/
   DEBUG_PROG  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*============================----end-of-source---============================*/
