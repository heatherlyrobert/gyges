/*===============================[[ beg code ]]===============================*/
/* htag ::  714,  326 docs 36%,  386 code 72%,  326 othr 21%,  304 slocl      */

/*===[[ HEADER ]]=============================================================*

 *   focus         : (TO) tools
 *   niche         : (SS) spreadsheet
 *   application   : gyges
 *   purpose       : light, clean, vim-ish, technical, and wicked spreadsheet
 *
 *   base_system   : gnu/linux   (powerful, ubiquitous, technical, and hackable)
 *   lang_name     : ansi-c      (wicked, limitless, universal, and everlasting)
 *   dependencies  : ncurses, yLOG, yRPN
 *   size goal     : toolish (around 10,000 slocL)
 * 
 *   author        : the_heatherlys
 *   created       : 2010-09
 *   priorities    : direct, simple, brief, vigorous, and lucid (h.w. fowler)
 *   end goal      : loosely coupled, strict interface, maintainable, portable
 *
 */
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




/*===[[ STANDARD C LIBRARIES ]]===============================================*/
#include    <stdio.h>        /* C_ANSI : strcpy, strlen, strchr, strcmp, ...  */
#include    <string.h>       /* C_ANSI : printf, snprintf, fgets, fopen, ...  */
#include    <stdlib.h>       /* C_ANSI : exit                                 */
#include    <math.h>         /* C_ANSI : fabs, cos, sin, round, trunc, ...    */
#include    <ctype.h>        /* C_ANSI : tolower, toupper, ...                */
#include    <time.h>         /* C_ANSI : time, strftime, localtime            */
#include    <malloc.h>       /* C_ANSI : malloc, free                         */

/*---(posix standard)--------------------*/
#include   <unistd.h>             /* POSIX  standard operating system API     */
#include   <sys/time.h>           /* POSIX  standard time access              */

/*===[[ DE-FACTO STANDARD LIBRARIES ]]========================================*/
#include    <ncurses.h>      /* CURSES : mvprintw, refresh, getch, ...        */

/*===[[ CUSTOM LIBRARIES ]]===================================================*/
#include    <yURG.h>         /* CUSTOM : heatherly urgent processing          */
#include    <yLOG.h>         /* CUSTOM : heatherly program logging            */
#include    <yRPN.h>         /* CUSTOM : heatherly infix to RPN conversion    */
#include    <yVIKEYS.h>      /* CUSTOM : heatherly vi_keys standard           */
#include    <ySTR.h>         /* CUSTOM : heatherly string handling            */
#include    <yVAR.h>         /* CUSTOM : heatherly variable testing           */
#include    <yREGEX.h>       /* CUSTOM : heatherly regular expressions        */
#include    <yCOLOR.h>             /* heatherly color library                 */
#include    <yCALC.h>        /* CUSTOM : heatherly interactive calculation    */



#define     PR        static
#define     PRIV      static

/* rapidly evolving version number to aid with visual change confirmation     */
#define     VER_NUM   "3.2h"
#define     VER_TXT   "updated to handle new shrike font properly"




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
/*> typedef     struct   cDEBUG       tDEBUG;                                         <*/
typedef     struct   cCELL        tCELL;
typedef     struct   cCOLS        tCOLS;
typedef     struct   cROWS        tROWS;
typedef     struct   cTAB         tTAB;
typedef     struct   cDEP         tDEP;          /* inter-cell dependency     */
typedef     struct   cCALC        tCALC;         /* cell calculation entry    */
typedef     struct   cREG         tREG;          /* cut and paste registers   */
typedef     struct   cHIST        tHIST;         /* undo-redo history         */

typedef struct timespec  tTSPEC;


