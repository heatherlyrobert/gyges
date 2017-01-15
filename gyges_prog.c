/*============================----beg-of-source---============================*/

#include   "gyges.h"

char          unit_answer [ LEN_TEXT ];


/*====================------------------------------------====================*/
/*===----                        program wide                          ----===*/
/*====================------------------------------------====================*/

char       /*----: display command line help/usage ---------------------------*/
PROG_usage         (void)
{
   printf("\n");
   printf("s : light, clean, fast, and aesthetic console-based spreadsheet\n");
   printf("\n");
   printf("usage : s [urgents] [options]\n");
   printf("\n");
   printf("standard urgents -- debugging tracing/information\n");
   printf("   startup---  ---------------------------\n");
   printf("   @c          command line arguments\n");
   printf("   @x          setup and teardown\n");
   printf("   @i          file input\n");
   printf("   process---  ---------------------------\n");
   printf("   @e          xwindows events/input\n");
   printf("   @g          ncurses/opengl graphics\n");
   printf("   @p          processing\n");
   printf("   @d          data structures/mgmt\n");
   printf("   @a          programming api\n");
   printf("   wrapup----  ---------------------------\n");
   printf("   @o          file output\n");
   printf("\n");
   printf("specific urgents -- debugging tracing/information\n");
   printf("   @@averages  conversion of raw to basic to average points\n");
   printf("   @@circles   circle identificition and marking\n");
   printf("   @@curves    curve marking\n");
   printf("   @@keys      key point identification and marking\n");
   printf("   @@deps      dependency creation and processing\n");
   printf("\n");
   exit(0);
}

char      verstring    [500];

char*      /* ---- : return library versioning information -------------------*/
PROG_version       (void)
{
   char    t [20] = "";
#if    __TINYC__ > 0
   strncpy (t, "[tcc built  ]", 15);
#elif  __GNUC__  > 0
   strncpy (t, "[gnu gcc    ]", 15);
#elif  __CBANG__  > 0
   strncpy (t, "[cbang      ]", 15);
#else
   strncpy (t, "[unknown    ]", 15);
#endif
   snprintf (verstring, 100, "%s   %s : %s", t, VER_NUM, VER_TXT);
   return verstring;
}

/*> char         /+--: evaluate logger needs early -----------[ leaf   [ ------ ]-+/   <* 
 *> PROG_logger        (int a_argc, char *a_argv[])                                    <* 
 *> {                                                                                  <* 
 *>    /+---(locals)-----------+-----------+-+/                                        <* 
 *>    int         i           = 0;                                                    <* 
 *>    char       *a           = NULL;                                                 <* 
 *>    char        x_prog      [LEN_STR] = "";                                         <* 
 *>    char        x_log       = '-';                                                  <* 
 *>    /+---(default urgents)----------------+/                                        <* 
 *>    PROG_urgsmass  ('-', 'y');   /+ turn everything off +/                          <* 
 *>    debug.logger   = -1;                                                            <* 
 *>    strlcpy (x_prog, a_argv [0], LEN_STR);                                          <* 
 *>    /+---(test for normal version)--------+/                                        <* 
 *>    if        (strcmp (a_argv[0], "gyges"        ) == 0)  return 0;                 <* 
 *>    /+---(check for urgents)--------------+/                                        <* 
 *>    for (i = 1; i < a_argc; ++i) {                                                  <* 
 *>       a = a_argv[i];                                                               <* 
 *>       if (a[0] != '@')  continue;                                                  <* 
 *>       x_log = 'y';                                                                 <* 
 *>    }                                                                               <* 
 *>    if (x_log != 'y')  return 0;                                                    <* 
 *>    /+---(startup logging)----------------+/                                        <* 
 *>    debug.tops     = 'y';                                                           <* 
 *>    if (strcmp (a_argv [0], "gyges_debug"  ) == 0)                                  <* 
 *>       strlcpy (x_prog, "gyges"         , LEN_STR);                                 <* 
 *>    debug.logger = yLOG_begin (x_prog, yLOG_SYSTEM    , yLOG_NOISE);                <* 
 *>    /+---(complete)-----------------------+/                                        <* 
 *>    return 0;                                                                       <* 
 *> }                                                                                  <*/

