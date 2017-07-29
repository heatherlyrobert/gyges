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

/*===[[ DE-FACTO STANDARD LIBRARIES ]]========================================*/
#include    <ncurses.h>      /* CURSES : mvprintw, refresh, getch, ...        */

/*===[[ CUSTOM LIBRARIES ]]===================================================*/
#include    <yURG.h>         /* CUSTOM : heatherly urgent processing          */
#include    <yLOG.h>         /* CUSTOM : heatherly program logging            */
#include    <yRPN.h>         /* CUSTOM : heatherly infix to RPN conversion    */
#include    <yVIKEYS.h>      /* CUSTOM : heatherly vi_keys standard           */
#include    <ySTR.h>         /* CUSTOM : heatherly string handling            */
#include    <yVAR.h>         /* CUSTOM : heatherly variable testing           */



#define     PR        static
#define     PRIV      static

/* rapidly evolving version number to aid with visual change confirmation     */
#define     VER_NUM   "2.3e"
#define     VER_TXT   "cleaned up key handling in formatting sub-mode"




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
/*> typedef     struct   cDEBUG       tDEBUG;                                         <*/
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
#define     MAX_MERGE   10
/*---(cell height)--------------------*/
#define     MAX_HEIGHT  4
#define     DEF_HEIGHT  1
#define     MIN_HEIGHT  1
/*---(string length)------------------*/
#define     LEN_RECD    2000
#define     LEN_UNIT    500
#define     LEN_STR     500
#define     LEN_DESC    100
#define     LEN_ABBR    10
/*---(registers)----------------------*/


char        reqs        [LEN_RECD];
char        deps        [LEN_RECD];
char        like        [LEN_RECD];
char        rpn         [LEN_RECD];
char        keys        [LEN_RECD];
char        buf0        [LEN_RECD];
char        bufc        [LEN_RECD];

char        keylog      [10000];
int         nkeylog;






struct cACCESSOR {
   /*---(files)----------------*/
   char        quiet;          /* bool : 0=normal, 1=quiet                    */
   int         logger;         /* log file so that we don't close it          */
   /*---(layout)----------*/
   char        layout_formula;              /* formula line display mode      */
   char        layout_status;               /* status line display mode       */
   char        layout_command;              /* command line display mode      */
   char        message     [LEN_RECD];       /* message line                   */
   /*---(file hanndling)--*/
   char        f_title     [LEN_RECD];       /* specific file base name        */
   char        f_suffix    [LEN_RECD];       /* file suffix for spreadsheet    */
   char        f_name      [LEN_RECD];       /* full file name                 */
   FILE       *f_file;                      /* file pointer                   */
   int         f_lines;                     /* file line number               */
   char        f_recd      [LEN_RECD];       /* current file record            */
   char        f_type      [LEN_RECD];       /* current record verb            */
   char        f_vers;                       /* current record version         */
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
   char        mark_head;      /* first sequential mark                         */
   char        mark_save;      /* last mark requested or used                   */
   char        mark_tail;      /* last sequential mark                          */
   char        mark_list   [LEN_RECD];       /* current marks                  */
   char        mark_plus   [LEN_RECD];       /* current marks with mark id     */
   /*---(registers)-------*/
   char        reg_curr;
   /*---(cursus)----------*/
   char        info_win;
   char        menu;
};
extern    struct cACCESSOR my;


#define     G_INFO_NONE       ' '
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
   char        nrpn;         /* number of calculation tokens                  */
   char       *rpn;          /* rpn version of formula                        */
   tCALC      *calc;         /* pointer to head of calculation line           */
   char        clevel;       /* calculation level                             */
   tCELL      *cprev;        /* pointer to calculation execution prev         */
   tCELL      *cnext;        /* pointer to calculation execution next         */
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
#define     LINKED       'y'
#define     UNLINKED     '-'

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






#define     G_TAB_NOT   '/'
#define     G_TAB_FROZE '_'
#define     G_TAB_LIVE  'y'

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
#define     G_STATUS_VISUAL     'v'
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
/*---(layout_command)--------------------*/
#define     G_COMMAND_SHOW      's'
#define     G_COMMAND_HIDE      'h'
#define     G_COMMAND_FLOAT     'f'



#define     MENU_NONE      ' '
#define     MENU_ROOT      '-'



extern int     save;

#define     FILE_BLANK  "untitled"
#define     FILE_SUFFIX "gyges"
#define     FILE_BUF    "/var/run/buffer.gyges"

