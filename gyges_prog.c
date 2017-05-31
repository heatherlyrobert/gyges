/*============================----beg-of-source---============================*/

#include   "gyges.h"

char          unit_answer [LEN_UNIT];

tLAYOUT     g_layouts [MAX_LAYOUT] = {
   /*"1234567890", "123456789012" , 12345678901234567890, 12345678901234567890, 12345678901234567890, "1234567890123456789012345678901234567890"      */
   { "formula"   , "tiny"         , G_FORMULA_TINY      , 0                   , 0                   , "shows only contents/formula"                },
   { "formula"   , "small"        , G_FORMULA_SMALL     , 0                   , 0                   , "shows tiny plus location and version"       },
   { "formula"   , "debug"        , G_FORMULA_DEBUG     , 0                   , 0                   , "shows small plus current cell info"         },
   { "status"    , "hide"         , 0                   , G_STATUS_HIDE       , 0                   , "status line is not displayed"               },
   { "status"    , "file"         , 0                   , G_STATUS_FILE       , 0                   , "file name, dimensions, and version"         },
   { "status"    , "buffer"       , 0                   , G_STATUS_BUFFER     , 0                   , "details of current buffer"                  },
   { "status"    , "visual"       , 0                   , G_STATUS_VISUAL     , 0                   , "details of visual selection"                },
   { "status"    , "regs"         , 0                   , G_STATUS_REGS       , 0                   , "details of map register contents"           },
   { "status"    , "treg"         , 0                   , G_STATUS_TREG       , 0                   , "details of text register contents"          },
   { "status"    , "mark"         , 0                   , G_STATUS_MARK       , 0                   , "details of cell and location marks"         },
   { "status"    , "cell"         , 0                   , G_STATUS_CELL       , 0                   , "details of current cell"                    },
   { "status"    , "deps"         , 0                   , G_STATUS_DEPS       , 0                   , "details of current cell dependencies"       },
   { "status"    , "keylog"       , 0                   , G_STATUS_KEYLOG     , 0                   , "key logging details for debugging"          },
   { "status"    , "history"      , 0                   , G_STATUS_HISTORY    , 0                   , "change history for debugging"               },
   { "command"   , "hide"         , 0                   , 0                   , G_COMMAND_HIDE      , "command line is not shown"                  },
   { "command"   , "show"         , 0                   , 0                   , G_COMMAND_SHOW      , "display the command line"                   },
   { "command"   , "float"        , 0                   , 0                   , G_COMMAND_FLOAT     , "display command line when required"         },
   { "layout"    , "min"          , G_FORMULA_TINY      , G_STATUS_HIDE       , G_COMMAND_HIDE      , "greatest working room available"            },
   { "layout"    , "max"          , G_FORMULA_DEBUG     , G_STATUS_FILE       , G_COMMAND_SHOW      , "greatest supporting information"            },
   { ""          , ""             , 0                   , 0                   , 0                   , ""                                           },
};

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
   DEBUG_TOPS   yLOG_enter    (__FUNCTION__);
   /*---(initialize)---------------------*/
   hist_active = '-';
   nhist       =  0;
   chist       = -1;
   yVIKEYS_mode_init   ();
   yVIKEYS_mode_enter  (MODE_MAP);
   my.layout_formula     = G_FORMULA_DEBUG;
   my.info_win = G_INFO_NONE;
   my.menu     = ' ';
   /*---(complete)-----------------------*/
   DEBUG_TOPS   yLOG_exit     (__FUNCTION__);
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
   strncpy(my.f_name  , FILE_BLANK , LEN_RECD);
   strncpy(my.f_suffix, FILE_SUFFIX, LEN_RECD);
   /*---(process)------------------------*/
   for (i = 1; i < argc; ++i) {
      a = argv[i];
      ++x_total;
      if (a[0] == '@')  continue;
      DEBUG_ARGS  yLOG_info    ("cli arg", a);
      ++x_args;
      if      (strncmp (a, "-f"        ,10) == 0)  strncpy (my.f_name , argv[++i], LEN_RECD);
      else if (strncmp (a, "-h"        ,10) == 0)  PROG_usage();
      else if (strncmp (a, "--help"    ,10) == 0)  PROG_usage();
      /*---(prefixes)--------------------*/
      else if (strncmp (a, "--formula-"          , 10) == 0)  PROG_layout_set ("cli", "formula"  , a + 10);
      else if (strncmp (a, "--status-"           ,  9) == 0)  PROG_layout_set ("cli", "status"   , a +  9);
      else if (strncmp (a, "--command-"          , 10) == 0)  PROG_layout_set ("cli", "command"  , a + 10);
      else if (strncmp (a, "--layout-"           ,  9) == 0)  PROG_layout_set ("cli", "layout"   , a +  9);
      /*---(other)-----------------------*/
      else if (a[0] != '-'                     )  strncpy (my.f_name , argv[i]  , LEN_RECD);
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



/*====================------------------------------------====================*/
/*===----                     layout configuration                     ----===*/
/*====================------------------------------------====================*/
PRIV void  o___LAYOUT__________o () { return; }

char
PROG_layout_set     (char *a_who, char *a_cat, char *a_opt)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   int         x_found     = -1;
   /*---(defense)------------------------*/
   if (a_cat == NULL)   return -1;
   if (a_opt == NULL)   return -2;
   /*---(list)---------------------------*/
   if (strcmp (a_opt, "options") == 0)  PROG_layout_list ();
   /*---(find)---------------------------*/
   for (i = 0; i <= MAX_LAYOUT; ++i) {
      /*---(filter)----------------------*/
      if (g_layouts [i].cat [0] == '\0')            break;
      if (g_layouts [i].cat [0] != a_cat [0])       continue;
      if (g_layouts [i].opt [0] != a_opt [0])       continue;
      if (strcmp (g_layouts [i].cat, a_cat) != 0)   continue;
      if (strcmp (g_layouts [i].opt, a_opt) != 0)   continue;
      /*---(assign)----------------------*/
      x_found = i;
      if (g_layouts [i].formula > 0)  my.layout_formula = g_layouts [i].formula;
      if (g_layouts [i].status  > 0)  my.layout_status  = g_layouts [i].status;
      if (g_layouts [i].command > 0)  my.layout_command = g_layouts [i].command;
      /*---(reset)-----------------------*/
      CURS_size    ();
      if (strcmp (a_who, "cmd") == 0) {
         MOVE_prep    ();
         EROW         = 10000;
         MOVE_vert    ('r');
         MOVE_done    ();
      }
      break;
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   return x_found;
}

char
PROG_layout_list    (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_save      [LEN_LABEL] = "";
   char        x_cli       [LEN_DESC];
   char        x_cmd       [LEN_DESC];
   char       *x_title     = "   ---option-----------  ---command----------  ---description--------------------------";
   /*---(defense)------------------------*/
   printf ("gyges_hekatonkheires - screen layout options/commands\n");
   printf ("\n%s\n", x_title);
   for (i = 0; i <= MAX_LAYOUT; ++i) {
      /*---(filter)----------------------*/
      if (g_layouts [i].cat [0] == '\0')            break;
      /*---(show)------------------------*/
      if (strcmp (g_layouts [i].cat, x_save) != 0)  printf ("\n   %s options\n", g_layouts [i].cat);
      sprintf (x_cli, "--%s-%s", g_layouts [i].cat, g_layouts [i].opt);
      sprintf (x_cmd, ":%s %s" , g_layouts [i].cat, g_layouts [i].opt);
      printf  ("   %-20.20s  %-20.20s  %s\n", x_cli, x_cmd, g_layouts [i].desc);
      strcpy  (x_save, g_layouts [i].cat);
   }
   /*---(complete)-----------------------*/
   printf ("\n%s\n", x_title);
   exit (-1);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNITTEST________o () { return; }

char*
unit_accessor(char *a_question, void *a_thing)
{
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_curr      = (tCELL*) a_thing;
   tDEP       *x_deps      = (tDEP*)  a_thing;
   int         x_thing     = (int) a_thing;
   int         len         = 0;
   char        temp        [LEN_RECD];
   int         x_fore      = 0;
   int         x_back      = 0;
   /*---(sheet focus)--------------------*/
   if        (strcmp(a_question, "sheet_size")     == 0) {
      snprintf(unit_answer, LEN_UNIT, "sheet_size       : ncell=%4d, ncol=%3d, nrow=%3d", ncell, tab->ncol, tab->nrow);
   } else if (strcmp(a_question, "cell_list")      == 0) {
      snprintf(unit_answer, LEN_UNIT, "Cell Linked List : n=%4d, h=%9p, t=%9p", ncell, hcell, tcell);
   } else if (strcmp(a_question, "cell_count")     == 0) {
      /*> x_curr = hcell; while (x_curr != NULL) { printf("%2d) c=%4d, r=%4d, p=%9p\n", x_fore, x_curr->col, x_curr->row, x_curr); ++x_fore; x_curr = x_curr->next; }   <*/
      x_curr = hcell; while (x_curr != NULL) { ++x_fore; x_curr = x_curr->next; }
      x_curr = tcell; while (x_curr != NULL) { ++x_back; x_curr = x_curr->prev; }
      snprintf(unit_answer, LEN_UNIT, "Cell Links Count : n=%4d, f=%4d, b=%4d", ncell, x_fore, x_back);
   } else if (strcmp(a_question, "mode")           == 0) {
      snprintf(unit_answer, LEN_UNIT, "Mode             : %c", yVIKEYS_mode_curr ());
   }
   /*---(cell focus)---------------------*/
   else if   (strcmp(a_question, "cell_where")     == 0) {
      if (x_curr == NULL)  snprintf(unit_answer, LEN_UNIT, "s_cell loc       : p=%10p, t=%4d, c=%4d, r=%4d", NULL  , -1         , -1         , -1         );
      else                 snprintf(unit_answer, LEN_UNIT, "s_cell loc       : p=%10p, t=%4d, c=%4d, r=%4d", x_curr, x_curr->tab, x_curr->col, x_curr->row);
   } else if (strcmp(a_question, "sheet_who")      == 0) {
      snprintf(unit_answer, LEN_UNIT, "Sheet Location   : p=%9p", x_curr);
   } else if (strcmp(a_question, "cell_info")      == 0) {
      snprintf(unit_answer, LEN_UNIT, "Cell Information : t=%c f=%c d=%c a=%c c=%3d r=%3d d=%3d", x_curr->t, x_curr->f, x_curr->d, x_curr->a, x_curr->nrpn, x_curr->nrequire, x_curr->nprovide);
   }
   /*---(cell contents)------------------*/
   else if   (strcmp(a_question, "cell_source")    == 0) {
      snprintf(unit_answer, LEN_UNIT, "Cell Source      : (%5d) :%-.40s:", x_curr->l, x_curr->s);
   } else if (strcmp(a_question, "cell_value")     == 0) {
      snprintf(unit_answer, LEN_UNIT, "Cell Value       : %18.6F", x_curr->v_num);
   } else if (strcmp(a_question, "cell_modified")  == 0) {
      snprintf(unit_answer, LEN_UNIT, "Cell Modified    : (%4d) %-.40s", (int) strlen(x_curr->v_str), x_curr->v_str);
   } else if (strcmp(a_question, "cell_printable") == 0) {
      /*> snprintf(unit_answer, LEN_UNIT, "Cell Printable   : (%4d) :%-.40s:", (int) strlen(x_curr->p), x_curr->p);   <*/
      snprintf(unit_answer, LEN_UNIT, "Cell Printable   : (%4d) :%s:", (int) strlen(x_curr->p), x_curr->p);
   } else if (strcmp(a_question, "cell_contents")  == 0) {
      snprintf(unit_answer, LEN_UNIT, "Contents     (%c) : (%2d:%2d) :%-.40s:", (g_contents[my.cpos] >= ' ' && g_contents[my.cpos] <= '~') ? g_contents[my.cpos] : ' ', my.cpos, (int) strlen(g_contents), g_contents);
   }
   /*---(cell contents)------------------*/
   else if   (strcmp(a_question, "cell_rpn")       == 0) {
      snprintf(unit_answer, LEN_UNIT, "Cell RPN         : (%3d) %-.40s", x_curr->nrpn, x_curr->rpn);
   }
   /*---(display focus)------------------*/
   else if   (strcmp(a_question, "rows")           == 0) {
      snprintf(unit_answer, LEN_UNIT, "Rows             : n=%3d, a=%3d, b=%3d, c=%3d, e=%3d", tab->nrow, my.y_avail, BROW, tab->crow, EROW);
   } else if (strcmp(a_question, "cols")           == 0) {
      snprintf(unit_answer, LEN_UNIT, "Cols             : n=%3d, a=%3d, b=%3d, c=%3d, e=%3d", tab->ncol, my.x_avail, BCOL, tab->ccol, ECOL);
   }
   /*---(selection)----------------------*/
   /*> else if   (strcmp(a_question, "sel_range")      == 0) {                                                                                                                            <* 
    *>    snprintf(unit_answer, LEN_UNIT, "Select Range (%c) : ta=%3d, bc=%3d, br=%3d, ec=%3d, er=%3d", (sel.live == 0) ? '-' : 'L', sel.otab, sel.bcol, sel.brow, sel.ecol, sel.erow);   <* 
    *> } else if (strcmp(a_question, "sel_curr")       == 0) {                                                                                                                            <* 
    *>    snprintf(unit_answer, LEN_UNIT, "Select Current   : st=%3d, sc=%3d, sr=%3d", sel.otab, sel.ccol, sel.crow);                                                                     <* 
    *> } else if (strcmp(a_question, "sel_full")       == 0) {                                                                                                                            <* 
    *>    snprintf(unit_answer, LEN_UNIT, "Select Full      : st=%3d, sc=%3d, sr=%3d, sp=%9p", sel.otab, sel.ccol, sel.crow, tabs[sel.otab].sheet[sel.ccol][sel.crow]);                   <*/
   else if   (strcmp(a_question, "curr_pos")       == 0) {
      snprintf(unit_answer, LEN_UNIT, "current position : tab=%3d, col=%3d, row=%3d", my.ctab, tab->ccol, tab->crow);
   } else if (strcmp(a_question, "max_pos" )       == 0) {
      snprintf(unit_answer, LEN_UNIT, "maximum position : tab=%3d, col=%3d, row=%3d", my.ntab, tab->ncol, tab->nrow);
   }
   /*---(dependencies)-------------------*/
   /*> else if (strcmp(a_question, "deps_list")        == 0) {                                                                                                                    <* 
    *>    snprintf(unit_answer, LEN_UNIT, "Deps Linked List : n=%4d, h=%9p, t=%9p", s_ndep, s_hdep, s_tdep);                                                                          <* 
    *> } else if (strcmp(a_question, "deps_count")     == 0) {                                                                                                                    <* 
    *>    /+> x_curr = hcell; while (x_curr != NULL) { printf("%2d) c=%4d, r=%4d, p=%9p\n", x_fore, x_curr->col, x_curr->row, x_curr); ++x_fore; x_curr = x_curr->next; }   <+/   <* 
    *>    x_deps = s_hdep; while (x_deps != NULL) { ++x_fore; x_deps = x_deps->dnext; }                                                                                            <* 
    *>    x_deps = s_tdep; while (x_deps != NULL) { ++x_back; x_deps = x_deps->dprev; }                                                                                            <* 
    *>    snprintf(unit_answer, LEN_UNIT, "Deps Links Count : n=%4d, f=%4d, b=%4d", s_ndep, x_fore, x_back);                                                                        <* 
    *> } else if (strcmp(a_question, "cell_reqs")      == 0) {                                                                                                                    <* 
    *>    DEP_disp_reqs  (a_thing, temp);                                                                                                                                               <* 
    *>    snprintf(unit_answer, LEN_UNIT, "Cell Reqs List   : %-.35s", temp);                                                                                                     <* 
    *> } else if (strcmp(a_question, "cell_deps")      == 0) {                                                                                                                    <* 
    *>    DEP_disp_pros  (a_thing, temp);                                                                                                                                               <* 
    *>    snprintf(unit_answer, LEN_UNIT, "Cell Deps List   : %-.35s", temp);                                                                                                     <* 
    *> }                                                                                                                                                                          <*/
   /*---(UNKNOWN)------------------------*/
   else {
      snprintf(unit_answer, LEN_UNIT, "UNKNOWN          : question is not understood");
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




