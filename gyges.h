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

/*===[[ DE-FACTO STANDARD LIBRARIES ]]========================================*/
#include    <ncurses.h>      /* CURSES : mvprintw, refresh, getch, ...        */

/*===[[ CUSTOM LIBRARIES ]]===================================================*/
#include    <yLOG.h>         /* CUSTOM : heatherly program logging            */
#include    <yRPN.h>         /* CUSTOM : heatherly infix to RPN conversion    */
#include    <ySTR.h>         /* CUSTOM : heatherly string handling            */
#include    <yVAR.h>         /* CUSTOM : heatherly variable testing           */



#define     PR        static
#define     PRIV      static

/* rapidly evolving version number to aid with visual change confirmation     */
#define     VER_NUM   "1.7g"
#define     VER_TXT   "change display screens to use x_title var"




/*===[[ TYPEDEFS : COMBINATIONS ]]============================================*/
/*---(basics)--------------------------*/
typedef     unsigned char         uchar;
typedef     const    char         cchar;
typedef     unsigned short        ushort;
typedef     const    int          cint;
typedef     unsigned long long    ullong;
/*---(library simplifications)---------*/
typedef     struct   tm           tTIME;
/*---(data structures)-----------------*/
typedef     struct   cDEBUG       tDEBUG;
typedef     struct   cCELL        tCELL;
typedef     struct   cCOLS        tCOLS;
typedef     struct   cROWS        tROWS;
typedef     struct   cTAB         tTAB;
typedef     struct   cDEP         tDEP;          /* inter-cell dependency     */
typedef     struct   cCALC        tCALC;         /* cell calculation entry    */
typedef     struct   cREG         tREG;          /* cut and paste registers   */
typedef     struct   cHIST        tHIST;         /* undo-redo history         */



/*===[[ CONSTANTS : LIMITS ]]=================================================*/
/*---(tabs)---------------------------*/
#define     MAX_TABS    10
#define     DEF_TABS    1
#define     MIN_TABS    1
/*---(columns)------------------------*/
#define     MAX_COLS    130
#define     DEF_COLS    26
#define     MIN_COLS    1
/*---(rows)---------------------------*/
#define     MAX_ROWS    9999 
#define     DEF_ROWS    100
#define     MIN_ROWS    1
/*---(cell width)---------------------*/
#define     MAX_WIDTH   100
#define     DEF_WIDTH   8
#define     MIN_WIDTH   4
/*---(cell height)--------------------*/
#define     MAX_HEIGHT  4
#define     DEF_HEIGHT  1
#define     MIN_HEIGHT  1
/*---(string length)------------------*/
#define     MAX_STR     2000
#define     LEN_RECD    2000
#define     LEN_STR     200
/*---(registers)----------------------*/


char        reqs        [MAX_STR];
char        deps        [MAX_STR];
char        like        [MAX_STR];
char        rpn         [MAX_STR];
char        keys        [MAX_STR];
char        buf0        [MAX_STR];
char        bufc        [MAX_STR];

char        keylog      [10000];
int         nkeylog;



