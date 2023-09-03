/*============================----beg-of-source---============================*/

#include   "gyges.h"

char          unit_answer [LEN_FULL];



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
   strlcpy (t, "[tcc built  ]", 15);
#elif  __GNUC__  > 0
   strlcpy (t, "[gnu gcc    ]", 15);
#elif  __CBANG__  > 0
   strlcpy (t, "[cbang      ]", 15);
#else
   strlcpy (t, "[unknown    ]", 15);
#endif
   snprintf (verstring, LEN_HUND, "%s   %s : %s", t, P_VERNUM, P_VERTXT);
   return verstring;
}



/*====================------------------------------------====================*/
/*===----                       pre-initialization                     ----===*/
/*====================------------------------------------====================*/
static void      o___PREINIT_________________o (void) {;}

char
PROG_urgents            (int a_argc, char *a_argv [])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(set mute)-----------------------*/
   yURG_all_mute ();
   /*---(start logger)-------------------*/
   rc = yURG_logger  (a_argc, a_argv);
   DEBUG_PROG   yLOG_value    ("logger"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(process urgents)----------------*/
   rc = yURG_urgs    (a_argc, a_argv);
   DEBUG_PROG   yLOG_value    ("logger"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                        program startup                       ----===*/
/*====================------------------------------------====================*/
static void      o___STARTUP_________________o (void) {;}

char         /*-> very first setup -------------------[ shoot  [gz.633.201.0A]*/ /*-[00.0000.121.!]-*/ /*-[--.---.---.--]-*/
PROG__init         (int a_argc, char *a_argv[])
{
   char        rce         =  -10;
   char        rc          =    0;
   /*---(log header)---------------------*/
   DEBUG_PROG   yLOG_info     ("purpose" , P_PURPOSE);
   DEBUG_PROG   yLOG_info     ("namesake", P_NAMESAKE);
   DEBUG_PROG   yLOG_info     ("heritage", P_HERITAGE);
   DEBUG_PROG   yLOG_info     ("imagery" , P_IMAGERY);
   DEBUG_PROG   yLOG_info     ("gyges"   , PROG_version      ());
   DEBUG_PROG   yLOG_info     ("yURG"    , yURG_version      ());
   DEBUG_PROG   yLOG_info     ("ySTR"    , ySTR_version      ());
   DEBUG_PROG   yLOG_info     ("yLOG"    , yLOGS_version     ());
   DEBUG_PROG   yLOG_info     ("yRPN"    , yRPN_version      ());
   DEBUG_PROG   yLOG_info     ("yVICURS" , yVICURSES_version ());
   DEBUG_PROG   yLOG_info     ("yMODE"   , yMODE_version     ());
   DEBUG_PROG   yLOG_info     ("yKEYS"   , yKEYS_version     ());
   DEBUG_PROG   yLOG_info     ("yMACRO"  , yMACRO_version    ());
   DEBUG_PROG   yLOG_info     ("yVIEW"   , yVIEW_version     ());
   DEBUG_PROG   yLOG_info     ("yMAP"    , yMAP_version      ());
   DEBUG_PROG   yLOG_info     ("ySRC"    , ySRC_version      ());
   DEBUG_PROG   yLOG_info     ("yCALC"   , yCALC_version     ());
   DEBUG_PROG   yLOG_info     ("yPARSE"  , yPARSE_version    ());
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter    (__FUNCTION__);
   /*---(yvicurses config)---------------*/
   my.btree = 'y';
   api_ysort_init ();
   rc = yVICURSES_init   ("gyges spreadsheet", P_VERNUM, MODE_MAP);
   DEBUG_PROG   yLOG_value    ("yVICURSES" , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*> rc = yMODE_init (MODE_MAP);                                                    <*/
   /*> yVIKEYS_dump_add ("btree"      , api_ysort_btree_dump);                        <*/
   /*> yVIKEYS_dump_add ("seq"        , yCALC_seq_dump);                              <*/
   /*> yVIKEYS_dump_add ("big"        , PROG_bigdump);                                <*/
   /*---(gyges config)-------------------*/
   rc = NODE_init ();
   DEBUG_PROG   yLOG_value    ("NODE"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   rc = TAB_init  ();
   DEBUG_PROG   yLOG_value    ("TAB"       , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   rc = LOC_init  ();
   DEBUG_PROG   yLOG_value    ("LOC"       , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   rc = FILE_init     ();
   DEBUG_PROG   yLOG_value    ("FILE"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(library config)-----------------*/
   rc = yFILE_whoami       (P_FULLPATH, P_VERNUM, P_VERTXT, P_ONELINE, P_SUFFIX, P_CONTENT, api_yvikeys_handlers, FILE_prepper, FILE_finisher);
   DEBUG_PROG   yLOG_value    ("yFILE"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   rc = yMACRO_config (api_ymacro_get, api_ymacro_set);
   DEBUG_PROG   yLOG_value    ("yMACRO"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   rc = yMACRO_agrios_config (api_ymacro_getter, api_ymacro_forcer, api_ymacro_pusher);
   DEBUG_PROG   yLOG_value    ("agrios"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   my.ball = '-';
   rc = yMARK_config  (api_yvikeys_searcher , api_yvikeys_unsearcher, NULL);
   DEBUG_PROG   yLOG_value    ("yMARK"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   rc = ySRC_config   (api_yvikeys_saver, api_ysrc_saver);
   DEBUG_PROG   yLOG_value    ("ySRC"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   yMAP_config       (YMAP_OFFICE, api_ymap_locator, api_ymap_addressor, api_ymap_sizer, api_ymap_entry, api_ymap_placer, api_ymap_done);
   yMAP_mundo_config (5, api_ymap_mundo);
   yMAP_univ_config  (TAB_switch);
   yMAP_formatter    (api_ymap_formatter);
   if (rc == 0)  rc = yMAP_mreg_config  (api_ymap_clearer  , api_ymap_copier, api_ymap_paster, api_ymap_finisher, api_ymap_regkiller, api_yvikeys_exim);
   DEBUG_PROG   yLOG_value    ("yvikeys"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(globals)------------------------*/
   rc = CELL_init  ();
   DEBUG_PROG   yLOG_value    ("CELL"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   rc = EXIM_init ();
   DEBUG_PROG   yLOG_value    ("EXIM"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   rc = api_vikeys_init ();
   DEBUG_PROG   yLOG_value    ("API"       , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   yFILE_dump_add          ("cells"   , "", "current cell inventory", CELL_dump);
   yFILE_dump_add          ("universe", "", "detailed inventory of universes", TAB_dump);
   /*---(ycalc config)-------------------*/
   rc = yCALC_init ('g');
   DEBUG_PROG   yLOG_value    ("yCALC"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   rc = yCALC_exist_config (api_ycalc_enabler, api_ycalc_pointer, api_ycalc_reaper);
   DEBUG_PROG   yLOG_value    ("... exist" , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   rc = yCALC_label_config (api_ycalc_named  , api_ycalc_whos_at, api_ycalc_labeler);
   DEBUG_PROG   yLOG_value    ("... label" , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   rc = yCALC_value_config (api_ycalc_valuer , api_ycalc_address, api_ycalc_special, api_ycalc_printer);
   DEBUG_PROG   yLOG_value    ("... value" , rc);
   if (rc <  0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rc);
      return rc;
   }
   yFILE_dump_add          ("vars", "", "current ycalc vars inventory"  , yCALC_vars_dump);
   yFILE_dump_add          ("deps" ,"", "current dependencies inventory", yCALC_deps_dump);
   /*---(ystr config)--------------------*/
   rc = str0gyges (LOC_checker);
   DEBUG_PROG   yLOG_value    ("ySTR"      , rc);
   if (rc <  0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rc);
      return rc;
   }
   /*---(yrpn config)--------------------*/ rc = yRPN_addr_config   (str2gyges, str4gyges, str6gyges, str8gyges, yMAP_inside);
   DEBUG_PROG   yLOG_value    ("yRPN"      , rc);
   if (rc <  0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rc);
      return rc;
   }
   /*---(globals)------------------------*/
   my.info_win       = G_INFO_NONE;
   my.menu           = ' ';
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit     (__FUNCTION__);
   return 0;
}

char         /*-> process the command line args ------[ ------ [gz.952.251.B4]*/ /*-[01.0000.121.!]-*/ /*-[--.---.---.--]-*/
PROG__args              (int a_argc, char *a_argv[])
{
   /*---(locals)-------------------------*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   char       *a           = NULL;
   char       *b           = NULL;
   int         x_total     =    0;
   int         x_args      =    0;
   char        x_name      [LEN_FULL]   = "";
   char        t           [LEN_FULL]   = "";
   /*---(header)-------------------------*/
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*> FILE_rename ("");                                                              <*/
   /*> yVIKEYS_args (a_argc, a_argv);                                                     <*/
   /*---(process)------------------------*/
   for (i = 1; i < a_argc; ++i) {
      /*---(set up args)-----------------*/
      DEBUG_ARGS  yLOG_value   ("check----" , i);
      a = a_argv [i];
      if (i + 1 < a_argc)  b = a_argv [i + 1];
      else                 b = NULL;
      DEBUG_ARGS  yLOG_info    ("a"         , a);
      DEBUG_ARGS  yLOG_info    ("b"         , b);
      ++x_total;
      /*---(check vikeys)----------------*/
      rc = yKEYS_arg_handle (&i, a, b);
      DEBUG_ARGS  yLOG_value   ("ykeys"     , rc);
      if (rc == 1) {
         DEBUG_ARGS  yLOG_note    ("handled by yKEYS_arg_handle");
         DEBUG_ARGS  yLOG_value   ("bumped i"  , i);
         continue;
      }
      /*---(filter)----------------------*/
      if (a[0] == '@') {
         DEBUG_ARGS  yLOG_note    ("skipped urgent");
         continue;
      }
      /*---(local)-----------------------*/
      DEBUG_ARGS  yLOG_note    ("check for local argument handling");
      ++x_args;
      if      (strncmp (a, "-f"        ,10) == 0)  strlcpy (x_name , a_argv[++i], LEN_RECD);
      else if (strncmp (a, "-h"        ,10) == 0)  PROG_usage();
      else if (strncmp (a, "--help"    ,10) == 0)  PROG_usage();
      /*---(prefixes)--------------------*/
      /*> else if (strncmp (a, "--formula-"          , 10) == 0)  PROG_layout_set ("cli", "formula"  , a + 10);   <* 
       *> else if (strncmp (a, "--status-"           ,  9) == 0)  PROG_layout_set ("cli", "status"   , a +  9);   <* 
       *> else if (strncmp (a, "--command-"          , 10) == 0)  PROG_layout_set ("cli", "command"  , a + 10);   <* 
       *> else if (strncmp (a, "--layout-"           ,  9) == 0)  PROG_layout_set ("cli", "layout"   , a +  9);   <* 
       *> else if (strncmp (a, "--function-list"     ,  9) == 0)  CALC_func_list  ();                             <*/
      /*---(other)-----------------------*/
      else if (a[0] != '-'                     )   strlcpy (x_name , a_argv[i]  , LEN_RECD);
   }
   DEBUG_ARGS  yLOG_value  ("entries"   , x_total);
   DEBUG_ARGS  yLOG_value  ("arguments" , x_args);
   if (x_args == 0) {
      DEBUG_ARGS  yLOG_note   ("no arguments identified");
   }
   /*---(update title)-------------------*/
   --rce;  if (strlen (x_name) > 0) {
      sprintf (t, ":file %s", x_name);
      rc = yCMD_direct (t);
      DEBUG_ARGS  yLOG_value  (":file"     , rc);
      if (rc < 0) {
         DEBUG_PROG  yLOG_exitr (__FUNCTION__, rce);
         return rce;
      }
      rc = yCMD_direct (":read");
      DEBUG_ARGS  yLOG_value  (":read"     , rc);
      if (rc < 0) {
         DEBUG_PROG  yLOG_exitr (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   return 0;
}

char         /*-> initialize program and variables ---[ ------ [gz.741.041.07]*/ /*-[00.0000.121.!]-*/ /*-[--.---.---.--]-*/
PROG__begin             (void)
{
   DEBUG_PROG  yLOG_enter (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   int         i, j, k;
   char        tmp         [100];
   /*---(clear)--------------------------*/
   /*> MARK_init ();                                                                  <*/
   /*---(locals)-------------------------*/
   /*> VISU_init ();                                                                  <*/
   /*> REG_init  ();                                                                  <*/
   /*> KEYS_init ();                                                                  <*/
   /*---(char settings)------------------*/
   my.npos =    1;
   my.cpos =    0;
   my.bpos =    0;
   my.epos =    my.npos - 1;;
   /*---(repeat settings)----------------*/
   /*> yVIKEYS_repeat_init ();                                                        <*/
   /*---(prepare)------------------------*/
   /*> DEP_init  ();                                                                  <*/
   /*---(overall)------------------------*/
   /*> yVIEW_config   ("gyges spreadsheet", P_VERNUM, YVIEW_CURSES, 0, 0, 0);         <*/
   DEBUG_PROG   yLOG_note     ("initial tab creation");
   DEBUG_PROG   yLOG_note     ("zero");
   TAB_new_in_abbr ('0', NULL, NULL);
   DEBUG_PROG   yLOG_note     ("summary");
   TAB_new_in_abbr ('®', NULL, NULL);
   DEBUG_PROG   yLOG_note     ("system");
   TAB_new_in_abbr ('¯', NULL, NULL);
   DEBUG_PROG   yLOG_note     ("switch");
   TAB_switch      (0);
   DEBUG_PROG   yLOG_note     ("refresh");
   yMAP_refresh_full ();
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   return 0;
}

char
PROG_startup            (int a_argc, char *a_argv [])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   yURG_stage_check (YURG_BEG);
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(initialize)---------------------*/
   rc = PROG__init   (a_argc, a_argv);
   DEBUG_PROG   yLOG_value    ("init"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(arguments)----------------------*/
   rc = PROG__args   (a_argc, a_argv);
   DEBUG_PROG   yLOG_value    ("args"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(begin)--------------------------*/
   rc = PROG__begin  ();
   DEBUG_PROG   yLOG_value    ("begin"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   yURG_stage_check (YURG_MID);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                        program execution                     ----===*/
/*====================------------------------------------====================*/
static void      o___EXECUTION_______________o (void) {;}

char         /*-> initialize program and variables ---[ ------ [gz.421.001.08]*/ /*-[00.0000.101.!]-*/ /*-[--.---.---.--]-*/
PROG_dawn          (void)
{
   DEBUG_PROG  yLOG_enter (__FUNCTION__);
   DRAW_init  ();
   /*> CURS_screen_reset ();                                                          <*/
   yCALC_calculate   ();
   /*---(status options)-----------------*/
   yVIEW_switch_add (YVIEW_STATUS, "version", "", CURS_version        , "current location information"               );
   yVIEW_switch_add (YVIEW_STATUS, "gyges"  , "", CURS_current_status , "current location information"               );
   yVIEW_switch_add (YVIEW_STATUS, "tab"    , "", CURS_status_tab     , "tab name, type, and dimensions"             );
   yVIEW_switch_add (YVIEW_STATUS, "cell"   , "", CURS_status_cell    , "details of current cell"                    );
   yVIEW_switch_add (YVIEW_STATUS, "deps"   , "", CURS_status_deps    , "details of current cell dependencies"       );
   yVIEW_switch_add (YVIEW_STATUS, "rpn"    , "", CURS_status_rpn     , "details of current cell rpn notation"       );
   yVIEW_switch_add (YVIEW_STATUS, "vars"   , "", yCALC_vars_status   , "variable inventory statistics"              );
   /*> yVIEW_switch_add (YVIEW_STATUS, "mundo"  , CURS_status_history , "change history for debugging"               );   <*/
   yVIEW_switch_add (YVIEW_STATUS, "error"  , "", CURS_status_error   , "details on recent errors"                   );
   yVIEW_switch_add (YVIEW_STATUS, "detail" , "", CURS_status_detail  , "details on recent errors"                   );
   yVIEW_switch_add (YVIEW_BUFFER, "summary", "", CURS_bufsum         , "one-line buffer inventory"                  );
   yVIEW_switch_add (YVIEW_BUFFER, "detail" , "", CURS_bufdet         , "multi-line buffer list"                     );
   yCMD_direct (":status mode");
   yCMD_direct (":layout gyges");
   yCMD_direct (":formula label");
   yCMD_direct (":alt      disable");
   yCMD_direct (":nav      disable");
   yCMD_direct (":title    disable");
   yCMD_direct (":version  disable");
   yCMD_direct (":detail   disable");
   yCMD_direct (":ribbon   disable");
   yCMD_direct (":progress disable");
   /*> yCMD_direct (":read");                                                         <*/
   /*> MAP_mapper (YMAP_INIT);                                                        <*/
   /*> yCMD_add      (YVIKEYS_M_AUDIT , "hist"        , ""    , ""     , HIST_list                  , "" );   <*/
   yCMD_add      (YVIHUB_M_AUDIT , "ball"        , ""    , ""     , api_ymacro_ball            , "" );
   yCMD_add      (YVIHUB_M_AUDIT , "noball"      , ""    , ""     , api_ymacro_noball          , "" );
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   return 0;
}

char
PROG_dusk          (void)
{
   /*---(header)-------------------------*/
   DEBUG_PROG  yLOG_enter (__FUNCTION__);
   /*---(process)------------------------*/
   DRAW_wrap     ();
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   return 0;
}

char
PROG_cleanse         (void)
{
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   yCALC_cleanse ();
   CELL_wrap      ();
   LOC_wrap       ();
   LOC_init      ();
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
}



/*====================------------------------------------====================*/
/*===----                        program shutdown                      ----===*/
/*====================------------------------------------====================*/
static void      o___SHUTDOWN________________o (void) {;}

char         /*-> shutdown program and free memory ---[ ------ [gz.422.001.03]*/ /*-[00.0000.111.!]-*/ /*-[--.---.---.--]-*/
PROG__end            (void)
{
   /*> printf ("ending program now.\n");                                              <*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*> my.btree = '-';                                                                <*/
   yCALC_wrap     ();
   CELL_wrap      ();
   /*> LOC_wrap       ();                                                             <*/
   yVICURSES_wrap ();
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char             /* [------] drive the program closure activities ------------*/
PROG_shutdown           (void)
{
   /*---(header)-------------------------*/
   yURG_stage_check (YURG_END);
   DEBUG_PROG   yLOG_enter    (__FUNCTION__);
   PROG__end ();
   DEBUG_PROG   yLOG_exit     (__FUNCTION__);
   DEBUG_PROG   yLOGS_end    ();
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gc.630.122.30]*/ /*-[02.0000.00#.!]-*/ /*-[--.---.---.--]-*/
PROG_bigdump            (void *a_file)
{
   CELL_dump      (a_file);
   api_ysort_btree_dump (a_file);
   yCALC_seq_dump (a_file);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNITTEST________o () { return; }

/*> char*        /+-> tbd --------------------------------[ light  [us.JC0.271.X1]+/ /+-[01.0000.00#.!]-+/ /+-[--.---.---.--]-+/                                                                                                <* 
 *> PROG__unit           (char *a_question, void *a_thing)                                                                                                                                                                      <* 
 *> {                                                                                                                                                                                                                           <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                                                                                                                 <* 
 *>    tCELL      *x_curr      = (tCELL*) a_thing;                                                                                                                                                                              <* 
 *>    tDEP       *x_deps      = (tDEP*)  a_thing;                                                                                                                                                                              <* 
 *>    int         x_thing     = (int) a_thing;                                                                                                                                                                                 <* 
 *>    int         len         = 0;                                                                                                                                                                                             <* 
 *>    char        temp        [LEN_RECD];                                                                                                                                                                                      <* 
 *>    int         x_fore      = 0;                                                                                                                                                                                             <* 
 *>    int         x_back      = 0;                                                                                                                                                                                             <* 
 *>    /+---(sheet focus)--------------------+/                                                                                                                                                                                 <* 
 *>    if        (strcmp(a_question, "sheet_size")     == 0) {                                                                                                                                                                  <* 
 *>       snprintf(unit_answer, LEN_FULL, "sheet_size       : ncell=%4d, ncol=%3d, nrow=%3d", ncell, NCOL, NROW);                                                                                                               <* 
 *>    } else if (strcmp(a_question, "cell_list")      == 0) {                                                                                                                                                                  <* 
 *>       snprintf(unit_answer, LEN_FULL, "Cell Linked List : n=%4d, h=%9p, t=%9p", ncell, hcell, tcell);                                                                                                                       <* 
 *>    } else if (strcmp(a_question, "cell_count")     == 0) {                                                                                                                                                                  <* 
 *>       /+> x_curr = hcell; while (x_curr != NULL) { printf("%2d) c=%4d, r=%4d, p=%9p\n", x_fore, x_curr->col, x_curr->row, x_curr); ++x_fore; x_curr = x_curr->m_next; }   <+/                                               <* 
 *>       x_curr = hcell; while (x_curr != NULL) { ++x_fore; x_curr = x_curr->m_next; }                                                                                                                                         <* 
 *>       x_curr = tcell; while (x_curr != NULL) { ++x_back; x_curr = x_curr->m_prev; }                                                                                                                                         <* 
 *>       snprintf(unit_answer, LEN_FULL, "Cell Links Count : n=%4d, f=%4d, b=%4d", ncell, x_fore, x_back);                                                                                                                     <* 
 *>    } else if (strcmp(a_question, "mode")           == 0) {                                                                                                                                                                  <* 
 *>       /+> snprintf(unit_answer, LEN_FULL, "Mode             : %c", yVIKEYS_mode_curr ());   <+/                                                                                                                             <* 
 *>    }                                                                                                                                                                                                                        <* 
 *>    /+---(cell focus)---------------------+/                                                                                                                                                                                 <* 
 *>    else if   (strcmp(a_question, "cell_where")     == 0) {                                                                                                                                                                  <* 
 *>       if (x_curr == NULL)  snprintf(unit_answer, LEN_FULL, "s_cell loc       : p=%10p, t=%4d, c=%4d, r=%4d", NULL  , -1         , -1         , -1         );                                                                <* 
 *>       else                 snprintf(unit_answer, LEN_FULL, "s_cell loc       : p=%10p, t=%4d, c=%4d, r=%4d", x_curr, x_curr->tab, x_curr->col, x_curr->row);                                                                <* 
 *>    } else if (strcmp(a_question, "sheet_who")      == 0) {                                                                                                                                                                  <* 
 *>       snprintf(unit_answer, LEN_FULL, "Sheet Location   : p=%9p", x_curr);                                                                                                                                                  <* 
 *>    } else if (strcmp(a_question, "cell_info")      == 0) {                                                                                                                                                                  <* 
 *>       /+> snprintf(unit_answer, LEN_FULL, "Cell Information : t=%c f=%c d=%c a=%c c=%3d r=%3d d=%3d", x_curr->type, x_curr->format, x_curr->decs, x_curr->align, x_curr->nrpn, x_curr->nrequire, x_curr->nprovide);   <+/   <* 
 *>    }                                                                                                                                                                                                                        <* 
 *>    /+---(cell contents)------------------+/                                                                                                                                                                                 <* 
 *>    else if   (strcmp(a_question, "cell_source")    == 0) {                                                                                                                                                                  <* 
 *>       snprintf(unit_answer, LEN_FULL, "Cell Source      : (%5d) :%-.40s:", x_curr->len, x_curr->source);                                                                                                                    <* 
 *>    } else if (strcmp(a_question, "cell_value")     == 0) {                                                                                                                                                                  <* 
 *>       snprintf(unit_answer, LEN_FULL, "Cell Value       : %18.6F", x_curr->v_num);                                                                                                                                          <* 
 *>    } else if (strcmp(a_question, "cell_modified")  == 0) {                                                                                                                                                                  <* 
 *>       snprintf(unit_answer, LEN_FULL, "Cell Modified    : (%4d) %-.40s", (int) strlen(x_curr->v_str), x_curr->v_str);                                                                                                       <* 
 *>    } else if (strcmp(a_question, "cell_printable") == 0) {                                                                                                                                                                  <* 
 *>       /+> snprintf(unit_answer, LEN_FULL, "Cell Printable   : (%4d) :%-.40s:", (int) strlen(x_curr->print), x_curr->print);   <+/                                                                                           <* 
 *>       snprintf(unit_answer, LEN_FULL, "Cell Printable   : (%4d) :%s:", (int) strlen(x_curr->print), x_curr->print);                                                                                                         <* 
 *>    } else if (strcmp(a_question, "cell_contents")  == 0) {                                                                                                                                                                  <* 
 *>       snprintf(unit_answer, LEN_FULL, "Contents     (%c) : (%2d:%2d) :%-.40s:", (g_contents[my.cpos] >= ' ' && g_contents[my.cpos] <= '~') ? g_contents[my.cpos] : ' ', my.cpos, (int) strlen(g_contents), g_contents);     <* 
 *>    }                                                                                                                                                                                                                        <* 
 *>    /+---(cell contents)------------------+/                                                                                                                                                                                 <* 
 *>    else if   (strcmp(a_question, "cell_rpn")       == 0) {                                                                                                                                                                  <* 
 *>       /+> snprintf(unit_answer, LEN_FULL, "Cell RPN         : (%3d) %-.40s", x_curr->nrpn, x_curr->rpn);   <+/                                                                                                              <* 
 *>    }                                                                                                                                                                                                                        <* 
 *>    /+---(display focus)------------------+/                                                                                                                                                                                 <* 
 *>    else if   (strcmp(a_question, "rows")           == 0) {                                                                                                                                                                  <* 
 *>       snprintf(unit_answer, LEN_FULL, "Rows             : n=%3d, a=%3d, b=%3d, c=%3d, e=%3d", NROW, my.y_avail, BROW, CROW, EROW);                                                                                          <* 
 *>    } else if (strcmp(a_question, "cols")           == 0) {                                                                                                                                                                  <* 
 *>       snprintf(unit_answer, LEN_FULL, "Cols             : n=%3d, a=%3d, b=%3d, c=%3d, e=%3d", NCOL, my.x_avail, BCOL, CCOL, ECOL);                                                                                          <* 
 *>    }                                                                                                                                                                                                                        <* 
 *>    /+---(selection)----------------------+/                                                                                                                                                                                 <* 
 *>    /+> else if   (strcmp(a_question, "sel_range")      == 0) {                                                                                                                            <*                                <* 
 *>     *>    snprintf(unit_answer, LEN_FULL, "Select Range (%c) : ta=%3d, bc=%3d, br=%3d, ec=%3d, er=%3d", (sel.live == 0) ? '-' : 'L', sel.otab, sel.bcol, sel.brow, sel.ecol, sel.erow);   <*                                <* 
 *>     *> } else if (strcmp(a_question, "sel_curr")       == 0) {                                                                                                                            <*                                <* 
 *>     *>    snprintf(unit_answer, LEN_FULL, "Select Current   : st=%3d, sc=%3d, sr=%3d", sel.otab, sel.ccol, sel.crow);                                                                     <*                                <* 
 *>     *> } else if (strcmp(a_question, "sel_full")       == 0) {                                                                                                                            <*                                <* 
 *>     *>    snprintf(unit_answer, LEN_FULL, "Select Full      : st=%3d, sc=%3d, sr=%3d, sp=%9p", sel.otab, sel.ccol, sel.crow, tabs[sel.otab].sheet[sel.ccol][sel.crow]);                   <+/                               <* 
 *>    else if   (strcmp(a_question, "curr_pos")       == 0) {                                                                                                                                                                  <* 
 *>       snprintf(unit_answer, LEN_FULL, "current position : tab=%3d, col=%3d, row=%3d", CTAB, CCOL, CROW);                                                                                                                    <* 
 *>    } else if (strcmp(a_question, "max_pos" )       == 0) {                                                                                                                                                                  <* 
 *>       snprintf(unit_answer, LEN_FULL, "maximum position : tab=%3d, col=%3d, row=%3d", MAX_TABS, NCOL, NROW);                                                                                                                <* 
 *>    }                                                                                                                                                                                                                        <* 
 *>    /+---(dependencies)-------------------+/                                                                                                                                                                                 <* 
 *>    /+> else if (strcmp(a_question, "deps_list")        == 0) {                                                                                                                    <*                                        <* 
 *>     *>    snprintf(unit_answer, LEN_FULL, "Deps Linked List : n=%4d, h=%9p, t=%9p", s_ndep, s_hdep, s_tdep);                                                                          <*                                    <* 
*>     *> } else if (strcmp(a_question, "deps_count")     == 0) {                                                                                                                    <*                                        <* 
   *>     *>    /+> x_curr = hcell; while (x_curr != NULL) { printf("%2d) c=%4d, r=%4d, p=%9p\n", x_fore, x_curr->col, x_curr->row, x_curr); ++x_fore; x_curr = x_curr->m_next; }   <+/   <*                                      <* 
      *>     *>    x_deps = s_hdep; while (x_deps != NULL) { ++x_fore; x_deps = x_deps->dnext; }                                                                                            <*                                       <* 
      *>     *>    x_deps = s_tdep; while (x_deps != NULL) { ++x_back; x_deps = x_deps->dprev; }                                                                                            <*                                       <* 
      *>     *>    snprintf(unit_answer, LEN_FULL, "Deps Links Count : n=%4d, f=%4d, b=%4d", s_ndep, x_fore, x_back);                                                                        <*                                      <* 
      *>     *> } else if (strcmp(a_question, "cell_reqs")      == 0) {                                                                                                                    <*                                        <* 
         *>     *>    DEP_disp_reqs  (a_thing, temp);                                                                                                                                               <*                                  <* 
            *>     *>    snprintf(unit_answer, LEN_FULL, "Cell Reqs List   : %-.35s", temp);                                                                                                     <*                                        <* 
            *>     *> } else if (strcmp(a_question, "cell_deps")      == 0) {                                                                                                                    <*                                        <* 
               *>     *>    DEP_disp_pros  (a_thing, temp);                                                                                                                                               <*                                  <* 
                  *>     *>    snprintf(unit_answer, LEN_FULL, "Cell Deps List   : %-.35s", temp);                                                                                                     <*                                        <* 
                  *>     *> }                                                                                                                                                                          <+/                                       <* 
                  *>    /+---(UNKNOWN)------------------------+/                                                                                                                                                                                 <* 
                  *>    else {                                                                                                                                                                                                                   <* 
                     *>       snprintf(unit_answer, LEN_FULL, "UNKNOWN          : question is not understood");                                                                                                                                     <* 
                        *>    }                                                                                                                                                                                                                        <* 
                        *>    /+---(complete)-----------------------+/                                                                                                                                                                                 <* 
                        *>    return unit_answer;                                                                                                                                                                                                      <* 
                        *> }                                                                                                                                                                                                                           <*/

char         /*-> set up programgents/debugging ------[ light  [uz.320.011.05]*/ /*-[00.0000.00#.#]-*/ /*-[--.---.---.--]-*/
PROG__unit_quiet     (void)
{
   char        rce         =  -10;
   char        rc          =    0;
   int         x_argc      =    1;
   char       *x_args [1]  = { "gyges" };
   /*---(urgents)------------------------*/
   rc = PROG_urgents  (x_argc, x_args);
   DEBUG_PROG   yLOG_value    ("urgents"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(startup)------------------------*/
   rc = PROG_startup  (x_argc, x_args);
   DEBUG_PROG   yLOG_value    ("startup"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> set up programgents/debugging ------[ light  [uz.320.011.05]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
PROG__unit_loud      (void)
{
   char        rce         =  -10;
   char        rc          =    0;
   int         x_argc      =  3;
   char       *x_args [10] = { "gyges_unit", "@@kitchen", "@@ycalc" };
   /*---(urgents)------------------------*/
   rc = PROG_urgents  (x_argc, x_args);
   DEBUG_PROG   yLOG_value    ("urgents"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*> yURG_name   ("tops"  , YURG_ON);                                               <*/
   /*> yURG_name   ("prog"  , YURG_ON);                                               <*/
   /*> yURG_name   ("cell"  , YURG_ON);                                               <*/
   /*> yURG_name   ("regs"  , YURG_ON);                                               <*/
   /*> yURG_name   ("map"   , YURG_ON);                                               <*/
   /*> yURG_name   ("deps"  , YURG_ON);                                               <*/
   /*> yURG_name   ("ystr"  , YURG_ON);                                               <*/
   /*> yURG_name   ("args"  , YURG_ON);                                               <*/
   /*> yURG_name   ("apis"  , YURG_ON);                                               <*/
   /*> yURG_name   ("ycalc" , YURG_ON);                                               <*/
   /*> yURG_name   ("exec"  , YURG_ON);                                               <*/
   /*> yURG_name   ("adjs"  , YURG_ON);                                               <*/
   /*> yURG_name   ("srcp"  , YURG_ON);                                               <*/
   /*> yURG_name   ("yparse", YURG_ON);                                               <*/
   /*> yURG_name   ("ymode" , YURG_ON);                                               <*/
   /*> yURG_name   ("ykeys" , YURG_ON);                                               <*/
   /*> yURG_name   ("ymacro", YURG_ON);                                               <*/
   /*> yURG_name   ("ycmd"  , YURG_ON);                                               <*/
   /*> yURG_name   ("ysrc"  , YURG_ON);                                               <*/
   /*> yURG_name   ("srch"  , YURG_ON);                                               <*/
   /*> yURG_name   ("ymap"  , YURG_ON);                                               <*/
   /*> yURG_name   ("yview" , YURG_ON);                                               <*/
   /*> yURG_name   ("hist"  , YURG_ON);                                               <*/
   /*> yURG_name   ("sort"  , YURG_OFF);                                              <*/
   /*> yURG_name   ("ysort" , YURG_OFF);                                              <*/
   /*---(startup)------------------------*/
   rc = PROG_startup  (x_argc, x_args);
   DEBUG_PROG   yLOG_value    ("startup"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> set up program urgents/debugging ---[ light  [uz.210.001.01]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
PROG__unit_end       (void)
{
   char        rce         =  -10;
   char        rc          =    0;
   rc = PROG_shutdown  ();
   DEBUG_PROG   yLOG_value    ("shutdown"  , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
PROG__unit_cleanse   (void)
{
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   PROG_cleanse ();
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}




