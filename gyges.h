/*===============================[[ beg code ]]===============================*/
/* htag ::  714,  326 docs 36%,  386 code 72%,  326 othr 21%,  304 slocl      */


/*===[[ BEG_HEADER ]]=========================================================*/

/*===[[ ONE_LINERS ]]=========================================================*/
/*  12          12      -        1         2         3         4         5         6         7         8*/
/*345678901-12345678901-123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-*/

#define     P_FOCUS     "TO (major tools)"
#define     P_NICHE     "ss (spreadsheet)"
#define     P_SUBJECT   "wicked technical spreadsheet"
#define     P_PURPOSE   "technical, fullsome, aesthetic, and keyboard-centric spreadsheet"

#define     P_NAMESAKE  "gyges-hecatoncheires (hundred-handed)"
#define     P_HERITAGE  "gyges, the furious, is one of the three hecatoncheires"
#define     P_IMAGERY   "ugly, impossibly powerful, one-hundred handed, fifty headed giant"
#define     P_REASON    "gyges and spreadsheets are both impossibly scary and complex"

#define     P_ONELINE   P_NAMESAKE " " P_SUBJECT

#define     P_BASENAME  "gyges"
#define     P_FULLPATH  "/usr/local/bin/gyges"
#define     P_SUFFIX    "gyges"
#define     P_CONTENT   "spreadsheet data"

#define     P_SYSTEM    "gnu/linux   (powerful, ubiquitous, technical, and hackable)"
#define     P_LANGUAGE  "ansi-c      (wicked, limitless, universal, and everlasting)"
#define     P_CODESIZE  "large       (appoximately 10,000 slocl)"
#define     P_DEPENDS   "none"

#define     P_AUTHOR    "heatherlyrobert"
#define     P_CREATED   "2010-09"

#define     P_VERMAJOR  "3.--, totally reworking to use yVIKEYS and yCALC"
#define     P_VERMINOR  "3.6-, complete integration with new vi-keys libraries"
#define     P_VERNUM    "3.6g"
#define     P_VERTXT    "all existing cut/paste unit tests working again"

#define     P_PRIORITY  "direct, simple, brief, vigorous, and lucid (h.w. fowler)"
#define     P_PRINCIPAL "[grow a set] and build your wings on the way down (r. bradbury)"
#define     P_REMINDER  "there are many better options, but i *own* every byte of this one"

/*===[[ REASONING ]]==========================================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-*/

/*
 *  spreadsheet
 *  presentation
 *  writing
 *  graphing/charting
 *  engineering/cad/vector
 *  drawing/paint/pixel
 *  note taking
 *  database
 *  project management
 *  todo/task management
 *  workflow
 *  data browser/hypercard
 *
 *  signal processing
 *  computer vision
 *  simulation
 *  virtual reality/immersive
 *
 *
 */




#define     P_DEFINE    \
   "as one of the major user-empowerment tools, spreadsheets allow ad-hoc,¦" \
   "informal, low skill, quick, and evolving analysis and manipulation of any¦" \
   "kind of data -- from massive tables to irregular and unclear collections.¦"
#define     P_CURRENT   \
   "this is a fairly well-defined category of tool with a relatively stable¦" \
   "interface and several major alternative products.  while user experiences¦" \
   "vary, major functions and features are broadly comparable across competitors.¦"
#define     P_CONCERN   \
   "these tools have become overly complex, unwieldy, kitchen-sink tools that¦" \
   "overlap with other major tools rather than complement them.  spreadsheets in¦" \
   "particular have become dangerous, error-prone replacements for real systems.¦"
#define     P_ALTERNS   "visicalc, lotus 123, excel, google, oocalc, gnumeric¦"
#define     P_WHATFOR   \
   "i love spreadsheets.  love them.  but, in pursuit of market share, they have¦" \
   "evolved into massive, complex, mouse-driven, productivity suckers that¦" \
   "integrate poorly with other tools (anti-unix).¦"
#define     P_DECISION  \
   "gonna make my own.  crazy and unrealistic, but rewarding and sobering too.¦" \
   "if you want to learn, pick blisteringly hard goals.  my result will be¦" \
   "keyboard-driven, scriptable, light, fast, and flexible.¦"


/*===[[ END_HEADER ]]=========================================================*/





/*===[[ END_HEADER ]]=========================================================*/

/*===[[ PURPOSE ]]============================================================*

 *   gyges is a light, clean, fast, and aesthetic consol-based spreadsheet
 *   that focuses on functionality, analysis, interfacing, scripting, and
 *   tailored usage rather than the modern trend of formatting, presentation,
 *   layout, and data storage.  we are looking to "do one thing and do it well"
 *   in, hopefully, the best tradition of unix tools.
 *
 *   in general, modern spreadsheet software has become the universal tool
 *   since it is all most people really learn and all business computers share.
 *   as a result, these applications have become positively huge, unwieldy
 *   monsters focused on being the one viable tool.
 *
 *   gyges will specifically handle the following...
 *      - very rapid movement, editing, and manipulation in a vim-like manner
 *      - simple, editable, ascii-text file format (script creatable)
 *      - full set of core, basic functions such as in sc, oleo
 *      - dependency-graph calculation method to avoid issues (and learn)
 *      - compile calculations to byte-code for speed (and even more learning)
 *      - refactoring tools to prevent redundancy, mistakes, and inconsistencies
 *      - scriptable using the same commands as availabile interactively
 *      - accept data extracts into specific locations in the worksheet
 *      - write out a visual version for use as tables in other applications
 *      - ability to create files that oocalc and excel can pick up
 *      - transparent color console interface aesthetically matching our desktop
 *
 *   gyges will NOT handle specialty needs like that are less common...
 *      - international support (who cares)
 *      - proportional fonts or any formatting other than color (no value)
 *      - graphing (that can be done in other specific tools)
 *      - import or export of special formats (like xls, etc.)
 *      - separate note entries (just confusing, put them right in a cell)
 *      - different cell heights, etc
 *      - no manual calculation option to turn off auto calcs (fast enough)
 *
 *   gyges will additionally make some serious simplifying assumptions...
 *      - all numbers will be stored as doubles (flexible, but approximate)
 *      - spreadsheet cells will be allocated as needed, not upfront
 *
 *
 *
 *   a key design decision is to recognize that data is not meant to be stored
 *   long-term in a spreadsheet.  spreadsheets are for calculation and
 *   manipulation and therefore should focus on working with applicable extracts
 *   rather than the entire data set.  we will be developing a database as well.
 *
 *   a second key design decision is not to use a spreadsheet as a reporting and
 *   visualization mechanism.  again, spreadsheets are for calculation and
 *   manipulation.  there are better tools for final display that should really
 *   only receive a small, prepared set of data and go from there.
 *
 *   finally, gyges is not meant to replace awk, it is meant to supplement it by
 *   being a much more interactive mechanism where theories can be tried out
 *   and analysis can be interactive.  once the techniques for analysis and
 *   manipulation are finalized, awk or c programs should be used.
 *
 *   the largest modern day caution must be that spreadsheets are dangerous,
 *   potentially misleading, almost entirely incapable of validation, and quite
 *   subject to data alteration and version control issues.  everyone knows this
 *   but it doesn't stop spreadsheet prototypes from becoming the final
 *   solution.  holy bloody cow.
 *
 *   while it might seem peculiar, gyges will focus on being able to act as an
 *   interface and scriptable filter as well as a calculator replacement.  it
 *   should be so easy that you would use it instead of any of the gui calculator
 *   tools.  it should also be so useful and flexible that we will find new uses
 *   for it as a filter.
 *
 *   AS ALWAYS, there are many stable, accepted, existing programs that have
 *   been built by better programmers and are likely superior in speed, size
 *   capability, and reliability; BUT, i would not have learned nearly as much
*   using them, so i follow the adage
*
*   TO TRULY LEARN> do not seek to follow in the footsteps of the men of old;
*   seek what they sought ~ Matsuo Basho
*
*   THE UNDERLYING IDEA is to use this build process to learn new programming
*   capabilities and put additional pressure on me to improve my standards
*   maintenance, and coding environment.  just using programs and ideas does
*   not lead to understanding -- so get in there and get messy ;
*
*/


/*===[[ HEADER GUARD ]]=======================================================*/
#ifndef YGYGES
#define YGYGES loaded




