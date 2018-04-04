/*============================----beg-of-source---============================*/

#include   "gyges.h"

char          unit_answer [LEN_UNIT];

tLAYOUT     g_layouts [MAX_LAYOUT] = {
   /*"1234567890", "123456789012" , 12345678901234567890, 12345678901234567890, 12345678901234567890, "1234567890123456789012345678901234567890"      */
   { "formula"   , "tiny"         , G_FORMULA_TINY      , 0                   , 0                   , "shows only contents/formula"                },
   { "formula"   , "small"        , G_FORMULA_SMALL     , 0                   , 0                   , "shows tiny plus location and version"       },
   { "formula"   , "debug"        , G_FORMULA_DEBUG     , 0                   , 0                   , "shows small plus current cell info"         },
   { "status"    , "hide"         , 0                   , G_STATUS_HIDE       , 0                   , "status line is not displayed"               },
   { "status"    , "file"         , 0                   , G_STATUS_FILE       , 0                   , "file name, control, and version"            },
   { "status"    , "tab"          , 0                   , G_STATUS_TAB        , 0                   , "tab name, type, and dimensions"             },
   { "status"    , "buffer"       , 0                   , G_STATUS_BUFFER     , 0                   , "details of current buffer"                  },
   { "status"    , "visual"       , 0                   , G_STATUS_VISUAL     , 0                   , "details of visual selection"                },
   { "status"    , "regs"         , 0                   , G_STATUS_REGS       , 0                   , "details of map register contents"           },
   { "status"    , "treg"         , 0                   , G_STATUS_TREG       , 0                   , "details of text register contents"          },
   { "status"    , "mark"         , 0                   , G_STATUS_MARK       , 0                   , "details of cell and location marks"         },
   { "status"    , "cell"         , 0                   , G_STATUS_CELL       , 0                   , "details of current cell"                    },
   { "status"    , "deps"         , 0                   , G_STATUS_DEPS       , 0                   , "details of current cell dependencies"       },
   { "status"    , "rpn"          , 0                   , G_STATUS_RPN        , 0                   , "details of current cell rpn notation"       },
   { "status"    , "keylog"       , 0                   , G_STATUS_KEYLOG     , 0                   , "key logging details for debugging"          },
   { "status"    , "history"      , 0                   , G_STATUS_HISTORY    , 0                   , "change history for debugging"               },
   { "command"   , "hide"         , 0                   , 0                   , G_COMMAND_HIDE      , "command line is not shown"                  },
   { "command"   , "show"         , 0                   , 0                   , G_COMMAND_SHOW      , "display the command line"                   },
   { "command"   , "float"        , 0                   , 0                   , G_COMMAND_FLOAT     , "display command line when required"         },
   { "layout"    , "min"          , G_FORMULA_TINY      , G_STATUS_HIDE       , G_COMMAND_HIDE      , "greatest working room available"            },
   { "layout"    , "normal"       , G_FORMULA_SMALL     , G_STATUS_FILE       , G_COMMAND_SHOW      , "normal working environment"                 },
   { "layout"    , "max"          , G_FORMULA_DEBUG     , G_STATUS_FILE       , G_COMMAND_SHOW      , "greatest supporting information"            },
   { ""          , ""             , 0                   , 0                   , 0                   , ""                                           },
};
int         g_nlayout;

/*====================------------------------------------====================*/
/*===----                        program wide                          ----===*/
/*====================------------------------------------====================*/

char         /*-> display command line help/usage ----[ leaf   [gc.760.000.00]*/ /*-[00.0000.012.!]-*/ /*-[--.---.---.--]-*/
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

char*        /*-> return library versio --------------[ leaf   [gs.420.012.00]*/ /*-[00.0000.012.!]-*/ /*-[--.---.---.--]-*/
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

char         /*-> very first setup -------------------[ shoot  [gz.633.201.0A]*/ /*-[00.0000.121.!]-*/ /*-[--.---.---.--]-*/
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
   yVIKEYS_init         ();
   /*---(setup file)---------------------*/
   yVIKEYS_file_config  ("gyges", "gyges", VER_NUM, VER_TXT, "/usr/local/bin/gyges", "gyges-hekatonkheires (hundred-handed) spreadsheet");
   yVIKEYS_file_add     (FILE_DEPCEL , OUTP_cell_dep  , INPT_cell);
   yVIKEYS_file_add     (FILE_FREECEL, OUTP_cell_free , INPT_cell);
   yVIKEYS_file_add     (FILE_TABS   , TABS_writer_all, TABS_reader);
   yVIKEYS_file_add     (FILE_COLS   , COLS_writer_all, COLS_reader);
   yVIKEYS_file_add     (FILE_ROWS   , ROWS_writer_all, ROWS_reader);
   /*---(setup other)--------------------*/
   yVIKEYS_macro_config (CELL_macro_get, CELL_macro_set);
   yVIKEYS_srch_config  (SRCH_searcher , SRCH_clearer);
   yVIKEYS_src_config   (CELL_saver);
   yVIKEYS_regs_config  (CELL_killer, REG_copier, REG_clearer, REG_paster);
   /*---(globals)------------------------*/
   hist_active       = '-';
   nhist             =  0;
   chist             = -1;
   CALC_init           ();
   my.info_win       = G_INFO_NONE;
   my.menu           = ' ';
   /*---(complete)-----------------------*/
   DEBUG_TOPS   yLOG_exit     (__FUNCTION__);
   return 0;
}