/*===[[ CONSTANTS : LIMITS ]]=================================================*/
/*---(tabs)---------------------------*/
#define     MAX_TABS    38
#define     DEF_TABS    38
#define     MIN_TABS    1
/*---(columns)------------------------*/
/*> #define     MAX_COLS    130                                                       <*/
#define     MAX_COLS    26
#define     DEF_COLS    26
#define     MIN_COLS    1
/*---(rows)---------------------------*/
/*> #define     MAX_ROWS    9999                                                      <*/
#define     MAX_ROWS    100 
#define     DEF_ROWS    100
#define     MIN_ROWS    1
/*---(cell width)---------------------*/
#define     MAX_WIDTH   100
#define     DEF_WIDTH   8
#define     MIN_WIDTH   2
#define     MAX_MERGE   10
/*---(cell height)--------------------*/
#define     MAX_HEIGHT  4
#define     DEF_HEIGHT  1
#define     MIN_HEIGHT  1
/*---(string length)------------------*/
#define     LEN_RECD    4000
#define     LEN_UNIT    500
#define     LEN_STR     500
#define     LEN_DESC    100
#define     LEN_LABEL   20
#define     LEN_ABBR    10
/*---(registers)----------------------*/


char        buf0        [LEN_RECD];

char        keylog      [10000];
int         nkeylog;

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
   /*---(LOC vars)--------*/
   short       ntab;           /* number of worksheet tabs                         */
   short       ctab;           /* current tab                                      */
   short       ncol;
   short       ccol;
   short       bcol;
   short       ecol;
   short       nrow;
   short       crow;
   short       brow;
   short       erow;
   short       tab1;           /* tab of source                                    */
   short       tab2;           /* tab of destination                               */
   char        froz_col;
   short       froz_bcol;
   short       froz_ecol;
   char        froz_row;
   short       froz_brow;
   short       froz_erow;
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
   /*---(#1, LOCATION)-------------------*/
   /*   contains the current location of the cell so it can self-identity in  */
   /*   calculations, display, and testing/debugging.  i've used the short    */
   /*   type to conserve memory, but you can modify this later as desired.    */
   short       tab;          /* which tab contains the cell                   */
   short       col;          /* which column contains this cell               */
   short       row;          /* which row contains this cell                  */
   char        label [10];   /* label of the cell at its current location     */
   /*---(#2, SOURCE)---------------------*/
   /*   stores the original unmodified user-typed string as well as its       */
   /*   length which is required to supporting later editing.  string is      */
   /*   dynamically allocated to conserve space.                              */
   char       *s;            /* unmodified input string just as user typed    */
   short       l;            /* length of input string                        */
   /*---(#3, CONVERSION)-----------------*/
   /*   type characterizes the cell content type, the value of the cell if    */
   /*   numeric, and a modified version of the cell if its not.               */
   /*   see legend below for cell content types.                              */
   char        t;            /* type of contents (program assigned)           */
   double      v_num;        /* cell contents translated to a numeric value   */
   char       *v_str;        /* cell contents translated to a string value    */
   /*---(#4, FORMATTING)-----------------*/
   /*   contains all information required to convert the final cell value     */
   /*   into a printable string to be shown on the screen.                    */
   /*   see the legend below for alignment and formatting codes.              */
   char        a;            /* alignment code                                */
   char        d;            /* number of decimals to be shown                */
   char        f;            /* formatting/filler style                       */
   char       *p;            /* printable version of the cell                 */
   char        n;            /* note for error, searching, etc                */
   /*---(#5, CALCULATION)----------------*/
   /*   in the case that the cell is a formula, this section will contain the */
   /*   conversion into RPN (reverse polish notation) and translation into    */
   /*   a linked list of byte-code to conduct the calculation.                */
   void       *ycalc;            /* connection to yCALC library               */
   /*> char        nrpn;         /+ number of calculation tokens                  +/   <* 
    *> char       *rpn;          /+ rpn version of formula                        +/   <* 
    *> tCALC      *calc;         /+ pointer to head of calculation line           +/   <* 
    *> char        clevel;       /+ calculation level                             +/   <* 
    *> tCELL      *cprev;        /+ pointer to calculation execution prev         +/   <* 
    *> tCELL      *cnext;        /+ pointer to calculation execution next         +/   <*/
   /*---(#6, DEPENDENCIES)---------------*/
   /*   cells are related to each other as lists of requirements for this     */
   /*   calculation and a list of cells that are dependent on this one.       */
   /*   both lists are kept to enable full dependency calculation.            */
   /*> char        nrequire;     /+ number of required cells                      +/   <* 
    *> tDEP       *requires;     /+ incomming predesesors to this calc            +/   <* 
    *> char        nprovide;     /+ number of dependent cells                     +/   <* 
    *> tDEP       *provides;     /+ outgoing successors to this calc              +/   <* 
    *> long        u;            /+ timestamp of last update run                  +/   <*/
   /*---(#7, CELL LIST)------------------*/
   /*   all the cells are stored in a doublly linked list in order to make    */
   /*   them easy to manage and verify as a entire population.  the linked    */
   /*   field helps drive hooking and unhooking cells from sheets.            */
   char        linked;       /* 1=linked, 0=unlinked                          */
   tCELL      *next;         /* next cell in doubly linked list               */
   tCELL      *prev;         /* previous cell in doubly linked list           */
   /*---(#8, ERRORS)---------------------*/
   /*> char        nerror;       /+ number of current errors                      +/   <* 
    *> tERROR     *errors;       /+ error entries                                 +/   <*/
   /*---(end)----------------------------*/
};
/*
 *   data structure variables...
 *
 *   in order to enable the doublly linked list of cells as well as a little
 *   other safety, i have included the following globals.  i follow an odd
 *   convension, but it serves me well so far.  i use prefixes to indicate
 *   the supporting variables for a data structure.
 *      h....      head pointer
 *      t....      tail pointer
 *      n....      count of elements
 *   there are more, but that might just prove how crazy i am.  LOL.
 *      
 */