/*---(ansi-c standard)-------------------*/
#include    <stdio.h>        /* C_ANSI : strcpy, strlen, strchr, strcmp, ...  */
#include    <string.h>       /* C_ANSI : printf, snprintf, fgets, fopen, ...  */
#include    <stdlib.h>       /* C_ANSI : exit                                 */
#include    <math.h>         /* C_ANSI : fabs, cos, sin, round, trunc, ...    */
#include    <ctype.h>        /* C_ANSI : tolower, toupper, ...                */
#include    <time.h>         /* C_ANSI : time, strftime, localtime            */
#include    <malloc.h>       /* C_ANSI : malloc, free                         */
/*---(posix standard)--------------------*/
#include    <unistd.h>            /* POSIX  standard operating system API     */
#include    <sys/time.h>          /* POSIX  standard time access              */
/*---(defacto standard)------------------*/
#include    <ncurses.h>      /* CURSES : mvprintw, refresh, getch, ...        */
/*---(custom core)-----------------------*/
#include    <yURG.h>              /* heatherly urgent processing              */
#include    <yLOG.h>              /* heatherly program logging                */
#include    <ySTR.h>              /* heatherly string processing              */
/*---(custom vi-keys)--------------------*/
#include    <yKEYS.h>             /* heatherly vikeys key handling            */
#include    <yMODE.h>             /* heatherly vikeys mode tracking           */
#include    <yMACRO.h>            /* heatherly vikeys macro processing        */
#include    <ySRC.h>              /* heatherly vikeys source editing          */
#include    <yCMD.h>              /* heatherly vikeys command processing      */
#include    <yVIEW.h>             /* heatherly vikeys view management         */
#include    <yMAP.h>              /* heatherly vikeys location management     */
#include    <yFILE.h>             /* heatherly vikeys content file handling   */
#include    <yMARK.h>             /* heatherly vikeys search and marking      */
#include    <yVICURSES.h>         /* heatherly vikeys curses handler          */
/*---(custom other)----------------------*/
#include    <yVAR.h>         /* CUSTOM : heatherly variable testing           */
#include    <yREGEX.h>       /* CUSTOM : heatherly regular expressions        */
#include    <yCALC.h>        /* CUSTOM : heatherly interactive calculation    */
#include    <yPARSE.h>       /* heatherly file reading and writing      */
#include    <ySORT.h>        /* heatherly sorting library               */
#include    <yRPN.h>         /* CUSTOM : heatherly infix to RPN conversion    */
/*---(custom constants only)-------------*/
#include    <yDLST_solo.h>        /* heatherly double-double-list             */




#define     PR        static
#define     PRIV      static


/*===[[ TYPEDEFS : COMBINATIONS ]]============================================*/
/*---(basics)--------------------------*/
typedef     signed   char         schar;
typedef     unsigned char         uchar;
typedef     const    char         cchar;
typedef     unsigned short        ushort;
typedef     const    int          cint;
typedef     unsigned long long    ullong;
/*---(library simplifications)---------*/
typedef     struct   tm           tTIME;
/*---(data structures)-----------------*/
typedef     struct   cTAB         tTAB;
typedef     struct   cNODE        tNODE;
typedef     struct   cCELL        tCELL;
typedef     struct   cDEP         tDEP;          /* inter-cell dependency     */
typedef     struct   cCALC        tCALC;         /* cell calculation entry    */
typedef     struct   cREG         tREG;          /* cut and paste registers   */
typedef     struct   cHIST        tHIST;         /* undo-redo history         */

typedef struct timespec  tTSPEC;


/*===[[ CONSTANTS : LIMITS ]]=================================================*/
/*---(tabs)---------------------------*/
#define     MAX_TABS    40
#define     DEF_TABS    5
#define     MIN_TABS    1
/*---(columns)------------------------*/
#define     MAX_COLS    702
#define     DEF_COLS    26
#define     MIN_COLS    1
/*---(rows)---------------------------*/
#define     MAX_ROWS    9999
#define     DEF_ROWS    100
#define     MIN_ROWS    1
/*---(cell width)---------------------*/
#define     MAX_WIDTH   50
#define     DEF_WIDTH   8
#define     MIN_WIDTH   2
#define     MAX_MERGE   10
/*---(cell height)--------------------*/
#define     MAX_HEIGHT  4
#define     DEF_HEIGHT  1
#define     MIN_HEIGHT  1
/*---(registers)----------------------*/

#define     DEF_FORMAT  "??0--"

char        buf0        [LEN_RECD];


/*---(macros)-------------------------*/
/*---(run as)----------*/
#define     RUN_USER           'i'      /* running in user mode (ncurses)     */
#define     RUN_TEST           '-'      /* running as a test    (no ncurses)  */
/*---(mode)------------*/
/*> #define     MACRO_OFF          '-'      /+ normal keyboard input              +/   <*/
/*> #define     MACRO_RUN          'M'      /+ macro running with redisplay       +/   <*/
/*> #define     MACRO_DELAY        'D'      /+ macro delay playback controls      +/   <*/
/*> #define     MACRO_PLAYBACK     'P'      /+ macro under playback controls      +/   <*/
/*> #define     MACRO_RECORD       'r'      /+ macro recording                    +/   <*/
/*---(conditions)------*/
/*> #define     IF_MACRO_OFF         if (my.macro_mode == MACRO_OFF      )            <*/
/*> #define     IF_MACRO_RUN         if (my.macro_mode == MACRO_RUN      )            <*/
/*> #define     IF_MACRO_NOT_RUN     if (my.macro_mode != MACRO_RUN      )            <*/
/*> #define     IF_MACRO_DELAY       if (my.macro_mode == MACRO_DELAY    )            <*/
/*> #define     IF_MACRO_PLAYBACK    if (my.macro_mode == MACRO_PLAYBACK )            <*/
/*> #define     IF_MACRO_MOVING      if (my.macro_mode == MACRO_RUN      || my.macro_mode == MACRO_DELAY   )    <*/
/*> #define     IF_MACRO_NOT_PLAYING if (my.macro_mode == MACRO_OFF      || my.macro_mode == MACRO_RECORD  )   <*/
/*> #define     IF_MACRO_PLAYING     if (my.macro_mode != MACRO_OFF      && my.macro_mode != MACRO_RECORD  )   <*/
/*> #define     IF_MACRO_RECORDING   if (my.macro_mode == MACRO_RECORD   )            <*/
/*> #define     IF_MACRO_ON          if (my.macro_mode != MACRO_OFF      )            <*/
/*---(setting)---------*/
/*> #define     SET_MACRO_OFF        my.macro_mode = MACRO_OFF                        <*/
/*> #define     SET_MACRO_RUN        my.macro_mode = MACRO_RUN                        <*/
/*> #define     SET_MACRO_PLAYBACK   my.macro_mode = MACRO_PLAYBACK                   <*/
/*> #define     SET_MACRO_DELAY      my.macro_mode = MACRO_DELAY                      <*/
/*> #define     SET_MACRO_RECORD     my.macro_mode = MACRO_RECORD                     <*/



struct cACCESSOR {
   /*---(files)----------------*/
   char        quiet;          /* bool : 0=normal, 1=quiet                    */
   int         logger;         /* log file so that we don't close it          */
   char        btree;          /* updates active (y/n)                        */
   /*---(cell root)------------*/
   tCELL      *root;
   /*---(keyboard macro)-------*/
   char        macro_mode;                  /* keyboard, macro, silent        */
   char        macro_name;                  /* current macro name             */
   int         macro_pos;                   /* step in current macro          */
   int         macro_len;                   /* length of macro                */
   char        macro_keys  [LEN_RECD];      /* macro contents                 */
   char        macro_char;                  /* interpreted current char       */
   char        macro_delay;                 /* time between steps             */
   /*---(layout)----------*/
   char        layout_formula;              /* formula line display mode      */
   char        layout_status;               /* status line display mode       */
   char        layout_command;              /* command line display mode      */
   char        message     [LEN_RECD];      /* message line                   */
   /*---(file hanndling)--*/
   char        f_loc       [LEN_RECD];      /* specific file location         */
   char        f_name      [LEN_RECD];      /* full file name                 */
   char        f_title     [LEN_RECD];      /* specific file base name        */
   int         f_lines;                     /* file line number               */
   char        f_recd      [LEN_RECD];      /* current file record            */
   char        f_type      [LEN_RECD];      /* current record verb            */
   char        f_vers;                      /* current record version         */
   /*---(reg/clip file)---*/
   char        f_clip      [LEN_RECD];      /* register clip file             */
   /*---(current tab)-----*/
   uchar       start;                       /* tab start in reading file      */
   uchar       ntab;           /* number of worksheet tabs                         */
   uchar       ctab;           /* current tab number                               */
   tTAB       *ptab;           /* current tab pointer                              */
   uchar       tab1;           /* tab of source                                    */
   uchar       tab2;           /* tab of destination                               */
   /*---(current ool)-----*/
   ushort      ncol;
   ushort      ccol;
   ushort      bcol;
   ushort      ecol;
   uchar       froz_col;
   ushort      froz_bcol;
   ushort      froz_ecol;
   /*---(current row)-----*/
   ushort      nrow;
   ushort      crow;
   ushort      brow;
   ushort      erow;
   uchar       froz_row;
   ushort      froz_brow;
   ushort      froz_erow;
   /*---(horizontal size)----------------*/
   int         x_full;         // total screen size in chars
   int         x_left;         // number of chars for row labels to left
   int         x_right;        // number of chars for text to right
   int         x_avail;        // number of chars availible for cells
   /*---(column working vars)------------*/
   int         col1;           /* left side of range                               */
   int         col2;           /* right side of range                              */
   int         col1_save;      /* left side of range                               */
   int         col2_save;      /* right side of range                              */
   /*---(row vars)--------*/
   int         y_full;         // total screen size in chars
   int         y_top;          // number of chars for info/labels at top
   int         y_bottom;       // number of chars for status/info at bottom
   int         y_avail;        // number of chars availible for cells
   /*---(row working vars)---------------*/
   int         row1;           /* top side of range                             */
   int         row2;           /* bottom side of range                          */
   int         row1_save;      /* top side of range                             */
   int         row2_save;      /* bottom side of range                          */
   /*---(char vars)-------*/
   int         npos;           /* number of characters in current cell          */
   int         cpos;           /* current position                              */
   int         bpos;           /* beginning position in field                   */
   int         epos;           /* ending position in field                      */
   int         apos;           /* number of positions available for field       */
   /*---(marks)-----------*/
   char        mark_show;      /* show temporary marks (y/n)                    */
   /*---(registers)-------*/
   char        reg_curr;
   /*---(count)-----------*/
   /*> char        repeat;         /+ multiplier for normal keys                  +/   <*/
   /*> char        repeat_macro;   /+ multiplier for macro execution              +/   <*/
   /*---(ncurses)---------*/
   char        info_win;
   char        menu;
   char        reqs_list   [LEN_RECD];     /* cell requires                   */
   char        deps_list   [LEN_RECD];     /* cell depends                    */
   char        like_list   [LEN_RECD];     /* cell likes                      */
   char        rpn_list    [LEN_RECD];     /* cell rpn contents               */
   char        reg_list    [LEN_RECD];     /* register contents               */
   char        keys        [LEN_RECD];     /* current keystrokes              */
   /*---(done)------------*/
};
extern    struct cACCESSOR my;