/*===[[ DEBUGGING SETUP ]]====================================================*/
/* this is my latest standard format, vars, and urgents                       */
/* v3.0b : added signal handling                                (2014-feb-01) */
struct cDEBUG
{
   /*---(handle)-------------------------*/
   int         logger;                 /* log file so that we don't close it  */
   /*---(overall)------------------------*/  /* abcdefghi_kl__opq_stu__x__    */
   /* f = full urgents turns on all standard urgents                          */
   /* k = kitchen sink and turns everything, i mean everything on             */
   /* q = quiet turns all urgents off including the log itself                */
   char        tops;                   /* t) broad structure and context      */
   char        summ;                   /* s) statistics and analytical output */
   /*---(startup/shutdown)---------------*/
   char        args;                   /* a) command line args and urgents    */
   char        conf;                   /* c) configuration handling           */
   char        prog;                   /* p) program setup and teardown       */
   /*---(file processing)----------------*/
   char        inpt;                   /* i) text and data file input         */
   char        outp;                   /* o) text and data file output        */
   /*---(event handling)-----------------*/
   char        loop;                   /* l) main program event loop          */
   char        user;                   /* u) user input and handling          */
   char        apis;                   /* i) interprocess communication       */
   char        sign;                   /* x) os signal handling               */
   char        scrp;                   /* b) scripts and batch operations     */
   char        hist;                   /* h) history, undo, redo              */
   /*---(program)------------------------*/
   char        graf;                   /* g) grahpics, drawing, and display   */
   char        data;                   /* d) complex data structure handling  */
   char        envi;                   /* e) environment processing           */
   /*---(specific)-----------------------*/
   char        s;
   char        sylk;
   char        rpn;
   char        calc;
   char        exec;
   char        cell;
   char        deps;
   char        gnome;
   char        dtree;          /* displays the dependency tree and exits      */
   char        loc;
   char        visu;                   /* gyges_visu : visual selection       */
   char        ssel;                   /* gyges_visu : source selection       */
   char        mark;                   /* gyges_visu : location/object marks  */
   char        regs;                   /* s_sel  : copy and paste registers   */
   char        ystr;                   /* s_file : ystr functions             */
};
tDEBUG      debug;

#define     DEBUG_TOPS          if (debug.tops      == 'y')
#define     DEBUG_SUMM          if (debug.summ      == 'y')
#define     DEBUG_ARGS          if (debug.args      == 'y')
#define     DEBUG_CONF          if (debug.conf      == 'y')
#define     DEBUG_PROG          if (debug.prog      == 'y')
#define     DEBUG_INPT          if (debug.inpt      == 'y')
#define     DEBUG_OUTP          if (debug.outp      == 'y')
#define     DEBUG_LOOP          if (debug.loop      == 'y')
#define     DEBUG_USER          if (debug.user      == 'y')
#define     DEBUG_APIS          if (debug.apis      == 'y')
#define     DEBUG_SIGN          if (debug.sign      == 'y')
#define     DEBUG_SCRP          if (debug.scrp      == 'y')
#define     DEBUG_HIST          if (debug.hist      == 'y')
#define     DEBUG_GRAF          if (debug.graf      == 'y')
#define     DEBUG_DATA          if (debug.data      == 'y')
#define     DEBUG_ENVI          if (debug.envi      == 'y')
#define     DEBUG_S             if (debug.s         == 'y')
#define     DEBUG_RPN           if (debug.rpn       == 'y')
#define     DEBUG_CALC          if (debug.calc      == 'y')
#define     DEBUG_EXEC          if (debug.exec      == 'y')
#define     DEBUG_CELL          if (debug.cell      == 'y')
#define     DEBUG_DEPS          if (debug.deps      == 'y')
#define     DEBUG_GNOME         if (debug.gnome     == 'y')
#define     DEBUG_LOC           if (debug.loc       == 'y')
/*---(gyges_visu.c)----------------------*/
#define     DEBUG_VISU          if (debug.visu      == 'y')
#define     DEBUG_SSEL          if (debug.ssel      == 'y')
#define     DEBUG_MARK          if (debug.mark      == 'y')
/*---(gyges_reg.c)-----------------------*/
#define     DEBUG_REGS          if (debug.regs      == 'y')
/*---(ySTR)------------------------------*/
#define     DEBUG_YSTR          if (debug.ystr      == 'y')