tCELL      *hcell;           /* head pointer for cell data structure          */
tCELL      *tcell;           /* tail pointer for cell data structure          */
int         ncell;           /* count of linked cells in data structure       */
int         acell;           /* count of all cells                            */
/*
 *
 *   related constants...
 *
 *   for clarity sake, i sometimes use a couple constants so the code does not
 *   get lost in literals.  i have done this for the linked flag on the cell
 *   data structure with the following two constants.
 *
 */
#define     LINKED       'y'
#define     UNLINKED     '-'

#define     UNHOOKED    -1

#define     CELL_BUFFER  1
#define     CELL_SHEET   0



#define     G_TAB_NORMAL   'y'
#define     G_TAB_MACRO    'm'
#define     G_TAB_TABLE    't'
#define     G_TAB_DATA     'd'

#define     NTAB        my.ntab
#define     CTAB        my.ctab

#define     NCOL        my.ncol
#define     CCOL        my.ccol
#define     BCOL        my.bcol
#define     ECOL        my.ecol
#define     FR_COL      my.froz_col
#define     FR_BCOL     my.froz_bcol
#define     FR_ECOL     my.froz_ecol

#define     NROW        my.nrow
#define     CROW        my.crow
#define     BROW        my.brow
#define     EROW        my.erow
#define     FR_ROW      my.froz_row
#define     FR_BROW     my.froz_brow
#define     FR_EROW     my.froz_erow

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
struct cCOLS {
   char      l[3];            /* column label                                 */
   uchar     w;               /* column width                                 */
   ushort    x;               /* starting horizonal position on screen        */
   int       c;               /* optional: count of entries in column         */
};
struct cROWS {
   ushort    h;               /* row height                                   */
   ushort    y;               /* starting vertical position on screen         */
   int       c;               /* optional: count of entries in row            */
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
   short       tab;                         /* number of tab                  */
   char        name  [LEN_ABBR];            /* tab name for user reference    */
   char        type;                        /* tab type                       */
   /*---(contents)-----------------------*/
   /* tabs pull three other data structures together in a package: column     */
   /* characteristics, row characteristics, and a grid on which to hang cells.*/
   tCOLS       cols  [MAX_COLS];            /* column characteristics         */
   tROWS       rows  [MAX_ROWS];            /* row characteristics            */
   tCELL      *sheet [MAX_COLS][MAX_ROWS];  /* cell pointers                  */
   int         c;                           /* count of entries in sheet      */
   short       defwide;                     /* default col width              */
   short       deftall;                     /* default row height             */
   /*---(current size limits)------------*/
   /* while a maximum size sheet is allocated, there are logical user set     */
   /* maximums in order to manage the complexity.                             */
   short       ncol;                        /* current limit on cols          */
   short       nrow;                        /* current limit on rows          */
   /*---(current position)---------------*/
   /* while working, a user changes position to review and manipulate and     */
   /* these variables store the current screen position.                      */
   short       ccol;                        /* current column                 */
   short       crow;                        /* current row                    */
   /*---(screen limits)------------------*/
   /* given user movement, the program calculates and stores the first (beg)  */
   /* and last (end) cols and rows which can be seen.                         */
   short       bcol;                        /* beginning column               */
   short       brow;                        /* beginning row                  */
   short       ecol;                        /* ending column                  */
   short       erow;                        /* ending row                     */
   /*---(frozen rows and cols)-----------*/
   /* in order to handle large volumes of data in a table, it is necessary to */
   /* be able to freeze cols and/or rows so they remain visible               */
   char        froz_col;                    /* are the cols frozen            */
   short       froz_bcol;                   /* left of frozen cols            */
   short       froz_ecol;                   /* right of frozen cols           */
   char        froz_row;                    /* are the rows frozen            */
   short       froz_brow;                   /* top of frozen rows             */
   short       froz_erow;                   /* bottom of frozen rows          */
   /*---(end)----------------------------*/
};
tTAB     s_tabs [MAX_TABS];
tTAB    *p_tab;                        /* current tab pointer                 */