/*---(tabs)---------------------------*/
#define     NTAB        my.ntab
#define     CTAB        my.ctab
#define     PTAB        my.ptab
/*---(columns)------------------------*/
#define     NCOL        my.ncol
#define     CCOL        my.ccol
#define     BCOL        my.bcol
#define     ECOL        my.ecol
#define     FR_COL      my.froz_col
#define     FR_BCOL     my.froz_bcol
#define     FR_ECOL     my.froz_ecol
/*---(rows)---------------------------*/
#define     NROW        my.nrow
#define     CROW        my.crow
#define     BROW        my.brow
#define     EROW        my.erow
#define     FR_ROW      my.froz_row
#define     FR_BROW     my.froz_brow
#define     FR_EROW     my.froz_erow





#define     G_INFO_NONE       ' '
#define     G_INFO_BUFS       ','
#define     G_INFO_MARK       '\''
#define     G_INFO_REGS       '"'
#define     G_INFO_TREG       't'
#define     G_INFO_CELL       'c'
#define     G_INFO_ERROR      'E'
#define     G_INFO_LAYOUT     'L'


#define     MAX_ERROR      10000
typedef  struct cERROR  tERROR;
struct cERROR {
   tCELL      *owner;
   long        when;                        /* timestamp of error reporting   */
   char        phase;                       /* rpn, build, eval, display      */
   char        step;                        /* step in phase                  */
   char        func        [ 20];           /* function reporting issue       */
   char        narg;                        /* particular arg ref             */
   char        type;                        /* type of issue                  */
   char        desc        [100];           /* fuller error message           */
   tERROR     *next;                        /* next error for cell            */
   tERROR     *prev;                        /* next error for cell            */
   tERROR     *gnext;                       /* global error list next         */
   tERROR     *gprev;                       /* global error list prev         */
};
tERROR     *herror;  /* head  */
tERROR     *terror;  /* tail  */
int         nerror;  /* count */
#define     PERR_RPN     'r'
#define     PERR_BUILD   'b'
#define     PERR_EVAL    'e'
#define     PERR_DISP    'd'

#define     TERR_OTHER   '-'
#define     TERR_ARGS    's'
#define     TERR_ADDR    'a'
#define     TERR_RANGE   'r'
#define     TERR_FUNC    'f'



/*====================-----------------+------------------====================*/
/*===----                     CELL DATA STRUCTURE                      ----===*/
/*====================-----------------+------------------====================*/
/*
 *
 *   general discussion...
 *
 *   first we lay in the most fundamental of the data structures, although it's
 *   not the most complex.  a cell is simply a collection of characteristics,
 *   values, pointers to other data structures, and a fair amount of testing
 *   and debugging information which will aid you in verifying the code quality.
 *
 *   there are a couple of simplifications used to keep the complexity down.
 *
 *   first and foremost, the numeric value of the cell is stored as a double
 *   even if it is entered and displayed as another type.  this allows the
 *   calculation code to be very streamlined.  the only twist that adds is that
 *   all intermediates are also doubles and so rounding may be an issue at
 *   microscopic detail.  not my concern for this project.
 *
 *   second, as already stated, strings will only be allowed STR_MAX characters.
 *   this should not present an issue except if the spreadsheet is being used as
 *   a word processor, then you are on your own.  it would be easy enough to
 *   make them of unlimited length if you so desire.  they are stored as char
 *   pointers here anyway so the size won't change.
 *
 *   a point of debate would be whether the cell data structure warrants a
 *   doublly linked list.  don't care about the finer points as its not much
 *   harder, infintesimally slower to add cells, and gives me a very foregiving
 *   structure in case i make a mistake when altering the list.
 *
 *   finally, some would feel that i don't need to keep a few things like the
 *   original length and printable version on the strucuture.  also true, but i
 *   think they both simplify and speed the code with little loss of size.
 *   you can change it later if you like.
 *
 *   on to the data structure.  i have divided it into logical sections for
 *   documentation's sake.  each section has comments that i hope are helpful.
 *   normally i would not do this as then i have to count on the compilier to
 *   to reorder the elements to optimize the storage space.  i like doing all
 *   the work i can myself.
 *
 *   i will not document the workings of the dependency, requirements,
 *   calculation, or cell linked lists here.  i prefer that discussion to come
 *   with the code as it will be easier to see in context and digest.  you'll
 *   have to trust me for now ;)
 *
 *   and, just to get it out of the way, sometimes i use very short variable
 *   names where others whould make them much longer.  personal choice.  once
 *   you get the program working, feel free to tweak that as you so desire.
 *
 *
 *   actual data structure...
 *
 */
struct cCELL {
   /*---(location)-----------------------*/
   char        tab;          /* which tab contains the cell                   */
   short       col;          /* which column contains this cell               */
   short       row;          /* which row contains this cell                  */
   uchar      *label;        /* label of the cell at its current location     */
   long        key;          /* label as a unique long (for sorting)          */
   /*---(source)-------------------------*/
   uchar      *source;       /* unmodified input string just as user typed    */
   short       len;          /* length of input string                        */
   /*---(result)-------------------------*/
   uchar       type;         /* type of contents (program assigned)           */
   double      v_num;        /* cell contents translated to a numeric value   */
   uchar      *v_str;        /* cell contents translated to a string value    */
   /*---(formatting)---------------------*/
   uchar       align;        /* alignment code                                */
   uchar       format;       /* formatting/filler style                       */
   uchar       decs;         /* number of decimals to be shown                */
   uchar       unit;         /* units for conversion                          */
   uchar      *print;        /* printable version of the cell                 */
   uchar       note;         /* note for error, searching, etc                */
   /*---(calculation)--------------------*/
   void       *ycalc;        /* connection to yCALC library                   */
   /*---(master list)--------------------*/
   uchar       linked;       /* 1=linked, 0=unlinked                          */
   tCELL      *m_prev;       /* previous cell in doubly linked list           */
   tCELL      *m_next;       /* next cell in doubly linked list               */
   /*---(btree)--------------------------*/
   tCELL      *b_left;       /* btree left branch                             */
   tCELL      *b_right;      /* btree right branch                            */
   /*---(ties to sheet)------------------*/
   tNODE      *C_parent;     /* parent column                                 */
   tCELL      *c_prev;       /* prev in specific column                       */
   tCELL      *c_next;       /* next in specific column                       */
   tNODE      *R_parent;     /* parent row                                    */
   tCELL      *r_prev;       /* prev in specific row                          */
   tCELL      *r_next;       /* next in specific row                          */
   /*---(end)----------------------------*/
};
tCELL      *hcell;           /* head pointer for cell data structure          */
tCELL      *tcell;           /* tail pointer for cell data structure          */
tCELL      *rcell;           /* root pointer for tree operations              */
int         ncell;           /* count of linked cells in data structure       */
int         acell;           /* count of all cells                            */

#define     LINKED       'y'
#define     UNLINKED     'x'

#define     UNHOOKED    -1

#define     G_TAB_FIXED    'f'
#define     G_TAB_AUTO     'a'
#define     G_TAB_MACRO    'm'
#define     G_TAB_TABLE    't'
#define     G_TAB_DATA     'd'
#define     G_TAB_TYPES    "fmtda"
#define     G_TAB_LOCKED   "FMTDA"

#define     G_TAB_UNLOCK   'x'
#define     G_TAB_LOCK     'X'

#define    G_RESIZE_FIXED  'f'
#define    G_RESIZE_MIN    'm'
#define    G_RESIZE_AUTO   'a'
#define    G_RESIZE_MAX    '*'
#define    G_RESIZE_NADA   '-'


#define     NCEL        ncell
#define     ACEL        acell




/*====================-----------------+------------------====================*/
/*===----                  ROW & COL DATA STRUCTURES                   ----===*/
/*====================-----------------+------------------====================*/
/*
 *
 *   general discussion...
 *
 *   second, in order to define the tab/sheet data structure, we must first
 *   define the information to be stored about the rows and columns.
 *
 *   for columns, the simple one is column width.  for easing the program and
 *   speeding refreshes, it is valuable to store the column label as converted
 *   from the index, i.e., column 0 is A, 3 is C, and 26 is AA.  this only need
 *   be figured out once and the reused each time.  additionally, we'll store
 *   the screen position as well to speed display.
 *
 *   for rows, this initial version will not allow multi-line rows which are
 *   only really handy for entering gobs of text -- not our intent.  also, the
 *   screen labels are the numbers themselves, so the only valuable piece of
 *   information we need at this point is the screen position for each row.
 *   
 *   to conserve a little room, i assumed that there would never be a cell wider
 *   that 255 characters (uchar).
 *
 *
 *   actual data structures...
 *
 */