struct cACCESSOR {
   /*---(files)----------------*/
   char        quiet;          /* bool : 0=normal, 1=quiet                    */
   int         logger;         /* log file so that we don't close it          */
   char        autocalc;       /* 0=manual, 1=auto                            */
   /*---(mode)------------*/
   char        scrn;                        /* screen display mode            */
   char        message     [MAX_STR];       /* message line                   */
   /*---(file hanndling)--*/
   char        f_title     [MAX_STR];       /* specific file base name        */
   char        f_suffix    [MAX_STR];       /* file suffix for spreadsheet    */
   char        f_name      [MAX_STR];       /* full file name                 */
   FILE       *f_file;                      /* file pointer                   */
   int         f_lines;                     /* file line number               */
   char        f_recd      [MAX_STR];       /* current file record            */
   char        f_type      [MAX_STR];       /* current record verb            */
   char        f_vers      [MAX_STR];       /* current record version         */
   /*---(tab vars)--------*/
   int         ntab;           /* number of worksheet tabs                         */
   int         ctab;           /* current tab                                      */
   int         tab1;           /* tab of source                                    */
   int         tab2;           /* tab of destination                               */
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
   char        mark_head;      /* first sequential mark                         */
   char        mark_save;      /* last mark requested or used                   */
   char        mark_tail;      /* last sequential mark                          */
   char        mark_list   [MAX_STR];       /* current marks                  */
   char        mark_plus   [MAX_STR];       /* current marks with mark id     */
   /*---(registers)-------*/
   char        reg_curr;
   /*---(cursus)----------*/
   char        info_win;
};
extern    struct cACCESSOR my;




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
   char        nrpn;         /* number of calculation tokens                  */
   char       *rpn;          /* rpn version of formula                        */
   tCALC      *calc;         /* pointer to head of calculation line           */
   /*---(#6, DEPENDENCIES)---------------*/
   /*   cells are related to each other as lists of requirements for this     */
   /*   calculation and a list of cells that are dependent on this one.       */
   /*   both lists are kept to enable full dependency calculation.            */
   char        nrequire;     /* number of required cells                      */
   tDEP       *requires;     /* incomming predesesors to this calc            */
   char        nprovide;     /* number of dependent cells                     */
   tDEP       *provides;     /* outgoing successors to this calc              */
   long        u;            /* timestamp of last update run                  */
   /*---(#7, CELL LIST)------------------*/
   /*   all the cells are stored in a doublly linked list in order to make    */
   /*   them easy to manage and verify as a entire population.  the linked    */
   /*   field helps drive hooking and unhooking cells from sheets.            */
   char        linked;       /* 1=linked, 0=unlinked                          */
   tCELL      *next;         /* next cell in doubly linked list               */
   tCELL      *prev;         /* previous cell in doubly linked list           */
   /*---(#8, ERRORS)---------------------*/
   char        nerror;       /* number of current errors                      */
   tERROR     *errors;       /* error entries                                 */
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
#define     LINKED       1
#define     UNLINKED     0

#define     UNHOOKED    -1

#define     CELL_BUFFER  1
#define     CELL_SHEET   0


/*---(cell types)------------------------*/
/*---(multiples)----------*/
extern char G_CELL_ALL    [20];     /* list of all types                      */
extern char G_CELL_RPN    [20];     /* types that require RPN conversion      */
extern char G_CELL_CALC   [20];     /* calculated result types                */
extern char G_CELL_DEPS   [20];     /* must follow the dependencies           */
extern char G_CELL_NUM    [20];     /* numeric result types                   */
extern char G_CELL_STR    [20];     /* string result types                    */
extern char G_CELL_ERR    [20];     /* error cell types                       */
extern char G_CELL_FPRE   [20];     /* formula prefixes                       */
/*---(singles)------------*/
#define     CTYPE_BLANK  '-'        /* blank                                  */
#define     CTYPE_STR    's'        /* string literal                         */
#define     CTYPE_NUM    'n'        /* numeric literal                        */
#define     CTYPE_FORM   'f'        /* numeric formula                        */
#define     CTYPE_MOD    'm'        /* modified string (calculated)           */
#define     CTYPE_FLIKE  'l'        /* like another cell's formula            */
#define     CTYPE_MLIKE  'L'        /* like another cell's modified string    */
#define     CTYPE_RANGE  'p'        /* range pointer                          */
#define     CTYPE_ADDR   'a'        /* address pointer                        */
#define     CTYPE_SOURCE '>'        /* source of merged cells content         */
#define     CTYPE_MERGE  '+'        /* merged cell (type of blank)            */
#define     CTYPE_WARN   'w'        /* warning status                         */
#define     CTYPE_ERROR  'E'        /* error status                           */






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
   /* flipping the active flag and updating the name as desired.              */
   char        active;                      /* currently in use?  0=no, 1=yes */
   char        name  [MAX_STR ];            /* tab name for user reference    */
   /*---(contents)-----------------------*/
   /* tabs pull three other data structures together in a package: column     */
   /* characteristics, row characteristics, and a grid on which to hang cells.*/
   tCOLS       cols  [MAX_COLS];            /* column characteristics         */
   tROWS       rows  [MAX_ROWS];            /* row characteristics            */
   tCELL      *sheet [MAX_COLS][MAX_ROWS];  /* cell pointers                  */
   int         c;                           /* count of entries in sheet      */
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
tTAB     tabs [MAX_TABS];
tTAB    *tab;                          /* current tab                         */