#define     MAX_HIST    100000
struct cHIST {
   char        act         [15];
   int         btab;
   int         bcol;
   int         brow;
   char        before      [LEN_RECD];
   char        after       [LEN_RECD];
};
tHIST       hist        [MAX_HIST];
int         nhist;
int         chist;
char        hist_active;



















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


/*===[ MODES ]================================================================*/
#define     MAX_LAYOUT      100
typedef     struct cLAYOUT  tLAYOUT;
struct cLAYOUT {
   char        cat         [LEN_LABEL];
   char        opt         [LEN_LABEL];
   char        formula;
   char        status;
   char        command;
   char        desc        [LEN_DESC];
};
extern      tLAYOUT     g_layouts [MAX_LAYOUT];


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

char        f_maker     [LEN_RECD];


/*---(strchr validation strings)-----------*/
extern      char     *g_alpha;
extern      char     *g_rcnum;
extern      char     *g_rcops;



extern int     g_done;
extern char    g_contents [LEN_RECD];
extern char    g_saved    [LEN_RECD];
extern char    g_cmd      [10];
extern char    msg_type;
extern char    message    [LEN_RECD];
extern char    sta_error;
extern char    special;

extern char      g_empty    [200];
extern char      g_spaces   [200];
extern char      g_dashes   [200];

extern char      ver_ctrl;
extern char      ver_num  [10];
extern char      ver_txt  [100];

int     row_buffers;
int     row_formula;
int     row_chead;
int     row_main;
int     col_header;
int     col_far;



#define     K_DONE        -1

#define     K_CTRL_B       2
#define     K_CTRL_C       3
#define     K_CTRL_E       5
#define     K_CTRL_F       6
#define     K_CTRL_L      12
#define     K_CTRL_S      19

extern      char          unit_answer [LEN_UNIT];




/*===[[ PROTOTYPES ]]=====================================*/
/*---(from s_main.c)-----------------------*/
int       main               (int argc, char *argv[]);

/*---(from s.c)----------------------------*/
char      PROG_init            (int   argc, char *argv[]);
char      PROG_args            (int   argc, char *argv[]);
char      PROG_begin           (void);
char      PROG_final           (void);
char      PROG_end             (void);

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
char      PROG__unitloud      (void);
char      PROG__unitquiet     (void);
char      PROG__unitend       (void);



char      save_saved         (void);
char      clear_input        (void);