struct cNODE {
   uchar       type;            /* 'c' column vs 'r' row                      */
   uchar       tab;             /* number of tab                              */
   ushort      ref;             /* sequential identifier                      */
   uchar       size;            /* size                                       */
   tNODE      *N_prev;          /* prev col/row on tab                        */
   tNODE      *N_next;          /* next col/row on tab                        */
   ushort      count;           /* count of cells in col/row                  */
   tCELL      *n_head;          /* first used/real cell in col/row            */
   tCELL      *n_tail;          /* last/bottom used/real cell in col/row      */
};



/*====================-----------------+------------------====================*/
/*===----                      TAB DATA STRUCTURE                      ----===*/
/*====================-----------------+------------------====================*/
/*
 *
 *   general discussion...
 *
 *   third we define the overarching tab/sheet data structure that provides
 *   the iconic spreadsheet feel to the program.  a tab is simply a name
 *   associated with a retangle of potenial cells and a set of basic
 *   characteristics, such as width and height.
 *
 *   the formost simplification is that a retangular matrix of cell pointers
 *   is created from the get go rather than using a clever data structure that
 *   conserves more memory.  while it only allocates pointers, that's still a
 *   great deal of space that will never get used.  but, on the performance
 *   side, a two-dimensional array provides direct access without any need to
 *   search a data structure, no matter how efficient.  it also provides a very
 *   efficient means of conducting operations on visual ranges like columns,
 *   rows, and retangular selections.  i think its well worth it -- you can
 *   adapt it in the long run if you wish.
 *
 *   just like the cell data structure, i have divided it into sections and
 *   provided a little commentary for each.
 *
 *
 *   actual data structure...
 *
 */
struct cTAB {
   /*---(header)-------------------------*/
   /* tabs are pre-allocated and can put into and taken out of use simply by  */
   /* starting to use them.                                                   */
   uchar       tab;                         /* number of tab                  */
   uchar       abbr;                        /* abbreviation of tab            */
   uchar      *name;                        /* tab name for user reference    */
   uchar       type;                        /* tab type                       */
   /*---(columns)------------------------*/
   tNODE      *C_head;
   tNODE      *C_tail;
   ushort      C_count;
   ushort      ncol;                        /* current limit on cols          */
   ushort      ccol;                        /* current column                 */
   ushort      bcol;                        /* beginning column               */
   ushort      ecol;                        /* ending column                  */
   uchar       froz_col;                    /* are the cols frozen            */
   ushort      froz_bcol;                   /* left of frozen cols            */
   ushort      froz_ecol;                   /* right of frozen cols           */
   /*---(rows)---------------------------*/
   tNODE      *R_head;
   tNODE      *R_tail;
   ushort      R_count;
   ushort      nrow;                        /* current limit on rows          */
   ushort      crow;                        /* current row                    */
   ushort      brow;                        /* beginning row                  */
   ushort      erow;                        /* ending row                     */
   uchar       froz_row;                    /* are the rows frozen            */
   ushort      froz_brow;                   /* top of frozen rows             */
   ushort      froz_erow;                   /* bottom of frozen rows          */
   /*---(overall)------------------------*/
   uint        count;                       /* count of entries in sheet      */
   /*---(end)----------------------------*/
};
tTAB    *p_tab;                        /* current tab pointer                 */



























/*
 * calculation types
 *    f = function call
 *    n = constant number
 *    l = constant string literal
 *    s = string from another cell source field
 *    m = string from another cell modified field
 *    v = value from another cell value field
 */
/*> struct cCALC {                                                                     <* 
 *>    char      t;               /+ type of calculation element                  +/   <* 
 *>    double    v;               /+ numeric literal                              +/   <* 
 *>    char     *s;               /+ string literal                               +/   <* 
 *>    tCELL    *r;               /+ pointer to a cell                            +/   <* 
 *>    void    (*f) (void);       /+ function pointer                             +/   <* 
 *>    tCELL    *p;               /+ pointer to parent                            +/   <* 
 *>    tCALC    *n;               /+ pointer to next calc                         +/   <* 
 *> };                                                                                 <*/







/*
 *   selection modes...
 *
 *   selection mode SEL_FROM treats the starting point as fixed and the range
 *   revolves around it to the current point.  this is the traditional
 *   spreadsheet selection method.
 *
 *   selection mode SEL_CUM will grow the range in any direction you move by
 *   adjusting either the starting or ending of the selection as necessary.
 *   this method allows the selection to grow easily in any direction.
 *
 *   selection mode SEL_3D is unimplemented, but would allow the selection to
 *   grow just like SEL_FROM, but also range across tabs to form a cube of data.
 *   this is not that complicated, but needs a real reason to be done.
 *
 *   selection mode SEL_NONE essentially means that the selection is not live
 *   and is the default value.
 *
 */
/*
 *   communication value to represent the end of processing a selection range
 *   used by range actions and formulas
 *
 */
#define     VISU_FROM       'f'
#define     VISU_CUM        'c'
#define     VISU_3D         '3'
#define     VISU_NONE       '-'
/*
 *
 *
 */
tCELL       denada;
#define     DONE_DONE    &denada



/*---(layout_formula)--------------------*/
#define     G_FORMULA_TINY      't'    /* top line shows only source               */
#define     G_FORMULA_SMALL     's'    /* top line shows very little but source    */
#define     G_FORMULA_DEBUG     'n'    /* top line has lots of stuff to debug      */
/*---(layout_status)---------------------*/
#define     G_STATUS_HIDE       ' '
#define     G_STATUS_FILE       'f'
#define     G_STATUS_TAB        'T'
#define     G_STATUS_RPN        'r'
#define     G_STATUS_DEPS       'd'
#define     G_STATUS_REGS       '"'
#define     G_STATUS_TREG       't'
#define     G_STATUS_MARK       'm'
#define     G_STATUS_CELL       'C'
#define     G_STATUS_KEYLOG     'K'
#define     G_STATUS_HISTORY    'H'
#define     G_STATUS_MODELIST   'M'
#define     G_STATUS_ERRORS     'E'
#define     G_STATUS_BUFFER     'B'
#define     G_STATUS_VISUAL     'V'
/*---(layout_command)--------------------*/
#define     G_COMMAND_SHOW      's'
#define     G_COMMAND_HIDE      'h'
#define     G_COMMAND_FLOAT     'f'



#define     MENU_NONE      ' '
#define     MENU_ROOT      '-'



extern int     save;

#define     FILE_BUF    "/var/run/buffer.gyges"
#define     FILE_EXIM   "/root/z_gehye/vi_clip.txt"

char        f_maker     [LEN_RECD];


/*---(strchr validation strings)-----------*/
extern      char     *g_alpha;
extern      char     *g_rcnum;
extern      char     *g_rcops;


typedef struct cCURR tCURR;
struct cCURR {
   uchar       type;
   uchar       label      [LEN_LABEL];
   char        tab;
   short       col;
   short       row;
   short       len;
   uchar       align;
   uchar       format;
   uchar       decs;
   uchar       unit;
};
extern  tCURR g_curr;


extern char    g_contents [LEN_RECD];
extern char    message    [LEN_RECD];
extern char    sta_error;
extern char    special;

extern   char   *g_tbd;

extern char      ver_ctrl;
extern char      ver_num  [10];
extern char      ver_txt  [100];

int     row_formula;
int     row_main;



#define     K_DONE        -1

#define     K_CTRL_B       2
#define     K_CTRL_C       3
#define     K_CTRL_E       5
#define     K_CTRL_F       6
#define     K_CTRL_L      12
#define     K_CTRL_S      19

extern      char          unit_answer [LEN_FULL];




/*===[[ PROTOTYPES ]]=====================================*/
/*---(from s_main.c)-----------------------*/
int       main               (int argc, char *argv[]);


/*===[[ gyges_prog.c ]]=======================================================*/
/*··········>·······················>·········································*/
/*---(preinit)--------------*/
char        PROG_urgents            (int a_argc, char *a_argv []);
/*---(startup)--------------*/
char        PROG__init              (int a_argc, char *a_argv []);
char        PROG__args              (int a_argc, char *a_argv []);
char        PROG__begin             (void);
char        PROG_startup            (int a_argc, char *a_argv []);
/*---(execution)------------*/
char        PROG_dawn               (void);
char        PROG_dusk               (void);
/*---(shutdown)-------------*/
char        PROG__end               (void);
char        PROG_shutdown           (void);
/*---(unittest)-------------*/
char        PROG__unit_loud         (void);
char        PROG__unit_quiet        (void);
char        PROG__unit_end          (void);


/*---(from s.c)----------------------------*/
char      PROG_cleanse         (void);
char      PROG__end             (void);
char      PROG_bigdump            (void *a_file);

/*> char      PROG_main_input      (char  a_mode, char a_key);                        <*/
/*> char      PROG_main_handle     (char  a_key);                                     <*/
/*> char      PROG_main_string     (char *a_keys);                                    <*/

/*> char      PROG_layout_init     (void);                                            <*/
/*> char      PROG_layout_set      (char *a_who, char *a_cat, char *a_opt);           <* 
 *> char      PROG_layout_list     (char *a_who);                                     <* 
 *> char      PROG_layout_entry    (int   a_num, char *a_line);                       <* 
 *> char      PROG_layout_formula  (char *a_opt);                                     <* 
 *> char      PROG_layout_status   (char *a_opt);                                     <* 
 *> char      PROG_layout_command  (char *a_opt);                                     <* 
 *> char      PROG_layout_layout   (char *a_opt);                                     <*/