#define     NTAB        my.ntab
#define     CTAB        my.ctab

#define     NROW        tab->nrow
#define     CROW        tab->crow
#define     BROW        tab->brow
#define     EROW        tab->erow

#define     NCOL        tab->ncol
#define     CCOL        tab->ccol
#define     BCOL        tab->bcol
#define     ECOL        tab->ecol

#define     NCEL        ncell
#define     ACEL        acell
#define     CCEL        tab[my.ctab].sheet[tab->ccol][tab->crow]



#define     MAX_HIST    100000
struct cHIST {
   char        act         [15];
   int         btab;
   int         bcol;
   int         brow;
   char        before      [MAX_STR];
   char        after       [MAX_STR];
};
tHIST       hist        [MAX_HIST];
int         nhist;
int         chist;
char        hist_active;
#define     HISTORY     if (hist_active == 'y')



















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

extern    tDEP     *dhead;
extern    tDEP     *dtail;
extern    tCELL    *dtree;
extern    int       ndep;






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
/*---(mode stack)------------------------*/
/*---(major modes)-----------------------*/
#define     MODE_GOD       'G'
#define     MODE_MAP       'M'
#define     MODE_VISUAL    'V'
#define     MODE_SOURCE    'S'
#define     MODE_INPUT     'I'
#define     MODE_COMMAND   ':'
#define     MODE_SEARCH    '/'
/*---(sub-modes for source)--------------*/
#define     SMOD_REPLACE   'r'    /* replacing characters in source mode      */
#define     SMOD_SELECT    's'    /* visual selection of chars in source mode */
#define     SMOD_TEXTREG   't'    /* text register actions                    */
/*---(sub-modes for map)-----------------*/
#define     SMOD_ERROR     'e'    /* error reporting and actions              */
#define     SMOD_REGISTER  '"'    /* register actions                         */
#define     SMOD_BUFFER    ','    /* selecting buffers                        */
#define     SMOD_WANDER    '@'    /* formula creation by pointing             */
#define     SMOD_FORMAT    '$'    /* content formatting                       */
#define     SMOD_OBJECT    'o'    /* object formatting                        */
#define     SMOD_MARK      '\''   /* location and object marking              */


/*---(display modes)---------------------*/
#define     SCRN_TINY      't'    /* top line shows only source               */
#define     SCRN_SMALL     's'    /* top line shows very little but source    */
#define     SCRN_DEBUG     'n'    /* top line has lots of stuff to debug      */




extern int     save;

#define     FILE_BLANK  "untitled"
#define     FILE_SUFFIX "gyges"
#define     FILE_BUF    "/var/run/buffer.gyges"

char        f_maker     [MAX_STR];


/*---(strchr validation strings)-----------*/
extern      char     *g_alpha;
extern      char     *g_rcnum;
extern      char     *g_rcops;



extern int     done;
extern char    g_contents [MAX_STR];
extern char    saved      [MAX_STR];
extern char    cmd        [10];
extern char    msg_type;
extern char    message    [MAX_STR];
extern char    sta_type;
extern char    sta_error;
extern char    command    [MAX_STR];
extern char    special;