char         /*-> process the command line args ------[ ------ [gz.952.251.B4]*/ /*-[01.0000.121.!]-*/ /*-[--.---.---.--]-*/
PROG_args          (int argc, char *argv[])
{
   DEBUG_TOPS  yLOG_enter   (__FUNCTION__);
   /*---(locals)-------------------------*/
   int         i           = 0;
   char       *a           = NULL;
   int         x_total     = 0;
   int         x_args      = 0;
   char        x_name      [LEN_STR]   = "";
   /*---(begin)--------------------------*/
   /*> FILE_rename ("");                                                              <*/
   /*---(process)------------------------*/
   for (i = 1; i < argc; ++i) {
      a = argv[i];
      ++x_total;
      if (a[0] == '@')  continue;
      DEBUG_ARGS  yLOG_info    ("cli arg", a);
      ++x_args;
      if      (strncmp (a, "-f"        ,10) == 0)  strlcpy (x_name , argv[++i], LEN_RECD);
      else if (strncmp (a, "-h"        ,10) == 0)  PROG_usage();
      else if (strncmp (a, "--help"    ,10) == 0)  PROG_usage();
      /*---(prefixes)--------------------*/
      /*> else if (strncmp (a, "--formula-"          , 10) == 0)  PROG_layout_set ("cli", "formula"  , a + 10);   <* 
       *> else if (strncmp (a, "--status-"           ,  9) == 0)  PROG_layout_set ("cli", "status"   , a +  9);   <* 
       *> else if (strncmp (a, "--command-"          , 10) == 0)  PROG_layout_set ("cli", "command"  , a + 10);   <* 
       *> else if (strncmp (a, "--layout-"           ,  9) == 0)  PROG_layout_set ("cli", "layout"   , a +  9);   <* 
       *> else if (strncmp (a, "--function-list"     ,  9) == 0)  CALC_func_list  ();                             <*/
      /*---(other)-----------------------*/
      else if (a[0] != '-'                     )   strlcpy (x_name , argv[i]  , LEN_RECD);
   }
   DEBUG_ARGS  yLOG_value  ("entries"   , x_total);
   DEBUG_ARGS  yLOG_value  ("arguments" , x_args);
   if (x_args == 0) {
      DEBUG_ARGS  yLOG_note   ("no arguments identified");
   }
   /*---(update title)-------------------*/
   /*> FILE_rename (x_name);                                                          <*/
   /*---(complete)-----------------------*/
   DEBUG_TOPS  yLOG_exit  (__FUNCTION__);
   return 0;
}