/*---(screen formatting)------------------------*/

char      row_print          (void);
char      EDIT_start         (char *a_prefix);
char      EDIT_prep          (void);
char      EDIT_done          (void);
char      EDIT_pos           (char);



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
char      REG__inpt_map        (void);
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
char      api_yvikeys_format   (int   a_major, int   a_minor);
char      api_yvikeys_exim     (char  a_dir  , char  a_style);
char      EXIM_export          (char a_style);
char      SMOD_wander          (char  a_major, char  a_minor);
char      SMOD_error           (char  a_major, char  a_minor);
char      SMOD_menus           (char  a_major, char  a_minor);
/*---(micro-modes)----------*/
/*> char      UMOD_repeat          (char  a_major, char  a_minor);                    <*/
/*> char      UMOD_repeat_done     (void);                                            <*/

/*> char      KEYS_init          (void);                                              <*/
char      KEYS_record        (char  a_curr);
char      KEYS_reverse       (char  a_curr);
char      KEYS_z_family      (char  a_major, char  a_minor);
char      KEYS_col           (char  a_major, char  a_minor);
char      KEYS_bcol          (int);
char      KEYS_ecol          (int);
char      KEYS_pcol          (void);
char      KEYS_row           (char  a_major, char  a_minor);
char      KEYS_brow          (int);
char      KEYS_erow          (int);
char      KEYS_prow          (void);
char*     KEYS__unit         (char *a_question);
char        KEYS_quit        (void);
char        KEYS_writequit   (void);

/*---(key movements)--------*/
char      MOVE_prep          (void);
char      MOVE_done          (void);

char      KEYS_basics        (char  a_major, char  a_minor);
char      MOVE_horz          (char  a_major);
char      MOVE_vert          (char  a_major);

char      KEYS_gz_family     (char  a_major, char  a_minor);
char      MOVE_gz_horz       (char  a_major, char  a_minor);
char      MOVE_gz_vert       (char  a_major, char  a_minor);

char      KEYS_e_family      (char  a_major, char  a_minor);
char      MOVE_ends          (char  a_minor);
char      MOVE_edges         (char  a_minor);

char      KEYS_c_family      (char  a_major, char  a_minor);

char      KEYS_regbasic      (char  a_major, char  a_minor);

char*     MOVE_unit          (char *a_question, int a_num);


/*---(ncurses)----------------------------------*/

char        CURS_status_cell     (char *a_list);
char        CURS_status_deps     (char *a_list);
char        CURS_status_rpn      (char *a_list);
char        CURS_status_file     (char *a_list);
char        CURS_status_buffer   (char *a_list);
/*> char        CURS_status_textreg  (char *a_list);                                  <*/
char        CURS_status_mark     (char *a_list);
char        CURS_status_tab      (char *a_list);
char        CURS_status_reg      (char *a_list);
char        CURS_status_visual   (char *a_list);
char        CURS_status_error    (char *a_list);
char        CURS_status_history  (char *a_list);


/*> char      CURS_screen_reset    (void);                                            <*/
char      DRAW_init            (void);
char      DRAW_wrap            (void);
char      DRAW_xaxis           (void);
char      DRAW_yaxis           (void);
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
#define     G_DEP_BLANK          '-'
#define     G_DEP_REQUIRE        'R'
#define     G_DEP_PROVIDE        'p'
#define     G_DEP_RANGE          'P'
#define     G_DEP_CELL           'c'
#define     G_DEP_FORMAT         'F'
#define     G_DEP_COPY           'f'
#define     G_DEP_SOURCE         'S'
#define     G_DEP_LIKE           'l'
#define     G_DEP_MERGED         'M'
#define     G_DEP_BLEED          'b'
#define     G_DEP_CALCREF        'A'
#define     G_DEP_ADDRESS        'a'
#define     G_DEP_ROOT           'r'
#define     G_DEP_UNROOT         'u'
#define     G_DEP_CHECKROOT      'a'
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