char     *PROG__unit          (char *a_question, void *a_thing);
char      PROG__testing       (void);






/*===[ GYGES_VISU.C ]]========================================================*/
/*---(init)-----------------*/
char      VISU_init            (void);
char      VISU_clear           (void);
/*---(history)--------------*/
char      VISU_save            (void);
char      VISU_restore         (void);
/*---(setting)--------------*/
char      VISU_start           (int a_tab, int a_col, int a_row, char a_mode);
char      VISU_increase        (int a_tab, int a_col, int a_row);
char      VISU_update          (int a_tab, int a_col, int a_row);
char      VISU_set             (int a_tab, int a_bcol, int a_brow, int a_ecol, int a_erow);
char      VISU_mark            (void);
char      VISU_reverse         (void);
/*---(display)--------------*/
char      VISU_root            (int a_tab, int a_col, int a_row);
char      VISU_selected        (int a_tab, int a_col, int a_row);
/*---(simplifiers)----------*/
char      VISU_islive          (void);
char      VISU_from            (void);
char      VISU_col             (void);
char      VISU_row             (void);
/*---(retrieval)------------*/
char      VISU_range           (int*, int*, int*, int*, int*);
tCELL*    VISU_first           (int*, int*, int*);
tCELL*    VISU_next            (int*, int*, int*);
tCELL*    VISU_firstreal       (int*, int*, int*);
tCELL*    VISU_nextreal        (int*, int*, int*);
char      VISU_status          (char *a_msg);
/*---(submode)--------------*/
char      VISU_submode         (char a_major, char a_minor);

char      SELC_clear           (void);
char      SELC_save            (void);
char      SELC_restore         (void);
char      SELC_start           (void);
char      SELC_increase        (void);
char      SELC_set             (int   a_bpos , int   a_epos);
char      SELC_mode            (char  a_major, char  a_minor);
char      SELC_islive          (void);
int       SELC_from            (void);
int       SELC_to              (void);



/*===[ MARK   ]===============================================================*/
/*---(program)--------------*/
/*> char      MARK_init            (void);                                            <*/
/*> char      MARK_purge           (char  a_scope);                                   <*/
/*> char      MARK_wrap            (void);                                            <*/
/*---(basics)---------------*/
char      MARK_valid           (char  a_mark);
char      MARK_set             (char  a_mark);
char      MARK_unset           (char  a_mark);
char      MARK_return          (char  a_mark);
/*---(find)-----------------*/
char      MARK_which           (void);
char      MARK_find            (char *a_label);
/*---(seq)------------------*/
char      MARK_range           (void);
char      MARK_prev            (void);
char      MARK_next            (void);
/*---(info)-----------------*/
char      MARK_status          (char *a_status);
char      MARK_address         (char  a_mark, short *a_tab, short *a_col, short *a_row);
char      MARK_entry           (char  a_mark, char *a_entry);
char      MARK_list            (char *a_list);
char      MARK_listplus        (char *a_list);
/*---(files)----------------*/
char      MARK_write           (char  a_index);
char      MARK_write_head      (FILE *a_file);
char      MARK_write_foot      (FILE *a_file, int a_count);
char      MARK_writeall        (FILE *a_file);
char      MARK_read            (char  a_mark, char *a_label);
/*---(command)--------------*/
char      MARK_define          (char *a_string);
/*---(mode)-----------------*/
char      MARK_submode         (char  a_major, char a_minor);


/*===[ REG  ]=================================================================*/
#define   REG_CURR           '!'

char      REG_init           (void);
char      REG_clear          (char a_reg, char a_init);
char      REG_purge          (char a_init);

char      REG__hook          (tCELL *a_curr, char a_buf, char a_note);
char      REG__unhook        (tCELL *a_curr);

char      REG_mode           (int a_prev , int a_curr);

/*---(text registers)-------*/
/*> char      TREG_init          (void);                                                               <* 
 *> char      TREG_entry         (char  a_reg   , char *a_list);                                       <* 
 *> char      TREG_clear         (void);                                                               <* 
 *> char      TREG_delete        (void);                                                               <* 
 *> char      TREG_copy          (void);                                                               <* 
 *> char      TREG_replace       (void);                                                               <* 
 *> char      TREG_paste         (char  a_dir);                                                        <* 
 *> char      TREG_mode          (int   a_major , int   a_minor);                                      <* 
 *> char      TREG_read          (char  a_reg, char *a_label, int a_beg, int a_end, char *a_source);   <* 
 *> char      TREG_write         (FILE *a_file, int  *a_seq, char a_buf);                              <*/

char      REG_set            (char a_reg);
char      REG_list           (char a_buf, char *a_list);

char      REG_killer              (tCELL *a_curr);
char      REG_copy_one            (tCELL *a_curr, long a_stamp);

char      REG_save             (void);
char      REG_delorig          (void);
char      REG_copy             (void);
char      REG_cut              (void);

#define   G_PASTE_MERGE        '-'
#define   G_PASTE_CLEAR        'y'

#define   G_PASTE_NORM         'n'
#define   G_PASTE_REPL         'r'
#define   G_PASTE_DUPL         'd'
#define   G_PASTE_MOVE         'm'
#define   G_PASTE_FORCE        'f'

char      REG_inside           (int  a_index, short a_tab, short a_col, short a_row);
char      REG__paste_check     (void);
char      REG__paste_clear     (char a_clear);
char      REG__paste_cells     (char a_reqs);
char      REG__paste_pros      (char a_pros);
char      REG__paste_integ     (char a_integ);
char      REG__paste_main      (char a_clear, char a_reqs, char a_pros, char a_integ);
char      REG_paste            (char a_type);

/*---(data import)----------*/
char      REG__inpt_prep       (char a_style);
char      EXIM_mapping         (void);
char      REG__inpt_read       (void);
char      REG__inpt_width      (short a_col, short a_wide);
char      REG__inpt_sizer      (void);
char      REG__inpt_place      (short a_col, short a_row, char *a_value);
char      REG__inpt_values     (short a_row);
char      REG__inpt_delims     (short a_row);
char      REG__inpt_done       (void);
char      REG_inpt_driver      (char a_style);

char      REG_valuesout      (char a_trim);

char      REG_entry          (char a_reg, char *a_list);
char      REG_read           (char a_reg, int a_tab, char *a_beg, char *a_end, char *a_min, char *a_max, char a_type);
char      REG_write          (FILE *a_file, int  *a_seq, char a_buf);
char      REG_bufwrite       (char a_buf);

int       REG__reg2index     (char a_reg);
int       REG__reg2tab       (char a_reg);
int       REG__tab2index     (int  a_tab);

char      REG__setter        (char *a_request, char *a_data);
char*     REG__getter        (char *a_question, char a_reg);


/*---(sub-modes)------------*/
char      SMOD_buffer          (char  a_major, char  a_minor);
char      EXIM_init            (void);
char      EXIM_import          (char a_style);
char      EXIM_export          (char a_style);
char      SMOD_wander          (char  a_major, char  a_minor);
char      SMOD_error           (char  a_major, char  a_minor);
char      SMOD_menus           (char  a_major, char  a_minor);
/*---(micro-modes)----------*/
/*> char      UMOD_repeat          (char  a_major, char  a_minor);                    <*/
/*> char      UMOD_repeat_done     (void);                                            <*/

/*> char      KEYS_init          (void);                                              <*/
char      KEYS_record        (char  a_curr);
char*     KEYS__unit         (char *a_question);
char        KEYS_quit        (void);
char        KEYS_writequit   (void);



/*---(ncurses)----------------------------------*/

char        CURS_current_status     (char a_size, short a_wide, char *a_list);
char        CURS_status_detail      (char a_size, short a_wide, char *a_list);
char        CURS_status_cell        (char a_size, short a_wide, char *a_list);
char        CURS_status_deps        (char a_size, short a_wide, char *a_list);
char        CURS_status_rpn         (char a_size, short a_wide, char *a_list);
char        CURS_status_file        (char a_size, short a_wide, char *a_list);
/*> char        CURS_status_buffer   (char *a_list);                                  <*/
/*> char        CURS_status_textreg  (char *a_list);                                  <*/
char        CURS_status_mark        (char *a_list);
char        CURS_status_tab         (char a_size, short a_wide, char *a_list);
char        CURS_status_reg         (char *a_list);
char        CURS_status_visual      (char *a_list);
char        CURS_status_error       (char a_size, short a_wide, char *a_list);
char        CURS_status_history     (char *a_list);


/*> char      CURS_screen_reset    (void);                                            <*/
char      DRAW_init            (void);
char      DRAW_wrap            (void);
char      DRAW_xaxis           (void);
char      DRAW_yaxis           (void);
char      DRAW_buffer          (void);
char      CURS_bufsum          (char *a_list);
char      CURS_bufdet          (char *a_list);
char      DRAW_main            (void);
char      DRAW_coloration      (char *a_opt);
char      CURS_playback        (void);
char      CURS_col_head        (void);
char      CURS_row_head        (void);
char      CURS_cell            (int a_col, int a_row, short a_ypos, short a_xpos, short a_wide);
char      CURS_size            (void);
char      CURS_info_cell       (void);
char      CURS_info_layout     (void);
/*> char      CURS_list_mark         (void);                                          <*/
/*> char      CURS_listreg         (void);                                            <*/
/*> char      CURS_listtreg        (void);                                            <*/