extern char      g_empty    [200];
extern char      g_dashes   [200];
extern char      g_equals   [200];
extern char      g_unders   [200];
extern char      g_dots     [200];
extern char      g_pluses   [200];

extern char      ver_ctrl;
extern char      ver_num  [10];
extern char      ver_txt  [100];

int     row_buffers;
int     row_formula;
int     row_chead;
int     row_main;
int     row_status;
int     row_message;
int     col_header;
int     col_far;


#define     K_RETURN      10
#define     K_ESCAPE      27

#define     K_CTRL_B       2
#define     K_CTRL_C       3
#define     K_CTRL_E       5
#define     K_CTRL_F       6
#define     K_CTRL_L      12
#define     K_CTRL_S      19

#define     LEN_TEXT  2000
extern      char          unit_answer [ LEN_TEXT ];



/*---(character constants)------------*/
#define   CHAR_PLACE      164   /* ¤  placeholder      (  -)   */
#define   CHAR_GROUP      166   /* ¦  group separator  ( 29)   */
#define   CHAR_FIELD      167   /* §  field separator  ( 31)   */
#define   CHAR_BS         171   /* «  backspace        (127)   */
#define   CHAR_ENTER      182   /* ¶  return/newline   ( 10)   */
#define   CHAR_TAB        187   /* »  tab              (  9)   */
#define   CHAR_MARK       215   /* ×  mark             (  -)   */
#define   CHAR_NULL       216   /* Ø  null             (  0)   */
#define   CHAR_ESC        234   /* ê  escape           ( 27)   */
#define   CHAR_SPACE      223   /* ß  space            ( 32)   */
#define   CHAR_LQUEST     191   /* ¿  lead question    (  -)   */
#define   CHAR_DEGREE     176   /* °  degree mark      (  -)   */
#define   CHAR_FUNKY      186   /* º  funky mark       (  -)   */
#define   CHAR_STAFF      165   /* ¥  staff            (  -)   */



/*===[[ PROTOTYPES ]]=====================================*/
/*---(from s_main.c)-----------------------*/
int       main               (int argc, char *argv[]);

/*---(from s.c)----------------------------*/
char      PROG_logger        (int argc, char *argv[]);
char      PROG_init          (void);
char      PROG_urgsmass      (char a_set, char a_extra);
char      PROG_urgs          (int argc, char *argv[]);
char      PROG_args          (int argc, char *argv[]);
char      PROG_begin         (void);
char      PROG_end           (void);

char     *unit_accessor      (char *a_question, void *a_thing);
char      PROG_testing       (void);
char      PROG_testloud      (void);



char      save_saved         (void);
char      clear_input        (void);
char      cmd_exec           (char*);

/*---(screen formatting)------------------------*/

char      row_print          (void);
char      EDIT_start         (char *a_prefix);
char      EDIT_prep          (void);
char      EDIT_done          (void);
char      EDIT_pos           (char);



/*===[ GYGES_VISU.C ]]========================================================*/
/*---(init)-----------------*/
char      VISU_init          (void);
char      VISU_clear         (void);
/*---(history)--------------*/
char      VISU_save          (void);
char      VISU_restore       (void);
/*---(setting)--------------*/
char      VISU_start         (int a_tab, int a_col, int a_row, char a_mode);
char      VISU_increase      (int a_tab, int a_col, int a_row);
char      VISU_update        (int a_tab, int a_col, int a_row);
/*---(display)--------------*/
char      VISU_root          (int a_tab, int a_col, int a_row);
char      VISU_selected      (int a_tab, int a_col, int a_row);
char      VISU_set           (int a_tab, int a_bcol, int a_brow, int a_ecol, int a_erow);
char      VISU_reverse       (void);
/*---(simplifiers)----------*/
char      VISU_islive        (void);
char      VISU_from          (void);
char      VISU_col           (void);
char      VISU_row           (void);
/*---(retrieval)------------*/
char      VISU_range         (int*, int*, int*, int*, int*);
tCELL*    VISU_first         (int*, int*, int*);
tCELL*    VISU_next          (int*, int*, int*);