char        HIST_init          (void);
char        HIST_change        (char *a_type, int a_tab, int a_col, int a_row, char *a_before, char *a_after);
char        HIST_format        (char *a_type, int a_tab, int a_col, int a_row, char  a_before, char  a_after);
char        HIST_size          (char *a_type, int a_tab, int a_col, int a_row, int   a_before, int   a_after);
char        HIST_list          (void);
char        HIST_undo          (void);
char        HIST_redo          (void);
char*       HIST__unit         (char *a_question, int a_ref);


/*345678901-12345678901234567890->--------------------------------------------*/
/*---(program)-------------------*/
char        LOC_init             /* shoot  0----- */  (void);
char        LOC__clear_locs      /* septal 1----- */  (int a_tab);
char        LOC__purge           /* stem   0----- */  (void);
char        LOC_wrap             /* shoot  0----- */  (void);

char        LOC_legal            /* stigma 4----- */  (int a_col, int a_row, int a_tab, char a_adapt);
char        LOC_locator          (char *a_label, int *a_x, int *a_y, int *a_z);
char        LOC_addressor        (char *a_label, int a_x, int a_y, int a_z);

char        LOC_hook             /* stigma 4----- */  (tCELL *a_cell, short a_tab , short a_col , short a_row);
char        LOC_unhook           /* stigma 1----- */  (tCELL *a_cell);
char        LOC_move             /* stigma 6----- */  (short  a_tab1, short a_col1, short a_row1, short a_tab2, short a_col2, short a_row2);

tCELL      *LOC_cell_at_curr     /* petal  0----- */  (void);
tCELL      *LOC_cell_at_loc      /* petal  3----- */  (int  a_col, int  a_row, int a_tab);
tCELL      *LOC_cell_labeled     /* petal  1----- */  (char  *a_label);

char        LOC_coords           /* petal  4----- */  (tCELL *a_cell, short *a_tab, short *a_col, short *a_row);
char        LOC_parse            /* petal  4----- */  (char *a_label, short *a_tab, short *a_col, short *a_row, char  *a_abs);

char        LOC_label            /* petal  1----- */  (tCELL *a_curr, char *a_final);
char        LOC_ref              /* petal  5----- */  (short a_tab, short a_col, short a_row, char a_abs, char *a_label);



/*===[[ gyges_tab.c ]]========================================================*/
char        TAB_valid            /* petal  1----- */  (int a_tab);
char        TAB_label            (int a_tab);
char        TAB_index            (char  a_abbr);
int         TAB_used             (int a_tab);
char        TAB_type             (int a_tab);
char        TAB_retype           (int a_tab, char a_type);
char        TAB_defname          /* petal  2----- */  (int a_tab, char *a_name);
char        TAB_name             /* petal  2----- */  (int a_tab, char *a_name);
char        TAB_rename           /* stigma 2----- */  (int a_tab, char *a_name);
char        TAB_rename_curr      /* stigma 1----- */  (char *a_name);
char        TAB_size             /* petal  2----- */  (int a_tab, char *a_max);
char        TAB_resize           /* stigma 1----- */  (char *a_max);
char        TAB_resize_curr      /* stigma 1----- */  (char *a_max);
char        TAB_colwide          (int a_tab);
char        TAB_rowtall          (int a_tab);
char        TAB_first            (void);
char        TAB_prev             (void);
char        TAB_next             (void);
char        TAB_last             (void);
char        TAB_switch           (int a_tab);
char        TAB_switch_char      (char  a_tab);
char        TAB_status           (char  a_tab, char *a_list);
char        TAB_line             (char  a_tab, char *a_list);



/*===[[ gyges_col.c ]]========================================================*/
char        COL_clear            /* septal 1----- */  (int a_tab);
char        COL_valid            /* petal  2----- */  (int a_tab, int a_col);
int         COL_defmax           /* petal  0----- */  (void);
int         COL_max              /* petal  1----- */  (int a_tab);
int         COL_used             /* petal  2----- */  (int a_tab, int a_col);
int         COL_xpos             /* petal  2----- */  (int a_tab, int a_col);
char        COL_xset             /* stigma 3----- */  (int a_tab, int a_col, int a_pos);
char        COL_label            /* petal  3----- */  (int a_tab, int a_col, char *a_label);
char        COL_defwidth         (int a_tab, int a_size);
char        COL_width            /* petal  2----- */  (int a_tab, int a_col);
char        COL_widen            /* stigma 3----- */  (int a_tab, int a_col, int a_size);
char        COL_freeze           (int a_tab, int a_bcol, int a_ecol);
char        COL_unfreeze         (int a_tab);