char      CALC_init            (void);
char      CALC_cleanse         (tCELL *a_cell);
char      CALC_build           (tCELL *a_curr);
char      CALC_free            (tCELL *a_curr);
char      CALC_eval            (tCELL *a_curr);
char      calc_show            (void);
char*     CALC_strtok          (char *a_str);

char      CALC__find_fcat      (char a_fcat);
char      CALC__func_oper      (char *a_oper, char *a_terms, char *a_show);
char      CALC__func_func      (char *a_func, char *a_terms, char *a_show);
char      CALC_func_list       (void);

tERROR*   ERROR_create         (tCELL  *a_owner);
char      ERROR_delete         (tERROR *a_error);
char      ERROR_add            (tCELL  *a_owner, char a_phase, int a_step, char *a_func, char a_type, char *a_desc);
char      ERROR_entry          (tCELL  *a_cell, char a_seq, char *a_list);
char      ERROR_list           (void);
char      ERROR_cleanse        (tCELL  *a_owner);


/*---(overall)------------------------*/
#define     G_RPN_IGNORE         '-'    /* nothing to do                      */
/*---(requires)-----------------------*/
#define     G_RPN_RNONE          'n'    /* no cell adjustments                */
#define     G_RPN_RREL           'r'    /* adjust all relative references     */
#define     G_RPN_RINNER         'i'    /* adjust all (rel/abs) inner refs    */
#define     G_RPN_RBOTH          'b'    /* adjust both inner and relative     */
#define     G_RPN_REVERY         'e'    /* adjust all inner, rel, and abs     */
#define     G_RPN_REQS           "nribe"
#define     G_RPN_RINSIDE        "ibe"
/*---(provides)-----------------------*/
#define     G_RPN_PNONE          'N'    /* no provider adjustments            */
#define     G_RPN_PREL           'R'    /* adjust rel provider refs in source */
#define     G_RPN_PALL           'A'    /* adjust all provider refs in source */
#define     G_RPN_PSPLIT         'S'    /* adjust all provider refs in source */
#define     G_RPN_PROS           "NRAS"
/*---(compound)-----------------------*/
#define     G_RPN_ALL            "nribeNRAS"
/*---(adjustments)--------------------*/
/*> char      RPN_adjust           (tCELL *a_cell, int a_toff, int a_coff, int a_roff, char *a_source);   <*/
/*> char      RPN_adjust_reg       (tCELL *a_cell, char a_scope, int a_toff, int a_coff, int a_roff, char *a_source, int a_index);   <*/
/*> char      RPN_adjust_ref       (tCELL *a_cell, char a_scope, int a_toff, int a_coff, int a_roff, char *a_source, char *a_label);   <*/
/*---(conversion)---------------------*/
/*> char      RPN_convert          (tCELL *a_curr);                                   <*/



/*---(external)-------------*/
/*> #define     G_DEP_BLANK          '-'                                              <* 
 *> #define     G_DEP_REQUIRE        'R'                                              <* 
 *> #define     G_DEP_PROVIDE        'p'                                              <* 
 *> #define     G_DEP_RANGE          'P'                                              <* 
 *> #define     G_DEP_CELL           'c'                                              <* 
 *> #define     G_DEP_FORMAT         'F'                                              <* 
 *> #define     G_DEP_COPY           'f'                                              <* 
 *> #define     G_DEP_SOURCE         'S'                                              <* 
 *> #define     G_DEP_LIKE           'l'                                              <* 
 *> #define     G_DEP_MERGED         'M'                                              <* 
 *> #define     G_DEP_BLEED          'b'                                              <* 
 *> #define     G_DEP_CALCREF        'A'                                              <* 
 *> #define     G_DEP_ADDRESS        'a'                                              <* 
 *> #define     G_DEP_ROOT           'r'                                              <* 
 *> #define     G_DEP_UNROOT         'u'                                              <* 
 *> #define     G_DEP_CHECKROOT      'a'                                              <*/
/*---(program)--------------*/
char        DEP_init             (void);
char        DEP__purge           (void);
char        DEP_wrap             (void);
/*---(malloc)---------------*/
tDEP       *DEP__new             (void);
tDEP       *DEP__free            (tDEP *a_dep);


/*345678901-12345678901234567890->--------------------------------------------*/
tCELL*      DEP_merge_source     (tCELL *a_curr);




/*---(single)---------------*/
char      DEP_rooting        (tCELL *a_cell, char a_type);
char      DEP_create         (char a_type, tCELL *a_source, tCELL *a_target);
char      DEP_delete         (char a_type, tCELL *a_source, tCELL *a_target);

char      DEP_delcalcref     (tCELL *a_source);
tCELL    *DEP_delmerge       (tCELL *a_source);
char      DEP_delmergeroot   (tCELL *a_target);

char      DEP_range          /* ------ */  (tCELL *a_from, int a_btab, int a_bcol, int a_brow, int a_etab, int a_ecol, int a_erow);
char      DEP_cleanse        /* ------ */  (tCELL *a_curr);

char      DEP_disp_reqs      /* ------ */  (tCELL  *a_me, char* a_list);
char      DEP_disp_pros      /* ------ */  (tCELL  *a_me, char* a_list);
char      DEP_disp_like      /* ------ */  (tCELL  *a_me, char* a_list);
char      DEP_disp_sort      /* ------ */  (char *a_list);

char      DEP_updatelikes    (tCELL  *a_me);

char      DEP_dump           /* ------ */  (void);
char      DEP_checkall       /* ------ */  (char a_print);
char      DEP_check          /* ------ */  (int a_level, tCELL *a_curr, char a_print, long a_stamp);
char      DEP_circle         (int a_level, tCELL *a_source, tCELL *a_target, long a_stamp);


/*---(sequencing)------------------------*/
char      SEQ__clear         (void);
char      SEQ__add           (char a_level, tCELL *a_cell);
char      SEQ__del           (tCELL *a_cell);
char      SEQ__recursion     (int a_level, tDEP  *a_dep , char a_dir, long a_stamp, char a_action);
char      SEQ__driver        (             tCELL *a_cell, char a_dir, long a_stamp, char a_action, FILE *a_file);
char      SEQ_calc_up        (tCELL *a_cell);
char      SEQ_calc_down      (tCELL *a_cell);
char      SEQ_calc_full      (void);
char      SEQ_wipe_deps      (void);
char      SEQ_file_deps      (long a_stamp);
char      SEQ_reg_deps       (long a_stamp);



char      DEP_seqlist        (char *a_list);

char      DEP_write          (FILE *a_file, int a_level, tDEP *a_dep);
char      DEP_writeall       (void);
char      DEP_writescreen    (void);
char     *DEP_unit           (char *a_question, char *a_label);



/*345678901-12345678901234567890->--------------------------------------------*/
/*---(merge-specific)-----------------*/
char        CELL__merge_valid    (tCELL *a_curr);
tCELL*      CELL__merge_left     (tCELL *a_curr);
char        CELL__merge_right    (tCELL *a_left);
char        CELL__unmerge_right  (tCELL *a_left);
char        CELL_merge           (tCELL *a_curr);
char        CELL_unmerge         (tCELL *a_curr);
/*---(unit-testing)-------------------*/
char        CELL__unitchange     (tCELL *a_cell, char *a_source);








char*       HIST__unit         (char *a_question, int a_ref);


/*345678901-12345678901234567890->--------------------------------------------*/
/*---(program)-------------------*/
char        LOC_init             /* shoot  0----- */  (void);
char        LOC_purge            /* stem   0----- */  (void);
char        LOC_wrap             /* shoot  0----- */  (void);

char        loc__checker_legal      (int a_tab, int a_col, int a_row);
char        loc__checker_adapt      (int a_tab, int a_col, int a_row);
char        loc__checker_adapt      (int a_tab, int a_col, int a_row);
char        LOC_checker             (int a_tab, int a_col, int a_row, int a_nada, char a_check);
char        LOC_legal          (int a_tab, int a_col, int a_row, char a_adapt);

char        LOC_hook             /* stigma 4----- */  (tCELL *a_cell, char a_tab , short a_col , short a_row);
char        LOC_unhook           /* stigma 1----- */  (tCELL *a_cell);
/*> char        LOC_move             /+ stigma 6----- +/  (char  a_tab1, short a_col1, short a_row1, char a_tab2, short a_col2, short a_row2);   <*/

tCELL      *LOC_cell_at_curr     /* petal  0----- */  (void);
tCELL      *LOC_cell_at_loc      /* petal  3----- */  (int a_tab, int  a_col, int  a_row);
tCELL      *LOC_cell_labeled     /* petal  1----- */  (char  *a_label);

char        LOC_coords           /* petal  4----- */  (tCELL *a_cell, int *a_tab, int *a_col, int *a_row);

char        LOC_label            /* petal  1----- */  (tCELL *a_curr, char *a_final);