char        f_maker     [LEN_RECD];


/*---(strchr validation strings)-----------*/
extern      char     *g_alpha;
extern      char     *g_rcnum;
extern      char     *g_rcops;



extern int     done;
extern char    g_contents [LEN_RECD];
extern char    saved      [LEN_RECD];
extern char    cmd        [10];
extern char    msg_type;
extern char    message    [LEN_RECD];
extern char    sta_error;
extern char    g_command    [LEN_RECD];
extern char    special;

extern char      g_empty    [200];
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


#define     K_RETURN      10
#define     K_ESCAPE      27

#define     K_CTRL_B       2
#define     K_CTRL_C       3
#define     K_CTRL_E       5
#define     K_CTRL_F       6
#define     K_CTRL_L      12
#define     K_CTRL_S      19

extern      char          unit_answer [LEN_UNIT];



/*---(character constants)------------*/
#define   G_CHAR_PLACE      164   /* ¤  placeholder      (  -)   */
#define   G_CHAR_GROUP      166   /* ¦  group separator  ( 29)   */
#define   G_CHAR_FIELD      167   /* §  field separator  ( 31)   */
#define   G_CHAR_BS         171   /* «  backspace        (127)   */
#define   G_CHAR_ENTER      182   /* ¶  return/newline   ( 10)   */
#define   G_CHAR_TAB        187   /* »  tab              (  9)   */
#define   G_CHAR_MARK       215   /* ×  mark             (  -)   */
#define   G_CHAR_NULL       216   /* Ø  null             (  0)   */
#define   G_CHAR_ESC        234   /* ê  escape           ( 27)   */
#define   G_CHAR_SPACE      223   /* ß  space            ( 32)   */
#define   G_CHAR_LQUEST     191   /* ¿  lead question    (  -)   */
#define   G_CHAR_DEGREE     176   /* °  degree mark      (  -)   */
#define   G_CHAR_FUNKY      186   /* º  funky mark       (  -)   */
#define   G_CHAR_STAFF      165   /* ¥  staff            (  -)   */



/*===[[ PROTOTYPES ]]=====================================*/
/*---(from s_main.c)-----------------------*/
int       main               (int argc, char *argv[]);

/*---(from s.c)----------------------------*/
char      PROG_init            (int argc, char *argv[]);
char      PROG_urgsmass        (char a_set, char a_extra);
char      PROG_urgs            (int argc, char *argv[]);
char      PROG_args            (int argc, char *argv[]);
char      PROG_begin           (void);
char      PROG_end             (void);
char      PROG_layout_init     (void);
char      PROG_layout_set      (char *a_who, char *a_cat, char *a_opt);
char      PROG_layout_list     (char *a_who);
char      PROG_layout_entry    (int a_num, char *a_line);

char     *unit_accessor      (char *a_question, void *a_thing);
char      PROG_testing       (void);
char      PROG_testloud      (void);
char      PROG_testquiet     (void);
char      PROG_testend       (void);



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
char      REG_deps           (tCELL *a_curr, long a_stamp);
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
char      SMOD_menus         (char  a_major, char  a_minor);

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
char      CURS_begin           (void);
char      CURS_end             (void);
int       CURS_main            (void);
char      CURS_col_head        (void);
char      CURS_row_head        (void);
char      CURS_cell            (int  a_col, int a_row);
char      CURS_size            (void);
char      CURS_info_cell       (void);
char      CURS_info_layout     (void);
char      CURS_listmark        (void);
char      CURS_listreg         (void);
char      CURS_listtreg        (void);

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


char      RPN_adjust           (tCELL *a_cell, int a_toff, int a_coff, int a_roff, char *a_source);
char      RPN_convert          (tCELL *a_curr);




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
char      SEQ_file_deps      (long a_stamp, FILE *a_file);
char      SEQ_reg_deps       (long a_stamp);



char      DEP_seqlist        (char *a_list);

char      DEP_write          (FILE *a_file, int a_level, tDEP *a_dep);
char      DEP_writeall       (void);
char      DEP_writescreen    (void);
char     *DEP_unit           (char *a_question, char *a_label);


char        CELL_regdel        (tCELL *a_curr);