char       /*----: very first setup ------------------s-----------------------*/
PROG_init          (int a_argc, char *a_argv[])
{
   /*---(log header)---------------------*/
   DEBUG_TOPS   yLOG_info     ("purpose" , "light, clean, vim-ish, technical, and wicked spreadsheet");
   DEBUG_TOPS   yLOG_info     ("namesake", "ugly, impossibly powerful, one-hundred handed, fifty headed, primeval giant");
   DEBUG_TOPS   yLOG_info     ("gyges"   , PROG_version    ());
   DEBUG_TOPS   yLOG_info     ("yURG"    , yURG_version    ());
   DEBUG_TOPS   yLOG_info     ("ySTR"    , ySTR_version    ());
   DEBUG_TOPS   yLOG_info     ("yLOG"    , yLOG_version    ());
   DEBUG_TOPS   yLOG_info     ("yRPN"    , yRPN_version    ());
   DEBUG_TOPS   yLOG_info     ("yVIKEYS" , yVIKEYS_version ());
   /*---(header)-------------------------*/
   DEBUG_TOPS   yLOG_exit  (__FUNCTION__);
   /*---(initialize)---------------------*/
   hist_active = '-';
   nhist       =  0;
   chist       = -1;
   yVIKEYS_mode_init   ();
   yVIKEYS_mode_enter  (MODE_MAP);
   my.scrn     = SCRN_DEBUG;
   my.info_win = '-';
   my.menu     = ' ';
   /*---(complete)-----------------------*/
   DEBUG_TOPS   yLOG_exit  (__FUNCTION__);
   return 0;
}

char               /* PURPOSE : process the command line arguments            */
PROG_args          (int argc, char *argv[])
{
   DEBUG_TOPS  yLOG_enter   (__FUNCTION__);
   /*---(locals)-------------------------*/
   int         i           = 0;
   char       *a           = NULL;
   int         x_total     = 0;
   int         x_args      = 0;
   /*---(begin)--------------------------*/
   strncpy(my.f_name  , FILE_BLANK , MAX_STR);
   strncpy(my.f_suffix, FILE_SUFFIX, MAX_STR);
   /*---(process)------------------------*/
   for (i = 1; i < argc; ++i) {
      a = argv[i];
      ++x_total;
      if (a[0] == '@')  continue;
      DEBUG_ARGS  yLOG_info    ("cli arg", a);
      ++x_args;
      if      (strncmp(a, "-f"        ,10) == 0)  strncpy (my.f_name , argv[++i], MAX_STR);
      else if (strncmp(a, "-h"        ,10) == 0)  PROG_usage();
      else if (strncmp(a, "--help"    ,10) == 0)  PROG_usage();
      else if (strncmp(a, "--tiny"    ,10) == 0)  my.scrn = SCRN_TINY;
      else if (strncmp(a, "--small"   ,10) == 0)  my.scrn = SCRN_SMALL;
      else if (strncmp(a, "--debug"   ,10) == 0)  my.scrn = SCRN_DEBUG;
      else if (strncmp(a, "--status"  ,10) == 0)  sta_type = argv[++i][0];
      else if (a[0] != '-'                     )  strncpy (my.f_name , argv[i]  , MAX_STR);
   }
   DEBUG_ARGS  yLOG_value  ("entries"   , x_total);
   DEBUG_ARGS  yLOG_value  ("arguments" , x_args);
   if (x_args == 0) {
      DEBUG_ARGS  yLOG_note   ("no arguments identified");
   }
   /*---(update title)-------------------*/
   sprintf(my.f_title , "%s.%s", my.f_name, my.f_suffix);
   /*---(complete)-----------------------*/
   DEBUG_TOPS  yLOG_exit  (__FUNCTION__);
   return 0;
}