/*===[[ gyges_tab.c ]]========================================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        TAB_live                (char a_index);
char        TAB__name_check         (char *a_name);
char        TAB_ground              (void);
char        TAB_save                (void);
char        TAB_retrieve            (void);
/*---(memory)-------------------------*/
char        TAB_new                 (tTAB **a_new, char a_index, uchar *a_name, uchar *a_size);
char        TAB_free                (char a_index);
char        TAB_first_filled        (void);
char        TAB_first_open          (void);
char        TAB_new_quick           (void);
char        TAB_new_in_open         (uchar *a_name, uchar *a_size);
char        TAB_new_in_abbr         (uchar a_abbr, uchar *a_name, uchar *a_size);
char        TAB_free_all_empties    (void);
char        TAB_free_from_abbr      (uchar a_abbr);
/*---(search)-------------------------*/
char        TAB_by_cursor           (tTAB **a_found, char *a_start, char a_move);
char        TAB_by_index            (tTAB **a_found, char a_index);
char        TAB_by_abbr             (tTAB **a_found, uchar a_abbr);
char        TAB_by_name             (tTAB **a_found, uchar *a_regex);
char        TAB_browse              (char *a_entry);
char        TAB_pointer             (tTAB **a_found, char a_index);
char        TAB_ensure              (tTAB **a_found, char a_index);
/*---(hooking)------------------------*/
char        TAB_hook_cell           (tTAB **a_found, char a_index, tCELL *a_cell);
char        TAB_unhook_cell         (tCELL *a_cell);
/*---(program)------------------------*/
char        TAB_init                (void);
char        TAB_cleanse             (tTAB *a_tab);
/*---(usage)--------------------------*/
int         TAB_used                (char a_index);
/*---(switching)----------------------*/
char        TAB_switch              (char a_index);
char        TAB_switch_key          (uchar a_key);
/*---(names)--------------------------*/
char        TAB_name                (char a_index, char *a_name);
char        TAB_rename              (char a_index, char *a_name);
char        TAB_rename_curr         (char *a_name);
/*---(types)--------------------------*/
char        TAB_type                (char a_index);
char        TAB_retype              (char a_index, char a_type);
/*---(sizing)-------------------------*/
char        TAB_size                (char a_index, char *a_max);
char        TAB_size_detail         (char a_index, ushort *a_col, ushort *a_row);
char        TAB_resize              (char a_index, char *a_max);
char        TAB_resize_curr         (char *a_max);
/*---(mapping)------------------------*/
char        TAB_mapper              (void);
char        TAB_entry               (ushort a_pos, short *r_ref, uchar *r_wide, uchar *r_used);

/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
/*---(display)------------------------*/
char        TAB_line                (char a_index, char a_size, char *a_list);
char        TAB_status_curr         (char a_size, short a_wide, char *a_list);
char        TAB_inventory           (char a_size, char *a_list);
/*---(unit_test)----------------------*/
char*       TAB__unit            (char *a_question, int a_tab);



#define     LEGAL_TAB(A)       LEGAL_tab (NTAB, A)
#define     LEGAL_COL(A,B)     LEGAL_col (COL_max (A), B)
#define     LEGAL_ROW(A,B)     LEGAL_row (ROW_max (A), B)


#define     IS_COL     'c'
#define     IS_ROW     'r'

/*===[[ gyges_node.c ]]=======================================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
/*---(program)------------------------*/
short       NODE_cleanse            (tTAB *a_tab, char a_type);
char        NODE_init               (void);
/*---(memory)-------------------------*/
char        NODE_new                (tNODE **a_new, tTAB *a_tab, char a_type, ushort a_ref);
char        NODE_free               (tNODE **a_old);
/*---(search)-------------------------*/
char        NODE_by_cursor          (tNODE **a_found, tTAB *a_tab, char a_type, char a_move);
char        NODE_by_index           (tNODE **a_found, tTAB *a_tab, char a_type, ushort a_ref);
char        NODE_ensure             (tNODE **a_found, tTAB *a_tab, char a_type, ushort a_ref);
/*---(hooking)------------------------*/
char        NODE_hook_cell          (tTAB *a_tab, char a_type, ushort a_ref, ushort a_seq, tCELL *a_cell);
char        NODE_unhook_cell        (char a_type, tCELL *a_cell);
/*---(usage)--------------------------*/
short       NODE_used               (char a_index, char a_type, short a_ref);
short       NODE_min_used           (char a_index, char a_type);
short       NODE_max_used           (char a_index, char a_type);
short       NODE_max                (char a_index, char a_type);
short       NODE__max_set           (char a_index, char a_type, short a_max);
short       NODE_max_set            (char a_index, char a_type, short a_max);
short       NODE_max_adjust         (char a_index, char a_type);
/*---(sizing)-------------------------*/
char        NODE_size               (char a_tab, char a_type, short a_ref);
char        NODE_resize             (char a_tab, char a_type, short a_ref, char a_size);
/*---(freezing)-----------------------*/
char        NODE_freeze             (int a_tab, char a_type, int a_beg, int a_end);
char        NODE_unfreeze           (int a_tab, char a_type);
/*---(files)--------------------------*/
char        NODE_reader             (int c, uchar *a_verb);
char        NODE_writer             (int c, char a_type, tNODE *a_node, short n);
char        NODE_writer_one         (char a_index, char a_type, short a_ref);
char        NODE_writer_all         (char a_type);
/*---(mapping)------------------------*/
/*> char        NODE_map_clear          (tMAPPED **a_map, tTAB **a_tab, int *a_max, int *a_curr, tNODE **a_node, char a_type);   <*/
/*> char        NODE_map_mapper         (tMAPPED *a_map, tTAB *a_tab, int a_max, char a_type);   <*/
/*> char        NODE_map_display        (tMAPPED *a_map, char a_type, char a_section, uchar *a_out);   <*/
/*> char        NODE_map_used           (tMAPPED *a_map, tNODE *a_node, int a_max, int a_curr, char a_type);   <*/
/*> char        NODE_map_absolute       (tMAPPED *a_map, tTAB *a_tab, int a_max, char a_type);   <*/
/*> char        NODE_map_local          (tMAPPED *a_map, int a_max, int a_curr, tNODE *a_node, char a_type);   <*/
/*> char        NODE_map_ends           (uchar *a_map, int a_curr, int *a_prev, int *a_next);   <*/
char        NODE_entry              (char a_type, ushort a_pos, short *r_ref, uchar *r_wide, uchar *r_used);
/*---(unit_test)----------------------*/
char*       NODE__unit              (char *a_question, uchar a_tab, char a_type, ushort a_ref);



/*===[[ gyges_col.c ]]========================================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
/*---(memory)-------------------------*/
char        COL__new                (tNODE **a_new, tTAB *a_tab, ushort a_col);
char        COL__free               (tNODE **a_old);
/*---(search)-------------------------*/
char        COL__by_cursor          (tNODE **a_found, tTAB *a_tab, char a_move);
char        COL__by_index           (tNODE **a_found, tTAB *a_tab, ushort a_col);
char        COL__ensure             (tNODE **a_found, tTAB *a_tab, ushort a_col);
/*---(hooking)------------------------*/
char        COL_hook_cell           (tTAB *a_tab, ushort a_col, ushort a_row, tCELL *a_cell);
char        COL_unhook_cell         (tCELL *a_cell);
/*---(usage)--------------------------*/
short       COL_used                (char a_index, short a_ref);
short       COL_min_used            (char a_index);
short       COL_max_used            (char a_index);
short       COL_max                 (char a_index);
short       COL_max_set             (char a_index, short a_max);
short       COL_max_adjust          (char a_index);
/*---(program)------------------------*/
char        COL_cleanse             (tTAB *a_tab);
char        COL_cleanse_curr        (void);
/*---(sizing)-------------------------*/
char        COL_size                (char a_index, short a_ref);
char        COL_resize              (char a_index, short a_ref, char a_size);
/*---(freezing)-----------------------*/
char        COL_freeze              (char a_index, short a_bcol, short a_ecol);
char        COL_unfreeze            (char a_index);
/*---(files)--------------------------*/
char        COL_reader              (int c, uchar *a_verb);
char        COL_writer              (char a_index, short a_ref);
char        COL_writer_all          (void);
/*---(mapping)------------------------*/
char        COL_mapper              (char a_level);
char        COL_entry               (ushort a_pos, short *r_ref, uchar *r_wide, uchar *r_used);
/*---(unit_test)----------------------*/
char*       COL__unit               (char *a_question, uchar a_tab, ushort a_col);
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/



/*===[[ gyges_row.c ]]========================================================*/
/*---(memory)-------------------------*/
char        ROW__new                (tNODE **a_new, tTAB *a_tab, ushort a_row);
char        ROW__free               (tNODE **a_old);
/*---(search)-------------------------*/
char        ROW__by_cursor          (tNODE **a_found, tTAB *a_tab, char a_move);
char        ROW__by_index           (tNODE **a_found, tTAB *a_tab, ushort a_row);
char        ROW__ensure             (tNODE **a_found, tTAB *a_tab, ushort a_row);
/*---(hooking)------------------------*/
char        ROW_hook_cell           (tTAB *a_tab, ushort a_col, ushort a_row, tCELL *a_cell);
char        ROW_unhook_cell         (tCELL *a_cell);
/*---(usage)--------------------------*/
short       ROW_used                (char a_index, short a_ref);
short       ROW_min_used            (char a_index);
short       ROW_max_used            (char a_index);
short       ROW_max                 (char a_index);
short       ROW_max_set             (char a_index, short a_max);
short       ROW_max_adjust          (char a_index);
/*---(program)------------------------*/
char        ROW_cleanse             (tTAB *a_tab);
char        ROW_cleanse_curr        (void);
/*---(sizing)-------------------------*/
char        ROW_size                (char a_index, short a_ref);
char        ROW_resize              (char a_index, short a_ref, char a_size);
/*---(freezing)-----------------------*/
char        ROW_freeze              (char a_index, short a_bref, short a_eref);
char        ROW_unfreeze            (char a_index);
/*---(files)--------------------------*/
char        ROW_reader              (int c, uchar *a_verb);
char        ROW_writer              (char a_index, short a_ref);
char        ROW_writer_all          (void);
/*---(mapping)------------------------*/
char        ROW_mapper              (char a_level);
char        ROW_entry               (ushort a_pos, short *r_ref, uchar *r_wide, uchar *r_used);
/*---(unit_test)----------------------*/
char*       ROW__unit               (char *a_question, uchar a_tab, ushort a_row);
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/