char         /*-> initialize program and variables ---[ ------ [gz.741.041.07]*/ /*-[00.0000.121.!]-*/ /*-[--.---.---.--]-*/
PROG_begin         (void)
{
   DEBUG_PROG  yLOG_enter (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   int         i, j, k;
   char        tmp         [100];
   /*---(clear)--------------------------*/
   CELL_init ();
   /*> MARK_init ();                                                                  <*/
   /*---(overall tab settings)-----------*/
   LOC_init  ();
   /*---(locals)-------------------------*/
   VISU_init ();
   REG_init  ();
   /*> KEYS_init ();                                                                  <*/
   /*---(char settings)------------------*/
   my.npos =    1;
   my.cpos =    0;
   my.bpos =    0;
   my.epos =    my.npos - 1;;
   /*---(repeat settings)----------------*/
   /*> yVIKEYS_repeat_init ();                                                        <*/
   /*---(prepare)------------------------*/
   DEP_init  ();
   /*---(overall)------------------------*/
   yVIKEYS_view_config   ("gyges spreadsheet", VER_NUM, YVIKEYS_CURSES, 0, 0, 0);
   yVIKEYS_map_config    (YVIKEYS_OFFICE, MAP_mapper, LOC_locator, LOC_addressor);
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   return 0;
}

char         /*-> initialize program and variables ---[ ------ [gz.421.001.08]*/ /*-[00.0000.101.!]-*/ /*-[--.---.---.--]-*/
PROG_final         (void)
{
   DEBUG_PROG  yLOG_enter (__FUNCTION__);
   DRAW_init  ();
   /*> INPT_main         ();                                                          <*/
   /*> CURS_screen_reset ();                                                          <*/
   SEQ_calc_full     ();
   hist_active = 'y';
   /*---(process)------------------------*/
   /*> MOVE_vert ('r');                                                               <*/
   /*> MOVE_horz ('r');                                                               <*/
   /*> yVIKEYS_mode_mesg (my.message, yVIKEYS_cmds_curr ());                          <*/
   /*---(status options)-----------------*/
   yVIKEYS_view_option (YVIKEYS_STATUS, "tab"    , CURS_status_tab     , "tab name, type, and dimensions"             );
   yVIKEYS_view_option (YVIKEYS_STATUS, "buffer" , CURS_status_buffer  , "details of current buffer"                  );
   yVIKEYS_view_option (YVIKEYS_STATUS, "visual" , CURS_status_visual  , "details of visual selection"                );
   yVIKEYS_view_option (YVIKEYS_STATUS, "reg"    , CURS_status_reg     , "details of map register contents"           );
   /*> yVIKEYS_view_option (YVIKEYS_STATUS, "treg"   , CURS_status_textreg , "details of text register contents"          );   <*/
   yVIKEYS_view_option (YVIKEYS_STATUS, "mark"   , CURS_status_mark    , "details of cell and location marks"         );
   yVIKEYS_view_option (YVIKEYS_STATUS, "cell"   , CURS_status_cell    , "details of current cell"                    );
   yVIKEYS_view_option (YVIKEYS_STATUS, "deps"   , CURS_status_deps    , "details of current cell dependencies"       );
   yVIKEYS_view_option (YVIKEYS_STATUS, "rpn"    , CURS_status_rpn     , "details of current cell rpn notation"       );
   yVIKEYS_view_option (YVIKEYS_STATUS, "history", CURS_status_history , "change history for debugging"               );
   yVIKEYS_view_option (YVIKEYS_STATUS, "error"  , CURS_status_error   , "details on recent errors"                   );
   yVIKEYS_cmds_direct (":status mode");
   yVIKEYS_map_refresh ();
   yVIKEYS_mode_formatter    (SMOD_format);
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   return 0;
}

/*> char         /+-> gather main loop keyboard input ----[ ------ [gc.D44.233.C7]+/ /+-[02.0000.111.R]-+/ /+-[--.---.---.--]-+/   <* 
 *> PROG_main_input    (char a_mode, char a_key)                                                                                   <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                    <* 
 *>    char        x_ch        = ' ';                                                                                              <* 
 *>    char        x_play      = ' ';                                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_LOOP   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    DEBUG_LOOP   yLOG_char    ("a_mode"    , a_mode);                                                                           <* 
 *>    DEBUG_LOOP   yLOG_value   ("a_key"     , a_key);                                                                            <* 
 *>    DEBUG_LOOP   yLOG_char    ("m mode"    , my.macro_mode);                                                                    <* 
 *>    DEBUG_LOOP   yLOG_char    ("m delay"   , my.macro_delay);                                                                   <* 
 *>    DEBUG_LOOP   yLOG_value   ("m pos"     , my.macro_pos);                                                                     <* 
 *>    /+---(normal)-------------------------+/                                                                                    <* 
 *>    IF_MACRO_OFF {                                                                                                              <* 
 *>       DEBUG_LOOP   yLOG_note    ("normal/macro off");                                                                          <* 
 *>       if (yVIKEYS_repeat_macro ()) {                                                                                           <* 
 *>          DEBUG_USER   yLOG_note    ("but, in macro repeat mode");                                                              <* 
 *>          yVIKEYS_macro_exec_beg ('@');                                                                                         <* 
 *>          a_key = ' ';                                                                                                          <* 
 *>       } else {                                                                                                                 <* 
 *>          if (a_mode == RUN_USER)  x_ch   = CURS_main  ();                                                                      <* 
 *>          else                     x_ch   = a_key;                                                                              <* 
 *>       }                                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    /+---(recording)----------------------+/                                                                                    <* 
 *>    else IF_MACRO_RECORDING {                                                                                                   <* 
 *>       DEBUG_LOOP   yLOG_note    ("macro recording");                                                                           <* 
 *>       if (a_mode == RUN_USER) {                                                                                                <* 
 *>          IF_MACRO_NOT_RUN      x_ch   = CURS_main  ();                                                                         <* 
 *>       }                                                                                                                        <* 
 *>       else                     x_ch   = a_key;                                                                                 <* 
 *>    }                                                                                                                           <* 
 *>    /+---(run, delay, or playback)--------+/                                                                                    <* 
 *>    else IF_MACRO_PLAYING {                                                                                                     <* 
 *>       DEBUG_LOOP   yLOG_note    ("macro running, delay, or playback");                                                         <* 
 *>       x_ch = yVIKEYS_macro_exec_key ();                                                                                        <* 
 *>       IF_MACRO_OFF {                                                                                                           <* 
 *>          DEBUG_LOOP   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return -1;                                                                                                            <* 
 *>       }                                                                                                                        <* 
 *>       DEBUG_LOOP   yLOG_note    ("show screen");                                                                               <* 
 *>       if (a_mode == RUN_USER)  CURS_main  ();                                                                                  <* 
 *>       yVIKEYS_macro_exec_wait ();                                                                                              <* 
 *>       DEBUG_LOOP   yLOG_note    ("read playback keystroke");                                                                   <* 
 *>       if (a_mode == RUN_USER)  x_play = CURS_playback ();                                                                      <* 
 *>       else                     x_play = a_key;                                                                                 <* 
 *>       DEBUG_LOOP   yLOG_value   ("x_play"    , x_play);                                                                        <* 
 *>       if (yVIKEYS_macro_exec_play (x_play) < 0) {                                                                              <* 
 *>          DEBUG_LOOP   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return -2;                                                                                                            <* 
 *>       }                                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    /+---(record)-------------------------+/                                                                                    <* 
 *>    DEBUG_LOOP   yLOG_note    ("handle keystroke normally");                                                                    <* 
 *>    KEYS_record (x_ch);                                                                                                         <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_LOOP   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return x_ch;                                                                                                                <* 
 *> }                                                                                                                              <*/

/*> char         /+-> process main loop keyboard input ---[ leaf   [gc.GD1.132.IM]+/ /+-[05.0000.111.R]-+/ /+-[--.---.---.--]-+/   <* 
 *> PROG_main_handle   (char a_key)                                                                                                <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                    <* 
 *>    static char x_major     = ' ';      /+ saved keystroke                     +/                                               <* 
 *>    static char x_savemode  = '-';                                                                                              <* 
 *>    char        rc          = 0;                                                                                                <* 
 *>    int         x_repeat    = 0;                                                                                                <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_LOOP   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    DEBUG_LOOP   yLOG_value   ("a_key"     , a_key);                                                                            <* 
 *>    /+---(defense)------------------------+/                                                                                    <* 
 *>    if (a_key == K_DONE) {                                                                                                      <* 
 *>       DEBUG_LOOP   yLOG_exit    (__FUNCTION__);                                                                                <* 
 *>       return a_key;                                                                                                            <* 
 *>    }                                                                                                                           <* 
 *>    /+---(handle count)-------------------+/                                                                                    <* 
 *>    if (yVIKEYS_mode_curr () == SMOD_REPEAT) {                                                                                  <* 
 *>       rc = yVIKEYS_repeat_umode (x_major, a_key);                                                                              <* 
 *>       if (rc >  0)  x_major = ' ';                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    /+---(handle keystroke)---------------+/                                                                                    <* 
 *>    while (1) {                                                                                                                 <* 
 *>       switch (yVIKEYS_mode_curr ()) {                                                                                          <* 
 *>       case MODE_GOD      : rc = MODE_god            (x_major , a_key);  break;                                                 <* 
 *>       case MODE_MAP      : rc = MODE_map            (x_major , a_key);  break;                                                 <* 
 *>       case MODE_SOURCE   : rc = MODE_source         (x_major , a_key);  break;                                                 <* 
 *>       case MODE_INPUT    : rc = MODE_input          (x_major , a_key);  break;                                                 <* 
 *>       case MODE_COMMAND  : rc = yVIKEYS_cmds_mode   (x_major , a_key);  break;                                                 <* 
 *>       case MODE_SEARCH   : rc = yVIKEYS_srch_mode   (x_major , a_key);  break;                                                 <* 
 *>       case MODE_VISUAL   : rc = VISU_submode        (x_major , a_key);  break;                                                 <* 
 *>       case SMOD_ERROR    : rc = SMOD_error          (x_major , a_key);  break;                                                 <* 
 *>       case SMOD_SELECT   : rc = SELC_mode           (x_major , a_key);  break;                                                 <* 
 *>       case SMOD_TEXTREG  : rc = TREG_mode           (x_major , a_key);  break;                                                 <* 
 *>       case SMOD_REPLACE  : rc = SMOD_replace        (x_major , a_key);  break;                                                 <* 
 *>       case SMOD_FORMAT   : rc = SMOD_format         (x_major , a_key);  break;                                                 <* 
 *>       case SMOD_BUFFER   : rc = SMOD_buffer         (x_major , a_key);  break;                                                 <* 
 *>       case SMOD_WANDER   : rc = SMOD_wander         (G_KEY_SPACE , a_key);  break;                                             <* 
 *>       case SMOD_REGISTER : rc = REG_mode            (x_major , a_key);  break;                                                 <* 
 *>       case SMOD_MARK     : rc = MARK_submode        (x_major , a_key);  break;                                                 <* 
 *>       case SMOD_MENUS    : rc = SMOD_menus          (x_major , a_key);  break;                                                 <* 
 *>       case SMOD_MACRO    : rc = yVIKEYS_macro_smode (x_major , a_key);  break;                                                 <* 
 *>       }                                                                                                                        <* 
 *>       /+---(translate unprintable)----------+/                                                                                 <* 
 *>       x_repeat = yVIKEYS_repeat_value ();                                                                                      <* 
 *>       if      (a_key == 0       )      snprintf (g_cmd,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_NULL  );                  <* 
 *>       else if (a_key == G_KEY_RETURN)  snprintf (g_cmd,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_RETURN);                  <* 
 *>       else if (a_key == G_KEY_ESCAPE)  snprintf (g_cmd,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_ESCAPE);                  <* 
 *>       else if (a_key == G_KEY_TAB   )  snprintf (g_cmd,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_TAB   );                  <* 
 *>       else if (a_key == G_KEY_BS    )  snprintf (g_cmd,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_BS    );                  <* 
 *>       else if (a_key == G_KEY_SPACE )  snprintf (g_cmd,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_SPACE );                  <* 
 *>       else if (a_key <= G_KEY_SPACE )  snprintf (g_cmd,   9, "%2d %c%02x", x_repeat, x_major, a_key);                          <* 
 *>       else                             snprintf (g_cmd,   9, "%2d %c%c"  , x_repeat, x_major, a_key);                          <* 
 *>       /+---(multiplier)---------------------+/                                                                                 <* 
 *>       if (rc == 0 && x_repeat > 0 && yVIKEYS_mode_curr () != SMOD_REPEAT) {                                                    <* 
 *>          yVIKEYS_repeat_dec ();                                                                                                <* 
 *>          continue;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>       break;                                                                                                                   <* 
 *>    }                                                                                                                           <* 
 *>    /+---(setup for next keystroke)-------+/                                                                                    <* 
 *>    if      (rc == 0)    x_major = ' ';                                                                                         <* 
 *>    else if (rc >  0)    x_major = rc;                                                                                          <* 
 *>    else               { x_major = ' ';  sta_error = 'y';  yVIKEYS_repeat_init (); }                                            <* 
 *>    /+---(setup status line)--------------+/                                                                                    <* 
 *>    if        (yVIKEYS_mode_curr() == MODE_COMMAND) {                                                                           <* 
 *>       yVIKEYS_mode_mesg (my.message, yVIKEYS_cmds_curr ());                                                                    <* 
 *>    } else if (yVIKEYS_mode_curr() == MODE_SEARCH ) {                                                                           <* 
 *>       yVIKEYS_mode_mesg (my.message, yVIKEYS_srch_curr ());                                                                    <* 
 *>    } else if (x_savemode != yVIKEYS_mode_curr()) {                                                                             <* 
 *>       yVIKEYS_mode_mesg (my.message, yVIKEYS_cmds_curr ());                                                                    <* 
 *>    }                                                                                                                           <* 
 *>    x_savemode = yVIKEYS_mode_curr ();                                                                                          <* 
 *>    /+---(advance macros)-----------------+/                                                                                    <* 
 *>    IF_MACRO_ON  yVIKEYS_macro_exec_adv ();                                                                                     <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_LOOP   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> process input string in main loop --[ ------ [ge.C74.153.42]+/ /+-[02.0000.00#.D]-+/ /+-[--.---.---.--]-+/   <* 
 *> PROG_main_string     (char *a_keys)                                                                                            <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         =  -10;     /+ return code for errors              +/                                               <* 
 *>    char        rc          =    0;                                                                                             <* 
 *>    int         i           =    0;                                                                                             <* 
 *>    int         x_len       =    0;                                                                                             <* 
 *>    char        x_ch        =  ' ';     /+ current keystroke                   +/                                               <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_LOOP   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    DEBUG_LOOP   yLOG_point   ("a_keys"    , a_keys);                                                                           <* 
 *>    --rce;  if (a_keys == NULL) {                                                                                               <* 
 *>       DEBUG_LOOP   yLOG_note    ("a_keys is null");                                                                            <* 
 *>       DEBUG_LOOP   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    DEBUG_LOOP   yLOG_info    ("a_keys"    , a_keys);                                                                           <* 
 *>    x_len = strlen (a_keys);                                                                                                    <* 
 *>    DEBUG_LOOP   yLOG_value   ("x_len"     , x_len);                                                                            <* 
 *>    --rce;                                                                                                                      <* 
 *>    for (i = 0; i < x_len; ++i) {                                                                                               <* 
 *>       DEBUG_LOOP   yLOG_value   ("LOOP"      , i);                                                                             <* 
 *>       /+---(get next char)---------------+/                                                                                    <* 
 *>       x_ch = a_keys [i];                                                                                                       <* 
 *>       DEBUG_LOOP   yLOG_value   ("x_ch"      , x_ch);                                                                          <* 
 *>       if (x_ch <  0) {                                                                                                         <* 
 *>          DEBUG_SCRP   yLOG_note    ("special character");                                                                      <* 
 *>          DEBUG_SCRP   yLOG_value   ("256 + x_ch", 256 + x_ch);                                                                 <* 
 *>          /+---(translate special)--------+/                                                                                    <* 
 *>          switch (256 + x_ch) {                                                                                                 <* 
 *>          case G_CHAR_RETURN  :  x_ch = G_KEY_RETURN;  break;                                                                   <* 
 *>          case G_CHAR_ESCAPE  :  x_ch = G_KEY_ESCAPE;  break;                                                                   <* 
 *>          case G_CHAR_BS      :  x_ch = G_KEY_BS;      break;                                                                   <* 
 *>          case G_CHAR_TAB     :  x_ch = G_KEY_TAB;     break;                                                                   <* 
 *>          case G_CHAR_SPACE   :  x_ch = G_KEY_SPACE;   break;                                                                   <* 
 *>          case G_CHAR_GROUP   :  x_ch = G_KEY_GROUP;   break;                                                                   <* 
 *>          case G_CHAR_FIELD   :  x_ch = G_KEY_FIELD;   break;                                                                   <* 
 *>          }                                                                                                                     <* 
 *>          DEBUG_SCRP   yLOG_value   ("x_ch (new)", x_ch);                                                                       <* 
 *>       }                                                                                                                        <* 
 *>       /+---(handle input)----------------+/                                                                                    <* 
 *>       x_ch = PROG_main_input (RUN_TEST, x_ch);                                                                                 <* 
 *>       DEBUG_LOOP   yLOG_value   ("x_ch"      , x_ch);                                                                          <* 
 *>       if (x_ch == -1) continue;                                                                                                <* 
 *>       /+---(handle keystroke)------------+/                                                                                    <* 
 *>       rc = PROG_main_handle (x_ch);                                                                                            <* 
 *>       DEBUG_LOOP   yLOG_value   ("rc"        , rc);                                                                            <* 
 *>       /+---(done)------------------------+/                                                                                    <* 
 *>    }                                                                                                                           <* 
 *>    DEBUG_LOOP   yLOG_note    ("main loop done");                                                                               <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_LOOP   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

char         /*-> shutdown program and free memory ---[ ------ [gz.422.001.03]*/ /*-[00.0000.111.!]-*/ /*-[--.---.---.--]-*/
PROG_end             (void)
{
   /*> printf ("ending program now.\n");                                              <*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   DEBUG_PROG   yLOG_note    ("CELL_wrap");
   CELL_wrap    ();
   DEBUG_PROG   yLOG_note    ("DEP_wrap");
   DEP_wrap     ();   /* disconnect all cells */
   DEBUG_PROG   yLOG_note    ("LOC_wrap");
   LOC_wrap     ();
   DEBUG_PROG   yLOG_note    ("yVIKEYS_wrap");
   yVIKEYS_wrap ();
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   DEBUG_TOPS   yLOG_end     ();
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     layout configuration                     ----===*/
/*====================------------------------------------====================*/
PRIV void  o___LAYOUT__________o () { return; }

/*> char         /+-> tbd --------------------------------[ shoot  [gz.430.011.10]+/ /+-[01.0000.012.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> PROG_layout_init    (void)                                                                                                     <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    int         i           = 0;                                                                                                <* 
 *>    /+---(initialize data)----------------+/                                                                                    <* 
 *>    g_nlayout = 0;                                                                                                              <* 
 *>    for (i = 0; i <= MAX_LAYOUT; ++i) {                                                                                         <* 
 *>       /+---(filter)----------------------+/                                                                                    <* 
 *>       if (g_layouts [i].cat [0] == '\0')            break;                                                                     <* 
 *>       ++g_nlayout;                                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    /+---(set defaults)-------------------+/                                                                                    <* 
 *>    my.layout_formula     = G_FORMULA_SMALL;                                                                                    <* 
 *>    my.layout_status      = G_STATUS_FILE;                                                                                      <* 
 *>    my.layout_command     = G_COMMAND_SHOW;                                                                                     <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ ------ [gc.850.323.C5]+/ /+-[02.0000.152.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> PROG_layout_set     (char *a_who, char *a_cat, char *a_opt)                                                                    <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    int         i           = 0;                                                                                                <* 
 *>    int         x_found     = -1;                                                                                               <* 
 *>    /+---(defense)------------------------+/                                                                                    <* 
 *>    if (a_cat == NULL)   return -1;                                                                                             <* 
 *>    if (a_opt == NULL)   return -2;                                                                                             <* 
 *>    /+---(list)---------------------------+/                                                                                    <* 
 *>    if (strcmp (a_opt, "options") == 0)  PROG_layout_list (a_who);                                                              <* 
 *>    /+---(find)---------------------------+/                                                                                    <* 
 *>    for (i = 0; i <= g_nlayout; ++i) {                                                                                          <* 
 *>       /+---(filter)----------------------+/                                                                                    <* 
 *>       if (g_layouts [i].cat [0] == '\0')            break;                                                                     <* 
 *>       if (g_layouts [i].cat [0] != a_cat [0])       continue;                                                                  <* 
 *>       if (g_layouts [i].opt [0] != a_opt [0])       continue;                                                                  <* 
 *>       if (strcmp (g_layouts [i].cat, a_cat) != 0)   continue;                                                                  <* 
 *>       if (strcmp (g_layouts [i].opt, a_opt) != 0)   continue;                                                                  <* 
 *>       /+---(assign)----------------------+/                                                                                    <* 
 *>       x_found = i;                                                                                                             <* 
 *>       if (g_layouts [i].formula > 0)  my.layout_formula = g_layouts [i].formula;                                               <* 
 *>       if (g_layouts [i].status  > 0)  my.layout_status  = g_layouts [i].status;                                                <* 
 *>       if (g_layouts [i].command > 0)  my.layout_command = g_layouts [i].command;                                               <* 
 *>       /+---(reset)-----------------------+/                                                                                    <* 
 *>       CURS_size    ();                                                                                                         <* 
 *>       if (strcmp (a_who, "cmd") == 0) {                                                                                        <* 
 *>          /+> MOVE_prep    ();                                                         <+/                                      <* 
 *>          EROW         = 10000;                                                                                                 <* 
 *>          MOVE_vert    ('r');                                                                                                   <* 
 *>          MOVE_done    ();                                                                                                      <* 
 *>       }                                                                                                                        <* 
 *>       break;                                                                                                                   <* 
 *>       /+---(done)------------------------+/                                                                                    <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return x_found;                                                                                                             <* 
 *> }                                                                                                                              <*/

/*> char PROG_layout_formula (char *a_opt) { return PROG_layout_set ("cmd", "formula", a_opt); }   <*/
/*> char PROG_layout_status  (char *a_opt) { return PROG_layout_set ("cmd", "status" , a_opt); }   <*/
/*> char PROG_layout_command (char *a_opt) { return PROG_layout_set ("cmd", "command", a_opt); }   <*/
/*> char PROG_layout_layout  (char *a_opt) { return PROG_layout_set ("cmd", "layout" , a_opt); }   <*/

/*> char         /+-> tbd --------------------------------[ ------ [gc.750.153.41]+/ /+-[01.0000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> PROG_layout_list    (char *a_who)                                                                                              <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    int         i           = 0;                                                                                                <* 
 *>    char        x_save      [LEN_LABEL] = "";                                                                                   <* 
 *>    char        x_cli       [LEN_DESC];                                                                                         <* 
 *>    char        x_cmd       [LEN_DESC];                                                                                         <* 
 *>    char       *x_title     = "   ---option-----------  ---command----------  ---description------------------------";          <* 
 *>    /+---(defense)------------------------+/                                                                                    <* 
 *>    if (a_who == NULL)  return -1;                                                                                              <* 
 *>    if (strcmp (a_who, "cmd") == 0) {                                                                                           <* 
 *>       CURS_info_request  (G_INFO_LAYOUT);                                                                                      <* 
 *>       return 0;                                                                                                                <* 
 *>    }                                                                                                                           <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    printf ("gyges_hekatonkheires - screen layout options/commands\n");                                                         <* 
 *>    printf ("\n%s\n", x_title);                                                                                                 <* 
 *>    for (i = 0; i <= MAX_LAYOUT; ++i) {                                                                                         <* 
 *>       /+---(filter)----------------------+/                                                                                    <* 
 *>       if (g_layouts [i].cat [0] == '\0')            break;                                                                     <* 
 *>       /+---(show)------------------------+/                                                                                    <* 
 *>       if (strcmp (g_layouts [i].cat, x_save) != 0)  printf ("\n   %s options\n", g_layouts [i].cat);                           <* 
 *>       sprintf (x_cli, "--%s-%s", g_layouts [i].cat, g_layouts [i].opt);                                                        <* 
 *>       sprintf (x_cmd, ":%s %s" , g_layouts [i].cat, g_layouts [i].opt);                                                        <* 
 *>       printf  ("   %-20.20s  %-20.20s  %s\n", x_cli, x_cmd, g_layouts [i].desc);                                               <* 
 *>       strcpy  (x_save, g_layouts [i].cat);                                                                                     <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    printf ("\n%s\n", x_title);                                                                                                 <* 
 *>    exit (-1);                                                                                                                  <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ leaf   [ge.430.224.30]+/ /+-[00.0000.104.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> PROG_layout_entry   (int a_num, char *a_line)                                                                                  <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         = -10;                                                                                              <* 
 *>    char        x_cmd       [LEN_DESC];                                                                                         <* 
 *>    /+---(defense)------------------------+/                                                                                    <* 
 *>    --rce;  if (a_line == NULL)       return rce;                                                                               <* 
 *>    strcpy (a_line, " -------------------- ---------------------------------------- ");                                         <* 
 *>    --rce;  if (a_num <  0        )  return rce;                                                                                <* 
 *>    --rce;  if (a_num >= g_nlayout)  return rce;                                                                                <* 
 *>    /+---(format)-------------------------+/                                                                                    <* 
 *>    sprintf (x_cmd , ":%s %s" , g_layouts [a_num].cat, g_layouts [a_num].opt);                                                  <* 
 *>    sprintf (a_line, " %-20.20s %-40.40s ", x_cmd, g_layouts [a_num].desc);                                                     <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNITTEST________o () { return; }

char*        /*-> tbd --------------------------------[ light  [us.JC0.271.X1]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
PROG__unit           (char *a_question, void *a_thing)
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
      snprintf(unit_answer, LEN_UNIT, "sheet_size       : ncell=%4d, ncol=%3d, nrow=%3d", ncell, NCOL, NROW);
   } else if (strcmp(a_question, "cell_list")      == 0) {
      snprintf(unit_answer, LEN_UNIT, "Cell Linked List : n=%4d, h=%9p, t=%9p", ncell, hcell, tcell);
   } else if (strcmp(a_question, "cell_count")     == 0) {
      /*> x_curr = hcell; while (x_curr != NULL) { printf("%2d) c=%4d, r=%4d, p=%9p\n", x_fore, x_curr->col, x_curr->row, x_curr); ++x_fore; x_curr = x_curr->next; }   <*/
      x_curr = hcell; while (x_curr != NULL) { ++x_fore; x_curr = x_curr->next; }
      x_curr = tcell; while (x_curr != NULL) { ++x_back; x_curr = x_curr->prev; }
      snprintf(unit_answer, LEN_UNIT, "Cell Links Count : n=%4d, f=%4d, b=%4d", ncell, x_fore, x_back);
   } else if (strcmp(a_question, "mode")           == 0) {
      /*> snprintf(unit_answer, LEN_UNIT, "Mode             : %c", yVIKEYS_mode_curr ());   <*/
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
      snprintf(unit_answer, LEN_UNIT, "Rows             : n=%3d, a=%3d, b=%3d, c=%3d, e=%3d", NROW, my.y_avail, BROW, CROW, EROW);
   } else if (strcmp(a_question, "cols")           == 0) {
      snprintf(unit_answer, LEN_UNIT, "Cols             : n=%3d, a=%3d, b=%3d, c=%3d, e=%3d", NCOL, my.x_avail, BCOL, CCOL, ECOL);
   }
   /*---(selection)----------------------*/
   /*> else if   (strcmp(a_question, "sel_range")      == 0) {                                                                                                                            <* 
    *>    snprintf(unit_answer, LEN_UNIT, "Select Range (%c) : ta=%3d, bc=%3d, br=%3d, ec=%3d, er=%3d", (sel.live == 0) ? '-' : 'L', sel.otab, sel.bcol, sel.brow, sel.ecol, sel.erow);   <* 
    *> } else if (strcmp(a_question, "sel_curr")       == 0) {                                                                                                                            <* 
    *>    snprintf(unit_answer, LEN_UNIT, "Select Current   : st=%3d, sc=%3d, sr=%3d", sel.otab, sel.ccol, sel.crow);                                                                     <* 
    *> } else if (strcmp(a_question, "sel_full")       == 0) {                                                                                                                            <* 
    *>    snprintf(unit_answer, LEN_UNIT, "Select Full      : st=%3d, sc=%3d, sr=%3d, sp=%9p", sel.otab, sel.ccol, sel.crow, tabs[sel.otab].sheet[sel.ccol][sel.crow]);                   <*/
   else if   (strcmp(a_question, "curr_pos")       == 0) {
      snprintf(unit_answer, LEN_UNIT, "current position : tab=%3d, col=%3d, row=%3d", my.ctab, CCOL, CROW);
   } else if (strcmp(a_question, "max_pos" )       == 0) {
      snprintf(unit_answer, LEN_UNIT, "maximum position : tab=%3d, col=%3d, row=%3d", MAX_TABS, NCOL, NROW);
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

char         /*-> set up programgents/debugging ------[ light  [uz.320.011.05]*/ /*-[00.0000.00#.#]-*/ /*-[--.---.---.--]-*/
PROG__unitquiet     (void)
{
   char       *x_args [1]  = { "gyges" };
   yURG_logger (1, x_args);
   PROG_init   (1, x_args);
   yURG_urgs   (1, x_args);
   PROG_args   (1, x_args);
   PROG_begin  ();
   PROG_final  ();
   return 0;
}

char         /*-> set up programgents/debugging ------[ light  [uz.320.011.05]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
PROG__unitloud      (void)
{
   char       *x_args [3]  = { "gyges_unit", "@@kitchen", "@@LOCS"    };
   yURG_logger (3, x_args);
   PROG_init   (3, x_args);
   yURG_urgs   (3, x_args);
   PROG_args   (3, x_args);
   PROG_begin  ();
   return 0;
}

char         /*-> set up program urgents/debugging ---[ light  [uz.210.001.01]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
PROG__unitend       (void)
{
   PROG_end       ();
   return 0;
}