char                /* PURPOSE : initialize program and key variables --------*/
PROG_begin         (void)
{
   DEBUG_PROG  yLOG_enter (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   int         i, j, k;
   char        tmp         [100];
   /*---(clear)--------------------------*/
   CELL_init ();
   MARK_init ();
   /*---(overall tab settings)-----------*/
   DEBUG_PROG  yLOG_note  ("clean tab settings");
   NTAB    = 1;
   CTAB    = 0;
   tab  = &tabs[CTAB];
   /*---(clean each tab)-----------------*/
   DEBUG_PROG  yLOG_note  ("clean tabs contents");
   for (i = 0; i < MAX_TABS; ++i) {
      /*---(main config)-----------------*/
      tabs[i].active  = '-';
      sprintf (tmp, "tab%02d", i);
      strcpy  (tabs[i].name, tmp);
      tabs[i].c       =    0;
      /*---(size limits)-----------------*/
      tabs[i].ncol    = DEF_COLS;
      tabs[i].nrow    = DEF_ROWS;
      /*---(current position)------------*/
      tabs[i].ccol    =    0;
      tabs[i].crow    =    0;
      /*---(screen position)-------------*/
      tabs[i].bcol    =    0;
      tabs[i].brow    =    0;
      tabs[i].ecol    =    0;
      tabs[i].erow    =    0;
      /*---(initialize columns)----------*/
      for (j = 0; j < MAX_COLS; ++j) {
         tabs[i].cols[j].w        = DEF_WIDTH;
         tabs[i].cols[j].x        = 0;
         tabs[i].cols[j].c        = 0;
         if        (j < 26)  {
            tabs[i].cols[j].l[0] = '-';
            tabs[i].cols[j].l[1] = j + 'a';
         } else  {
            tabs[i].cols[j].l[0] = (j / 26) - 1 + 'a';
            tabs[i].cols[j].l[1] = (j % 26) + 'a';
         }
         tabs[i].cols[j].l[2] = '\0';
      }
      /*---(initialize rows)-------------*/
      for (j = 0; j < MAX_ROWS; ++j) {
         tabs[i].rows[j].h = DEF_HEIGHT;
         tabs[i].rows[j].y = 0;
         tabs[i].rows[j].c = 0;
      }
      /*---(clean cells)-----------------*/
      for (j = 0; j < MAX_COLS; ++j) {
         for (k = 0; k < MAX_ROWS; ++k) {
            tabs[i].sheet[j][k] = NULL;
         }
      }
      /*---(locked row/col)--------------*/
      tabs[i].froz_col  = '-';
      tabs[i].froz_bcol = 0;
      tabs[i].froz_ecol = 0;
      tabs[i].froz_row  = '-';
      tabs[i].froz_brow = 0;
      tabs[i].froz_erow = 0;
      /*---(done)------------------------*/
   }
   /*---(locals)-------------------------*/
   VISU_init  ();
   REG_init  ();
   KEYS_init ();
   /*---(column settings)----------------*/
   BCOL =    0;
   ECOL =    tabs[0].ncol - 1;
   /*---(row settings)-------------------*/
   BROW =    0;
   EROW =    tabs[0].nrow - 1;
   /*---(char settings)------------------*/
   my.npos =    1;
   my.cpos =    0;
   my.bpos =    0;
   my.epos =    my.npos - 1;;
   /*---(prepare)------------------------*/
   CALC_init ();
   DEP_init  ();
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   return 0;
}

char                /* PURPOSE : shutdown program and free memory ------------*/
PROG_end           (void)
{
   /*> printf ("ending program now.\n");                                              <*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   CELL_wrap    ();
   DEP_wrap     ();   /* disconnect all cells */
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   DEBUG_TOPS   yLOG_end     ();
   return 0;
}

char*
unit_accessor(char *a_question, void *a_thing)
{
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_curr      = (tCELL*) a_thing;
   tDEP       *x_deps      = (tDEP*)  a_thing;
   int         x_thing     = (int) a_thing;
   int         len         = 0;
   char        temp        [MAX_STR];
   int         x_fore      = 0;
   int         x_back      = 0;
   /*---(sheet focus)--------------------*/
   if        (strcmp(a_question, "sheet_size")     == 0) {
      snprintf(unit_answer, LEN_TEXT, "sheet_size       : ncell=%4d, ncol=%3d, nrow=%3d", ncell, tab->ncol, tab->nrow);
   } else if (strcmp(a_question, "cell_list")      == 0) {
      snprintf(unit_answer, LEN_TEXT, "Cell Linked List : n=%4d, h=%9p, t=%9p", ncell, hcell, tcell);
   } else if (strcmp(a_question, "cell_count")     == 0) {
      /*> x_curr = hcell; while (x_curr != NULL) { printf("%2d) c=%4d, r=%4d, p=%9p\n", x_fore, x_curr->col, x_curr->row, x_curr); ++x_fore; x_curr = x_curr->next; }   <*/
      x_curr = hcell; while (x_curr != NULL) { ++x_fore; x_curr = x_curr->next; }
      x_curr = tcell; while (x_curr != NULL) { ++x_back; x_curr = x_curr->prev; }
      snprintf(unit_answer, LEN_TEXT, "Cell Links Count : n=%4d, f=%4d, b=%4d", ncell, x_fore, x_back);
   } else if (strcmp(a_question, "mode")           == 0) {
      snprintf(unit_answer, LEN_TEXT, "Mode             : %c", yVIKEYS_mode_curr ());
   }
   /*---(cell focus)---------------------*/
   else if   (strcmp(a_question, "cell_where")     == 0) {
      if (x_curr == NULL)  snprintf(unit_answer, LEN_TEXT, "s_cell loc       : p=%10p, t=%4d, c=%4d, r=%4d", NULL  , -1         , -1         , -1         );
      else                 snprintf(unit_answer, LEN_TEXT, "s_cell loc       : p=%10p, t=%4d, c=%4d, r=%4d", x_curr, x_curr->tab, x_curr->col, x_curr->row);
   } else if (strcmp(a_question, "sheet_who")      == 0) {
      snprintf(unit_answer, LEN_TEXT, "Sheet Location   : p=%9p", x_curr);
   } else if (strcmp(a_question, "cell_info")      == 0) {
      snprintf(unit_answer, LEN_TEXT, "Cell Information : t=%c f=%c d=%c a=%c c=%3d r=%3d d=%3d", x_curr->t, x_curr->f, x_curr->d, x_curr->a, x_curr->nrpn, x_curr->nrequire, x_curr->nprovide);
   }
   /*---(cell contents)------------------*/
   else if   (strcmp(a_question, "cell_source")    == 0) {
      snprintf(unit_answer, LEN_TEXT, "Cell Source      : (%5d) :%-.40s:", x_curr->l, x_curr->s);
   } else if (strcmp(a_question, "cell_value")     == 0) {
      snprintf(unit_answer, LEN_TEXT, "Cell Value       : %18.6F", x_curr->v_num);
   } else if (strcmp(a_question, "cell_modified")  == 0) {
      snprintf(unit_answer, LEN_TEXT, "Cell Modified    : (%4d) %-.40s", (int) strlen(x_curr->v_str), x_curr->v_str);
   } else if (strcmp(a_question, "cell_printable") == 0) {
      /*> snprintf(unit_answer, LEN_TEXT, "Cell Printable   : (%4d) :%-.40s:", (int) strlen(x_curr->p), x_curr->p);   <*/
      snprintf(unit_answer, LEN_TEXT, "Cell Printable   : (%4d) :%s:", (int) strlen(x_curr->p), x_curr->p);
   } else if (strcmp(a_question, "cell_contents")  == 0) {
      snprintf(unit_answer, LEN_TEXT, "Contents     (%c) : (%2d:%2d) :%-.40s:", (g_contents[my.cpos] >= ' ' && g_contents[my.cpos] <= '~') ? g_contents[my.cpos] : ' ', my.cpos, (int) strlen(g_contents), g_contents);
   }
   /*---(cell contents)------------------*/
   else if   (strcmp(a_question, "cell_rpn")       == 0) {
      snprintf(unit_answer, LEN_TEXT, "Cell RPN         : (%3d) %-.40s", x_curr->nrpn, x_curr->rpn);
   }
   /*---(display focus)------------------*/
   else if   (strcmp(a_question, "rows")           == 0) {
      snprintf(unit_answer, LEN_TEXT, "Rows             : n=%3d, a=%3d, b=%3d, c=%3d, e=%3d", tab->nrow, my.y_avail, BROW, tab->crow, EROW);
   } else if (strcmp(a_question, "cols")           == 0) {
      snprintf(unit_answer, LEN_TEXT, "Cols             : n=%3d, a=%3d, b=%3d, c=%3d, e=%3d", tab->ncol, my.x_avail, BCOL, tab->ccol, ECOL);
   }
   /*---(selection)----------------------*/
   /*> else if   (strcmp(a_question, "sel_range")      == 0) {                                                                                                                            <* 
    *>    snprintf(unit_answer, LEN_TEXT, "Select Range (%c) : ta=%3d, bc=%3d, br=%3d, ec=%3d, er=%3d", (sel.live == 0) ? '-' : 'L', sel.otab, sel.bcol, sel.brow, sel.ecol, sel.erow);   <* 
    *> } else if (strcmp(a_question, "sel_curr")       == 0) {                                                                                                                            <* 
    *>    snprintf(unit_answer, LEN_TEXT, "Select Current   : st=%3d, sc=%3d, sr=%3d", sel.otab, sel.ccol, sel.crow);                                                                     <* 
    *> } else if (strcmp(a_question, "sel_full")       == 0) {                                                                                                                            <* 
    *>    snprintf(unit_answer, LEN_TEXT, "Select Full      : st=%3d, sc=%3d, sr=%3d, sp=%9p", sel.otab, sel.ccol, sel.crow, tabs[sel.otab].sheet[sel.ccol][sel.crow]);                   <*/
   else if   (strcmp(a_question, "curr_pos")       == 0) {
      snprintf(unit_answer, LEN_TEXT, "current position : tab=%3d, col=%3d, row=%3d", my.ctab, tab->ccol, tab->crow);
   } else if (strcmp(a_question, "max_pos" )       == 0) {
      snprintf(unit_answer, LEN_TEXT, "maximum position : tab=%3d, col=%3d, row=%3d", my.ntab, tab->ncol, tab->nrow);
   }
   /*---(dependencies)-------------------*/
   /*> else if (strcmp(a_question, "deps_list")        == 0) {                                                                                                                    <* 
    *>    snprintf(unit_answer, LEN_TEXT, "Deps Linked List : n=%4d, h=%9p, t=%9p", ndep, dhead, dtail);                                                                          <* 
    *> } else if (strcmp(a_question, "deps_count")     == 0) {                                                                                                                    <* 
    *>    /+> x_curr = hcell; while (x_curr != NULL) { printf("%2d) c=%4d, r=%4d, p=%9p\n", x_fore, x_curr->col, x_curr->row, x_curr); ++x_fore; x_curr = x_curr->next; }   <+/   <* 
    *>    x_deps = dhead; while (x_deps != NULL) { ++x_fore; x_deps = x_deps->dnext; }                                                                                            <* 
    *>    x_deps = dtail; while (x_deps != NULL) { ++x_back; x_deps = x_deps->dprev; }                                                                                            <* 
    *>    snprintf(unit_answer, LEN_TEXT, "Deps Links Count : n=%4d, f=%4d, b=%4d", ndep, x_fore, x_back);                                                                        <* 
    *> } else if (strcmp(a_question, "cell_reqs")      == 0) {                                                                                                                    <* 
    *>    DEP_requires (a_thing, temp);                                                                                                                                               <* 
    *>    snprintf(unit_answer, LEN_TEXT, "Cell Reqs List   : %-.35s", temp);                                                                                                     <* 
    *> } else if (strcmp(a_question, "cell_deps")      == 0) {                                                                                                                    <* 
    *>    DEP_provides (a_thing, temp);                                                                                                                                               <* 
    *>    snprintf(unit_answer, LEN_TEXT, "Cell Deps List   : %-.35s", temp);                                                                                                     <* 
    *> }                                                                                                                                                                          <*/
   /*---(UNKNOWN)------------------------*/
   else {
      snprintf(unit_answer, LEN_TEXT, "UNKNOWN          : question is not understood");
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}

char       /*----: set up programgents/debugging -----------------------------*/
PROG_testquiet     (void)
{
   char       *x_args [1]  = { "gyges" };
   yURG_logger (1, x_args);
   PROG_init   (1, x_args);
   yURG_urgs   (1, x_args);
   PROG_args   (1, x_args);
   PROG_begin  ();
   return 0;
}

char       /*----: set up programgents/debugging -----------------------------*/
PROG_testloud      (void)
{
   char       *x_args [2]  = { "gyges_unit", "@@kitchen"    };
   yURG_logger (2, x_args);
   PROG_init   (2, x_args);
   yURG_urgs   (2, x_args);
   PROG_args   (2, x_args);
   PROG_begin  ();
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
PROG_testend       (void)
{
   PROG_end       ();
   return 0;
}