char        MAP_mapper           (char a_req);

/*---(unit testing)--------------*/
char*       LOC__unit            /* petal  2----- */  (char *a_question, char *a_label);
char*       LOC__unit_OLD        /* petal  2----- */  (char *a_question, tCELL *a_cell);




char      CELL__wipe         /* ------ */  (tCELL *a_cell);




#define   CELL_FIXED         'n'
#define   CELL_GROW          'y'
#define   CELL_EXACT         'e'



/*---(basics)----------------------------*/
char        CELL__valid          /* ------ */  (tCELL  *a_cell, char a_linked);
/*345678901-12345678901234567890->--------------------------------------------*/
/*---(memory)----------------------------*/
char        CELL__new            (tCELL **a_cell, char a_linked);
char        CELL__free           (tCELL **a_cell);
char        CELL__create         /* ------ */  (tCELL **a_cell, int  a_tab, int  a_col, int  a_row);
char        CELL__delete       (char a_mode, int a_tab, int a_col, int a_row);
char        CELL_dup             /* ------ */  (tCELL **a_cell, tCELL* a_old);





char      CELL_change          (tCELL **a_cell, char a_mode, int  a_tab, int  a_col, int  a_row, char *a_source);
tCELL    *CELL_overwrite       (char a_mode, int a_tab, int a_col, int a_row, char *a_source, char *a_format);

/*> char      CELL__depwipe        (FILE *a_file, char a_type, int *a_seq, int a_level, tCELL *a_curr, long a_stamp);   <*/
char      CELL_purge           (void);

char      CELL__numerics       (tCELL *a_cell);

/*> char      CELL__rpn            (tCELL *a_cell);                                   <*/
char      CELL__build          (tCELL *a_cell);
char      CELL__like           (tCELL *a_cell);
char      CELL__point          (tCELL *a_cell);
char      CELL__eval           (tCELL *a_cell);
char      CELL__formulas       (tCELL *a_cell);

char      CELL__ftype          (char a_prefix);
char      CELL_init            (void);
char      CELL_wrap            (void);
char      CELL__interpret      (tCELL *a_curr);


char      CELL_align           (tCELL *a_curr, char a_abbr);
char      CELL_format          (tCELL *a_curr, char a_abbr);
char      CELL_decimals        (tCELL *a_curr, char a_abbr);
char      CELL_units           (tCELL *a_curr, char a_abbr);

char      CELL_erase           (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num);
char      CELL_merge_visu      (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num);
char      CELL_unmerge_visu    (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num);
/*> char      CELL_visual          (char   a_what, char a_mode, char a_how);          <*/

/*> #define   CHANGE_WIDTH    'w'                                                     <* 
 *> #define   CHANGE_HEIGHT   'h'                                                     <* 
 *> #define   CHANGE_FORMAT   'f'                                                     <* 
 *> #define   CHANGE_UNITS    'K'                                                     <* 
 *> #define   CHANGE_ERASE    'e'                                                     <* 
 *> #define   CHANGE_ALIGN    'a'                                                     <* 
 *> #define   CHANGE_DECIMAL  'd'                                                     <* 
 *> #define   CHANGE_MERGE    'm'                                                     <* 
 *> #define   CHANGE_UNMERGE  'u'                                                     <*/



char        CELL_dump               (FILE *a_file);
char*       CELL__unit_better       (char *a_question, tCELL *a_cell, char *a_label, int a_ref);
char       *CELL__unit              (char  *a_question, tCELL *a_cell);
char       *CELL__unitnew           (char  *a_question, char *a_label);



/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        FILE_recd_shove         (char *a_recd);
char        FILE_init               (void);
char        FILE_prepper            (void);
char        FILE_finisher           (void);
char        FILE_new                (void);
char        FILE_rename             (char  *a_name);
char        FILE_write              (void);
/*> char      FILE_writeas         (char *a_name);                                    <*/
char*       FILE__unit              (char *a_question, int a_ref);
char        EXIM__import_bounds     (void);
char        EXIM__import_sizer      (void);


/*---(file)------------------*/


char        TABS_reader             (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i);
char        TABS_writer             (char  a_tab);
char        TABS_writer_all         (void);

char        TAB_reader              (int c, uchar *a_verb);
char        TAB_writer              (int c, char  a_tab);
char        TAB_writer_all          (void);

char        CELL_reader             (int c, uchar *a_verb);
char        CELL_writer             (uchar *a_verb, tCELL *a_curr);
char        CELL_writer_all         (void);






/*===[[ gyges_calc.c ]]=======================================================*/

char        api_ycalc_enabler       (void *a_owner, void *a_deproot);
char        api_ycalc_pointer       (void *a_owner, char **r_source, char **r_type, double **r_value, char **r_string);
char        api_ycalc_reaper        (void **a_owner);

char        api_ycalc_named         (char *a_label, char a_force, void **a_owner, void **a_deproot);
char        api_ycalc_whos_at       (int b, int x, int y, int z, char a_force, void **a_owner, void **a_deproot);
char*       api_ycalc_labeler       (void *a_owner);

char        api_ycalc_valuer        (void *a_owner, char *a_type, double *a_value, char **a_string);
char        api_ycalc_address       (void *a_owner, int *b, int *x, int *y, int *z);
char        api_ycalc_special       (void *a_owner, char a_what, double *a_value, char **a_string);

char        api_ycalc_printer       (void *a_owner);

/*345678901 12345678901 12345678901 12345678901 12345678901 12345678901 123456*/
char*       api_ycalc__unit         (char *a_question, char *a_label);


/*===[[ gyges_yvikeys.c ]]====================================================*/
char        api_vikeys_init         (void);
char        api_yvikeys_handlers    (void);
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        api_yvikeys_axis_get    (char a_axis, ushort *n, ushort *b, ushort *c, ushort *e);
char        api_yvikeys_axis_set    (char a_axis, ushort b, ushort c, ushort e);
char        api_yvikeys_axis_entry  (char a_axis, ushort a_pos, short *a_ref, uchar *a_size, uchar *a_used);
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        api_yvikeys_saver       (char *a_contents);
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        api_yvikeys_macro_get   (char   a_name, char  *a_keys);
char        api_yvikeys_macro_set   (char   a_name, char  *a_keys);
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        api_yvikeys_searcher    (uchar a_not, uchar *a_search);
char        api_yvikeys_unsearcher  (uchar *a_label, ushort u, ushort x, ushort y, ushort z);
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        api_yvikeys_copier      (char a_type, long a_stamp);
char        api_yvikeys_clearer     (char a_1st, ushort u, ushort x, ushort y, ushort z);
char        api_yvikeys_router      (tCELL *a_cell, char *a_list);
char        api_yvikeys_paster      (char a_reqs, char a_pros, char a_intg, char a_1st, ushort a_uoff, ushort a_xoff, ushort a_yoff, ushort a_zoff, tCELL *a_cell, char *a_list);
char        api_yvikeys_finisher    (ushort a_uoff, ushort a_xoff, ushort a_yoff, ushort a_zoff, tCELL *a_cell);
char        api_yvikeys_regkiller   (tCELL *a_curr);
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        api_yvikeys_exim        (char  a_dir  , char  a_style);
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char*       api_myvikeys__unit      (char *a_question, int a_num);


/*===[[ gyges_ymap.c ]]=======================================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
/*---(label)----------------*/
char        api_ymap_locator        (char a_strict, char *a_label, ushort *u, ushort *x, ushort *y, ushort *z);
char        api_ymap_addressor      (char a_strict, char *a_label, ushort u, ushort x, ushort y, ushort z);
/*---(load)-----------------*/
char        api_ymap_sizer          (char a_axis, ushort *n, ushort *a, ushort *b, ushort *c, ushort *e, ushort *m, ushort *x);
char        api_ymap_entry          (char a_axis, ushort a_pos, short *r_ref, uchar *r_wide, uchar *r_used);
/*---(update)---------------*/
char        api_ymap_placer         (char a_axis, ushort b, ushort c, ushort e);
char        api_ymap_done           (void);
/*---(other)----------------*/
char        api_ymap_mundo          (char a_dir, char a_act, char *a_label, char *a_format, char *a_content);
char        api_ymap_formatter      (uchar a_type, uchar a_abbr, ushort u, ushort x, ushort y, ushort z, uchar *r);
/*---(done)-----------------*/


char        api_ysort_init          (void);
long        api_ysort_label2key     (char *a_label);
long        api_ysort_coord2key     (int a_tab, int a_col, int a_row, char *a_label);
char        api_ysort_by_label      (tCELL **a_found, char *a_label);
char        api_ysort_by_coord      (tCELL **a_found, int a_tab, int a_col, int a_row);
char        api_ysort_update        (void);
char        api_ysort_btree_dump    (FILE *a_file);
char*       api_ysort__unit         (char *a_question, int n);



/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/

/*===[ SCRP   ]===============================================================*/
/*345678901-12345678901234567890->--------------------------------------------*/


#endif



/*===============================[[ end code ]]===============================*/
/* htag :  714,  326 docs (36%),  386 code (72%),  326 othr (21%),  304 slocl */