char      SELC_clear         (void);
char      SELC_save          (void);
char      SELC_restore       (void);
char      SELC_start         (void);
char      SELC_increase      (void);
char      SELC_set           (int   a_bpos , int   a_epos);
char      SELC_mode          (char  a_major, char  a_minor);
char      SELC_islive        (void);
int       SELC_from          (void);
int       SELC_to            (void);

char      MARK_mode          (char  a_major, char a_minor);
char      MARK_init          (void);
char      MARK_unset         (char  a_mark);
char      MARK_prev          (void);
char      MARK_next          (void);
char      MARK_set           (char  a_mark);
char      MARK_label         (char  a_mark, char *a_label);
char      MARK_return        (char  a_mark);
char      MARK_list          (char *a_list);
char      MARK_listplus      (char *a_list);
char      MARK_write         (FILE *a_file, int *a_seq);
char      MARK_read          (char a_mark, char *a_label);


/*===[ REG  ]=================================================================*/
#define   REG_CURR           '!'

char      REG_init           (void);
char      REG_clear          (char a_reg, char a_init);
char      REG_purge          (char a_init);

char      REG__hook          (tCELL *a_curr, char a_buf, char a_note);
char      REG__unhook        (tCELL *a_curr);

char      REG_mode           (int a_prev , int a_curr);

/*---(text registers)-------*/
char      TREG_init          (void);
char      TREG_entry         (char  a_reg   , char *a_list);
char      TREG_clear         (void);
char      TREG_delete        (void);
char      TREG_copy          (void);
char      TREG_replace       (void);
char      TREG_paste         (char  a_dir);
char      TREG_mode          (int   a_major , int   a_minor);
char      TREG_read          (char  a_reg, char *a_label, int a_beg, int a_end, char *a_source);
char      TREG_write         (FILE *a_file, int  *a_seq, char a_buf);

char      REG_set            (char a_reg);
char      REG_list           (char a_buf, char *a_list);
char      REG_tail           (FILE *a_file, char a_type, int *a_seq, int a_level, tCELL *a_curr, long a_stamp);
char      REG_save           (char a_type);
char      REG_copy           (void);
char      REG_cut            (void);
char      REG_append         (void);
char      REG_paste          (char a_adapt);
char      REG_valuesin       (char a_trim);
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


/*---(mode handling)--------*/
char      MODE_init          (void);
char      MODE_enter         (char  a_mode);
char      MODE_return        (void);
char      MODE_curr          (void);
char      MODE_prev          (void);
char      MODE_not           (char  a_mode);
char      MODE_list          (char *a_list);
char      MODE_message       (void);
/*---(major modes)----------*/
char      MODE_god           (char  a_major, char  a_minor);
char      MODE_map           (char  a_major, char  a_minor);
char      MODE_source        (char  a_major, char  a_minor);
char      MODE_input         (char  a_major, char  a_minor);
char      MODE_command       (char  a_major, char  a_minor);
/*---(sub-modes)------------*/
char      SMOD_buffer        (char  a_major, char  a_minor);
char      SMOD_replace       (char  a_major, char  a_minor);
char      SMOD_format        (char  a_major, char  a_minor);
char      SMOD_wander        (char  a_major, char  a_minor);
char      SMOD_error         (char  a_major, char  a_minor);

char      KEYS_init          (void);
char      KEYS_record        (int   a_curr);
char      KEYS_status        (char *msg);
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
char      CURS_begin         (void);
char      CURS_end           (void);
int       CURS_main          (void);
char      CURS_colhead       (void);
char      CURS_rowhead       (void);
char      CURS_cell          (int  a_col, int a_row);
char      CURS_size          (void);
char      CURS_listmark      (void);
char      CURS_listreg       (void);
char      CURS_listtreg      (void);

char      CALC_init          (void);
char      CALC_cleanse       (tCELL *a_cell);
char      CALC_build         (tCELL *a_curr);
char      CALC_free          (tCELL *a_curr);
char      CALC_eval          (tCELL *a_curr);
char      calc_show          (void);
char*     CALC_strtok        (char *a_str);