/*345678901-12345678901234567890->--------------------------------------------*/
/*---(merge-specific)-----------------*/
char        CELL__merge_valid    (tCELL *a_curr);
tCELL*      CELL__merge_left     (tCELL *a_curr);
char        CELL__merge_right    (tCELL *a_left);
char        CELL__unmerge_right  (tCELL *a_left);
char        CELL_merge           (tCELL *a_curr);
char        CELL_unmerge         (tCELL *a_curr);
char        CELL_merge_visu      (void);
char        CELL_unmerge_visu    (void);
/*---(unit-testing)-------------------*/
char        CELL_unitchange      (tCELL *a_cell, char *a_source);



char      HIST_change        (char *a_type, int a_tab, int a_col, int a_row, char *a_before, char *a_after);
char      HIST_format        (char *a_type, int a_tab, int a_col, int a_row, char  a_before, char  a_after);
char      HIST_size          (char *a_type, int a_tab, int a_col, int a_row, int   a_before, int   a_after);
char      HIST_list          (void);
char      HIST_undo          (void);
char      HIST_redo          (void);


/*345678901-12345678901234567890->--------------------------------------------*/
/*---(program)-------------------*/
char        LOC_init             /* shoot  0----- */  (void);
char        LOC__clear_locs      /* septal 1----- */  (short a_tab);
char        LOC__purge           /* stem   0----- */  (void);
char        LOC_wrap             /* shoot  0----- */  (void);

char        LOC_legal            /* stigma 4----- */  (short a_tab, short a_col, short a_row, char a_adapt);
char        LOC_jump             /* stigma 3----- */  (short a_tab, short a_col, short a_row);

char        LOC_hook             /* stigma 4----- */  (tCELL *a_cell, short a_tab , short a_col , short a_row);
char        LOC_unhook           /* stigma 1----- */  (tCELL *a_cell);
char        LOC_move             /* stigma 6----- */  (short  a_tab1, short a_col1, short a_row1, short a_tab2, short a_col2, short a_row2);

tCELL      *LOC_cell_at_curr     /* petal  0----- */  (void);
tCELL      *LOC_cell_at_loc      /* petal  3----- */  (short  a_tab, short  a_col, short a_row);
tCELL      *LOC_cell_labeled     /* petal  1----- */  (char  *a_label);

char        LOC_coords           /* petal  4----- */  (tCELL *a_cell, short *a_tab, short *a_col, short *a_row);
char        LOC_parse            /* petal  4----- */  (char *a_label, short *a_tab, short *a_col, short *a_row, char  *a_abs);

char        LOC_label            /* petal  1----- */  (tCELL *a_curr, char *a_final);
char        LOC_ref              /* petal  5----- */  (short a_tab, short a_col, short a_row, char a_abs, char *a_label);
/*---(tabs)----------------------*/
char        LOC_tab_valid        /* petal  1----- */  (short a_tab);
char        LOC_tab_defname      /* petal  2----- */  (short a_tab, char *a_name);
char        LOC_tab_name         /* petal  2----- */  (short a_tab, char *a_name);
char        LOC_tab_rename       /* stigma 2----- */  (short a_tab, char *a_name);
char        LOC_tab_size         /* petal  2----- */  (short a_tab, char *a_max);
char        LOC_tab_resize       /* stigma 1----- */  (char *a_max);
/*---(columns)-------------------*/
char        LOC_col_clear        /* septal 1----- */  (short a_tab);
char        LOC_col_valid        /* petal  2----- */  (short a_tab, short a_col);
short       LOC_col_defmax       /* petal  0----- */  (void);
short       LOC_col_max          /* petal  1----- */  (short a_tab);
short       LOC_col_used         /* petal  2----- */  (short a_tab, short a_col);
short       LOC_col_xpos         /* petal  2----- */  (short a_tab, short a_col);
char        LOC_col_xset         /* stigma 3----- */  (short a_tab, short a_col, short a_pos);
char        LOC_col_label        /* petal  3----- */  (short a_tab, short a_col, char *a_label);
char        LOC_col_width        /* petal  2----- */  (short a_tab, short a_col);
char        LOC_col_widen        /* stigma 3----- */  (short a_tab, short a_col, short a_size);
char        LOC_col_freeze       (short a_tab, short a_bcol, short a_ecol);
char        LOC_col_unfreeze     (short a_tab);
/*---(rows)----------------------*/
char        LOC_row_clear        /* septal 1----- */  (short a_tab);
char        LOC_row_valid        /* petal  2----- */  (short a_tab, short a_row);
short       LOC_row_defmax       /* petal  0----- */  (void);
short       LOC_row_max          /* petal  1----- */  (short a_tab);
short       LOC_row_used         /* petal  2----- */  (short a_tab, short a_row);
short       LOC_row_ypos         /* petal  2----- */  (short a_tab, short a_row);
char        LOC_row_yset         /* sigma  3----- */  (short a_tab, short a_row, short a_pos);
char        LOC_row_label        /* petal  3----- */  (short a_tab, short a_row, char *a_label);
char        LOC_row_height       /* petal  2----- */  (short a_tab, short a_row);
char        LOC_row_heighten     /* sigma  3----- */  (short a_tab, short a_row, short a_size);
char        LOC_row_freeze       (short a_tab, short a_brow, short a_erow);
char        LOC_row_unfreeze     (short a_tab);
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
char        CELL_dup             /* ------ */  (tCELL **a_cell, tCELL* a_old);