/*===[[ gyges_row.c ]]========================================================*/
char        ROW_clear            /* septal 1----- */  (int a_tab);
char        ROW_valid            /* petal  2----- */  (int a_tab, int a_row);
int         ROW_defmax           /* petal  0----- */  (void);
int         ROW_max              /* petal  1----- */  (int a_tab);
int         ROW_used             /* petal  2----- */  (int a_tab, int a_row);
int         ROW_ypos             /* petal  2----- */  (int a_tab, int a_row);
char        ROW_yset             /* sigma  3----- */  (int a_tab, int a_row, int a_pos);
char        ROW_label            /* petal  3----- */  (int a_tab, int a_row, char *a_label);
char        ROW_defheight        (int a_tab, int a_size);
char        ROW_height           /* petal  2----- */  (int a_tab, int a_row);
char        ROW_heighten         /* sigma  3----- */  (int a_tab, int a_row, int a_size);
char        ROW_freeze           (int a_tab, int a_brow, int a_erow);
char        ROW_unfreeze         (int a_tab);



char        MAP_mapper           (char a_req);

/*---(unit testing)--------------*/
char*       LOC__unit            /* petal  2----- */  (char *a_question, char *a_label);
char*       TAB__unit            /* petal  2----- */  (char *a_question, int a_num);
char*       LOC__unit_OLD        /* petal  2----- */  (char *a_question, tCELL *a_cell);




char      CELL__wipe         /* ------ */  (tCELL *a_cell);




#define   CELL_FIXED         'n'
#define   CELL_GROW          'y'
#define   CELL_EXACT         'e'



/*---(basics)----------------------------*/
char        CELL__valid          /* ------ */  (tCELL  *a_cell, char a_linked);
/*345678901-12345678901234567890->--------------------------------------------*/
/*---(memory)----------------------------*/
char        CELL__new            /* ------ */  (tCELL **a_cell, char a_linked);
char        CELL__free           /* ------ */  (tCELL **a_cell, char a_linked);
char        CELL__create         /* ------ */  (tCELL **a_cell, int  a_tab, int  a_col, int  a_row);
char        CELL__delete       (char a_mode, int a_tab, int a_col, int a_row);
char        CELL_dup             /* ------ */  (tCELL **a_cell, tCELL* a_old);

#define   CHG_INPUT          'i'
#define   CHG_INPUTAND       'A'
#define   CHG_OVER           '#'
#define   CHG_OVERAND        '&'
#define   CHG_NOHIST         'x'


char      CELL_change          (tCELL **a_cell, char a_mode, int  a_tab, int  a_col, int  a_row, char *a_source);
tCELL    *CELL_overwrite       (char a_mode, int a_tab, int a_col, int a_row, char *a_source, char *a_format);

/*> char      CELL__depwipe        (FILE *a_file, char a_type, int *a_seq, int a_level, tCELL *a_curr, long a_stamp);   <*/
char      CELL__purge          (void);

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


char      CELL_erase           (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num);
char      CELL_align           (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num);
char      CELL_decimals        (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num);
char      CELL_width           (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num);
char      CELL_format          (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num);
char      CELL_merge_visu      (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num);
char      CELL_unmerge_visu    (tCELL *a_head, tCELL *a_curr, char a_mode, char a_num);
char      CELL_visual          (char   a_what, char a_mode, char a_how);