tERROR*   ERROR_create       (tCELL  *a_owner);
char      ERROR_delete       (tERROR *a_error);
char      ERROR_add          (tCELL  *a_owner, char a_phase, int a_step, char *a_func, char a_type, char *a_desc);
char      ERROR_entry        (tCELL  *a_cell, char a_seq, char *a_list);
char      ERROR_list         (void);
char      ERROR_cleanse      (tCELL  *a_owner);


char      RPN_adjust         (tCELL *a_cell, int a_toff, int a_coff, int a_roff, char *a_source);
char      RPN_convert        (tCELL *a_curr);



tDEP     *DEP__new           /* ------ */  (void);
tDEP     *DEP__free          /* ------ */  (tDEP *a_dep);

char      DEP_init           /* ------ */  (void);
char      DEP_wrap           /* ------ */  (void);
char      DEP_purge          /* ------ */  (void);

#define   DEP_BLANK     '-'

#define   DEP_REQUIRE   'R'
#define   DEP_PROVIDE   'p'

#define   DEP_RANGE     'P'
#define   DEP_CELL      'c'

#define   DEP_FORMAT    'F'
#define   DEP_COPY      'f'

#define   DEP_SOURCE    'S'
#define   DEP_LIKE      'l'

#define   DEP_MERGED    'M'
#define   DEP_EMPTY     'e'

#define   DEP_CALCREF   'A'
#define   DEP_ADDRESS   'a'

char      DEP_create         /* ------ */  (char a_type, tCELL *a_source, tCELL *a_target);
char      DEP_delete         /* ------ */  (char a_type, tCELL *a_me, tCELL *a_other);
char      DEP_delcalcref     (tCELL *a_source);
tCELL    *DEP_delmerge       (tCELL *a_source);
char      DEP_delmergeroot   (tCELL *a_target);

char      DEP_range          /* ------ */  (tCELL *a_from, int a_btab, int a_bcol, int a_brow, int a_etab, int a_ecol, int a_erow);
char      DEP_cleanse        /* ------ */  (tCELL *a_curr);

char      DEP_requires       /* ------ */  (tCELL  *a_me, char* a_list);
char      DEP_provides       /* ------ */  (tCELL  *a_me, char* a_list);
char      DEP_like           /* ------ */  (tCELL  *a_me, char* a_list);
char      DEP_updatelikes    (tCELL  *a_me);

char      DEP_tail           (FILE *a_file, char a_type, int *a_seq, int a_level, tCELL *a_curr, long a_stamp, char (*a_func) (FILE *r_file, char r_type, int *r_seq, int r_level, tCELL *r_curr, long r_stamp));
char      DEP_recalc         /* ------ */  (void);
char      DEP_calc_up        /* ------ */  (tCELL*);
char      DEP_dump           /* ------ */  (void);
char      DEP_checkall       /* ------ */  (char a_print);
char      DEP_check          /* ------ */  (int a_level, tCELL *a_curr, char a_print, long a_stamp);

char      DEP_write          (FILE *a_file, int a_level, tCELL *a_curr, char a_type);
char      DEP_writeall       (void);
char     *DEP_unit           (char *a_question, char *a_label);



/*---(basics)----------------------------*/
tCELL    *CELL__new          /* ------ */  (char a_linked);
char      CELL__valid        /* ------ */  (tCELL *a_cell, char a_linked);
char      CELL__free         /* ------ */  (tCELL *a_cell, char a_linked);

char      CELL_dtree         (char *a_action);
char      CELL_regdel        (tCELL *a_curr);

char      HIST_change        (char *a_type, int a_tab, int a_col, int a_row, char *a_before, char *a_after);
char      HIST_format        (char *a_type, int a_tab, int a_col, int a_row, char  a_before, char  a_after);
char      HIST_size          (char *a_type, int a_tab, int a_col, int a_row, int   a_before, int   a_after);
char      HIST_list          (void);
char      HIST_undo          (void);
char      HIST_redo          (void);