char        CELL_delete          /* ------ */  (char a_mode, int  a_tab, int  a_col, int  a_row);


#define   CHG_INPUT          'i'
#define   CHG_INPUTAND       'A'
#define   CHG_OVER           '#'
#define   CHG_OVERAND        '&'
#define   CHG_NOHIST         'x'

char      CELL_change        (tCELL **a_cell, char a_mode, int  a_tab, int  a_col, int  a_row, char *a_source);
tCELL    *CELL_overwrite     (char a_mode, int a_tab, int a_col, int a_row, char *a_source, char *a_format);

char      CELL__depwipe      (FILE *a_file, char a_type, int *a_seq, int a_level, tCELL *a_curr, long a_stamp);
char      CELL__purge        (void);

char      CELL__numerics     (tCELL *a_cell);

char      CELL__rpn          (tCELL *a_cell);
char      CELL__build        (tCELL *a_cell);
char      CELL__like         (tCELL *a_cell);
char      CELL__point        (tCELL *a_cell);
char      CELL__eval         (tCELL *a_cell);
char      CELL__formulas     (tCELL *a_cell);

char      CELL__ftype        (char a_prefix);
char      CELL_init          (void);
char      CELL_wrap          (void);
char      CELL__interpret    (tCELL *a_curr);

char      CELL_erase           (void);
char      CELL_align_valid     (char a_align);
char      CELL_align           (char a_mode, char a_align);
char      CELL_decimals_valid  (char a_decs);
char      CELL_decimals        (char a_mode, char a_decs);
char      CELL_format_valid    (char a_format);
char      CELL_format          (char a_mode, char a_format);
char      CELL_width           (char a_mode, char a_num);
char      CELL_printable       (tCELL *a_cell);

char     *CELL__unit         (char *a_question, tCELL *a_cell);
char     *CELL__unitnew      (char *a_question, char *a_label);

char      SYLK_read          (char *a_name);
char      SYLK_write         (char *a_name, char a_variation);
char      FILE_version       (char *a_ver, char *a_final);
char      FILE_bump          (char a_type, char *a_ver);


/*---(tabs)------------------*/
char      INPT_tab             (char  *a_label, char *a_name);
char      OUTP_tab             (short  a_tab  );
char      OUTP_tab_head        (FILE  *a_file );
char      OUTP_tab_foot        (FILE  *a_file , int   a_count);
int       OUTP_tabs            (FILE  *a_file );
/*---(columns)---------------*/
char      INPT_col             (char  *a_label, int   a_size);
char      OUTP_col             (short  a_tab  , short a_col);
char      OUTP_col_head        (FILE  *a_file );
char      OUTP_col_foot        (FILE  *a_file , int   a_count);
int       OUTP_cols            (FILE  *a_file );
/*---(cells)-----------------*/
char      INPT_cell            (char *a_label, char *a_format, char *a_source);
char      OUTP_cell            (FILE *a_file, char *a_type, int a_seq, char *a_level, tCELL *a_curr);
char      OUTP_cell_dep        (FILE *a_file, int a_seq, int a_level, tCELL *a_curr);
char      OUTP_cell_free       (FILE *a_file, int *a_seq, long a_stamp, int a_tab, int a_bcol, int a_ecol, int a_brow, int a_erow);




char      INPT_open          (cchar *a_name);
char      INPT_read          (void);
char      INPT_parse         (cchar *a_recd);
char      INPT_close         (void);
char      INPT_main          (cchar *a_name);

char      OUTP_header        (FILE *a_file);

char      FILE_write         (char *a_name);
char      XML3_read          (char *a_name);



#endif

/*===============================[[ end code ]]===============================*/
/* htag :  714,  326 docs (36%),  386 code (72%),  326 othr (21%),  304 slocl */