char      CELL_align_valid     (char   a_align);
char      CELL_decimals_valid  (char   a_decs);
char      CELL_format_valid    (char   a_format);
#define   CHANGE_WIDTH    'w'
#define   CHANGE_FORMAT   'f'
#define   CHANGE_ERASE    'e'
#define   CHANGE_ALIGN    'a'
#define   CHANGE_DECIMAL  'd'
#define   CHANGE_MERGE    'm'
#define   CHANGE_UNMERGE  'u'



char     *CELL__unit           (char  *a_question, tCELL *a_cell);
char     *CELL__unitnew        (char  *a_question, char *a_label);

char      SYLK_read            (char  *a_name);
char      SYLK_write           (char  *a_name, char a_variation);
char      FILE_controlled      (char  *a_yes);
char      FILE_control         (void);
char      FILE_nocontrol       (void);
char      FILE_version         (char  *a_ver);
char      FILE_vertxt          (char  *a_txt);
char      FILE_bump            (char  *a_type);
char      FILE_bump_major      (void);
char      FILE_bump_minor      (void);
char      FILE_bump_inc        (void);
char      FILE_rename          (char  *a_name);


/*---(file)------------------*/
char        TABS_writer             (char  a_tab);
char        DETAIL_writer           (char a_type, char a_tab, int a_cur);
char        TABS_writer_all         (void);
char        COLS_writer_all         (void);
char        ROWS_writer_all         (void);
char        OUTP_cell               (char a_type, int a_seq, int a_level, tCELL *a_curr);
char        OUTP_seq_cell           (void *a_owner, void *a_deproot, int a_seq, int a_level);
char        OUTP_cell_dep           (void);
char        OUTP_cell_free          (void);
char        TABS_reader             (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i);
char        COLS_reader             (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i);
char        ROWS_reader             (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i);
char        INPT_cell               (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i);






char      INPT_open            (void);
char      INPT_read            (void);
char      INPT_parse           (cchar *a_recd);
char      INPT_close           (void);
/*> char      INPT_main            (void);                                            <*/

char      OUTP_header          (FILE *a_file);

char      FILE_write           (void);
/*> char      FILE_writeas         (char *a_name);                                    <*/
char      XML3_read            (char *a_name);



/*===[[ gyges_calc.c ]]=======================================================*/

char        api_ycalc_enabler       (void *a_owner, void *a_deproot);
char        api_ycalc_pointer       (void *a_owner, char **a_source, char **a_type, double **a_value, char **a_string);
char        api_ycalc_reaper        (void **a_owner);

char        api_ycalc_named         (char *a_label, char a_force, void **a_owner, void **a_deproot);
char        api_ycalc_whos_at       (int x, int y, int z, char a_force, void **a_owner, void **a_deproot);
char*       api_ycalc_labeler       (void *a_owner);

char        api_ycalc_valuer        (void *a_owner, char *a_type, double *a_value, char **a_string);
char        api_ycalc_address       (void *a_owner, int *x, int *y, int *z);
char        api_ycalc_special       (void *a_owner, char a_what, double *a_value, char **a_string);

char        api_ycalc_printer       (void *a_owner);



/*===[[ gyges_yvikeys.c ]]====================================================*/

char        api_yvikeys_saver       (char *a_contents);

char        api_yvikeys_macro_get   (char   a_name, char  *a_keys);
char        api_yvikeys_macro_set   (char   a_name, char  *a_keys);

char        api_yvikeys_searcher    (char *a_search);
char        api_yvikeys_unsearcher  (int a_x, int a_y, int a_z);

char        api_yvikeys_clearer     (char a_1st, int x, int y, int z);
char        api_yvikeys_copier      (char a_type, long a_stamp);
char        api_yvikeys_paster      (char a_reqs, char a_pros, char a_intg, char a_1st, int a_xoff, int a_yoff, int a_zoff, tCELL *a_cell);
char        api_yvikeys_regkiller   (tCELL *a_curr);



/*===[ SCRP   ]===============================================================*/
/*345678901-12345678901234567890->--------------------------------------------*/


#endif

/*===============================[[ end code ]]===============================*/
/* htag :  714,  326 docs (36%),  386 code (72%),  326 othr (21%),  304 slocl */