char      LOC_hook           /* ------ */  (tCELL *a_cell, int a_tab, int a_col, int a_row);
char      LOC_unhook         /* ------ */  (tCELL *a_cell);
char      LOC_move           /* ------ */  (int  a_tab1, int  a_col1, int  a_row1, int  a_tab2, int  a_col2, int  a_row2);
tCELL    *LOC_cell           /* ------ */  (int  a_tab, int  a_col, int a_row);
tCELL    *LOC_at                           (char *a_label);

char      LOC_label          /* ------ */  (tCELL *a_curr, char *a_final);
char      LOC_ref            /* ------ */  (int a_tab, int a_col, int a_row, char a_abs, char *a_label);
char      LOC_coordinates                  (tCELL *a_cell, int *a_tab, int *a_col, int *a_row);
char      LOC_parse          /* ------ */  (char *a_label, int *a_tab, int *a_col, int *a_row, char  *a_abs);
int       LOC_tab            /* ------ */  (char  *a_name);

char*     TAB_unit           /* ------ */  (char *a_question, tCELL *a_cell);

char      CELL__wipe         /* ------ */  (tCELL *a_cell);
char      LOC_jump           /* ------ */  (int a_tab, int a_col, int a_row);
char      LOC_legal          /* ------ */  (int a_tab, int a_col, int a_row, char a_adapt);
char      CELL_merge         /* ------ */  (char);

#define   CELL_FIXED         'n'
#define   CELL_ADAPT         'y'

tCELL    *CELL__create       (int  a_tab, int  a_col, int  a_row);
char      CELL__delete       (char a_mode, int  a_tab, int  a_col, int  a_row);


#define   CHG_INPUT          'i'
#define   CHG_INPUTAND       'A'
#define   CHG_OVER           '#'
#define   CHG_OVERAND        '&'
#define   CHG_NOHIST         'x'

tCELL    *CELL_dup           (tCELL* a_old);
tCELL    *CELL_change        (char a_mode, int  a_tab, int  a_col, int  a_row, char *a_source);
tCELL    *CELL_overwrite     (char a_mode, int a_tab, int a_col, int a_row, char *a_source, char *a_format);

char      CELL__depwipe      (FILE *a_file, char a_type, int *a_seq, int a_level, tCELL *a_curr, long a_stamp);
char      CELL__purge        (void);


char      CELL_init          (void);
char      CELL_wrap          (void);
char      CELL__interpret    (tCELL *a_curr);

char      CELL_erase         (void);
char      CELL_align         (char a_mode, char a_align);
char      CELL_decimals      (char a_mode, char a_num);
char      CELL_format        (char a_mode, char a_num);
char      CELL_width         (char a_mode, char a_num);
char      CELL_height        (char a_mode, char a_num);
char      CELL__print_comma  (char a_format, char a_decimal, double a_num, char *a_text);
char      CELL__print_number (char a_format, char a_decimal, double a_num, char *a_text);
char      CELL__print_special(char a_format, char a_decimal, double a_num, char *a_text);
char      CELL__print_times  (char a_format, double a_num, char *a_text);
char      CELL_printable     (tCELL *a_cell);

char     *CELL__unit         (char *a_question, tCELL *a_cell);
char     *CELL__unitnew      (char *a_question, char *a_label);

char      SYLK_read          (char *a_name);
char      SYLK_write         (char *a_name, char a_variation);
char      FILE_version       (char *a_ver, char *a_final);
char      FILE_bump          (char a_type, char *a_ver);

char      INPT_cell          (cchar *a_recd);
char      INPT_tab           (cchar *a_recd);
char      INPT_main          (char *a_name);

char      FILE_dep           (FILE *a_file, char a_type, int *a_seq, int a_level, tCELL *a_curr, long a_stamp);
char      FILE_write         (char *a_name);
char      XML3_read          (char *a_name);



#endif

/*===============================[[ end code ]]===============================*/
/* htag :  714,  326 docs (36%),  386 code (72%),  326 othr (21%),  304 slocl */
